#include "PulseExtractor.h"
#include <stdlib.h>
//add frequency weights resulting from frequency response
//add mask reading
//add mask calculation from frequency response


PulseExtractor::PulseExtractor()
{
}

PulseExtractor::~PulseExtractor()
{
}

PulseExtractor::PulseExtractor(BaseRun* run, float dm)
{
  fBaseRun=run;
  fDM=dm;

  fillSumProfile();

  fCompensatedSignal=SignalContainer(fBaseRun->GetNumpointwin()*fBaseRun->GetNumpuls(),0,fBaseRun->GetNumpointwin()*fBaseRun->GetNumpuls()+1);
  fCompensatedSignalSum=SignalContainer(fBaseRun->GetNumpointwin(),0,fBaseRun->GetNumpointwin()+1);
 
  for (int i=0; i<fBaseRun->GetNBands(); i++) {fBandMask.push_back(1);}
}


int PulseExtractor::compensateDM()
{
  std::cout<<"do compensation"<<std::endl;
  float dm=fDM;
  //define which bins current thread would sum
  //  std::cout<<fTau<<"  "<<fPeriod<<" "<<fNPeriods<<" "<<fNThreads<<"   start: "<<startPeriod<<"  "<<endPeriod<<std::endl;
  for (int i=0; i<fBaseRun->GetNPoints(); i++){
    //(i<=fNBinsPerPeriod)||i>fNBins-fNBinsPerPeriod) continue;
    
    //for (int i=1; i<fNBins+1; i++){
    float fDnu=fabs(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/512;
    float fDL=fabs(fBaseRun->GetWLLast()-fBaseRun->GetWLFirst())/512;
    float bico=0;
    for (int y=0; y<fBaseRun->GetNBands(); y++) {
      if (fBandMask[y]==0) continue;
      if (fSumProfile.freq==-1) {
	fSumProfile.freq=fBaseRun->GetFreqFirst()+fDnu*y;
      }
      //take sigTimeProfile[511-y] as 511-th is shorter wavelength
      //calculate delay wrt 511th for particular freq[511-y]
      float dT=4.6*(-pow(fBaseRun->GetWLLast(),2)+pow(fBaseRun->GetWLLast()+y*fDL,2))*fDM*0.001; //covert to ms
      //calculate residual difference to nearest positive side pulse
      float dTnearest=dT-fBaseRun->GetPeriod()*floor(dT*pow(fBaseRun->GetPeriod(),-1));
      //move frequency band by -dTnearest bins, add lower bins to "upper side"
      float delta=dTnearest*pow(fBaseRun->GetTau(),-1);
      //float bico1=fSigArray[((fNFreq-1)-y)*fNBinsInput+int(floor(i+delta))];
      //float bico2=fSigArray[((fNFreq-1)-y)*fNBinsInput+int(floor(i+delta)+1)];
      //float bico1=sigArray[((511-y)*nBinsInput+int(floor(i+delta)))%(nBinsInput*nFreq)];
      //float bico2=sigArray[((511-y)*nBinsInput+int((floor(i+delta)+1)))%(nBinsInput*nFreq)];

      //add normalisation here
      float bandMean=fBaseRun->GetFreqResponse(y);

      float normFactor;
      if (bandMean==0) normFactor=1;
      else normFactor=pow(bandMean,-1);

  
      float bico1=normFactor*fBaseRun->GetBandSignal((fBaseRun->GetNBands()-1)-y)->GetSignal(int(floor(i+delta))%fBaseRun->GetNPoints());
      float bico2=normFactor*fBaseRun->GetBandSignal((fBaseRun->GetNBands()-1)-y)->GetSignal(int((floor(i+delta)+1))%fBaseRun->GetNPoints());
      float lowerBinFrac=1-((i+delta)-floor(i+delta));
      float upperBinFrac=1-lowerBinFrac;
      if (floor(i+delta)+1<fBaseRun->GetNPoints()) bico+=lowerBinFrac*bico1+upperBinFrac*bico2;
    }

    fCompensatedSignal.SetSignal(i-1,bico);
    //    std::cout<<"thread: "<<iThread<<"   bin index: "<<i-1<<"   value: "<<bico<<std::endl;
    //    if (bico==bico) fHCompSig[iStep]->Fill(i-1,bico);
  }
    return 0;
}

int PulseExtractor::fillSumProfile()
{
  fSumProfile.telcode = fBaseRun->GetTelcode();
  fSumProfile.obscode = fBaseRun->GetObscode();
  fSumProfile.rtype = fBaseRun->GetRtype();
  fSumProfile.psrname = fBaseRun->GetPsrname();
  fSumProfile.datatype = fBaseRun->GetDatatype();
  fSumProfile.npol = fBaseRun->GetNpol();

  fSumProfile.year = fBaseRun->GetYear();
  fSumProfile.month = fBaseRun->GetMonth();
  fSumProfile.day = fBaseRun->GetDay();
  fSumProfile.hour = fBaseRun->GetHour();
  fSumProfile.min = fBaseRun->GetMinute();
  fSumProfile.sec = fBaseRun->GetSecond();
  fSumProfile.nsec = fBaseRun->GetNsec();
  fSumProfile.utcyear = fBaseRun->GetUtcYear();
  fSumProfile.utcmonth = fBaseRun->GetUtcMonth();
  fSumProfile.utcday = fBaseRun->GetUtcDay();
  fSumProfile.utchour = fBaseRun->GetUtcHour();
  fSumProfile.utcmin = fBaseRun->GetUtcMinute();
  fSumProfile.utcsec = fBaseRun->GetUtcSecond();
  fSumProfile.utcnsec = fBaseRun->GetUtcNsec();

  fSumProfile.period = fBaseRun->GetPeriod();
  fSumProfile.numpuls = fBaseRun->GetNumpuls();
  fSumProfile.tau = fBaseRun->GetTau();
  fSumProfile.numpointwin = fBaseRun->GetNumpointwin();

  fSumProfile.freq = -1;

  for (int i=0; i<fSumProfile.numpointwin; i++) fSumProfile.prfdata.push_back(0);
}

int PulseExtractor::ReadMask(std::string fname)
{
  std::cout<<"read mask"<<std::endl;
  std::ifstream fmask;
  fmask.open(fname.c_str());
  char tmp[100];
  int iband;
  float value;
  for (int i=0; i<512; i++){
    fmask>>iband>>value;
    //std::cout<<iband<<std::endl;
    fBandMask[iband-1]=value;
  }
}

int PulseExtractor::sumPeriods()
{
  std::cout<<"sum periods"<<std::endl;
  for (int i=0; i<fBaseRun->GetNumpuls(); i++){
    for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
      fSumProfile.prfdata[j]+=fCompensatedSignal.GetSignal(i*fBaseRun->GetNumpointwin()+j);
      fCompensatedSignalSum.SetSignal(j,fSumProfile.prfdata[j]);
    }
  }
}

int PulseExtractor::PrintSumProfile(std::string filename)
{
  std::cout<<"print profile"<<std::endl;
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"sumProfile_%s.dat",fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  sumProfileStream<<"### HEADER "<<"\n";
  sumProfileStream<<"telcode: "<<fSumProfile.telcode<<"\n";
  sumProfileStream<<"obscode: "<<fSumProfile.obscode<<"\n";
  sumProfileStream<<"rtype: "<<fSumProfile.rtype<<"\n";
  sumProfileStream<<"psrname: "<<fSumProfile.psrname<<"\n";
  sumProfileStream<<"period: "<<fSumProfile.period<<"\n";
  sumProfileStream<<"tau: "<<fSumProfile.tau<<"\n";
  sumProfileStream<<"date: "<<fSumProfile.day<<"/"<<fSumProfile.month<<"/"<<fSumProfile.year<<"\n";
  sumProfileStream<<"time: "<<fSumProfile.hour<<":"<<fSumProfile.min<<":"<<fSumProfile.sec<<"\n";
  sumProfileStream<<"frequency: "<<fSumProfile.freq<<"\n";

  
  sumProfileStream<<"### COMPENSATED SUM PROFILE "<<"\n";
  sumProfileStream<<"time        signal"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<time<<"     "<<fSumProfile.prfdata[i]<<"\n";
  }
  return 1;
}

int PulseExtractor::DoCompensation()
{
  compensateDM();
  return 1;
}

int PulseExtractor::SumPeriods()
{
  sumPeriods();
  return 1;
}

