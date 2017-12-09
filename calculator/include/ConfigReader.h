//набор процедур для чтения конфигурационного файла

struct ConfigParam{
  std::string rawdata_dir;
  std::string output_dir;
  std::string maskfile; // добавлено мной 17.08
  std::string tplfile; // добавлено мной 28.08
  bool doRemoveSpikes;
  bool doFRfiltering;
  bool getDynSpectrum;
  bool getIndImpulses;
  bool getFR;
  bool useTrueNorm;
  bool print_data;
  int pgran;
  float utccorr;
  float nVarFR;
  float nVarSpike;
  std::vector<std::string> runs;
};

int convertStringParam(std::string IN, bool* OUT)
{
  int returnValue=0;
  if (IN=="yes") *OUT=true;
  else if (IN=="no") *OUT=false;
  else returnValue=1;
  return returnValue;
}

ConfigParam ReadConfig(std::string cname)
{
  ConfigParam output;
  
  std::ifstream flist;
  flist.open(cname.c_str());
  char tmp[100];
  int nFiles, nThreads;
  int startFileNumber, endFileNumber;
  std::string confParam;
  //  std::string rawdata_dir, output_dir;
  //  std::string fname;

  bool bRemoveSpikes=false;
  bool bFRfilter=false;
  bool bGetDynSpectrum=false;
  bool bGetIndImpulses=false;
  bool bGetFR=false;
  bool bKeepNorm=true;
  bool bPrintData=false;
  float utccorr;
  std::string sRemoveSpikes, sFRfilter;
  std::string sGetDynSpectrum, sGetIndImpulses, sGetFR, sKeepNorm, sPrintData;
  float utccor;
  float nVarFR, nVarSpike;
  flist>>confParam;
  while(confParam!="runs:"){		 
    //    std::cout<<confParam<<std::endl;
    if (confParam=="inputDir") flist>>output.rawdata_dir;
    else if (confParam=="outputDir") flist>>output.output_dir;
    else if (confParam=="channelMask") flist>>output.maskfile; // добавлено мной 17.08
    else if (confParam=="tplFile") flist>>output.tplfile; // добавлено мной 28.08
    else if (confParam=="startFileNumber") flist>>startFileNumber;
    else if (confParam=="nRuns") flist>>nFiles;
    else if (confParam=="removeSpikes") flist>>sRemoveSpikes;
    else if (confParam=="FRfilter") flist>>sFRfilter;
    else if (confParam=="getDynSpectrum") flist>>sGetDynSpectrum;
    else if (confParam=="getIndImpulses") flist>>sGetIndImpulses;
    else if (confParam=="getFR") flist>>sGetFR;
    else if (confParam=="FRcleaningCut") flist>>nVarFR;
    else if (confParam=="SpikeCleaningCut") flist>>nVarSpike;
    else if (confParam=="useTrueNormalisation") flist>>sKeepNorm;
    else if (confParam=="utcCorrection") flist>>utccorr;
    else if (confParam=="printData") flist>>sPrintData;
    else if (confParam=="outputGranularity")flist>>output.pgran;
    flist.getline(tmp,100,'\n');
    flist>>confParam;
  }

  output.utccorr=utccorr;
  
  int err=0;
  err+=convertStringParam(sRemoveSpikes, &bRemoveSpikes);
  err+=convertStringParam(sFRfilter, &bFRfilter);
  err+=convertStringParam(sGetDynSpectrum, &bGetDynSpectrum);
  err+=convertStringParam(sGetIndImpulses, &bGetIndImpulses);
  err+=convertStringParam(sGetFR, &bGetFR);
  err+=convertStringParam(sKeepNorm, &bKeepNorm);
  err+=convertStringParam(sPrintData, &bPrintData);
  
  output.doRemoveSpikes=bRemoveSpikes;
  output.doFRfiltering=bFRfilter;
  output.getDynSpectrum=bGetDynSpectrum;
  output.getIndImpulses=bGetIndImpulses;
  output.getFR=bGetFR;
  output.nVarFR=nVarFR;
  output.nVarSpike=nVarSpike;
  output.useTrueNorm=bKeepNorm;
  output.print_data=bPrintData;
  
  std::string rID;
  int runCounter=0;
  while (flist>>rID){
    runCounter++;
    flist.getline(tmp,100,'\n');
    //std::cout<<runCounter<<std::endl;
    if (runCounter<startFileNumber) continue;
    if (runCounter>=startFileNumber+nFiles) break;
    output.runs.push_back(rID);
  }
  return output;
}
