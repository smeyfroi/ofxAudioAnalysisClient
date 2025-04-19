#include "LocalGistClient.hpp"

namespace ofxAudioAnalysisClient {

// --- File input, device output
LocalGistClient::LocalGistClient(std::string _wavPath, int _bufferSize, int _nChannels, int _sampleRate) :
  ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR),
  wavPath(_wavPath),
  bufferSize(_bufferSize),
  nChannels(_nChannels),
  sampleRate(_sampleRate)
{
  setupGist();
  
  soundPlayer.load(wavPath, false); // false to read the whole file; true to stream
  
  ofxSoundUtils::printOutputSoundDevices();
  auto outDevices = ofxSoundUtils::getOutputSoundDevices();
  int outDeviceIndex = 0;
  cout << ofxSoundUtils::getSoundDeviceString(outDevices[outDeviceIndex], false, true) << endl;
  
  ofSoundStreamSettings settings;
  settings.numInputChannels = nChannels;
  settings.numOutputChannels = 2;
  settings.sampleRate = soundPlayer.getSoundFile().getSampleRate();
  settings.bufferSize = bufferSize; // 256
  settings.numBuffers = 1;
  soundStream.setup(settings);
  soundStream.setOutput(deviceOutput);
  
  soundPlayer.connectTo(*this).connectTo(deviceOutput);
  soundPlayer.play();
  soundPlayer.setLoop(true);
  playerEndListener = soundPlayer.endEvent.newListener(this, &LocalGistClient::playerEnded);
}

// --- Device input, no output
LocalGistClient::LocalGistClient() :
  ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR)
{
  setupGist();
  
  ofxSoundUtils::printInputSoundDevices();
  ofxSoundUtils::printOutputSoundDevices();
  
//  auto inDevices = ofxSoundUtils::getInputSoundDevices();
//  auto outDevices = ofxSoundUtils::getOutputSoundDevices();
//  
//  size_t inDeviceIndex = 2;
//  size_t outDeviceIndex = 3;
//  ofLogNotice() << ofToString(inDevices[inDeviceIndex].sampleRates);
//
//  nChannels = inDevices[inDeviceIndex].inputChannels;
  nChannels = 1;
  bufferSize = 256;
  sampleRate = 44100;
  
  ofSoundStreamSettings settings;
  settings.numInputChannels = nChannels;
  settings.numOutputChannels = 1;
  settings.sampleRate = sampleRate;
  settings.bufferSize = bufferSize;
  settings.numBuffers = 1; // 4
  soundStream.setup(settings);
  soundStream.setInput(deviceInput);
  soundStream.setOutput(nullOutput);

  deviceInput.connectTo(*this).connectTo(nullOutput);
}

void LocalGistClient::setupGist() {
  {
//    vector<string> features = ofxGist::getFeatureNames();
    const static vector<string> features {
      "GIST_ROOT_MEAN_SQUARE", "GIST_PEAK_ENERGY", "GIST_ZERO_CROSSING_RATE",
      "GIST_SPECTRAL_CENTROID", "GIST_SPECTRAL_CREST",
      "GIST_SPECTRAL_DIFFERENCE", "GIST_SPECTRAL_DIFFERENCE_COMPLEX",
      "GIST_PITCH"
    };
    int num = features.size();
    for(int v = 0; v < num; v++) {
      GIST_FEATURE f = ofxGist::getFeatureFromName(features[v]);
      gist.setDetect(f);
    }
  }
}

void LocalGistClient::closeStream() {
  soundStream.close();
}

void LocalGistClient::process(ofSoundBuffer &input, ofSoundBuffer &output) {
  gist.processAudio(input.getBuffer(), bufferSize, nChannels, sampleRate);

  scalarValues[AnalysisScalar::rootMeanSquare] = gist.getValue(GIST_ROOT_MEAN_SQUARE);
  scalarValues[AnalysisScalar::peakEnergy] = gist.getValue(GIST_PEAK_ENERGY);
  scalarValues[AnalysisScalar::zeroCrossingRate] = gist.getValue(GIST_ZERO_CROSSING_RATE);
  scalarValues[AnalysisScalar::spectralCentroid] = gist.getValue(GIST_SPECTRAL_CENTROID);
  scalarValues[AnalysisScalar::spectralCrest] = gist.getValue(GIST_SPECTRAL_CREST);
  scalarValues[AnalysisScalar::spectralDifference] = gist.getValue(GIST_SPECTRAL_DIFFERENCE);
  scalarValues[AnalysisScalar::complexSpectralDifference] = gist.getValue(GIST_SPECTRAL_DIFFERENCE_COMPLEX);
  float pitchEstimate = gist.getValue(GIST_PITCH);
  if (pitchEstimate < 5000.0 && pitchEstimate > 10.0) scalarValues[AnalysisScalar::pitch] = pitchEstimate;
  
//  auto mfccs = gist.getMelFrequencyCepstralCoefficients();
//  if (mfccs.size() != mfcc.size()) {
//    if (mfcc.size() != 0) ofLogError() << "mfcc of size " << mfccs.size() << " != previous size " << mfcc.size();
//    mfcc.resize(mfccs.size());
//  }
//  int i = 0;
//  for (auto iter = message7.ArgumentsBegin(); iter != message7.ArgumentsEnd(); iter++) {
//    mfcc[i++] = (*iter).AsFloat();
//  }
//
//  for (auto iter = gist.getMelFrequencyCepstralCoefficients().begin(); iter !=
  
  if (soundPlayerVolume > 0.0) {
    output = input;
  }
}

void LocalGistClient::playerEnded(size_t &id) {
  ofLogNotice() << "File stream ended";
}

bool LocalGistClient::keyPressed(int key) {
  if (key == '`') {
    soundPlayerVolume = 1.0 - soundPlayerVolume;
    soundPlayer.setVolume(soundPlayerVolume);
    return true;
  }
  return BaseClient::keyPressed(key);
}

}
