/*!
\file
\brief Функции и классы общего назначения / Объявления
*/
#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include "fstream"
#include "string"
#include "conio.h"
#include "regex"
#include "memory"
#include "vector"
#include "math.h"
#include "windows.h"
#include "tlhelp32.h"

#include "AL\al.h"
#include "AL\alc.h"
#include "AL\alut.h"
#include "AL\alext.h"
#include "AL\efx.h"

using namespace std;

template <class T>
/*!
\brief Шаблон высвобождения объекта
\details Высвобождает объект и обнуляет указатель
*/
void Free(T &x)
{
	delete x;
	x = NULL;
}

#pragma pack ( push, 1 )

/*!
\struct WAVEHEADER

Структура интерпретации загруженного wave файла
*/
typedef struct
{
	char			szRIFF[4];
	long			lRIFFSize;
	char			szWave[4];
	char			szFmt[4];
	long			lFmtSize;
	WAVEFORMATEX	wfex;
	char			szData[4];
	long			lDataSize;
} WAVEHEADER;
#pragma pack ( pop )

class Smoother
{
public:

	bool firstAttempt = 1;
	double newDbGain = 0;
	double dbPerSec = 3;//Скорость нарастания/убывания 3дб

	double delay(double nsGain, double deltaTime);
};

/*!
\brief Класс "точка"
\details Определяет класс точки
*/
class point {
public:
	double x = 0;//Абсцисса
	double y = 0;//Ордината

	point()
	{

	}

	point(double xi, double yi)
	{
		x = xi;
		y = yi;
	}

	//!<Меняет x и y местами
	point swap()
	{
		double _x;
		double _y;

		_x = y;
		_y = x;
		y = _y;
		x = _x;

		return *this;
	}

	point& operator =(const point &copy);
};

/*!
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически (точками)
\param[in] p Точки функции
\param[in] n Количество точек
\param[in] parameter Значение переменной в искомой точке
\return Возвращает значение функции
*/
double getValue(double parameter, int n, point p, ...);

/*!
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически (точками)
\param[in] p1 Точка функции
\param[in] p2 Точка функции
\param[in] x Значение переменной в искомой точке
\return Возвращает значение функции
*/
double getValue(point p1, point p2, double x);

/*!
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически (точками), с учетом предельного значения функции
\param[in] p1 Точка функции
\param[in] p2 Точка функции
\param[in] x Значение переменной в искомой точке
\param[in] limit Предел функции
\param[in] w Переменная определяющая верхний или нижний предел
\return Возвращает значение функции
*/
double getValue(point p1, point p2, double x, double limit, string w);

/*!
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически (точками), с учетом предельного значения функции
\param[in] p1 Точка функции
\param[in] p2 Точка функции
\param[in] x Значение переменной в искомой точке
\param[in] low_limit Нижний предел функции
\param[in] hi_limit Верхний предел функции
\return Возвращает значение функции
*/
double getValue(point p1, point p2, double x, double low_limit, double hi_limit);

/*!
\brief Возвращает значение функции
\details Возвращает значение функции в точке offset заданной графически, вектором точек
\param[in] &value Вектор точек функции
\param[in] offset Значение переменной в искомой точке
\return Возвращает значение функции
*/
double getParameterFromVector(vector<point> &value, double offset);

//
vector <point> getVectorFromFile(string filename);

/*!
\brief Возвращает номер искомой ячейки вектора
\details Возвращает номер искомой ячейки упорядоченного вектора с помощью бинарного поиска
\param[in] &time Вектор точек
\param[in] offset Значение переменной в искомой точке
\return Номер искомой ячейки
*/
int binSer(vector<point> &points, double offset);

/*!
\brief Вычисляет квадратную интерполяцию
\details Вычисляет интерполяцию для fx в точке x,на отрезке [x0,x2], при определенных неодинаковых x0,x1,x2 и известных fx0,fx1,fx2, если x0=x1 или x1=x2
вычисляется линейная интерполяция

Код вычисления интерполяции:
\code
a2 = ((fx2 - fx0) / ((x2 - x0)*(x2 - x1))) - ((fx1 - fx0) / ((x1 - x0)*(x2 - x1)));
a1 = ((fx1 - fx0) / (x1 - x0)) - (a2*(x1 + x0));
a0 = fx0 - a1 * x0 - a2 * x0*x0;
return fx = a0 + a1 * x + a2*x*x;
\endcode

\param[in] x0 Абсцисса 1ой точки
\param[in] fx0 Ордината 1ой точки
\param[in] x1 Абсцисса 2ой точки
\param[in] fx1 Ордината 2ой точки
\param[in] x2 Абсцисса 3ой точки
\param[in] fx2 Ордината 3ой точки
\param[in] x Абсцисса искомой fx
\return Значение fx в точке x
*/
double interpolation(point p1, point p2, point p3, double x);

/*!
\brief Вычисляет линейную интерполяцию
\details В точках x>x1 и x<x0 - возвращает граничные значения

Код вычисления интерполяции:
\code
fx = fx0 + ((fx1 - fx0) / (x1 - x0))*(x - x0);
\endcode

\param[in] x0 Абсцисса 1ой точки
\param[in] fx0 Ордината 1ой точки
\param[in] x1 Абсцисса 2ой точки
\param[in] fx1 Ордината 2ой точки
\param[in] x Абсцисса искомой fx
\return Значение fx в точке x
*/
double interpolation(point p1, point p2, double x);

/*!
\brief Проверяет наличие процесса
\details Проверяет наличие процесса

\param[in] szExe имя процесса
\return True если процесс выполняется в системе, иначе False
*/
bool IsProcessPresent(wchar_t * szExe);

/*!
\brief Переводит коэффициент громкости в децибелы

\param[in] coef коэффициент громкости
\return децибелы
*/
double toDb(double coef);

/*!
\brief Переводит децибелы в коэффициент громкости

\param[in] db децибелы
\return коэффициент громкости
*/
double toCoef(double db);

/*!
\brief Округляет вещественное число до необходимой точности

nullsAfterInt - принимает значения 1/m, где m = 10^n, n колиство 0лей после запятой
/code
roundFloat(3.34678, 0.01) -> 3.35
roundFloat(3.34678, 0.001) -> 3.347
/endcode

\param[in] x число
\param[in] nullsAfterInt точность
\return округленное значение
*/
double roundFloat(double x, double nullsAfterInt);

/*!
\brief Вычисляет длительность WAVE файла
\details Вычисляет длительность несжатого WAVE файла
\param[in] filename имя файла
\return длительность WAVE файла
*/
double getLengthWAV(string filename);

/*!
\brief Загружает буфер данными
\details Структурирует массив данных для поканального вывода при различных конфигурациях устройств вывода и
загружает в буфер OpenAL
\param[in] Buffer Объект буфера
\param[in] path Имя файла
\param[in] channelsCount Конфигурация устройств вывода
\param[in] channels Каналы для вывода
\return 1 если успешно, иначе 0
*/
int setBuffer(ALuint Buffer, string path, vector<double> channels);

double getPitch(double maxPar, double currentPar, double supposedPar);