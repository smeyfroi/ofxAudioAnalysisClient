#include "LocalGistClient.hpp"

namespace ofxAudioAnalysisClient {

// --- File input, device output
LocalGistClient::LocalGistClient(std::string _wavPath, int _bufferSize, int _nChannels,int _sampleRate) :
  ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR),
  wavPath(_wavPath),
  bufferSize(_bufferSize),
  nChannels(_nChannels),
  sampleRate(_sampleRate)
{
  setupGist();
  
  soundPlayer.load(wavPath, false); // false to read the whole file; true to stream
  
  //  ofxSoundUtils::printOutputSoundDevices();
  //  auto outDevices = ofxSoundUtils::getOutputSoundDevices();
  //  int outDeviceIndex = 0;
  //  cout << ofxSoundUtils::getSoundDeviceString(outDevices[outDeviceIndex], false, true) << endl;
  
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
//  soundPlayer.setLoop(true);
  playerEndListener = soundPlayer.endEvent.newListener(this, &LocalGistClient::playerEnded);
}

// --- Device input, no output
LocalGistClient::LocalGistClient(int _bufferSize, int _nChannels,int _sampleRate) :
  ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR),
  bufferSize(_bufferSize),
  nChannels(_nChannels),
  sampleRate(_sampleRate)
{
  setupGist();
  
  //  ofxSoundUtils::printInputSoundDevices();
  //  auto inDevices = ofxSoundUtils::getInputSoundDevices();
  //  int inDeviceIndex = 0;
  //  cout << ofxSoundUtils::getSoundDeviceString(inDevices[inDeviceIndex], false, true) << endl;

  ofSoundStreamSettings settings;
  settings.numInputChannels = nChannels;
  settings.numOutputChannels = 1;
  settings.sampleRate = sampleRate;
  settings.bufferSize = bufferSize;
  settings.numBuffers = 1; // 4
  soundStream.setup(settings);
  soundStream.setInput(deviceInput);
  soundStream.setOutput(deviceOutput);
  
  deviceInput.connectTo(*this).connectTo(nullOutput);
}

void LocalGistClient::setupGist() {
  {
    vector<string> features = ofxGist::getFeatureNames();
    int num = features.size();
    for(int v=0; v<num; v++) {
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

  scalarValues[static_cast<int>(AnalysisScalar::rootMeanSquare)] = gist.getValue(GIST_ROOT_MEAN_SQUARE);
  scalarValues[static_cast<int>(AnalysisScalar::peakEnergy)] = gist.getValue(GIST_PEAK_ENERGY);
  scalarValues[static_cast<int>(AnalysisScalar::zeroCrossingRate)] = gist.getValue(GIST_ZERO_CROSSING_RATE);
  scalarValues[static_cast<int>(AnalysisScalar::spectralCentroid)] = gist.getValue(GIST_SPECTRAL_CENTROID);
  scalarValues[static_cast<int>(AnalysisScalar::spectralCrest)] = gist.getValue(GIST_SPECTRAL_CREST);
//  scalarValues[static_cast<int>(AnalysisScalar::spectralFlatness)] = gist.getValue(GIST_SPECTRAL_FLATNESS);
//  scalarValues[static_cast<int>(AnalysisScalar::spectralRollof)] = gist.getValue(GIST_SPECTRAL_);
//  scalarValues[static_cast<int>(AnalysisScalar::spectralKurtosis)] = gist.getValue(GIST_K);
//  scalarValues[static_cast<int>(AnalysisScalar::energyDifference)] gist.getValue(GIST_);
  scalarValues[static_cast<int>(AnalysisScalar::spectralDifference)] = gist.getValue(GIST_SPECTRAL_DIFFERENCE);
//  scalarValues[static_cast<int>(AnalysisScalar::spectralDifferenceHWR)] gist.getValue(GIST_SPEC);
//  scalarValues[static_cast<int>(AnalysisScalar::complexSpectralDifference)] = gist.getValue(GIST_SPECTRAL_DIFFERENCE_COMPLEX);
  // SPECTRAL_DIFFERENCE_HALFWAY
//  scalarValues[static_cast<int>(AnalysisScalar::highFrequencyContent)] = gist.getValue(GIST_HIGH_FREQUENCY_CONTENT);
  float pitchEstimate = gist.getValue(GIST_PITCH);
  if (pitchEstimate < 10000) scalarValues[static_cast<int>(AnalysisScalar::pitch)] = pitchEstimate;
  
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
