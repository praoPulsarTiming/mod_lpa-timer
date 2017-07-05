#include <iostream>
#include <fstream> 
#include <stdint.h>
#include <cmath>
#include <bitset>
#include <vector>
#include <stdio.h>
#include <string.h>
//#include "TH1F.h"
//#include "TFile.h"
//#include "TH2F.h"
//#include "TTree.h"
//#include "TRandom.h"
//#include "TProfile.h"
//#include "TProfile2D.h"

struct runData{
  int fNPeriods;
  int fNBins;
  int fNBinsPerPeriod;
  float fTau;
  float fFreq0;
  float fFreq511;
  float fPeriod;
  int fDay;
  int fMonth;
  int fYear;
  int fHour;
  int fMinute;
  int fSecond;

  std::vector<float> fFreqResponse;
  std::vector<float> fPerBandSignal[512];
};

int chToNum(char ch)
{
  int num=int(ch)-int('0');
  return num;
}

double readNumber(char* buffer, int iStart, int N0, int N1)
{
  double number=0;
  double decimal=0;

  for (int i=1; i<=N0; i++){
    number+=pow(10,i-1+N1)*chToNum(buffer[iStart+N0-i]);
  }
  for (int i=1; i<=N1; i++){
    decimal+=pow(10,i-1)*chToNum(buffer[iStart+N0+N1+1-i]);
  }
  double result=number+decimal;
  double returnValue=result*pow(10,-N1);
  return returnValue;
}


float pulseToFloat(unsigned int pulse, float tau)
{
  float exp, spectr;
  spectr=(pulse&0xFFFFFF);
  exp=int(pulse&0x7F000000) >> 24;
  exp=exp-64-24;
  float ratio=tau/0.2048;
  spectr=spectr*std::pow(2,exp)/ratio;
  return spectr;
}

float getVectorMean(std::vector<float> bandSignal)
{
  float mean=0;
  if (bandSignal.size()==0) return mean;
  for (int i=0; i<bandSignal.size(); i++){
    mean+=bandSignal[i];
  }
  mean=(float)mean/(float)bandSignal.size();
  return mean;
}

float getVectorVariance(std::vector<float> bandSignal)
{
  float rms=0;
  if (bandSignal.size()==0) return rms;
  float mean=getVectorMean(bandSignal);
  for (int i=0; i<bandSignal.size(); i++){
    rms+=pow(mean-bandSignal[i],2);
  }
  rms=(float)rms/(float)bandSignal.size();
  return rms;
}

int readRAW(std::string runID, std::string rawdata_dir, std::string output_dir)
{
  std::string fname = rawdata_dir+"/"+runID;
  std::ifstream data(fname.c_str(),std::ios::binary|std::ios::in);
  
  std::cout<<"READING RAW RUN "<<runID<<"    path: "<<fname<<std::endl;

  //read header
  int length = 40; 
  char * buffer = new char [length];
  int sizeHeader;

  runData run;
  /*
  int fNBinsPerPeriod, fNPeriods;
  int fYear, fMonth, fDay, fHour, fMinute;
  double fTau, fFreq0, fFreq511;
  double fPeriod, fSecond;
  */
  //      std::cout<<"READING HEADER"<<std::endl;
  for (int k=0; k<13; k++)
    {
      // std::cout << "Reading " << length << " characters... "<<std::endl;
      // read data as a block:
      data.read(buffer,length);
      //  int size = data.tellg();
      sizeHeader = data.tellg();
      std::cout<<k<<"   "<<buffer<<std::endl;
      for (int q=0; q<length; q++)
	{
	  //	  std::cout<<q<<":"<<buffer[q]<<" ";
	}
      
      //read period (in ms)
      if (k==4)	run.fPeriod=1000*readNumber(buffer,13,1,11);

      //covert buffer to a number
      //read nPeriods and nBinsPerPeriod
      int number;
      if (buffer[17]==' ') {
	//number=1000*chToNum(buffer[13])+100*chToNum(buffer[14])+10*chToNum(buffer[15])+chToNum(buffer[16]);
	number=readNumber(buffer,13,4,0);
      }
      if (buffer[16]==' ') {
	number=readNumber(buffer,13,3,0);
      }
      if (buffer[15]==' ') {
	number=readNumber(buffer,13,2,0);
      }
      
      if (k==5) run.fNPeriods=number;
      if (k==7) run.fNBinsPerPeriod=number;
      
      //read tau (in ms)
      if (k==6) run.fTau=readNumber(buffer,13,1,4);
      
      //read freq 0
      if (k==10) run.fFreq0=readNumber(buffer,13,3,3);

      //read freq 511
      if (k==11) run.fFreq511=readNumber(buffer,13,3,3);

      //decode time
      if (k==12){
	run.fDay=readNumber(buffer,13,2,0);
	run.fMonth=readNumber(buffer,16,2,0);
	run.fYear=readNumber(buffer,19,2,0);
	run.fHour=readNumber(buffer,22,2,0);
	run.fMinute=readNumber(buffer,25,2,0);
	run.fSecond=readNumber(buffer,28,2,7);
      }
    }   

  run.fNBins=run.fNPeriods*run.fNBinsPerPeriod;
  
  //  char tmp[100];
  //  TH1F sigTimeProfile[512];
  std::vector<float> freqResponse;
  std::vector<float> perBandSignal[512];
  std::cout<<"READING DATA    numPeriods: "<<run.fNPeriods<<"   binsPerPeriod: "<<run.fNBinsPerPeriod<<"   tau: "<<run.fTau<<std::endl;
   //	   <<"   tau: "<<fTau<<"   period: "<<fPeriod<<"  fDay: "<<fDay<<"  fSec: "<<fSecond<<"\n"
  //	   <<"     fre0: "<<fFreq0<<"   freq511: "<<fFreq511<<std::endl;
  // int lengthData= 8*sizeof(uint32_t);
  int lengthData = sizeof(uint32_t);
  // lengthData=4;
  char* fileContents;
  fileContents = new char[lengthData];
  //      std::cout<<"length: "<<lengthData<<std::endl;
  unsigned int number=0;
  int ipos;
  int iPoint=0;
  int iPointAbs=0;
  int iFreq=0;
  int iPeriod=0;
  while(data.good())
    {
      data.read((char *) &number,lengthData);
      int ipos = data.tellg();
      if (!data.good()) continue;
      float ampl=pulseToFloat(number,run.fTau);
      int iFreq=(((ipos-sizeHeader)/lengthData-1)%512);
      if (iFreq!=513) {
	run.fPerBandSignal[iFreq].push_back(ampl);
      }
      if (iFreq==511){
	iPoint++;
	if (iPoint%run.fNBinsPerPeriod==0) {
	  iPoint=0;
	  iPeriod++;
	}	      
	iPointAbs++;
      }	
    }
  
  for (int i=0; i<512; i++){
    run.fFreqResponse.push_back(getVectorMean(run.fPerBandSignal[i]));
  }
  
  
  data.close();
  delete fileContents;
  delete buffer;
  std::ofstream freqResInfo;
  char tmp[100];
  sprintf(tmp,"fftOutput_%s.dat",runID.c_str());
  freqResInfo.open(tmp);
  freqResInfo<<"period: "<<run.fPeriod<<"\n";
  freqResInfo<<"tau: "<<run.fTau<<"\n";
  freqResInfo<<"date: "<<run.fDay<<"/"<<run.fMonth<<"/"<<run.fYear<<"\n";
  freqResInfo<<"time: "<<run.fHour<<":"<<run.fMinute<<":"<<run.fSecond<<"\n";
  freqResInfo<<"frequence response: "<<"\n";
  //  freqResInfo<<"fast fourier image [bin center (Hz)     power]:"<<"\n";
  for (int i=0; i<512; i++){
    float freq=run.fFreq0+(run.fFreq511-run.fFreq0)/512;
    freqResInfo<<i<<"      "<<freq<<"      "<<run.fFreqResponse[i]<<std::endl;   
  }
  freqResInfo.close();
  std::cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;
  return 0;
}

struct readRunID{
  std::string runID;
  std::string rawdata_dir;
  std::string output_dir;
};

int main(int argc, char *argv[])
{
  //read input with options
  if (argc > 3) {
    std::cout<<"too many params"<<std::endl;
    return 1;
  }
  
  std::string configName;

  if (argc==3){
    if (strcmp("-f",argv[1])==0) configName=std::string(argv[2]);
    else {
      std::cout<<argv[1]<<": wrong parameter option, only -f option is understood"<<std::endl;
      return 1;
    }
  }
  if (argc==2){
    std::cout<<argv[1]<<": do not understand this parameter"<<std::endl;
    return 1;
  }
  
  if (argc==1){
    configName="config/mainConfig.cff";
  }
  
  std::ifstream flist;
  flist.open(configName.c_str());
  char tmp[100];
  int nFiles, nThreads;
  int startFileNumber, endFileNumber;
  std::string confParam;
  std::string rawdata_dir, output_dir;
  //  std::string fname;
  std::vector<std::string> runID;
  flist>>confParam;
  while(confParam!="runs:"){		 
    std::cout<<confParam<<std::endl;
    if (confParam=="inputDir") flist>>rawdata_dir;
    else if (confParam=="outputDir") flist>>output_dir;
    else if (confParam=="startFileNumber") flist>>startFileNumber;
    else if (confParam=="nRuns") flist>>nFiles;
    flist.getline(tmp,100,'\n');
    flist>>confParam;
  }
  std::string rID;
  int runCounter=0;
  while (flist>>rID){
    runCounter++;
    flist.getline(tmp,100,'\n');
    std::cout<<runCounter<<std::endl;
    if (runCounter<startFileNumber) continue;
    if (runCounter>=startFileNumber+nFiles) break;
    runID.push_back(rID);
  }
  
  for (int iPack=0; iPack<floor(nFiles); iPack++){
    readRAW(runID[iPack], rawdata_dir, output_dir);
  }
  return 0;
}


