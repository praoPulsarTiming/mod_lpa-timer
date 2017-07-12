#include "BaseRun.h"

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

BaseRun::BaseRun()
{
  fTelcode="bsa1";
  fObscode="PO";
  fRtype="DPP1";
  fDatatype="I";
  fNpol=1;
  fNBands=512;
}

BaseRun::~BaseRun()
{
}

int BaseRun::ReadRAWData(std::string runID, std::string rawdata_dir, std::string output_dir)
{
  fRunID=runID;

  std::string fname = rawdata_dir+"/"+runID;
  std::ifstream data(fname.c_str(),std::ios::binary|std::ios::in);
  
  std::cout<<"READING RAW RUN "<<runID<<"    path: "<<fname<<std::endl;

  //read header
  int length = 40; 
  char * buffer = new char [length];
  int sizeHeader;

  //      std::cout<<"READING HEADER"<<std::endl;
  for (int k=0; k<13; k++)
    {
      // std::cout << "Reading " << length << " characters... "<<std::endl;
      // read data as a block:
      data.read(buffer,length);
      //  int size = data.tellg();
      sizeHeader = data.tellg();
      std::cout<<k<<"   "<<buffer<<std::endl;
      //     for (int q=0; q<length; q++)
      //	{
	  //	  std::cout<<q<<":"<<buffer[q]<<" ";
      //	}
      
      //read period (in ms)
      if (k==4)	fPeriod=1000*readNumber(buffer,13,1,11);

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

      if (k==1) {
	for (int ibuf=13; ibuf<19; ibuf++){
	  fPsrname+=buffer[ibuf];
	}
      }
      
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
	fUtcsec=readNumber(buffer,28,2,0);
	fUtcnsec=readNumber(buffer,31,7,0);
      }
      if (k==2){
	fDay=readNumber(buffer,13,2,0);
	fMonth=readNumber(buffer,16,2,0);
	fYear=readNumber(buffer,19,2,0);
      }
      if (k==3){
	fHour=readNumber(buffer,13,2,0);
	fMinute=readNumber(buffer,16,2,0);
	fSecond=readNumber(buffer,19,2,0);
	fNsec=readNumber(buffer,23,7,0);
      }
    }   

  fNPoints=fNumpuls*fNumpointwin;
  fDuration=fNPoints*fTau;
  
  //  char tmp[100];
  //  TH1F sigTimeProfile[512];
  std::vector<float> freqResponse;
  //  std::vector<SignalContainer> perBandSignal;
  std::cout<<"READING DATA    numPeriods: "<<fNumpuls<<"   binsPerPeriod: "<<fNumpointwin<<"   tau: "<<fTau<<std::endl;
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

    for (int i=0; i<512; i++){
    fPerBandSignal.push_back(SignalContainer(fNPoints,0,fDuration));
  }
  while(data.good())
    {
      data.read((char *) &number,lengthData);
      int ipos = data.tellg();
      if (!data.good()) continue;
      float ampl=pulseToFloat(number,fTau);
      int iFreq=(((ipos-sizeHeader)/lengthData-1)%512);
      if (iFreq!=513) {
	fPerBandSignal[iFreq].SetSignal(iPoint,ampl);
      }
      if (iFreq==511){
	iPoint++;
	if (iPoint%fNumpointwin==0) {
	  iPoint=0;
	  iPeriod++;
	}	      
	iPointAbs++;
      }	
    }

  
  for (int i=0; i<512; i++){
    fFreqResponse.push_back(fPerBandSignal[i].GetSignalMean(0,1000000));
  }
  
  data.close();
  delete fileContents;
  delete buffer;
  std::ofstream freqResInfo;
  char tmp[100];
  sprintf(tmp,"fftOutput_%s.dat",runID.c_str());
  freqResInfo.open(tmp);
  freqResInfo<<"period: "<<fPeriod<<"\n";
  freqResInfo<<"tau: "<<fTau<<"\n";
  freqResInfo<<"date: "<<fDay<<"/"<<fMonth<<"/"<<fYear<<"\n";
  freqResInfo<<"time: "<<fHour<<":"<<fMinute<<":"<<fSecond<<"\n";
  freqResInfo<<"frequence response: "<<"\n";
  //  freqResInfo<<"fast fourier image [bin center (Hz)     power]:"<<"\n";
  for (int i=0; i<512; i++){
    float freq=fFreqFirst+(fFreqLast-fFreqLast)/512;
    freqResInfo<<i<<"      "<<freq<<"      "<<fFreqResponse[i]<<std::endl;
  }
  freqResInfo.close();
  std::cout<<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<std::endl;
  return 0;

}
  
