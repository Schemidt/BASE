#pragma once
#pragma pack ( push, 1 )
#include "string.h"
#include "map"
using namespace std;


#ifndef Helicopter_h
#define Helicopter_h
/*!
\brief Класс параметров вертолета
\author Самсонов А.В
\version 3.0
\date Ноябрь 2017 года

Класс определяющая формат хранения некоторых констант, определяющих особенности вертолета,
а также адреса файлов переходных функций некоторых параметров необходимые для корректной работы программы и адреса звуковых файлов .WAV
*/
class Helicopter
{
public:

	string modelName;//!<Название и модификация вертолета
	//Константы
	float engTurnoverHp = 0; //!<обороты двигателя на холодной прокрутке
	float engTurnoverMg = 0; //!<обороты двигателя на малом газу
	float engTurnoverAvt = 0; //!<обороты двигателя на режиме автомат
	float redTurnoverMg1 = 0; //!<обороты редуктора с 1им двигателем на малом газу
	float redTurnoverMg2 = 0; //!<обороты редуктора с 2мя двигателями на малом газу
	float redTurnoverAvt = 0; //!<обороты редуктора на режиме автомат
	//Множители громкости
	float vsuFactor = 0;//!<Множитель громкости ВСУ
	float vsuHpFactor = 0;//!<Множитель громкости ВСУ ХП
	float vsuCraneFactor = 0;
	float engFactor = 0;//!<Множитель громкости двигателя
	float engHpFactor = 0;//!<Множитель громкости двигателя
	float redFactor = 0;//!<Множитель громкости редуктора
	float accumFactor = 0;//!<Множитель громкости Аккумулятора
	float ptsFactor = 0;//!<Множитель громкости ПТС
	float ptsToneFactor = 0;//!<Множитель громкости ПТС ТОН
	float tr36Factor = 0;//!<Множитель громкости Трансформатор 36В
	float tr115Factor = 0;//!<Множитель громкости Трансформатор 115В
	float pumpLeftFactor = 0;//!<Множитель громкости насоса подкачки
	float pumpRightFactor = 0;//!<Множитель громкости насоса подкачки
	float cutoffCraneFactor = 0;//!<Множитель громкости перекрывного крана
	float ko50Factor = 0;//!<Множитель громкости KO-50
	float skvFactor = 0;//!<Множитель громкости СКВ
	float runwayFactor = 0;//!<Множитель громкости звука движения по ВПП и РД
	float airNoiseFactor = 0;//!<Множитель громкости аэродинамического шума
	float rocketSturmFactor = 0;//!<Множитель громкости ракеты штурм
	float rocketNar8Factor = 0;//!<Множитель громкости НАР 8
	float rocketNar13Factor = 0;//!<Множитель громкости НАР 13
	float rocketHitFactor = 0;//!<Множитель громкости попадания ракеты
	float upkFactor = 0;//!<Множитель громкости выстрелов УПК
	float ppuFactor = 0;//!<Множитель громкости выстрелов ППУ
	float vintSwishFactor = 0;//!<Множитель громкости винта на низких обортах
	float vintBrakeFactor = 0;//!<Множитель громкости тормоза винта
	float vintFlapFactor = 0;//!<Множитель громкости хлопков винта
	float chassisBrakePumpFactor = 0;//!<Множитель громкости тормоза шасси
	float chassisBrakeReleaseFactor = 0;//!<Множитель громкости сброса давления
	float consumTankFactor = 0;//!<Множитель громкости насоса расходного бака
	float nipFactor = 0;//НИП
	float fireCraneFactor = 0;//Пожарный кран
	float girovertFactor = 0;//Гировертикаль
	float pumpStationFactor = 0;//Насосная станция
	float rainFactor = 0;//Дождь
	float buzzerFactor = 0;//Зуммер
	float fenFactor = 0;//Фен
	float undefinedFactor = 0;//Неизвестный
	float circlingCraneFactor = 0;//Кран кольцевания
	float rocketIglaFactor = 0;//Игла
	float vpryam = 0;//Выпрямительное устройство

	map<string, string> shortName;
	map<string, string> fullName;

	void setPath(string pathToFile);
	void setParam(string model);
	Helicopter();
	~Helicopter();
};

#endif