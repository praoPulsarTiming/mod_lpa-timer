#include "PulseExtractor.h"
#include <iomanip>
//add frequency weights resulting from frequency response
//add mask reading
//add mask calculation from frequency response


PulseExtractor::PulseExtractor()
{
}

PulseExtractor::~PulseExtractor()
{
}

PulseExtractor::PulseExtractor(BaseRun* run)
{
  fBaseRun=run;
  fDM=fBaseRun->GetDM();

  fillSumProfile();

  fCompensatedSignal=SignalContainer(fBaseRun->GetNumpointwin()*fBaseRun->GetNumpuls(),0,fBaseRun->GetNumpointwin()*fBaseRun->GetNumpuls()+1);
  fCompensatedSignalSum=SignalContainer(fBaseRun->GetNumpointwin(),0,fBaseRun->GetNumpointwin()+1);

  for (int i=0; i<fBaseRun->GetNBands(); i++)
    fCompensatedSignalBandSum.push_back(SignalContainer(fBaseRun->GetNumpointwin(),0,fBaseRun->GetNumpointwin()+1));
  
  for (int i=0; i<fBaseRun->GetNBands(); i++) {fBandMask.push_back(1);}
  fIsSumPerBandAvailable=false;
}


int PulseExtractor::compensateDM()
{
  std::cout<<"do compensation"<<std::endl;
  float dm=fDM;
  //define which bins current thread would sum
  //  std::cout<<fTau<<"  "<<fPeriod<<" "<<fNPeriods<<" "<<fNThreads<<"   start: "<<startPeriod<<"  "<<endPeriod<<std::endl;
  int npoints;
  if (!fIsSumPerBandAvailable) npoints=fBaseRun->GetNPoints();
  else npoints=fBaseRun->GetNumpointwin();

  for (int i=0; i<npoints; i++){
    //(i<=fNBinsPerPeriod)||i>fNBins-fNBinsPerPeriod) continue;
    
    //for (int i=1; i<fNBins+1; i++){
    float fDnu=fabs(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/512;
    float fDL=fabs(fBaseRun->GetWLLast()-fBaseRun->GetWLFirst())/512;
    float bico=0;
    for (int y=0; y<fBaseRun->GetNBands(); y++) {
      if (fBandMask[y]==0) continue;
      //if (fSumProfile.freq==-1) {
      //fSumProfile.freq=fBaseRun->GetFreqFirst()+fDnu*y;
      // }
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

      float normFactor=1;
//      if (bandMean==0) normFactor=1;
//     else normFactor=pow(bandMean,-1);
      float bico1, bico2;
      if (!fIsSumPerBandAvailable){
	bico1=normFactor*fBaseRun->GetBandSignal((fBaseRun->GetNBands()-1)-y)->GetSignal(int(floor(i+delta))%npoints);
	bico2=normFactor*fBaseRun->GetBandSignal((fBaseRun->GetNBands()-1)-y)->GetSignal(int((floor(i+delta)+1))%npoints);
      }
      else {
	bico1=normFactor*fCompensatedSignalBandSum[(fBaseRun->GetNBands()-1)-y].GetSignal(int(floor(i+delta))%npoints);
	bico2=normFactor*fCompensatedSignalBandSum[(fBaseRun->GetNBands()-1)-y].GetSignal(int((floor(i+delta)+1))%npoints);
      }
      float lowerBinFrac=1-((i+delta)-floor(i+delta));
      float upperBinFrac=1-lowerBinFrac;
      if (floor(i+delta)+1<fBaseRun->GetNPoints()) bico+=lowerBinFrac*bico1+upperBinFrac*bico2;
    }

    fCompensatedSignal.SetSignal(i-1,bico);
    fCompensatedSignalSum.SetSignal(i-1,bico);
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

  fSumProfile.freq = fBaseRun->GetFreqLast();

  for (int i=0; i<fSumProfile.numpointwin; i++) fSumProfile.prfdata.push_back(0);

  return 1;
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
  return 1;
}

int PulseExtractor::sumPeriods()
{
  std::cout<<"sum periods"<<std::endl;
  if (!fIsSumPerBandAvailable){
    for (int i=0; i<fBaseRun->GetNumpuls(); i++){
      for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
	fSumProfile.prfdata[j]+=fCompensatedSignal.GetSignal(i*fBaseRun->GetNumpointwin()+j);
	fCompensatedSignalSum.SetSignal(j,fSumProfile.prfdata[j]);
      }
    }
  }
  else{
    for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
      fSumProfile.prfdata[j]+=fCompensatedSignalSum.GetSignal(j);
    }
  }
  return 1;
}

int PulseExtractor::sumPerBandPeriods()
{
  std::cout<<"sum per band periods"<<std::endl;
  for (int k=0; k<fBaseRun->GetNBands(); k++){
    std::vector<float> sums;
    for (int j=0; j<fBaseRun->GetNumpointwin(); j++) sums.push_back(0);
    for (int i=0; i<fBaseRun->GetNumpuls(); i++){
      for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
	sums[j]+=fBaseRun->GetBandSignal(k)->GetSignal(i*fBaseRun->GetNumpointwin()+j);
	fCompensatedSignalBandSum[k].SetSignal(j,sums[j]);
      }
    }
  }
  return 1;
}

int PulseExtractor::PrintSumProfile(std::string dirname)
{
  std::cout<<"print profile"<<std::endl;
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/%s.prf",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### COMPENSATED SUM PROFILE "<<"\n";
  sumProfileStream<<"time        signal"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<time<<"     "<<fSumProfile.prfdata[i]<<"\n";
  }
  return 1;
}

int PulseExtractor::PrintFrequencyResponse(std::string dirname)
{
  std::cout<<"print profile"<<std::endl;
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/%s.fr",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### FREQUENCY RESPONSE "<<"\n";
  sumProfileStream<<"frequency         mean signal"<<"\n";
  for (int i=0; i<fBaseRun->GetNBands(); i++){
    float freq=fBaseRun->GetFreqFirst()+i*(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/fBaseRun->GetNBands();
    sumProfileStream<<freq<<"     "<<fBaseRun->GetFreqResponse(i)<<"\n";
  }
  return 1;
}

int PulseExtractor::printHeader(std::ofstream* stream)
{
  *stream<<"### HEADER "<<"\n";
  *stream<<"telcode: "<<fSumProfile.telcode<<"\n";
  *stream<<"obscode: "<<fSumProfile.obscode<<"\n";
  *stream<<"rtype: "<<fSumProfile.rtype<<"\n";
  *stream<<"psrname: "<<fSumProfile.psrname<<"\n";
  *stream<<"period: "<<fSumProfile.period<<"\n";
  *stream<<"tau: "<<fSumProfile.tau<<"\n";
  *stream<<"date: "<<fSumProfile.day<<"/"<<fSumProfile.month<<"/"<<fSumProfile.year<<"\n";
  *stream<<"time: "<<fSumProfile.hour<<":"<<fSumProfile.min<<":"<<fSumProfile.sec<<"\n";
  *stream<<"frequency: "<<fSumProfile.freq<<"\n";
  return 1;
}

int PulseExtractor::PrintPerBandSumProfile(std::string dirname)
{
  std::cout<<"print per band profile"<<std::endl;
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/bands_%s.prf",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### COMPENSATED SUM PROFILE FOR EACH FREQUENCY BAND"<<"\n";
  sumProfileStream<<"time        signal1 signal2... signal 512"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<std::setw(6)<<time<<"        ";
    for (int j=0; j<512; j++){
      sumProfileStream<<std::setw(6)<<fCompensatedSignalBandSum[j].GetSignal(i)<<"         ";
    }
    sumProfileStream<<std::endl;
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

int PulseExtractor::SumPerBandPeriods()
{
  fIsSumPerBandAvailable=true;
  sumPerBandPeriods();
  return 1;
}

std::vector<float> PulseExtractor::GetSumPeriodsVec()
{
  std::vector<float> vec;
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    vec.push_back(fCompensatedSignalSum.GetSignal(i));
  }
  return vec;
}


