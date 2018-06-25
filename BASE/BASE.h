/*!
\file
\brief Класс Sound

Данный файл объявляет класс Sound и его наследников
*/

#define _USE_MATH_DEFINES

#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include "fstream"
#include "string"
#include "conio.h"
#include "regex"
#include "memory"
#include "vector"
#include "math.h"

//обмен
#include "Shared.h"
#include "realtime.h"

//звук
#include "EFXptr.h"
#include "Sound.h"
#include "Helicopter.h"

using namespace std;

#ifndef Sound_h
#define Sound_h
/*!
\brief Класс состояния агрегатов вертолета
\author Самсонов А.В.
\version 6.0
\date  02.03.2018

Класс основных переменных и функций, определяющих работу программы
*/
class Sound {
public:

	static int sourcesInUse;//!< Переменная для хранения количества использующихся источников (не более 256)
	static int effectSlotsInUse;//!< Переменная для хранения количества использующихся слотов для эффектов (не более 16)
	static int maxSources;//!< Переменная хранящая количество максимально возможных источников на устройстве, 256 максимально
	static int maxSlots;//!< Переменная хранящая количество максимально возможных слотов для эффектов на устройстве, 16 максимально
	static double masterGain;//!< Глобальный модификатор громкости
	static double currentTime;//!< Переменная хранящая текущее время полученное из общей памяти от USPO
	static double deltaTime;//!< Переменная для отслеживания изменения времени

	static double step; //!< Шаг
	static double tangaz;//!< Тангаж
	static double derivTangaz;//!<скорость изменения Тангажа (dtangaz/dt)
	static double hight;//!< Высота
	static double velocityVectorXZ;//!< суммарный вектор скорости в плоскости vx-vz
	static double accelerationVectorXZ;//!< Ускорение по вектору скорости (dv/dt)
	static double velocityY;//!< вертикальная скорость
	static double dashVectorXZ;//!< Рывок (d2v/dt)
	static double accelerationVy;//!< Вертикальное ускорение (dvy/dt)
	static double derivStep;//!< Скорость изменения шага
	static double calcA;//!< Атака винта
	static double redTurnAcc;//!< Ускорение оборотов редуктора (dturn/dt)
	static double groundTouch;//!< Признак касания земли

	static vector<double> vectorVy;//!<Массив для рассчета производной от вертикальной скорости
	static vector<double> vectorVXZ;//!<Массив для рассчета производной от горизонтальной скорости
	static vector<double> vectorAccXZ;//!<Массив для рассчета производной от горизонтального ускорения
	static vector<double> vectorStep;//!<Массив для рассчета производной от шага
	static vector<double> vectorTangaz;//!<Массив для рассчета производной от тангажа
	static vector<double> vectorTime;//!<Массив для рассчета производной
	static vector<double> vectorRedTurn;//!<Массив для рассчета производной от оборотов редуктора

	static vector<double> vectorAvrEng1Turn;//!<Массив для рассчета среднего оборотов первого двигателя
	static vector<double> vectorAvrEng2Turn;//!<Массив для рассчета среднего оборотов второго двигателя
	static vector<double> vectorAvrRedTurn;//!<Массив для рассчета среднего оборотов редуктора
	static vector<double> vectorAvrStep;//!<Массив для рассчета среднего шага
	static vector<double> vectorAvrAtk;//!<Массив для рассчета средней атаки

	static double globalWindow;//!<Переменная времени для набора значений в массивы для рассчета среднего

	unique_ptr<int[]> sourceStatus;//!< Переменная для статуса источника
	unique_ptr<double[]> pitch;//!< Переменная для параметра высоты тона звука агрегата
	unique_ptr<double[]> gain;//!< Переменная для параметра громкости звука агрегата
	unique_ptr<float[]> offset;//!< 
	unique_ptr<ALuint[]> source;//!< Переменная для источника
	unique_ptr<ALuint[]> buffer;//!< Переменная для буффера
	unique_ptr<ALuint[]> effectSlot;//!< Переменная для слота эффекта
	unique_ptr<ALuint[]> effect;//!< переменная для эффекта
	unique_ptr<ALuint[]> filter;//!< переменная для эффекта
	unique_ptr<string[]> filetoBuffer;//!< 
	unique_ptr<string[]> fileBuffered;//!<
	vector<string> ModeSequence = { "0","0","0" };//Вектор истории режимов 
	string mode = "0";//Переменная текущего режима
	double switcher = 0;//Переменная - таймер, для кроссфейда по времени
	double crossFadeDuration = 1;//Кроссфейд по времени за crossFadeDuration секунд
	int id = 0;//Текущий используемый источник

	bool soundOn = 0;//!< Переменная для определения состояния звука
	bool soundWork = 0;//!< Переменная для определения состояния звука
	bool soundOff = 0;//!< Переменная для определения состояния звука
	float offsetOn = 0;
	float offsetOff = 0;
	double lengthOn = 0;//!< Переменная для хранения длительности файла в секундах, как правило для файла запуска агрегата
	double lengthOff = 0;//!< Переменная для хранения длительности файла в секундах, как правило для файла остановки агрегата

	vector<double> channel = { 1,1,0,0,0,0,0 };//!< массив для поканального вывода звука (по умолчанию вывод в левый и правый канал)
	int sourceNumber = 2;//!< Переменная для хранения количества источников используемых объектом звука агрегата
	int bufferNumber = 2;//!< Переменная для хранения количества буфферов используемых объектом звука агрегата
	int effectSlotNumber = 0;//!< Переменная для хранения количества слотов эффектов используемых объектом звука агрегата

	Sound();//!< Конструктор по умолчанию, для объекта с 1им источником
	Sound(const Sound &copy);//!< Конструктор копирования*/
	Sound& operator =(const Sound &copy);
	Sound(int sources, int buffers, int effectslots);//!< Конструктор для объекта с sources источниками, buffers буферами и effectslots слотами эффектов
	~Sound();//!< Деструктор (да неужели)

	/*!
	\brief Инициализирует воспроизведение звука
	\details <pre>Устанавливает шаблон воспроизведения звука при получении признака работы агрегата или физ.эффекта:
	звук устанавления режима работы агрегата (при признаке - "1")
		 |
		\\|/
	звук работы агрегата (при окончании звука установления работы и признаке - "1")
		 |
		\\|/
	звук завершения работы агрегата (при признаке - "0")


	\image html phases.png "Фазы звучания"
	</pre>
	\param[in] status Признак работы
	\param[in] path_on Указатель на строку с адресом звука устанавления режима работы
	\param[in] path_w Указатель на строку с адресом звука режима работы
	\param[in] path_off Указатель на строку с адресом звука завершения работы
	\param[in] gain_mult Коэффициент громкости для агрегата
	\return Статус источника OpenAL
	*/
	virtual int play(bool status, string pathOn, string pathW, string pathOff, double gainMult);

	/*!
	\brief Загружает буфер, подключает к источнику и запускает
	\details
	<pre>
	Выгружает буффер, загружает данными из file_path, подключает к источнику,
	запускает источник используя параметр отступа в секундах offset.
	Преобразовал часто используемый код в функцию
	</pre>
	\param[in] Buffer Объект буфера
	\param[in] Source Объект источника
	\param[in] offset оступ от начала файла, сек
	\param[in] file_path Полное имя файла
	\return Статус источника OpenAL
	*/
	int setSource(ALuint *Buffer, ALuint *Source, string file_path);

	int switchBuffer(ALuint *Buffer, ALuint *Source);

	int setAndDeploySound(ALuint *Buffer, ALuint *Source, double offset, string file_path);

	double getAverange(vector<double> &vectorOfParameters, double seconds);
};

/*!
\brief Класс объекта редуктора
\author Самсонов А.В.
\version 3.0
\date  06.03.2018

Класс определяющий работу объекта - редуктора
*/
class Reductor : public Sound
{
public:

	//bool setup = 0;

	bool hovering = 0;/*!< Переменная для статуса полета на висении */
	string eq[2];/*!< Переменная для однократной загрузки буфера */
	string pinkNoise;/*!< Переменная для однократной загрузки буфера */
	string beats;/*!< Переменная для однократной загрузки буфера */
	string takeOff;/*!< Переменная для однократной загрузки буфера */
	Smoother sm;
	double flapOn = 0;
	double reperTurn = 0;
	string reperSet;

	Reductor();

	~Reductor();
	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter &h, SOUNDREAD &sr);
};

/*!
\brief Класс объекта двигателя
\author Самсонов А.В.
\version 3.0
\date  06.03.2018

Класс определяющий работу объекта - двигателя
*/
class Engine : public Sound
{
public:

	static int engCount;/*!< Переменная для количества инициализированных двигателей в программе */
	double phase;//!<Фаза для двигателей, чтобы их звуки не сливались(0-1, смещаем на 0.33 для каждого нового объекта, т.е. запускаем с 33% * n процентов длительности)
	int engNum;//!<Номер двигателя
	char angle;
	string eq[2];/*!< Переменная для однократной загрузки буфера */
	double reperTurn = 0;
	string reperSet;

	Engine();

	~Engine();
	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] status_on Переменная статуса запуска
	\param[in] status_off Переменная статуса остановки
	\param[in] parameter Обороты двигателя
	\param[in] h Объект хранящий характеристики вертолета
	\return Статус источника OpenAL
	*/
	int play(bool status_on, bool status_off, bool status_hp, double parameter, Helicopter &h);
};

/*!
\brief Класс объекта двигателя
\author Самсонов А.В.
\version 3.0
\date  06.03.2018

Класс определяющий работу объекта - двигателя
*/
class Vsu : public Sound
{
public:

	double vsuDownTimer = 0;
	double vsuUpTimer = 0;
	double vsuTurnover = 0;

	string init = "NULL";

	string eq[2];/*!< Переменная для однократной загрузки буфера */

	Vsu();

	~Vsu();
	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] status_on Переменная статуса запуска
	\param[in] status_off Переменная статуса остановки
	\param[in] parameter Обороты двигателя
	\param[in] h Объект хранящий характеристики вертолета
	\return Статус источника OpenAL
	*/
	int play(SOUNDREAD &sr, Helicopter &h);
};

/*!
\brief Класс объекта - звук движения по ВПП
\author Самсонов А.В.
\version 3.0
\date  06.03.2018

Класс определяющий логику звука движения по ВПП и РД
*/
class Runway : public Sound
{
public:

	string eq;/*!< Переменная для однократной загрузки буфера */
	Smoother sm[2];

	Runway();

	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter &h, SOUNDREAD &sr);
};

/*!
\brief Класс объекта - хлопков винта
\author Самсонов А.В.
\version 3.0
\date  06.03.2018

Класс определяющий логику звука хлопков винта
*/
class VintFlap : public Sound
{
public:

	string key[3];/*!< Переменная для однократной загрузки буфера */
	double flapOn = 0;
	Smoother sm;
	VintFlap();

	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter &h, SOUNDREAD &sr);
};

/*!
\brief Класс объекта - свиста винта
\author Самсонов А.В.
\version 3.0
\date  06.03.2018

Класс определяющий логику звука свиста винта
*/
class VintSwish : public Sound
{
public:

	double reperTurn = 0;
	string reperSet;
	VintSwish();

	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter &h, SOUNDREAD &sr);
};

/*!
\brief Класс объекта - СКВ
\author Самсонов А.В.
\version 3.0
\date  06.03.2018

Класс определяющий логику звука объекта - СКВ (система кондиционирования воздуха)
*/
class Skv : public Sound
{
public:

	string eq[2];/*!< Переменная для однократной загрузки буфера */
	string harm;
	Skv();

	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter &h, SOUNDREAD &sr);

};

/*!
\brief Класс объекта - Кран
\author Самсонов А.В.
\version 1.0
\date  09.06.2018

Класс определяющий логику всех кранов
*/
class Crane : public Sound
{
public:

	/*!
	\brief Инициализирует воспроизведение звука
	\details <pre>Устанавливает шаблон воспроизведения звука при получении признака работы агрегата или физ.эффекта:
	звук устанавления режима работы агрегата (при признаке - "1")
	|
	\\|/
	звук работы агрегата (при окончании звука установления работы и признаке - "1")
	|
	\\|/
	звук завершения работы агрегата (при признаке - "0")


	\image html phases.png "Фазы звучания"
	</pre>
	\param[in] status Признак работы
	\param[in] path_on Указатель на строку с адресом звука устанавления режима работы
	\param[in] path_w Указатель на строку с адресом звука режима работы
	\param[in] path_off Указатель на строку с адресом звука завершения работы
	\param[in] gain_mult Коэффициент громкости для агрегата
	\return Статус источника OpenAL
	*/
	int play(char status, string path_on, string path_w, string path_off, double gain_mult);

};
#endif

/*!\brief Очищает объекты OpenAL*/
void freeOpenAL();

/*!
\brief Производит Crossfade
\details Осуществляет плавный переход 1ой записи в другую путем уменьшения громкости 1ой записи и повышения громкости другой, по мере изменения параметра
от значения fadeLimit к riseLimit.
<pre>
Если громкость звуков изменяется не в границах [0..1],
то стоит указать множитель громкости - т.е. предельная громкость нарастающей записи
\image html parametricalCrossfade.png "CrossFade"
</pre>
\param[in] fadeGain Громкость затухающего источника
\param[in] riseGain Громкость нарастающего источника
\param[in] parameter Значение параметра
\param[in] fadeLimit Значение параметра, при уровне громкости затухающей записи равном 1
\param[in] riseLimit Значение параметра, при уровне громкости Нарастающей записи равном 1
\param[in] gainMultiplicator Множитель громкости записей
\return 1 ,если громкость затухающей записи равняется 0, а нарастающей 1
*/
int parametricalCrossfade(double *fadeGain, double *riseGain, double parameter, double fadeLimit, double riseLimit);
/*!
\brief Производит Crossfade
\details Осуществляет плавный переход 1ой записи в другую путем уменьшения громкости 1ой записи и повышения громкости другой, по мере изменения параметра
от значения fadeLimit к riseLimit.
<pre>
Если громкость звуков изменяется не в границах [0..1],
то стоит указать множитель громкости - т.е. предельная громкость нарастающей записи
\image html parametricalCrossfade.png "CrossFade"
</pre>
\param[in] fadeGain Громкость затухающего источника
\param[in] riseGain Громкость нарастающего источника
\param[in] parameter Значение параметра
\param[in] fadeLimit Значение параметра, при уровне громкости затухающей записи равном 1
\param[in] riseLimit Значение параметра, при уровне громкости Нарастающей записи равном 1
\param[in] gainMultiplicator Множитель громкости записей
\return 1 ,если громкость затухающей записи равняется 0, а нарастающей 1
*/
int timeCrossfade(double &fadeGain, double &riseGain, double crossFadeDuration, double timer);
/*!\brief Определяет указатели на функции расширений EFX*/
void setEFXPointers();

/*!
\brief Возвращает максимальное количество доступных источников

\return максимальное количество доступных источников, 0 в случае ошибки
*/
int getMaxAvaliableSources();
/*!
\brief Вычисляет атаку
\details Вычисляет атаку (Угол атаки несущего винта — угол между направлением воздушного потока (противоположным направлению полета) и плоскостью вращения втулки несущего винта. Этот угол может иметь любое значение (от —180° до +180°) в зависимости от направления полета.
Если воздушный поток подходит к плоскости вращения винта снизу(автожирный режим), то угол атаки несущего винта считается положительным.Если воздушный поток набегает на несущий винт сверху, то угол атаки принимается отрицательным.)

Код вычисления атаки:
\code
tangaz = tangaz * M_PI / 180.0;
calcA = atan(tan(tangaz) - (2 * deltaHigh) / ((velocityXPrevious + velocityVectorXZ) * cos(tangaz) * periodCalc));
calcA = calcA * 180 / M_PI;
\endcode

\param[in] velocityVectorXZ Приборная скорость в прошлый момент времени t0
\param[in] velocityXPrevious Приборная скорость в момент времени t0 + periodCalc
\param[in] tangaz Тангаж в момент времени t0 + periodCalc
\param[in] deltaHigh Разница высоты между выбранными моментами
\param[in] periodCalc Разница по времени между выбранными моментами
\return Значение атаки при данных параметрах
*/
double attack(double velocityVectorXZ, double velocityXPrevious, double tangaz, double velocityY);





