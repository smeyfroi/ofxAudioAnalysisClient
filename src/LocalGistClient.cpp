#include "LocalGistClient.hpp"

namespace ofxAudioAnalysisClient {

// --- File input, device output
LocalGistClient::LocalGistClient(const std::string& _wavPath, const std::string& outDeviceName, int _bufferSize, int _nChannels, int _sampleRate) :
  ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR),
  wavPath(_wavPath),
  bufferSize(_bufferSize),
  nChannels(_nChannels),
  sampleRate(_sampleRate)
{
  setupGist();
  
  soundPlayer.load(wavPath, false); // false to read the whole file; true to stream
  
  ofSoundStreamSettings settings;
  ofxSoundUtils::printOutputSoundDevices();

  auto outDevices = ofxSoundUtils::getOutputSoundDevices();
  auto deviceIter = std::find_if(outDevices.cbegin(), outDevices.cend(), [&](const auto& d) {
    return (outDeviceName.empty() && d.isDefaultInput) || (d.name == outDeviceName);
  });
  if (deviceIter == outDevices.end()) {
    ofLogError() << "No device called '" << outDeviceName << "'";
  }
  
  settings.setOutDevice(*deviceIter);
  ofLogNotice() << "Using output device: " << deviceIter->name;
  
  settings.numInputChannels = 0;
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
LocalGistClient::LocalGistClient(const std::string& deviceName, bool saveRecording, std::string recordingPath) :
  ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR)
{
  setupGist();

  ofSoundStreamSettings settings;
  ofxSoundUtils::printInputSoundDevices();
  
  auto inDevices = ofxSoundUtils::getInputSoundDevices();
  auto deviceIter = std::find_if(inDevices.cbegin(), inDevices.cend(), [&](const auto& d) {
    return (deviceName.empty() && d.isDefaultInput) || (d.name == deviceName);
  });
  if (deviceIter == inDevices.end()) {
    ofLogError() << "No device called '" << deviceName << "'";
//    ofExit();
    // Assume IOS and set defaults for that
    nChannels = 1;
    sampleRate = 44100;
    bufferSize = 512;
    // don't set ofSoundStreamSettings device on IOS
  } else {
    nChannels = deviceIter->inputChannels;
    sampleRate = deviceIter->sampleRates[0];
    bufferSize = 256; // 512
    settings.setInDevice(*deviceIter);
  }
  
  settings.numInputChannels = nChannels;
  settings.numOutputChannels = 1;
  settings.sampleRate = sampleRate;
  settings.bufferSize = bufferSize;
  settings.numBuffers = 1; // 4

  soundStream.setup(settings);
  soundStream.setOutput(nullOutput);

  deviceInput.setInputStream(soundStream);
  deviceInput.connectTo(*getRecorder()).connectTo(*this).connectTo(nullOutput);
  
  if (saveRecording) {
    getRecorder()->startRecording(recordingPath + "/audio-"+ofGetTimestampString()+".wav", false);
  }
}

std::unique_ptr<ofxSoundRecorderObject>& LocalGistClient::getRecorder() const {
  if (!recorderPtr) recorderPtr = std::make_unique<ofxSoundRecorderObject>();
  return recorderPtr;
}

void LocalGistClient::startSegmentRecording(const std::string& filepath) {
  std::lock_guard<std::mutex> lock(segmentMutex);
  if (segmentRecordingActive) {
    ofLogWarning("LocalGistClient") << "Segment recording already active, ignoring start request";
    return;
  }
  segmentFilepath = filepath;
  segmentRecordingPendingStart = true;
  ofLogNotice("LocalGistClient") << "Segment recording pending start to: " << filepath;
}

void LocalGistClient::stopSegmentRecording() {
  std::lock_guard<std::mutex> lock(segmentMutex);
  if (!segmentRecordingActive && !segmentRecordingPendingStart) {
    return;
  }
  segmentRecordingPendingStop = true;
  ofLogNotice("LocalGistClient") << "Segment recording pending stop";
}

bool LocalGistClient::isSegmentRecording() const {
  return segmentRecordingActive || segmentRecordingPendingStart;
}

void LocalGistClient::stopRecording() {
  if (getRecorder()->isRecording()) {
    getRecorder()->stopRecording();
    while(getRecorder()->isRecording()) {
      ofLogNotice() << ofGetTimestampString() << ": " << getRecorder()->getRecStateString();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
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
  // Ensure segment recording is stopped before closing stream
  if (segmentWavHandle) {
    drwav_uninit(segmentWavHandle);
    segmentWavHandle = nullptr;
    segmentRecordingActive = false;
    ofLogNotice("LocalGistClient") << "Segment recording closed on stream shutdown";
  }
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
  // Handle segment recording state transitions and write audio data
  // This runs on the audio thread, so we use atomics for thread-safe state checks
  if (segmentRecordingPendingStart.exchange(false)) {
    // Initialize WAV file for writing
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.channels = input.getNumChannels();
    format.sampleRate = input.getSampleRate();
    format.bitsPerSample = 32;
    
    segmentWavHandle = drwav_open_file_write(segmentFilepath.c_str(), &format);
    if (segmentWavHandle) {
      segmentRecordingActive = true;
      ofLogNotice("LocalGistClient") << "Segment recording started: " << segmentFilepath
        << " (channels: " << format.channels << ", sampleRate: " << format.sampleRate << ")";
    } else {
      ofLogError("LocalGistClient") << "Failed to open segment recording file: " << segmentFilepath;
    }
  }
  
  if (segmentRecordingPendingStop.exchange(false)) {
    if (segmentWavHandle) {
      drwav_uninit(segmentWavHandle);
      segmentWavHandle = nullptr;
      segmentRecordingActive = false;
      ofLogNotice("LocalGistClient") << "Segment recording stopped: " << segmentFilepath;
    }
  }
  
  // Write audio data if recording is active
  if (segmentRecordingActive && segmentWavHandle) {
    drwav_uint64 framesWritten = drwav_write_pcm_frames(segmentWavHandle, input.getNumFrames(), input.getBuffer().data());
    if (framesWritten != input.getNumFrames()) {
      ofLogWarning("LocalGistClient") << "Segment recording: wrote " << framesWritten << " frames, expected " << input.getNumFrames();
    }
  }

  if (soundPlayerVolume > 0.0) {
    output = input;
  }
}

void LocalGistClient::playerEnded(size_t &id) {
  ofLogNotice() << "File stream ended";
}

void LocalGistClient::setPositionSeconds(int seconds) {
  if (!soundPlayer.isLoaded()) {
    ofLogWarning("LocalGistClient") << "setPositionSeconds(): audio file not loaded";
    return;
  }
  int ms = seconds * 1000;
  int durationMS = static_cast<int>(soundPlayer.getDurationMS());
  if (ms < 0 || ms > durationMS) {
    ofLogWarning("LocalGistClient") << "setPositionSeconds(): " << seconds << "s is out of range (duration: " << durationMS / 1000 << "s)";
    ms = std::clamp(ms, 0, durationMS);
  }
  soundPlayer.setPositionMS(ms);
  ofLogNotice("LocalGistClient") << "setPositionSeconds(): set to " << seconds << "s";
}

bool LocalGistClient::keyPressed(int key) {
  if (key == '`') {
    soundPlayerVolume = 1.0 - soundPlayerVolume;
    soundPlayer.setVolume(soundPlayerVolume);
    return true;
  }
  
  // Audio playback seeking (only when playing from file)
  if (soundPlayer.isLoaded()) {
    int skipMS = ofGetKeyPressed(OF_KEY_SHIFT) ? 60000 : 10000; // 60s or 10s
    
    if (key == OF_KEY_UP) {
      // Skip forward
      int currentMS = soundPlayer.getPositionMS();
      int durationMS = soundPlayer.getDurationMS();
      soundPlayer.setPositionMS(std::min(currentMS + skipMS, (int)durationMS));
      ofLogNotice("LocalGistClient") << "Skip forward to " << soundPlayer.getPositionMS() / 1000 << "s";
      return true;
    }
    
    if (key == OF_KEY_DOWN) {
      // Skip backward
      int currentMS = soundPlayer.getPositionMS();
      soundPlayer.setPositionMS(std::max(0, currentMS - skipMS));
      ofLogNotice("LocalGistClient") << "Skip backward to " << soundPlayer.getPositionMS() / 1000 << "s";
      return true;
    }
  }
  
  return BaseClient::keyPressed(key);
}

}
