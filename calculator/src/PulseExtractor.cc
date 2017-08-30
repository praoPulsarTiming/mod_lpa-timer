#include "PulseExtractor.h"
#include <iomanip>

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

  fCompensatedSignal=SignalContainer(fBaseRun->GetNPoints(),0,fBaseRun->GetNPoints());
  fCompensatedSignalSum=SignalContainer(fBaseRun->GetNumpointwin(),0,fBaseRun->GetNumpointwin());

  for (int i=0; i<fBaseRun->GetNChannels(); i++)
    fDynamicSpectrum.push_back(SignalContainer(fBaseRun->GetNumpointwin(),0,fBaseRun->GetNumpointwin()));
  
  for (int i=0; i<fBaseRun->GetNChannels(); i++) {fChannelMask.push_back(1);}
  for (int i=0; i<fBaseRun->GetNPoints(); i++) {fSpikeMask.push_back(1);}
  fIsDynSpecAvailable=false;
  fNChan=fBaseRun->GetNChannels();
  fNChanAfterMask=fBaseRun->GetNChannels();
}
 

int PulseExtractor::compensateDM()
{
  std::cout<<"компенсация дисперсионного запаздывания"<<std::endl;
  float dm=fDM;
  int npoints;
  if (fIsDynSpecAvailable||fBaseRun->GetSumchan()==1) npoints=fBaseRun->GetNumpointwin();
  else npoints=fBaseRun->GetNPoints();

  for (int i=0; i<npoints; i++){
    float fDnu=fabs(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/512;
    float fDL=fabs(fBaseRun->GetWLLast()-fBaseRun->GetWLFirst())/512;
    float bico=0;
    for (int y=0; y<fBaseRun->GetNChannels(); y++) {
      if (fChannelMask[y]==0) continue;
      //calculate delay wrt 511th for particular freq[511-y]
      float dT=4.6*(-pow(fBaseRun->GetWLLast(),2)+pow(fBaseRun->GetWLLast()+y*fDL,2))*fDM*0.001; //covert to ms
      //calculate residual difference to nearest positive side pulse
      float dTnearest=dT-1000*fBaseRun->GetPeriod()*floor(dT*pow(1000*fBaseRun->GetPeriod(),-1));
      //move frequency band by -dTnearest bins, add lower bins to "upper side"
      float delta=dTnearest*pow(fBaseRun->GetTau(),-1);

      //define normalisation based on channel mask
      float normFactor=fChannelMask[y];

      //channel summation
      float bico1, bico2;
      if (!fIsDynSpecAvailable||fBaseRun->GetSumchan()==1){
	bico1=normFactor*fBaseRun->GetChannelSignal((fBaseRun->GetNChannels()-1)-y)->GetSignal(int(floor(i+delta))%npoints);
	bico2=normFactor*fBaseRun->GetChannelSignal((fBaseRun->GetNChannels()-1)-y)->GetSignal(int((floor(i+delta)+1))%npoints);
      }
      if (fIsDynSpecAvailable) {
	bico1=normFactor*fDynamicSpectrum[(fBaseRun->GetNChannels()-1)-y].GetSignal(int(floor(i+delta))%npoints);
	bico2=normFactor*fDynamicSpectrum[(fBaseRun->GetNChannels()-1)-y].GetSignal(int((floor(i+delta)+1))%npoints);
      }
      float lowerBinFrac=1-((i+delta)-floor(i+delta));
      float upperBinFrac=1-lowerBinFrac;
      if (floor(i+delta)+1<fBaseRun->GetNPoints()) bico+=lowerBinFrac*bico1+upperBinFrac*bico2;
    }

    fCompensatedSignal.SetSignal(i,bico)/fBaseRun->GetNChannels();
    fCompensatedSignalSum.SetSignal(i,bico)/fBaseRun->GetNChannels();
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

  fSumProfile.nChan=fNChan;
  fSumProfile.nChanAfterMask=fNChanAfterMask;
  
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
  fSumProfile.nChan=fNChan;
  fSumProfile.nChanAfterMask=fNChanAfterMask;
  
  return 1;
}

int PulseExtractor::ReadMask(std::string fname)
{
  std::cout<<"чтение маски из файла: "<<fname<<std::endl;
  std::ifstream fmask;
  fmask.open(fname.c_str());
  char tmp[100];
  int iband;
  float value;
  int nActive=0;
  for (int i=0; i<512; i++){
    fmask>>iband>>value;
    if (value!=0) nActive++;
    fChannelMask[iband-1]=value;
  }
  fNChanAfterMask=nActive;
  //  std::cout<<"active channels: "<<nActive<<std::endl;
  return 1;
}

int PulseExtractor::sumPeriods()
{
  std::cout<<"суммирование индивидуальных импульсов"<<std::endl;

  if (!fIsDynSpecAvailable){
    for (int i=0; i<fBaseRun->GetNumpuls(); i++){
      for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
	fSumProfile.prfdata[j]+=fCompensatedSignal.GetSignal(i*fBaseRun->GetNumpointwin()+j)/fBaseRun->GetNumpuls();
	fCompensatedSignalSum.SetSignal(j,fSumProfile.prfdata[j]);
      }
    }
  }
  else{
    for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
      fSumProfile.prfdata[j]+=fCompensatedSignalSum.GetSignal(j);
    }
  }

  //subtract mean over sum profile (or median):
  float median=fCompensatedSignalSum.GetSignalMedian(0,1000000);
  for (int i=0; i<fSumProfile.prfdata.size(); i++){
    fSumProfile.prfdata[i]=fSumProfile.prfdata[i]-median;
  }

  return 1;
}

int PulseExtractor::sumPerChannelPeriods()
{
  std::cout<<"суммирование периодов в каждом канале"<<std::endl;
  for (int k=0; k<fBaseRun->GetNChannels(); k++){
    std::vector<float> sums;

    for (int j=0; j<fBaseRun->GetNumpointwin(); j++) sums.push_back(0);
    
    for (int i=0; i<fBaseRun->GetNumpuls(); i++){
      for (int j=0; j<fBaseRun->GetNumpointwin(); j++){
	sums[j]+=fBaseRun->GetChannelSignal(k)->GetSignal(i*fBaseRun->GetNumpointwin()+j)/fBaseRun->GetNumpuls();
	fDynamicSpectrum[k].SetSignal(j,sums[j]);
      }
    }
  }

  //subtract zero from dynamic spectrum:
  for (int k=0; k<fBaseRun->GetNChannels(); k++){
    float median=fDynamicSpectrum[k].GetSignalMedian(0,1000000);
    for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
      float zsub=fDynamicSpectrum[k].GetSignal(i)-median;
      fDynamicSpectrum[k].SetSignal(i,zsub);
    }
  }
  
  return 1;
}

int PulseExtractor::PrintSumProfile(std::string dirname)
{
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
  }
  std::cout<<"   суммарный профиль записан в файл: "<<tmp<<std::endl;
  return 1;
}

int PulseExtractor::PrintFrequencyResponse(std::string dirname)
{
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
    sumProfileStream<<std::setprecision(7)<<freq<<"     "<<fBaseRun->GetFreqResponse(i)<<"\n";
  }
  std::cout<<"   АЧХ записана в файл: "<<tmp<<std::endl;

  std::ofstream sumProfileStream1;
  sprintf(tmp,"%s/masked_%s.fr",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream1.open(tmp);
  printHeader(&sumProfileStream1);

  sumProfileStream1<<"### MASKED FREQUENCY RESPONSE "<<"\n";
  sumProfileStream1<<"frequency         mean signal"<<"\n";
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    //    std::cout<<"mask: "<<i<<"  "<<fChannelMask[i]<<std::endl;
    double freq=fBaseRun->GetFreqFirst()+i*(float)(fBaseRun->GetFreqLast()-fBaseRun->GetFreqFirst())/(float)fBaseRun->GetNChannels();
    freq+=0.00001;
    //    std::cout<<"mask: "<<i<<"  "<<std::setprecision(7)<<"   "<<freq<<"   "<<fChannelMask[i]<<std::endl;
    if (fChannelMask[i]==0) sumProfileStream1<<std::setprecision(7)<<freq<<"     "<<0<<"\n";
    else sumProfileStream1<<std::setprecision(7)<<freq<<"     "<<fBaseRun->GetFreqResponse(i)<<"\n";
    
  }
  std::cout<<"   АЧХ после маски записана в файл: "<<tmp<<std::endl;
  return 1;
}

int PulseExtractor::printHeader(std::ofstream* stream)
{
  *stream<<"### HEADER "<<"\n";
  *stream<<"telcode: "<<fSumProfile.telcode<<"\n";
  *stream<<"obscode: "<<fSumProfile.obscode<<"\n";
  *stream<<"rtype: "<<fSumProfile.rtype<<"\n";
  *stream<<"psrname: "<<fSumProfile.psrname<<"\n";
  *stream<<"period: "<<std::setprecision(13)<<fSumProfile.period<<"\n";
  *stream<<"tau: "<<std::setprecision(6)<<fSumProfile.tau<<"\n";
  *stream<<"date: "<<fSumProfile.day<<"/"<<fSumProfile.month<<"/"<<fSumProfile.year<<"\n";
  *stream<<"time: "<<fSumProfile.hour<<":"<<fSumProfile.min<<":"<<std::setprecision(7)<<fSumProfile.sec<<"\n";
  *stream<<"utctime: "<<fSumProfile.utchour<<":"<<fSumProfile.utcmin<<":"<<std::setprecision(8)<<fSumProfile.utcsec<<"\n";
  *stream<<"frequency: "<<std::setprecision(7)<<fSumProfile.freq<<"\n";
  *stream<<"N used channels: "<<fSumProfile.nChanAfterMask<<"\n";
  return 1;
}

int PulseExtractor::PrintChannelSumProfile(std::string dirname)
{
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
      sumProfileStream<<std::setw(6)<<fDynamicSpectrum[j].GetSignal(i)<<"         ";
    }
    sumProfileStream<<std::endl;
  }
  std::cout<<"   динамический спектр записан в файл: "<<tmp<<std::endl;
  return 1;
}

int PulseExtractor::PrintCompensatedImpulses(std::string dirname)
{
  std::ofstream sumProfileStream;
  char tmp[100];
  sprintf(tmp,"%s/compPulses_%s.prf",dirname.c_str(),fBaseRun->GetRunID().c_str());
  sumProfileStream.open(tmp);
  printHeader(&sumProfileStream);

  sumProfileStream<<"### COMPENSATED PROFILE FOR EACH IMPULSE"<<"\n";
  sumProfileStream<<"time       signal1  signal2... signal(fBaseRun->GetNumpuls())"<<"\n";
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    float time=fBaseRun->GetTau()*i;
    sumProfileStream<<std::setw(6)<<i<<"        ";
    for (int j=0; j<fBaseRun->GetNumpuls(); j++){
      sumProfileStream<<std::setw(6)<<fCompensatedSignal.GetSignal(i+j*fBaseRun->GetNumpointwin())<<"         ";
    }
    sumProfileStream<<std::endl;
  }
  std::cout<<"   массив индивидуальных импульсов записан в файл: "<<tmp<<std::endl;
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
  if (fIsDynSpecAvailable) return 1;
  fIsDynSpecAvailable=true;
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

int PulseExtractor::removeSpikes(float nVar)
{
  std::cout<<"удаление импульсных помех"<<std::endl;
  
  SignalContainer sumSigRef(fBaseRun->GetNPoints(),0,fBaseRun->GetNPoints()*fBaseRun->GetTau());

  //calculate reference signal sum with DM=0
  for (int y=0; y<fBaseRun->GetNChannels(); y++){
    if (fChannelMask[y]==0) continue;
    for (int i=0; i<fBaseRun->GetNPoints(); i++){
      sumSigRef.SetSignal(i,sumSigRef.GetSignal(i)+fBaseRun->GetChannelSignal(y)->GetSignal(i)*fChannelMask[y]);
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
    if (variance==0) continue;
    //    std::cout<<"spikeFinder:   "<<i<<"   "<<median<<"      "<<variance<<"       "<<sumSigRef.GetSignal(i)<<std::endl;
    if (fabs(sumSigRef.GetSignal(i)-median)/variance > nVar) {
      //      std::cout<<"FOUND SPIKE"<<std::endl;
      //      std::cout<<" before filter:     "<<fBaseRun->GetChannelSignal(100)->GetSignal(i)<<std::endl;
      fSpikeMask[i]=0;
      //      std::cout<<"FOUND SPIKE"<<std::endl;
      for (int y=0; y<fBaseRun->GetNChannels(); y++){
	fBaseRun->GetChannelSignal(y)->SetSignal(i,pow(fChannelMask[y],-1)*median/fBaseRun->GetNChannels());
      }
      //      std::cout<<" after filter:    "<<fBaseRun->GetChannelSignal(100)->GetSignal(i)<<std::endl;
    }
  }
  return 1;
}

int PulseExtractor::frequencyFilter(float nVar)
{
  std::cout<<"удаление зашумленных каналов"<<std::endl;
  SignalContainer buf(fBaseRun->GetNChannels(),0,fBaseRun->GetNChannels());
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    buf.SetSignal(i,fBaseRun->GetFreqResponse(i));
  }
  float variance=buf.GetSignalVariance(0,1000000);
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    float med=0;
    if (i>=5||i<=fBaseRun->GetNChannels()-5){
      
      med=buf.GetSignalMedian(i-5,i+5);
      variance=buf.GetSignalVariance(i-5,i+5);
      //      std::cout<<"mean debug   "<<buf.GetSignalMedian(i-5,i+5)<<"   "<<buf.GetSignalMean(i-5,i+5)<<"   "<<buf.GetSignalVariance(i-5,i+5)<<std::endl;
      //      for (int j=i-5; j<=i+5; j++) std::cout<<"      "<<buf.GetSignal(j)<<std::endl;
    }
    else if (i<5) {
      med=buf.GetSignalMedian(0,i+5);
      variance=buf.GetSignalVariance(0,i+5);
    }
    else if (i>fBaseRun->GetNChannels()-5) {
      med=buf.GetSignalMedian(i-5,100000);
      variance=buf.GetSignalVariance(i-5,100000);
    }
    //    std::cout<<"freqCleaner:  "<<med<<"     "<<variance<<"     "<<buf.GetSignal(i)<<std::endl;
    if (variance==0) continue;
    if (fabs(buf.GetSignal(i)-med)/variance>nVar) {
      //     std::cout<<"SPIKE FOUND"<<std::endl;
      fChannelMask[i]=0;
    }
  }
  return 1;
}

int PulseExtractor::FillMaskFRweights()
{
  std::cout<<"вычисление весов каналов по АЧХ"<<std::endl;
  float frmean=0;
  float frweight;
  int nchan=0;
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    if (fChannelMask[i]==0) continue;
    frmean+=fBaseRun->GetFreqResponse(i);
    nchan++;
  }
  
  if (frmean==0||nchan==0) return 0;
  frmean=frmean/nchan;
  for (int i=0; i<fBaseRun->GetNChannels(); i++){
    if (fChannelMask[i]==0) continue;
    if (fBaseRun->GetFreqResponse(i)==0) {
      fChannelMask[i]=0;
      continue;
    }
    frweight=pow(fBaseRun->GetFreqResponse(i)/frmean,-1);
    fChannelMask[i]=frweight;
  }

  int nActive=0;
  for (int i=0; i<fNChan; i++){
    if (fChannelMask[i]!=0) nActive++;
  }
  fNChanAfterMask=nActive;
    
  return 1;
}

int PulseExtractor::RemoveSpikes(float nVar)
{
  removeSpikes(nVar);
  return 1;
}

int PulseExtractor::CleanFrequencyResponse(float nVar)
{
  frequencyFilter(nVar);
  return 1;
}


std::vector<float> PulseExtractor::GetChannelSumProfile(int iChan)
{
  std::vector<float> rVec;
  for (int i=0; i<fBaseRun->GetNumpointwin(); i++){
    rVec.push_back(fDynamicSpectrum[iChan].GetSignal(i)*fChannelMask[iChan]);
  }
  return rVec;
}
