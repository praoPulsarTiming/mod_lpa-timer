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
  
  int nChan;
  int nChanAfterMask;
  
  int year; //дата наблюдений, соответствует date в файле данных БСА
  int month;
  int day;
  int hour; //время в текущей шкале локального стандарта, соответствует time hh:mm:ss
  int min;
  long double sec;

  int utcday;
  int utcmonth;
  int utcyear;
  int utchour; //время в истиной шкале локального стандарта, dt_utc hh:mm:ss - для БСА соответствует UTC+3 часа
  int utcmin;
  long double utcsec;
  
  long double period; // период пульсара в секундах
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
  int SumPerChannelPeriods(); // функция суммирует периоды для каждой из частот
  
  float GetDM() {return fDM;}
  int PrintFrequencyResponse(std::string outdir); // записать АЧХ (средний сигнал на частоте) в файл с именем outdir/<номер сеанса>.fr
  int PrintSumProfile(std::string outdir);   // записать суммарный профиль в файл с именем outdir/<номер сеанса>.prf
  int PrintChannelSumProfile(std::string outdir); // записать суммарный профиль для каждого частотного канала в файл с именем outdir/bands_<номер сеанса>.prf
  int PrintCompensatedImpulses(std::string outdir);  //записать в файл набор компенсированных импульсов до fBasRun->GetNumpuls() 

  SumProfile GetSumProfile() {return fSumProfile;} // получить структуру, описанную в начале этог файла

  std::vector<float> GetSumPeriodsVec(); // получить суммарный профиль в виде вектора

  std::vector<float> GetChannelSumProfile(int iChan); //получить суммарный профиль для частотного канала iChan 
  
  SignalContainer GetCompensatedImpulse(int i); // получить i-й компенсированный импульс
  std::vector<float> GetCompensatedImpulseVec(int i); // получить i-й компенсированный импульс как вектор

  int FillMaskFRweights();  // заполняются веса частотных каналов в маске. Определяется среднее значение сигнала по всем частотам, M, вес для канала определен так: w=pow(m(f)/M, -1)
  
  int ReadMask(std::string fname); // считать маску частот
  int SetChannelMask(std::vector<float> mask) {fChannelMask=mask;} // задать маску
  std::vector<float> GetChannelMask() {return fChannelMask;} // получить маску в виде вектора
  
  int RemoveSpikes(float nVar);   // удалить шумовые импульсы, частотные каналы складываются с dm=0, выбросы > nVar*sigma заменяются на медианное значение подложки
  int CleanFrequencyResponse(float nVar);  //удалить зашемленные частоты, выбросы на АЧХ по модулю > nVar*sigma добавляются в маску с весом 0
  

 private:
  BaseRun* fBaseRun;
  float fDM;
  SignalContainer fCompensatedSignal;
  SignalContainer fCompensatedSignalSum;
  std::vector<SignalContainer> fDynamicSpectrum;
  SumProfile fSumProfile;
  int compensateDM();
  int sumPeriods();
  int sumPerChannelPeriods();
  int fillSumProfile();
  int printHeader(std::ofstream* str);

  int removeSpikes(float);
  int frequencyFilter(float);
  
  std::vector<float> fChannelMask;

  bool fIsDynSpecAvailable;
  
  std::vector<int> fSpikeMask;

  int fNChan;
  int fNChanAfterMask;
};
