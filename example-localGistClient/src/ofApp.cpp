#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("violin.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("Nightsong.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("Treganna.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("bells-descending-peal.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("violin-tune.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>();
  audioDataProcessorPtr = std::make_shared<ofxAudioData::Processor>(audioAnalysisClientPtr);
  audioDataPlotsPtr = std::make_shared<ofxAudioData::Plots>(audioDataProcessorPtr);
  audioDataSpectrumPlotsPtr = std::make_shared<ofxAudioData::SpectrumPlots>(audioDataProcessorPtr);
}

//--------------------------------------------------------------
void ofApp::update() {
  introspector.update();
  audioDataProcessorPtr->update();
  
  float s = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::pitch, 0.0, 1000.0);
  float t = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::rootMeanSquare, 0.0, 0.1);

  introspector.addCircle(s, t, 1.0/ofGetWindowWidth()*5.0, ofColor::yellow, true, 30); // introspection: small yellow circle for new raw source sample
}

//--------------------------------------------------------------
void ofApp::draw() {
  introspector.draw(ofGetWindowWidth());

  // audio analysis graphs
  {
    ofPushStyle();
    ofPushView();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    float plotHeight = ofGetWindowHeight() / 4.0;
    audioDataPlotsPtr->drawPlots(ofGetWindowWidth(), plotHeight);
    audioDataSpectrumPlotsPtr->draw();
    ofPopView();
    ofPopStyle();
  }
}

//--------------------------------------------------------------
void ofApp::exit(){
  audioAnalysisClientPtr->closeStream();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (audioAnalysisClientPtr->keyPressed(key)) return;
  if (introspector.keyPressed(key)) return;

  {
    float plotHeight = ofGetWindowHeight() / 4.0;
    int plotIndex = ofGetMouseY() / plotHeight;
    bool plotKeyPressed = audioDataPlotsPtr->keyPressed(key, plotIndex);
    bool spectrumPlotKeyPressed = audioDataSpectrumPlotsPtr->keyPressed(key);
    if (plotKeyPressed || spectrumPlotKeyPressed) return;
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
