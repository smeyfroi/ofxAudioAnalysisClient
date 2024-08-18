#pragma once

#include <array>

namespace ofxAudioAnalysisClient {

// float scalars from the analysis
enum class AnalysisScalar {
  rootMeanSquare, peakEnergy, zeroCrossingRate,
  spectralCentroid, spectralCrest, spectralFlatness, spectralRollof, spectralKurtosis,
  energyDifference, spectralDifference, spectralDifferenceHWR, complexSpectralDifference, highFrequencyContent,
  pitch,
  _count // index beyond final element
};

class BaseClient {

public:
  BaseClient() {};
  virtual ~BaseClient() {};
  inline float getScalarValue(AnalysisScalar scalar) {
    return getScalarValue(static_cast<int>(scalar));
  };
  inline float getScalarValue(int scalarIndex) {
    return scalarValues[scalarIndex];
  };
  inline float* getScalarValuePtr(int scalarIndex) {
    return &scalarValues[scalarIndex];
  }
  
  float frequencyToMidi(float freq) const;
  float getNoteFrequency() const;
  const std::string getNoteName() const;
  const std::pair<float, float> getNote() const;

  virtual void update() { updateOsc(); };
  virtual bool keyPressed(int key) { return false; };

protected:
  static constexpr int MAX_PACKET_SIZE = 512;
  char buf[MAX_PACKET_SIZE];
  virtual int nextOscPacket() = 0;

private:
  std::array<float, static_cast<int>(ofxAudioAnalysisClient::AnalysisScalar::_count)> scalarValues;
  std::vector<float> mfcc;
  //  std::vector<float> spectrum, mel;

  void updateOsc();
};

} // namespace
