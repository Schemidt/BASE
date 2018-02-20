#pragma once
#include "Mono2channels.h"
#include "memory"

using namespace std;

//Шаблон очишения указателей
template <class T>
void Free(T &x)
{
	delete x;
	x = NULL;
}

#pragma pack ( push, 1 )
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

#ifndef Sound_h
#define Sound_h
/*!
\brief Класс состояния агрегатов вертолета
\author Самсонов А.В.
\version 3.5

Класс основных переменных и функций, определяющих работу программы
*/
class Sound {
public:

	static int sourcesInUse;
	static int effectSlotsInUse;
	static int maxSources;
	static int maxSlots;
	static float masterGain;//Глобальный модификатор громкости
	static AL_SOUND_CHANNELS channelsSetup;//Конфигурация каналов - устройств вывода (2.1,4.1,5.1,6.1,7.1)
	static float currentTime;//
	static float deltaTime;//переменная для отслеживания изменения времени

	static float step; //Шаг
	static float tangaz;//Тангаж
	static float high;//Высота
	static float velocityX;//приборная скорость
	static float accelerationX;//Ускорение по приборной скорости
	static float velocityY;//вертикальная скорость
	static float dash;//рывок
	static float accelerationVy;//Вертикальное ускорение
	static float derivStep;//Скорость изменения шага
	static float calcA;//Атака винта
	static float RedTurnAcc;//Ускорение оборотов редуктора

	static vector<double> vectorHigh, vectorVy, vectorVx, vectorAcc, vectorStep, vectorTime, vectorRedTurn;

	std::unique_ptr<int[]> sourceStatus;
	std::unique_ptr<ALuint[]> source;
	std::unique_ptr<ALuint[]> buffer;
	std::unique_ptr<ALuint[]> effectSlot;/*!< Переменная для слота эффекта */
	std::unique_ptr<ALuint[]> effect;/*!< переменная для эффекта */
	std::unique_ptr<ALuint[]> filter;/*!< переменная для эффекта */
	bool soundOn = 0;
	bool soundWork = 0;
	bool soundOff = 0;
	float offsetOn = 0;
	float offsetOff = 0;
	float lengthOn = 0;
	float lengthOff = 0;
	float pitch = 1;
	float gain = 1;
	double channel[7] = { 1,1,0,0,0,0,0 };//массив для поканального вывода звука
	int sourceNumber = 1;
	int effectSlotNumber = 0;

	Sound();
	Sound(int n, int ns);
	~Sound();
	//возвращает длительность несжатого WAVE файла
	float getLengthWAV(string filename);
	int initializeSound(bool status, string path_on, string path_w, string path_off, float gain_mult);
	int setBuffer(ALuint Buffer, string path, AL_SOUND_CHANNELS channelsCount, double *channels);
	
};

//Наследуем класс редуктора
class Reductor : public Sound
{
public:

	FILE *fred = nullptr;
	float lowFreqGain = 0;
	float mid1FreqGain = 0;
	float mid2FreqGain = 0;
	float highFreqGain = 0;

	float lowCutoffFreq = 0;
	float mid1CutoffFreq = 0;
	float mid2CutoffFreq = 0;
	float highCutoffFreq = 0;

	float pinkNoiseGain = 0;
	float highGain = 0;
	float stepGain = 0;
	float mid2FreqStepGain = 0;
	float velocityGain = 0;
	float lowFreqVelocityGain = 0;
	float accelerationGain = 0;
	float turnGain = 0;
	float highFreqTurnGain = 0;
	float multiplierStep = 1;

	float takeoffStep = 8;
	string pinkNoise;

	double outputPeriod = 0;
	double calcPeriod = 0;

	double averangeCalcPeriod = 0;
	double vectorElemSumm = 0;
	double averangeTurn = 0;
	
	double averangeCalcPeriodStep = 0;
	double vectorElemSummStep = 0;
	double averangeStep = 0;

	double tay = 0;
	bool hovering = 0;

	vector<double> vector, vectorStep;

	char eq_key[2] = { ' ',' ' };
	char red_key[2];//!<переменная для единоразовой загрузки буферов в редукторе

	Reductor() : Sound(3, 2)
	{
		remove("red.txt");
	}
	~Reductor()
	{
		
	}
	int Play(Helicopter h, SOUNDREAD sr);
};

//Наследуем класс двигателя
class Engine : public Sound
{
public:

	FILE *feng = nullptr;
	float highFreqTurnGain = 0;
	float takeoffStep = 0;
	float turnGain = 0;
	float stepGain = 0;

	float lowFreqGain = 0;
	float mid1FreqGain = 0;
	float mid2FreqGain = 0;
	float highFreqGain = 0;

	float lowCutoffFreq = 0;
	float mid1CutoffFreq = 0;
	float mid2CutoffFreq = 0;
	float highCutoffFreq = 0;

	double averangeCalcPeriod = 0;
	double outputPeriod = 0;
	double vectorElemSumm = 0;
	double averangeTurn = 0;

	vector<double> vector;

	char eng_key[2];
	char eq_key[2] = { ' ',' ' };

	Engine() : Sound(2, 2)
	{
		remove("eng.txt");
	}
	int Play(bool status_on, bool status_off, float parameter, SOUNDREAD sr, Helicopter h);
};

//Наследуем класс двигателя
class Runway : public Sound
{
public:

	string load[2];

	Runway() : Sound(2, 0)
	{

	}
	int Play(Helicopter h, SOUNDREAD sr);
};

//Наследуем класс хлопков
class VintFlap : public Sound
{
public:

	FILE *fflaps = nullptr;
	FILE *fderiv = nullptr;
	FILE *ffront = nullptr;
	double path = 0;
	float lowerFreqLimit;//Нижняя граница частоты среза
	float highterFreqLimit;//Верхняя граница частоты среза
	float freqCutoffFromTurns = 0;//Частота среза
	float freqCutoffResult = 0;//Частота среза
	float lowFreqGain = 0;
	float mid1FreqGain = 0;
	float mid2FreqGain = 0;
	float highFreqGain = 0;
	float lowCutoff = 0;
	float mid1Cutoff = 0;
	float mid2Cutoff = 0;
	float highCutoff = 0;
	float accelerationGain = 0;
	float hiSpeedGain = 0;
	float resFlapCGain = 0;

	float accelerationXBorder = 0.28;//мс/с*с
	float velocityYBorder = -2;//мс/с
	float dashBorder = -0.672;
	int flapIndicator = 0;
	float turnsGain = 0;//усиление от оборотов
	
	float outputPeriod = 0;
	float averangeCalcPeriod = 0;
	float vectorElemSumm = 0;
	float averangeTurn = 0;

	vector<double> vector;

	char flap_key[3];

	VintFlap() : Sound(3, 2) 
	{
		remove("flap.txt");
		remove("der.txt");
		remove("front.txt");
	}

	~VintFlap()
	{
		
	}
	//Выводит звук
	int Play(Helicopter h, SOUNDREAD sr);
};

class VintSwish : public Sound
{
public:

	char red_key[2];//!<переменная для единоразовой загрузки буферов в редукторе

	VintSwish() : Sound(2, 0)
	{
		
	}

	~VintSwish()
	{

	}
	//Выводит звук
	int Play(Helicopter h, SOUNDREAD sr);

};

class SKV : public Sound
{
public:

	char SKV_key[2];//!<переменная для единоразовой загрузки буферов в редукторе

	SKV() : Sound(2, 0)
	{

	}

	~SKV()
	{

	}
	//Выводит звук
	int Play(Helicopter h, SOUNDREAD sr);

};

#endif


/*!\brief Очищает объекты OpenAL*/
void freeOpenAL();
/*!
\brief Возвращает Pitch
\details Возвращает Pitch оценивая значения parameter из базы filename,которая хранит
значения parameter при записи аудиофайла относительно начала записи offset, и входящего параметра parameter
Например: при значении оборотов редуктора равного 60% на 32 секунде проигрывания файла, вернет значение pitch = 60/59
т.к. в базе значений, при записи, на 32 секунде редуктор имел 59% оборотов
если реальное и базовое значения отступа от начала в секундах, не совпадают - применяется квадратичная интерполяция
\param[in] offset Время от начала проигрывания записи
\param[in] filename Путь к файлу с переходной функцией
\param[in] parameter Значение параметра
\return Возвращает значение высоты звука
*/
float getPitch(float offset, string filename, float parameter);
/*!
\brief Возвращает Offset
\details Возвращает время в секундах на которое необходимо сделать отступ от начала проигрывания звука учитывая переходную функцию параметра
, его значение в данный момент, и отличие реальной кривой от эталонной.
\param[in] pitch Отношение значения реального параметра и его значения при получении переходной функции
\param[in] filename Путь к файлу с переходной функцией
\param[in] parameter Значение параметра
\return Время отступа в секундах
*/
float getOffset(float pitch, string filename, float parameter);
/*!
\brief Производит Crossfade
\details Осуществляет плавный переход 1ой записи в другую путем уменьшения громкости 1ой записи и повышения громкости другой, по мере изменения параметра
от значения limit1 к limit2.
Если громкость звуков изменяется не в границах [0..1], то стоит указать множитель громкости - т.е. предельная громкость начастающей записи
\param[in] source id Затухающего источника
\param[in] source2 id Нарастающего источника
\param[in] parameter Значение параметра
\param[in] limit1 Значение параметра, при уровне громкости затухающей записи равном 1
\param[in] limit2 Значение параметра, при уровне громкости Нарастающей записи равном 1
\param[in] mult Множитель громкости записей
\return 1 ,если громкость затухающей записи равняется 0, а нарастающей 1
*/
int crossFade(float *gf, float *gr, float parameter, float limit1, float limit2, float mult);
/*!\brief Определяет указатели на функции расширений EFX*/
void setEFXPointers();
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
float squareInterpolation(float x0, float fx0, float x1, float fx1, float x2, float fx2, float x);
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
float lineInterpolation(float x0, float fx0, float x1, float fx1, float x);
/*!\brief Функция отладки*/
void printProgrammStatus(SOUNDREAD srd);
/*!
\brief Возвращает максимальное количество доступных источников

\return максимальное количество доступных источников, 0 в случае ошибки
*/
int getMaxAvaliableSources();
//Вычисляет атаку (Угол атаки несущего винта — угол между направлением воздушного потока (противоположным направлению полета) и плоскостью вращения втулки несущего винта. Этот угол может иметь любое значение (от —180° до +180°) в зависимости от направления полета.
//Если воздушный поток подходит к плоскости вращения винта снизу(автожирный режим), то угол атаки несущего винта считается положительным.Если воздушный поток набегает на несущий винт сверху, то угол атаки принимается отрицательным.)
float attack(float velocityX, float velocityXPrevious, float tangaz, float deltaHigh, float periodCalc);

bool IsProcessPresent(wchar_t * szExe);

