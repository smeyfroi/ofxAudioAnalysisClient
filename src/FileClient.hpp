#pragma once

#include "BaseClient.hpp"
#include "ofxTCPClient.h"
#include "ofSoundPlayer.h"

namespace ofxAudioAnalysisClient {

constexpr float SAMPLE_RATE = 48000.0;
constexpr float SAMPLES_PER_FRAME = 1024.0;
constexpr float FRAMES_PER_MS = (SAMPLE_RATE / SAMPLES_PER_FRAME) / 1000.0;
inline int frameFromPositionMs(int positionMs) { return (float)positionMs * FRAMES_PER_MS; };
constexpr float OSC_FRAME_CHARS = 252; // TODO get this from metadata file
inline int frameToOscsOffset(int frame) { return frame * OSC_FRAME_CHARS; };

class FileClient : public BaseClient {
  
public:
  FileClient(std::string wavPath, std::string oscPath);
  std::ifstream oscsStream;
  bool keyPressed(int key) override;

protected:
  int nextOscPacket() override;

private:
  std::string wavPath, oscPath;
  ofSoundPlayer soundPlayer;
  uint64_t startTimeMs;
  uint64_t lastFrame;
  float soundPlayerVolume;
};

} // namespace
