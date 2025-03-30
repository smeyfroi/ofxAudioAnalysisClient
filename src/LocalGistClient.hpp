#pragma once

#include "BaseClient.hpp"
#include "ofxSoundPlayerObject.h"
#include "ofSoundStream.h"
#include "ofxGist.h"
#include "ofxOsc.h"
#include "ofxSoundObjects.h"

namespace ofxAudioAnalysisClient {

class NullOutput : public ofxSoundOutput {
  void process(ofSoundBuffer &input, ofSoundBuffer &output) override {};
};

class LocalGistClient : public BaseClient, ofxSoundObject {
  
public:
  LocalGistClient(int _bufferSize = 512, int _nChannels = 1,int _sampleRate = 44100);
//  LocalGistClient(std::string wavPath);
  void process(ofSoundBuffer &input, ofSoundBuffer &output) override; // ofxSoundObject
  void update() override {}; // from base class, but is done by process
  
protected:
  int nextOscPacket() override { return 0; }; // need to refactor the base class away from being just an OSC client
  
private:
  void setupGist();

  //  std::string wavPath;
  
  ofxSoundInput deviceInput;
  ofxSoundOutput deviceOutput;
  ofxSoundOutput nullOutput;
  
  void setupInputDeviceSoundStream();
  ofSoundStream soundStream;
  
  ofxSoundPlayerObject soundPlayer; //file
  ofEventListener playerEndListener;
  void playerEnded(size_t & id);
  
  ofxGist gist;
  int bufferSize, nChannels, sampleRate;
};

}
