#pragma once
#include "Mono2channels.h"
#include "memory"

using namespace std;

//������ �������� ����������
template <class T>
void Free(T &x)
{
	delete x;
	x = NULL;
}

#pragma pack ( push, 1 )
//��������� ��� �������� ����������� wave ������
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
\brief ����� ��������� ��������� ���������
\author �������� �.�.
\version 3.5

����� �������� ���������� � �������, ������������ ������ ���������
*/
class Sound {
public:

	static int sourcesInUse;//���������� ��� �������� ���������� �������������� ���������� (�� ����� 256)
	static int effectSlotsInUse;//���������� ��� �������� ���������� �������������� ������ ��� �������� (�� ����� 16)
	static int maxSources;//���������� �������� ���������� ����������� ��������� ���������� �� ����������, 256 �����������
	static int maxSlots;//���������� �������� ���������� ����������� ��������� ������ ��� �������� �� ����������, 16 �����������
	static float masterGain;//���������� ����������� ���������
	static AL_SOUND_CHANNELS channelsSetup;//������������ ������� - ��������� ������ (2.1,4.1,5.1,6.1,7.1)
	static float currentTime;//���������� �������� ������� ����� ���������� �� ����� ������ �� USPO
	static float deltaTime;//G��������� ��� ������������ ��������� �������

	static float step; //���
	static float tangaz;//������
	static float high;//������
	static float velocityX;//��������� ��������
	static float accelerationX;//��������� �� ��������� �������� (dvx/dt)
	static float velocityY;//������������ ��������
	static float dash;//����� (d2vx/dt)
	static float accelerationVy;//������������ ��������� (dvy/dt)
	static float derivStep;//�������� ��������� ����
	static float calcA;//����� �����
	static float RedTurnAcc;//��������� �������� ��������� (dturn/dt)

	static vector<double> vectorHigh, vectorVy, vectorVx, vectorAcc, vectorStep, vectorTime, vectorRedTurn;//������� ��� �������� �������� ���������� �� ��������� ����� (���������� �������� � main), ��� ������� �����������

	std::unique_ptr<int[]> sourceStatus;//���������� ��� ������� ���������
	std::unique_ptr<ALuint[]> source;//���������� ��� ���������
	std::unique_ptr<ALuint[]> buffer;//���������� ��� �������
	std::unique_ptr<ALuint[]> effectSlot;/*!< ���������� ��� ����� ������� */
	std::unique_ptr<ALuint[]> effect;/*!< ���������� ��� ������� */
	std::unique_ptr<ALuint[]> filter;/*!< ���������� ��� ������� */
	bool soundOn = 0;//���������� ��� ����������� ��������� �����
	bool soundWork = 0;//���������� ��� ����������� ��������� �����
	bool soundOff = 0;//���������� ��� ����������� ��������� �����
	float offsetOn = 0;//���������� ��� �������� ������� �� ������ ����� � ��������, ��� ������� ��� ����� ������� ��������
	float offsetOff = 0;//���������� ��� �������� ������� �� ������ ����� � ��������, ��� ������� ��� ����� ��������� ��������
	float lengthOn = 0;//���������� ��� �������� ������������ ����� � ��������, ��� ������� ��� ����� ������� ��������
	float lengthOff = 0;//���������� ��� �������� ������������ ����� � ��������, ��� ������� ��� ����� ��������� ��������
	float pitch = 1;//���������� ��� ��������� ������ ���� ����� ��������
	float gain = 1;//���������� ��� ��������� ��������� ����� ��������
	double channel[7] = { 1,1,0,0,0,0,0 };//������ ��� ������������ ������ �����
	int sourceNumber = 1;//���������� ��� �������� ���������� ���������� ������������ �������� ����� ��������
	int effectSlotNumber = 0;//���������� ��� �������� ���������� ������ �������� ������������ �������� ����� ��������

	Sound();//����������� �� ���������, ��� ������� � 1�� ����������
	Sound(int n, int ns);//����������� ��� ������� � n ����������� � ns ������� ��������
	~Sound();//���������� (�� �������)

	//���������� ������������ ��������� WAVE �����
	float getLengthWAV(string filename);

	//�������������� �������� ������ �� ���������� ������->������->���������� � ����������� �� �������� ��������� ������� � ����������� ���������,
	//��������� �������� ��������� �������� ���� ������ ������������ ����������� ��������� �������� (master_gain)
	int initializeSound(bool status, string path_on, string path_w, string path_off, float gain_mult);

	//������������� ������ ������ ��� ������������ ������ ��� ��������� ������������� ��������� ������
	int setBuffer(ALuint Buffer, string path, AL_SOUND_CHANNELS channelsCount, double *channels);

	//��������� ������, ��������� ������� �� file_path, ���������� � ���������, ��������� �������� ��������� �������� ������� � �������� offset
	int setAndDeploySound(ALuint *Buffer, ALuint *Source, float offset, string file_path);
	
};

//��������� ����� ���������
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

//��������� ����� ���������
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

//��������� ����� ���������
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

//��������� ����� �������
class VintFlap : public Sound
{
public:

	FILE *fflaps = nullptr;
	FILE *fderiv = nullptr;
	FILE *ffront = nullptr;
	double path = 0;
	float lowerFreqLimit;//������ ������� ������� �����
	float highterFreqLimit;//������� ������� ������� �����
	float freqCutoffFromTurns = 0;//������� �����
	float freqCutoffResult = 0;//������� �����
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

	float accelerationXBorder = 0.28;//��/�*�
	float velocityYBorder = -2;//��/�
	float dashBorder = -0.672;
	int flapIndicator = 0;
	float turnsGain = 0;//�������� �� ��������
	
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
	//������� ����
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
	//������� ����
	int Play(Helicopter h, SOUNDREAD sr);

};

class SKV : public Sound
{
public:

	char SKV_key[2];//!<���������� ��� ������������ �������� ������� � ���������

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
	//������� ����
	int Play(Helicopter h, SOUNDREAD sr);

};

#endif


/*!\brief ������� ������� OpenAL*/
void freeOpenAL();
/*!
\brief ���������� Pitch
\details ���������� Pitch �������� �������� parameter �� ���� filename,������� ������
�������� parameter ��� ������ ���������� ������������ ������ ������ offset, � ��������� ��������� parameter
��������: ��� �������� �������� ��������� ������� 60% �� 32 ������� ������������ �����, ������ �������� pitch = 60/59
�.�. � ���� ��������, ��� ������, �� 32 ������� �������� ���� 59% ��������
���� �������� � ������� �������� ������� �� ������ � ��������, �� ��������� - ����������� ������������ ������������
\param[in] offset ����� �� ������ ������������ ������
\param[in] filename ���� � ����� � ���������� ��������
\param[in] parameter �������� ���������
\return ���������� �������� ������ �����
*/
double getPitch(double offset, string filename, double parameter);
/*!
\brief ���������� Offset
\details ���������� ����� � �������� �� ������� ���������� ������� ������ �� ������ ������������ ����� �������� ���������� ������� ���������
, ��� �������� � ������ ������, � ������� �������� ������ �� ���������.
\param[in] pitch ��������� �������� ��������� ��������� � ��� �������� ��� ��������� ���������� �������
\param[in] filename ���� � ����� � ���������� ��������
\param[in] parameter �������� ���������
\return ����� ������� � ��������
*/
double getOffset(double pitch, string filename, double parameter);
/*!
\brief ���������� Crossfade
\details ������������ ������� ������� 1�� ������ � ������ ����� ���������� ��������� 1�� ������ � ��������� ��������� ������, �� ���� ��������� ���������
�� �������� limit1 � limit2.
���� ��������� ������ ���������� �� � �������� [0..1], �� ����� ������� ��������� ��������� - �.�. ���������� ��������� ����������� ������
\param[in] source id ����������� ���������
\param[in] source2 id ������������ ���������
\param[in] parameter �������� ���������
\param[in] limit1 �������� ���������, ��� ������ ��������� ���������� ������ ������ 1
\param[in] limit2 �������� ���������, ��� ������ ��������� ����������� ������ ������ 1
\param[in] mult ��������� ��������� �������
\return 1 ,���� ��������� ���������� ������ ��������� 0, � ����������� 1
*/
int crossFade(double *gf, double *gr, double parameter, double limit1, double limit2, double mult);
/*!\brief ���������� ��������� �� ������� ���������� EFX*/
void setEFXPointers();
/*!
\brief ��������� ���������� ������������
\details ��������� ������������ ��� fx � ����� x,�� ������� [x0,x2], ��� ������������ ������������ x0,x1,x2 � ��������� fx0,fx1,fx2, ���� x0=x1 ��� x1=x2
����������� �������� ������������

��� ���������� ������������:
\code
a2 = ((fx2 - fx0) / ((x2 - x0)*(x2 - x1))) - ((fx1 - fx0) / ((x1 - x0)*(x2 - x1)));
a1 = ((fx1 - fx0) / (x1 - x0)) - (a2*(x1 + x0));
a0 = fx0 - a1 * x0 - a2 * x0*x0;
return fx = a0 + a1 * x + a2*x*x;
\endcode

\param[in] x0 �������� 1�� �����
\param[in] fx0 �������� 1�� �����
\param[in] x1 �������� 2�� �����
\param[in] fx1 �������� 2�� �����
\param[in] x2 �������� 3�� �����
\param[in] fx2 �������� 3�� �����
\param[in] x �������� ������� fx
\return �������� fx � ����� x
*/
double squareInterpolation(double x0, double fx0, double x1, double fx1, double x2, double fx2, double x);
/*!
\brief ��������� �������� ������������
\details � ������ x>x1 � x<x0 - ���������� ��������� ��������

��� ���������� ������������:
\code
fx = fx0 + ((fx1 - fx0) / (x1 - x0))*(x - x0);
\endcode

\param[in] x0 �������� 1�� �����
\param[in] fx0 �������� 1�� �����
\param[in] x1 �������� 2�� �����
\param[in] fx1 �������� 2�� �����
\param[in] x �������� ������� fx
\return �������� fx � ����� x
*/
double lineInterpolation(double x0, double fx0, double x1, double fx1, double x);
/*!\brief ������� �������*/
void printProgrammStatus(SOUNDREAD srd);
/*!
\brief ���������� ������������ ���������� ��������� ����������

\return ������������ ���������� ��������� ����������, 0 � ������ ������
*/
int getMaxAvaliableSources();
//��������� ����� (���� ����� �������� ����� � ���� ����� ������������ ���������� ������ (��������������� ����������� ������) � ���������� �������� ������ �������� �����. ���� ���� ����� ����� ����� �������� (�� �180� �� +180�) � ����������� �� ����������� ������.
//���� ��������� ����� �������� � ��������� �������� ����� �����(���������� �����), �� ���� ����� �������� ����� ��������� �������������.���� ��������� ����� �������� �� ������� ���� ������, �� ���� ����� ����������� �������������.)
double attack(double velocityX, double velocityXPrevious, double tangaz, double deltaHigh, double periodCalc);

bool IsProcessPresent(wchar_t * szExe);

