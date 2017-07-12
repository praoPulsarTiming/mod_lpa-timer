//#include "BaseRun.h"
#include "PulseExtractor.h"

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

  BaseRun br;
  //loop over files
  for (int iPack=0; iPack<floor(nFiles); iPack++){

    br.ReadRAWData(runID[iPack], rawdata_dir, output_dir);

    PulseExtractor pulse(&br, 13.977);
    
    pulse.ReadMask("examples/bandMask.dat");
    //   pulse.FrequencyFilter(0.5);
    //   pulse.SpikeFilter(0.5);
    pulse.DoCompensation();
    pulse.SumPeriods();
    
    SumProfile finPulse=pulse.GetSumProfile();
    pulse.PrintSumProfile("examples/output.txt");
  }

  return 0;
}
