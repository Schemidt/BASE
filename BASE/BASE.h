/*!
\file
\brief Класс Sound

Данный файл объявляет класс Sound и его наследников
*/

#pragma once
#include "Mono2channels.h"
#include "memory"

using namespace std;


template <class T>
/*!
\brief Шаблон высвобождения объекта
\details Высвобождает объект и обнуляет указатель
*/
void Free(T &x)
{
	delete x;
	x = NULL;
}

#pragma pack ( push, 1 )

/*!
\struct WAVEHEADER

Структура интерпретации загруженного wave файла
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

#ifndef Sound_h
#define Sound_h
/*!
\brief Класс состояния агрегатов вертолета
\author Самсонов А.В.
\version 5.1
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
	static AL_SOUND_CHANNELS channelsSetup;//!< Конфигурация каналов - устройств вывода (2.1,4.1,5.1,6.1,7.1)
	static double currentTime;//!< Переменная хранящая текущее время полученное из общей памяти от USPO
	static double deltaTime;//!< Переменная для отслеживания изменения времени

	static double step; //!< Шаг
	static double tangaz;//!< Тангаж
	static double derivTangaz;//!<скорость изменения Тангажа (dtangaz/dt)
	static double high;//!< Высота
	static double velocityX;//!< приборная скорость
	static double accelerationX;//!< Ускорение по приборной скорости (dvx/dt)
	static double velocityY;//!< вертикальная скорость
	static double dash;//!< Рывок (d2vx/dt)
	static double accelerationVy;//!< Вертикальное ускорение (dvy/dt)
	static double derivStep;//!< Скорость изменения шага
	static double calcA;//!< Атака винта
	static double RedTurnAcc;//!< Ускорение оборотов редуктора (dturn/dt)

	static vector<double> vectorHigh;//!<Массив для рассчета производной от высоты
	static vector<double> vectorVy;//!<Массив для рассчета производной от вертикальной скорости
	static vector<double> vectorVx;//!<Массив для рассчета производной от горизонтальной скорости
	static vector<double> vectorAcc;//!<Массив для рассчета производной от горизонтального ускорения
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
	unique_ptr<ALuint[]> source;//!< Переменная для источника
	unique_ptr<ALuint[]> buffer;//!< Переменная для буффера
	unique_ptr<ALuint[]> effectSlot;//!< Переменная для слота эффекта
	unique_ptr<ALuint[]> effect;//!< переменная для эффекта
	unique_ptr<ALuint[]> filter;//!< переменная для эффекта
	string load;
	bool soundOn = 0;//!< Переменная для определения состояния звука
	bool soundWork = 0;//!< Переменная для определения состояния звука
	bool soundOff = 0;//!< Переменная для определения состояния звука
	float offsetOn = 0;//!< Переменная для хранения отступа от начала файла в секундах, как правило для файла запуска агрегата
	float offsetOff = 0;//!< Переменная для хранения отступа от начала файла в секундах, как правило для файла остановки агрегата
	double lengthOn = 0;//!< Переменная для хранения длительности файла в секундах, как правило для файла запуска агрегата
	double lengthOff = 0;//!< Переменная для хранения длительности файла в секундах, как правило для файла остановки агрегата
	double pitch = 1;//!< Переменная для параметра высоты тона звука агрегата
	double gain = 1;//!< Переменная для параметра громкости звука агрегата
	vector<double> channel = { 1,1,0,0,0,0,0 };//!< массив для поканального вывода звука
	int sourceNumber = 1;//!< Переменная для хранения количества источников используемых объектом звука агрегата
	int bufferNumber = 3;//!< Переменная для хранения количества буфферов используемых объектом звука агрегата
	int effectSlotNumber = 0;//!< Переменная для хранения количества слотов эффектов используемых объектом звука агрегата

	Sound();//!< Конструктор по умолчанию, для объекта с 1им источником
	Sound(const Sound &copy);//!< Конструктор копирования*/
	Sound(int sources, int buffers, int effectslots);//!< Конструктор для объекта с sources источниками, buffers буферами и effectslots слотами эффектов
	~Sound();//!< Деструктор (да неужели)

	/*!
	\brief Вычисляет длительность WAVE файла
	\details Вычисляет длительность несжатого WAVE файла
	\param[in] filename имя файла
	\return длительность WAVE файла
	*/
	double getLengthWAV(string filename);

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
	virtual int play(bool status, string path_on, string path_w, string path_off, double gain_mult);

	/*!
	\brief Загружает буфер данными
	\details Структурирует массив данных для поканального вывода при различных конфигурациях устройств вывода и
	загружает в буфер OpenAL
	\param[in] Buffer Объект буфера
	\param[in] path Имя файла
	\param[in] channelsCount Конфигурация устройств вывода
	\param[in] channels Каналы для вывода
	\return 1 если успешно, иначе 0
	*/
	int setBuffer(ALuint Buffer, string path, AL_SOUND_CHANNELS channelsCount, vector<double> channels);

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
	int setAndDeploySound(ALuint *Buffer, ALuint *Source, double offset, string file_path);

	/*!
	\brief Подключает буфер к источнику и запускает
	\details
	<pre>
	Предполагается что буферы уже заполнены необходимыми данными,
	и требуется лишь переподлючать их к источнику.
	Призвана убрать задержку при переключении буферов, что вызывает кликающий звук.
	</pre>
	\param[in] Buffer Объект буфера
	\param[in] Source Объект источника
	\param[in] offset оступ от начала файла, сек
	\return Статус источника OpenAL
	*/
	int switchBufferAndPlay(ALuint *Buffer, ALuint *Source, double offset);

	double getAverange(string parameter, double seconds);

};

/*!
\brief Класс объекта редуктора
\author Самсонов А.В.
\version 2.0
\date  06.03.2018

Класс определяющий работу объекта - редуктора
*/
class Reductor : public Sound
{
public:

	bool hovering = 0;/*!< Переменная для статуса полета на висении */

	string eq[2];/*!< Переменная для однократной загрузки буфера */
	string pinkNoise;/*!< Переменная для однократной загрузки буфера */
	string beats;/*!< Переменная для однократной загрузки буфера */
	string filetoBuffer[2];/*!< Переменная для однократной загрузки буфера */
	string fileBuffered[2];/*!< Переменная для хранения имени загруженного файла */
	double offset[2] = { 0 };

	Reductor();

	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);
};

/*!
\brief Класс объекта двигателя
\author Самсонов А.В.
\version 2.0
\date  06.03.2018

Класс определяющий работу объекта - двигателя
*/
class Engine : public Sound
{
public:

	static int engCount;/*!< Переменная для количества инициализированных двигателей в программе */
	double phase;//!<Фаза для двигателей, чтобы их звуки не сливались(0-1, смещаем на 0.33 для каждого нового объекта, т.е. запускаем с 33% * n процентов длительности)
	int engNum;//!<Номер двигателя

	string eq[2];/*!< Переменная для однократной загрузки буфера */
	string filetoBuffer[2];/*!< Переменная для однократной загрузки буфера */
	string fileBuffered[2];/*!< Переменная для хранения имени загруженного файла */
	double offset[2] = { 0 };

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
	int play(bool status_on, bool status_off, double parameter, Helicopter h);
};

/*!
\brief Класс объекта - звук движения по ВПП
\author Самсонов А.В.
\version 2.0
\date  06.03.2018

Класс определяющий логику звука движения по ВПП и РД
*/
class Runway : public Sound
{
public:

	string eq;/*!< Переменная для однократной загрузки буфера */
	string filetoBuffer[2];/*!< Переменная для однократной загрузки буфера */
	string fileBuffered[2];/*!< Переменная для хранения имени загруженного файла */
	double offset[2] = { 0 };

	Runway();

	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);
};

/*!
\brief Класс объекта - хлопков винта
\author Самсонов А.В.
\version 2.0
\date  06.03.2018

Класс определяющий логику звука хлопков винта
*/
class VintFlap : public Sound
{
public:

	string key[3];/*!< Переменная для однократной загрузки буфера */

	VintFlap();

	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);
};

/*!
\brief Класс объекта - свиста винта
\author Самсонов А.В.
\version 2.0
\date  06.03.2018

Класс определяющий логику звука свиста винта
*/
class VintSwish : public Sound
{
public:

	string filetoBuffer[2];/*!< Переменная для однократной загрузки буфера */
	string fileBuffered[2];/*!< Переменная для хранения имени загруженного файла */
	double offset[2] = { 0 };

	VintSwish();

	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);
};

/*!
\brief Класс объекта - СКВ
\author Самсонов А.В.
\version 2.0
\date  06.03.2018

Класс определяющий логику звука объекта - СКВ (система кондиционирования воздуха)
*/
class Skv : public Sound
{
public:

	string eq;/*!< Переменная для однократной загрузки буфера */

	Skv();

	/*!
	\brief Определяет логику вывода звука
	\details определяет логику изменения параметров акустических объектов OpenAL
	\param[in] h Объект хранящий характеристики вертолета
	\param[in] sr Объект хранящий текущие параметры вертолета
	\return Статус источника OpenAL
	*/
	int play(Helicopter h, SOUNDREAD sr);

};

#endif

/*!
\brief Класс "точка"
\details Определяет класс точки
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
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически (точками)
\param[in] p Точки функции
\param[in] n Количество точек
\param[in] parameter Значение переменной в искомой точке
\return Возвращает значение функции
*/
double getValue(double parameter, int n, point p, ...);

/*!
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически (точками)
\param[in] p1 Точка функции
\param[in] p2 Точка функции
\param[in] x Значение переменной в искомой точке
\return Возвращает значение функции
*/
double getValue(point p1, point p2 , double x);

/*!
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически (точками), с учетом предельного значения функции
\param[in] p1 Точка функции
\param[in] p2 Точка функции
\param[in] x Значение переменной в искомой точке
\param[in] limit Предел функции
\param[in] w Переменная определяющая верхний или нижний предел
\return Возвращает значение функции
*/
double getValue(point p1, point p2, double x, double limit, string w);

/*!
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически (точками), с учетом предельного значения функции
\param[in] p1 Точка функции
\param[in] p2 Точка функции
\param[in] x Значение переменной в искомой точке
\param[in] low_limit Нижний предел функции
\param[in] hi_limit Верхний предел функции
\return Возвращает значение функции
*/
double getValue(point p1, point p2, double x, double low_limit, double hi_limit);

/*!
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически, векторами точек
\param[in] &value Вектор значений функции
\param[in] &time Вектор значений параметра
\param[in] offset Значение переменной в искомой точке
\return Возвращает значение функции
*/
double getParameterFromVector(vector<double> &value, vector<double> &time, double offset);

/*!
\brief Возвращает значение функции
\details Возвращает значение функции заданной графически, вектором точек
\param[in] &value Вектор точек функции
\param[in] offset Значение переменной в искомой точке
\return Возвращает значение функции
*/
double getParameterFromVector(vector<point> &value, double offset);

/*!
\brief Возвращает номер искомой ячейки вектора
\details Возвращает номер искомой ячейки упорядоченного вектора с помощью бинарного поиска
\param[in] &time Упорядоченный вектор переменных
\param[in] offset Значение переменной в искомой точке
\return Номер искомой ячейки
*/
int binSer(vector<double> &time, double offset);

/*!
\brief Возвращает номер искомой ячейки вектора
\details Возвращает номер искомой ячейки упорядоченного вектора с помощью бинарного поиска
\param[in] &time Вектор точек
\param[in] offset Значение переменной в искомой точке
\return Номер искомой ячейки
*/
int binSer(vector<point> &time, double offset);

/*!\brief Очищает объекты OpenAL*/
void freeOpenAL();
/*!
\brief Возвращает Pitch
\details Возвращает Pitch оценивая значения parameter из базы filename,которая хранит
значения parameter при записи аудиофайла относительно начала записи offset, и входящего параметра parameter
Например: при значении оборотов редуктора равного 60% на 32 секунде проигрывания файла, вернет значение pitch = 60/59
т.к. в базе значений, при записи, на 32 секунде редуктор имел 59% оборотов
если реальное и базовое значения отступа от начала в секундах, не совпадают - применяется квадратичная интерполяция
\param[in] offset Время от начала проигрывания записи
\param[in] filename Путь к файлу с переходной функцией
\param[in] parameter Значение параметра
\return Возвращает значение высоты звука
*/
double getPitch(double offset, string filename, double parameter);
/*!
\brief Возвращает Offset
\details Возвращает время в секундах на которое необходимо сделать отступ от начала проигрывания звука учитывая переходную функцию параметра
, его значение в данный момент, и отличие реальной кривой от эталонной.
\param[in] pitch Отношение значения реального параметра и его значения при получении переходной функции
\param[in] filename Путь к файлу с переходной функцией
\param[in] parameter Значение параметра
\return Время отступа в секундах
*/
double getOffset(double pitch, string filename, double parameter);
/*!
\brief Производит Crossfade
\details Осуществляет плавный переход 1ой записи в другую путем уменьшения громкости 1ой записи и повышения громкости другой, по мере изменения параметра
от значения limit1 к limit2.
<pre>
Если громкость звуков изменяется не в границах [0..1],
то стоит указать множитель громкости - т.е. предельная громкость нарастающей записи
\image html crossFade.png "CrossFade"
</pre>
\param[in] gf Громкость затухающего источника
\param[in] gr Громкость нарастающего источника
\param[in] parameter Значение параметра
\param[in] limit1 Значение параметра, при уровне громкости затухающей записи равном 1
\param[in] limit2 Значение параметра, при уровне громкости Нарастающей записи равном 1
\param[in] mult Множитель громкости записей
\return 1 ,если громкость затухающей записи равняется 0, а нарастающей 1
*/
int crossFade(double *gf, double *gr, double parameter, double limit1, double limit2, double mult);
/*!\brief Определяет указатели на функции расширений EFX*/
void setEFXPointers();
/*!
\brief Вычисляет квадратную интерполяцию
\details Вычисляет интерполяцию для fx в точке x,на отрезке [x0,x2], при определенных неодинаковых x0,x1,x2 и известных fx0,fx1,fx2, если x0=x1 или x1=x2
вычисляется линейная интерполяция

Код вычисления интерполяции:
\code
a2 = ((fx2 - fx0) / ((x2 - x0)*(x2 - x1))) - ((fx1 - fx0) / ((x1 - x0)*(x2 - x1)));
a1 = ((fx1 - fx0) / (x1 - x0)) - (a2*(x1 + x0));
a0 = fx0 - a1 * x0 - a2 * x0*x0;
return fx = a0 + a1 * x + a2*x*x;
\endcode

\param[in] x0 Абсцисса 1ой точки
\param[in] fx0 Ордината 1ой точки
\param[in] x1 Абсцисса 2ой точки
\param[in] fx1 Ордината 2ой точки
\param[in] x2 Абсцисса 3ой точки
\param[in] fx2 Ордината 3ой точки
\param[in] x Абсцисса искомой fx
\return Значение fx в точке x
*/
double interpolation(double x0, double fx0, double x1, double fx1, double x2, double fx2, double x);
/*!
\brief Вычисляет линейную интерполяцию
\details В точках x>x1 и x<x0 - возвращает граничные значения

Код вычисления интерполяции:
\code
fx = fx0 + ((fx1 - fx0) / (x1 - x0))*(x - x0);
\endcode

\param[in] x0 Абсцисса 1ой точки
\param[in] fx0 Ордината 1ой точки
\param[in] x1 Абсцисса 2ой точки
\param[in] fx1 Ордината 2ой точки
\param[in] x Абсцисса искомой fx
\return Значение fx в точке x
*/
double interpolation(double x0, double fx0, double x1, double fx1, double x);

double interpolation(point p1, point p2, double x);

double interpolation(point p1, point p2, point p3, double x);

/*!\brief Функция отладки*/
void printProgrammStatus(SOUNDREAD srd);
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
calcA = atan(tan(tangaz) - (2 * deltaHigh) / ((velocityXPrevious + velocityX) * cos(tangaz) * periodCalc));
calcA = calcA * 180 / M_PI;
\endcode

\param[in] velocityX Приборная скорость в прошлый момент времени t0
\param[in] velocityXPrevious Приборная скорость в момент времени t0 + periodCalc
\param[in] tangaz Тангаж в момент времени t0 + periodCalc
\param[in] deltaHigh Разница высоты между выбранными моментами
\param[in] periodCalc Разница по времени между выбранными моментами
\return Значение атаки при данных параметрах
*/
double attack(double velocityX, double velocityXPrevious, double tangaz, double deltaHigh, double periodCalc);
/*!
\brief Проверяет наличие процесса
\details Проверяет наличие процесса

\param[in] szExe имя процесса
\return True если процесс выполняется в системе, иначе False
*/
bool IsProcessPresent(wchar_t * szExe);

/*!
\brief Переводит коэффициент громкости в децибелы

\param[in] coef коэффициент громкости
\return децибелы
*/
double toDb(double coef);

/*!
\brief Переводит децибелы в коэффициент громкости

\param[in] db децибелы
\return коэффициент громкости
*/
double toCoef(double db);

