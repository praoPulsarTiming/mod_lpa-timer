#include "BaseRun.h"
#include <stdlib.h>
struct SumProfile {
  
  std::string telcode; // код телескопа, по умолчанию bsa1 возможные опции bsa2, bsa3, Klz64
  std::string obscode;  // 2-символьный код обсерватории, по умолчанию "PO" - Пущинская обсерватория, антенна БСА
  std::string rtype; // тип приемника, по умолчанию = "DPP1" (digital pulsar processor 1)
  std::string psrname; // соотв. переменной name в файле данных БСА - не более 10 символов
  std::string datatype; //тип данных, однобуквенный код, по умолчанию = 'I' - интенсивность
  int npol; // число поляризаций, по умолчанию для БСА = 1

  int sumchan;
  
  int year; //дата наблюдений, соответствует date в файле данных БСА
  int month;
  int day;
  int hour; //время в текущей шкале локального стандарта, соответствует time hh:mm:ss
  int min;
  float sec;
  //  int sec;
  //  int nsec; //time в 100 нс 

  int utcday;
  int utcmonth;
  int utcyear;
  int utchour; //время в истиной шкале локального стандарта, dt_utc hh:mm:ss - для БСА соответствует UTC+3 часа
  int utcmin;
  float utcsec;
  //  int utcsec;
  //  int utcnsec; //dt_utc в 100 нс 
  
  float period; // период пульсара в секундах
  int numpuls; // число импульсов, сложенных в данном суммарном профиле
  
  float tau; // дискрет отсчета в миллисекундах, соотв. переменной tay в файле данных БСА
  int numpointwin; // число точек в окне <= int(period/tau)
  float freq; //  приведенная частота, МГц, если суммировали по мере дисперсии к первому (самому низкочастотному) каналу БСА = 109.584, если к последнему, 512-му = 112.084, если к другому, то вычисляется с учетом ширины канала = 2500/511 кГц
  
  std::vector<float> prfdata; // последовательность отсчетов в суммарном, после устранения меры дисперсии и приведения к нулевому уровню (вычитания нулевого уровня), профиле. 
  
};

//  класс для обработки сеансов
//  все параметры сеанса считываются в конструкторе
class PulseExtractor : BaseRun
{
 public:
  PulseExtractor();
  PulseExtractor(BaseRun* run); // конструктор принимает ссылку на класс сеанса
  ~PulseExtractor();
  
  int SetBaseRun(BaseRun* br) {fBaseRun=br;} // можно задать ссылку на сеанс
  int SetDM(float DM) {fDM=DM;} // можно утановить произвольную DM 
  
  int DoCompensation(); // функция проводит компенсацию запаздывания, время всегда приводится к самой высокой частоте 112.084, если ранее было проведено суммирование периодов для отдельных частот, исполуются свернутые профили, иначе полные профили для каждой частоты
  int SumPeriods();   // функция суммирует периоды в компенсированных данных, если суммирование было проведено ранее для каждой из частот, функция ничего не меняет, только заполняет финальный объект SumProfile
  int SumPerBandPeriods(); // функция суммирует периоды для каждой из частот
  
  float GetDM() {return fDM;}
  int PrintFrequencyResponse(std::string outdir); // распечатать АЧХ (средний сигнал на частоте) в файл с именем outdir/<номер сеанса>.fr
  int PrintSumProfile(std::string outdir);   // распечатать суммарный профиль в файл с именем outdir/<номер сеанса>.prf
  int PrintPerBandSumProfile(std::string outdir); // распечатать суммарный профиль  для каждой из частот в файл с именем outdir/bands_<номер сеанса>.prf
  int PrintCompensatedImpulses(std::string outdir);

  SumProfile GetSumProfile() {return fSumProfile;} // получить структуру, описанную выше

  std::vector<float> GetSumPeriodsVec(); // получить суммарный профиль в виде вектора
  
  int ReadMask(std::string fname); // считать маску частот
  int SetBandMask(std::vector<float> mask) {fBandMask=mask;} // задать маску

 private:
  BaseRun* fBaseRun;
  float fDM;
  SignalContainer fCompensatedSignal;
  SignalContainer fCompensatedSignalSum;
  std::vector<SignalContainer> fCompensatedSignalBandSum;
  SumProfile fSumProfile;
  int compensateDM();
  int sumPeriods();
  int sumPerBandPeriods();
  int fillSumProfile();
  int printHeader(std::ofstream* str);
      
  std::vector<float> fBandMask;

  bool fIsSumPerBandAvailable;
};
