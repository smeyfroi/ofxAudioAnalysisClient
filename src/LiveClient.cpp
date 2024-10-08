#include "LiveClient.hpp"
#include "ofLog.h"
#include "ofAppRunner.h"

namespace ofxAudioAnalysisClient {

LiveClient::LiveClient(std::string host, short port) {
  if (!tcpClient.setup(host, port)) {
    ofLogError() << "Failed to open TCP connection to " << host << ":" << port << std::endl;
    ofExit(-1);
  } else {
    ofLogNotice() << "Connected to analyser " << host << ":" << port << std::endl;
  }
}

int LiveClient::nextOscPacket() {
  return tcpClient.receiveRawMsg(buf, MAX_PACKET_SIZE);
}

} // namespace
