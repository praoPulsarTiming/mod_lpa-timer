//#include "BaseRun.h"
#include "PulseExtractor.h"

//СЮДА МОЖНО ВСТАВЛЯТЬ ФУНКЦИИ ИЛИ ИНКЛЮДЫ С НИМИ 

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
    //    std::cout<<confParam<<std::endl;
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

  // создаем контейнер для данных сеанса
  BaseRun br;
  //loop over files
  
  for (int iPack=0; iPack<floor(nFiles); iPack++){

    // считываем данные сеанса
    br.ReadRAWData(runID[iPack], rawdata_dir, output_dir);

    // создаем объект класса для обработки
    PulseExtractor pulse(&br);

    //  считываем маску
    pulse.ReadMask("examples/bandMask.dat");

    // фильтры на данный момент отсутствуют (13.07.17)
    //   pulse.FrequencyFilter(0.5);
    //   pulse.SpikeFilter(0.5);

    // суммируем периоды для каждой из частот
    pulse.SumPerBandPeriods();
    // компенсируем dm 
    pulse.DoCompensation();
    // суммируем периоды для компенсированных данных
    pulse.SumPeriods();

    // далее получаем данные
    // получение структуры суммарного импульса
    SumProfile finPulse=pulse.GetSumProfile();
    // распечатать суммарный импульс в файл
    pulse.PrintSumProfile(output_dir.c_str());
    //  распечатать суммарные импульсы по частотам в файл
    pulse.PrintPerBandSumProfile(output_dir.c_str());

    pulse.PrintCompensatedImpulses(output_dir.c_str());
    //  распечатать АЧХ в файл
    pulse.PrintFrequencyResponse(output_dir.c_str());

    // получить суммарный импульс в виде вектора
    //std::vector<float> sumpuls=pulse.GetSumPeriodsVec();


    // СЮДА МОЖНО ДОБАВЛЯТЬ КОД ДЛЯ КРОСС-КОРРЕЛЯЦИИ

  }

  return 0;
}
