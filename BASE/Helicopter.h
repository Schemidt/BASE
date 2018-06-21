/*!
\file
\brief Класс Helicopter / объявление

Данный файл объявляет класс Helicopter
*/
#pragma once

#include "string"
#include "map"
#include "iostream"
#include "vector"

#include "Utility.h"

#pragma pack ( push, 1 )
using namespace std;

#ifndef Helicopter_h
#define Helicopter_h
/*!
\brief Класс параметров вертолета
\author Самсонов А.В
\version 6.0
\date  02.03.2018

Класс определяющая формат хранения некоторых констант, определяющих особенности вертолета,
а также адреса файлов переходных функций некоторых параметров необходимые для корректной работы программы и адреса звуковых файлов .WAV
*/
class Helicopter
{
public:

	string modelName;//!<Название и модификация вертолета
	//Константы
	double engTurnoverHp = 25; //!<обороты двигателя на холодной прокрутке
	double engTurnoverMg = 0; //!<обороты двигателя на малом газу
	double engTurnoverAvt = 0; //!<обороты двигателя на режиме автомат
	double redTurnoverMg1 = 0; //!<обороты редуктора с 1им двигателем на малом газу
	double redTurnoverMg2 = 0; //!<обороты редуктора с 2мя двигателями на малом газу
	double redTurnoverAvt = 0; //!<обороты редуктора на режиме автомат
	double vsuTurnoverFull = 100;//!<обороты ВСУ на пике мощности
	double vsuTurnoverHp = 35;//!<обороты ВСУ на холодной прокрутке
	//Множители громкости
	double vsuFactor = 0;//!<Множитель громкости ВСУ
	double vsuHpFactor = 0;//!<Множитель громкости ВСУ ХП
	double vsuCraneFactor = 0;//!<Множитель громкости крана ВСУ
	double engFactor = 0;//!<Множитель громкости двигателя
	double engHpFactor = 0;//!<Множитель громкости двигателя
	double redFactor = 0;//!<Множитель громкости редуктора
	double accumFactor = 0;//!<Множитель громкости Аккумулятора
	double ptsFactor = 0;//!<Множитель громкости ПТС
	double ptsToneFactor = 0;//!<Множитель громкости ПТС ТОН
	double tr36Factor = 0;//!<Множитель громкости Трансформатор 36В
	double tr115Factor = 0;//!<Множитель громкости Трансформатор 115В
	double pumpLeftFactor = 0;//!<Множитель громкости насоса подкачки
	double pumpRightFactor = 0;//!<Множитель громкости насоса подкачки
	double cutoffCraneFactor = 0;//!<Множитель громкости перекрывного крана
	double ko50Factor = 0;//!<Множитель громкости KO-50
	double skvFactor = 0;//!<Множитель громкости СКВ
	double runwayFactor = 1;//!<Множитель громкости звука движения по ВПП и РД
	double rocketSturmFactor = 0;//!<Множитель громкости ракеты штурм
	double rocketNar8Factor = 0;//!<Множитель громкости НАР 8
	double rocketNar13Factor = 0;//!<Множитель громкости НАР 13
	double rocketHitFactor = 0;//!<Множитель громкости попадания ракеты
	double upkFactor = 0;//!<Множитель громкости выстрелов УПК
	double ppuFactor = 0;//!<Множитель громкости выстрелов ППУ
	double vintSwishFactor = 0;//!<Множитель громкости винта на низких обортах
	double vintBrakeFactor = 0;//!<Множитель громкости тормоза винта
	double vintFlapFactor = 0;//!<Множитель громкости хлопков винта
	double chassisBrakePumpFactor = 0;//!<Множитель громкости тормоза шасси
	double chassisBrakeReleaseFactor = 0;//!<Множитель громкости сброса давления
	double consumTankFactor = 0;//!<Множитель громкости насоса расходного бака
	double nipFactor = 0;//!<НИП
	double fireCraneFactor = 0;//!<Пожарный кран
	double girovertFactor = 0;//!<Гировертикаль
	double pumpStationFactor = 0;//!<Насосная станция
	double rainFactor = 1;//!<Дождь
	double buzzerFactor = 0;//!<Зуммер
	double fenFactor = 0;//!<Фен
	double undefinedFactor = 0;//!<Неизвестный
	double circlingCraneFactor = 0;//!<Кран кольцевания
	double rocketIglaFactor = 0;//!<Игла
	double vpryam = 0;//!<Выпрямительное устройство
	double vadd = 0;//!<Скоростная добавка
	double shock = 1;//!<Удар о стык плит
	double trim = 0;//!<Триммер
	double frict = 0;//!>Фрикцион

	// Переменные для хранения длительности файлов в секундах
	double redLengthOn = 0;//!<длительность запуска агрегата
	double redLengthOff = 0;//!<длительность остановки агрегата
	double redLengthMg1 = 0;
	double redLengthMg2 = 0;
	double redLengthAvt = 0;
	//
	double engLengthOn = 0;
	double engLengthOff = 0;
	double engLengthHpOn = 0;
	double engLengthMg = 0;
	double engLengthWAavt = 0;
	double engLengthHpW = 0;
	double engLengthHpOff = 0;
	//
	double vsuLengthOn = 0;
	double vsuLengthHpOn = 0;
	double vsuLengthOff = 0;
	double vsuLengthHpOff = 0;
	//Вектора переходных функций (время - обороты) запуска и остановки агрегатов
	//redFunctionOnSwap - вектор, где точкам p{x,y} вектора redFunctionOn, соответствуют точки p{y,x}
	vector<point> redFunctionOn;
	vector<point> redFunctionOff;
	vector<point> redFunctionOnSwap;
	vector<point> redFunctionOffSwap;
	//
	vector<point> engFunctionOn;
	vector<point> engFunctionOff;
	vector<point> engFunctionOnSwap;
	vector<point> engFunctionOffSwap;

	//!<Ассоциативный массив с именами файлов - звуков
	map<string, string> shortName;

	//!<Ассоциативный массив с полными именами файлов - звуков
	map<string, string> fullName;

	//!<Ассоциативный массив с полными именами файлов - звуков
	map<string, ALuint> bufferMap;

	ALuint *buffers;

	/*!
	\brief Заполняет массив с полными именами файлов
	\details Заполняет массив fullName с полными именами файлов: shortName + pathToFile
	\param[in] pathToFile Путь к аудиофайлам
	*/
	void setPath(string pathToFile);

	/*!
	\brief Заполняет параметры объекта ЛА
	\details Заполняет параметры объекта ЛА в зависимости от переданного параметра model
	\param[in] model Модель летательного аппарата
	*/
	void setParam(string model);

	void setBufferMap();

	Helicopter();
	~Helicopter();
};

#endif