#include "BaseClient.hpp"
#include <algorithm>
#include "ofxOsc.h"
#include "OscPrintReceivedElements.h"
#include "ofAppRunner.h"

namespace ofxAudioAnalysisClient {

const std::array<std::string, AnalysisScalar::Value::_count> AnalysisScalar::names = {
  "Root Mean Square", "Peak Energy", "Zero Crossing Rate",
  "SpectralCentroid", "Spectral Crest",
  "Energy Difference", "Spectral Difference", "Spectral Difference Complex",
  "Pitch"
};

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
    scalarValues[AnalysisScalar::rootMeanSquare] = (*messageIter1++).AsFloat();
    scalarValues[AnalysisScalar::peakEnergy] = (*messageIter1++).AsFloat();
    scalarValues[AnalysisScalar::zeroCrossingRate] = (*messageIter1++).AsFloat();
    // /freq
    osc::ReceivedBundleElement element2 = *bundleIter++;
    osc::ReceivedMessage message2(element2);
    osc::ReceivedMessage::const_iterator messageIter2 = message2.ArgumentsBegin();
    scalarValues[AnalysisScalar::spectralCentroid] = (*messageIter2++).AsFloat();
    scalarValues[AnalysisScalar::spectralCrest] = (*messageIter2++).AsFloat();
    // /onset
    osc::ReceivedBundleElement element3 = *bundleIter++;
    osc::ReceivedMessage message3(element3);
    osc::ReceivedMessage::const_iterator messageIter3 = message3.ArgumentsBegin();
    scalarValues[AnalysisScalar::energyDifference] = (*messageIter3++).AsFloat();
    scalarValues[AnalysisScalar::spectralDifference] = (*messageIter3++).AsFloat();
    messageIter3++;
    scalarValues[static_cast<int>(AnalysisScalar::complexSpectralDifference)] = (*messageIter3++).AsFloat();
    // /pitch
    osc::ReceivedBundleElement element4 = *bundleIter++;
    osc::ReceivedMessage message4(element4);
    osc::ReceivedMessage::const_iterator messageIter4 = message4.ArgumentsBegin();
    float pitchEstimate = (*messageIter4++).AsFloat();
    if (pitchEstimate < 2000) {
      scalarValues[AnalysisScalar::pitch] = pitchEstimate;
    }
    // /spectrum
//    osc::ReceivedBundleElement element5 = *bundleIter++;
//    osc::ReceivedMessage message5(element5);
//    if (spectrum.size() != message5.ArgumentCount()) {
//      if (spectrum.size() != 0) ofLogError() << "spectrum of size " << message5.ArgumentCount() << " != previous size " << spectrum.size();
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
//      if (mel.size() != 0) ofLogError() << "mel of size " << message6.ArgumentCount() << " != previous size " << mel.size();
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
      if (mfcc.size() != 0) ofLogError() << "mfcc of size " << message7.ArgumentCount() << " != previous size " << mfcc.size();
      mfcc.resize(message7.ArgumentCount());
    }
    int i = 0;
    for (auto iter = message7.ArgumentsBegin(); iter != message7.ArgumentsEnd(); iter++) {
      mfcc[i++] = (*iter).AsFloat();
    }

    packetSize = nextOscPacket();
  } // while clearing backlog
  
  // TODO: detect end of stream so can report back
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
      return frequencyToMidi(scalarValues[AnalysisScalar::pitch]);
//  }
};

const std::string n[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
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
