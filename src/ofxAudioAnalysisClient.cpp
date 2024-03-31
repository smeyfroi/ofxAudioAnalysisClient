#include "ofxAudioAnalysisClient.h"
#include "ofxOsc.h"
#include "OscPrintReceivedElements.h"

namespace ofxAudioAnalysisClient {

LiveClient::LiveClient(std::string host, short port) {
  if (!tcpClient.setup(host, port)) {
    ofLogError() << "Failed to open TCP connection to " << host << ":" << port << std::endl;
    ofExit(-1);
  } else {
    ofLogNotice() << "Connected to analyser " << host << ":" << port << std::endl;
  }

  plots.resize(4);
  plotValueIndexes.resize(4);
  changePlot(0, static_cast<int>(AnalysisScalar::peakEnergy));
  changePlot(1, static_cast<int>(AnalysisScalar::spectralCentroid));
  changePlot(2, static_cast<int>(AnalysisScalar::highFrequencyContent));
  changePlot(3, static_cast<int>(AnalysisScalar::pitch));
}

ofxHistoryPlot* makePlot(float* plottedValuePtr, std::string name, float low, float high) {
  float numSamples = ofGetWindowWidth() / 2;
  ofxHistoryPlot* plotPtr = new ofxHistoryPlot{plottedValuePtr, name, numSamples, true};
  if (low != 0.0 || high != 0.0) {
//    plotPtr->setLowerRange(low);
    plotPtr->setRange(low, high);
  }
  plotPtr->setColor(ofColor::white);
  plotPtr->setShowNumericalInfo(true);
  plotPtr->setRespectBorders(true);
  plotPtr->setDrawFromRight(true);
  plotPtr->setCropToRect(true);
  plotPtr->update(0);
  return plotPtr;
}

void LiveClient::resetPlots() {
  for (const auto& plotPtr : plots) {
    plotPtr->reset();
  }
}

void LiveClient::changePlot(size_t plotIndex, size_t valueIndex) {
  plots[plotIndex] = std::unique_ptr<ofxHistoryPlot>(makePlot(&scalarValues[valueIndex], scalarNames[valueIndex], minScalarValues[valueIndex], maxScalarValues[valueIndex]));
  plotValueIndexes[plotIndex] = valueIndex;
}

void LiveClient::drawPlots(float width, float height) {
  for(int i = 0; i < 4; i++) {
    plots[i]->draw(0, i * height, width, height);
  }
}

bool LiveClient::keyPressed(int key, int plotIndex) {
  if (key == OF_KEY_UP) {
    changePlot(plotIndex, (plotValueIndexes[plotIndex]+1) % static_cast<int>(AnalysisScalar::_count));
    resetPlots();
  } else if (key == OF_KEY_DOWN) {
    changePlot(plotIndex, (plotValueIndexes[plotIndex]-1+static_cast<int>(AnalysisScalar::_count)) % static_cast<int>(AnalysisScalar::_count));
    resetPlots();
  } else {
    return false;
  }
  return true;
}

void LiveClient::update() {
  int packetSize = tcpClient.receiveRawMsg(buf, MAX_PACKET_SIZE);
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
    scalarValues[static_cast<int>(AnalysisScalar::pitch)] = (*messageIter4++).AsFloat();
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

    packetSize = tcpClient.receiveRawMsg(buf, MAX_PACKET_SIZE);
  } // while clearing backlog
}

} //namespace
