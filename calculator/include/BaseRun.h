#include <iostream>
#include <fstream> 
#include <stdint.h>
#include <cmath>
#include <bitset>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "SignalContainer.h"

class BaseRun
{
  
 public:
  BaseRun();
  ~BaseRun();
  
  int ReadRAWData(std::string runID, std::string rawdata_dir, std::string output_dir);

  std::string GetRunID() {return fRunID;}
  
  //getters:
  std::string GetTelcode() {return fTelcode;}
  std::string GetObscode() {return fObscode;}
  std::string GetRtype() {return fRtype;}
  std::string GetPsrname() {return fPsrname;}
  std::string GetDatatype() {return fDatatype;}
  int GetNpol() {return fNpol;}
  
  int GetSumchan() {return fSumchan;}
  
  int GetUtcDay() {return fUtcday;}
  int GetUtcMonth() {return fUtcmonth;}
  int GetUtcYear() {return fUtcyear;}
  int GetUtcHour() {return fUtchour;}
  int GetUtcMinute() {return fUtcmin;}
  float GetUtcSecond() {return fUtcsec;}
  int GetDay() {return fDay;}
  int GetMonth() {return fMonth;}
  int GetYear() {return fYear;}
  int GetHour() {return fHour;}
  int GetMinute() {return fMinute;}
  float GetSecond() {return fSec;}
  
  float GetPeriod() {return fPeriod;}
  int GetNumpuls() {return fNumpuls;}
  int GetNumpointwin() {return fNumpointwin;}
  int GetNBands() {return fNBands;}
  float GetFreqFirst() {return fFreqFirst;}
  float GetFreqLast() {return fFreqLast;}
  float GetWLFirst() {return fWLFirst;}
  float GetWLLast() {return fWLLast;}
  float GetTau() {return fTau;}
  float GetDM() {return fDM;}

  int GetNPoints() {return fNPoints;}
  float GetDuration() {return fDuration;}

  SignalContainer* GetBandSignal(int iband) {return &fPerBandSignal[iband];}
  float GetFreqResponse(int iband) {return fFreqResponse[iband];} 
  
 private:
  std::string fTelcode;
  std::string fObscode;
  std::string fRtype;
  std::string fPsrname;
  std::string fDatatype;
  int fNpol;
  int fSumchan;

  int fUtcday;
  int fUtcmonth;
  int fUtcyear;
  int fUtchour;
  int fUtcmin;
  //  int fUtcsec;
  //  int fUtcnsec;
  float fUtcsec;

  //temporary, it should be in the upper level run class
  float fPeriod;
  int fNumpuls;
  int fNumpointwin;
  ////////////////
  
  int fNBands;       //number of bands
  float fFreqFirst;  //lowest frequency MHz
  float fFreqLast;   //highest frequency MHz
  float fWLFirst;    //longest wavelength m
  float fWLLast;     //shortest wavelength m
  float fTau;        //integration time s
  float fNPoints;    //number of measurements in the run
  float fDuration;   //duration in seconds
  //start time: 
  int fDay;
  int fMonth;
  int fYear;
  int fHour;
  int fMinute;
  float fSec;
  //  int fSecond;
  //  int fNsec;
  float fDM;

  std::string fRunID;
  
  std::vector<SignalContainer> fPerBandSignal;
  std::vector<float> fFreqResponse;
  std::vector<float> fFreqResponseMedian;
};
