#pragma once
#pragma pack ( push, 1 )
#include "string.h"
#include "map"
using namespace std;


#ifndef Helicopter_h
#define Helicopter_h
/*!
\brief ����� ���������� ���������
\author �������� �.�
\version 3.0
\date ������ 2017 ����

����� ������������ ������ �������� ��������� ��������, ������������ ����������� ���������,
� ����� ������ ������ ���������� ������� ��������� ���������� ����������� ��� ���������� ������ ��������� � ������ �������� ������ .WAV
*/
class Helicopter
{
public:

	string modelName;//!<�������� � ����������� ���������
	//���������
	float engTurnoverHp = 0; //!<������� ��������� �� �������� ���������
	float engTurnoverMg = 0; //!<������� ��������� �� ����� ����
	float engTurnoverAvt = 0; //!<������� ��������� �� ������ �������
	float redTurnoverMg1 = 0; //!<������� ��������� � 1�� ���������� �� ����� ����
	float redTurnoverMg2 = 0; //!<������� ��������� � 2�� ����������� �� ����� ����
	float redTurnoverAvt = 0; //!<������� ��������� �� ������ �������
	//��������� ���������
	float vsuFactor = 0;//!<��������� ��������� ���
	float vsuHpFactor = 0;//!<��������� ��������� ��� ��
	float vsuCraneFactor = 0;
	float engFactor = 0;//!<��������� ��������� ���������
	float engHpFactor = 0;//!<��������� ��������� ���������
	float redFactor = 0;//!<��������� ��������� ���������
	float accumFactor = 0;//!<��������� ��������� ������������
	float ptsFactor = 0;//!<��������� ��������� ���
	float ptsToneFactor = 0;//!<��������� ��������� ��� ���
	float tr36Factor = 0;//!<��������� ��������� ������������� 36�
	float tr115Factor = 0;//!<��������� ��������� ������������� 115�
	float pumpLeftFactor = 0;//!<��������� ��������� ������ ��������
	float pumpRightFactor = 0;//!<��������� ��������� ������ ��������
	float cutoffCraneFactor = 0;//!<��������� ��������� ������������ �����
	float ko50Factor = 0;//!<��������� ��������� KO-50
	float skvFactor = 0;//!<��������� ��������� ���
	float runwayFactor = 0;//!<��������� ��������� ����� �������� �� ��� � ��
	float airNoiseFactor = 0;//!<��������� ��������� ����������������� ����
	float rocketSturmFactor = 0;//!<��������� ��������� ������ �����
	float rocketNar8Factor = 0;//!<��������� ��������� ��� 8
	float rocketNar13Factor = 0;//!<��������� ��������� ��� 13
	float rocketHitFactor = 0;//!<��������� ��������� ��������� ������
	float upkFactor = 0;//!<��������� ��������� ��������� ���
	float ppuFactor = 0;//!<��������� ��������� ��������� ���
	float vintSwishFactor = 0;//!<��������� ��������� ����� �� ������ �������
	float vintBrakeFactor = 0;//!<��������� ��������� ������� �����
	float vintFlapFactor = 0;//!<��������� ��������� ������� �����
	float chassisBrakePumpFactor = 0;//!<��������� ��������� ������� �����
	float chassisBrakeReleaseFactor = 0;//!<��������� ��������� ������ ��������
	float consumTankFactor = 0;//!<��������� ��������� ������ ���������� ����
	float nipFactor = 0;//���
	float fireCraneFactor = 0;//�������� ����
	float girovertFactor = 0;//�������������
	float pumpStationFactor = 0;//�������� �������
	float rainFactor = 0;//�����
	float buzzerFactor = 0;//������
	float fenFactor = 0;//���
	float undefinedFactor = 0;//�����������
	float circlingCraneFactor = 0;//���� �����������
	float rocketIglaFactor = 0;//����
	float vpryam = 0;//�������������� ����������

	map<string, string> shortName;
	map<string, string> fullName;

	void setPath(string pathToFile);
	void setParam(string model);
	Helicopter();
	~Helicopter();
};

#endif