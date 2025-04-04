#pragma once

#include <array>
#include <vector>

//Spectral Difference, derivate, shows the amount of change…effectively onset
//Spectral Crest - How tonal the signal is, useful for distinguishing instuments
//Spectral Centroid - Correlates to the brightness of the sound
//Mel-frequency spectrum - Human perception FFT
//Zero Crossing Rate - Sound brightness
//Root mean square - Signal energy

namespace ofxAudioAnalysisClient {

// float scalars from the analysis
enum class AnalysisScalar {
  rootMeanSquare, peakEnergy, zeroCrossingRate,
  spectralCentroid, spectralCrest,
  energyDifference, spectralDifference,
  pitch,
  _count // index beyond final element
};
//enum class AnalysisScalar {
//  rootMeanSquare, peakEnergy, zeroCrossingRate,
//  spectralCentroid, spectralCrest, spectralFlatness, spectralRollof, spectralKurtosis,
//  energyDifference, spectralDifference, spectralDifferenceHWR, complexSpectralDifference, highFrequencyContent,
//  pitch,
//  _count // index beyond final element
//};

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
  inline auto& getMfcc() const { return mfcc; }
  
  float frequencyToMidi(float freq) const;
  float getNoteFrequency() const;
  const std::string getNoteName() const;
  const std::pair<float, float> getNote() const;

  virtual void update() { updateOsc(); };
  virtual bool keyPressed(int key) { return false; };

protected:
  static constexpr int MAX_PACKET_SIZE = 512; // refactor: this isn't used for LocalGistClient
  char buf[MAX_PACKET_SIZE]; // refactor: this isn't used for LocalGistClient
  virtual int nextOscPacket() = 0; // refactor: this isn't used for LocalGistClient

  std::array<float, static_cast<int>(ofxAudioAnalysisClient::AnalysisScalar::_count)> scalarValues;
  std::vector<float> mfcc;
  //  std::vector<float> spectrum, mel;

  void updateOsc(); // rename: this isn't anything to do with OSC for LocalGistClient

private:
};

} // namespace
