#include "BaseRun.h"

struct SumProfile {
  
  std::string telcode; // код телескопа, по умолчанию bsa1 возможные опции bsa2, bsa3, Klz64
  std::string obscode;  // 2-символьный код обсерватории, по умолчанию "PO" - Пущинская обсерватория, антенна БСА
  std::string rtype; // тип приемника, по умолчанию = "DPP1" (digital pulsar processor 1)
  std::string psrname; // соотв. переменной name в файле данных БСА - не более 10 символов
  std::string datatype; //тип данных, однобуквенный код, по умолчанию = 'I' - интенсивность
  int npol; // число поляризаций, по умолчанию для БСА = 1
  
  int year; //дата наблюдений, соответствует date в файле данных БСА
  int month;
  int day;
  int hour; //время в текущей шкале локального стандарта, соответствует time hh:mm:ss
  int min;
  int sec;
  int nsec; //time в 100 нс 

  int utcday;
  int utcmonth;
  int utcyear;
  int utchour; //время в истиной шкале локального стандарта, dt_utc hh:mm:ss - для БСА соответствует UTC+3 часа
  int utcmin;
  int utcsec;
  int utcnsec; //dt_utc в 100 нс 
  
  float period; // период пульсара в секундах
  int numpuls; // число импульсов, сложенных в данном суммарном профиле
  
  float tau; // дискрет отсчета в миллисекундах, соотв. переменной tay в файле данных БСА
  int numpointwin; // число точек в окне <= int(period/tau)
  float freq; //  приведенная частота, МГц, если суммировали по мере дисперсии к первому (самому низкочастотному) каналу БСА = 109.584, если к последнему, 512-му = 112.084, если к другому, то вычисляется с учетом ширины канала = 2500/511 кГц
  
  std::vector<float> prfdata; // последовательность отсчетов в суммарном, после устранения меры дисперсии и приведения к нулевому уровню (вычитания нулевого уровня), профиле. 
  
};

class PulseExtractor : BaseRun
{
 public:
  PulseExtractor();
  PulseExtractor(BaseRun* run, float DM);
  ~PulseExtractor();
  
  int SetBaseRun(BaseRun* br) {fBaseRun=br;}
  int SetDM(float DM) {fDM=DM;}
  
  int DoCompensation();
  int SumPeriods();
  
  float GetDM();
  int PrintSumProfile(std::string outFile);
  SumProfile GetSumProfile() {return fSumProfile;}

  int ReadMask(std::string fname);
  int SetBandMask(std::vector<float> mask) {fBandMask=mask;}

 private:
  BaseRun* fBaseRun;
  float fDM;
  SignalContainer fCompensatedSignal;
  SignalContainer fCompensatedSignalSum;
  SumProfile fSumProfile;
  int compensateDM();
  int sumPeriods();
  int fillSumProfile();
  
  std::vector<float> fBandMask;
};
