#pragma once

#include "BaseClient.hpp"
#include "ofxSoundPlayerObject.h"
#include "ofSoundStream.h"
#include "ofxGist.h"
#include "ofxOsc.h"
#include "ofxSoundObjects.h"
#include "ofxSoundRecorderObject.h"
#include "NullOutput.h"
#include "dr_wav.h"
#include <atomic>
#include <mutex>

namespace ofxAudioAnalysisClient {

class LocalGistClient : public BaseClient, ofxSoundObject {
  
public:
  LocalGistClient(const std::string& deviceName, bool saveRecording, std::string recordingPath);
  LocalGistClient(const std::string& wavPath, const std::string& outDeviceName = "Apple Inc.: MacBook Pro Speakers", int _bufferSize = 2048, int _nChannels = 1,int _sampleRate = 48000); // defaults for saved Jamulus wav
  void stopRecording();
  void closeStream() override;

  void process(ofSoundBuffer &input, ofSoundBuffer &output) override; // ofxSoundObject
  void update() override {}; // from base class, but is done by process
  
  void playerEnded(size_t &id);
  bool keyPressed(int key) override;
  void setPositionSeconds(int seconds);

  // Segment recording - for recording audio clips synchronized with video recording
  void startSegmentRecording(const std::string& filepath);
  void stopSegmentRecording();
  bool isSegmentRecording() const;
  
protected:
  int nextOscPacket() override { return 0; }; // need to refactor the base class away from being just an OSC client
  
private:
  void setupGist();

  std::string wavPath;
  
  ofxSoundInput deviceInput;
  ofxSoundOutput deviceOutput;
  NullOutput nullOutput;
  ofSoundStream soundStream;
  
  ofxSoundPlayerObject soundPlayer; //file
  ofEventListener playerEndListener;
  float soundPlayerVolume = 1.0;
  
  std::unique_ptr<ofxSoundRecorderObject>& getRecorder() const;
  mutable std::unique_ptr<ofxSoundRecorderObject> recorderPtr;
  
  // Segment recording - direct WAV writing for synchronized audio/video recording
  drwav* segmentWavHandle { nullptr };
  std::string segmentFilepath;
  std::atomic<bool> segmentRecordingActive { false };
  std::atomic<bool> segmentRecordingPendingStart { false };
  std::atomic<bool> segmentRecordingPendingStop { false };
  std::mutex segmentMutex;
  
  ofxGist gist;
  unsigned int sampleRate;
  int bufferSize, nChannels;
};

}
