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

  fCompensatedSignal=SignalContainer(fBaseRun->GetNumpointwin()*fBaseRun->GetNumpuls(),0,fBaseRun->GetNumpointwin()*fBaseRun->GetNumpuls());
  fCompensatedSignalSum=SignalContainer(fBaseRun->GetNumpointwin(),0,fBaseRun->GetNumpointwin());

  for (int i=0; i<fBaseRun->GetNChannels(); i++)
    fCompensatedSignalChannelSum.push_back(SignalContainer(fBaseRun->GetNumpointwin(),0,fBaseRun->GetNumpointwin()));
  
  for (int i=0; i<fBaseRun->GetNChannels(); i++) {fChannelMask.push_back(1);}
  for (int i=0; i<fBaseRun->GetNPoints(); i++) {fSpikeMask.push_back(1);}
  fIsSumPerChannelAvailable=false;
  //  if (fBaseRun->GetSumchan()==1) fIsSumPerChannelAvailable=true; 
}


int PulseExtractor::compensateDM()
{
  std::cout<<"do compensation"<<std::endl;
  float dm=fDM;
  //define which bins current thread would sum
  //  std::cout<<fTau<<"  "<<fPeriod<<" "<<fNPeriods<<" "<<fNThreads<<"   start: "<<startPeriod<<"  "<<endPeriod<<std::endl;
  int npoints;
  if (fIsSumPerChannelAvailable||fBaseRun->GetSumchan()==1) npoints=fBaseRun->GetNumpointwin();
  else npoints=fBaseRun->GetNPoints();
  
  for (int i=0; i<npoints; i++){
    //(i<=fNBinsPerPeriod)||i>fNBins-fNBinsPerPeriod) continue;
    
    //for (int i=1; i<fNBins+1; i++){
    float fDnu=fabs(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/512;
    float fDL=fabs(fBaseRun->GetWLLast()-fBaseRun->GetWLFirst())/512;
    float bico=0;
    for (int y=0; y<fBaseRun->GetNChannels(); y++) {
      if (fChannelMask[y]==0) continue;
      //if (fSumProfile.freq==-1) {
      //fSumProfile.freq=fBaseRun->GetFreqFirst()+fDnu*y;
      // }
      //take sigTimeProfile[511-y] as 511-th is shorter wavelength
      //calculate delay wrt 511th for particular freq[511-y]
      float dT=4.6*(-pow(fBaseRun->GetWLLast(),2)+pow(fBaseRun->GetWLLast()+y*fDL,2))*fDM*0.001; //covert to ms
      //calculate residual difference to nearest positive side pulse
      float dTnearest=dT-1000*fBaseRun->GetPeriod()*floor(dT*pow(1000*fBaseRun->GetPeriod(),-1));
      //move frequency band by -dTnearest bins, add lower bins to "upper side"
      float delta=dTnearest*pow(fBaseRun->GetTau(),-1);
      //float bico1=fSigArray[((fNFreq-1)-y)*fNBinsInput+int(floor(i+delta))];
      //float bico2=fSigArray[((fNFreq-1)-y)*fNBinsInput+int(floor(i+delta)+1)];
      //float bico1=sigArray[((511-y)*nBinsInput+int(floor(i+delta)))%(nBinsInput*nFreq)];
      //float bico2=sigArray[((511-y)*nBinsInput+int((floor(i+delta)+1)))%(nBinsInput*nFreq)];

      //add normalisation here
      //float bandMean=fBaseRun->GetFreqResponse(y);
      float normFactor=fChannelMask[y];

      //      if (bandMean==0) normFactor=1;
      //     else normFactor=pow(bandMean,-1);
      float bico1, bico2;
      if (!fIsSumPerChannelAvailable||fBaseRun->GetSumchan()==1){
	bico1=normFactor*fBaseRun->GetChannelSignal((fBaseRun->GetNChannels()-1)-y)->GetSignal(int(floor(i+delta))%npoints);
	bico2=normFactor*fBaseRun->GetChannelSignal((fBaseRun->GetNChannels()-1)-y)->GetSignal(int((floor(i+delta)+1))%npoints);
      }
      if (fIsSumPerChannelAvailable) {
	bico1=normFactor*fCompensatedSignalChannelSum[(fBaseRun->GetNChannels()-1)-y].GetSignal(int(floor(i+delta))%npoints);
	bico2=normFactor*fCompensatedSignalChannelSum[(fBaseRun->GetNChannels()-1)-y].GetSignal(int((floor(i+delta)+1))%npoints);
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

  fSumProfile.sumchan = fBaseRun->GetSumchan();

  fSumProfile.year = fBaseRun->GetYear();
  fSumProfile.month = fBaseRun->GetMonth();
  fSumProfile.day = fBaseRun->GetDay();
  fSumProfile.hour = fBaseRun->GetHour();
  fSumProfile.min = fBaseRun->GetMinute();
  fSumProfile.sec = fBaseRun->GetSecond();
  fSumProfile.utcyear = fBaseRun->GetUtcYear();
  fSumProfile.utcmonth = fBaseRun->GetUtcMonth();
  fSumProfile.utcday = fBaseRun->GetUtcDay();
  fSumProfile.utchour = fBaseRun->GetUtcHour();
  fSumProfile.utcmin = fBaseRun->GetUtcMinute();
  fSumProfile.utcsec = fBaseRun->GetUtcSecond();

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
    fChannelMask[iband-1]=value;
  }
  return 1;
}

int PulseExtractor::sumPeriods()
{
  std::cout<<"sum periods"<<std::endl;
  if (!fIsSumPerChannelAvailable){
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
  
  //subtract mean:
  for (int i=0; i<fSumProfile.prfdata.size(); i++){
    fSumProfile.prfdata[i]=fSumProfile.prfdata[i]-fCompensatedSignalSum.GetSignalMedian(0,1000000);
  }
  
  return 1;
}

int PulseExtractor::sumPerChannelPeriods()
{
  std::cout<<"sum per band periods"<<std::endl;
  for (int k=0; k<fBaseRun->GetNChannels(); k++){
    std::vector<float> sums;

    for (int j=0; j<fBaseRun->GetNumpointwin(); j++) sums.push_back(0);
    
    for (int i=0; i<fBaseRun->GetNumpuls(); i++){
      for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
	sums[j]+=fBaseRun->GetChannelSignal(k)->GetSignal(i*fBaseRun->GetNumpointwin()+j);
	//std::cout<<i<<"  "<<j<<"   "<<i*fBaseRun->GetNumpointwin()+j<<"   "<<fBaseRun->GetChannelSignal(k)->GetSignal(i*fBaseRun->GetNumpointwin()+j)<<std::endl;
	fCompensatedSignalChannelSum[k].SetSignal(j,sums[j]);
      }
    }
  }
  return 1;
}

int PulseExtractor::PrintSumProfile(std::string dirname)
{
  std::cout<<"print profile  "<<fBaseRun->GetNumpointwin()<<std::endl;
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/%s.prf",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### COMPENSATED SUM PROFILE "<<"\n";
  sumProfileStream<<"time        signal"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<i<<"     "<<fSumProfile.prfdata[i]<<"\n";
    //    std::cout<<i<<"  "<<fSumProfile.prfdata[i]<<std::endl;
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
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    double freq=fBaseRun->GetFreqFirst()+i*(float)(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/(float)fBaseRun->GetNChannels();
    freq+=0.00001;
    //std::cout<<std::setprecision(7)<<"zz:   "<<fBaseRun->GetFreqFirst()<<"   "<<freq<<"   "<<fBaseRun->GetFreqLast()<<std::endl;
    sumProfileStream<<std::setprecision(7)<<freq<<"     "<<fBaseRun->GetFreqResponse(i)<<"\n";
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
  *stream<<"period: "<<std::setprecision(9) <<fSumProfile.period<<"\n";
  *stream<<"tau: "<<std::setprecision(4)<<fSumProfile.tau<<"\n";
  *stream<<"date: "<<fSumProfile.day<<"/"<<fSumProfile.month<<"/"<<fSumProfile.year<<"\n";
  *stream<<"time: "<<fSumProfile.hour<<":"<<fSumProfile.min<<":"<<fSumProfile.sec<<"\n";
  *stream<<"frequency: "<<fSumProfile.freq<<"\n";
  return 1;
}

int PulseExtractor::PrintPerChannelSumProfile(std::string dirname)
{
  std::cout<<"print per band profile"<<std::endl;
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/bands_%s.prf",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### COMPENSATED SUM PROFILE FOR EACH FREQUENCY BAND"<<"\n";
  sumProfileStream<<"time     signal1     signal2...    signal512"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<std::setw(6)<<time<<"        ";
    for (int j=0; j<512; j++){
      sumProfileStream<<std::setw(6)<<fCompensatedSignalChannelSum[j].GetSignal(i)<<"         ";
    }
    sumProfileStream<<std::endl;
  }
  
  return 1;
}

int PulseExtractor::PrintCompensatedImpulses(std::string dirname)
{
  std::cout<<"print comp impulse profile"<<std::endl;
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/compPulses_%s.prf",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### COMPENSATED PROFILE FOR EACH IMPULSE"<<"\n";
  sumProfileStream<<"time       signal1  signal2... signal(fBaseRun->GetNumpuls())"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<std::setw(6)<<time<<"        ";
    for (int j=0; j<fBaseRun->GetNumpuls(); j++){
      sumProfileStream<<std::setw(6)<<fCompensatedSignal.GetSignal(i+j*fBaseRun->GetNumpointwin())<<"         ";
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

int PulseExtractor::SumPerChannelPeriods()
{
  if (fIsSumPerChannelAvailable) return 1;
  fIsSumPerChannelAvailable=true;
  sumPerChannelPeriods();
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

SignalContainer PulseExtractor::GetCompensatedImpulse(int i)
{
  SignalContainer returnProfile(fBaseRun->GetNumpointwin(),0,fBaseRun->GetTau()*fBaseRun->GetNumpointwin());
  for (int k=0; k<fBaseRun->GetNumpointwin(); k++){
    returnProfile.SetSignal(k,fCompensatedSignal.GetSignal(i*fBaseRun->GetNumpointwin()+k));
  }
  return returnProfile;
}

std::vector<float> PulseExtractor::GetCompensatedImpulseVec(int i)
{
  std::vector<float> returnVec;
  for (int k=0; k<fBaseRun->GetNumpointwin(); k++){
    returnVec.push_back(fCompensatedSignal.GetSignal(i*fBaseRun->GetNumpointwin()+k));
  }
  return returnVec;
}

int PulseExtractor::removeSpikes()
{
  std::cout<<"removing spikes"<<std::endl;
  
  SignalContainer sumSigRef(fBaseRun->GetNPoints(),0,fBaseRun->GetNPoints()*fBaseRun->GetTau());

  std::cout<<"ref"<<std::endl;
  //calculate reference signal sum with DM=0
  for (int y=0; y<fBaseRun->GetNChannels(); y++){
    if (fChannelMask[y]==0) continue;
    for (int i=0; i<fBaseRun->GetNPoints(); i++){
      sumSigRef.SetSignal(i,sumSigRef.GetSignal(i)+fBaseRun->GetChannelSignal(y)->GetSignal(i));
    }
  }
  //find spikes
  for (int i=0; i<fBaseRun->GetNPoints(); i++){
    float median, variance;
    if (i>=5) {
      median=sumSigRef.GetSignalMedian(i-5, i+5);
      variance=sumSigRef.GetSignalVariance(i-5, i+5);
    }
    else {
      median=sumSigRef.GetSignalMedian(0, i+5);
      variance=sumSigRef.GetSignalVariance(0, i+5);
    }
    /*
    else if (i>fBaseRun->GetNPoints()-5) {
      median=sumSigRef.GetSignalMedian(i-5, i+5);
      variance=sumSigRef.GetSignalVariance(i-5, i+5);
    }
    */
    if (fabs(sumSigRef.GetSignal(i)-median)/variance > 5) {
      //      std::cout<<"got spike"<<std::endl;
      fSpikeMask[i]=0;
      for (int y=0; y<fBaseRun->GetNChannels(); y++){
	fBaseRun->GetChannelSignal(y)->SetSignal(i,median/fBaseRun->GetNChannels());
      }
    }
  }
  return 1;
}

int PulseExtractor::frequencyFilter()
{
  std::cout<<"clean frequency response"<<std::endl;
  SignalContainer buf(fBaseRun->GetNChannels(),0,fBaseRun->GetNChannels());
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    buf.SetSignal(i,fBaseRun->GetFreqResponse(i));
  }
  float variance=buf.GetSignalVariance(0,1000000);
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    float med=0;
    if (i>=5||i<=fBaseRun->GetNChannels()-5){
      med=buf.GetSignalMedian(i-5,i+5);
    }
    else if (i<5) med=buf.GetSignalMedian(0,i+5);
    else if (i>fBaseRun->GetNChannels()-5) med=buf.GetSignalMedian(i-5,100000);
    if (fabs(buf.GetSignal(i)-med)/variance>5) fChannelMask[i]=0;
  }
  return 1;
}

int PulseExtractor::FillMaskFRweights()
{
  float frmean=0;
  float frweight;
  int nchan=0;
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    if (fChannelMask[i]==0) continue;
    frmean+=fBaseRun->GetFreqResponse(i);
    nchan++;
  }
  frmean=frmean/nchan;
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    frweight=pow(fBaseRun->GetFreqResponse(i)/frmean,-1);
    fChannelMask[i]=frweight;
  }
  return 1;
}

int PulseExtractor::RemoveSpikes()
{
  removeSpikes();
  return 1;
}

int PulseExtractor::CleanFrequencyResponse()
{
  frequencyFilter();
  return 1;
}
