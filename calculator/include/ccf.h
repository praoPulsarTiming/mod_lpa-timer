#include <vector>
#include <iostream>
#include "PulseExtractor.h"
// #include "ConfigReader.h"

// осуществляет кросс-корреляцию данных с шаблоном записанным  в файле 'tpl' 
// в папке  tpldata_dir, определяет максимум ККФ и записывает в файлы
// itoa - МПИ в локальной шкале времени в стандарте itoa (TEMPO) и 
// kkf - результат кросс-корреляции для каждого файла
// по умолчанию данные в файлы itoa и kkf записываются последовательно во внешнем цикле

struct Tpl {
  std::string psrname; // имя пульсара соотв. переменной name в файле данных БСА - не более 10 символов
  std::string date;   
  //int year; //дата создания шаблона
  //int month;
  //int day;

  float freq; //  приведенная частота, МГц
  float tau; // дискрет между отсчетами шаблона в микросекунднах
  int numpoint; // длина шаблона
  
  std::vector<float> tpldata; // последовательность отсчетов профиля шаблона 
};

struct Skkf{
std::vector<float> kk; // значение 5 точек в окрестностях максимума дискретной ККФ
int numpoint; //число точек в векторе ККФ
float maxp;  // максимальная точка ККФ в микросекундах
float errmax; // ошибка вписывания в микросекундах
float snr; // отношение сигнал/шум в профиле
std::vector<float> kkfdata;
};

struct Itoa {
std::string psrname; // имя пульсара
long double TOAMJD; // МПИ в MJD и долях MJD с точностью до 12-13 знака
int iMJD;
float fMJD;
std::string sMJD;
float TOAerr; // ошибка определения МПИ в мкс
float freq; // частота приема в мегагерцах
float ddm; // поправка в меру дисперсии (для многочастотных наблюдений)
std::string obscode; // одно- или двухбуквенный код обсерватории
};

class Cor { 

public:
Cor(); // конструктор
~Cor();

// private:

//int DateToMjd (int Year, int Month, int Day) {};

public:

// кросс-корреляция данных, основная функция, вызывает остальные, возращает МПИ в долях MJD

void ccf(SumProfile finPulse, std::string rawdata_dir, std::string output_dir, std::string runs, std::string tplfile, float utcloc); 

Tpl Tplread(std::string rawdata_dir); // чтение шаблона из файла tpl в директории rawdata_dir

Tpl ScaleTpl(Tpl intpl, float tau); // приведение шаблона к новому дискрету дискрет tau - дискрет к которому надо привести


std::vector<float> dccf(Tpl tpl, SumProfile prf); // расчет нормированной на 1 диксретной ККФ кросс-корреляцией шаблона и профиля

// расчет MJD по времени старта + максимум ККФ в микросекундах

long double utc2mjd(SumProfile finPulse, float utcloc, float dtkkf);

// поиск максимума ККФ вписыванием полинома в 5 точек в окрестностях максимума возвращает положение максимума в долях отсчета от-но точки дисерктного максимума
float ApproxMax(float, float, float, float, float); 

float CalcErrorW50(SumProfile finPulse, float snr); // расчет ошибки вписывания шаблона

float SNR(SumProfile finPulse); // расчет отношения сигнал/шум в профиле

float CofM(std::vector<float> data, int datalength, float dt); // расчет центра масс вектора данных

int TOAWrite(std::string output_dir, Itoa sitoa); // запись МПИ в файл toa в формате itoa

int KKFWrite(std::string output_dir, Skkf kkf);   // запись данных по кросс-корреляции в файлы kkfdat - график и kkf - результат вписывания

int KKFdatWrite(std::string output_dir, Skkf kkf, std::string runs);   // запись данных по кросс-корреляции в файлы kkfdat - график и kkf - результат вписывания

std::string utc2mjds(SumProfile finPulse, float utcloc, float dtkkf); // возвращает MJD как строку

}; //end class Cor
