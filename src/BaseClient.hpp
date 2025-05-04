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
class AnalysisScalar {
public:
  enum Value : uint8_t {
    rootMeanSquare, peakEnergy, zeroCrossingRate,
    spectralCentroid, spectralCrest,
    spectralDifference, complexSpectralDifference,
    pitch,
    _count // index beyond final element
  };
  static const std::array<std::string, _count> names;
  
  AnalysisScalar() = default;
  constexpr AnalysisScalar(Value value_) : value { value_ } {};
  constexpr operator Value() const { return value; }
  static constexpr AnalysisScalar next(AnalysisScalar s) { return AnalysisScalar(static_cast<Value>((s.value+1) % _count)); }
  static constexpr AnalysisScalar previous(AnalysisScalar s) { return AnalysisScalar(static_cast<Value>((s.value-1+_count) % _count)); }
  static const std::string name(AnalysisScalar s) { return names[s.value]; }
private:
  Value value;
};

using scalarValuesT = std::array<float, static_cast<int>(AnalysisScalar::_count)>;

class BaseClient {
public:
  BaseClient() {};
  virtual ~BaseClient() {};
  virtual void closeStream() {};
  
  const auto& getScalarValues() { return scalarValues; };
  float getScalarValue(AnalysisScalar scalar) { return scalarValues[scalar]; };
  float* getScalarValuePtr(AnalysisScalar scalar) { return &scalarValues[scalar]; }
  auto& getMfcc() const { return mfcc; }
  
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

  scalarValuesT scalarValues;
  std::vector<float> mfcc;
  //  std::vector<float> spectrum, mel;
  
  void updateOsc(); // rename: this isn't anything to do with OSC for LocalGistClient

private:
};

} // namespace
