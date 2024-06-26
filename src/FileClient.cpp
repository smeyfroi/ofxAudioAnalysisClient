#include "FileClient.hpp"

namespace ofxAudioAnalysisClient {

FileClient::FileClient(std::string directoryName_)
: directoryName(directoryName_),
  lastFrame(-1),
  soundPlayerVolume(0)
{
  soundPlayer.load(directoryName + "/____-46_137_90_x_22141-0-1.wav");
  std::string oscsPath = ofToDataPath(directoryName + "/____-46_137_90_x_22141.oscs");
  oscsStream = std::ifstream(oscsPath, std::ios::binary);
  if (!oscsStream.is_open()) {
    ofLogError() << "Failed to open " << oscsPath;
    ofExit();
  }

  soundPlayer.setVolume(soundPlayerVolume);
  soundPlayer.play();
  startTimeMs = ofGetElapsedTimeMillis();
}

bool FileClient::keyPressed(int key, int plotIndex) {
  if (key == '`') {
    soundPlayerVolume = 1.0 - soundPlayerVolume;
    soundPlayer.setVolume(soundPlayerVolume);
    return true;
  }
  return BaseClient::keyPressed(key, plotIndex);
}

int FileClient::nextOscPacket() {
  uint64_t positionMs = ofGetElapsedTimeMillis() - startTimeMs;
  int frame = frameFromPositionMs(positionMs);
  if (frame == lastFrame) {
    return 0;
  }
  lastFrame = frame;

  int offset = frameToOscsOffset(frame);
  oscsStream.seekg(offset);
  oscsStream.read(buf, OSC_FRAME_CHARS);
  return OSC_FRAME_CHARS;
}

} // namespace
