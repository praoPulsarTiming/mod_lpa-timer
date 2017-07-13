#include <iostream>
#include <cmath>
#include <algorithm>
#include "SignalContainer.h"

SignalContainer::SignalContainer()
{
  fNTimeBins=0;
  fTimeStep=0;
}

SignalContainer::~SignalContainer()
{
}


SignalContainer::SignalContainer(std::vector<float> timeValues)
{
  fNTimeBins=timeValues.size();
  fTimeStep=(float)(timeValues[timeValues.size()-1]-timeValues[0])/fNTimeBins;
  fTimeVector=timeValues;
  for (int j=0; j<fNTimeBins; j++){
    fSignalVector.push_back(0);
  }
}

SignalContainer::SignalContainer(float nbins, float time0, float time1)
{
  fNTimeBins=nbins;
  fTimeStep=(float)(time0-time1)/fNTimeBins;
  for (int j=0; j<fNTimeBins; j++){
    fTimeVector.push_back(time0+j*fTimeStep);
  }
  for (int j=0; j<fNTimeBins; j++){
    fSignalVector.push_back(0);
  }
}

SignalContainer::SignalContainer(std::vector<float> timeValues, std::vector<float> signalValues)
{
  fNTimeBins=timeValues.size();
  fTimeStep=(float)(timeValues[timeValues.size()-1]-timeValues[0])/fNTimeBins;
  fTimeVector=timeValues;
  
  //  fNTimeBins=timeValues.size();
    //  fTimeStep=(float)(timeValues[timeValues.size()-1]-timeValues[0])/fNTimeBins;
    //  fTimeVector=timeValues;
  if (timeValues.size()!=signalValues.size()){
    std::cout<<"WARNING: time vector is not of the same size as signal vector"<<std::endl;
  }
  fSignalVector=signalValues;
}

SignalContainer::SignalContainer(std::vector<float> timeValues, std::vector<float> signalValues, std::vector<float> errors)
{
  SignalContainer(timeValues, signalValues);
  fErrorVector=errors;
}

float SignalContainer::GetSignalMean(int b0, int b1)
{
  float mean=0;
  if (fSignalVector.size()==0) return mean;
  if (b1>=fSignalVector.size()) b1=fSignalVector.size()-1;
  for (int i=b0; i<=b1; i++){
    mean+=fSignalVector[i];
  }
  mean=(float)mean/(float)(b1-b0+1);
  return mean;
}

float SignalContainer::GetSignalMedian(int b0, int b1)
{
  float med=0;
  if (fSignalVector.size()==0) return med;
  if (b1>=fSignalVector.size()) b1=fSignalVector.size()-1;
  std::vector<float> sorted;
  for (int i=b0; i<=b1; i++){
    sorted.push_back(fSignalVector[i]);
  }
  std::sort(sorted.begin(), sorted.end());
  if (sorted.size()%2==0){
    med=(sorted[floor(sorted.size()/2)]+sorted[floor(sorted.size()/2)-1])/2;
  }
  else med=(sorted[floor(sorted.size()/2)]);
  
  return med;
}


float SignalContainer::GetSignalVariance(int b0, int b1)
{
  float rms=0;
  if (fSignalVector.size()==0) return rms;
  float mean=GetSignalMean(0,1e8);
  for (int i=b0; i<=b1; i++){
    rms+=pow(mean-fSignalVector[i],2);
  }
  rms=(float)rms/(float)(b1-b0+1);
  return rms;
}

float SignalContainer::GetTime(int iBin)
{
  if (fTimeVector.size()==0) return 0;
  else if (fTimeVector.size()<=iBin) return fTimeVector[fTimeVector.size()];
  else {
    return fTimeVector[iBin];
  }
}

float SignalContainer::GetSignal(int iBin)
{
  //std::cout<<"get signal: "<<fSignalVector[iBin]<<std::endl;
  if (fSignalVector.size()==0) return 0;
  else if (fSignalVector.size()<=iBin) return fSignalVector[fSignalVector.size()];
  else {
    return fSignalVector[iBin];
  }
}

int SignalContainer::SetSignal(int iBin, float value)
{
  if (fSignalVector.size()<=iBin) return 0;
  else {
    fSignalVector[iBin]=value;
    return 1;
  }
}

float SignalContainer::GetSignalError(int iBin)
{
  if (fErrorVector.size()==0) return 0;
  else if (fErrorVector.size()<=iBin) return fErrorVector[fErrorVector.size()];
  else {
    return fErrorVector[iBin];
  }
}

int SignalContainer::SetSignalError(int iBin, float value)
{
  if (fErrorVector.size()<=iBin) return 0;
  else {
    fErrorVector[iBin]=value;
    return 1;
  }
}


