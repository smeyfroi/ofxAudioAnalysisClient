#include "BaseClient.hpp"
#include <algorithm>
#include "ofxOsc.h"
#include "OscPrintReceivedElements.h"

namespace ofxAudioAnalysisClient {

BaseClient::BaseClient(bool darkMode_) :
darkMode(darkMode_)
{
  plots.resize(4);
  plotValueIndexes.resize(4);
  changePlot(0, static_cast<int>(AnalysisScalar::peakEnergy));
  changePlot(1, static_cast<int>(AnalysisScalar::spectralCentroid));
  changePlot(2, static_cast<int>(AnalysisScalar::highFrequencyContent));
  changePlot(3, static_cast<int>(AnalysisScalar::pitch));
}

ofxHistoryPlot* BaseClient::makePlot(float* plottedValuePtr, std::string name, float low, float high) {
  float numSamples = ofGetWindowWidth() / 2;
  ofxHistoryPlot* plotPtr = new ofxHistoryPlot(plottedValuePtr, name, numSamples, true);
  if (low != 0.0 || high != 0.0) {
//    plotPtr->setLowerRange(low);
    plotPtr->setRange(low, high);
  }
  if (!darkMode) {
    plotPtr->setColor(ofColor::black);
    plotPtr->setBackgroundColor(ofColor(0, 0));
    plotPtr->setGridColor(ofColor(240, 128));
  } else {
    plotPtr->setColor(ofColor::white);
  }
  plotPtr->setShowNumericalInfo(true);
  plotPtr->setRespectBorders(true);
  plotPtr->setDrawFromRight(true);
  plotPtr->setCropToRect(true);
  plotPtr->update(0);
  return plotPtr;
}

void BaseClient::resetPlots() {
  for (const auto& plotPtr : plots) {
    plotPtr->reset();
  }
}

void BaseClient::changePlot(size_t plotIndex, size_t valueIndex) {
  plots[plotIndex] = std::unique_ptr<ofxHistoryPlot>(makePlot(&scalarValues[valueIndex], scalarNames[valueIndex], minScalarValues[valueIndex], maxScalarValues[valueIndex]));
  plotValueIndexes[plotIndex] = valueIndex;
}

void BaseClient::drawPlots(float width, float height) {
  if (!plotsVisible) return;
  for(int i = 0; i < 4; i++) {
    plots[i]->draw(0, i * height, width, height);
  }
}

bool BaseClient::keyPressed(int key, int plotIndex) {
  if (key == '<') {
    changePlot(plotIndex, (plotValueIndexes[plotIndex]+1) % static_cast<int>(AnalysisScalar::_count));
    resetPlots();
  } else if (key == '>') {
    changePlot(plotIndex, (plotValueIndexes[plotIndex]-1+static_cast<int>(AnalysisScalar::_count)) % static_cast<int>(AnalysisScalar::_count));
    resetPlots();
  } else if (key == 'p') {
    plotsVisible = !plotsVisible;
  } else {
    return false;
  }
  return true;
}

void BaseClient::updateOsc() {
  int packetSize = nextOscPacket();
  while (packetSize > 0) { // process any backlog
    const osc::ReceivedPacket packet(buf, packetSize);
    if (packetSize > MAX_PACKET_SIZE) {
      ofLogError() << "Buffer size " << packetSize << " > MAX_PACKET SIZE " << MAX_PACKET_SIZE;
      ofExit(-1);
    }
    //    ofLogVerbose() << packet << std::endl;
    osc::ReceivedBundle bundle(packet);
//    ofLogNotice() << bundle.TimeTag();
    osc::ReceivedBundle::const_iterator bundleIter = bundle.ElementsBegin();
    // /meta
    osc::ReceivedBundleElement element0 = *bundleIter++;
    osc::ReceivedMessage message0(element0);
    osc::ReceivedMessage::const_iterator messageIter0 = message0.ArgumentsBegin();
    int channelId = (*messageIter0++).AsInt32(); // TODO: use this
//    values[static_cast<int>(AnalysisValue::channelId)] = (*messageIter1++).AsInt();
    // /time
    osc::ReceivedBundleElement element1 = *bundleIter++;
    osc::ReceivedMessage message1(element1);
    osc::ReceivedMessage::const_iterator messageIter1 = message1.ArgumentsBegin();
    scalarValues[static_cast<int>(AnalysisScalar::rootMeanSquare)] = (*messageIter1++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::peakEnergy)] = (*messageIter1++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::zeroCrossingRate)] = (*messageIter1++).AsFloat();
    // /freq
    osc::ReceivedBundleElement element2 = *bundleIter++;
    osc::ReceivedMessage message2(element2);
    osc::ReceivedMessage::const_iterator messageIter2 = message2.ArgumentsBegin();
    scalarValues[static_cast<int>(AnalysisScalar::spectralCentroid)] = (*messageIter2++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::spectralCrest)] = (*messageIter2++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::spectralFlatness)] = (*messageIter2++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::spectralRollof)] = (*messageIter2++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::spectralKurtosis)] = (*messageIter2++).AsFloat();
    // /onset
    osc::ReceivedBundleElement element3 = *bundleIter++;
    osc::ReceivedMessage message3(element3);
    osc::ReceivedMessage::const_iterator messageIter3 = message3.ArgumentsBegin();
    scalarValues[static_cast<int>(AnalysisScalar::energyDifference)] = (*messageIter3++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::spectralDifference)] = (*messageIter3++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::spectralDifferenceHWR)] = (*messageIter3++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::complexSpectralDifference)] = (*messageIter3++).AsFloat();
    scalarValues[static_cast<int>(AnalysisScalar::highFrequencyContent)] = (*messageIter3++).AsFloat();
    // /pitch
    osc::ReceivedBundleElement element4 = *bundleIter++;
    osc::ReceivedMessage message4(element4);
    osc::ReceivedMessage::const_iterator messageIter4 = message4.ArgumentsBegin();
    float pitchEstimate = (*messageIter4++).AsFloat();
    if (pitchEstimate < 10000) {
      scalarValues[static_cast<int>(AnalysisScalar::pitch)] = pitchEstimate;
    }
    // /spectrum
//    osc::ReceivedBundleElement element5 = *bundleIter++;
//    osc::ReceivedMessage message5(element5);
//    if (spectrum.size() != message5.ArgumentCount()) {
//      if (spectrum.size() != 0) ofLogError() << "spectrum of size " << message5.ArgumentCount() << " != previous size size " << spectrum.size();
//      spectrum.resize(message5.ArgumentCount());
//    }
//    int i = 0;
//    for (auto iter = message5.ArgumentsBegin(); iter != message5.ArgumentsEnd(); iter++) {
//      spectrum[i++] = (*iter).AsFloat();
//    }
    // /mel
//    osc::ReceivedBundleElement element6 = *bundleIter++;
//    osc::ReceivedMessage message6(element6);
//    if (mel.size() != message6.ArgumentCount()) {
//      if (mel.size() != 0) ofLogError() << "mel of size " << message6.ArgumentCount() << " != previous size size " << mel.size();
//      mel.resize(message6.ArgumentCount());
//    }
//    i = 0;
//    for (auto iter = message6.ArgumentsBegin(); iter != message6.ArgumentsEnd(); iter++) {
//      mel[i++] = (*iter).AsFloat();
//    }
    // /mfcc
    osc::ReceivedBundleElement element7 = *bundleIter++;
    osc::ReceivedMessage message7(element7);
    if (mfcc.size() != message7.ArgumentCount()) {
      if (mfcc.size() != 0) ofLogError() << "mfcc of size " << message7.ArgumentCount() << " != previous size size " << mfcc.size();
      mfcc.resize(message7.ArgumentCount());
    }
    int i = 0;
    for (auto iter = message7.ArgumentsBegin(); iter != message7.ArgumentsEnd(); iter++) {
//      mfcc[i++] = (*iter).AsFloat();
    }

    packetSize = nextOscPacket();
  } // while clearing backlog
}



float BaseClient::frequencyToMidi(float freq) const {
    float midi;
    if (freq < 2. || freq > 100000.) return 0.; // avoid nans and infs
    /* log(freq/A-2)/log(2) */
    midi = freq / 6.875;
    midi = logf(midi) / 0.69314718055995;
    midi *= 12;
    midi -= 3;
    return std::max(0.0f, midi);
};

float BaseClient::getNoteFrequency() const {
//  if(_doDetect[GIST_PITCH] ){
      return frequencyToMidi(scalarValues[static_cast<int>(AnalysisScalar::pitch)]);
//  }
};

const string n[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const std::string BaseClient::getNoteName() const {
    int val = getNoteFrequency();
    int relVal = val % 12;
    int oct = floor(val/12);
    return n[relVal]+"-"+ofToString(oct);
};

const std::pair<float, float> BaseClient::getNote() const {
  int val = getNoteFrequency();
  int relVal = val % 12;
  int oct = floor(val/12);
  return std::pair<float, float> { relVal, oct };
}

} //namespace
