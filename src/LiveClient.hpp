#pragma once

#include "BaseClient.hpp"
#include "ofxTCPClient.h"

namespace ofxAudioAnalysisClient {

class LiveClient : public BaseClient {
  
public:
  LiveClient(std::string host, short port);

protected:
  int nextOscPacket() override;
  
private:
  ofxTCPClient tcpClient;

};

} // namespace
