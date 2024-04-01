#pragma once

#include "BaseClient.hpp"
#include "ofxTCPClient.h"

namespace ofxAudioAnalysisClient {

constexpr float SAMPLE_RATE = 48000.0;
constexpr float SAMPLES_PER_FRAME = 128.0;
constexpr float FRAMES_PER_MS = (SAMPLE_RATE / SAMPLES_PER_FRAME) / 1000.0;
inline int frameFromPositionMs(int positionMs) { return (float)positionMs*FRAMES_PER_MS; };

class FileClient : public BaseClient {
  
public:
  FileClient(std::string directoryName_);
  void update() override;

protected:
  int nextOscPacket() override;

private:
  std::string directoryName;
  ofSoundPlayer soundPlayer;
  uint64_t startTimeMs;

};

} // namespace
