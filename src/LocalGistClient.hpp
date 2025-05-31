#pragma once

#include "BaseClient.hpp"
#include "ofxSoundPlayerObject.h"
#include "ofSoundStream.h"
#include "ofxGist.h"
#include "ofxOsc.h"
#include "ofxSoundObjects.h"
#include "ofxSoundRecorderObject.h"
#include "NullOutput.h"

namespace ofxAudioAnalysisClient {

class LocalGistClient : public BaseClient, ofxSoundObject {
  
public:
  LocalGistClient(bool saveRecording, std::string recordingPath); // default sound input device
  LocalGistClient(std::string wavPath, int _bufferSize = 256, int _nChannels = 1,int _sampleRate = 48000); // defaults for saved Jamulus wav
  void stopRecording();
  void closeStream() override;

  void process(ofSoundBuffer &input, ofSoundBuffer &output) override; // ofxSoundObject
  void update() override {}; // from base class, but is done by process
  
  void playerEnded(size_t &id);
  bool keyPressed(int key) override;

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
  
  ofxSoundRecorderObject recorder;
  
  ofxGist gist;
  int bufferSize, nChannels, sampleRate;
};

}
