#pragma once

#include "ofxNetwork.h"
#include "ofxHistoryPlot.h"

namespace ofxAudioAnalysisClient {

// float scalars from the analysis
enum class AnalysisScalar {
  rootMeanSquare, peakEnergy, zeroCrossingRate,
  spectralCentroid, spectralCrest, spectralFlatness, spectralRollof, spectralKurtosis,
  energyDifference, spectralDifference, spectralDifferenceHWR, complexSpectralDifference, highFrequencyContent,
  pitch,
  _count // index beyond final element
};

class LiveClient {

public:
  LiveClient(std::string host, short port);
  inline float getScalarValue(AnalysisScalar scalar) {
    return getScalarValue(static_cast<int>(scalar));
  };
  inline float getScalarValue(int scalarIndex) {
    return scalarValues[scalarIndex];
  };
  void update();
  void drawPlots(float width, float height);
  bool keyPressed(int key, int plotIndex);

private:
  ofxTCPClient tcpClient;
  static constexpr int MAX_PACKET_SIZE = 512;
  char buf[MAX_PACKET_SIZE];

  std::vector<std::unique_ptr<ofxHistoryPlot>> plots;
  std::vector<size_t> plotValueIndexes;
  void resetPlots();
  void changePlot(size_t plotIndex, size_t valueIndex);

  std::array<float, static_cast<int>(AnalysisScalar::_count)> scalarValues;
  std::vector<float> mfcc;
  //  std::vector<float> spectrum, mel;

  std::array<float, static_cast<int>(AnalysisScalar::_count)> minScalarValues {
    0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0
  };
  std::array<float, static_cast<int>(AnalysisScalar::_count)> maxScalarValues {
    10000, 10000, 30,
    30, 50, 1, 1, 50,
    50000000, 2000000, 2000000, 10000000, 20000000,
    4000
  };
  const std::array<std::string, static_cast<int>(AnalysisScalar::_count)> scalarNames {
    "Root Mean Square", "Peak Energy", "Zero Crossing Rate",
    "SpectralCentroid", "Spectral Crest", "Spectral Flatness", "Spectral Rollof", "Spectral Kurtosis",
    "Energy Difference", "Spectral Difference", "Spectral Difference HWR", "Complex Spectral Difference", "High Frequency Content",
    "Pitch"
  };

};

} // namespace
