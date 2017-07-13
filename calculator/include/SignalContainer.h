#include <vector>
//replacement of a histogram

class SignalContainer
{
 public:
  SignalContainer();
  SignalContainer(std::vector<float> timeValues);
  SignalContainer(float, float, float);
  SignalContainer(std::vector<float> timeValues, std::vector<float> signalValues);
  SignalContainer(std::vector<float> timeValues, std::vector<float> signalValues, std::vector<float> errors);
  ~SignalContainer();

  float GetSignalMean(int t0, int t1);
  float GetSignalMedian(int t0, int t1);
  float GetSignalVariance(int t0, int t1);

  float GetTime(int iBin);
  float GetSignal(int iBin);
  
  int SetSignal(int iBin, float value);
  
  float GetSignalError(int iBin);
  int SetSignalError(int iBin, float value);
  
 private:
  int fNTimeBins;
  float fTimeStep;
  // float
  std::vector<float> fTimeVector;
  std::vector<float> fSignalVector;
  std::vector<float> fErrorVector;
  //  float getVectorMean();
  //  float getVectorVariance();
};
