#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetBackgroundColor(0);
  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>("", false, "");
  audioDataProcessorPtr = std::make_shared<ofxAudioData::Processor>(audioAnalysisClientPtr);
  audioDataPlotsPtr = std::make_shared<ofxAudioData::Plots>(audioDataProcessorPtr);
  introspector.visible = true;
  audioDataPlotsPtr->plotsVisible = true;
}

//--------------------------------------------------------------
void ofApp::update(){
  introspector.update();
  audioDataProcessorPtr->update();
  
  float s = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::pitch);
  float t = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::rootMeanSquare);
  float u = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::complexSpectralDifference);
  float v = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::spectralCrest);
  float w = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::zeroCrossingRate);
  
  introspector.addCircle(s, t, 1.0/ofGetWindowWidth()*5.0, ofColor::darkGreen, true, 30); // introspection: small yellow circle for new raw source sample
  introspector.addLine(0.0, u, 1.0, u, ofColor::grey);
  introspector.addLine(0.0, v, 1.0, v, ofColor::darkGreen);
  introspector.addLine(0.0, w, 1.0, w, ofColor::greenYellow);
}

//--------------------------------------------------------------
void ofApp::draw(){
  introspector.draw(ofGetWindowWidth());
  audioDataPlotsPtr->drawPlots();
}

//--------------------------------------------------------------
void ofApp::exit(){
  audioAnalysisClientPtr->closeStream();
}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

}

//--------------------------------------------------------------
void ofApp::launchedWithURL(std::string url){

}
