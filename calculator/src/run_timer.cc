//#include "BaseRun.h"
//#include "PulseExtractor.h"
#include "ccf.h"
#include "ConfigReader.h"
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
  
  ConfigParam conf=ReadConfig(configName);

  // создаем контейнер для данных сеанса
  BaseRun br;
  //loop over files
  
  for (int iPack=0; iPack<floor(conf.runs.size()); iPack++){

    // считываем данные сеанса
    br.ReadRAWData(conf.runs[iPack], conf.rawdata_dir, conf.output_dir);

    // создаем объект класса для обработки
    PulseExtractor pulse(&br);

    //  считываем маску
    pulse.ReadMask("examples/channel_mask.dat");

    pulse.NormaliseToUnity(!conf.useTrueNorm);
    pulse.FillMaskFRweights();

    // фильтры на данный момент отсутствуют (13.07.17)
    if (conf.doFRfiltering) pulse.CleanFrequencyResponse(conf.nVarFR);
    if (conf.doRemoveSpikes) pulse.RemoveSpikes(conf.nVarSpike);
    
    if (!conf.getIndImpulses){
      pulse.SumPerChannelPeriods();
      pulse.DoCompensation();
      if (conf.getDynSpectrum) pulse.PrintChannelSumProfile(conf.output_dir.c_str());
    }
    
    if (conf.getIndImpulses){
      if (!conf.getDynSpectrum){
	pulse.DoCompensation();
	pulse.PrintCompensatedImpulses(conf.output_dir.c_str());
      }
      if (conf.getDynSpectrum) {
	pulse.DoCompensation();
	pulse.PrintCompensatedImpulses(conf.output_dir.c_str());
	pulse.SumPerChannelPeriods();
	pulse.DoCompensation();
	pulse.PrintChannelSumProfile(conf.output_dir.c_str());
      }
    }
    
    /*
    // суммируем периоды для каждой из частот
    if (conf.getDynSpectrum) {
      pulse.SumPerChannelPeriods();
      pulse.PrintPerChannelSumProfile(conf.output_dir.c_str());
      // компенсируем dm 
      pulse.DoCompensation();
    }
    */
    
    // суммируем периоды для компенсированных данных
    pulse.SumPeriods();
    
    // далее получаем данные
    // получение структуры суммарного импульса
    SumProfile finPulse=pulse.GetSumProfile();
    // распечатать суммарный импульс в файл
    pulse.PrintSumProfile(conf.output_dir.c_str());
    //  распечатать суммарные импульсы по частотам в файл
    // pulse.PrintPerChannelSumProfile(output_dir.c_str());

    //  распечатать АЧХ в файл
    if (conf.getFR) pulse.PrintFrequencyResponse(conf.output_dir.c_str());

    // получить суммарный импульс в виде вектора
    //std::vector<float> sumpuls=pulse.GetSumPeriodsVec();

    // СЮДА МОЖНО ДОБАВЛЯТЬ КОД ДЛЯ КРОСС-КОРРЕЛЯЦИИ

    Cor cor;
    Skkf skkf;
     
    cor.ccf(finPulse, conf.rawdata_dir, conf.output_dir, conf.runs[iPack], conf.tplfile, conf.utccorr);
    
  }
  std::cout<<"######################################"<<std::endl;
  std::cout<<"# работа программы успешно завершена #"<<std::endl;
  std::cout<<"######################################"<<std::endl;  
  return 0;
}
