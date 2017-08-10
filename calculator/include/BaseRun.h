#include <iostream>
#include <fstream> 
#include <stdint.h>
#include <cmath>
#include <bitset>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "SignalContainer.h"

//базовый класс для хранения данных о сеансе наблюдения пульсара

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
  double GetUtcSecond() {return fUtcsec;}
  int GetDay() {return fDay;}
  int GetMonth() {return fMonth;}
  int GetYear() {return fYear;}
  int GetHour() {return fHour;}
  int GetMinute() {return fMinute;}
  double GetSecond() {return fSec;}
  
  double GetPeriod() {return fPeriod;}
  int GetNumpuls() {return fNumpuls;}
  int GetNumpointwin() {return fNumpointwin;}
  int GetNChannels() {return fNChannels;}
  float GetFreqFirst() {return fFreqFirst;}
  float GetFreqLast() {return fFreqLast;}
  float GetWLFirst() {return fWLFirst;}
  float GetWLLast() {return fWLLast;}
  float GetTau() {return fTau;}
  float GetDM() {return fDM;}

  int GetNPoints() {return fNPoints;}
  float GetDuration() {return fDuration;}

  SignalContainer* GetChannelSignal(int iband) {return &fPerChannelSignal[iband];}
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
  long double fUtcsec;

  long double fPeriod;   //период пульсара
  int fNumpuls;          //число наблюденных импульсов
  int fNumpointwin;      //число измерений сигнала на импульс
  ////////////////
  
  int fNChannels;       //число каналов
  float fFreqFirst;  //низкая частота MHz
  float fFreqLast;   //высокая частота MHz
  float fWLFirst;    //большая длина волны m
  float fWLLast;     //маленькая длина волны m
  float fTau;        //время интегрирования сигнала s
  float fNPoints;    //число измерений сигнала в сеансе
  float fDuration;   //продолжительность сеанса
  //start time: 
  int fDay; 
  int fMonth;
  int fYear;
  int fHour;
  int fMinute;
  long double fSec;
  //  int fSecond;
  //  int fNsec;
  float fDM;

  std::string fRunID;
  
  std::vector<SignalContainer> fPerChannelSignal;         // хранилище первичного сигнала
  std::vector<float> fFreqResponse;			  // АЧХ
  std::vector<float> fFreqResponseMedian;
};
