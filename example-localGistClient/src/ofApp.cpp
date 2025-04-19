#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("violin.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("trombone.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("Nightsong.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("Treganna.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("bells-descending-peal.wav"));
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(ofToDataPath("violin-tune.wav"));
  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>();
  audioDataProcessorPtr = std::make_shared<ofxAudioData::Processor>(audioAnalysisClientPtr);
  audioDataPlotsPtr = std::make_shared<ofxAudioData::Plots>(audioDataProcessorPtr);
  
  audioDataProcessorPtr->setMAWindow(ofxAudioAnalysisClient::AnalysisScalar::pitch, 16);
  audioDataProcessorPtr->setMAWindow(ofxAudioAnalysisClient::AnalysisScalar::rootMeanSquare, 4);
  audioDataProcessorPtr->setMAWindow(ofxAudioAnalysisClient::AnalysisScalar::complexSpectralDifference, 4);
  audioDataProcessorPtr->setMAWindow(ofxAudioAnalysisClient::AnalysisScalar::spectralCrest, 32);
  audioDataProcessorPtr->setMAWindow(ofxAudioAnalysisClient::AnalysisScalar::zeroCrossingRate, 4);
}

//--------------------------------------------------------------
void ofApp::update() {
  introspector.update();
  audioDataProcessorPtr->update();
  
//  float s = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::pitch, 0.0, 1000.0);
//  float t = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::rootMeanSquare, 0.0, 0.15);
//  float u = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::complexSpectralDifference, 0.0, 100.0);
//  float v = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::spectralCrest, 0.0, 100.0);
//  float w = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::zeroCrossingRate, 0.0, 20.0);

  float s = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::pitch);
  float t = audioDataProcessorPtr->getNormalisedScalarValue(ofxAudioAnalysisClient::AnalysisScalar::rootMeanSquare);
  float u = audioDataProcessorPtr->getNormalisedScalarValueMA(ofxAudioAnalysisClient::AnalysisScalar::complexSpectralDifference);
  float v = audioDataProcessorPtr->getNormalisedScalarValueMA(ofxAudioAnalysisClient::AnalysisScalar::spectralCrest);
  float w = audioDataProcessorPtr->getNormalisedScalarValueMA(ofxAudioAnalysisClient::AnalysisScalar::zeroCrossingRate);

  introspector.addCircle(s, t, 1.0/ofGetWindowWidth()*5.0, ofColor::darkGreen, true, 30); // introspection: small yellow circle for new raw source sample
  introspector.addLine(0.0, u, 1.0, u, ofColor::grey);
  introspector.addLine(0.0, v, 1.0, v, ofColor::darkGreen);
  introspector.addLine(0.0, w, 1.0, w, ofColor::greenYellow);
}

//--------------------------------------------------------------
void ofApp::draw() {
  introspector.draw(ofGetWindowWidth());
  audioDataPlotsPtr->drawPlots();
}

//--------------------------------------------------------------
void ofApp::exit(){
  audioAnalysisClientPtr->closeStream();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (audioAnalysisClientPtr->keyPressed(key)) return;
  if (introspector.keyPressed(key)) return;
  if (audioDataPlotsPtr->keyPressed(key)) return;
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
