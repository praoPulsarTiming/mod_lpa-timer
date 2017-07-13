#include "BaseRun.h"
#include <stdlib.h>
struct SumProfile {
  
  std::string telcode; // п╨п╬п╢ я┌п╣п╩п╣я│п╨п╬п©п╟, п©п╬ я┐п╪п╬п╩я┤п╟п╫п╦я▌ bsa1 п╡п╬п╥п╪п╬п╤п╫я▀п╣ п╬п©я├п╦п╦ bsa2, bsa3, Klz64
  std::string obscode;  // 2-я│п╦п╪п╡п╬п╩я▄п╫я▀п╧ п╨п╬п╢ п╬п╠я│п╣я─п╡п╟я┌п╬я─п╦п╦, п©п╬ я┐п╪п╬п╩я┤п╟п╫п╦я▌ "PO" - п÷я┐я┴п╦п╫я│п╨п╟я▐ п╬п╠я│п╣я─п╡п╟я┌п╬я─п╦я▐, п╟п╫я┌п╣п╫п╫п╟ п▒п║п░
  std::string rtype; // я┌п╦п© п©я─п╦п╣п╪п╫п╦п╨п╟, п©п╬ я┐п╪п╬п╩я┤п╟п╫п╦я▌ = "DPP1" (digital pulsar processor 1)
  std::string psrname; // я│п╬п╬я┌п╡. п©п╣я─п╣п╪п╣п╫п╫п╬п╧ name п╡ я└п╟п╧п╩п╣ п╢п╟п╫п╫я▀я┘ п▒п║п░ - п╫п╣ п╠п╬п╩п╣п╣ 10 я│п╦п╪п╡п╬п╩п╬п╡
  std::string datatype; //я┌п╦п© п╢п╟п╫п╫я▀я┘, п╬п╢п╫п╬п╠я┐п╨п╡п╣п╫п╫я▀п╧ п╨п╬п╢, п©п╬ я┐п╪п╬п╩я┤п╟п╫п╦я▌ = 'I' - п╦п╫я┌п╣п╫я│п╦п╡п╫п╬я│я┌я▄
  int npol; // я┤п╦я│п╩п╬ п©п╬п╩я▐я─п╦п╥п╟я├п╦п╧, п©п╬ я┐п╪п╬п╩я┤п╟п╫п╦я▌ п╢п╩я▐ п▒п║п░ = 1
  
  int year; //п╢п╟я┌п╟ п╫п╟п╠п╩я▌п╢п╣п╫п╦п╧, я│п╬п╬я┌п╡п╣я┌я│я┌п╡я┐п╣я┌ date п╡ я└п╟п╧п╩п╣ п╢п╟п╫п╫я▀я┘ п▒п║п░
  int month;
  int day;
  int hour; //п╡я─п╣п╪я▐ п╡ я┌п╣п╨я┐я┴п╣п╧ я┬п╨п╟п╩п╣ п╩п╬п╨п╟п╩я▄п╫п╬пЁп╬ я│я┌п╟п╫п╢п╟я─я┌п╟, я│п╬п╬я┌п╡п╣я┌я│я┌п╡я┐п╣я┌ time hh:mm:ss
  int min;
  int sec;
  int nsec; //time п╡ 100 п╫я│ 

  int utcday;
  int utcmonth;
  int utcyear;
  int utchour; //п╡я─п╣п╪я▐ п╡ п╦я│я┌п╦п╫п╬п╧ я┬п╨п╟п╩п╣ п╩п╬п╨п╟п╩я▄п╫п╬пЁп╬ я│я┌п╟п╫п╢п╟я─я┌п╟, dt_utc hh:mm:ss - п╢п╩я▐ п▒п║п░ я│п╬п╬я┌п╡п╣я┌я│я┌п╡я┐п╣я┌ UTC+3 я┤п╟я│п╟
  int utcmin;
  int utcsec;
  int utcnsec; //dt_utc п╡ 100 п╫я│ 
  
  float period; // п©п╣я─п╦п╬п╢ п©я┐п╩я▄я│п╟я─п╟ п╡ я│п╣п╨я┐п╫п╢п╟я┘
  int numpuls; // я┤п╦я│п╩п╬ п╦п╪п©я┐п╩я▄я│п╬п╡, я│п╩п╬п╤п╣п╫п╫я▀я┘ п╡ п╢п╟п╫п╫п╬п╪ я│я┐п╪п╪п╟я─п╫п╬п╪ п©я─п╬я└п╦п╩п╣
  
  float tau; // п╢п╦я│п╨я─п╣я┌ п╬я┌я│я┤п╣я┌п╟ п╡ п╪п╦п╩п╩п╦я│п╣п╨я┐п╫п╢п╟я┘, я│п╬п╬я┌п╡. п©п╣я─п╣п╪п╣п╫п╫п╬п╧ tay п╡ я└п╟п╧п╩п╣ п╢п╟п╫п╫я▀я┘ п▒п║п░
  int numpointwin; // я┤п╦я│п╩п╬ я┌п╬я┤п╣п╨ п╡ п╬п╨п╫п╣ <= int(period/tau)
  float freq; //  п©я─п╦п╡п╣п╢п╣п╫п╫п╟я▐ я┤п╟я│я┌п╬я┌п╟, п°п⌠я├, п╣я│п╩п╦ я│я┐п╪п╪п╦я─п╬п╡п╟п╩п╦ п©п╬ п╪п╣я─п╣ п╢п╦я│п©п╣я─я│п╦п╦ п╨ п©п╣я─п╡п╬п╪я┐ (я│п╟п╪п╬п╪я┐ п╫п╦п╥п╨п╬я┤п╟я│я┌п╬я┌п╫п╬п╪я┐) п╨п╟п╫п╟п╩я┐ п▒п║п░ = 109.584, п╣я│п╩п╦ п╨ п©п╬я│п╩п╣п╢п╫п╣п╪я┐, 512-п╪я┐ = 112.084, п╣я│п╩п╦ п╨ п╢я─я┐пЁп╬п╪я┐, я┌п╬ п╡я▀я┤п╦я│п╩я▐п╣я┌я│я▐ я│ я┐я┤п╣я┌п╬п╪ я┬п╦я─п╦п╫я▀ п╨п╟п╫п╟п╩п╟ = 2500/511 п╨п⌠я├
  
  std::vector<float> prfdata; // п©п╬я│п╩п╣п╢п╬п╡п╟я┌п╣п╩я▄п╫п╬я│я┌я▄ п╬я┌я│я┤п╣я┌п╬п╡ п╡ я│я┐п╪п╪п╟я─п╫п╬п╪, п©п╬я│п╩п╣ я┐я│я┌я─п╟п╫п╣п╫п╦я▐ п╪п╣я─я▀ п╢п╦я│п©п╣я─я│п╦п╦ п╦ п©я─п╦п╡п╣п╢п╣п╫п╦я▐ п╨ п╫я┐п╩п╣п╡п╬п╪я┐ я┐я─п╬п╡п╫я▌ (п╡я▀я┤п╦я┌п╟п╫п╦я▐ п╫я┐п╩п╣п╡п╬пЁп╬ я┐я─п╬п╡п╫я▐), п©я─п╬я└п╦п╩п╣. 
  
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
  
  float GetDM();
  int PrintFrequencyResponse(std::string outdir); // распечатать АЧХ (средний сигнал на частоте) в файл с именем outdir/<номер сеанса>.fr
  int PrintSumProfile(std::string outdir);  // распечатать суммарный профиль в файл с именем outdir/<номер сеанса>.prf
  int PrintPerBandSumProfile(std::string outdir); // распечатать суммарный профиль  для каждой из частот в файл с именем outdir/bands_<номер сеанса>.prf
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
