#include "FileClient.hpp"

namespace ofxAudioAnalysisClient {

FileClient::FileClient(std::string directoryName_)
: directoryName(directoryName_)
{
  soundPlayer.load(directoryName + "/____-46_137_90_x_22141-0-1.wav");
  soundPlayer.play();
  startTimeMs = ofGetElapsedTimeMillis();
}

void FileClient::update() {
  uint64_t positionMs = ofGetElapsedTimeMillis() - startTimeMs;
  ofLogNotice() << "- " << positionMs;
  ofLogNotice() << "+ " << soundPlayer.getPositionMS();
  ofLogNotice() << "= " << frameFromPositionMs(positionMs);
}

int FileClient::nextOscPacket() {
  return 0;
}

} // namespace
