/*!
\file
\brief �������� ���� ��������� 

������ ���� ��������� ��� �������� ��������� � ���������� �� ������
*/
#include "conio.h"
#include "stdlib.h"
#include "iostream"
#include "stdio.h"
#include "string.h"
#include "Windows.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "fstream"
#include "string"
#include "map"
#include "regex"
#include "tlhelp32.h"
#include "memory"

#include "AL\al.h"
#include "AL\alc.h"
#include "AL\alut.h"
#include "AL\alext.h"
#include "AL\efx.h"

#include "Shared.h"
#include "realtime.h"

#include "EFXptr.h"
#include "Sound.h"
#include "Helicopter.h"
#include "BASE.h"

using namespace std;

#define MAX_AUX_SLOTS 16

SOUNDREAD soundread;/*!< ���������� ������ soundread ��� �������� ����������� ��������� */

int Sound::maxSlots;/*!< ���������� ������������������ ������������ ������ ���������� ,������� ����� ������������� ������������*/
int Sound::maxSources;/*!< ���������� ������������������ ������������ ������ ���������� ,������� ����� ������������� ������������*/
float Sound::masterGain = 1;//!< ����� ��������� �����
int Sound::sourcesInUse = 0;
int Sound::effectSlotsInUse = 0;
float Sound::currentTime = 0;//
float Sound::deltaTime = 0;//���������� ��� ������������ ��������� �������
float Sound::step = 0; //��� (�������� ���������� �������� ����������)
float Sound::tangaz = 0;
float Sound::high = 0;
float Sound::velocityX = 0;//��������� ��������
float Sound::accelerationX = 0;//
float Sound::velocityY = 0;//������������ ��������
float Sound::dash = 0;
float Sound::accelerationVy = 0;
float Sound::derivStep = 0;
float Sound::calcA = 0;

AL_SOUND_CHANNELS Sound::channelsSetup = AL_SOUND_CHANNELS_2;//������������ ������� �����
vector<double> vectorHigh, vectorVy, vectorVx, vectorAcc, vectorStep, vectorTime;
float window = 1;//��� ���������� ������������ ����������� ����� ��������� �������� �� ������� 
float periodCalc = 0;//���������� ��� ��������� �������� ������� ����������, ����� ��� ������� ����� window

/*!\brief �������� ������� ���������*/
int main(int argc, char *argv[])
{
	float timerPodk = 0;
	float timerNar8 = 0;
	float timerNar13 = 0;
	int counterNar8 = 0;
	int counterNar13 = 0;
	float vsuDownTimer = 0;
	float vsuUpTimer = 0;
	//�������� ��������� �� ������� EFX
	setEFXPointers();
	vector <string> helicoptersNames = { "mi_8_mtv5","mi_8_amtsh","mi_26","mi_28","ka_226","ansat","ka_27","ka_29"};
	string model;
	Helicopter helicopter;/*!< ���������� ������ Helicopter ��� �������� ���������� ���������� ��������� */
	if (argc > 1)// ���� �������� ���������, �� argc ����� ������ 1(� ����������� �� ���-�� ����������)
	{
		for (size_t i = 0; i < helicoptersNames.size(); i++)
		{
			if (regex_match(argv[1], regex("^(" + helicoptersNames[i] + ")")))
			{
				model = argv[1];
			}
		}
		if (model.empty())
		{
			cout << " Unknown argument..." << endl;
			helicopter.setParam("ka_29");
		}
		else
		{
			helicopter.setParam(model);
		}
	}
	else
	{
		helicopter.setParam("ka_29");
	}
	system("cls");
	cout << " Using " << helicopter.modelName << endl;
	helicopter.setPath(helicopter.modelName + "/");
	ALCdevice *device;
	ALCcontext *context;

	ALint attribs[4] = { 0 };
	attribs[0] = ALC_MAX_AUXILIARY_SENDS;
	attribs[1] = MAX_AUX_SLOTS;

	Sound::maxSources = getMaxAvaliableSources(); //�������� ������������ ���������� ���������� ������� ����� ������������ �� ������ ����������
	cout << " " << Sound::maxSources << " - Sources avaliable" << endl;
	Sound::maxSlots = MAX_AUX_SLOTS;

	device = alcOpenDevice(NULL);
	if (device == NULL)
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		return AL_FALSE;
	}
	context = alcCreateContext(device, attribs);
	if (context == NULL)
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		alcCloseDevice(device);
		return AL_FALSE;
	}
	if (!alcMakeContextCurrent(context))
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		alcDestroyContext(context);
		alcCloseDevice(device);
		return AL_FALSE;
	}
	if (!shaInit())// ������������� ����� ������ 
	{
		cout << "shared memory initialization error..." << endl;
		return 0;
	}
	InitRealTime(1);//������������ "��������" ����� � ��������� � 1��
	//��������� �� ������� ������������ ��������� ���������� �����
	Sound *vsu = nullptr;
	Sound *vsuHp = nullptr;
	Sound *vsuKran = nullptr;
	Sound *engHp[2] = { nullptr };
	Sound *engPomp[2] = { nullptr };
	Sound *redCrash = nullptr;
	Sound *accum = nullptr;
	Sound *accumTr = nullptr;
	Sound *pts = nullptr;
	Sound *ptsTone = nullptr;
	Sound *tr36 = nullptr;
	Sound *tr115 = nullptr;
	Sound *nip = nullptr;
	Sound *girovert = nullptr;
	Sound *podk1 = nullptr;
	Sound *podk2 = nullptr;
	Sound *perek1 = nullptr;
	Sound *perek2 = nullptr;
	Sound *ko50 = nullptr;
	Sound *skv = nullptr;
	Sound *fire1 = nullptr;
	Sound *fire2 = nullptr;
	Sound *consTank = nullptr;
	Sound *pstat = nullptr;
	Sound *runway = nullptr;
	Runway *runwayMi8 = nullptr;
	Sound *airNoise = nullptr;
	Sound *crash = nullptr;
	Sound *vintSwish = nullptr;
	Sound *vintBrake = nullptr;
	VintFlap *vintFlap = nullptr;
	Sound *brake = nullptr;
	Sound *rain = nullptr;
	Sound *sturm = nullptr;
	Sound *igla = nullptr;
	Sound *rocket = nullptr;
	Sound *ppu = nullptr;
	Sound *upk = nullptr;
	Sound *nar8[20] = { nullptr };
	Sound *nar13[5] = { nullptr };
	Engine *eng[2] = { nullptr };
	Reductor *red = nullptr;
	Sound *beep = nullptr;
	Sound *undefined0 = nullptr;
	Sound *undefined1 = nullptr;
	Sound *vintFlapUniversal = nullptr;
	Sound *kranKolc = nullptr;
	Sound *vpryam = nullptr;
	SOUNDREAD localdata = soundread;//��������� ����� ������ � USPO �����
	Sound::currentTime = localdata.time;
	

	//���������� ��� ����� ��������� � ����������� �����
	while (1)
	{
		if (IsProcessPresent(L"USPO_3.0.exe"))//���������� �������� � ������ �� ����� ������ USPO
		{
			//�������� ������ �� ����� ������ �� ��������� ���������
			localdata = soundread;
			//���� ������ ������� ��������� ������ ��� ����� ����������� � 0 - ������������ ���������� ����������
			if (localdata.p_model_stop)
			{
				vectorHigh.clear();
				vectorVy.clear();
				vectorVx.clear();
				vectorAcc.clear();
				vectorStep.clear();
				vectorTime.clear();
				if (eng[0])
				{
					Free(eng[0]);
					//eng[0]->averangeCalcPeriod = 0;
					//eng[0]->vector.clear();
				}
				if (eng[1])
				{
					//eng[1]->averangeCalcPeriod = 0;
					//eng[1]->vector.clear();
					Free(eng[1]);
				}
				if (red)
				{
					//red->averangeCalcPeriod = 0;
					//red->averangeCalcPeriodStep = 0;
					//red->vector.clear();
					//red->vectorStep.clear();
					Free(red);
				}
				if (vintFlap)
				{
					Free(vintFlap);
				}
				periodCalc = 0;
				Sound::currentTime = localdata.time;
			}
			else
			{
				if (Sound::currentTime == 0)
					Sound::currentTime = localdata.time;
				//��������� ��������� ������� � �������� ����� ������ ���������
				Sound::deltaTime = localdata.time - Sound::currentTime;
				Sound::currentTime = localdata.time;
				Sound::masterGain = localdata.master_gain;
				Sound::tangaz = localdata.osadki;//������ (�������� ���������� �������� ��� �������)
				Sound::velocityX = localdata.v;//��������� ��������
				Sound::high = localdata.styk_hv;
				Sound::step = localdata.ny; //��� (�������� ���������� �������� ����������)
				//���� �� ������ ������� ��������� ������ - ��������� ����������
				while (periodCalc >= window && vectorTime.size() > 2)
				{
					if (!vectorTime.empty())
					{
						vectorHigh.erase(vectorHigh.begin());
						vectorVy.erase(vectorVy.begin());
						vectorVx.erase(vectorVx.begin());
						vectorAcc.erase(vectorAcc.begin());
						vectorStep.erase(vectorStep.begin());
						vectorTime.erase(vectorTime.begin());
					}
					periodCalc = Sound::currentTime - vectorTime.front();
				}
				vectorTime.push_back(Sound::currentTime);
				vectorHigh.push_back(Sound::high);
				vectorVy.push_back(Sound::velocityY);
				vectorVx.push_back(Sound::velocityX);
				vectorAcc.push_back(Sound::accelerationX);
				vectorStep.push_back(Sound::step);
				if (!vectorTime.empty())
				{
					//�����������
					periodCalc = Sound::currentTime - vectorTime.front();
					if (periodCalc > 0)
					{
						Sound::velocityY = (Sound::high - vectorHigh.front()) / periodCalc;
						Sound::accelerationVy = (Sound::velocityY - vectorVy.front()) / periodCalc;
						Sound::accelerationX = (Sound::velocityX - vectorVx.front()) / periodCalc;
						Sound::dash = (Sound::accelerationX - vectorAcc.front()) / periodCalc;
						Sound::derivStep = (Sound::step - vectorStep.front()) / periodCalc;
						Sound::calcA = attack(Sound::velocityX, vectorVx.front(), Sound::tangaz, Sound::high - vectorHigh.front(), periodCalc);
					}
				}
				//���
				if (helicopter.vsuFactor)//���� ��� �������� � ������
				{
					if (localdata.p_vsu_zap | localdata.p_vsu_ostanov)
					{
						if (!vsu)
						{
							vsu = new Sound;
						}
					}
					if (vsu)
					{
						vsu->lengthOff = vsu->getLengthWAV(helicopter.fullName["vsu_off"]);
						vsu->lengthOn = vsu->getLengthWAV(helicopter.fullName["vsu_on"]);
						vsu->offsetOff = vsu->lengthOff * (1 - localdata.vsu_obor / 100.);//�������� ������ � �������� ������ ��������
						vsu->offsetOn = vsu->lengthOn * localdata.vsu_obor / 100.;//�������� ������ � �������� ������ ��������
																				  //��� 27 29 � 8
						if (helicopter.modelName == "ka_27" || helicopter.modelName == "ka_29" || helicopter.modelName == "mi_8_mtv5" || helicopter.modelName == "mi_8_amtsh")
						{
							//�������� ��� ��� ���������� ���������, �� �� ������� ���������
							if (localdata.reduktor_gl_obor > 0)
							{
								vsu->gain = squareInterpolation(0, 0.7, 2.5, 0.85, 5, 1, vsuUpTimer);
								vsuUpTimer += Sound::deltaTime;
								vsuDownTimer = 0;
							}
							else
							{
								if (localdata.p_eng1_zap || localdata.p_eng1_hp || localdata.p_eng2_zap || localdata.p_eng2_hp)
								{
									vsu->gain = squareInterpolation(0, 1, 0.25, 0.85, 0.5, 0.7, vsuDownTimer);
									vsuDownTimer += Sound::deltaTime;
									vsuUpTimer = 0;
								}
								else
								{
									vsu->gain = squareInterpolation(0, 0.7, 2.5, 0.85, 5, 1, vsuUpTimer);
									vsuUpTimer += Sound::deltaTime;
									vsuDownTimer = 0;
								}
							}
						}
						vsu->initializeSound(localdata.p_vsu_zap, helicopter.fullName["vsu_on"], helicopter.fullName["vsu_w"], helicopter.fullName["vsu_off"], helicopter.vsuFactor);
					}
					if (vsu && vsu->sourceStatus[0] != AL_PLAYING)
					{
						Free(vsu);
					}

					//��� ��
					if (localdata.p_vsu_hp)
					{
						if (!vsuHp)
						{
							vsuHp = new Sound;
						}
					}
					if (vsuHp)
					{
						if (localdata.vsu_obor > 0 & localdata.vsu_obor < 35.)
						{
							vsuHp->lengthOff = vsuHp->getLengthWAV(helicopter.fullName["vsu_hp_off"]);
							vsuHp->lengthOn = vsuHp->getLengthWAV(helicopter.fullName["vsu_hp_on"]);
							vsuHp->offsetOff = vsuHp->lengthOff * (1 - localdata.vsu_obor / 35.);//�������� ������ � �������� ������ ��������
							vsuHp->offsetOn = vsuHp->lengthOn * localdata.vsu_obor / 35.;//�������� ������ � �������� ������ ��������
						}
						if (localdata.p_vsu_zap | localdata.p_vsu_ostanov)//���� ��� �������� - �������� ���������
						{
							vsuHp->initializeSound(localdata.p_vsu_hp, "NULL", "NULL", "NULL", helicopter.vsuHpFactor);
						}
						else
						{
							vsuHp->initializeSound(localdata.p_vsu_hp, helicopter.fullName["vsu_hp_on"], helicopter.fullName["vsu_hp_w"], helicopter.fullName["vsu_hp_off"], helicopter.vsuHpFactor);
						}
						if (vsuHp->sourceStatus[0] != AL_PLAYING)
						{
							Free(vsuHp);
						}
					}
				}
				//�����������
				if (helicopter.accumFactor)
				{
					if ("ansat" != helicopter.modelName)//226
					{
						if (localdata.rez_2)//������� �������� �������
							if (!accum)//���� ������ �� ������ 
								accum = new Sound;//������� ������
						if (accum)//���� ������ ������ - ���������� ���
						{
							accum->initializeSound(localdata.rez_2, "NULL", helicopter.fullName["accum"], "NULL", helicopter.accumFactor);//������������� ���� - ���������� ��������� ����� � play
							if (accum->sourceStatus[0] != AL_PLAYING)//������� �������� �������
								Free(accum);//������� ������
						}
					}
					if ("ka_226" == helicopter.modelName || "ansat" == helicopter.modelName)//226
					{
						if (localdata.rez_2)//������� �������� �������
							if (!accumTr)//���� ������ �� ������ 
								accumTr = new Sound;//������� ������
						if (accumTr)//���� ������ ������ - ���������� ���
						{
							accumTr->initializeSound(localdata.rez_2, "NULL", helicopter.fullName["accum_tr"], "NULL", helicopter.accumFactor);//������������� ���� - ���������� ��������� ����� � play
							if (accumTr->sourceStatus[0] != AL_PLAYING)//������� �������� �������
								Free(accumTr);//������� ������
						}
					}
				}
				//���
				if (helicopter.ptsFactor)
				{
					if (localdata.p_pts)//������� �������� �������
						if (!pts)//���� ������ �� ������ 
							pts = new Sound;//������� ������
					if (pts)//���� ������ ������ - ���������� ���
					{
						pts->initializeSound(localdata.p_pts, helicopter.fullName["pts_on"], helicopter.fullName["pts_w"], helicopter.fullName["pts_off"], helicopter.ptsFactor);//������������� ���� - ���������� ��������� ����� � play
						if (pts->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(pts);//������� ������
					}
				}
				//��� ���
				if (helicopter.ptsToneFactor)
				{
					//��� ���
					if (localdata.p_pts)//������� �������� �������
						if (!ptsTone)//���� ������ �� ������ 
							ptsTone = new Sound;//������� ������
					if (ptsTone)//���� ������ ������ - ���������� ���
					{
						ptsTone->initializeSound(localdata.p_pts, "NULL", helicopter.fullName["pts_tone"], "NULL", helicopter.ptsToneFactor);//������������� ���� - ���������� ��������� ����� � play
						if (ptsTone->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(ptsTone);//������� ������
					}
				}
				//��
				if (helicopter.vpryam)
				{
					//��
					if (localdata.p_vu1)//������� �������� �������
						if (!vpryam)//���� ������ �� ������ 
							vpryam = new Sound;//������� ������
					if (vpryam)//���� ������ ������ - ���������� ���
					{
						vpryam->initializeSound(localdata.p_vu1, "NULL", helicopter.fullName["vpryam"], "NULL", helicopter.vpryam);//������������� ���� - ���������� ��������� ����� � play
						if (vpryam->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(vpryam);//������� ������
					}
				}
				//���� ������������� 36� ������������ �� �����
				if (helicopter.tr36Factor)
				{
					if (localdata.p_trans_36_osn || localdata.p_trans_36_rez)//������� �������� �������
						if (!tr36)//���� ������ �� ������ 
							tr36 = new Sound;//������� ������
					if (tr36)//���� ������ ������ - ���������� ���
					{
						tr36->initializeSound(localdata.p_trans_36_osn || localdata.p_trans_36_rez, helicopter.fullName["tr36_on"], helicopter.fullName["tr36_w"], helicopter.fullName["tr36_off"], helicopter.tr36Factor);//������������� ���� - ���������� ��������� ����� � play
						if (tr36->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(tr36);//������� ������
					}
				}
				//���� ������������� 115� ������������ �� �����
				if (helicopter.tr115Factor)
				{
					if (localdata.p_po500)//������� �������� �������
						if (!tr115)//���� ������ �� ������ 
							tr115 = new Sound;//������� ������
					if (tr115)//���� ������ ������ - ���������� ���
					{
						tr115->initializeSound(localdata.p_po500, helicopter.fullName["tr115_on"], helicopter.fullName["tr115_w"], helicopter.fullName["tr115_off"], helicopter.tr115Factor);//������������� ���� - ���������� ��������� ����� � play
						if (tr115->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(tr115);//������� ������
					}
				}
				//���
				if (helicopter.nipFactor)
				{
					if (localdata.rez_3)//������� �������� �������
						if (!nip)//���� ������ �� ������ 
							nip = new Sound;//������� ������
					if (nip)//���� ������ ������ - ���������� ���
					{
						nip->initializeSound(localdata.rez_3, "NULL", helicopter.fullName["nip_tone"], "NULL", helicopter.nipFactor);
						if (nip->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(nip);//������� ������
						else
						{
							float p1 = 1, p2 = 1;
							//���� �������� ��, �� ������ ���� ���������� �� 1.5%
							if (localdata.p_vu1 && "mi_26" == helicopter.modelName)
							{
								p1 = 1.015;
							}
							if (!localdata.p_vu1 && "mi_26" == helicopter.modelName)
							{
								p1 = 1;
							}
							//��������� � ������ ��� ����������� ��� �� ����
							if (localdata.p_vsu_hp || localdata.p_vsu_zap)
							{

								nip->offsetOn += Sound::deltaTime;

								//_____  
								//  1  \____   
								//     0.985\  ______
								//		0.977\/
								//		 0.946
								//	    |1c |1c|

								//����� ������
								p2 = squareInterpolation(0, 1, 1.5, 0.946, 2, 0.977, nip->offsetOn);

							}
							//��������� � ������ ��� ����������� ��� �� ����
							if (!localdata.p_vsu_hp && !localdata.p_vsu_zap)
							{
								nip->offsetOn = 0;
								p2 = 1;
							}

							nip->pitch = 1 * p1 * p2;//
						}

					}
				}
				//�������������
				if (helicopter.girovertFactor)
				{
					if (localdata.rez_3)//������� �������� �������
						if (!girovert)//���� ������ �� ������ 
							girovert = new Sound;//������� ������
					if (girovert)//���� ������ ������ - ���������� ���
					{
						girovert->lengthOn = girovert->getLengthWAV(helicopter.fullName["girovert_on"]);
						if (girovert->pitch < 1)
							girovert->offsetOn = girovert->lengthOn * girovert->pitch;//�������� ������ � �������� ������ ��������
						girovert->initializeSound(localdata.rez_3, helicopter.fullName["girovert_on"], helicopter.fullName["girovert_w"], helicopter.fullName["girovert_w"], helicopter.ptsFactor);
						if (girovert->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(girovert);//������� ������
						else
						{
							if (girovert->soundOn)
								girovert->pitch = 1;
							if (girovert->soundOff)
							{
								alSourcei(girovert->source[0], AL_LOOPING, AL_TRUE);
								girovert->pitch -= Sound::deltaTime*0.0024;//��������� ���� �� 0.0024 � ���
																		   //���� Pitch == 0 - ���� ������, �������� ����� ���������
								if (girovert->pitch <= 0.8)
									girovert->initializeSound(localdata.rez_3, "NULL", "NULL", "NULL", helicopter.ptsFactor);

							}
						}


					}
				}
				//�������� �������
				//���� ������ �������� ������������ �� �����
				if (helicopter.pumpLeftFactor || helicopter.pumpRightFactor)
				{
					if (localdata.p_nasos_podk_1)//������� �������� �������
						if (!podk1)//���� ������ �� ������ 
							podk1 = new Sound;//������� ������
					if (podk1)//���� ������ ������ - ���������� ���
					{
						//podk1->position = PosLeft;
						podk1->channel[0] = 1;
						podk1->channel[1] = 0;
						if ("ka_226" == helicopter.modelName)
						{
							if (localdata.p_eng1_zap | localdata.p_eng1_hp | localdata.p_eng2_hp | localdata.p_eng2_zap)
							{
								timerPodk += Sound::deltaTime;
								podk1->pitch = squareInterpolation(0, 1, 1, 0.85, 5, 0.88, timerPodk);
							}
							else
							{
								timerPodk = 0;
								podk1->pitch = 1;
							}
							podk1->initializeSound(localdata.p_nasos_podk_1, helicopter.fullName["podk_l_on"], helicopter.fullName["podk_l_w"], helicopter.fullName["podk_l_off"], helicopter.pumpLeftFactor);//������������� ���� - ���������� ��������� ����� � play
						}
						else
							podk1->initializeSound(localdata.p_nasos_podk_1, helicopter.fullName["podk_l_on"], helicopter.fullName["podk_l_w"], "NULL", helicopter.pumpLeftFactor);//������������� ���� - ���������� ��������� ����� � play
						if (podk1->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(podk1);//������� ������
					}

					if (localdata.p_nasos_podk_2)//������� �������� �������
						if (!podk2)//���� ������ �� ������ 
							podk2 = new Sound;//������� ������
					if (podk2)//���� ������ ������ - ���������� ���
					{
						//podk2->position = PosRight;
						podk2->channel[0] = 0;
						podk2->channel[1] = 1;
						if ("ka_226" == helicopter.modelName)
						{
							if (localdata.p_eng1_zap | localdata.p_eng1_hp | localdata.p_eng2_hp | localdata.p_eng2_zap)
							{
								timerPodk += Sound::deltaTime;
								podk2->pitch = squareInterpolation(0, 1, 1, 0.85, 5, 0.88, timerPodk);
							}
							else
							{
								timerPodk = 0;
								podk2->pitch = 1;
							}
							podk2->initializeSound(localdata.p_nasos_podk_2, helicopter.fullName["podk_r_on"], helicopter.fullName["podk_r_w"], helicopter.fullName["podk_r_off"], helicopter.pumpRightFactor);//������������� ���� - ���������� ��������� ����� � play
						}
						else
							podk2->initializeSound(localdata.p_nasos_podk_2, helicopter.fullName["podk_r_on"], helicopter.fullName["podk_r_w"], "NULL", helicopter.pumpRightFactor);//������������� ���� - ���������� ��������� ����� � play
						if (podk2->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(podk2);//������� ������
					}
				}
				//���� ���� ����������� ������������ �� �����
				if (helicopter.circlingCraneFactor)
				{
					if (localdata.p_kran_kolcev)//������� �������� �������
						if (!kranKolc)//���� ������ �� ������ 
							kranKolc = new Sound;//������� ������
					if (kranKolc)//���� ������ ������ - ���������� ���
					{
						//perek1->position = PosLeft;
						kranKolc->channel[0] = 1;
						kranKolc->channel[1] = 1;
						kranKolc->initializeSound(localdata.p_kran_kolcev, helicopter.fullName["kran_circle"], "NULL", "NULL", helicopter.circlingCraneFactor);//������������� ���� - ���������� ��������� ����� � play
						if (kranKolc->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_kolcev)//������� �������� �������
							Free(kranKolc);//������� ������
					}
				}
				//���� ����������� ����� ������������ �� �����
				if (helicopter.cutoffCraneFactor)
				{
					if (localdata.p_kran_perekr_1)//������� �������� �������
						if (!perek1)//���� ������ �� ������ 
							perek1 = new Sound;//������� ������
					if (perek1)//���� ������ ������ - ���������� ���
					{
						//perek1->position = PosLeft;
						perek1->channel[0] = 1;
						perek1->channel[1] = 0;
						perek1->initializeSound(localdata.p_kran_perekr_1, helicopter.fullName["perekr_l"], "NULL", "NULL", helicopter.cutoffCraneFactor);//������������� ���� - ���������� ��������� ����� � play
						if (perek1->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_perekr_1)//������� �������� �������
							Free(perek1);//������� ������
					}

					if (localdata.p_kran_perekr_2)//������� �������� �������
						if (!perek2)//���� ������ �� ������ 
							perek2 = new Sound;//������� ������
					if (perek2)//���� ������ ������ - ���������� ���
					{
						//perek2->position = PosRight;
						perek2->channel[0] = 0;
						perek2->channel[1] = 1;
						perek2->initializeSound(localdata.p_kran_perekr_2, helicopter.fullName["perekr_r"], "NULL", "NULL", helicopter.cutoffCraneFactor);//������������� ���� - ���������� ��������� ����� � play
						if (perek2->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_perekr_2)//������� �������� �������
							Free(perek2);//������� ������
					}
				}
				//���� ��� ������������ �� �����
				if (helicopter.skvFactor)
				{
					if (localdata.p_skv_on)//������� �������� �������
						if (!skv)//���� ������ �� ������ 
							skv = new Sound;//������� ������
					if (skv)//���� ������ ������ - ���������� ���
					{
						skv->initializeSound(localdata.p_skv_on, helicopter.fullName["skv_on"], helicopter.fullName["skv_w"], helicopter.fullName["skv_off"], helicopter.skvFactor);//������������� ���� - ���������� ��������� ����� � play
						if (skv->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(skv);//������� ������
					}
				}
				//���� ��-50 ������������ �� �����
				if (helicopter.ko50Factor)
				{
					if (localdata.rez_9)//������� �������� �������
						if (!ko50)//���� ������ �� ������ 
							ko50 = new Sound;//������� ������
					if (ko50)//���� ������ ������ - ���������� ���
					{
						ko50->initializeSound(localdata.rez_9, helicopter.fullName["ko50_on"], helicopter.fullName["ko50_w"], helicopter.fullName["ko50_off"], helicopter.ko50Factor);//������������� ���� - ���������� ��������� ����� � play
						if (ko50->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(ko50);//������� ������
					}
				}
				//���� ����� ���������� ����
				if (helicopter.consumTankFactor)
				{
					if (localdata.rez_4)//������� �������� �������
						if (!consTank)//���� ������ �� ������ 
							consTank = new Sound;//������� ������
					if (consTank)//���� ������ ������ - ���������� ���
					{
						consTank->initializeSound(localdata.rez_4, helicopter.fullName["cons_tank_on"], helicopter.fullName["cons_tank_w"], "NULL", helicopter.consumTankFactor);//������������� ���� - ���������� ��������� ����� � play
						if (consTank->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(consTank);//������� ������
					}
				}
				//�������� ����
				if (helicopter.fireCraneFactor)
				{
					if (localdata.p_kran_poj_l)//������� �������� �������
						if (!fire1)//���� ������ �� ������ 
							fire1 = new Sound;//������� ������
					if (fire1)//���� ������ ������ - ���������� ���
					{
						//fire1->position = PosLeft;
						fire1->channel[0] = 1;
						fire1->channel[1] = 0;
						fire1->initializeSound(localdata.p_kran_poj_l, helicopter.fullName["kran_fire_l"], "NULL", "NULL", helicopter.fireCraneFactor);//������������� ���� - ���������� ��������� ����� � play
						if (fire1->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_poj_l)//������� �������� �������
							Free(fire1);//������� ������
					}

					if (localdata.p_kran_poj_r)//������� �������� �������
						if (!fire2)//���� ������ �� ������ 
							fire2 = new Sound;//������� ������
					if (fire2)//���� ������ ������ - ���������� ���
					{
						//fire2->position = PosRight;
						fire2->channel[0] = 0;
						fire2->channel[1] = 1;
						fire2->initializeSound(localdata.p_kran_poj_r, helicopter.fullName["kran_fire_r"], "NULL", "NULL", helicopter.fireCraneFactor);//������������� ���� - ���������� ��������� ����� � play
						if (fire2->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_poj_r)//������� �������� �������
							Free(fire2);//������� ������
					}
				}
				//�������� �������
				if (helicopter.pumpStationFactor)
				{
					if (localdata.p_nasos)//������� �������� �������
						if (!pstat)//���� ������ �� ������ 
							pstat = new Sound;//������� ������
					if (pstat)//���� ������ ������ - ���������� ���
					{
						pstat->initializeSound(localdata.p_nasos, helicopter.fullName["pstat_on"], helicopter.fullName["pstat_w"], helicopter.fullName["pstat_off"], helicopter.ko50Factor);//������������� ���� - ���������� ��������� ����� � play
						if (pstat->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(pstat);//������� ������
					}
				}
				//�������� �� ��� � ��
				//���� ����� �������� �� ��� �������� � ������ �����
				if (helicopter.runwayFactor)
				{
					if (helicopter.modelName == "mi_8_amtsh" || helicopter.modelName == "mi_8_mtv5")
					{
						if (localdata.v != 0 && Sound::high <= 0)//������� �������� �������
							if (!runwayMi8)//���� ������ �� ������ 
								runwayMi8 = new Runway;//������� ������
						if (runwayMi8)//���� ������ ������ - ���������� ���
						{
							runwayMi8->Play(helicopter, localdata);//������������� ���� - ���������� ��������� ����� � play
							if (localdata.v == 0 || Sound::high > 0)//������� �������� �������
								Free(runwayMi8);//������� ������
						}
					}
					else
					{
						if (localdata.v != 0 && Sound::high <= 0)//������� �������� �������
							if (!runway)//���� ������ �� ������ 
								runway = new Sound;//������� ������
						if (runway)//���� ������ ������ - ���������� ���
						{
							runway->initializeSound(localdata.p_na_vpp && localdata.v != 0, "NULL", helicopter.fullName["runway"], "NULL", helicopter.runwayFactor);//������������� ���� - ���������� ��������� ����� � play
							if (localdata.v == 0 || Sound::high > 0)//������� �������� �������
								Free(runway);//������� ������
							else
							{
								runway->gain = Sound::masterGain * lineInterpolation(0, 0, 13.8, 1, abs(localdata.v)) * helicopter.runwayFactor;
							}
						}
					}
				}
				//���������������� ���
				//����� ���������������� ����� �������� � ������
				if (helicopter.airNoiseFactor)
				{
					if (localdata.v > 20)//������� �������� �������
						if (!airNoise)//���� ������ �� ������ 
							airNoise = new Sound;//������� ������
					if (airNoise)//���� ������ ������ - ���������� ���
					{
						airNoise->initializeSound(localdata.v > 20, "NULL", helicopter.fullName["air"], "NULL", helicopter.airNoiseFactor);//������������� ���� - ���������� ��������� ����� � play
						if (airNoise->sourceStatus[0] != AL_PLAYING && localdata.v <= 20)//������� �������� �������
							Free(airNoise);//������� ������
						else
						{
							if (localdata.v > 20 && localdata.v < 70)
								airNoise->gain = Sound::masterGain * helicopter.airNoiseFactor * ((localdata.v / 50.) - .4);
							else
								airNoise->gain = Sound::masterGain * helicopter.airNoiseFactor;
						}
					}
				}
				//��������
				//���� ���� ������������ � ������������ ������� � ������
				if (1)
				{
					if (localdata.p_crash)//������� �������� �������
						if (!crash)//���� ������ �� ������ 
							crash = new Sound;//������� ������
					if (crash)//���� ������ ������ - ���������� ���
					{
						crash->initializeSound(localdata.p_crash, helicopter.fullName["crash"], "NULL", "NULL", 1);//������������� ���� - ���������� ��������� ����� � play
						if (crash->sourceStatus[0] != AL_PLAYING && !localdata.p_crash)//������� �������� �������
							Free(crash);//������� ������
					}
				}
				//����
				if (helicopter.vintSwishFactor)
				{
					if (localdata.reduktor_gl_obor >= helicopter.redTurnoverMg2)//������� �������� �������
						if (!vintSwish)//���� ������ �� ������ 
							vintSwish = new Sound;//������� ������
					if (vintSwish)//���� ������ ������ - ���������� ���
					{
						vintSwish->initializeSound(localdata.reduktor_gl_obor >= helicopter.redTurnoverMg2, "NULL", helicopter.fullName["vint_hi"], "NULL", helicopter.vintSwishFactor);//������������� ���� - ���������� ��������� ����� � play
						if (vintSwish->sourceStatus[0] != AL_PLAYING && localdata.reduktor_gl_obor < helicopter.redTurnoverMg2)//������� �������� �������
							Free(vintSwish);//������� ������
						else
						{
							//�������� ������ ���� � ����������� �� �������� ��������� � ������ ������
							vintSwish->pitch = localdata.reduktor_gl_obor / helicopter.redTurnoverAvt;
							//��������� ������ ����� �� �������� ��������� ���� �������� ������ ���� ���������
							//������� ����� ���� � ������� 3� ��������
							if (localdata.reduktor_gl_obor <= helicopter.redTurnoverAvt)
								vintSwish->gain = lineInterpolation(helicopter.redTurnoverAvt, 1, helicopter.redTurnoverMg2, 0, localdata.reduktor_gl_obor);
						}
					}
				}
				//���� ���� ������� ����� ������� � ������
				if (helicopter.vintBrakeFactor)
				{
					if (localdata.tormoz_vint)//������� �������� �������
						if (!vintBrake)//���� ������ �� ������ 
							vintBrake = new Sound;//������� ������
					if (vintBrake)//���� ������ ������ - ���������� ���
					{
						vintBrake->initializeSound(localdata.tormoz_vint, "NULL", helicopter.fullName["vint_torm"], "NULL", helicopter.vintBrakeFactor);//������������� ���� - ���������� ��������� ����� � play
						if (vintBrake->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(vintBrake);//������� ������
						else
						{
							vintBrake->pitch = localdata.reduktor_gl_obor / 30.;
						}
					}
				}
				//���� ���� ������� ����� ������� � ������
				if (helicopter.vintFlapFactor)
				{
					if (helicopter.modelName == "ka_29" || helicopter.modelName == "mi_8_mtv5" || helicopter.modelName == "mi_8_amtsh")
					{
						if (localdata.v >= 8.3 && localdata.styk_hv > 0)//������� �������� �������
							if (!vintFlap)//���� ������ �� ������ 
								vintFlap = new VintFlap;//������� ������
						if (vintFlap)//���� ������ ������ - ���������� ���
						{
							vintFlap->Play(helicopter, localdata);//������������� ���� - ���������� ��������� ����� � play
							if (localdata.v < 8.3 || localdata.styk_hv <= 0)//������� �������� �������
								Free(vintFlap);//������� ������
						}
					}
					else if (helicopter.modelName == "ka_27")
					{
						if (localdata.styk_hv > 0)//������� �������� �������
							if (!vintFlap)//���� ������ �� ������ 
								vintFlap = new VintFlap;//������� ������
						if (vintFlap)//���� ������ ������ - ���������� ���
						{
							vintFlap->Play(helicopter, localdata);//������������� ���� - ���������� ��������� ����� � play
							if (localdata.styk_hv <= 0)//������� �������� �������
								Free(vintFlap);//������� ������
						}
					}
					else
					{
						if (localdata.rez_10)//������� �������� �������
							if (!vintFlapUniversal)//���� ������ �� ������ 
								vintFlapUniversal = new Sound;//������� ������
						if (vintFlapUniversal)//���� ������ ������ - ���������� ���
						{
							vintFlapUniversal->initializeSound(localdata.rez_10, "NULL", helicopter.fullName["vint_flap"], "NULL", helicopter.vintFlapFactor);//������������� ���� - ���������� ��������� ����� � play
							if (vintFlapUniversal->sourceStatus[0] != AL_PLAYING)//������� �������� �������
								Free(vintFlapUniversal);//������� ������
						}
					}

				}
				//���� ������� �������� �� �����
				if (helicopter.chassisBrakePumpFactor)
				{
					if (localdata.p_tormoz)//������� �������� �������
						if (!brake)//���� ������ �� ������ 
							brake = new Sound;//������� ������
					if (brake)//���� ������ ������ - ���������� ���
					{
						if (helicopter.modelName == "ka_27" | helicopter.modelName == "ka_29")
						{
							brake->initializeSound(localdata.p_tormoz, "NULL", helicopter.fullName["brake"], "NULL", helicopter.chassisBrakePumpFactor);//������������� ���� - ���������� ��������� ����� � play
						}
						else
						{
							brake->initializeSound(localdata.p_tormoz, helicopter.fullName["brake"], "NULL", helicopter.fullName["poff"], helicopter.chassisBrakePumpFactor);//������������� ���� - ���������� ��������� ����� � play
						}
						if (brake->sourceStatus[0] != AL_PLAYING && !localdata.p_tormoz)//������� �������� �������
							Free(brake);//������� ������
					}
				}
				//�����
				if (1)
				{
					if (localdata.p_rain)//������� �������� �������
						if (!rain)//���� ������ �� ������ 
							rain = new Sound;//������� ������
					if (rain)//���� ������ ������ - ���������� ���
					{
						rain->initializeSound(localdata.p_rain, "NULL", helicopter.fullName["rain"], "NULL", helicopter.rainFactor);//������������� ���� - ���������� ��������� ����� � play
						if (rain->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(rain);//������� ������
					}
				}
				//���� ���� ��������� ������ ������� � ������
				if (helicopter.rocketHitFactor)
				{
					if (localdata.p_rocket_hit)//������� �������� �������
						if (!rocket)//���� ������ �� ������ 
							rocket = new Sound;//������� ������
					if (rocket)//���� ������ ������ - ���������� ���
					{
						rocket->initializeSound(localdata.p_rocket_hit, helicopter.fullName["rocket"], "NULL", "NULL", helicopter.rocketHitFactor);//������������� ���� - ���������� ��������� ����� � play
						if (rocket->sourceStatus[0] != AL_PLAYING && !localdata.p_rocket_hit)//������� �������� �������
							Free(rocket);//������� ������
					}
				}
				//���� ���8 ������� �� �����
				if (helicopter.rocketNar8Factor)
				{
					if (localdata.p_nar_s8)//������� �������� �������
					{
						timerNar8 += Sound::deltaTime;
						for (int i = 0; i < 20; i++)
						{
							//������������� ���� �������� 1�� ���8 ������ 0.05� - 20 ���
							//������ 20 �������� ������� ����� ��������� ��� ������ ������� ������
							//���������� ����������� ������� ����� ���������� ������, ������� � ���� �������
							//������� �� �������������� ��������� � ������ ������ ����� �������� ���8
							if (timerNar8 >= 0.05*i & counterNar8 < 20 & i >= counterNar8)
							{
								if (!nar8[i])//���� ������ �� ������ 
									nar8[i] = new Sound;//������� ������
								if (nar8[i])//���� ������ ������ - ���������� ���
								{
									nar8[i]->initializeSound(localdata.p_nar_s8, helicopter.fullName["nar8"], "NULL", "NULL", helicopter.rocketNar8Factor);//������������� ���� - ���������� ��������� ����� � play
									nar8[i]->soundOn = 0;
								}
								counterNar8++;
							}
						}
						if (counterNar8 == 20)
						{
							counterNar8 = 0;
							timerNar8 = 0;
						}
					}
					else
					{
						for (int i = 0; i < 20; i++)
						{
							if (nar8[i])
							{
								alGetSourcei(nar8[i]->source[0], AL_SOURCE_STATE, &nar8[i]->sourceStatus[0]);
								if (nar8[i]->sourceStatus[0] != AL_PLAYING)//������� �������� �������
									Free(nar8[i]);//������� ������
							}
						}
						timerNar8 = 0;
						counterNar8 = 0;
					}
				}
				//���� ���13 ������� �� �����
				if (helicopter.rocketNar13Factor)
				{
					if (localdata.p_nar_c13)//������� �������� �������
					{
						timerNar13 += Sound::deltaTime;
						for (int i = 0; i < 5; i++)
						{
							//������������� ���� �������� 1�� ���13 ������ 0.05� - 20 ���
							//������ 20 �������� ������� ����� ��������� ��� ������ ������� ������
							//���������� ����������� ������� ����� ���������� ������, ������� � ���� �������
							//������� �� �������������� ��������� � ������ ������ ����� �������� ���13
							if (timerNar13 >= 0.12*i & counterNar13 < 5 & i >= counterNar13)
							{
								if (!nar13[i])//���� ������ �� ������ 
									nar13[i] = new Sound;//������� ������
								if (nar13[i])//���� ������ ������ - ���������� ���
								{
									nar13[i]->initializeSound(localdata.p_nar_c13, helicopter.fullName["nar13"], "NULL", "NULL", helicopter.rocketNar13Factor);//������������� ���� - ���������� ��������� ����� � play
									nar13[i]->soundOn = 0;
								}
								counterNar13++;
							}
						}
						if (counterNar13 == 5)
						{
							counterNar13 = 0;
							timerNar13 = 0;
						}
					}
					else
					{
						for (int i = 0; i < 5; i++)
						{
							if (nar13[i])
							{
								alGetSourcei(nar13[i]->source[0], AL_SOURCE_STATE, &nar13[i]->sourceStatus[0]);
								if (nar13[i]->sourceStatus[0] != AL_PLAYING)//������� �������� �������
									Free(nar13[i]);//������� ������
							}
						}
						timerNar13 = 0;
						counterNar13 = 0;

					}
				}
				//���� ��� ������� �� �����
				if (helicopter.ppuFactor)
				{
					if (localdata.p_spo_ppu)//������� �������� �������
						if (!ppu)//���� ������ �� ������ 
							ppu = new Sound;//������� ������
					if (ppu)//���� ������ ������ - ���������� ���
					{
						ppu->initializeSound(localdata.p_spo_ppu, "NULL", helicopter.fullName["ppu"], "NULL", helicopter.ppuFactor);//������������� ���� - ���������� ��������� ����� � play
						if (ppu->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(ppu);//������� ������
					}
				}
				//���� ��� ������� �� �����
				if (helicopter.upkFactor)
				{
					if (localdata.p_spo_upk)//������� �������� �������
						if (!upk)//���� ������ �� ������ 
							upk = new Sound;//������� ������
					if (upk)//���� ������ ������ - ���������� ���
					{
						upk->initializeSound(localdata.p_spo_upk, "NULL", helicopter.fullName["upk"], "NULL", helicopter.upkFactor);//������������� ���� - ���������� ��������� ����� � play
						if (upk->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(upk);//������� ������
					}
				}
				//���� �� ����� ������� �� �����
				if (helicopter.rocketSturmFactor)
				{
					if (localdata.p_ur_ataka)//������� �������� �������
						if (!sturm)//���� ������ �� ������ 
							sturm = new Sound;//������� ������
					if (sturm)//���� ������ ������ - ���������� ���
					{
						sturm->initializeSound(localdata.p_ur_ataka, helicopter.fullName["sturm"], "NULL", "NULL", helicopter.rocketSturmFactor);//������������� ���� - ���������� ��������� ����� � play
						if (sturm->sourceStatus[0] != AL_PLAYING && !localdata.p_ur_ataka)//������� �������� �������
							Free(sturm);//������� ������
					}
				}
				//���� �� ���� ������� �� �����
				if (helicopter.rocketIglaFactor)
				{
					if (localdata.p_ur_igla)//������� �������� �������
						if (!igla)//���� ������ �� ������ 
							igla = new Sound;//������� ������
					if (igla)//���� ������ ������ - ���������� ���
					{
						igla->initializeSound(localdata.p_ur_igla, helicopter.fullName["igla"], "NULL", "NULL", helicopter.rocketIglaFactor);//������������� ���� - ���������� ��������� ����� � play
						if (igla->sourceStatus[0] != AL_PLAYING && !localdata.p_ur_igla)//������� �������� �������
							Free(igla);//������� ������
					}
				}
				//��������
				if (helicopter.redFactor)
				{
					if (localdata.p_reduktor_gl_crash)//������� �������� �������
						if (!redCrash)//���� ������ �� ������ 
							redCrash = new Sound;//������� ������
					if (redCrash)//���� ������ ������ - ���������� ���
					{
						redCrash->initializeSound(localdata.p_reduktor_gl_crash, "NULL", helicopter.fullName["red_crash"], "NULL", helicopter.redFactor);//������������� ���� - ���������� ��������� ����� � play
						if (redCrash->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(redCrash);//������� ������
						else
							redCrash->pitch = localdata.reduktor_gl_obor / helicopter.redTurnoverAvt / 2.;
					}

					if (localdata.p_eng1_zap | localdata.p_eng2_zap | localdata.reduktor_gl_obor > 0)//������� �������� ������� ���������
						if (!red)//���� ������ �� ������ 
							red = new Reductor;//������� ������
					if (red)//���� ������ ������ - ���������� ���
					{
						red->channel[0] = 1;
						red->channel[1] = 1;
						red->Play(helicopter, localdata);//
						if (red->sourceStatus[0] != AL_PLAYING && red->sourceStatus[1] != AL_PLAYING && !(localdata.p_eng1_zap | localdata.p_eng2_zap | localdata.reduktor_gl_obor > 0))//������� �������� �������
							Free(red);//������� ������
					}

				}
				//���������
				if (helicopter.engFactor)
				{
					if (localdata.p_eng1_hp & localdata.eng1_obor != 0)//������� �������� �������
						if (!engHp[0])//���� ������ �� ������ 
							engHp[0] = new Sound;//������� ������
					if (engHp[0])//���� ������ ������ - ���������� ���
					{
						//eng_hp[0]->position = PosLeft;
						engHp[0]->channel[0] = 1;
						engHp[0]->channel[1] = 0;
						if (localdata.p_eng1_zap | localdata.p_eng1_ostanov)
						{
							engHp[0]->initializeSound(localdata.p_eng1_hp & localdata.eng1_obor != 0, "NULL", "NULL", "NULL", helicopter.engHpFactor);//������������� ���� - ���������� ��������� ����� � play
						}
						else
							engHp[0]->initializeSound(localdata.p_eng1_hp & localdata.eng1_obor != 0, helicopter.fullName["eng_on_hp_w"], helicopter.fullName["eng_w_hp_w"], helicopter.fullName["eng_off_hp_w"], helicopter.engHpFactor);//������������� ���� - ���������� ��������� ����� � play
						if (engHp[0]->sourceStatus[0] != AL_PLAYING && !localdata.p_eng1_hp)//������� �������� �������
							Free(engHp[0]);//������� ������
					}

					if (localdata.p_eng2_hp & localdata.eng2_obor != 0)//������� �������� �������
						if (!engHp[1])//���� ������ �� ������ 
							engHp[1] = new Sound;//������� ������
					if (engHp[1])//���� ������ ������ - ���������� ���
					{
						//eng_hp[1]->position = PosRight;
						engHp[1]->channel[0] = 0;
						engHp[1]->channel[1] = 1;
						if (localdata.p_eng2_zap | localdata.p_eng2_ostanov)
						{
							engHp[1]->initializeSound(localdata.p_eng2_hp & localdata.eng2_obor != 0, "NULL", "NULL", "NULL", helicopter.engHpFactor);//������������� ���� - ���������� ��������� ����� � play
						}
						else
							engHp[1]->initializeSound(localdata.p_eng2_hp & localdata.eng2_obor != 0, helicopter.fullName["eng_on_hp_w"], helicopter.fullName["eng_w_hp_w"], helicopter.fullName["eng_off_hp_w"], helicopter.engHpFactor);//������������� ���� - ���������� ��������� ����� � play
						if (engHp[1]->sourceStatus[0] != AL_PLAYING && !localdata.p_eng2_hp)//������� �������� �������
							Free(engHp[1]);//������� ������
					}

					if (localdata.p_eng1_pomp)//������� �������� �������
						if (!engPomp[0])//���� ������ �� ������ 
							engPomp[0] = new Sound;//������� ������
					if (engPomp[0])//���� ������ ������ - ���������� ���
					{
						//eng_pomp[0]->position = PosLeft;
						engPomp[0]->channel[0] = 1;
						engPomp[0]->channel[1] = 0;
						engPomp[0]->initializeSound(localdata.p_eng1_pomp, helicopter.fullName["eng_pomp_on"], helicopter.fullName["eng_pomp_w"], "NULL", helicopter.engFactor);//������������� ���� - ���������� ��������� ����� � play
						if (engPomp[0]->sourceStatus[0] != AL_PLAYING && !localdata.p_eng1_pomp)//������� �������� �������
							Free(engPomp[0]);//������� ������
					}

					if (localdata.p_eng2_pomp)//������� �������� �������
						if (!engPomp[1])//���� ������ �� ������ 
							engPomp[1] = new Sound;//������� ������
					if (engPomp[1])//���� ������ ������ - ���������� ���
					{
						//eng_pomp[1]->position = PosRight;
						engPomp[1]->channel[0] = 0;
						engPomp[1]->channel[1] = 1;
						engPomp[1]->initializeSound(localdata.p_eng2_pomp, helicopter.fullName["eng_pomp_on"], helicopter.fullName["eng_pomp_w"], "NULL", helicopter.engFactor);//������������� ���� - ���������� ��������� ����� � play
						if (engPomp[1]->sourceStatus[0] != AL_PLAYING && !localdata.p_eng2_pomp)//������� �������� �������
							Free(engPomp[1]);//������� ������
					}

					if (localdata.p_eng1_zap | localdata.eng1_obor > 0)
						if (!eng[0])//���� ������ �� ������ 
							eng[0] = new Engine;//������� ������
					if (eng[0])//���� ������ ������ - ���������� ���
					{
						//eng[0]->position = PosLeft;
						eng[0]->channel[0] = 1;//magic numbers//1
						eng[0]->channel[1] = -1;//-1
						eng[0]->Play(localdata.p_eng1_zap, localdata.p_eng1_ostanov, localdata.eng1_obor, localdata, helicopter);
						if (eng[0]->sourceStatus[0] != AL_PLAYING && eng[0]->sourceStatus[1] != AL_PLAYING && !(localdata.p_eng1_zap | localdata.eng1_obor > 0))//������� �������� �������
							Free(eng[0]);//������� ������
					}

					if (localdata.p_eng2_zap | localdata.eng2_obor > 0)
						if (!eng[1])//���� ������ �� ������ 
							eng[1] = new Engine;//������� ������
					if (eng[1])//���� ������ ������ - ���������� ���
					{
						//eng[1]->position = PosRight;
						eng[1]->channel[0] = 0;//magic numbers
						eng[1]->channel[1] = 2;//
						eng[1]->Play(localdata.p_eng2_zap, localdata.p_eng2_ostanov, localdata.eng2_obor, localdata, helicopter);
						if (eng[1]->sourceStatus[0] != AL_PLAYING && eng[1]->sourceStatus[1] != AL_PLAYING && !(localdata.p_eng2_zap | localdata.eng2_obor > 0))//������� �������� �������
							Free(eng[1]);//������� ������
					}
				}
				//����������� ���� ���
				if (helicopter.vsuCraneFactor)
				{
					if (localdata.rez_1)//������� �������� �������
						if (!vsuKran)//���� ������ �� ������ 
							vsuKran = new Sound;//������� ������
					if (vsuKran)//���� ������ ������ - ���������� ���
					{
						if (helicopter.modelName == "ka_27" | helicopter.modelName == "ka_29")
						{
							vsuKran->initializeSound(localdata.rez_1, helicopter.fullName["vsu_kran_on"], "NULL", "NULL", helicopter.vsuCraneFactor);//������������� ���� - ���������� ��������� ����� � play
						}
						else
						{
							vsuKran->initializeSound(localdata.rez_1, helicopter.fullName["vsu_kran_on"], helicopter.fullName["vsu_kran_w"], "NULL", helicopter.vsuCraneFactor);//������������� ���� - ���������� ��������� ����� � play
						}
						if (vsuKran->sourceStatus[0] != AL_PLAYING && !localdata.rez_1)//������� �������� �������
							Free(vsuKran);//������� ������
					}
				}
				//������
				if (helicopter.buzzerFactor)
				{
					if (localdata.rez_7)//������� �������� �������
						if (!beep)//���� ������ �� ������ 
							beep = new Sound;//������� ������
					if (beep)//���� ������ ������ - ���������� ���
					{
						beep->initializeSound(localdata.rez_7, "NULL", helicopter.fullName["beep"], "NULL", helicopter.buzzerFactor);//������������� ���� - ���������� ��������� ����� � play
						if (beep->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(beep);//������� ������
					}
				}
				//����������
				if (helicopter.fenFactor)
				{
					if (localdata.rez_5)//������� �������� �������
						if (!undefined0)//���� ������ �� ������ 
							undefined0 = new Sound;//������� ������
					if (undefined0)//���� ������ ������ - ���������� ���
					{
						undefined0->initializeSound(localdata.rez_5, helicopter.fullName["undefined0_on"], helicopter.fullName["undefined0_w"], "NULL", helicopter.fenFactor);//������������� ���� - ���������� ��������� ����� � play
						if (undefined0->sourceStatus[0] != AL_PLAYING)//������� �������� �������
							Free(undefined0);//������� ������
					}
				}
				//�������������� 1
				if (helicopter.undefinedFactor)
				{
					if (helicopter.modelName == "mi_28")
					{
						if (localdata.rez_6)//������� �������� �������
							if (!undefined1)//���� ������ �� ������ 
								undefined1 = new Sound;//������� ������
						if (undefined1)//���� ������ ������ - ���������� ���
						{
							undefined1->initializeSound(localdata.rez_6, "NULL", helicopter.fullName["undefined1_w"], "NULL", helicopter.undefinedFactor);//������������� ���� - ���������� ��������� ����� � play
							if (undefined1->sourceStatus[0] != AL_PLAYING)//������� �������� �������
								Free(undefined1);//������� ������
						}
					}
					else if (helicopter.modelName == "ka_29")
					{
						if (localdata.rez_6)//������� �������� �������
							if (!undefined1)//���� ������ �� ������ 
								undefined1 = new Sound;//������� ������
						if (undefined1)//���� ������ ������ - ���������� ���
						{
							undefined1->initializeSound(localdata.rez_6, helicopter.fullName["undefined1_on"], helicopter.fullName["undefined1_w"], "NULL", helicopter.undefinedFactor);//������������� ���� - ���������� ��������� ����� � play
							if (undefined1->sourceStatus[0] != AL_PLAYING)//������� �������� �������
								Free(undefined1);//������� ������
						}
					}
				}
				//printProgrammStatus(localdata);//������� ��������� ���������
				//if (eng[0] && eng[1] && red)
				//{
				//	//printf(" sz: %i e1p: %.3f e2p: %.3f rp: %.3f\t\t\t\t\t\t\r", vectorTime.size(), eng[0]->pitch, eng[1]->pitch,red->pitch);
				//	if (!vintFlap)
				//		printf(" Acc = %8.3f AccVy = %8.3f calcA = %8.3f vY = %8.3f vX = %8.3f tangaz = %8.3f Dash = %8.3f vectorTime.size() = %i avrcps = %i avrcp = %i front = %8.3f time = %8.3f \t\t\t\r", Sound::accelerationX, Sound::accelerationVy, Sound::calcA, Sound::velocityY, Sound::velocityX, Sound::tangaz, Sound::dash, vectorTime.size(), red->vectorStep.size(), red->vector.size(), vectorTime.front(), Sound::currentTime);
				//	else
				//	{
				//		if (vintFlap->flapIndicator)
				//			printf(" Acc = %8.3f AccVy = %8.3f calcA = %8.3f vY = %8.3f vX = %8.3f tangaz = %8.3f Dash = %8.3f vectorTime.size() = %i avrcps = %i avrcp = %i front = %8.3f time = %8.3f  FLAPPPS!!!\t\t\t\r", Sound::accelerationX, Sound::accelerationVy, Sound::calcA, Sound::velocityY, Sound::velocityX, Sound::tangaz, Sound::dash, vectorTime.size(), red->vectorStep.size(), red->vector.size(), vectorTime.front(), Sound::currentTime);
				//		else
				//			printf(" Acc = %8.3f AccVy = %8.3f calcA = %8.3f vY = %8.3f vX = %8.3f tangaz = %8.3f Dash = %8.3f vectorTime.size() = %i avrcps = %i avrcp = %i front = %8.3f time = %8.3f \t\t\t\r", Sound::accelerationX, Sound::accelerationVy, Sound::calcA, Sound::velocityY, Sound::velocityX, Sound::tangaz, Sound::dash, vectorTime.size(), red->vectorStep.size(), red->vector.size(), vectorTime.front(), Sound::currentTime);

				//	}
				//}

			}
		}
		else
		{
			vectorHigh.clear();
			vectorVy.clear();
			vectorVx.clear();
			vectorAcc.clear();
			vectorStep.clear();
			vectorTime.clear();
			if (eng[0])
			{
				eng[0]->vector.clear();
			}
			if (eng[1])
			{
				eng[1]->vector.clear();
			}
			if (red)
			{
				red->vector.clear();
				red->vectorStep.clear();
			}
			periodCalc = 0;
			printf(" Waiting for USPO to initializate.\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\r");
			alutSleep(0.5);
			printf(" Waiting for USPO to initializate..\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\r");
			alutSleep(0.5);
			printf(" Waiting for USPO to initializate...\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\r");
			alutSleep(0.5);
		}
	}
	atexit(freeOpenAL);//!<���������� ������� KillALData �� ���������� ���������
	StopRealTime();
	return 0;
}

float lineInterpolation(float x0, float fx0, float x1, float fx1, float x)
{
	double fx, a0, a1, a2;
	if (x0<x1 && x>x1)
	{
		return fx1;
	}
	if (x0<x1 && x<x0)
	{
		return fx0;
	}
	if (x0>x1 && x<x1)
	{
		return fx1;
	}
	if (x0>x1 && x>x0)
	{
		return fx0;
	}

	return	fx = fx0 + ((fx1 - fx0) / (x1 - x0))*(x - x0);
}

float squareInterpolation(float x0, float fx0, float x1, float fx1, float x2, float fx2, float x)
{
	double fx, a0, a1, a2;
	if (x0<x2 && x>x2)
	{
		return fx2;
	}
	if (x0<x2 && x<x0)
	{
		return fx0;
	}
	if (x0>x2 && x<x2)
	{
		return fx2;
	}
	if (x0>x2 && x>x0)
	{
		return fx0;
	}

	//���� ������������ ������������ �� �������� - ����� ��������
	if (x1 == x0 | x2 == x1)
	{
		return	lineInterpolation(x0,fx0,x1,fx1,x);
	}
	else
	{
		a2 = ((fx2 - fx0) / ((x2 - x0)*(x2 - x1))) - ((fx1 - fx0) / ((x1 - x0)*(x2 - x1)));
		a1 = ((fx1 - fx0) / (x1 - x0)) - (a2*(x1 + x0));
		a0 = fx0 - a1 * x0 - a2 * x0*x0;
		return fx = a0 + a1 * x + a2*x*x;

	}

}

float getPitch(float offset, string filename, float parameter)
{
	float new_pitch;
	float turn = 0;
	float t = 0;
	float v = 0;
	int i = 0;
	vector <float> time, value;

	//������ � ���� ������ ��������� � ������� ������, �� ���� � ������ ������ (�� ������)
	string str;
	ifstream base(filename);
	while (!base.eof())
	{
		getline(base, str);
		sscanf(str.c_str(), "%f %f", &t, &v);
		time.push_back(t);
		value.push_back(v);
	}
	base.close();

	float x, x0, x1, x2, fx, fx0, fx1, fx2, a0, a1, a2;
	int n = time.size();

	for (i = 0; i < n; i++)
	{
		if (offset < time[0])
		{
			turn = value[0];//������� ������� �� ����
			break;
		}
		if (offset == time[i])//�������� ������� ������� ������� � �������� �� ��
		{
			turn = value[i];//������� ������� �� ����
			break;
		}
		if (offset > time[n - 1])//������� �� ������� � �����
		{
			turn = value[n - 1];//������� ������� �� ����
			break;
		}
		if (offset > time[i] && offset < time[i + 1])//������� �� ������� � �����
		{

			//������������ ������������
			if (i - 1 == -1 || i + 1 == n)
			{
				if (i - 1 == -1)
				{
					x = offset; x0 = time[i]; fx0 = value[i]; x1 = time[i + 1]; fx1 = value[i + 1]; x2 = time[i + 2]; fx2 = value[i + 2];
				}
				if (i + 1 == n)
				{
					x = offset; x0 = time[i - 2]; fx0 = value[i - 2]; x1 = time[i - 1]; fx1 = value[i - 1]; x2 = time[i]; fx2 = value[i];
				}
			}
			else
			{
				x = offset; x0 = time[i - 1]; fx0 = value[i - 1]; x1 = time[i]; fx1 = value[i]; x2 = time[i + 1]; fx2 = value[i + 1];
			}
			//���� ������������ ������������ �� �������� - ����� ��������
			if (x1 == x0 | x2 == x1)
			{
				turn = lineInterpolation(x0, fx0, x1, fx1, x);
			}
			else
			{
				turn = squareInterpolation(x0, fx0, x1, fx1, x2, fx2, x);
			}
		}
	}
	if (turn <= 0)
		new_pitch = 1;
	else
		new_pitch = parameter / turn;	//��������� �������������� Pitch �� ������ ��������z ���������� ������ �������� � �������� (������� ��� ������ �����-�����)

	return new_pitch;
}

float getOffset(float pitch, string filename, float parameter)
{
	float new_offset = 0;
	float turn = 0;
	int i = 0;

	float t = 0;
	float v = 0;
	vector <float> time, value;

	//������ � ���� ������ ��������� � ������� ������, �� ���� � ������ ������ (�� ������)
	string str;
	ifstream base(filename);
	while (!base.eof())
	{
		getline(base, str);
		sscanf(str.c_str(), "%f %f", &t, &v);
		time.push_back(t);
		value.push_back(v);
	}
	base.close();
	int n = time.size();

	if (parameter < 0)
		turn = 0;
	else
		turn = parameter / pitch;

	float x, x0, x1, x2, fx, fx0, fx1, fx2, a0, a1, a2;

	if (value[0] <= value[n - 1])
	{
		for (i = 0; i < n; i++)
		{
			if (turn < value[i] && i == 0)
			{
				new_offset = time[i];//������� ������� �� ����
				break;
			}
			if (turn == value[i])//�������� ������� ������� ������� � �������� �� ��
			{
				new_offset = time[i];//������� ������� �� ����
				break;
			}
			if (turn > value[i] && i == n - 1)//������� �� ������� � �����
			{
				new_offset = time[i];//������� ������� �� ����
				break;
			}
			if (turn > value[i] && turn < value[i + 1])//������� �� ������� � �����
			{

				//������������ ������������
				if (i + 2 == n || i + 1 == n)
				{
					if (i + 2 == n)
					{
						x = turn; x0 = value[i - 1]; fx0 = time[i - 1]; x1 = value[i]; fx1 = time[i]; x2 = value[i + 1]; fx2 = time[i + 1];
					}
					if (i + 1 == n)
					{
						x = turn; x0 = value[i - 2]; fx0 = time[i - 2]; x1 = value[i - 1]; fx1 = time[i - 1]; x2 = value[i]; fx2 = time[i];
					}
				}
				else
				{
					x = turn; x0 = value[i]; fx0 = time[i]; x1 = value[i + 1]; fx1 = time[i + 1]; x2 = value[i + 2]; fx2 = time[i + 2];
				}

				new_offset = squareInterpolation(x0, fx0, x1, fx1, x2, fx2, x);
			}

		}

	}
	else
	{

		for (i = 0; i < n; i++)
		{
			if (turn > value[0])
			{
				new_offset = time[0];//������� ������� �� ����
				break;
			}
			if (turn == value[i])//�������� ������� ������� ������� � �������� �� ��
			{
				new_offset = time[i];//������� ������� �� ����
				break;
			}
			if (turn < value[n - 1])//������� �� ������� � �����
			{
				new_offset = time[n - 1];//������� ������� �� ����
				break;
			}
			if (turn < value[i] && turn > value[i + 1])//������� �� ������� � �����
			{

				//������������ ������������
				if (i + 2 == n || i + 1 == n)
				{
					if (i + 2 == n)
					{
						x = turn; x0 = value[i - 1]; fx0 = time[i - 1]; x1 = value[i]; fx1 = time[i]; x2 = value[i + 1]; fx2 = time[i + 1];
					}
					if (i + 1 == n)
					{
						x = turn; x0 = value[i - 2]; fx0 = time[i - 2]; x1 = value[i - 1]; fx1 = time[i - 1]; x2 = value[i]; fx2 = time[i];
					}
				}
				else
				{
					x = turn; x0 = value[i]; fx0 = time[i]; x1 = value[i + 1]; fx1 = time[i + 1]; x2 = value[i + 2]; fx2 = time[i + 2];
				}

				new_offset = squareInterpolation(x0, fx0, x1, fx1, x2, fx2, x);
			}

		}
	}

	if (new_offset <= 0)
		new_offset = 0;

	return new_offset;

}

int crossFade(float *gf, float *gr, float parameter, float limit1,float limit2,float mult)//source - fade , source2 - rise
{
	//limit1 - �������� ��������� � ������ �����-�����
	//limit2 - �������� ��������� � �����
	//parameter - �������� ���������� - ������ ��������� �� limit1 � limit2
	//source - ��������, ��������� �������� �������
	//source2 - ��������, ��������� �������� ���������
	//float gf, gr;
	
	if ((limit2 > limit1 && parameter < limit1) || (limit2 < limit1 && parameter > limit1))
	{	
		*gf = 1 * mult;
		*gr = 0;
	}	
	if ((limit2 > limit1 && parameter > limit2) || (limit2 < limit1 && parameter < limit2))
	{
		*gf = 0;
		*gr = 1* mult;
	}
	if ((limit2 > limit1 && parameter <= limit2 && parameter >= limit1) || (limit2 < limit1 && parameter >= limit2 && parameter <= limit1))
	{
		*gf = squareInterpolation(limit1, mult, abs(limit2 - limit1) / 2, 0.5*mult, limit2, 0, parameter);
		*gr = squareInterpolation(limit1, 0, abs(limit2 - limit1) / 2, 0.5*mult, limit2, mult, parameter);
		*gf = (*gf < 0 )? 0 : *gf;
		*gr = (*gf < 0) ? 0 : *gr;
	}

	//alSourcef(source, AL_GAIN, gf);
	//alSourcef(source2, AL_GAIN, gr);

	if (limit2 > limit1 && parameter > limit2)
		return 1;//���������� 1 ����� ��������� ��������
	
	if (limit2 < limit1 && parameter < limit2)
		return 1;
	
	return 0;//���������� 0 ����� ��������� �� ��������
}

void setEFXPointers()
{
	// Get function pointers
	alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
	alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
	alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
	alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
	alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
	alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
	alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
	alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
	alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
	alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
	alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
	alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
	alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
	alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
	alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
	alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv");
	alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
	alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv");
	alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
	alGetFilteriv = (LPALGETFILTERIV)alGetProcAddress("alGetFilteriv");
	alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");
	alGetFilterfv = (LPALGETFILTERFV)alGetProcAddress("alGetFilterfv");
	alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
	alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
	alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
	alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
	alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
	alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
	alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
	alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
	alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
	alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
	alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");
}

void freeOpenAL()
{
	alutExit();
}

void printProgrammStatus(SOUNDREAD srd)
{
	char kor = '0';
	//���������� ��������� (��� �������)
	if (srd.p_eng1_lkorr & srd.p_eng2_lkorr)
		kor = 'L';
	else if (srd.p_eng1_rkorr & srd.p_eng2_rkorr)
		kor = 'R';
	else kor = '0';

	printf(" Eng-1 = %8.3f  Eng-2 = %8.3f  Rotor = %8.3f  Time = %9.3f Sources(free) - %3i Sources(lock) - %3i Cor - %1c\t\t\t\t\t\r", srd.eng1_obor, srd.eng2_obor, srd.reduktor_gl_obor, srd.time, Sound::maxSources - Sound::sourcesInUse, Sound::sourcesInUse, kor);
	//cout << " Eng-1 = " << srd.eng1_obor << "  Eng-2 = " << srd.eng2_obor << "  Rotor = " << srd.reduktor_gl_obor << "  Time = " << srd.time << "  Sources(free) - " << Sound::maxSources - Sound::sourcesInUse << "  Sources(lock) - " << Sound::sourcesInUse << "  Cor - " << kor << "\r";
}

int getMaxAvaliableSources()
{
	ALCdevice *device;
	ALCcontext *context;

	int maxmono = 0, maxstereo = 0;

	device = alcOpenDevice(NULL);
	if (device == NULL)
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		return AL_FALSE;
	}

	context = alcCreateContext(device, NULL);
	if (context == NULL)
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		alcCloseDevice(device);
		return AL_FALSE;
	}
	if (!alcMakeContextCurrent(context))
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		alcDestroyContext(context);
		alcCloseDevice(device);
		return AL_FALSE;
	}

	ALCint size;
	alcGetIntegerv(device, ALC_ATTRIBUTES_SIZE, 1, &size);
	ALCint *attrs;
	attrs = new ALCint[size];
	alcGetIntegerv(device, ALC_ALL_ATTRIBUTES, size, attrs);

	for (size_t i = 0; i<size; ++i)
	{
		if (attrs[i] == ALC_SYNC)
		{
			std::cout << " Flag, indicating a synchronous context : " << attrs[i + 1] << std::endl;
		}
		if (attrs[i] == ALC_REFRESH)
		{
			std::cout << " Refresh intervals, in units of Hz: " << attrs[i + 1] << std::endl;
		}
		if (attrs[i] == ALC_FREQUENCY)
		{
			std::cout << " Frequency for mixing output buffer, in units of Hz : " << attrs[i + 1] << std::endl;
		}
		if (attrs[i] == ALC_MONO_SOURCES)
		{
			std::cout << " A hint indicating how many sources should be capable of supporting mono data: " << attrs[i + 1] << std::endl;
			maxmono = attrs[i + 1];
		}
		if (attrs[i] == ALC_STEREO_SOURCES)
		{
			std::cout << " A hint indicating how many sources should be capable of supporting stereo data: " << attrs[i + 1] << std::endl;
			maxstereo = attrs[i + 1];
		}
	}
	alcDestroyContext(context);
	alcCloseDevice(device);
	return maxmono + maxstereo;	
}
/*!
\brief �������������� ��������������� �����
\details ������������� ������ ��������������� ����� ��� ��������� �������� ������ �������� ��� ���.�������:
���� ������������ ������ ������ �������� (��� �������� - "1")
 |
\|/
���� ������ �������� (��� ��������� ����� ������������ ������ � �������� - "1")
 |
\|/
���� ���������� ������ �������� (��� �������� - "0")

\param[in] status ������� ������

\param[in] path_on ��������� �� ������ � ������� ����� ������������ ������ ������
\param[in] path_w ��������� �� ������ � ������� ����� ������ ������
\param[in] path_off ��������� �� ������ � ������� ����� ���������� ������
\param[in] gain_mult ����������� ��������� ��� ��������
\param[in] master_gain ����������� ��������� �������
\return ������ ��������� OpenAL
*/
int Sound::initializeSound(bool status, string path_on, string path_w, string path_off, float gain_mult)
{
	bool start;
	bool work;
	bool end;
	bool free;

	if (path_on != "NULL")
		lengthOn = getLengthWAV(path_on);
	if (path_off != "NULL")
		lengthOff = getLengthWAV(path_off);

	alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);
	//������� ������� ����� ��� ����� ������������
	if (path_on != "NULL" & path_w != "NULL" & path_off != "NULL")
	{
		start = status & !soundOn & !soundWork;
		work = status & soundOn & !soundWork & sourceStatus[0] != AL_PLAYING;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[0] != AL_PLAYING;
	}
	//������� ������� ����� ����������� ���������
	if (path_on != "NULL" & path_w != "NULL" & path_off == "NULL")
	{
		start = status & !soundOn & !soundWork;
		work = status & soundOn & !soundWork & sourceStatus[0] != AL_PLAYING;
		end = 0;
		free = !status;
	}
	//������� ������� ����� ����������� ������
	if (path_on != "NULL" & path_w == "NULL" & path_off != "NULL")
	{
		start = status & !soundOn;
		work = 0;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[0] != AL_PLAYING;
	}
	//������� ������� ����� ����������� ������
	if (path_on == "NULL" & path_w != "NULL" & path_off != "NULL")
	{
		start = 0;
		work = status & !soundWork;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[0] != AL_PLAYING;
	}
	//������� ������� ����� ����������� ������ � ������
	if (path_on == "NULL" & path_w == "NULL" & path_off != "NULL")
	{
		start = 0;
		work = 0;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[0] != AL_PLAYING;
	}
	//������� ������� ����� ����������� ������ � ����������
	if (path_on == "NULL" & path_w != "NULL" & path_off == "NULL")
	{
		start = 0;
		work = status & !soundWork;
		end = 0;
		free = !status;
	}
	//������� ������� ����� ����������� ������ � ����������
	if (path_on != "NULL" & path_w == "NULL" & path_off == "NULL")
	{
		start = status & !soundOn;
		work = 0;
		end = 0;
		free = !status & sourceStatus[0] != AL_PLAYING;
	}
	//��� 0
	if (path_on == "NULL" & path_w == "NULL" & path_off == "NULL")
	{
		start = 0;
		work = 0;
		end = 0;
		free = 1;
	}
	//��������� (���� path_o ��������� �� ������ ������� -> � �������� ����������� ���� �������)
	if (start)
	{
		soundOn = 1;
		soundWork = 0;
		soundOff = 0;

		alSourceStop(source[0]);
		alSourcei(source[0], AL_BUFFER, NULL);
		alDeleteBuffers(1, &buffer[0]);
		alGenBuffers(1, &buffer[0]);
		if (!setBuffer(buffer[0], path_on, channelsSetup, channel))
			return 0;
		alSourcei(source[0], AL_BUFFER, buffer[0]);
		alSourcef(source[0], AL_PITCH, pitch);
		alSourcef(source[0], AL_GAIN, gain*gain_mult*masterGain);
		alSourcei(source[0], AL_LOOPING, AL_FALSE);
		alSourcef(source[0], AL_SEC_OFFSET, offsetOn);
		alSourcePlay(source[0]);
		alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);
	}
	//������ (���� path_w ��������� �� ������ ������� -> � �������� ����������� ���� ������ ������)
	if (work)
	{
		soundOn = 0;
		soundWork = 1;
		soundOff = 0;

		alSourceStop(source[0]);
		alSourcei(source[0], AL_BUFFER, NULL);
		alDeleteBuffers(1, &buffer[0]);
		alGenBuffers(1, &buffer[0]);
		if (!setBuffer(buffer[0], path_w, channelsSetup, channel))
			return 0;
		alSourcei(source[0], AL_BUFFER, buffer[0]);
		alSourcef(source[0], AL_PITCH, pitch);
		alSourcef(source[0], AL_GAIN, gain*gain_mult*masterGain);
		alSourcei(source[0], AL_LOOPING, AL_TRUE);
		alSourcePlay(source[0]);
		alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);
	}
	//���������� (���� path_off ��������� �� ������ ������� -> � �������� ����������� ���� ����������)
	if (end)
	{
		soundOn = 0;
		soundWork = 0;
		soundOff = 1;

		alSourceStop(source[0]);
		alSourcei(source[0], AL_BUFFER, NULL);
		alDeleteBuffers(1, &buffer[0]);
		alGenBuffers(1, &buffer[0]);
		if (!setBuffer(buffer[0], path_off, channelsSetup, channel))
			return 0;
		alSourcei(source[0], AL_BUFFER, buffer[0]);
		alSourcef(source[0], AL_PITCH, pitch);
		alSourcef(source[0], AL_GAIN, gain*gain_mult*masterGain);
		alSourcei(source[0], AL_LOOPING, AL_FALSE);
		alSourcef(source[0], AL_SEC_OFFSET, offsetOff);
		alSourcePlay(source[0]);
		alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);
	}
	//������������ ������
	if (free)
	{
		soundOn = 0;
		soundWork = 0;
		soundOff = 0;
		alSourceStop(source[0]);
		alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);
	}
	//��������� ��������� �� ���� ���������������
	if (sourceStatus[0] == AL_PLAYING)
	{
		alSourcef(source[0], AL_PITCH, pitch);
		alSourcef(source[0], AL_GAIN, gain*gain_mult*masterGain);
		if (soundOn)
		{
			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
			offsetOff = lengthOff * (1 - (offsetOn / lengthOn));
		}
		if (soundOff)
		{
			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);
			offsetOn = lengthOn * (1 - (offsetOff / lengthOff));
		}
			
	}
	return sourceStatus[0];
}

int Sound::setBuffer(ALuint Buffer, string path, AL_SOUND_CHANNELS channelsCount, double *channels)
{
	int format;
	int size;
	int freq;
	void *iData;
	void *rData;
	int rSize;
	bool ok = 0;
	int bitsPerSample = 0;
	ALboolean loop;
	FILE *check;

	check = fopen(path.c_str(), "r");
	if (!check)
	{
		cout <<"\n file [" << path << "] is missing" << endl;
		system("cls");
		return 0;
	}
	fclose(check);
	alutLoadWAVFile((ALbyte*)path.c_str(), &format, &iData, &size, &freq, &loop);

	if (format == AL_FORMAT_STEREO8 || format == AL_FORMAT_STEREO16)
	{
		alBufferData(Buffer, format, iData, size, freq);
		alutUnloadWAV(format, iData, size, freq);
		return 1;
	}
	if (channelsCount != 0)//
	{
		if (format == AL_FORMAT_MONO8)//8���
		{
			bitsPerSample = 8;
			unsigned char *monodata0 = ((unsigned char*)iData);
			ok = mono2channels(monodata0, size, channelsCount, channels, &rData, &rSize);//��������������� � ������ � �������� ������ ��� 1�� ������ � 4��� �������� ������� (�������� �����)
		}
		if (format == AL_FORMAT_MONO16)//16���
		{
			bitsPerSample = 16;
			short *monodata1 = ((short*)iData);
			ok = mono2channels(monodata1, size, channelsCount, channels, &rData, &rSize);//��������������� � ������ � �������� ������ ��� 1�� ������ � 4��� �������� ������� (�������� �����)
		}
		if (format != AL_FORMAT_MONO16 && format != AL_FORMAT_MONO8)//���������������� ������
			return 0;

		format = getFormat(channelsCount, bitsPerSample);
		alBufferData(Buffer, format, rData, rSize, freq);
		free(rData);
	}
	else
	{
		alBufferData(Buffer, format, iData, size, freq);
	}
	alutUnloadWAV(format, iData, size, freq);
	return 1;
}

int Reductor::Play(Helicopter h, SOUNDREAD sr) 
{
	for (size_t i = 0; i < 2; i++)
	{
		if (eq_key[i] != 'q')
		{
			alEffecti(effect[i], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//���������� ������ ��� ����������
			alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
			alFilteri(filter[i], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[i], AL_LOWPASS_GAIN, 0);
			alSource3i(source[i], AL_AUXILIARY_SEND_FILTER, effectSlot[i], 0, NULL);
			alSourcei(source[i], AL_DIRECT_FILTER, filter[i]);
			eq_key[i] = 'q';
		}
	}
	//�������� ������
	if (h.modelName == "ansat")
	{
		//0 -> ��
		if ((sr.p_eng1_zap | sr.p_eng2_zap) && sr.reduktor_gl_obor < h.redTurnoverMg1)
		{
			if (red_key[0] != '1')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);

				if (!setBuffer(buffer[0], h.fullName["red_on_w"], channelsSetup, channel))
					return 0;

				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcef(source[0], AL_GAIN, masterGain*h.redFactor);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOn = getOffset(1, h.fullName["red_on"], sr.reduktor_gl_obor);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOn);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				red_key[0] = '1';
			}
			if (sourceStatus[0] == AL_PLAYING)
			{
				alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
				if (sr.reduktor_gl_obor >= 5)
					alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["red_on"], sr.reduktor_gl_obor));
				else
					alSourcef(source[0], AL_PITCH, 1);

				if (sr.reduktor_gl_obor < (h.redTurnoverMg1 - 10))
				{
					alSourcef(source[0], AL_GAIN, masterGain*h.redFactor);
				}
			}
			if (sourceStatus[0] != AL_PLAYING && red_key[0] == '1')
			{
				if (red_key[1] != '2')//���������� - ����������� - ��������� ������
				{
					alSourceStop(source[1]);
					alSourcei(source[1], AL_BUFFER, NULL);
					alDeleteBuffers(1, &buffer[1]);
					alGenBuffers(1, &buffer[1]);

					if (!setBuffer(buffer[1], h.fullName["red_w_w"], channelsSetup, channel))
						return 0;

					alSourcei(source[1], AL_BUFFER, buffer[1]);
					alSourcei(source[1], AL_LOOPING, AL_TRUE);
					alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)

		
					alSourcePlay(source[1]);
					alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
					red_key[1] = '2';
				}
				if (sourceStatus[1] == AL_PLAYING && red_key[1] == '2')//���� ������ ������������� - ������ �� �� �����������
				{
					alSourcef(source[1], AL_GAIN, masterGain*h.redFactor);
					alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				}
			}

		}
		//0 -> ��
		if (sr.reduktor_gl_obor > (h.redTurnoverMg1 - 10) && sr.reduktor_gl_obor <= h.redTurnoverMg1 && (sr.p_eng1_zap | sr.p_eng2_zap))
		{
			if (red_key[0] != '1')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);

				if (!setBuffer(buffer[0], h.fullName["red_on_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOn = getOffset(1, h.fullName["red_on"], sr.reduktor_gl_obor);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOn);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				red_key[0] = '1';
			}
			if (red_key[1] != '2')//���������� - ����������� - ��������� ������
			{
				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);
				if (!setBuffer(buffer[1], h.fullName["red_w_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[1], AL_BUFFER, buffer[1]);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);
				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.redFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
				red_key[1] = '2';
			}

			float fade, rise;
			if (sourceStatus[0] == AL_PLAYING)
			{
				crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1 - 10, h.redTurnoverMg1, masterGain*h.redFactor);
				alSourcef(source[0], AL_GAIN, fade);//0
				alSourcef(source[1], AL_GAIN, rise);//1
			}
			else
				alSourcef(source[1], AL_GAIN, masterGain*h.redFactor);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
			if (sr.reduktor_gl_obor >= 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["red_on"], sr.reduktor_gl_obor));
			else
				alSourcef(source[0], AL_PITCH, 1);
			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
		}
		//�� <-> ���
		if (sr.reduktor_gl_obor > h.redTurnoverMg2)
		{
			if (red_key[1] != '3')
			{
				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);
				if (!setBuffer(buffer[1], h.fullName["red_w_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[1], AL_BUFFER, buffer[1]);
				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);//
				alSourcei(source[1], AL_LOOPING, AL_TRUE);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
				red_key[1] = '3';
			}
			if (red_key[0] != '4')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);
				if (!setBuffer(buffer[0], h.fullName["red_w_avt_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//
																					 //alSourcefv(Source[0], AL_POSITION, position);
																					 //alSourcefv(Source[0], AL_VELOCITY, velocity);
				alSourcei(source[0], AL_LOOPING, AL_TRUE);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				red_key[0] = '4';
			}
			float fade, rise;
			crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg2, h.redTurnoverAvt, masterGain*h.redFactor);
			alSourcef(source[1], AL_GAIN, fade);//
			alSourcef(source[0], AL_GAIN, rise);//
			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
			alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
		}
		//�� -> 0
		if (!sr.p_eng1_zap && !sr.p_eng2_zap && sr.reduktor_gl_obor > 0 && sr.reduktor_gl_obor < h.redTurnoverMg1 - 1)
		{
			if (red_key[1] != '2')
			{
				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);
				if (!setBuffer(buffer[1], h.fullName["red_w_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[1], AL_BUFFER, buffer[1]);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);
				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.redFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
				red_key[1] = '2';
			}
			if (red_key[0] != '5')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);
				if (!setBuffer(buffer[0], h.fullName["red_off_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOff = getOffset(1, h.fullName["red_off"], sr.reduktor_gl_obor);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOff);
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["red_off"], sr.reduktor_gl_obor));
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				red_key[0] = '5';
			}
			float fade, rise;
			crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1, h.redTurnoverMg1 - 7., masterGain*h.redFactor);
			alSourcef(source[1], AL_GAIN, fade);//
			alSourcef(source[0], AL_GAIN, rise);//

			alSourcei(source[0], AL_LOOPING, AL_FALSE);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);

			if (sr.reduktor_gl_obor > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["red_off"], sr.reduktor_gl_obor));
			else
				alSourcef(source[0], AL_PITCH, 1);
		}

		alGetSourcef(source[0], AL_PITCH, &pitch);//
		alGetSourcef(source[0], AL_GAIN, &pitch);//
	}
	else
	{
		//0 -> �� 1��
		if ((sr.p_eng1_zap | sr.p_eng2_zap) && sr.reduktor_gl_obor < h.redTurnoverMg1)
		{
			if (red_key[0] != '1')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);

				if (!setBuffer(buffer[0], h.fullName["red_on_w"], channelsSetup, channel))
					return 0;

				//alutLoadWAVFile(h.path["red_on_w"], &format, &data, &size, &freq, &loop);				// �������� 0-45
				//alBufferData(Buffer[0], format, data, size, freq);
				//alutUnloadWAV(format, data, size, freq);

				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcef(source[0], AL_GAIN, masterGain*h.redFactor);
				//alSourcefv(Source[0], AL_POSITION, position);
				//alSourcefv(Source[0], AL_VELOCITY, velocity);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOn = getOffset(1, h.fullName["red_on"], sr.reduktor_gl_obor);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOn);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				red_key[0] = '1';
			}
			if (sourceStatus[0] == AL_PLAYING)
			{
				alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
				if (sr.reduktor_gl_obor >= 5)
					alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["red_on"], sr.reduktor_gl_obor));
				else
					alSourcef(source[0], AL_PITCH, 1);

				if (sr.reduktor_gl_obor < (h.redTurnoverMg1 - 10))
				{
					alSourcef(source[0], AL_GAIN, masterGain*h.redFactor);
				}
			}
			if (sourceStatus[0] != AL_PLAYING && red_key[0] == '1')
			{
				if (red_key[1] != '2')//���������� - ����������� - ��������� ������
				{
					alSourceStop(source[1]);
					alSourcei(source[1], AL_BUFFER, NULL);
					alDeleteBuffers(1, &buffer[1]);
					alGenBuffers(1, &buffer[1]);
					if (!setBuffer(buffer[1], h.fullName["red_w_w"], channelsSetup, channel))
						return 0;
					alSourcei(source[1], AL_BUFFER, buffer[1]);
					alSourcei(source[1], AL_LOOPING, AL_TRUE);
					alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
					alSourcePlay(source[1]);
					alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
					red_key[1] = '2';
				}
				if (sourceStatus[1] == AL_PLAYING && red_key[1] == '2')//���� ������ ������������� - ������ �� �� �����������
				{
					alSourcef(source[1], AL_GAIN, masterGain*h.redFactor);
					alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				}
			}

		}
		//0 -> �� 1��
		if (sr.reduktor_gl_obor > (h.redTurnoverMg1 - 10) && sr.reduktor_gl_obor <= h.redTurnoverMg1 && (sr.p_eng1_zap | sr.p_eng2_zap))
		{
			if (red_key[0] != '1')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);

				if (!setBuffer(buffer[0], h.fullName["red_on_w"], channelsSetup, channel))
					return 0;

				//alutLoadWAVFile(h.path["red_on_w"], &format, &data, &size, &freq, &loop);				// �������� 0-45
				//alBufferData(Buffer[0], format, data, size, freq);
				//alutUnloadWAV(format, data, size, freq);

				alSourcei(source[0], AL_BUFFER, buffer[0]);
				//alSourcefv(Source[0], AL_POSITION, position);
				//alSourcefv(Source[0], AL_VELOCITY, velocity);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOn = getOffset(1, h.fullName["red_on"], sr.reduktor_gl_obor);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOn);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				red_key[0] = '1';
			}
			if (red_key[1] != '2')//���������� - ����������� - ��������� ������
			{


				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);

				if (!setBuffer(buffer[1], h.fullName["red_w_w"], channelsSetup, channel))
					return 0;

				//alutLoadWAVFile(h.path["red_w_w"], &format, &data, &size, &freq, &loop);				// �������� 0-45
				//alBufferData(Buffer[1], format, data, size, freq);
				//alutUnloadWAV(format, data, size, freq);

				alSourcei(source[1], AL_BUFFER, buffer[1]);
				//alSourcefv(Source[1], AL_POSITION, position);
				//alSourcefv(Source[1], AL_VELOCITY, velocity);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);

				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.redFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
				red_key[1] = '2';
			}

			float fade, rise;
			if (sourceStatus[0] == AL_PLAYING)
			{
				crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1 - 10, h.redTurnoverMg1, masterGain*h.redFactor);
				alSourcef(source[0], AL_GAIN, fade);//0
				alSourcef(source[1], AL_GAIN, rise);//1
			}
			else
				alSourcef(source[1], AL_GAIN, masterGain*h.redFactor);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
			if (sr.reduktor_gl_obor >= 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["red_on"], sr.reduktor_gl_obor));
			else
				alSourcef(source[0], AL_PITCH, 1);
			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)


		}
		//��1�� <-> ��2��
		if (sr.reduktor_gl_obor > h.redTurnoverMg1 && sr.reduktor_gl_obor <= h.redTurnoverMg2)
		{
			if (red_key[1] != '2')
			{

				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);

				if (!setBuffer(buffer[1], h.fullName["red_w_w"], channelsSetup, channel))
					return 0;

				//alutLoadWAVFile(h.path["red_w_w"], &format, &data, &size, &freq, &loop);				// �������� 0-45
				//alBufferData(Buffer[1], format, data, size, freq);
				//alutUnloadWAV(format, data, size, freq);

				alSourcei(source[1], AL_BUFFER, buffer[1]);
				//alSourcefv(Source[1], AL_POSITION, position);
				//alSourcefv(Source[1], AL_VELOCITY, velocity);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);

				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.redFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
				red_key[1] = '2';

			}
			if (red_key[0] != '3')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);

				if (!setBuffer(buffer[0], h.fullName["red_w_mg_w"], channelsSetup, channel))
					return 0;

				//alutLoadWAVFile(h.path["red_w_mg_w"], &format, &data, &size, &freq, &loop);				// �������� 0-45
				//alBufferData(Buffer[0], format, data, size, freq);
				//alutUnloadWAV(format, data, size, freq);

				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);//
																					 //alSourcefv(Source[0], AL_POSITION, position);
																					 //alSourcefv(Source[0], AL_VELOCITY, velocity);
				alSourcei(source[0], AL_LOOPING, AL_TRUE);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				red_key[0] = '3';
			}

			float fade, rise;
			crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1, h.redTurnoverMg2, masterGain*h.redFactor);
			alSourcef(source[1], AL_GAIN, fade);//0
			alSourcef(source[0], AL_GAIN, rise);//1

			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);
			alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
		}
		//��2�� <-> ���
		if (sr.reduktor_gl_obor > h.redTurnoverMg2)
		{
			if (red_key[1] != '4')
			{
				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);

				if (!setBuffer(buffer[1], h.fullName["red_w_avt_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[1], AL_BUFFER, buffer[1]);
				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//
																					 //alSourcefv(Source[1], AL_POSITION, position);
																					 //alSourcefv(Source[1], AL_VELOCITY, velocity);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
				red_key[1] = '4';
			}
			if (red_key[0] != '3')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);

				if (!setBuffer(buffer[0], h.fullName["red_w_mg_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);//
																					 //alSourcefv(Source[0], AL_POSITION, position);
																					 //alSourcefv(Source[0], AL_VELOCITY, velocity);
				alSourcei(source[0], AL_LOOPING, AL_TRUE);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				red_key[0] = '3';
			}

			float fade, rise;
			crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg2, h.redTurnoverAvt, masterGain*h.redFactor);
			alSourcef(source[0], AL_GAIN, fade);//
			alSourcef(source[1], AL_GAIN, rise);//

			alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);
			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
		}
		//��1�� -> 0
		if (!sr.p_eng1_zap && !sr.p_eng2_zap && sr.reduktor_gl_obor > 0 && sr.reduktor_gl_obor < h.redTurnoverMg1 - 1)
		{
			if (red_key[1] != '2')
			{

				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);

				if (!setBuffer(buffer[1], h.fullName["red_w_w"], channelsSetup, channel))
					return 0;

				//alutLoadWAVFile(h.path["red_w_w"], &format, &data, &size, &freq, &loop);				// �������� 0-45
				//alBufferData(Buffer[1], format, data, size, freq);
				//alutUnloadWAV(format, data, size, freq);

				alSourcei(source[1], AL_BUFFER, buffer[1]);
				//alSourcefv(Source[1], AL_POSITION, position);
				//alSourcefv(Source[1], AL_VELOCITY, velocity);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);

				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.redFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
				red_key[1] = '2';

			}
			if (red_key[0] != '5')
			{

				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);

				if (!setBuffer(buffer[0], h.fullName["red_off_w"], channelsSetup, channel))
					return 0;

				//alutLoadWAVFile(h.path["red_off_w"], &format, &data, &size, &freq, &loop);				// �������� 0-45
				//alBufferData(Buffer[0], format, data, size, freq);
				//alutUnloadWAV(format, data, size, freq);

				alSourcei(source[0], AL_BUFFER, buffer[0]);
				//alSourcefv(Source[0], AL_POSITION, position);
				//alSourcefv(Source[0], AL_VELOCITY, velocity);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOff = getOffset(1, h.fullName["red_off"], sr.reduktor_gl_obor);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOff);
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["red_off"], sr.reduktor_gl_obor));
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				red_key[0] = '5';
			}
			//alSourcefv(Source[1], AL_POSITION, position);
			//alSourcefv(Source[1], AL_VELOCITY, velocity);

			float fade, rise;
			crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1, h.redTurnoverMg1 - 7., masterGain*h.redFactor);
			alSourcef(source[1], AL_GAIN, fade);//
			alSourcef(source[0], AL_GAIN, rise);//

			alSourcei(source[0], AL_LOOPING, AL_FALSE);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);

			if (sr.reduktor_gl_obor > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["red_off"], sr.reduktor_gl_obor));
			else
				alSourcef(source[0], AL_PITCH, 1);
		}

		alGetSourcef(source[1], AL_PITCH, &pitch);//
		alGetSourcef(source[1], AL_GAIN, &gain);//
	}

	//��� �������� ��������� = 0 � ������������ ����������, ������������ ���������
	if (sr.reduktor_gl_obor == 0 && !sr.p_eng1_zap && !sr.p_eng2_zap)
	{
		alSourceStop(source[0]);
		alSourceStop(source[1]);
		alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
		alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
		red_key[0] = ' ';
		red_key[1] = ' ';
	}

	//������ 8 ���5, 8 ����
	if (h.modelName == "mi_8_amtsh" || h.modelName == "mi_8_mtv5")
	{
		if (sr.reduktor_gl_obor > h.redTurnoverAvt - 2)
		{
			//��������� ��� ��� �������� ������� ������
			if (pinkNoise != "set")
			{
				alSourceStop(source[2]);
				alSourcei(source[2], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[2]);
				alGenBuffers(1, &buffer[2]);

				if (!setBuffer(buffer[2], h.fullName["pinkNoise"], channelsSetup, channel))
					return 0;
				alSourcef(source[2], AL_GAIN, pinkNoiseGain);
				alSourcei(source[2], AL_BUFFER, buffer[2]);
				alSourcei(source[2], AL_LOOPING, AL_TRUE);
				alSourcePlay(source[2]);
				pinkNoise = "set";
			}

			averangeCalcPeriod += deltaTime;
			if (high > 0)
			{
				averangeCalcPeriodStep += deltaTime;
			}
			else
			{
				averangeCalcPeriodStep = 0;
				vectorStep.clear();
			}

			//���������� ��������� ����
			if (abs(velocityX) < 41.67)
			{
				pinkNoiseGain = pow(10, ((41.67 - abs(velocityX)) / -2.78) * 2 * 0.05) * 0.25;
			}
			else
			{
				pinkNoiseGain = squareInterpolation(41.67, 0.25, 50, 0.5, 69.4, 1, abs(velocityX));
			}
			alSourcef(source[2], AL_GAIN, pinkNoiseGain);//230��.�

			if (averangeCalcPeriod >= 30 && !vector.empty())
				vector.erase(vector.begin());
			vector.push_back(sr.reduktor_gl_obor);
			for (auto& x : vector)
				vectorElemSumm += x;
			averangeTurn = vectorElemSumm / vector.size();
			vectorElemSumm = 0;
			//����� �������� �� �������� ���� 28�/�
			if (abs(velocityX) >= 28)
				velocityGain = (abs(velocityX) - 28)* 0.15;//0.15�� �� 1 �/�
			else
				velocityGain = 0;

			////���������� �� �� 400�� �� ���������
			//accelerationGain = averangeAcc * (-3.75);
			//if (accelerationGain < -3)
			//	accelerationGain = -3;
			//else if (accelerationGain > 0)
			//	accelerationGain = 0;
			//�������� �� ����
			if (high > 0)
			{
				if (averangeCalcPeriodStep >= 35 && !vectorStep.empty())
					vectorStep.erase(vectorStep.begin());
				vectorStep.push_back(step);
				for (auto& x : vectorStep)
					vectorElemSummStep += x;
				averangeStep = vectorElemSummStep / vectorStep.size();
				vectorElemSummStep = 0;
			}
			stepGain = 0.75 * (step - averangeStep) * lineInterpolation(0, 0, 1, 1, high);//

			//�������� �� ���� � ��
			mid2FreqStepGain = step * 0.6 * lineInterpolation(0, 1, 10, 0, high);//~3��

			//�������� ��� ������
			//highGain = squareInterpolation(0, 0, 5, 3, 10, 0, high);//3��

			 //�������� �� �������� ���� 10000
			highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1.5;
			highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;
			//�������� �� ��������
			turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75;

			lowFreqGain = pow(10, (/*turnGain + stepGain + velocityGain + lowFreqVelocityGain + highGain +*/ mid2FreqStepGain /*+ accelerationGain*/)*0.05);
			mid1FreqGain = pow(10, (turnGain + stepGain + velocityGain + mid2FreqStepGain/*+ lowFreqVelocityGain*/)*0.05);
			mid2FreqGain = pow(10, (turnGain + stepGain + velocityGain /*+ lowFreqVelocityGain*/)*0.05);
			highFreqGain = pow(10, (turnGain + stepGain + velocityGain + highFreqTurnGain)*0.05);

			lowCutoffFreq = 250;//�� 50-800
			mid1CutoffFreq = 400;//������ 1 200-3000
			mid2CutoffFreq = 3000;//������ 2 1000-8000
			highCutoffFreq = 10000;//�� 4000-16000

			for (size_t i = 0; i < 2; i++)
			{
				alEffectf(effect[i], AL_EQUALIZER_LOW_CUTOFF, lowCutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_MID1_CENTER, mid1CutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_MID2_CENTER, mid2CutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_HIGH_CUTOFF, highCutoffFreq);

				alEffectf(effect[i], AL_EQUALIZER_LOW_GAIN, lowFreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_MID1_GAIN, mid1FreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_MID2_GAIN, mid2FreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_HIGH_GAIN, highFreqGain);//

				alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
																					   //alSource3i(source[i], AL_AUXILIARY_SEND_FILTER, effectSlot[i], 0, filterWet[i]);
			}
		}
	}
	//������ �� 29
	if (h.modelName == "ka_29")
	{
		if (sr.reduktor_gl_obor > h.redTurnoverAvt - 2)
		{
			//��������� ��� ��� �������� ������� ������
			if (pinkNoise != "set")
			{
				alSourceStop(source[2]);
				alSourcei(source[2], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[2]);
				alGenBuffers(1, &buffer[2]);

				if (!setBuffer(buffer[2], h.fullName["pinkNoise"], channelsSetup, channel))
					return 0;
				alSourcef(source[2], AL_GAIN, pinkNoiseGain);
				alSourcei(source[2], AL_BUFFER, buffer[2]);
				alSourcei(source[2], AL_LOOPING, AL_TRUE);
				alSourcePlay(source[2]);
				pinkNoise = "set";
			}

			averangeCalcPeriod += deltaTime;
			//�������� ������ ��� �������� �������� �� �������� ����, ���� ��������� � �������, ������� ������ �� �����
			if (high > 0)
			{
				averangeCalcPeriodStep += deltaTime;
			}
			else
			{
				averangeCalcPeriodStep = 0;
				vectorStep.clear();
			}

			//���������� ��������� ����
			pinkNoiseGain = pow(10, ((69.4 - abs(velocityX)) * (-0.86)) * 0.05);
			alSourcef(source[2], AL_GAIN, pinkNoiseGain);//230��.�
														 //�������� ������ ��� �������� �������� �� �������� �������� �������� ��������� �� 30�
			if (averangeCalcPeriod >= 30 && !vector.empty())
				vector.erase(vector.begin());
			vector.push_back(sr.reduktor_gl_obor);
			for (auto& x : vector)
				vectorElemSumm += x;
			averangeTurn = vectorElemSumm / vector.size();
			vectorElemSumm = 0;

			//����� �������� �� �������� ���� 50�/�
			if (abs(velocityX) >= 50)
			{
				velocityGain = (abs(velocityX) - 50)* 0.2;//0.1�� �� 1 �/�
			}
			else
			{
				velocityGain = 0;
			}
			//�������� ������ ��� �������� �������� �� �������� �������� ���� �� 50�
			if (high > 0)
			{
				if (averangeCalcPeriodStep >= 50 && !vectorStep.empty())// 20 -> 50
					vectorStep.erase(vectorStep.begin());
				vectorStep.push_back(step);
				for (auto& x : vectorStep)
					vectorElemSummStep += x;
				averangeStep = vectorElemSummStep / vectorStep.size();
				vectorElemSummStep = 0;
			}
			//������������ �������� �� ��������
			stepGain = (step - averangeStep) * lineInterpolation(0, 0, 1, 1, high);

			//�������� �� ���� � ��
			mid2FreqStepGain = step * lineInterpolation(0, 1, 5, 0, high);//0.3 -> 0.2

																				//�������� �� ���� � ������� �������
			float absStepGain = step * squareInterpolation(0, 1, 10.5, 0.5, 27.78, 0, abs(velocityX));

			//�������� �� �������� ���� 10000
			highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1;
			highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;
			//�������� �� ��������
			turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75;

			
			//�������� �� ����� ��� ������� �� ������� 2�� �������
			float flapCGain = 0;
			//���� ����� ������� ������� � ���� �� ����
			if (abs(dash) > 0.8)
			{
				if (((velocityX < 0 && accelerationX > 0.56) || (velocityX > 0 && accelerationX < -0.56)) && abs(velocityX) <= 16.67)
				{
					//flapCGain = ((abs(accelerationX) - 0.56) * 4)*(1 - exp((-tay) / 0.5));//RC ����
					flapCGain = ((abs(accelerationX) - 0.56) * 4) *squareInterpolation(-0.25, 0, 0.5, 0.5, 0.25, 1, velocityY);//��������� � �������� �� �� vy
					flapCGain = tay * ((flapCGain > 4) ? 4 : flapCGain);
					tay += deltaTime;
					tay = (tay > 1) ? 1 : tay;
				}
				else
				{
					//flapCGain = 0;
					//tay = 0;
					tay -= deltaTime;
					tay = (tay < 0) ? 0 : tay;
				}
			}
			else
			{
				if (((velocityX < 0 && accelerationX > 0.56) || (velocityX > 0 && accelerationX < -0.56)) && abs(velocityX) <= 16.67)
				{
					flapCGain = (abs(accelerationX) - 0.56) * 4 * squareInterpolation(-0.25, 0, 0.5, 0.5, 0.25, 1, velocityY);//��������� � �������� �� �� vy
					flapCGain = (flapCGain > 4) ? 4 : flapCGain;
				}
				else
				{
					flapCGain = 0;
				}
			}
			

			lowFreqGain = pow(10, (turnGain + stepGain * 0.15 + absStepGain * 0.1 + mid2FreqStepGain * 0.3 + flapCGain + velocityGain)*0.05); //0.15 -> 0.15
			mid1FreqGain = pow(10, (turnGain + stepGain * 0.2 + absStepGain * 0.1 + mid2FreqStepGain * 0.2 + flapCGain)*0.05);//0.3 -> 0.2
			mid2FreqGain = pow(10, (turnGain + stepGain * 0.3 + absStepGain * 0.1 + velocityGain * 0.75)*0.05);//0.4 -> 0.3
			highFreqGain = pow(10, (turnGain + stepGain * 0.5 + absStepGain * 0.3 + highFreqTurnGain)*0.05);//

			lowFreqGain = (lowFreqGain <= 1) ? 1 : lowFreqGain;
			mid1FreqGain = (mid1FreqGain <= 1) ? 1 : mid1FreqGain;
			mid2FreqGain = (mid2FreqGain <= 1) ? 1 : mid2FreqGain;
			highFreqGain = (highFreqGain <= 1) ? 1 : highFreqGain;

			lowCutoffFreq = 400;//�� 50-800
			mid1CutoffFreq = 1000;//������ 1 200-3000
			mid2CutoffFreq = 3000;//������ 2 1000-8000
			highCutoffFreq = 10000;//�� 4000-16000

			for (size_t i = 0; i < 2; i++)
			{
				alEffectf(effect[i], AL_EQUALIZER_LOW_CUTOFF, lowCutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_MID1_CENTER, mid1CutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_MID2_CENTER, mid2CutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_HIGH_CUTOFF, highCutoffFreq);

				alEffectf(effect[i], AL_EQUALIZER_LOW_GAIN, lowFreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_MID1_GAIN, mid1FreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_MID2_GAIN, mid2FreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_HIGH_GAIN, highFreqGain);//

				alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
																					   //alSource3i(source[i], AL_AUXILIARY_SEND_FILTER, effectSlot[i], 0, filterWet[i]);
			}
			outputPeriod += deltaTime;
			if (outputPeriod >= 1)
			{
				fred = fopen("red.txt", "at");
				fprintf(fred, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", sr.reduktor_gl_obor, averangeTurn, turnGain, highFreqTurnGain, stepGain, velocityGain, accelerationX, pinkNoiseGain, mid2FreqStepGain, accelerationGain, lowFreqGain, mid1FreqGain, mid2FreqGain, flapCGain, soundread.time);
				fclose(fred);
				outputPeriod = 0;
			}
			printf("step = %10.3f averangeStep = %10.3f velocityY = %10.3f accelerationX = %10.3f tay = %10.3f flapCGain = %10.3f\r", step, averangeStep, velocityY, accelerationX, tay, flapCGain);
		}
	}
	//������ �� 27
	if (h.modelName == "ka_27")
	{
		if (sr.reduktor_gl_obor > h.redTurnoverAvt - 2)
		{
			//��������� ��� ��� �������� ������� ������
			if (pinkNoise != "set")
			{
				alSourceStop(source[2]);
				alSourcei(source[2], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[2]);
				alGenBuffers(1, &buffer[2]);

				if (!setBuffer(buffer[2], h.fullName["pinkNoise"], channelsSetup, channel))
					return 0;
				alSourcef(source[2], AL_GAIN, pinkNoiseGain);
				alSourcei(source[2], AL_BUFFER, buffer[2]);
				alSourcei(source[2], AL_LOOPING, AL_TRUE);
				alSourcePlay(source[2]);
				pinkNoise = "set";
			}

			averangeCalcPeriod += deltaTime;
			//�������� ������ ��� �������� �������� �� �������� ����, ���� ��������� � �������, ������� ������ �� �����
			if (high > 0)
			{
				averangeCalcPeriodStep += deltaTime;
			}
			else
			{
				averangeCalcPeriodStep = 0;
				vectorStep.clear();
			}

			//���������� ��������� ����
			pinkNoiseGain = pow(10, ((69.4 - abs(velocityX)) * (-0.86)) * 0.05);
			alSourcef(source[2], AL_GAIN, pinkNoiseGain);//230��.�
			//�������� ������ ��� �������� �������� �� �������� �������� �������� ��������� �� 30�
			if (averangeCalcPeriod >= 30 && !vector.empty())
				vector.erase(vector.begin());
			vector.push_back(sr.reduktor_gl_obor);
			for (auto& x : vector)
				vectorElemSumm += x;
			averangeTurn = vectorElemSumm / vector.size();
			vectorElemSumm = 0;

			//�������� �� �������� ���� 50��/� (14�/c)
			if (abs(velocityX) < 42)
			{
				velocityGain = lineInterpolation(14, 0, 42, 3, abs(velocityX));
			}
			else if (abs(velocityX) >= 42 && abs(velocityX) < 56)
			{
				velocityGain = lineInterpolation(42, 3, 56, 7, abs(velocityX));
			}
			else 
			{
				velocityGain = 7 + (abs(velocityX) - 56) * 0.071;
				velocityGain = (velocityGain > 9) ? 9 : velocityGain;
			}

			//�������� ������ ��� �������� �������� �� �������� �������� ���� �� 50�
			if (high > 0)
			{
				if (averangeCalcPeriodStep >= 50 && !vectorStep.empty())// 20 -> 50
					vectorStep.erase(vectorStep.begin());
				vectorStep.push_back(step);
				for (auto& x : vectorStep)
					vectorElemSummStep += x;
				averangeStep = vectorElemSummStep / vectorStep.size();
				vectorElemSummStep = 0;
			}
			//������������ �������� �� ��������
			stepGain = (step - averangeStep) * lineInterpolation(0, 0, 1, 1, high);

			//�������� �� ���� � ��
			mid2FreqStepGain = step * lineInterpolation(0, 1, 5, 0, high);//0.3 -> 0.2

																		  //�������� �� ���� � ������� �������
			float absStepGain = step * squareInterpolation(0, 1, 10.5, 0.5, 27.78, 0, abs(velocityX));

			//�������� �� �������� ���� 10000
			highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1;
			highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;
			//�������� �� ��������
			turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75;


			//�������� �� ����� ��� ������� �� ������� 2�� �������
			float flapCGain = 0;
			//���� ����� ������� ������� � ���� �� ����
			if (abs(dash) > 0.8)
			{
				if (((velocityX < 0 && accelerationX > 0.56) || (velocityX > 0 && accelerationX < -0.56)) && abs(velocityX) <= 16.67)
				{
					//flapCGain = ((abs(accelerationX) - 0.56) * 4)*(1 - exp((-tay) / 0.5));//RC ����
					flapCGain = ((abs(accelerationX) - 0.56) * 4) *squareInterpolation(-0.25, 0, 0.5, 0.5, 0.25, 1, velocityY);//��������� � �������� �� �� vy
					flapCGain = tay * ((flapCGain > 4) ? 4 : flapCGain);
					tay += deltaTime;
					tay = (tay > 1) ? 1 : tay;
				}
				else
				{
					//flapCGain = 0;
					//tay = 0;
					tay -= deltaTime;
					tay = (tay < 0) ? 0 : tay;
				}
			}
			else
			{
				if (((velocityX < 0 && accelerationX > 0.56) || (velocityX > 0 && accelerationX < -0.56)) && abs(velocityX) <= 16.67)
				{
					flapCGain = (abs(accelerationX) - 0.56) * 4 * squareInterpolation(-0.25, 0, 0.5, 0.5, 0.25, 1, velocityY);//��������� � �������� �� �� vy
					flapCGain = (flapCGain > 4) ? 4 : flapCGain;
				}
				else
				{
					flapCGain = 0;
				}
			}


			lowFreqGain = pow(10, (turnGain + stepGain * 0.15 + absStepGain * 0.1 + mid2FreqStepGain * 0.3 + flapCGain + velocityGain)*0.05); //0.15 -> 0.15
			mid1FreqGain = pow(10, (turnGain + stepGain * 0.2 + absStepGain * 0.1 + mid2FreqStepGain * 0.2 + flapCGain)*0.05);//0.3 -> 0.2
			mid2FreqGain = pow(10, (turnGain + stepGain * 0.3 + absStepGain * 0.1 + velocityGain * 0.75)*0.05);//0.4 -> 0.3
			highFreqGain = pow(10, (turnGain + stepGain * 0.5 + absStepGain * 0.3 + highFreqTurnGain)*0.05);//

			lowFreqGain = (lowFreqGain <= 1) ? 1 : lowFreqGain;
			mid1FreqGain = (mid1FreqGain <= 1) ? 1 : mid1FreqGain;
			mid2FreqGain = (mid2FreqGain <= 1) ? 1 : mid2FreqGain;
			highFreqGain = (highFreqGain <= 1) ? 1 : highFreqGain;

			lowCutoffFreq = 400;//�� 50-800
			mid1CutoffFreq = 1000;//������ 1 200-3000
			mid2CutoffFreq = 3000;//������ 2 1000-8000
			highCutoffFreq = 10000;//�� 4000-16000

			for (size_t i = 0; i < 2; i++)
			{
				alEffectf(effect[i], AL_EQUALIZER_LOW_CUTOFF, lowCutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_MID1_CENTER, mid1CutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_MID2_CENTER, mid2CutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_HIGH_CUTOFF, highCutoffFreq);

				alEffectf(effect[i], AL_EQUALIZER_LOW_GAIN, lowFreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_MID1_GAIN, mid1FreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_MID2_GAIN, mid2FreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_HIGH_GAIN, highFreqGain);//

				alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
																					   //alSource3i(source[i], AL_AUXILIARY_SEND_FILTER, effectSlot[i], 0, filterWet[i]);
			}
			outputPeriod += deltaTime;
			if (outputPeriod >= 1)
			{
				fred = fopen("red.txt", "at");
				fprintf(fred, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", sr.reduktor_gl_obor, averangeTurn, turnGain, highFreqTurnGain, stepGain, velocityGain, accelerationX, pinkNoiseGain, mid2FreqStepGain, accelerationGain, lowFreqGain, mid1FreqGain, mid2FreqGain, flapCGain, soundread.time);
				fclose(fred);
				outputPeriod = 0;
			}
			//printf("step = %10.3f averangeStep = %10.3f turnGain = %10.3f stepGain = %10.3f absStepGain = %10.3f mid2FreqStepGain = %10.3f\r", step, averangeStep, turnGain, stepGain, absStepGain, mid2FreqStepGain);
		}
	}
	return 1;
}

int Engine::Play(bool status_on, bool status_off, float parameter,SOUNDREAD sr, Helicopter h)
{
	for (size_t i = 0; i < 2; i++)
	{
		if (eq_key[i] != 'q')
		{
			alEffecti(effect[i], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//���������� ������ ��� ����������
			alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
			alFilteri(filter[i], AL_FILTER_TYPE, AL_FILTER_LOWPASS);//���������� ������ ��� ��
			alFilterf(filter[i], AL_LOWPASS_GAIN, 0);//������� ���� ������� ���������
			alSource3i(source[i], AL_AUXILIARY_SEND_FILTER, effectSlot[i], 0, NULL);//�������� ����� ��������� ����� ���� � ��������
			alSourcei(source[i], AL_DIRECT_FILTER, filter[i]);//���������� ������ � ���������
			eq_key[i] = 'q';
		}
	}

	//��������� ������
	if (h.modelName == "ansat")
	{
		//0 -> ��
		if (status_on && parameter < h.engTurnoverMg)
		{
			if (eng_key[0] != '1')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);
				if (!setBuffer(buffer[0], h.fullName["eng_on_w"], channelsSetup, channel))
					return 0;

				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcef(source[0], AL_GAIN, masterGain*h.engFactor);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOn = getOffset(1, h.fullName["eng_on"], parameter);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOn);

				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);// ���������� ������� 
				eng_key[0] = '1';
			}
			if (sourceStatus[0] == AL_PLAYING)
			{
				alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
				if (parameter >= 5)
					alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["eng_on"], parameter));
				else
					alSourcef(source[0], AL_PITCH, 1);

				if (parameter < (h.engTurnoverMg - 10))
				{
					alSourcef(source[0], AL_GAIN, masterGain*h.engFactor);
				}

				//alSourcefv(Source[0], AL_POSITION, position);
				//alSourcefv(Source[0], AL_VELOCITY, velocity);
				alGetSourcef(source[0], AL_PITCH, &pitch);//���� ��� �������
			}
			if (sourceStatus[0] != AL_PLAYING && eng_key[0] == '1')
			{
				if (eng_key[1] != '2')//���������� - ����������� - ��������� ������
				{
					alSourceStop(source[1]);
					alSourcei(source[1], AL_BUFFER, NULL);
					alDeleteBuffers(1, &buffer[1]);
					alGenBuffers(1, &buffer[1]);
					if (!setBuffer(buffer[1], h.fullName["eng_w_w"], channelsSetup, channel))
						return 0;
					alSourcei(source[1], AL_BUFFER, buffer[1]);
					alSourcei(source[1], AL_LOOPING, AL_TRUE);
					alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
					alSourcePlay(source[1]);
					alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
					eng_key[1] = '2';
				}
				if (sourceStatus[1] == AL_PLAYING && eng_key[1] == '2')//���� ������ ������������� - ������ �� �� �����������
				{
					alSourcef(source[1], AL_GAIN, masterGain*h.engFactor);
					alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				}
			}

		}
		//0 -> ��
		if (parameter > (h.engTurnoverMg - 10) && parameter <= h.engTurnoverMg && status_on)
		{
			if (eng_key[0] != '1')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);
				if (!setBuffer(buffer[0], h.fullName["eng_on_w"], channelsSetup, channel))
					return 0;

				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);

				offsetOn = getOffset(1, h.fullName["eng_on"], parameter);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOn);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				eng_key[0] = '1';
			}
			if (eng_key[1] != '2')//���������� - ����������� - ��������� ������
			{
				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);
				if (!setBuffer(buffer[1], h.fullName["eng_w_w"], channelsSetup, channel))
					return 0;

				alSourcei(source[1], AL_BUFFER, buffer[1]);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);
				alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.engFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
				eng_key[1] = '2';
			}

			float fade, rise;
			if (sourceStatus[0] == AL_PLAYING)
			{
				crossFade(&fade, &rise, parameter, h.engTurnoverMg - 10, h.engTurnoverMg, masterGain*h.engFactor);
				alSourcef(source[0], AL_GAIN, fade);
				alSourcef(source[1], AL_GAIN, rise);
			}
			else
				alSourcef(source[1], AL_GAIN, masterGain*h.engFactor);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
			if (parameter > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["eng_on"], parameter));
			else
				alSourcef(source[0], AL_PITCH, 1);

			alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)


		}
		//�� -> 0
		if (status_off && parameter > 0 && parameter < h.engTurnoverMg - 2)
		{
			if (eng_key[1] != '2')
			{

				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);
				if (!setBuffer(buffer[1], h.fullName["eng_w_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[1], AL_BUFFER, buffer[1]);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);

				alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.engFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
				eng_key[1] = '2';
			}
			if (eng_key[0] != '5')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);
				if (!setBuffer(buffer[0], h.fullName["eng_off_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOff = getOffset(1, h.fullName["eng_off"], parameter);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOff);
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["eng_off"], parameter));
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				eng_key[0] = '5';
			}

			float fade, rise;
			crossFade(&fade, &rise, parameter, h.engTurnoverMg, h.engTurnoverMg - 7., masterGain*h.engFactor);
			alSourcef(source[1], AL_GAIN, fade);
			alSourcef(source[0], AL_GAIN, rise);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);

			if (parameter > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["eng_off"], parameter));
			else
				alSourcef(source[0], AL_PITCH, 1);
		}

		alGetSourcef(source[1], AL_PITCH, &pitch);//
		alGetSourcef(source[1], AL_GAIN, &pitch);//
	}
	else
	{
		//0 -> ��
		if (status_on && parameter < h.engTurnoverMg)
		{
			if (eng_key[0] != '1')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);
				if (!setBuffer(buffer[0], h.fullName["eng_on_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcef(source[0], AL_GAIN, masterGain*h.engFactor);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOn = getOffset(1, h.fullName["eng_on"], parameter);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOn);

				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);// ���������� ������� 
				eng_key[0] = '1';
			}
			if (sourceStatus[0] == AL_PLAYING)
			{
				alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
				if (parameter >= 5)
					alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["eng_on"], parameter));
				else
					alSourcef(source[0], AL_PITCH, 1);

				if (parameter < (h.engTurnoverMg - 10))
				{
					alSourcef(source[0], AL_GAIN, masterGain*h.engFactor);
				}
				alGetSourcef(source[0], AL_PITCH, &pitch);//���� ��� �������
			}
			if (sourceStatus[0] != AL_PLAYING && eng_key[0] == '1')
			{
				if (eng_key[1] != '2')//���������� - ����������� - ��������� ������
				{
					alSourceStop(source[1]);
					alSourcei(source[1], AL_BUFFER, NULL);
					alDeleteBuffers(1, &buffer[1]);
					alGenBuffers(1, &buffer[1]);
					if (!setBuffer(buffer[1], h.fullName["eng_w_w"], channelsSetup, channel))
						return 0;
					alSourcei(source[1], AL_BUFFER, buffer[1]);
					alSourcei(source[1], AL_LOOPING, AL_TRUE);
					alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
					alSourcePlay(source[1]);
					alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
					eng_key[1] = '2';
				}
				if (sourceStatus[1] == AL_PLAYING && eng_key[1] == '2')//���� ������ ������������� - ������ �� �� �����������
				{
					alSourcef(source[1], AL_GAIN, masterGain*h.engFactor);
					alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				}
			}
		}
		//0 -> ��
		if (parameter > (h.engTurnoverMg - 10) && parameter <= h.engTurnoverMg && status_on)
		{
			if (eng_key[0] != '1')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);
				if (!setBuffer(buffer[0], h.fullName["eng_on_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);

				offsetOn = getOffset(1, h.fullName["eng_on"], parameter);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOn);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				eng_key[0] = '1';
			}
			if (eng_key[1] != '2')//���������� - ����������� - ��������� ������
			{
				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);
				if (!setBuffer(buffer[1], h.fullName["eng_w_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[1], AL_BUFFER, buffer[1]);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);

				alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.engFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
				eng_key[1] = '2';
			}

			float fade, rise;
			if (sourceStatus[0] == AL_PLAYING)
			{
				crossFade(&fade, &rise, parameter, h.engTurnoverMg - 10, h.engTurnoverMg, masterGain*h.engFactor);
				alSourcef(source[0], AL_GAIN, fade);
				alSourcef(source[1], AL_GAIN, rise);
			}
			else
				alSourcef(source[1], AL_GAIN, masterGain*h.engFactor);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);

			if (parameter > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["eng_on"], parameter));
			else
				alSourcef(source[0], AL_PITCH, 1);
			alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
		}
		//�� <-> ���
		if (parameter > h.engTurnoverMg)
		{
			if (eng_key[1] != '2')
			{

				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);
				if (!setBuffer(buffer[1], h.fullName["eng_w_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[1], AL_BUFFER, buffer[1]);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);

				alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.engFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
				eng_key[1] = '2';
			}
			if (eng_key[0] != '3')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);

				if (!setBuffer(buffer[0], h.fullName["eng_w_avt_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcef(source[0], AL_PITCH, parameter / h.engTurnoverAvt);//
				alSourcef(source[0], AL_GAIN, masterGain*h.engFactor);
				alSourcei(source[0], AL_LOOPING, AL_TRUE);
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				eng_key[0] = '3';
				//alSource3i(Source[0], AL_AUXILIARY_SEND_FILTER, NULL, 0, NULL);
			}

			float fade, rise;
			crossFade(&fade, &rise, parameter, h.engTurnoverMg, h.engTurnoverAvt, masterGain*h.engFactor);
			alSourcef(source[1], AL_GAIN, fade);
			alSourcef(source[0], AL_GAIN, rise);

			alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);
			alSourcef(source[0], AL_PITCH, parameter / h.engTurnoverAvt);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
		}
		//�� -> 0
		if (status_off && parameter > 0 && parameter < h.engTurnoverMg - 2)
		{
			if (eng_key[1] != '2')
			{

				alSourceStop(source[1]);
				alSourcei(source[1], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[1]);
				alGenBuffers(1, &buffer[1]);
				if (!setBuffer(buffer[1], h.fullName["eng_w_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[1], AL_BUFFER, buffer[1]);
				alSourcei(source[1], AL_LOOPING, AL_TRUE);

				alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//������ pitch (���� ���������� ������ ��� ��������� ������� ��)
				alSourcef(source[1], AL_GAIN, masterGain*h.engFactor);
				alSourcePlay(source[1]);
				alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
				eng_key[1] = '2';
			}
			if (eng_key[0] != '5')
			{
				alSourceStop(source[0]);
				alSourcei(source[0], AL_BUFFER, NULL);
				alDeleteBuffers(1, &buffer[0]);
				alGenBuffers(1, &buffer[0]);
				if (!setBuffer(buffer[0], h.fullName["eng_off_w"], channelsSetup, channel))
					return 0;
				alSourcei(source[0], AL_BUFFER, buffer[0]);
				alSourcei(source[0], AL_LOOPING, AL_FALSE);
				offsetOff = getOffset(1, h.fullName["eng_off"], parameter);
				alSourcef(source[0], AL_SEC_OFFSET, offsetOff);
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["eng_off"], parameter));
				alSourcePlay(source[0]);
				alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
				eng_key[0] = '5';
			}

			float fade, rise;
			crossFade(&fade, &rise, parameter, h.engTurnoverMg, h.engTurnoverMg - 7., masterGain*h.engFactor);
			alSourcef(source[1], AL_GAIN, fade);
			alSourcef(source[0], AL_GAIN, rise);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);

			if (parameter > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["eng_off"], parameter));
			else
				alSourcef(source[0], AL_PITCH, 1);
		}

		alGetSourcef(source[0], AL_PITCH, &pitch);//
		alGetSourcef(source[0], AL_GAIN, &gain);//
	}

	//��������� ��������� ���� ������� ����� 0 � ��������� �� �����������
	if (!status_on && parameter == 0)
	{
		alSourceStop(source[0]);
		alSourceStop(source[1]);
		alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
		alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� ������� 
		eng_key[0] = ' ';
		eng_key[1] = ' ';
	}
	//������ 8 ���5, 8 ����, �� 27�, �� 29
	if (h.modelName == "mi_8_amtsh" || h.modelName == "mi_8_mtv5" || h.modelName == "ka_27" || h.modelName == "ka_29")
	{
		if (parameter > h.engTurnoverAvt - 2)
		{
			averangeCalcPeriod += deltaTime;
			if (averangeCalcPeriod >= 25 && !vector.empty())
				vector.erase(vector.begin());
			vector.push_back(parameter);
			for (auto& x : vector)
				vectorElemSumm += x;
			averangeTurn = vectorElemSumm / vector.size();
			vectorElemSumm = 0;

			//�������� �� �������� ���� 10000
			highFreqTurnGain = (parameter - averangeTurn) * 1.5;
			highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;
			//�������� �� ��������
			turnGain = (parameter - averangeTurn) * 0.35;

			lowFreqGain = pow(10, (turnGain)*0.05);
			mid1FreqGain = pow(10, (turnGain)*0.05);
			mid2FreqGain = pow(10, (turnGain)*0.05);
			highFreqGain = pow(10, (turnGain + highFreqTurnGain)*0.05);

			lowCutoffFreq = AL_EQUALIZER_DEFAULT_LOW_CUTOFF;
			mid1CutoffFreq = AL_EQUALIZER_DEFAULT_MID1_CENTER;
			mid2CutoffFreq = AL_EQUALIZER_DEFAULT_MID2_CENTER;
			highCutoffFreq = 10000;

			for (size_t i = 0; i < 2; i++)
			{
				alEffectf(effect[i], AL_EQUALIZER_LOW_CUTOFF, lowCutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_MID1_CENTER, mid1CutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_MID2_CENTER, mid2CutoffFreq);
				alEffectf(effect[i], AL_EQUALIZER_HIGH_CUTOFF, highCutoffFreq);

				alEffectf(effect[i], AL_EQUALIZER_LOW_GAIN, lowFreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_MID1_GAIN, mid1FreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_MID2_GAIN, mid2FreqGain);//
				alEffectf(effect[i], AL_EQUALIZER_HIGH_GAIN, highFreqGain);//

				alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
			}
		}
	}
	
	return 1;
}

int VintFlap::Play(Helicopter h, SOUNDREAD sr)
{
	//������ 8 ���5, 8 ����
	if (h.modelName == "mi_8_amtsh" || h.modelName == "mi_8_mtv5")
	{

		if (flap_key[1] != '2')
		{
			alSourceStop(source[1]);
			alSourcei(source[1], AL_BUFFER, NULL);
			alDeleteBuffers(1, &buffer[1]);
			alGenBuffers(1, &buffer[1]);
			if (!setBuffer(buffer[1], h.fullName["vint_flap"], channelsSetup, channel))//�����������
				return 0;
			alSourcei(source[1], AL_BUFFER, buffer[1]);
			alSourcef(source[1], AL_GAIN, 1 * h.vintFlapFactor);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);

			alEffecti(effect[1], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//���������� ������ ��� ����������
			alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[1]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
			alFilteri(filter[1], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[1], AL_LOWPASS_GAIN, 0);
			alSource3i(source[1], AL_AUXILIARY_SEND_FILTER, effectSlot[1], 0, NULL);
			alSourcei(source[1], AL_DIRECT_FILTER, filter[1]);

			alSourcePlay(source[1]);
			alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
			flap_key[1] = '2';

		}

		averangeCalcPeriod += deltaTime;
		if (averangeCalcPeriod >= 30 && !vector.empty())
			vector.erase(vector.begin());
		vector.push_back(sr.reduktor_gl_obor);
		for (auto& x : vector)
			vectorElemSumm += x;
		averangeTurn = vectorElemSumm / vector.size();
		vectorElemSumm = 0;

		float gain_a = 0;
		float h_g = 0;
		float v_g = 0;

		h_g = squareInterpolation(0, 0, 0.5, 0.5, 1, 1, high);
		v_g = squareInterpolation(14, 0, 17, 0.5, 20, 1, abs(velocityX));//72(1) - 50(0)
		gain_a = squareInterpolation(-1, -18, 1, -12, 3, -6, calcA);

		//�������� �� ��������
		if (calcA >= 2)
		{
			turnsGain = (sr.reduktor_gl_obor - averangeTurn) * 3;
		}
		else
		{
			turnsGain = 0;
		}
		float m = pow(10, (turnsGain + gain_a)*0.05) * h_g * v_g;

		alSourcef(source[1], AL_GAIN, m);
	}
	//������ �� 29
	if (h.modelName == "ka_29")
	{
		if (flap_key[2] != '1' || flap_key[1] != '2' || flap_key[0] != '3')
		{
			//��������� �� ������
			alSourceStop(source[2]);
			alSourcei(source[2], AL_BUFFER, NULL);
			alDeleteBuffers(1, &buffer[2]);
			alGenBuffers(1, &buffer[2]);
			if (!setBuffer(buffer[2], h.fullName["vint_flap_C"], channelsSetup, channel))//�����������
				return 0;
			alSourcei(source[2], AL_BUFFER, buffer[2]);
			alSourcef(source[2], AL_GAIN, 0);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);
			//��������� ������������� �� ������
			alSourceStop(source[1]);
			alSourcei(source[1], AL_BUFFER, NULL);
			alDeleteBuffers(1, &buffer[1]);
			alGenBuffers(1, &buffer[1]);
			if (!setBuffer(buffer[1], h.fullName["vint_flap_B"], channelsSetup, channel))//�����������
				return 0;
			alSourcei(source[1], AL_BUFFER, buffer[1]);
			alSourcef(source[1], AL_GAIN, 0);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alEffecti(effect[1], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//���������� ������ ��� ����������
			alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[1]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
			alFilteri(filter[1], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[1], AL_LOWPASS_GAIN, 0);
			alSource3i(source[1], AL_AUXILIARY_SEND_FILTER, effectSlot[1], 0, NULL);
			alSourcei(source[1], AL_DIRECT_FILTER, filter[1]);
			//��������� ����������� �� ������
			alSourceStop(source[0]);
			alSourcei(source[0], AL_BUFFER, NULL);
			alDeleteBuffers(1, &buffer[0]);
			alGenBuffers(1, &buffer[0]);
			if (!setBuffer(buffer[0], h.fullName["vint_flap_A"], channelsSetup, channel))//�������������
				return 0;
			alSourcei(source[0], AL_BUFFER, buffer[0]);
			alSourcef(source[0], AL_GAIN, 0);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			alEffecti(effect[0], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//���������� ������ ��� ����������
			alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
			alFilteri(filter[0], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[0], AL_LOWPASS_GAIN, 0);
			alSource3i(source[0], AL_AUXILIARY_SEND_FILTER, effectSlot[0], 0, NULL);
			alSourcei(source[0], AL_DIRECT_FILTER, filter[0]);
			//��������� ������������ (�����������)
			alSourcePlay(source[0]);
			alSourcePlay(source[1]);
			alSourcePlay(source[2]);
			alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
			alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
			alGetSourcei(source[2], AL_SOURCE_STATE, &sourceStatus[2]);		// ���������� �������
			flap_key[0] = '3';
			flap_key[1] = '2';
			flap_key[2] = '1';
		}
		flapIndicator = 0;
		//������� �������
		//��� ������������� �������� ������� ����������
		if (velocityX < 0)
		{
			//dv>1 � vy<-2
			if (accelerationX >= -accelerationXBorder && velocityY <= velocityYBorder)//1
			{
				if ((calcA > 0) & (dash < -dashBorder) & (accelerationVy < 3))
				{
					flapIndicator = 1;
				}
			}
			//dv>1 � vy>-2
			if (accelerationX >= -accelerationXBorder && velocityY > velocityYBorder)//2
			{

				if ((accelerationX > 0.56 & velocityY < 0) || ((velocityY > 4) & (dash > 0.5) & (accelerationVy > 3)))
				{
					flapIndicator = 2;
				}
			}
			//
			if (accelerationX < -accelerationXBorder && velocityY <= velocityYBorder)//3
			{
				if ((accelerationX < -1.4) & (abs(derivStep) > 2))
				{
					flapIndicator = 3;
				}
			}
			//
			if (accelerationX < -accelerationXBorder && velocityY > velocityYBorder)//4
			{
				if ((accelerationX < -0.56) & ((velocityY > 2) | ((abs(velocityX) > 50) & (step > 20))))
				{
					flapIndicator = 4;
				}
			}
		}
		else
		{
			//dv<1 � vy<-2
			if (accelerationX <= accelerationXBorder && velocityY <= velocityYBorder)//1
			{
				if ((calcA > 0) & (dash > dashBorder) & (accelerationVy < 3))
				{
					flapIndicator = 1;
				}
			}
			//dv<1 � vy>-2
			if (accelerationX <= accelerationXBorder && velocityY > velocityYBorder)//2
			{

				if ((accelerationX < -0.56 & velocityY < 0) || ((velocityY > 4) & (dash < -0.5) & (accelerationVy > 3)))
				{
					flapIndicator = 2;
				}
			}
			//
			if (accelerationX > accelerationXBorder && velocityY <= velocityYBorder)//3
			{
				if ((accelerationX > 1.4) & (abs(derivStep) > 2))
				{
					flapIndicator = 3;
				}
			}
			//
			if (accelerationX > accelerationXBorder && velocityY > velocityYBorder)//4
			{
				if ((accelerationX > 0.56) & ((velocityY > 2) | ((abs(velocityX) > 50) & (step > 20))))
				{
					flapIndicator = 4;
				}
			}
		}

		//��������� �������� ����� ������� �� �������� ���������
		lowerFreqLimit = log10(1000);//��������� � �������� �����
		highterFreqLimit = log10(6000);
		if (sr.reduktor_gl_obor >= 89 & sr.reduktor_gl_obor <= 90)
		{
			freqCutoffFromTurns = lineInterpolation(89, lowerFreqLimit, 90, highterFreqLimit, sr.reduktor_gl_obor);
		}
		else if (sr.reduktor_gl_obor > 90)
		{
			freqCutoffFromTurns = (sr.reduktor_gl_obor / (90. / 100.))*highterFreqLimit / 100.;
		}
		else
		{
			freqCutoffFromTurns = lowerFreqLimit;
		}
		freqCutoffFromTurns = pow(10, freqCutoffFromTurns);//������������ � ������
														   //���� ����������� ������� �������, ������� ������
		if (flapIndicator)//�������
		{
			offsetOn += deltaTime;
			offsetOn = (offsetOn > 1) ? 1 : offsetOn;//������ ������� ��������� �� 1�
		}
		else
		{
			offsetOn -= deltaTime;
			offsetOn = (offsetOn < 0) ? 0 : offsetOn;
		}
		lowFreqGain = 1;
		mid1FreqGain = squareInterpolation(log10(1000), 0.126, log10(2000), 0.5, log10(3000), 1, log10(freqCutoffResult));
		mid2FreqGain = squareInterpolation(log10(2000), 0.126, log10(3000), 0.5, log10(4000), 1, log10(freqCutoffResult));//���� 4� �������� �������� �������
		highFreqGain = 0.126;

		lowCutoff = 800;
		mid1Cutoff = 2000;//1-3
		mid2Cutoff = 3000;//4-6
		highCutoff = (freqCutoffResult > AL_EQUALIZER_MIN_HIGH_CUTOFF) ? freqCutoffResult : AL_EQUALIZER_MIN_HIGH_CUTOFF;//���� 4� ������� ������� �����

		alEffectf(effect[0], AL_EQUALIZER_LOW_CUTOFF, lowCutoff);
		alEffectf(effect[0], AL_EQUALIZER_MID1_CENTER, mid1Cutoff);
		alEffectf(effect[0], AL_EQUALIZER_MID2_CENTER, mid2Cutoff);
		alEffectf(effect[0], AL_EQUALIZER_HIGH_CUTOFF, highCutoff);

		alEffectf(effect[0], AL_EQUALIZER_LOW_GAIN, lowFreqGain);//
		alEffectf(effect[0], AL_EQUALIZER_MID1_GAIN, mid1FreqGain);//
		alEffectf(effect[0], AL_EQUALIZER_MID2_GAIN, mid2FreqGain);//
		alEffectf(effect[0], AL_EQUALIZER_HIGH_GAIN, highFreqGain);//

		alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
		alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[0]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)

																			   //������� � ��������� �� ������� � ��������� ������� ������������ ���������� � ������� ���������
		accelerationGain = (3 * abs(accelerationX)) - 15;
		accelerationGain = (accelerationGain > 5) ? -accelerationGain : accelerationGain;//��
		accelerationGain = pow(10, accelerationGain * 0.05);//����

		hiSpeedGain = lineInterpolation(50, 0, 66.66, 1, abs(velocityX));/*���� 200��� ������ �� ���������, ���� - � ����� ������*/

		resFlapCGain = (hiSpeedGain > accelerationGain) ? hiSpeedGain : accelerationGain;//�� ���� ����������� ��������� �� ������� �������� �������������

		if (sr.reduktor_gl_obor <= 91)
		{
			turnsGain = (91 - sr.reduktor_gl_obor) * (-3);//������������ �������� �� �������� ����� - 3�� �� ������
		}

		//������� ��������� �������
		float off = (accelerationX > 0) ? ((abs(velocityX)>14) ? 1 : 0) : lineInterpolation(14, 1, 0, 0, abs(velocityX));
		//������� ������ �����������-������������� �������
		float flapA = 0;
		float flapB = 0;
		if (velocityX < 0)
		{
			flapA = (accelerationX >= -accelerationXBorder) ? 1 : 0;//������� ����������� �������
			flapB = (accelerationX < -accelerationXBorder) ? 1 : 0;//������� ������������� �������
		}
		else
		{
			flapA = (accelerationX <= accelerationXBorder) ? 1 : 0;//������� ����������� �������
			flapB = (accelerationX > accelerationXBorder) ? 1 : 0;//������� ������������� �������
		}

		//������� ������� ����� �� � �� �������� �� ����
		float flapABStep = 0;
		float flapCStep = 0;
		crossFade(&flapCStep, &flapABStep, step, 8, 12, 1);
		//������� ������ ���� �� ��� �������,���������� �� ��������, ��� ��� ���� �������� ����� ������������
		if (abs(velocityX) < 14)
		{
			flapABStep = 1;
			flapCStep = 1;
		}
		//������� ������� ����� �� � �� �������� �� ��������
		float flapABVX = 0;
		float flapCVX = 0;
		crossFade(&flapCVX, &flapABVX, abs(velocityX), 15.28, 16.67, 1);

		//��� ������ ������� ���������� ��������� � �������� ���������� ������� �������
		float flapCGainAccX = 1;
		if (flapIndicator == 2 && abs(velocityX) < 16.67)
		{
			flapCGainAccX = lineInterpolation(0.56, 0, 1, 1, abs(accelerationX)) * squareInterpolation(-0.25, 1, 0.5, 0.5, 0.25, 0, velocityY);//��������� � �������� �� �� vy
		}
		//������������ �������������� ��������� ������� � ������ ������ �������
		float flapAGain = flapA * offsetOn * off * masterGain * h.vintFlapFactor * flapABStep * flapABVX * pow(10, turnsGain*0.05);
		float flapBGain = flapB * offsetOn * off * masterGain * h.vintFlapFactor * flapABStep * flapABVX * pow(10, turnsGain*0.05);
		float flapCGain = ((flapIndicator) ? (flapCGainAccX * flapCStep * flapCVX * offsetOn * off * pow(10, turnsGain*0.05) * masterGain * (h.vintFlapFactor + (1 - h.vintFlapFactor)*0.5)) : (masterGain * (h.vintFlapFactor + (1 - h.vintFlapFactor)*0.5) * (1 - offsetOn) * resFlapCGain * off));

		alSourcef(source[0], AL_GAIN, flapAGain);//�����������
		alSourcef(source[1], AL_GAIN, flapBGain);//�������������
		alSourcef(source[2], AL_GAIN, flapCGain);//�����

												 //������� ������ � ����� ��� � ������������� ������
		outputPeriod += deltaTime;
		if (outputPeriod >= 1)
		{
			fflaps = fopen("flap.txt", "at");
			fprintf(fflaps, "%i\t%f\t%f\t%f\t%f\t%f\n", flapIndicator, flapAGain, flapBGain, flapCGain, calcA, soundread.time);
			fclose(fflaps);
			fderiv = fopen("der.txt", "at");
			fprintf(fderiv, "%f\t%f\t%f\t%f\t%f\t%f\t%f\n", accelerationX, dash, velocityY, accelerationVy, derivStep, calcA, soundread.time);
			fclose(fderiv);
			ffront = fopen("front.txt", "at");
			fprintf(ffront, "%f\t%f\t%f\t%f\t%f\t%f\t%f\n", accelerationX, vectorVx.front(), velocityX, periodCalc, vectorTime.front(), currentTime, soundread.time);
			fclose(ffront);
			outputPeriod = 0;
		}
		//printf(" Acc = %8.3f AccVy = %8.3f calcA = %8.3f vY = %8.3f vX = %8.3f tangaz = %8.3f Dash = %8.3f flapI = %i flap_a = %1.3f flap_b= %1.3f flap_c = %1.3f offset = %1.3f \r", accelerationX, accelerationVy, calcA, velocityY, velocityX, tangaz, dash, flapIndicator, flapAGain, flapBGain, flapCGain, offsetOn);
	}
	//������ �� 27
	if (h.modelName == "ka_27")
	{
		if (flap_key[2] != '1' || flap_key[1] != '2' || flap_key[0] != '3')
		{
			//��������� �� ������
			alSourceStop(source[2]);
			alSourcei(source[2], AL_BUFFER, NULL);
			alDeleteBuffers(1, &buffer[2]);
			alGenBuffers(1, &buffer[2]);
			if (!setBuffer(buffer[2], h.fullName["vint_flap_C"], channelsSetup, channel))//�����������
				return 0;
			alSourcei(source[2], AL_BUFFER, buffer[2]);
			alSourcef(source[2], AL_GAIN, 0);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);
			//��������� ������������� �� ������
			alSourceStop(source[1]);
			alSourcei(source[1], AL_BUFFER, NULL);
			alDeleteBuffers(1, &buffer[1]);
			alGenBuffers(1, &buffer[1]);
			if (!setBuffer(buffer[1], h.fullName["vint_flap_B"], channelsSetup, channel))//�����������
				return 0;
			alSourcei(source[1], AL_BUFFER, buffer[1]);
			alSourcef(source[1], AL_GAIN, 0);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alEffecti(effect[1], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//���������� ������ ��� ����������
			alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[1]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
			alFilteri(filter[1], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[1], AL_LOWPASS_GAIN, 0);
			alSource3i(source[1], AL_AUXILIARY_SEND_FILTER, effectSlot[1], 0, NULL);
			alSourcei(source[1], AL_DIRECT_FILTER, filter[1]);
			//��������� ����������� �� ������
			alSourceStop(source[0]);
			alSourcei(source[0], AL_BUFFER, NULL);
			alDeleteBuffers(1, &buffer[0]);
			alGenBuffers(1, &buffer[0]);
			if (!setBuffer(buffer[0], h.fullName["vint_flap_A"], channelsSetup, channel))//�������������
				return 0;
			alSourcei(source[0], AL_BUFFER, buffer[0]);
			alSourcef(source[0], AL_GAIN, 0);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			alEffecti(effect[0], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//���������� ������ ��� ����������
			alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
			alFilteri(filter[0], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[0], AL_LOWPASS_GAIN, 0);
			alSource3i(source[0], AL_AUXILIARY_SEND_FILTER, effectSlot[0], 0, NULL);
			alSourcei(source[0], AL_DIRECT_FILTER, filter[0]);
			//��������� ������������ (�����������)
			alSourcePlay(source[0]);
			alSourcePlay(source[1]);
			alSourcePlay(source[2]);
			alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� ������� 
			alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
			alGetSourcei(source[2], AL_SOURCE_STATE, &sourceStatus[2]);		// ���������� �������
			flap_key[0] = '3';
			flap_key[1] = '2';
			flap_key[2] = '1';
		}
		flapIndicator = 0;
		//������� �������
		//��� ������������� �������� ������� ����������
		if (velocityX < 0)
		{
			//dv>1 � vy<-2
			if (accelerationX >= -accelerationXBorder && velocityY <= velocityYBorder)//1
			{
				if ((calcA > 0) & (dash < -dashBorder) & (accelerationVy < 3))
				{
					flapIndicator = 1;
				}
			}
			//dv>1 � vy>-2
			if (accelerationX >= -accelerationXBorder && velocityY > velocityYBorder)//2
			{

				if ((accelerationX > 0.56 & velocityY < 0) || ((velocityY > 4) & (dash > 0.5) & (accelerationVy > 3)))
				{
					flapIndicator = 2;
				}
			}
			//
			if (accelerationX < -accelerationXBorder && velocityY <= velocityYBorder)//3
			{
				if ((accelerationX < -1.4) & (abs(derivStep) > 2))
				{
					flapIndicator = 3;
				}
			}
			//
			if (accelerationX < -accelerationXBorder && velocityY > velocityYBorder)//4
			{
				if ((accelerationX < -0.56) & ((velocityY > 2) | ((abs(velocityX) > 50) & (step > 20))))
				{
					flapIndicator = 4;
				}
			}
		}
		else
		{
			//dv<1 � vy<-2
			if (accelerationX <= accelerationXBorder && velocityY <= velocityYBorder)//1
			{
				if ((calcA > 0) & (dash > dashBorder) & (accelerationVy < 3))
				{
					flapIndicator = 1;
				}
			}
			//dv<1 � vy>-2
			if (accelerationX <= accelerationXBorder && velocityY > velocityYBorder)//2
			{

				if ((accelerationX < -0.56 & velocityY < 0) || ((velocityY > 4) & (dash < -0.5) & (accelerationVy > 3)))
				{
					flapIndicator = 2;
				}
			}
			//
			if (accelerationX > accelerationXBorder && velocityY <= velocityYBorder)//3
			{
				if ((accelerationX > 1.4) & (abs(derivStep) > 2))
				{
					flapIndicator = 3;
				}
			}
			//
			if (accelerationX > accelerationXBorder && velocityY > velocityYBorder)//4
			{
				if ((accelerationX > 0.56) & ((velocityY > 2) | ((abs(velocityX) > 50) & (step > 20))))
				{
					flapIndicator = 4;
				}
			}
		}
		
		//��������� �������� ����� ������� �� �������� ���������
		lowerFreqLimit = log10(1000);//��������� � �������� �����
		highterFreqLimit = log10(6000);
		if (sr.reduktor_gl_obor >= 89 & sr.reduktor_gl_obor <= 90)
		{
			freqCutoffFromTurns = lineInterpolation(89, lowerFreqLimit, 90, highterFreqLimit, sr.reduktor_gl_obor);
		}
		else if (sr.reduktor_gl_obor > 90)
		{
			freqCutoffFromTurns = (sr.reduktor_gl_obor / (90. / 100.))*highterFreqLimit / 100.;
		}
		else
		{
			freqCutoffFromTurns = lowerFreqLimit;
		}
		freqCutoffFromTurns = pow(10, freqCutoffFromTurns);//������������ � ������
														   //���� ����������� ������� �������, ������� ������
		if (flapIndicator)//�������
		{
			offsetOn += deltaTime;
			offsetOn = (offsetOn > 1) ? 1 : offsetOn;//������ ������� ��������� �� 1�
		}
		else
		{
			offsetOn -= deltaTime;
			offsetOn = (offsetOn < 0) ? 0 : offsetOn;
		}
		lowFreqGain = 1;
		mid1FreqGain = squareInterpolation(log10(1000), 0.126, log10(2000), 0.5, log10(3000), 1, log10(freqCutoffResult));
		mid2FreqGain = squareInterpolation(log10(2000), 0.126, log10(3000), 0.5, log10(4000), 1, log10(freqCutoffResult));//���� 4� �������� �������� �������
		highFreqGain = 0.126;

		lowCutoff = 800;
		mid1Cutoff = 2000;//1-3
		mid2Cutoff = 3000;//4-6
		highCutoff = (freqCutoffResult > AL_EQUALIZER_MIN_HIGH_CUTOFF) ? freqCutoffResult : AL_EQUALIZER_MIN_HIGH_CUTOFF;//���� 4� ������� ������� �����

		alEffectf(effect[0], AL_EQUALIZER_LOW_CUTOFF, lowCutoff);
		alEffectf(effect[0], AL_EQUALIZER_MID1_CENTER, mid1Cutoff);
		alEffectf(effect[0], AL_EQUALIZER_MID2_CENTER, mid2Cutoff);
		alEffectf(effect[0], AL_EQUALIZER_HIGH_CUTOFF, highCutoff);

		alEffectf(effect[0], AL_EQUALIZER_LOW_GAIN, lowFreqGain);//
		alEffectf(effect[0], AL_EQUALIZER_MID1_GAIN, mid1FreqGain);//
		alEffectf(effect[0], AL_EQUALIZER_MID2_GAIN, mid2FreqGain);//
		alEffectf(effect[0], AL_EQUALIZER_HIGH_GAIN, highFreqGain);//

		alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)
		alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[0]);//�������� ������ � ���� (� 1 ���� ����� ��������� 1 ������)

		//������� � ��������� �� ������� � ��������� ������� ������������ ���������� � ������� ���������
		accelerationGain = (3 * abs(accelerationX)) - 15;
		accelerationGain = (accelerationGain > 5) ? -accelerationGain : accelerationGain;//��
		accelerationGain = pow(10, accelerationGain * 0.05);//����

		hiSpeedGain = lineInterpolation(50, 0, 66.66, 1, abs(velocityX));/*���� 200��� ������ �� ���������, ���� - � ����� ������*/

		resFlapCGain = (hiSpeedGain > accelerationGain) ? hiSpeedGain : accelerationGain;//�� ���� ����������� ��������� �� ������� �������� �������������

		if (sr.reduktor_gl_obor <= 91)
		{
			turnsGain = (91 - sr.reduktor_gl_obor) * (-3);//������������ �������� �� �������� ����� - 3�� �� ������
		}

		//������� ��������� �������
		float off = (accelerationX > 0)? ((abs(velocityX)>14)? 1:0 ) : lineInterpolation(14, 1, 0, 0, abs(velocityX));
		//������� ������ �����������-������������� �������
		float flapA = 0;
		float flapB = 0;
		if (velocityX < 0)
		{
			flapA = (accelerationX >= -accelerationXBorder) ? 1 : 0;//������� ����������� �������
			flapB = (accelerationX < -accelerationXBorder) ? 1 : 0;//������� ������������� �������
		}
		else
		{
			flapA = (accelerationX <= accelerationXBorder) ? 1 : 0;//������� ����������� �������
			flapB = (accelerationX > accelerationXBorder) ? 1 : 0;//������� ������������� �������
		}
															
		//������� ������� ����� �� � �� �������� �� ����
		float flapABStep = 0;
		float flapCStep = 0;
		crossFade(&flapCStep, &flapABStep, step, 8, 12, 1);
		//������� ������ ���� �� ��� �������,���������� �� ��������, ��� ��� ���� �������� ����� ������������
		if (abs(velocityX) < 14)
		{
			flapABStep = 1;
			flapCStep = 1;
		}
		//������� ������� ����� �� � �� �������� �� ��������
		float flapABVX = 0;
		float flapCVX = 0;
		crossFade(&flapCVX, &flapABVX, abs(velocityX), 15.28, 16.67, 1);
		
		//��� ������ ������� ���������� ��������� � �������� ���������� ������� �������
		float flapCGainAccX = 1;
		if (flapIndicator == 2 && abs(velocityX) < 16.67)
		{
			flapCGainAccX = lineInterpolation(0.56, 0, 1, 1, abs(accelerationX)) * squareInterpolation(-0.25, 1, 0.5, 0.5, 0.25, 0, velocityY);//��������� � �������� �� �� vy
		}
		//������������ �������������� ��������� ������� � ������ ������ �������
		float flapAGain = flapA * offsetOn * off * masterGain * h.vintFlapFactor * flapABStep * flapABVX * pow(10, turnsGain*0.05);
		float flapBGain = flapB * offsetOn * off * masterGain * h.vintFlapFactor * flapABStep * flapABVX * pow(10, turnsGain*0.05);
		float flapCGain = ((flapIndicator) ? (flapCGainAccX * flapCStep * flapCVX * offsetOn * off * pow(10, turnsGain*0.05) * masterGain * (h.vintFlapFactor + (1 - h.vintFlapFactor)*0.5)) : (masterGain * (h.vintFlapFactor + (1 - h.vintFlapFactor)*0.5) * (1 - offsetOn) * resFlapCGain * off));

		alSourcef(source[0], AL_GAIN, flapAGain);//�����������
		alSourcef(source[1], AL_GAIN, flapBGain);//�������������
		alSourcef(source[2], AL_GAIN, flapCGain);//�����

												 //������� ������ � ����� ��� � ������������� ������
		outputPeriod += deltaTime;
		if (outputPeriod >= 1)
		{
			fflaps = fopen("flap.txt", "at");
			fprintf(fflaps, "%i\t%f\t%f\t%f\t%f\t%f\n", flapIndicator, flapAGain, flapBGain, flapCGain, calcA, soundread.time);
			fclose(fflaps);
			fderiv = fopen("der.txt", "at");
			fprintf(fderiv, "%f\t%f\t%f\t%f\t%f\t%f\t%f\n", accelerationX, dash, velocityY, accelerationVy, derivStep, calcA, soundread.time);
			fclose(fderiv);
			ffront = fopen("front.txt", "at");
			fprintf(ffront, "%f\t%f\t%f\t%f\t%f\t%f\t%f\n", accelerationX, vectorVx.front(),velocityX,periodCalc,vectorTime.front(),currentTime,soundread.time);
			fclose(ffront);
			outputPeriod = 0;
		}
		//printf(" Acc = %8.3f AccVy = %8.3f calcA = %8.3f vY = %8.3f vX = %8.3f tangaz = %8.3f Dash = %8.3f flapI = %i flap_a = %1.3f flap_b= %1.3f flap_c = %1.3f offset = %1.3f \r", accelerationX, accelerationVy, calcA, velocityY, velocityX, tangaz, dash, flapIndicator, flapAGain, flapBGain, flapCGain, offsetOn);
	}
	return 1;
}

int Runway::Play(Helicopter h, SOUNDREAD sr)
{
	//��������� ����������� ������
	if (load[1] != "loaded")
	{
		alSourceStop(source[1]);
		alSourcei(source[1], AL_BUFFER, NULL);
		alDeleteBuffers(1, &buffer[1]);
		alGenBuffers(1, &buffer[1]);
		if (!setBuffer(buffer[1], h.fullName["runway"], channelsSetup, channel))//�����������
			return 0;
		alSourcei(source[1], AL_BUFFER, buffer[1]);
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		alSourcePlay(source[1]);
		alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// ���������� �������
		load[1] = "loaded";
	}
	//��������� ����������� ������
	if (load[0] != "loaded")
	{
		alSourceStop(source[0]);
		alSourcei(source[0], AL_BUFFER, NULL);
		alDeleteBuffers(1, &buffer[0]);
		alGenBuffers(1, &buffer[0]);
		if (!setBuffer(buffer[0], h.fullName["runway"] /*h.fullName["planeLikeTakeoffMi8"]*/, channelsSetup, channel))//����������� //���� ���� �����������
			return 0;
		alSourcei(source[0], AL_BUFFER, buffer[0]);
		alSourcei(source[0], AL_LOOPING, AL_TRUE);
		alSourcePlay(source[0]);
		alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// ���������� �������
		load[0] = "loaded";
	}

	float fade, rise;
	crossFade(&fade, &rise, abs(velocityX), 8.33, 13.88, masterGain * h.runwayFactor);
	alSourcef(source[1], AL_GAIN, fade * lineInterpolation(0, 0, 8.33, 1, abs(velocityX)));//
	alSourcef(source[0], AL_GAIN, rise * lineInterpolation(0, 0, 8.33, 1, abs(velocityX)));//

	return 1;
}

float attack(float velocityX, float velocityXPrevious, float tangaz, float deltaHigh, float periodCalc)
{
	float calcA = 0;
	if ((velocityX + velocityXPrevious) == 0)
	{
		if (deltaHigh < 0)
		{
			calcA = 90;
		}
		else if (deltaHigh > 0)
		{
			calcA = -90;
		}
		else
		{
			calcA = 0;
		}
	}
	else if (tangaz == 90 || tangaz == -90)
	{
		if (deltaHigh != 0)
		{
			calcA = 0;
		}
		else
		{
			calcA = 90;
		}
	}
	else
	{
		tangaz = tangaz * M_PI / 180.0;
		calcA = atan(tan(tangaz) - (2 * deltaHigh) / ((velocityXPrevious + velocityX) * cos(tangaz) * periodCalc));
		calcA = calcA * 180 / M_PI;
	}
	return calcA;
}

Sound::Sound() : sourceStatus(new int), source(new ALuint), buffer(new ALuint), effectSlot(new ALuint), effect(new ALuint), filter(new ALuint)
{
	try
	{
		if (sourcesInUse == maxSources)
			throw 0;
	}
	catch (int z)
	{
		cout << " Cant gen more sources...\n" << endl;
	}
	alGenSources(1, &source[0]);
	alGenEffects(1, &effect[0]);
	alGenBuffers(1, &buffer[0]);
	alGenFilters(1, &filter[0]);
	sourcesInUse++;
}

Sound::Sound(int n, int ns) : sourceStatus(new int[n]), source(new ALuint[n]), buffer(new ALuint[n]), effectSlot(new ALuint[ns]), effect(new ALuint[n]), filter(new ALuint[n])
{
	try
	{
		if (sourcesInUse == maxSources)
			throw 0;
		if (sourcesInUse == maxSlots)
			throw 1;
	}
	catch (int z)
	{
		switch (z)
		{
		case 0: cout << " Cant gen more sources...\n" << endl; break;
		case 1: cout << " Cant gen more aux slots...\n" << endl; break;
		}
	}
	sourceNumber = n;
	for (size_t i = 0; i < n; i++)
	{
		alGenSources(1, &source[i]);
		alGenEffects(1, &effect[i]);
		alGenBuffers(1, &buffer[i]);
		alGenFilters(1, &filter[i]);
	}
	for (size_t i = 0; i < ns; i++)
		alGenAuxiliaryEffectSlots(1, &effectSlot[i]);
	effectSlotNumber = ns;
	sourcesInUse += n;
	effectSlotsInUse += ns;
}

Sound::~Sound()
{
	for (size_t i = 0; i < sourceNumber; i++)
	{
		alDeleteEffects(1, &effect[i]);
		alDeleteFilters(1, &filter[i]);
		alDeleteSources(1, &source[i]);
		alDeleteBuffers(1, &buffer[i]);
	}
	for (size_t i = 0; i < effectSlotNumber; i++)
		alDeleteAuxiliaryEffectSlots(1, &effect[i]);
	sourcesInUse -= sourceNumber;
	effectSlotsInUse -= effectSlotNumber;
}
//���������� ������������ ��������� WAVE �����
float Sound::getLengthWAV(string filename)
{
	WAVEHEADER *header;
	FILE *in;
	in = fopen(filename.c_str(), "rb");
	if (!in)
	{
		cout << "\n file [" << filename << "] is missing" << endl;
		system("cls");
		return 0;
	}
	header = new WAVEHEADER;
	fread(header, sizeof(WAVEHEADER), 1, in);//��������� ������������ ����������
	float length = ((float)header->lDataSize / ((float)header->wfex.wBitsPerSample * (float)header->wfex.nSamplesPerSec)) * 8;//��������� ������ � ��������
	fclose(in);
	delete header;
	return length;
}

bool IsProcessPresent(wchar_t * szExe)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe);

	if (!_wcsicmp((wchar_t *)&pe.szExeFile, szExe))
	{
		return true;
	}

	while (Process32Next(hSnapshot, &pe))
	{
		if (!_wcsicmp((wchar_t *)&pe.szExeFile, szExe))
		{
			return true;
		}
	}

	return false;
}