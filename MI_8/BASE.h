#include "AL\al.h"
#include "AL\alc.h"
#include "AL\alut.h"
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
	}

	~VintFlap()
	{
		
	}
	//Выводит звук
	int Play(Helicopter h, SOUNDREAD sr);
};

#endif


