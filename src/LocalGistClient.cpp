#include "LocalGistClient.hpp"

namespace ofxAudioAnalysisClient {

// --- File input, device output
//LocalGistClient::LocalGistClient(std::string wavPath_) {
//  soundPlayer.load(wavPath_, false); // false to read the whole file; true to stream
//  ofSoundStreamSettings settings;
//  //  soundStream.printDeviceList();
//  //  auto devices = soundStream.getDeviceList();
//  //  if(!devices.empty()){
//  //    settings.setInDevice(devices[0]);
//  //  }
//  settings.numOutputChannels = 1;
//  settings.numInputChannels = 0;
//  settings.sampleRate = soundPlayer.getSoundFile().getSampleRate();
//  settings.bufferSize = 256; // device is 512?
//  settings.numBuffers = 1; // 4
//  settings.setInListener(this);
//  soundStream.setup(settings);
//}

// --- Device input, no output
LocalGistClient::LocalGistClient(int _bufferSize, int _nChannels,int _sampleRate) :
  ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR),
  bufferSize(_bufferSize),
  nChannels(_nChannels),
  sampleRate(_sampleRate)
{
  setupGist();
  setupInputDeviceSoundStream();
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

void LocalGistClient::setupInputDeviceSoundStream() {
  ofSoundStreamSettings settings;
  //  soundStream.printDeviceList();
  //  auto devices = soundStream.getDeviceList();
  //  if(!devices.empty()){
  //    settings.setInDevice(devices[0]);
  //  }
  settings.numOutputChannels = 1;
  settings.numInputChannels = nChannels;
  settings.sampleRate = sampleRate;
  settings.bufferSize = bufferSize;
  settings.numBuffers = 1; // 4
  soundStream.setup(settings);
  soundStream.setInput(deviceInput);
  soundStream.setOutput(deviceOutput);
  
  deviceInput.connectTo(*this).connectTo(nullOutput);
}

void LocalGistClient::process(ofSoundBuffer &input, ofSoundBuffer &output) {
  gist.processAudio(input.getBuffer(), bufferSize, nChannels, sampleRate);

  scalarValues[static_cast<int>(AnalysisScalar::rootMeanSquare)] = gist.getValue(GIST_ROOT_MEAN_SQUARE);
  scalarValues[static_cast<int>(AnalysisScalar::peakEnergy)] = gist.getValue(GIST_PEAK_ENERGY);
  scalarValues[static_cast<int>(AnalysisScalar::zeroCrossingRate)] = gist.getValue(GIST_ZERO_CROSSING_RATE);
  scalarValues[static_cast<int>(AnalysisScalar::spectralCentroid)] = gist.getValue(GIST_SPECTRAL_CENTROID);
  scalarValues[static_cast<int>(AnalysisScalar::spectralCrest)] = gist.getValue(GIST_SPECTRAL_CREST);
  scalarValues[static_cast<int>(AnalysisScalar::spectralFlatness)] = gist.getValue(GIST_SPECTRAL_FLATNESS);
//  scalarValues[static_cast<int>(AnalysisScalar::spectralRollof)] = gist.getValue(GIST_SPECTRAL_);
//  scalarValues[static_cast<int>(AnalysisScalar::spectralKurtosis)] = gist.getValue(GIST_K);
//  scalarValues[static_cast<int>(AnalysisScalar::energyDifference)] gist.getValue(GIST_);
  scalarValues[static_cast<int>(AnalysisScalar::spectralDifference)] = gist.getValue(GIST_SPECTRAL_DIFFERENCE);
//  scalarValues[static_cast<int>(AnalysisScalar::spectralDifferenceHWR)] gist.getValue(GIST_SPEC);
  scalarValues[static_cast<int>(AnalysisScalar::complexSpectralDifference)] = gist.getValue(GIST_SPECTRAL_DIFFERENCE_COMPLEX);
  // SPECTRAL_DIFFERENCE_HALFWAY
  scalarValues[static_cast<int>(AnalysisScalar::highFrequencyContent)] = gist.getValue(GIST_HIGH_FREQUENCY_CONTENT);
  float pitchEstimate = gist.getValue(GIST_PITCH);
  if (pitchEstimate < 10000) scalarValues[static_cast<int>(AnalysisScalar::pitch)] = pitchEstimate;
  
  output = input;
}


}
