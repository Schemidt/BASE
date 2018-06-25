/*!
\file
\brief Класс Helicopter / определение

Данный файл определяет работу методов класса Helicopter
*/

#include "helicopter.h"

using namespace std;

void Helicopter::setPath(string pathToFile)
{
	//TXT
	//Двигатель
	shortName["eng_on"] = "eng_on.txt"; //!<имя файла с переходной функцией разгона двигателя до режима МГ
	shortName["eng_off"] = "eng_off.txt"; //!<имя файла с переходной функцией остановки двигателя 
	shortName["eng_mg_avt"] = "eng_mg_avt.txt"; //!<имя файла с переходной функцией разгона двигателя до режима автомат
	shortName["eng_avt_mg"] = "eng_avt_mg.txt"; //!<имя файла c переходной функцией остановки до режима малого газа
												//Редуктор
	shortName["red_on"] = "red_on.txt"; //!<имя файла c переходной функцией разгона редуктора до режима малого газа на 1ом двигателе
	shortName["red_on_wfe"] = "red_on_wfe.txt"; //!<имя файла c переходной функцией разгона редуктора до режима малого газа на 1ом двигателе
	shortName["red_on_mg"] = "red_on_mg.txt"; //!<имя файла c переходной функцией разгона редуктора до режима малого газа на 2х двигателях
	shortName["red_mg_avt"] = "red_mg_avt.txt"; //!<имя файла c переходной функцией разгона редуктора до режима автомат
	shortName["red_avt_mg"] = "red_avt_mg.txt"; //!<имя файла c переходной функцией замедления редуктора до режима мг
	shortName["red_off"] = "red_off.txt"; //!<имя файла c переходной функцией остановки редуктора ниже режима мг
										  //WAV
										  //Двигатель
	shortName["eng_on_w"] = "eng_on.wav"; //!<имя файла звука разгона двигателя до режима МГ
	shortName["eng_w_w"] = "eng_w.wav"; //!<имя файла звука работы двигателя на режиме МГ
	shortName["eng_off_w"] = "eng_off.wav"; //!<имя файла звука остановки двигателя 
	shortName["eng_on_hp_w"] = "eng_hp_on.wav"; //!<имя файла звука разгона двигателя до режима ХП
	shortName["eng_w_hp_w"] = "eng_hp_w.wav"; //!<имя файла звука работы двигателя на режиме ХП
	shortName["eng_off_hp_w"] = "eng_hp_off.wav"; //!<имя файла звука остановки двигателя из ХП
	shortName["eng_w_avt_w"] = "eng_w_avt.wav"; //!<имя файла звука работы двигателя на режиме автомат
	shortName["eng_on_avt_w"] = "eng_on_avt.wav"; //!<имя файла звука разгона двигателя до режима автомат
	shortName["eng_avt_mg_w"] = "eng_avt_mg.wav"; //!<имя файла звука остановки до режима малого газа
	shortName["eng_pomp_on"] = "pomp_on.wav"; //!<имя файла звука помпажа
	shortName["eng_pomp_w"] = "pomp_w.wav"; //!<имя файла звука помпажа
											//Редуктор
	shortName["red_on_w"] = "red_on.wav"; //!<имя файла звука разгона редуктора до режима малого газа на 1ом двигателе
	shortName["red_w_w"] = "red_w.wav"; //!<имя файла звука работы редуктора на режиме малого газа на 1ом двигателе
	shortName["red_on_mg_w"] = "red_on_mg.wav"; //!<имя файла звука разгона редуктора до режима малого газа на 2х двигателях
	shortName["red_w_mg_w"] = "red_w_mg.wav"; //!<имя файла звука работы редуктора на режиме малого газа на 2х двигателях
	shortName["red_mg_avt_w"] = "red_mg_avt.wav"; //!<имя файла звука разгона редуктора до режима автомат
	shortName["red_w_avt_w"] = "red_w_avt.wav"; //!<имя файла звука работы редуктора на режиме автомат
	shortName["red_avt_mg_w"] = "red_avt_mg.wav"; //!<имя файла звука замедления редуктора до режима мг
	shortName["red_off_w"] = "red_off.wav"; //!<имя файла звука остановки редуктора ниже режима мг
	shortName["red_crash"] = "red_crash.wav"; //!<имя файла звука поломки редуктора
	shortName["red_w_avt_fly"] = "red_w_avt_fly.wav"; //!<имя файла звука работы редуктора на режиме автомат
	shortName["red_harm"] = "red_harm.wav"; //!<имя файла гармоники редуктора
	//ВСУ
	shortName["vsu_on"] = "vsu_on.wav";//!<имя файла звука запуска ВСУ
	shortName["vsu_w"] = "vsu_w.wav";//!<имя файла звука работы ВСУ
	shortName["vsu_off"] = "vsu_off.wav";//!<имя файла звука выключения ВСУ
	shortName["vsu_hp_on"] = "vsu_hp_on.wav";//!<имя файла звука запуска ВСУ ХП
	shortName["vsu_hp_w"] = "vsu_hp_w.wav";//!<имя файла звука работы ВСУ ХП
	shortName["vsu_hp_off"] = "vsu_hp_off.wav";//!<имя файла звука выключения ВСУ ХП
	shortName["vsu_kran_on"] = "vsu_kran_on.wav";//!<имя файла звука включения перекрывного крана ВСУ
	shortName["vsu_kran_w"] = "vsu_kran_w.wav";//!<имя файла звука работы перекрывного крана ВСУ
	//Аккумулятор
	shortName["accum"] = "accu.wav";//!<имя файла звука Аккумулятора
	shortName["accum_tr"] = "accu_tr.wav";//!<имя файла звука Аккумулятора
	shortName["vpryam"] = "vy_tone.wav";//!<имя файла звука Аккумулятора
	//ПТС
	shortName["pts_on"] = "pts_on.wav";//!<имя файла звука включения ПТС
	shortName["pts_w"] = "pts_w.wav";//!<имя файла звука работы ПТС
	shortName["pts_off"] = "pts_off.wav";//!<имя файла звука выключения ПТС
	shortName["pts_tone"] = "pts_tone.wav";//!<имя файла звука ПТС ТОН
	//Трансформатор 36В
	shortName["tr36_on"] = "tr36_on.wav";//!<имя файла звука Трансформатор 36В
	shortName["tr36_w"] = "tr36_w.wav";//!<имя файла звука Трансформатор 36В
	shortName["tr36_off"] = "tr36_off.wav";//!<имя файла звука Трансформатор 36В
	//Трансформатор 115В
	shortName["tr115_on"] = "tr115_on.wav";//!<имя файла звука Трансформатор 115В
	shortName["tr115_w"] = "tr115_w.wav";//!<имя файла звука Трансформатор 115В
	shortName["tr115_off"] = "tr115_off.wav";//!<имя файла звука Трансформатор 115В
	//Насос подкачки
	shortName["podk_r_on"] = "pump_r_on.wav";//!<имя файла звука насоса подкачки
	shortName["podk_r_w"] = "pump_r_w.wav";//!<имя файла звука насоса подкачки
	shortName["podk_r_off"] = "pump_r_off.wav";//!<имя файла звука насоса подкачки
	shortName["podk_l_on"] = "pump_l_on.wav";//!<имя файла звука насоса подкачки
	shortName["podk_l_w"] = "pump_l_w.wav";//!<имя файла звука насоса подкачки
	shortName["podk_l_off"] = "pump_l_off.wav";//!<имя файла звука насоса подкачки

	shortName["cons_tank_on"] = "consum_tank_on.wav";//!<имя файла звука насоса расходного бака
	shortName["cons_tank_w"] = "consum_tank_w.wav";//!<имя файла звука насоса расходного бака
												   //Перекрывной кран
	shortName["perekr_l"] = "kran_l.wav";//!<имя файла звука перекрывного крана
	shortName["perekr_r"] = "kran_r.wav";//!<имя файла звука перекрывного крана
										 //КО-50
	shortName["ko50_on"] = "ko50_on.wav";//!<имя файла звука KO-50
	shortName["ko50_w"] = "ko50_w.wav";//!<имя файла звука KO-50
	shortName["ko50_off"] = "ko50_off.wav";//!<имя файла звука KO-50
										   //СКВ
	shortName["skv_on"] = "skv_on.wav";//!<имя файла звука СКВ
	shortName["skv_w"] = "skv_w.wav";//!<имя файла звука СКВ
	shortName["skv_off"] = "skv_off.wav";//!<имя файла звука СКВ
	shortName["skv_harm_29"] = "skv_harm.wav";//!<имя файла гармоники СКВ для Ка-29
										 //Движение по ВПП и РД
	shortName["runway"] = "driving.wav";//!<имя файла звука звука движения по ВПП и РД

								 //Вооружение
	shortName["sturm"] = "sturm.wav";//!<имя файла звука ракеты штурм
	shortName["nar8"] = "s8.wav";//!<имя файла звука НАР 8
	shortName["nar13"] = "s13.wav";//!<имя файла звука НАР 13
	shortName["rocket"] = "rocket.wav";//!<имя файла звука попадания ракеты
	shortName["upk"] = "gun_loop.wav";//!<имя файла звука выстрелов УПК
	shortName["ppu"] = "gun_loop.wav";//!<имя файла звука выстрелов ППУ
	shortName["shoot"] = "shoot.wav";//!<имя файла звука выстрелов ППУ
	shortName["igla"] = "sturm.wav";//!<имя файла звука ракеты штурм
									//Винт
	shortName["vint_hi"] = "vint_hi.wav";//!<имя файла звука свиста винта
	shortName["vint_hi_avt"] = "vint_hi_avt.wav";//!<имя файла звука свиста винта
	shortName["vint_torm"] = "tormoz.wav";//!<имя файла звука тормоза винта
	shortName["vint_flap"] = "vint_flap.wav";//!<имя файла звука равномерных хлопков винта
	shortName["vint_flap_hi"] = "vint_flap_hi.wav";//!<имя файла звука винта на низких оборотах
	shortName["vint_flap_low"] = "vint_flap_lo.wav";//!<
	shortName["vint_flap_A"] = "flap_A.wav";//!<имя файла звука равномерных хлопков винта
	shortName["vint_flap_B"] = "flap_B.wav";//!<имя файла звука неравномерных хлопков винта
	shortName["vint_flap_C"] = "flap_C.wav";//!<имя файла звука неравномерных хлопков винта
	shortName["vint_swish_on"] = "vint_hi_on.wav";//!<имя файла звука свиста винта
	shortName["vint_swish_w"] = "vint_hi_1eng.wav";//!<имя файла звука свиста винта
	shortName["vint_swish_w_mg"] = "vint_hi_mg.wav";//!<имя файла звука свиста винта
	shortName["vint_swish_w_avt"] = "vint_hi_avt.wav";//!<имя файла звука свиста винта
	shortName["beats"] = "beats.wav";//!<имя файла звука биения
	shortName["flapping"] = "flapping.wav";//!<имя файла звука ???
											//Тормоз шасси
	shortName["brake"] = "brake_on.wav";//!<имя файла звука тормоза шасси
	shortName["poff"] = "check_brake.wav";//!<имя файла звука сброса давления
										  //Разрушение
	shortName["crash"] = "crash.wav";//!<имя файла звука столкновения с препятствием
									 //НИП
	shortName["nip_tone"] = "nip_tone.wav";//
										   //Пожарный кран
	shortName["kran_fire_l"] = "kran_fire_l.wav";
	shortName["kran_fire_r"] = "kran_fire_r.wav";
	//Гировертикаль
	shortName["girovert_on"] = "girovert_on.wav";//
	shortName["girovert_w"] = "girovert_w.wav";
	//shortName["girovert"] = "girovert_on.txt";
	//Насосная станция
	shortName["pstat_on"] = "pstat_on.wav";//
	shortName["pstat_w"] = "pstat_w.wav";//
	shortName["pstat_off"] = "pstat_off.wav";//
											 //Кран кольцевания
	shortName["kran_circle"] = "kran_l.wav";//!<имя файла звука перекрывного крана
											//Осадки
	shortName["rain"] = "rain.wav";//
								   //Зуммер
	shortName["beep"] = "beep.wav";//
								   //Неопознанный звук
	shortName["undefined0_on"] = "undefined0_on.wav";//
	shortName["undefined0_w"] = "undefined0_w.wav";//
	shortName["undefined1_on"] = "undefined1_on.wav";//
	shortName["undefined1_w"] = "undefined1_w.wav";//
	shortName["undefined1_off"] = "undefined1_off.wav";
	//Другое
	//Шум
	shortName["pinkNoise"] = "vadd.wav";//
	shortName["landing"] = "landing.wav";//
	shortName["testNoise"] = "testNoise.wav";
	//бумбум
	shortName["BumBum"] = "LF_.wav";
	//Отрыв
	shortName["takeOff"] = "take_off.wav";
	//Удар при проходе стыка плит
	shortName["shock"] = "shock.wav";
	//Триммер
	shortName["trim_on"] = "trim_on.wav";
	shortName["trim_off"] = "trim_off.wav";
	//Фрикцион
	shortName["frict"] = "brake_on.wav";

	fullName = shortName;

	map<string, string> ::iterator num;
	for (num = fullName.begin(); num != fullName.end(); num++)
	{
		(*num).second = pathToFile + (*num).second;
	}
}

void Helicopter::setParam(string model)
{
	modelName = model;

	setPath(modelName + "/");

	setBufferMap();

	if (model == "mi_8_mtv5")
	{
		//Константы		
		engTurnoverMg = 75;
		engTurnoverAvt = 88;
		redTurnoverMg1 = 50;
		redTurnoverMg2 = 63;
		redTurnoverAvt = 96;
		//Множители громкости		
		vsuFactor = 1/*всу*/;
		vsuHpFactor = 0.06/*всу хп*/;
		engFactor = 1/*дв*/;
		engHpFactor = 0.35/*дв хп*/;
		redFactor = 1/*Редуктор*/;
		accumFactor = 0.1/*аккум*/;
		ptsFactor = 0.18/*птс*/;
		ptsToneFactor = 0.25/*птс тон*/;
		tr36Factor = 0.15/*тр36*/;
		tr115Factor = 0.15/*тр115*/;
		pumpLeftFactor = 0.08/*подк л*/;
		pumpRightFactor = 0.12/*подк р*/;
		cutoffCraneFactor = 0.18/*перекр*/;
		ko50Factor = 0.5/*КО-50*/;
		runwayFactor = 1/*ВПП*/;
		rocketNar8Factor = 1/*НАР8*/;
		rocketNar13Factor = 1/*НАР13*/;
		rocketHitFactor = 0.8/*Попадание ракеты*/;
		upkFactor = 1/*УПК*/;
		vintSwishFactor = 1/*ВИНТ Верх*/;
		vintBrakeFactor = 0.2/*Винт тормоз*/;
		vintFlapFactor = 0.794/*Винт хлопки*/;
		chassisBrakePumpFactor = 0.03/*Тормоз шасси*/;
		chassisBrakeReleaseFactor = 0.03/*Сброс давления*/;
		consumTankFactor = 0.25/*Расходный бак*/;
		rainFactor = 0.5/*Дождь*/;
		vadd = 1/*Скоростная добавка*/;
	}
	else if (model == "mi_8_amtsh")
	{
		//Константы		
		engTurnoverMg = 75;
		engTurnoverAvt = 88;
		redTurnoverMg1 = 50;
		redTurnoverMg2 = 63;
		redTurnoverAvt = 96;
		//Множители громкости		
		vsuFactor = 1/*всу*/;
		vsuHpFactor = 0.06/*всу хп*/;
		engFactor = 1/*дв*/;
		engHpFactor = 0.35/*дв хп*/;
		redFactor = 1/*Редуктор*/;
		accumFactor = 0.1/*аккум*/;
		ptsFactor = 0.18/*птс*/;
		ptsToneFactor = 0.25/*птс тон*/;
		tr36Factor = 0.15/*тр36*/;
		tr115Factor = 0.15/*тр115*/;
		pumpLeftFactor = 0.08/*подк л*/;
		pumpRightFactor = 0.12/*подк р*/;
		cutoffCraneFactor = 0.18/*перекр*/;
		ko50Factor = 0.5/*КО-50*/;
		runwayFactor = 0.75/*ВПП*/;
		rocketNar8Factor = 1/*НАР8*/;
		rocketNar13Factor = 1/*НАР13*/;
		rocketHitFactor = 0.8/*Попадание ракеты*/;
		upkFactor = 1/*УПК*/;
		vintSwishFactor = 1/*ВИНТ Верх*/;
		vintBrakeFactor = 0.2/*Винт тормоз*/;
		vintFlapFactor = 0.794/*Винт хлопки*/;
		chassisBrakePumpFactor = 0.03/*Тормоз шасси*/;
		chassisBrakeReleaseFactor = 0.03/*Сброс давления*/;
		consumTankFactor = 0.25/*Расходный бак*/;
		rainFactor = 0.5/*Дождь*/;
		vadd = 1/*Скоростная добавка*/;
	}
	else if (model == "mi_26")
	{
		//Константы		
		engTurnoverMg = 69;
		engTurnoverAvt = 84;
		redTurnoverMg1 = 37;
		redTurnoverMg2 = 47;
		redTurnoverAvt = 85;
		//Множители громкости
		vsuFactor = 0.35/*всу*/;
		vsuHpFactor = 0.04/*всу хп*/;
		engFactor = 0.446/*дв*/;//-7
		engHpFactor = 0.11/*дв хп*/;
		redFactor = 0.35/*Редуктор*/;
		tr115Factor = 0.08/*тр115*/;
		skvFactor = 0.21/*СКВ*/;
		runwayFactor = 0.35/*ВПП*/;
		//rocketHitFactor = 0.8/*Попадание ракеты*/;
		vintSwishFactor = 0.55/*ВИНТ Верх*/;
		vintBrakeFactor = 0.12/*Винт тормоз*/;
		vintFlapFactor = 0.4/*Винт хлопки*/;
		chassisBrakePumpFactor = 0.03/*Тормоз шасси*/;
		chassisBrakeReleaseFactor = 0.03/*Сброс давления*/;
		nipFactor = 0.0875/*НИП*/;
		fireCraneFactor = 0.0875/*Пожарный кран*/;
		girovertFactor = 0.105/*Гировертикаль*/;
		pumpStationFactor = 0.05/*Насосная станция*/;
		consumTankFactor = 0.17/*Расходный бак*/;
		rainFactor = 0.5/*Дождь*/;
		//vadd = 0.5/*Скоростная добавка*/;
	}
	else if (model == "mi_28")
	{
		//Константы		
		engTurnoverMg = 73;
		engTurnoverAvt = 86;
		redTurnoverMg1 = 48;
		redTurnoverMg2 = 60;
		redTurnoverAvt = 93;
		//Множители громкости
		vsuFactor = 0.08/*всу*/;
		vsuHpFactor = 0.6/*всу хп*/;
		vsuCraneFactor = 0.8/*всу кран*/;
		engFactor = 0.446/*дв*/;
		engHpFactor = 0.35/*дв хп*/;
		redFactor = 0.446/*Редуктор*/;
		accumFactor = 0.1/*аккум*/;
		ptsToneFactor = 0.25/*птс тон*/;
		tr115Factor = 0.15/*тр115*/;
		skvFactor = 0.708/*СКВ*/;
		runwayFactor = 1/*ВПП*/;
		rocketSturmFactor = 1/*ШТУРМ*/;
		rocketNar8Factor = 1/*НАР8*/;
		rocketNar13Factor = 1/*НАР13*/;
		rocketHitFactor = 0.8/*Попадание ракеты*/;
		ppuFactor = 1/*ППУ*/;
		vintBrakeFactor = 0.6/*Винт тормоз*/;
		vintFlapFactor = 0.5/*Винт хлопки*/;
		vintSwishFactor = 0.63;
		chassisBrakePumpFactor = 0.03/*Тормоз шасси*/;
		chassisBrakeReleaseFactor = 0.03/*Сброс давления*/;
		fireCraneFactor = 0.9/*Пожарный кран*/;
		pumpStationFactor = 0.0725/*Насосная станция*/;
		rainFactor = 0.5/*Дождь*/;
		buzzerFactor = 0.9 /*beep*/;
		fenFactor = 0.1 /*undefined0*/;
		undefinedFactor = 0.2 /*undefined1*/;
		rocketIglaFactor = 1 /*ИГЛА*/;
		vadd = 1/*Скоростная добавка*/;
	}
	else if (model == "ka_226")
	{
		//Константы		
		engTurnoverMg = 61;
		engTurnoverAvt = 79;
		redTurnoverMg1 = 49;
		redTurnoverMg2 = 62;
		redTurnoverAvt = 99;
		//Множители громкости
		engFactor = 0.398/*дв*/;
		engHpFactor = 0.141/*дв хп*/;
		redFactor = 0.398/*Редуктор*/;
		accumFactor = 0.100/*аккум*/;
		pumpLeftFactor = 0.100/*подк л*/;
		pumpRightFactor = 0.100/*подк р*/;
		cutoffCraneFactor = 0.126/*перекр*/;
		runwayFactor = 1/*ВПП*/;
		vintBrakeFactor = 1/*Винт тормоз*/;
		vintFlapFactor = 1/*Винт хлопки*/;
		vintSwishFactor = 1/*Винт свист*/;
		chassisBrakePumpFactor = 0.178/*Тормоз шасси*/;
		chassisBrakeReleaseFactor = 0.178/*Сброс давления*/;
		rainFactor = 1/*Дождь*/;
		circlingCraneFactor = 0.126 /*Кран кольцевания*/;
		vadd = 1/*Скоростная добавка*/;
	}
	else if (model == "ansat")
	{
		//Константы		
		engTurnoverMg = 65;
		engTurnoverAvt = 73;
		redTurnoverMg1 = 65;
		redTurnoverMg2 = 65;
		redTurnoverAvt = 100;
		//Множители громкости
		engFactor = 0.63/*дв*/;//-4
		engHpFactor = 0.141/*дв хп*/;
		redFactor = 0.63/*Редуктор*/;//-4
		accumFactor = 0.089/*аккум*/;//-21
		vintBrakeFactor = 0.015/*Винт тормоз*/;
		vintFlapFactor = 1/*Винт хлопки*/;
		vintSwishFactor = 1;//
		chassisBrakePumpFactor = 0.178/*Тормоз шасси*/;
		chassisBrakeReleaseFactor = 0.178/*Сброс давления*/;
		rainFactor = 1/*Дождь*/;
		vadd = 1/*Скоростная добавка*/;
		runwayFactor = 1/*ВПП*/;
		undefinedFactor = 1; /*хз1*/
	}
	else if (model == "ka_27")
	{
		//Константы		
		engTurnoverMg = 75;
		engTurnoverAvt = 85;
		redTurnoverMg1 = 44;
		redTurnoverMg2 = 60;
		redTurnoverAvt = 90;
		//Множители громкости		
		vsuFactor = 0.05/*всу*/;//-26
		vsuHpFactor = 0.25/*всу хп*/;//-12
		vsuCraneFactor = 0.354/*всу кран*/;//-9
		engFactor = 1/*дв*/; //0
		engHpFactor = 0.334/*дв хп*/;//-9.5
		redFactor = 0.794/*Редуктор*/; //-2
		vpryam = 0.199/*ВУ*/;//-14
		cutoffCraneFactor = 0.354/*перекр*/;//-9
		vintBrakeFactor = 0.2/*Винт тормоз*/;
		vintFlapFactor = 0.5/*Винт хлопки*/;
		chassisBrakePumpFactor = 0.1/*Тормоз шасси*/;
		chassisBrakeReleaseFactor = 0.1/*Сброс давления*/;
		undefinedFactor = 0.25; /*хз1*///-12
		runwayFactor = 0.63/*ВПП*/;
		vadd = 1/*Скоростная добавка*/;
		skvFactor = 1;

		rainFactor = 0.5/*Дождь*/;

		//ptsToneFactor = 0.25/*птс тон*/;
		//vintSwishFactor = 0.5/*ВИНТ Верх*/;
		fireCraneFactor = 0.354/*Пожарный кран*/;
	}
	else if (model == "ka_29")
	{
		//Константы		
		engTurnoverMg = 75;
		engTurnoverAvt = 85;
		redTurnoverMg1 = 44;
		redTurnoverMg2 = 60;
		redTurnoverAvt = 90;
		//Множители громкости		
		vsuFactor = 0.237/*всу*/;//-12.5
		vsuHpFactor = 0.25/*всу хп*/;//-12
		vsuCraneFactor = 0.354/*всу кран*/;//-9
		engFactor = 1/*дв*/; //0
		engHpFactor = 0.334/*дв хп*/;//-9.5
		redFactor = 0.794/*Редуктор*/; //-2
		vpryam = 0.199/*ВУ*/;//-14
		cutoffCraneFactor = 0.354/*перекр*/;//-9
		vintBrakeFactor = 0.5/*Винт тормоз*/;
		vintFlapFactor = 0.5/*Винт хлопки*/;
		chassisBrakePumpFactor = 0.1/*Тормоз шасси*/;
		chassisBrakeReleaseFactor = 0.1/*Сброс давления*/;
		undefinedFactor = 0.25; /*хз1*///-12
		runwayFactor = 0.63/*ВПП*/;
		vadd = 1/*Скоростная добавка*/;
		skvFactor = 1;
		trim = 0.28;
		//frict = 1;

		rocketSturmFactor = 1/*ШТУРМ*/;
		rocketNar8Factor = 1/*НАР8*/;
		rocketNar13Factor = 1/*НАР13*/;
		rocketHitFactor = 0.8/*Попадание ракеты*/;
		upkFactor = 1/*УПК*/;
		ppuFactor = 1/*ППУ*/;

		rainFactor = 0.5/*Дождь*/;

		fireCraneFactor = 0.354/*Пожарный кран*/;
	}
	else
	{
		cout << " Unknown argument" << endl;
		throw 0;
	}

	//
	redLengthOn = getLengthWAV(fullName["red_on_w"]);
	redLengthOff = getLengthWAV(fullName["red_off_w"]);
	redLengthMg1 = getLengthWAV(fullName["red_w_w"]);
	redLengthMg2 = getLengthWAV(fullName["red_w_mg_w"]);
	redLengthAvt = getLengthWAV(fullName["red_w_avt_w"]);
	//
	engLengthOn = getLengthWAV(fullName["eng_on_w"]);
	engLengthOff = getLengthWAV(fullName["eng_off_w"]);
	engLengthHpOn = getLengthWAV(fullName["eng_on_hp_w"]);
	engLengthMg = getLengthWAV(fullName["eng_w_w"]);
	engLengthWAavt = getLengthWAV(fullName["eng_w_avt_w"]);
	engLengthHpW = getLengthWAV(fullName["eng_w_hp_w"]);
	engLengthHpOff = getLengthWAV(fullName["eng_off_hp_w"]);
	//
	vsuLengthOn = getLengthWAV(fullName["vsu_on"]);
	vsuLengthHpOn = getLengthWAV(fullName["vsu_hp_on"]);
	vsuLengthOff = getLengthWAV(fullName["vsu_off"]);
	vsuLengthHpOff = getLengthWAV(fullName["vsu_hp_off"]);
	//
	redFunctionOn = getVectorFromFile(fullName["red_on"]);
	redFunctionOnSwap = redFunctionOn;
	redFunctionOff = getVectorFromFile(fullName["red_off"]);
	redFunctionOffSwap = redFunctionOff;
	//
	engFunctionOn = getVectorFromFile(fullName["eng_on"]);
	engFunctionOnSwap = engFunctionOn;
	engFunctionOff = getVectorFromFile(fullName["eng_off"]);
	engFunctionOffSwap = engFunctionOff;

	for (auto &swp : redFunctionOnSwap)
	{
		swp.swap();
	}
	for (auto &swp : redFunctionOffSwap)
	{
		swp.swap();
	}
	for (auto &swp : engFunctionOnSwap)
	{
		swp.swap();
	}
	for (auto &swp : engFunctionOffSwap)
	{
		swp.swap();
	}
}

void Helicopter::setBufferMap()
{
	buffers = new ALuint[fullName.size() * 2];
	vector<double> channel = { 1,1,0,0,0,0,0 };

	int i = 0;
	for (auto it : fullName)
	{
		if (it.first == "eng_on_w" ||
			it.first == "eng_w_w" ||
			it.first == "eng_off_w" ||
			it.first == "eng_on_hp_w" ||
			it.first == "eng_w_hp_w" ||
			it.first == "eng_off_hp_w" ||
			it.first == "eng_w_avt_w" ||
			it.first == "eng_on_avt_w" ||
			it.first == "eng_avt_mg_w")
		{
			alGenBuffers(1, &buffers[i]);
			channel = { 1,-1,0,0,0,0,0 };
			setBuffer(buffers[i], it.second, channel);
			bufferMap[it.second + "l"] = buffers[i];
			i++;

			alGenBuffers(1, &buffers[i]);
			channel = { 0,2,0,0,0,0,0 };
			setBuffer(buffers[i], it.second, channel);
			bufferMap[it.second + "r"] = buffers[i];
			i++;
		}
		else if (it.first == "vsu_on" ||
			it.first == "vsu_w" ||
			it.first == "vsu_off" ||
			it.first == "vsu_hp_on" ||
			it.first == "vsu_hp_w" ||
			it.first == "vsu_hp_off")
		{
			alGenBuffers(1, &buffers[i]);
			if (modelName == "mi_26")
			{
				channel = { 0,0,1,1,0,0,0 };
			}
			else
			{
				channel = { 1,1,0,0,0,0,0 };
			}
			setBuffer(buffers[i], it.second, channel);
			bufferMap[it.second] = buffers[i];
			i++;
		}
		else if (it.first == "red_on_w" ||
			it.first == "red_w_w" ||
			it.first == "red_on_mg_w" ||
			it.first == "red_w_mg_w" ||
			it.first == "red_mg_avt_w" ||
			it.first == "red_w_avt_w" ||
			it.first == "red_avt_mg_w" ||
			it.first == "red_off_w")
		{
			alGenBuffers(1, &buffers[i]);
			channel = { 1,1,0,0,0,0,0 };
			setBuffer(buffers[i], it.second, channel);
			bufferMap[it.second] = buffers[i];
			i++;
		}
	}
	bufferMap["NULL"] = -1;//служебное значение
}

Helicopter::Helicopter()
{

}

Helicopter::~Helicopter()
{

}

