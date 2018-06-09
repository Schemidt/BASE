/*!
\file
\brief ����� Helicopter / ����������

������ ���� ��������� ����� Helicopter
*/
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
\version 2.0
\date  02.03.2018

����� ������������ ������ �������� ��������� ��������, ������������ ����������� ���������,
� ����� ������ ������ ���������� ������� ��������� ���������� ����������� ��� ���������� ������ ��������� � ������ �������� ������ .WAV
*/
class Helicopter
{
public:

	string modelName;//!<�������� � ����������� ���������
	//���������
	double engTurnoverHp = 25; //!<������� ��������� �� �������� ���������
	double engTurnoverMg = 0; //!<������� ��������� �� ����� ����
	double engTurnoverAvt = 0; //!<������� ��������� �� ������ �������
	double redTurnoverMg1 = 0; //!<������� ��������� � 1�� ���������� �� ����� ����
	double redTurnoverMg2 = 0; //!<������� ��������� � 2�� ����������� �� ����� ����
	double redTurnoverAvt = 0; //!<������� ��������� �� ������ �������
	double vsuTurnoverFull = 100;
	double vsuTurnoverHp = 35;
	//��������� ���������
	double vsuFactor = 0;//!<��������� ��������� ���
	double vsuHpFactor = 0;//!<��������� ��������� ��� ��
	double vsuCraneFactor = 0;
	double engFactor = 0;//!<��������� ��������� ���������
	double engHpFactor = 0;//!<��������� ��������� ���������
	double redFactor = 0;//!<��������� ��������� ���������
	double accumFactor = 0;//!<��������� ��������� ������������
	double ptsFactor = 0;//!<��������� ��������� ���
	double ptsToneFactor = 0;//!<��������� ��������� ��� ���
	double tr36Factor = 0;//!<��������� ��������� ������������� 36�
	double tr115Factor = 0;//!<��������� ��������� ������������� 115�
	double pumpLeftFactor = 0;//!<��������� ��������� ������ ��������
	double pumpRightFactor = 0;//!<��������� ��������� ������ ��������
	double cutoffCraneFactor = 0;//!<��������� ��������� ������������ �����
	double ko50Factor = 0;//!<��������� ��������� KO-50
	double skvFactor = 0;//!<��������� ��������� ���
	double runwayFactor = 1;//!<��������� ��������� ����� �������� �� ��� � ��
	double rocketSturmFactor = 0;//!<��������� ��������� ������ �����
	double rocketNar8Factor = 0;//!<��������� ��������� ��� 8
	double rocketNar13Factor = 0;//!<��������� ��������� ��� 13
	double rocketHitFactor = 0;//!<��������� ��������� ��������� ������
	double upkFactor = 0;//!<��������� ��������� ��������� ���
	double ppuFactor = 0;//!<��������� ��������� ��������� ���
	double vintSwishFactor = 0;//!<��������� ��������� ����� �� ������ �������
	double vintBrakeFactor = 0;//!<��������� ��������� ������� �����
	double vintFlapFactor = 0;//!<��������� ��������� ������� �����
	double chassisBrakePumpFactor = 0;//!<��������� ��������� ������� �����
	double chassisBrakeReleaseFactor = 0;//!<��������� ��������� ������ ��������
	double consumTankFactor = 0;//!<��������� ��������� ������ ���������� ����
	double nipFactor = 0;//!<���
	double fireCraneFactor = 0;//!<�������� ����
	double girovertFactor = 0;//!<�������������
	double pumpStationFactor = 0;//!<�������� �������
	double rainFactor = 1;//!<�����
	double buzzerFactor = 0;//!<������
	double fenFactor = 0;//!<���
	double undefinedFactor = 0;//!<�����������
	double circlingCraneFactor = 0;//!<���� �����������
	double rocketIglaFactor = 0;//!<����
	double vpryam = 0;//!<�������������� ����������
	double vadd = 0;//!<���������� �������
	double shock = 1;//!<���� � ���� ����
	double trim = 0;//!<�������
	double frict = 0;//!>��������

	//!<������������� ������ � ������� ������ - ������
	map<string, string> shortName;

	//!<������������� ������ � ������� ������� ������ - ������
	map<string, string> fullName;

	/*!
	\brief ��������� ������ � ������� ������� ������
	\details ��������� ������ fullName � ������� ������� ������: shortName + pathToFile
	\param[in] pathToFile ���� � �����������
	*/
	void setPath(string pathToFile);

	/*!
	\brief ��������� ��������� ������� ��
	\details ��������� ��������� ������� �� � ����������� �� ����������� ��������� model
	\param[in] model ������ ������������ ��������
	*/
	void setParam(string model);
	Helicopter();
	~Helicopter();
};

#endif