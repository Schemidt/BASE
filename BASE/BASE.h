/*!
\file
\brief ����� Sound

������ ���� ��������� ����� Sound � ��� �����������
*/

#pragma once
#include "Mono2channels.h"
#include "memory"

using namespace std;


template <class T>
/*!
\brief ������ ������������� �������
\details ������������ ������ � �������� ���������
*/
void Free(T &x)
{
	delete x;
	x = NULL;
}

#pragma pack ( push, 1 )

/*!
\struct WAVEHEADER

��������� ������������� ������������ wave �����
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
	double dbPerSec = 3;//�������� ����������/�������� 3��

	double delay(double nsGain, double deltaTime);
};

#ifndef Sound_h
#define Sound_h
/*!
\brief ����� ��������� ��������� ���������
\author �������� �.�.
\version 5.1
\date  02.03.2018

����� �������� ���������� � �������, ������������ ������ ���������
*/
class Sound {
public:

	static int sourcesInUse;//!< ���������� ��� �������� ���������� �������������� ���������� (�� ����� 256)
	static int effectSlotsInUse;//!< ���������� ��� �������� ���������� �������������� ������ ��� �������� (�� ����� 16)
	static int maxSources;//!< ���������� �������� ���������� ����������� ��������� ���������� �� ����������, 256 �����������
	static int maxSlots;//!< ���������� �������� ���������� ����������� ��������� ������ ��� �������� �� ����������, 16 �����������
	static double masterGain;//!< ���������� ����������� ���������
	static AL_SOUND_CHANNELS channelsSetup;//!< ������������ ������� - ��������� ������ (2.1,4.1,5.1,6.1,7.1)
	static double currentTime;//!< ���������� �������� ������� ����� ���������� �� ����� ������ �� USPO
	static double deltaTime;//!< ���������� ��� ������������ ��������� �������

	static double step; //!< ���
	static double tangaz;//!< ������
	static double derivTangaz;//!<�������� ��������� ������� (dtangaz/dt)
	static double hight;//!< ������
	static double velocityVectorXZ;//!< ��������� ������ �������� � ��������� vx-vz
	static double accelerationVectorXZ;//!< ��������� �� ������� �������� (dv/dt)
	static double velocityY;//!< ������������ ��������
	static double dashVectorXZ;//!< ����� (d2v/dt)
	static double accelerationVy;//!< ������������ ��������� (dvy/dt)
	static double derivStep;//!< �������� ��������� ����
	static double calcA;//!< ����� �����
	static double RedTurnAcc;//!< ��������� �������� ��������� (dturn/dt)

	static vector<double> vectorVy;//!<������ ��� �������� ����������� �� ������������ ��������
	static vector<double> vectorVXZ;//!<������ ��� �������� ����������� �� �������������� ��������
	static vector<double> vectorAccXZ;//!<������ ��� �������� ����������� �� ��������������� ���������
	static vector<double> vectorStep;//!<������ ��� �������� ����������� �� ����
	static vector<double> vectorTangaz;//!<������ ��� �������� ����������� �� �������
	static vector<double> vectorTime;//!<������ ��� �������� �����������
	static vector<double> vectorRedTurn;//!<������ ��� �������� ����������� �� �������� ���������

	static vector<double> vectorAvrEng1Turn;//!<������ ��� �������� �������� �������� ������� ���������
	static vector<double> vectorAvrEng2Turn;//!<������ ��� �������� �������� �������� ������� ���������
	static vector<double> vectorAvrRedTurn;//!<������ ��� �������� �������� �������� ���������
	static vector<double> vectorAvrStep;//!<������ ��� �������� �������� ����
	static vector<double> vectorAvrAtk;//!<������ ��� �������� ������� �����

	static double globalWindow;//!<���������� ������� ��� ������ �������� � ������� ��� �������� ��������

	unique_ptr<int[]> sourceStatus;//!< ���������� ��� ������� ���������
	unique_ptr<double[]> pitch;//!< ���������� ��� ��������� ������ ���� ����� ��������
	unique_ptr<double[]> gain;//!< ���������� ��� ��������� ��������� ����� ��������
	unique_ptr<float[]> offset;//!< 
	unique_ptr<ALuint[]> source;//!< ���������� ��� ���������
	unique_ptr<ALuint[]> buffer;//!< ���������� ��� �������
	unique_ptr<ALuint[]> effectSlot;//!< ���������� ��� ����� �������
	unique_ptr<ALuint[]> effect;//!< ���������� ��� �������
	unique_ptr<ALuint[]> filter;//!< ���������� ��� �������
	unique_ptr<string[]> filetoBuffer;//!< 
	unique_ptr<string[]> fileBuffered;//!<
	vector<string> ModeSequence = { "0","0","0" };
	string mode = "0";
	double switcher = 0;
	double crossFadeDuration = 0.5;//��������� �� ������� �� crossFadeDuration ������
	int id = 0;

	bool soundOn = 0;//!< ���������� ��� ����������� ��������� �����
	bool soundWork = 0;//!< ���������� ��� ����������� ��������� �����
	bool soundOff = 0;//!< ���������� ��� ����������� ��������� �����
	double lengthOn = 0;//!< ���������� ��� �������� ������������ ����� � ��������, ��� ������� ��� ����� ������� ��������
	double lengthOff = 0;//!< ���������� ��� �������� ������������ ����� � ��������, ��� ������� ��� ����� ��������� ��������
	float offsetOn = 0;
	float offsetOff = 0;

	vector<double> channel = { 1,1,0,0,0,0,0 };//!< ������ ��� ������������ ������ �����
	int sourceNumber = 2;//!< ���������� ��� �������� ���������� ���������� ������������ �������� ����� ��������
	int bufferNumber = 2;//!< ���������� ��� �������� ���������� �������� ������������ �������� ����� ��������
	int effectSlotNumber = 0;//!< ���������� ��� �������� ���������� ������ �������� ������������ �������� ����� ��������

	Sound();//!< ����������� �� ���������, ��� ������� � 1�� ����������
	Sound(const Sound &copy);//!< ����������� �����������*/
	Sound& operator =(const Sound &copy);
	Sound(int sources, int buffers, int effectslots);//!< ����������� ��� ������� � sources �����������, buffers �������� � effectslots ������� ��������
	~Sound();//!< ���������� (�� �������)

	/*!
	\brief ��������� ������������ WAVE �����
	\details ��������� ������������ ��������� WAVE �����
	\param[in] filename ��� �����
	\return ������������ WAVE �����
	*/
	double getLengthWAV(string filename);

	/*!
	\brief �������������� ��������������� �����
	\details <pre>������������� ������ ��������������� ����� ��� ��������� �������� ������ �������� ��� ���.�������:
	���� ������������ ������ ������ �������� (��� �������� - "1")
		 |
		\\|/
	���� ������ �������� (��� ��������� ����� ������������ ������ � �������� - "1")
		 |
		\\|/
	���� ���������� ������ �������� (��� �������� - "0")


	\image html phases.png "���� ��������"
	</pre>
	\param[in] status ������� ������
	\param[in] path_on ��������� �� ������ � ������� ����� ������������ ������ ������
	\param[in] path_w ��������� �� ������ � ������� ����� ������ ������
	\param[in] path_off ��������� �� ������ � ������� ����� ���������� ������
	\param[in] gain_mult ����������� ��������� ��� ��������
	\return ������ ��������� OpenAL
	*/
	virtual int play(bool status, string path_on, string path_w, string path_off, double gain_mult);

	/*!
	\brief ��������� ����� �������
	\details ������������� ������ ������ ��� ������������ ������ ��� ��������� ������������� ��������� ������ �
	��������� � ����� OpenAL
	\param[in] Buffer ������ ������
	\param[in] path ��� �����
	\param[in] channelsCount ������������ ��������� ������
	\param[in] channels ������ ��� ������
	\return 1 ���� �������, ����� 0
	*/
	int setBuffer(ALuint Buffer, string path, AL_SOUND_CHANNELS channelsCount, vector<double> channels);

	/*!
	\brief ��������� �����, ���������� � ��������� � ���������
	\details
	<pre>
	��������� ������, ��������� ������� �� file_path, ���������� � ���������,
	��������� �������� ��������� �������� ������� � �������� offset.
	������������ ����� ������������ ��� � �������
	</pre>
	\param[in] Buffer ������ ������
	\param[in] Source ������ ���������
	\param[in] offset ����� �� ������ �����, ���
	\param[in] file_path ������ ��� �����
	\return ������ ��������� OpenAL
	*/
	int setAndDeploySound(ALuint *Buffer, ALuint *Source, double offset, string file_path);

	double getAverange(string parameter, double seconds);

	private:

};

/*!
\brief ����� ������� ���������
\author �������� �.�.
\version 2.0
\date  06.03.2018

����� ������������ ������ ������� - ���������
*/
class Reductor : public Sound
{
public:

	bool hovering = 0;/*!< ���������� ��� ������� ������ �� ������� */
	string eq[2];/*!< ���������� ��� ����������� �������� ������ */
	string pinkNoise;/*!< ���������� ��� ����������� �������� ������ */
	string beats;/*!< ���������� ��� ����������� �������� ������ */
	string takeOff;/*!< ���������� ��� ����������� �������� ������ */
	Smoother sm;
	double flapOn = 0;
	double reperTurn = 0;
	string reperSet;

	Reductor();

	~Reductor();
	/*!
	\brief ���������� ������ ������ �����
	\details ���������� ������ ��������� ���������� ������������ �������� OpenAL
	\param[in] h ������ �������� �������������� ���������
	\param[in] sr ������ �������� ������� ��������� ���������
	\return ������ ��������� OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);
};

/*!
\brief ����� ������� ���������
\author �������� �.�.
\version 2.0
\date  06.03.2018

����� ������������ ������ ������� - ���������
*/
class Engine : public Sound
{
public:

	static int engCount;/*!< ���������� ��� ���������� ������������������ ���������� � ��������� */
	double phase;//!<���� ��� ����������, ����� �� ����� �� ���������(0-1, ������� �� 0.33 ��� ������� ������ �������, �.�. ��������� � 33% * n ��������� ������������)
	int engNum;//!<����� ���������
	string eq[2];/*!< ���������� ��� ����������� �������� ������ */
	double reperTurn = 0;
	string reperSet;

	Engine();

	~Engine();
	/*!
	\brief ���������� ������ ������ �����
	\details ���������� ������ ��������� ���������� ������������ �������� OpenAL
	\param[in] status_on ���������� ������� �������
	\param[in] status_off ���������� ������� ���������
	\param[in] parameter ������� ���������
	\param[in] h ������ �������� �������������� ���������
	\return ������ ��������� OpenAL
	*/
	int play(bool status_on, bool status_off, bool status_hp, double parameter, Helicopter h);
};

/*!
\brief ����� ������� ���������
\author �������� �.�.
\version 2.0
\date  06.03.2018

����� ������������ ������ ������� - ���������
*/
class Vsu : public Sound
{
public:

	double vsuDownTimer = 0;
	double vsuUpTimer = 0;
	double vsuTurnover = 0;

	string init = "NULL";

	string eq[2];/*!< ���������� ��� ����������� �������� ������ */

	Vsu();

	~Vsu();
	/*!
	\brief ���������� ������ ������ �����
	\details ���������� ������ ��������� ���������� ������������ �������� OpenAL
	\param[in] status_on ���������� ������� �������
	\param[in] status_off ���������� ������� ���������
	\param[in] parameter ������� ���������
	\param[in] h ������ �������� �������������� ���������
	\return ������ ��������� OpenAL
	*/
	int play(SOUNDREAD sr, Helicopter h);
};

/*!
\brief ����� ������� - ���� �������� �� ���
\author �������� �.�.
\version 2.0
\date  06.03.2018

����� ������������ ������ ����� �������� �� ��� � ��
*/
class Runway : public Sound
{
public:

	string eq;/*!< ���������� ��� ����������� �������� ������ */
	Smoother sm[2];

	Runway();

	/*!
	\brief ���������� ������ ������ �����
	\details ���������� ������ ��������� ���������� ������������ �������� OpenAL
	\param[in] h ������ �������� �������������� ���������
	\param[in] sr ������ �������� ������� ��������� ���������
	\return ������ ��������� OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);
};

/*!
\brief ����� ������� - ������� �����
\author �������� �.�.
\version 2.0
\date  06.03.2018

����� ������������ ������ ����� ������� �����
*/
class VintFlap : public Sound
{
public:

	string key[3];/*!< ���������� ��� ����������� �������� ������ */
	double flapOn = 0;

	VintFlap();

	/*!
	\brief ���������� ������ ������ �����
	\details ���������� ������ ��������� ���������� ������������ �������� OpenAL
	\param[in] h ������ �������� �������������� ���������
	\param[in] sr ������ �������� ������� ��������� ���������
	\return ������ ��������� OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);
};

/*!
\brief ����� ������� - ������ �����
\author �������� �.�.
\version 2.0
\date  06.03.2018

����� ������������ ������ ����� ������ �����
*/
class VintSwish : public Sound
{
public:

	double reperTurn = 0;
	string reperSet;
	VintSwish();

	/*!
	\brief ���������� ������ ������ �����
	\details ���������� ������ ��������� ���������� ������������ �������� OpenAL
	\param[in] h ������ �������� �������������� ���������
	\param[in] sr ������ �������� ������� ��������� ���������
	\return ������ ��������� OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);
};

/*!
\brief ����� ������� - ���
\author �������� �.�.
\version 2.0
\date  06.03.2018

����� ������������ ������ ����� ������� - ��� (������� ����������������� �������)
*/
class Skv : public Sound
{
public:

	string eq[2];/*!< ���������� ��� ����������� �������� ������ */
	string harm;
	Skv();

	/*!
	\brief ���������� ������ ������ �����
	\details ���������� ������ ��������� ���������� ������������ �������� OpenAL
	\param[in] h ������ �������� �������������� ���������
	\param[in] sr ������ �������� ������� ��������� ���������
	\return ������ ��������� OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);

};

#endif

/*!
\brief ����� "�����"
\details ���������� ����� �����
*/
class point {
public:
	double x;
	double y;

	point()
	{
		x = 0;
		y = 0;
	}

	point(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
};

/*!
\brief ���������� �������� �������
\details ���������� �������� ������� �������� ���������� (�������)
\param[in] p ����� �������
\param[in] n ���������� �����
\param[in] parameter �������� ���������� � ������� �����
\return ���������� �������� �������
*/
double getValue(double parameter, int n, point p, ...);

/*!
\brief ���������� �������� �������
\details ���������� �������� ������� �������� ���������� (�������)
\param[in] p1 ����� �������
\param[in] p2 ����� �������
\param[in] x �������� ���������� � ������� �����
\return ���������� �������� �������
*/
double getValue(point p1, point p2, double x);

/*!
\brief ���������� �������� �������
\details ���������� �������� ������� �������� ���������� (�������), � ������ ����������� �������� �������
\param[in] p1 ����� �������
\param[in] p2 ����� �������
\param[in] x �������� ���������� � ������� �����
\param[in] limit ������ �������
\param[in] w ���������� ������������ ������� ��� ������ ������
\return ���������� �������� �������
*/
double getValue(point p1, point p2, double x, double limit, string w);

/*!
\brief ���������� �������� �������
\details ���������� �������� ������� �������� ���������� (�������), � ������ ����������� �������� �������
\param[in] p1 ����� �������
\param[in] p2 ����� �������
\param[in] x �������� ���������� � ������� �����
\param[in] low_limit ������ ������ �������
\param[in] hi_limit ������� ������ �������
\return ���������� �������� �������
*/
double getValue(point p1, point p2, double x, double low_limit, double hi_limit);

/*!
\brief ���������� �������� �������
\details ���������� �������� ������� � ����� offset �������� ����������, �������� �����
\param[in] &value ������ ����� �������
\param[in] offset �������� ���������� � ������� �����
\return ���������� �������� �������
*/
double getParameterFromVector(vector<point> &value, double offset);

/*!
\brief ���������� ����� ������� ������ �������
\details ���������� ����� ������� ������ �������������� ������� � ������� ��������� ������
\param[in] &time ������������� ������ ����������
\param[in] offset �������� ���������� � ������� �����
\return ����� ������� ������
*/
int binSer(vector<double> &time, double offset);

/*!
\brief ���������� ����� ������� ������ �������
\details ���������� ����� ������� ������ �������������� ������� � ������� ��������� ������
\param[in] &time ������ �����
\param[in] offset �������� ���������� � ������� �����
\return ����� ������� ������
*/
int binSer(vector<point> &time, double offset);

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
�� �������� fadeLimit � riseLimit.
<pre>
���� ��������� ������ ���������� �� � �������� [0..1],
�� ����� ������� ��������� ��������� - �.�. ���������� ��������� ����������� ������
\image html parametricalCrossfade.png "CrossFade"
</pre>
\param[in] fadeGain ��������� ����������� ���������
\param[in] riseGain ��������� ������������ ���������
\param[in] parameter �������� ���������
\param[in] fadeLimit �������� ���������, ��� ������ ��������� ���������� ������ ������ 1
\param[in] riseLimit �������� ���������, ��� ������ ��������� ����������� ������ ������ 1
\param[in] gainMultiplicator ��������� ��������� �������
\return 1 ,���� ��������� ���������� ������ ��������� 0, � ����������� 1
*/
int parametricalCrossfade(double *fadeGain, double *riseGain, double parameter, double fadeLimit, double riseLimit);
/*!
\brief ���������� Crossfade
\details ������������ ������� ������� 1�� ������ � ������ ����� ���������� ��������� 1�� ������ � ��������� ��������� ������, �� ���� ��������� ���������
�� �������� fadeLimit � riseLimit.
<pre>
���� ��������� ������ ���������� �� � �������� [0..1],
�� ����� ������� ��������� ��������� - �.�. ���������� ��������� ����������� ������
\image html parametricalCrossfade.png "CrossFade"
</pre>
\param[in] fadeGain ��������� ����������� ���������
\param[in] riseGain ��������� ������������ ���������
\param[in] parameter �������� ���������
\param[in] fadeLimit �������� ���������, ��� ������ ��������� ���������� ������ ������ 1
\param[in] riseLimit �������� ���������, ��� ������ ��������� ����������� ������ ������ 1
\param[in] gainMultiplicator ��������� ��������� �������
\return 1 ,���� ��������� ���������� ������ ��������� 0, � ����������� 1
*/
int timeCrossfade(double *fadeGain, double *riseGain, double crossFadeDuration, double timer);
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
double interpolation(double x0, double fx0, double x1, double fx1, double x2, double fx2, double x);
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
double interpolation(double x0, double fx0, double x1, double fx1, double x);

double interpolation(point p1, point p2, double x);

double interpolation(point p1, point p2, point p3, double x);

/*!
\brief ���������� ������������ ���������� ��������� ����������

\return ������������ ���������� ��������� ����������, 0 � ������ ������
*/
int getMaxAvaliableSources();
/*!
\brief ��������� �����
\details ��������� ����� (���� ����� �������� ����� � ���� ����� ������������ ���������� ������ (��������������� ����������� ������) � ���������� �������� ������ �������� �����. ���� ���� ����� ����� ����� �������� (�� �180� �� +180�) � ����������� �� ����������� ������.
���� ��������� ����� �������� � ��������� �������� ����� �����(���������� �����), �� ���� ����� �������� ����� ��������� �������������.���� ��������� ����� �������� �� ������� ���� ������, �� ���� ����� ����������� �������������.)

��� ���������� �����:
\code
tangaz = tangaz * M_PI / 180.0;
calcA = atan(tan(tangaz) - (2 * deltaHigh) / ((velocityXPrevious + velocityVectorXZ) * cos(tangaz) * periodCalc));
calcA = calcA * 180 / M_PI;
\endcode

\param[in] velocityVectorXZ ��������� �������� � ������� ������ ������� t0
\param[in] velocityXPrevious ��������� �������� � ������ ������� t0 + periodCalc
\param[in] tangaz ������ � ������ ������� t0 + periodCalc
\param[in] deltaHigh ������� ������ ����� ���������� ���������
\param[in] periodCalc ������� �� ������� ����� ���������� ���������
\return �������� ����� ��� ������ ����������
*/
double attack(double velocityVectorXZ, double velocityXPrevious, double tangaz, double velocityY);
/*!
\brief ��������� ������� ��������
\details ��������� ������� ��������

\param[in] szExe ��� ��������
\return True ���� ������� ����������� � �������, ����� False
*/
bool IsProcessPresent(wchar_t * szExe);

/*!
\brief ��������� ����������� ��������� � ��������

\param[in] coef ����������� ���������
\return ��������
*/
double toDb(double coef);

/*!
\brief ��������� �������� � ����������� ���������

\param[in] db ��������
\return ����������� ���������
*/
double toCoef(double db);

/*!
\brief ��������� ������������ ����� �� ����������� ��������

nullsAfterInt - ��������� �������� 1/m, ��� m = 10^n, n �������� 0��� ����� ������� 
/code
roundFloat(3.34678, 0.01) -> 3.35
roundFloat(3.34678, 0.001) -> 3.347
/endcode

\param[in] x �����
\param[in] nullsAfterInt ��������
\return ����������� ��������
*/
double roundFloat(double x, double nullsAfterInt);





