#include "BaseRun.h"
#include <iomanip>

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
    number+=pow(10,i-1+N1)*chToNum (buffer[iStart+N0-i]);
  }
  for (int i=1; i<=N1; i++){
    decimal+=pow(10,i-1)*chToNum(buffer[iStart+N0+N1+1-i]);
  }
  double result=number+decimal;
  double returnValue=result*pow(10,-N1);
  return returnValue;
}

long double readNumberMod(char* buffer, int iStart, int N)
{
  std::vector<int> integer;
  std::vector<int> decimal;
  bool isInt=true;
  bool isDec=false;
  for (int i=0; i<N; i++){
    if (chToNum(buffer[iStart+i])==-2) {
      isDec=true;
      isInt=false;
      continue;
    }
    if (chToNum(buffer[iStart+i])<0) continue;
    if (isInt) integer.push_back(chToNum(buffer[iStart+i]));
    if (isDec) {
      decimal.push_back(chToNum(buffer[iStart+i]));
      //      std::cout<<"testDec1: "<<chToNum(buffer[iStart+i])<<std::endl;
    }
  }
  long double number=0;
  for (int i=0; i<integer.size(); i++){
    if (integer[integer.size()-1-i]>0) number += pow(10,i)*(integer[integer.size()-1-i]);
    //   if (integer[integer.size()-1-i]==0) i++;
  }
  long double dec=0;
  for (int i=0; i<decimal.size(); i++){
    if (decimal[i]>0) dec += pow(10,-i-1)*decimal[i];
    //  if (decimal[i]==0) i++;
    //   std::cout<<"testdec2: "<<pow(10,-i-1)*decimal[i]<<std::endl;
  }
  number+=dec;
  return number;
}
/*
int convertStringParam(std::string IN, bool* OUT)
{
  int returnValue=0;
  if (IN=="ye") *OUT=true;
  else if (IN=="no") *OUT=false;
  else returnValue=1;
  return returnValue;
}
*/
/*
//OLD FUNCTION (02.08.17)
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
*/

float pulseToFloat(unsigned int pulse, float tau)
{
  float exp,spectr_t;
  
  spectr_t =  (pulse&0xFFFFFF);
  
  exp = int(pulse&0x7F000000) >> 24;
  
  exp = exp-64-24;
  
  float ratio=tau/0.2048;
  
  spectr_t=spectr_t*pow(2,exp)/ratio;
  
  spectr_t=sqrt(spectr_t*4/2048/2048);
  
  spectr_t=spectr_t*2000/2047;
  
  spectr_t=spectr_t*sqrt(2.0)/2;
  
  spectr_t=spectr_t*spectr_t;
  
  return spectr_t;
}

BaseRun::BaseRun()
{
  fTelcode="bsa1";
  fObscode="PO";
  fRtype="DPP1";
  fDatatype="I";
  fNpol=1;
  fNChannels=512;

  fPrintData=false;
}

BaseRun::~BaseRun()
{
}

int BaseRun::ReadRAWData(std::string runID, std::string rawdata_dir, std::string output_dir, bool printData, int printGranularity)
{
  fPrintData=printData;
  
  int retVal=1;
  
  fPerChannelSignal.clear();
  
  fRunID=runID;

  std::string fname = rawdata_dir+"/"+runID;
  std::ifstream data(fname.c_str(),std::ios::binary|std::ios::in);
  
  std::cout<<"чтение сеанса "<<runID<<" из директории: "<<fname<<std::endl;
   //read header
  int length = 40; 
  char * buffer = new char [length];
  int sizeHeader;

  std::cout<<"заголовочная часть:"<<std::endl;

  std::cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;

  for (int k=0; k<13; k++)
    {
      // std::cout << "Reading " << length << " characters... "<<std::endl;
      // read data as a block:
      data.read(buffer,length);
      //  int size = data.tellg();
      sizeHeader = data.tellg();
      std::cout<<k<<"   "<<buffer<<std::endl;
      
      //read period (in s)
      if (k==4)	{
	fPeriod=readNumberMod(buffer,13,15);
      }
      
      if (k==9) {
	fDM=readNumberMod(buffer,13,7);
      }
      
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

      if (k==5) fNumpuls=number;
      if (k==7) fNumpointwin=number;

      std::string sbuf;
      if (k==1) {
	for (int ibuf=13; ibuf<20; ibuf++){
	  sbuf+=buffer[ibuf];
	}
	fPsrname=sbuf;
      }
      

      /////////////////////////////
      //read cumchan
      if (k==8) {
	std::string buf;
	for (int ibuf=13; ibuf<15; ibuf++){
	  buf+=buffer[ibuf];
	}
	bool ff=true;
	if (buf=="ye") ff=true;
	else if (buf=="no") ff=false;
	fSumchan=(int)ff;
	//	std::cout<<"fSumchan: "<<fSumchan<<std::endl;
      }
	/////////////////////////////
      
      //read tau (in ms)
      if (k==6) fTau=readNumber(buffer,13,1,4);
      
      //read freq 0
      if (k==10) {
	fFreqFirst=readNumber(buffer,13,3,3);
	fWLFirst=3e10*pow(fFreqFirst*1e6,-1);
      }
      
      //read freq 511
      if (k==11) {
	fFreqLast=readNumber(buffer,13,3,3);
	fWLLast=3e10*pow(fFreqLast*1e6,-1);
      }
      
      //decode time
      if (k==12){
	fUtcday=readNumber(buffer,13,2,0);
	fUtcmonth=readNumber(buffer,16,2,0);
	fUtcyear=readNumber(buffer,19,2,0);
	fUtchour=readNumber(buffer,22,2,0);
	fUtcmin=readNumber(buffer,25,2,0);
	fUtcsec=readNumber(buffer,28,2,0)+1e-7*readNumber(buffer,31,7,0);
	//	fUtcsec=readNumber(buffer,28,2,0);
	//	fUtcnsec=readNumber(buffer,31,7,0);
      }
      if (k==2){
	fDay=readNumber(buffer,13,2,0);
	fMonth=readNumber(buffer,16,2,0);
	fYear=readNumber(buffer,19,4,0);
      }
      if (k==3){
	fHour=readNumber(buffer,13,2,0);
	fMinute=readNumber(buffer,16,2,0);
	fSec=readNumber(buffer,19,2,0)+1e-7*readNumber(buffer,23,7,0);
	//	fSecond=readNumber(buffer,19,2,0);
	//	fNsec=readNumber(buffer,23,7,0);
      }
    }   
  
  fNPoints=fNumpuls*fNumpointwin;
  if (fSumchan==1) fNPoints=fNumpointwin;
  fDuration=fNPoints*fTau;
  
  //  char tmp[100];
  //  TH1F sigTimeProfile[512];
  std::vector<float> freqResponse;
  //  std::vector<SignalContainer> perChannelSignal;
  //  std::cout<<"READING DATA    numPeriods: "<<fNumpuls<<"   binsPerPeriod: "<<fNumpointwin<<"   tau: "<<fTau<<std::endl;
  //	   <<"   tau: "<<fTau<<"   period: "<<fPeriod<<"  fDay: "<<fDay<<"  fSec: "<<fSecond<<"\n"
  //	   <<"     fre0: "<<fFreq0<<"   freq511: "<<fFreq511<<std::endl;
  // int lengthData= 8*sizeof(uint32_t);
  int lengthData = sizeof(uint32_t);
  char* fileContents;
  fileContents = new char[lengthData];
  unsigned int number=0;
  int ipos;
  int iPoint=0;
  int iPointAbs=0;
  int iFreq=0;
  int iPeriod=0;

  std::ofstream textDataStream;
  if (fPrintData) {
    char tmp[100];
    sprintf(tmp,"%s.data",runID.c_str());
    textDataStream.open(tmp);
    textDataStream<<std::setw(15)<<std::left<<"start time";     
  }
  
  for (int i=0; i<512; i++){
    fPerChannelSignal.push_back(SignalContainer(fNPoints,0,fDuration));
    if (fPrintData) textDataStream<<std::setprecision(6)<<"f="<<std::setw(13)<<std::left<<fFreqFirst+i*(-fFreqFirst+fFreqLast)/512;
  }

  if (fPrintData) textDataStream<<std::endl<<std::setprecision(6)<<std::setw(15)<<std::left<<iPointAbs*fTau;

  float avgAmpl[512]={0};

  while(data.good())
    {
      data.read((char *) &number,lengthData);
      int ipos = data.tellg();
      if (!data.good()) continue;
      float ampl=pulseToFloat(number,fTau);
      int iFreq=(((ipos-sizeHeader)/lengthData-1)%512);
      if (iFreq!=513) {
	//      std::cout<<iFreq<<"    "<<iPointAbs<<"    "<<ampl<<
	fPerChannelSignal[iFreq].SetSignal(iPointAbs,ampl);
	avgAmpl[iFreq]+=ampl;
	if ((iPointAbs+1)%printGranularity==0) {
	  textDataStream<<std::setprecision(6)<<std::setw(15)<<std::left<<avgAmpl[iFreq]/printGranularity;
	  avgAmpl[iFreq]=0;
	  
	}
	//	std::cout<<iFreq<<"    "<<iPointAbs<<"    "<<ampl<<"   "<<number<<"    "<<fPerChannelSignal[iFreq].GetSignal(iPointAbs)<<std::endl;
      }
      if (iFreq==511){
	iPoint++;
	if (iPoint%fNumpointwin==0) {
	  iPoint=0;
	  iPeriod++;
	}
	if (fPrintData&&(iPointAbs+1)%printGranularity==0) textDataStream<<std::endl<<std::setprecision(6)<<std::setw(15)<<std::left<<iPointAbs*fTau;
	iPointAbs++;
      }	
    }
  //  for (int i=0; i<fNPoints; i++){
    //  std::cout<<"test: "<<i<<"  "<<fPerChannelSignal[18].GetSignal(i)<<std::endl;
  //  }
  
  for (int i=0; i<512; i++){
    fFreqResponse.push_back(fPerChannelSignal[i].GetSignalMean(0,1000000));
    //    std::cout<<i<<"     "<<fPerChannelSignal[i].GetSignalMean(0,1000000)<<std::endl;
    fFreqResponseMedian.push_back(fPerChannelSignal[i].GetSignalMedian(0,1000000));
  }

  //  for (int i=0; i<512; i++){
    //    fFreqResponse.push_back(fPerChannelSignal[i].GetSignalMean(0,1000000));
    //    for (int j=0; j<fNPoints; j++){
      //std::cout<<i<<"    "<<j<<"     "<<fPerChannelSignal[i].GetSignal(j)<<"    "<<pow(fFreqResponse[i],-1)<<"    "<<fPerChannelSignal[i].GetSignal(j)*pow(fFreqResponse[i],-1)<<"    "<<fFreqResponseMedian[i]<<std::endl;
      //fPerChannelSignal[i].SetSignal(j,fPerChannelSignal[i].GetSignal(j)
				     //*pow(fFreqResponse[i],-1));
      //fPerChannelSignal[i].SetSignal(j,fPerChannelSignal[i].GetSignal(j)-fFreqResponseMedian[i]);
    //    }
  //  }
  
  data.close();
  delete fileContents;
  delete buffer;
  std::cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;
  return retVal;

}
  
