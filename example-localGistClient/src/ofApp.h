#pragma once

#include "ofMain.h"
#include "ofxAudioAnalysisClient.h"
#include "ofxAudioData.h"
#include "ofxIntrospector.h"

class ofApp: public ofBaseApp{
public:
  void setup();
  void update();
  void draw();
  void exit();
  
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);
  
private:
  std::shared_ptr<ofxAudioAnalysisClient::LocalGistClient> audioAnalysisClientPtr;
  std::shared_ptr<ofxAudioData::Processor> audioDataProcessorPtr;
  std::shared_ptr<ofxAudioData::Plots> audioDataPlotsPtr;
  std::shared_ptr<ofxAudioData::SpectrumPlots> audioDataSpectrumPlotsPtr;
  
  Introspector introspector;

};
