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
//Структура для хранения загруженных wave файлов
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

	static int sourcesInUse;//Переменная для хранения количества использующихся источников (не более 256)
	static int effectSlotsInUse;//Переменная для хранения количества использующихся слотов для эффектов (не более 16)
	static int maxSources;//Переменная хранящая количество максимально возможных источников на устройстве, 256 максимально
	static int maxSlots;//Переменная хранящая количество максимально возможных слотов для эффектов на устройстве, 16 максимально
	static float masterGain;//Глобальный модификатор громкости
	static AL_SOUND_CHANNELS channelsSetup;//Конфигурация каналов - устройств вывода (2.1,4.1,5.1,6.1,7.1)
	static float currentTime;//Переменная хранящая текущее время полученное из общей памяти от USPO
	static float deltaTime;//Gеременная для отслеживания изменения времени

	static float step; //Шаг
	static float tangaz;//Тангаж
	static float high;//Высота
	static float velocityX;//приборная скорость
	static float accelerationX;//Ускорение по приборной скорости (dvx/dt)
	static float velocityY;//вертикальная скорость
	static float dash;//Рывок (d2vx/dt)
	static float accelerationVy;//Вертикальное ускорение (dvy/dt)
	static float derivStep;//Скорость изменения шага
	static float calcA;//Атака винта
	static float RedTurnAcc;//Ускорение оборотов редуктора (dturn/dt)

	static vector<double> vectorHigh, vectorVy, vectorVx, vectorAcc, vectorStep, vectorTime, vectorRedTurn;//Массивы для хранения значений переменных за некоторое время (определено условием в main), для расчета производных

	std::unique_ptr<int[]> sourceStatus;//Переменная для статуса источника
	std::unique_ptr<ALuint[]> source;//Переменная для источника
	std::unique_ptr<ALuint[]> buffer;//Переменная для буффера
	std::unique_ptr<ALuint[]> effectSlot;/*!< Переменная для слота эффекта */
	std::unique_ptr<ALuint[]> effect;/*!< переменная для эффекта */
	std::unique_ptr<ALuint[]> filter;/*!< переменная для эффекта */
	bool soundOn = 0;//Переменная для определения состояния звука
	bool soundWork = 0;//Переменная для определения состояния звука
	bool soundOff = 0;//Переменная для определения состояния звука
	float offsetOn = 0;//Переменная для хранения отступа от начала файла в секундах, как правило для файла запуска агрегата
	float offsetOff = 0;//Переменная для хранения отступа от начала файла в секундах, как правило для файла остановки агрегата
	float lengthOn = 0;//Переменная для хранения длительности файла в секундах, как правило для файла запуска агрегата
	float lengthOff = 0;//Переменная для хранения длительности файла в секундах, как правило для файла остановки агрегата
	float pitch = 1;//Переменная для параметра высоты тона звука агрегата
	float gain = 1;//Переменная для параметра громкости звука агрегата
	double channel[7] = { 1,1,0,0,0,0,0 };//массив для поканального вывода звука
	int sourceNumber = 1;//Переменная для хранения количества источников используемых объектом звука агрегата
	int effectSlotNumber = 0;//Переменная для хранения количества слотов эффектов используемых объектом звука агрегата

	Sound();//Конструктор по умолчанию, для объекта с 1им источником
	Sound(int n, int ns);//Конструктор для объекта с n источниками и ns слотами эффектов
	~Sound();//Деструктор (да неужели)

	//Возвращает длительность несжатого WAVE файла
	float getLengthWAV(string filename);

	//Инициализирует звуковой объект со структурой запуск->работа->выключение в зависимости от текущего состояния объекта и управлющего параметра,
	//последний параметр описывает усиление всех звуков относительно глобального параметра усиления (master_gain)
	int initializeSound(bool status, string path_on, string path_w, string path_off, float gain_mult);

	//Структурирует массив данных для поканального вывода при различных конфигурациях устройств вывода
	int setBuffer(ALuint Buffer, string path, AL_SOUND_CHANNELS channelsCount, double *channels);

	//Выгружает буффер, загружает данными из file_path, подключает к источнику, запускает источник используя параметр отступа в секундах offset
	int setAndDeploySound(ALuint *Buffer, ALuint *Source, float offset, string file_path);
	
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

	double averangeCalcPeriodAtk = 0;
	double vectorElemSummAtk = 0;
	double averangeAtk = 0;
	
	double averangeCalcPeriodStep = 0;
	double vectorElemSummStep = 0;
	double averangeStep = 0;

	double tay = 0;
	bool hovering = 0;

	vector<double> vector, vectorStep, vectorAtk;

	char eq_key[2] = { ' ',' ' };

	string filetoBuffer[2];
	string fileBuffered[2];
	float offset[2] = {0};

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

	string filetoBuffer[2];
	string fileBuffered[2];
	float offset[2] = { 0 };

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

	string filetoBuffer[2];
	string fileBuffered[2];
	float offset[2] = { 0 };

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

	string filetoBuffer[2];
	string fileBuffered[2];
	float offset[2] = { 0 };

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

	double averangeCalcPeriod = 0;
	double vectorElemSumm = 0;
	double averangeTurn = 0;

	double averangeCalcPeriodStep = 0;
	double vectorElemSummStep = 0;
	double averangeStep = 0;

	vector<double> vector, vectorStep;

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
double getPitch(double offset, string filename, double parameter);
/*!
\brief Возвращает Offset
\details Возвращает время в секундах на которое необходимо сделать отступ от начала проигрывания звука учитывая переходную функцию параметра
, его значение в данный момент, и отличие реальной кривой от эталонной.
\param[in] pitch Отношение значения реального параметра и его значения при получении переходной функции
\param[in] filename Путь к файлу с переходной функцией
\param[in] parameter Значение параметра
\return Время отступа в секундах
*/
double getOffset(double pitch, string filename, double parameter);
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
int crossFade(double *gf, double *gr, double parameter, double limit1, double limit2, double mult);
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
double squareInterpolation(double x0, double fx0, double x1, double fx1, double x2, double fx2, double x);
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
double lineInterpolation(double x0, double fx0, double x1, double fx1, double x);
/*!\brief Функция отладки*/
void printProgrammStatus(SOUNDREAD srd);
/*!
\brief Возвращает максимальное количество доступных источников

\return максимальное количество доступных источников, 0 в случае ошибки
*/
int getMaxAvaliableSources();
//Вычисляет атаку (Угол атаки несущего винта — угол между направлением воздушного потока (противоположным направлению полета) и плоскостью вращения втулки несущего винта. Этот угол может иметь любое значение (от —180° до +180°) в зависимости от направления полета.
//Если воздушный поток подходит к плоскости вращения винта снизу(автожирный режим), то угол атаки несущего винта считается положительным.Если воздушный поток набегает на несущий винт сверху, то угол атаки принимается отрицательным.)
double attack(double velocityX, double velocityXPrevious, double tangaz, double deltaHigh, double periodCalc);

bool IsProcessPresent(wchar_t * szExe);

