/*!
\file
\brief Основной файл программы

Данный файл связывает все элементы программы и определяет ее работу.
Определяет методы Sound и наследников.
*/

/*! \mainpage Документация

Эта документация разделена на следующие разделы:
- \subpage intro
- \subpage advanced
- \subpage todo
*/

/*! \page intro Введение
<pre>
Данная документация призвана помочь программисту разобраться в работе данного ПО.
</pre>

\section install_sec Установка
<pre>
Копируем папку с проектом в любое удобное место
</pre>

\section Use_sec Использование
<pre>
Запустить из коммандной строки "BASE.exe" с необходимым параметром:
"mi_8_mtv5","mi_8_amtsh","mi_26","mi_28","ka_226","ansat","ka_27","ka_29".
</pre>
*/

/*! \page advanced Основные сведения
\tableofcontents
<pre>
Ниже будут поясненны все элементы программы, которые ,по мнению разработчика,
могут быть непонятны будущему разработчику или тестировщику
</pre>

\section sec0 Общая структура и диаграмма последовательности
<pre>
Данный компонент можно разбить на 4 части:
•	Логика обмена данными через общую память
•	Класс определяющий объект вертолета – его основные константы, коэффициенты громкости и пути хранения звуков
•	Класс определяющий параметры и функции объектов - звуков вертолета
•	Инициализация объектов основной функцией («main»)
Если рассматривать работу компонента, используя эти структурные элементы, то можно описать процесс так:
1.	Создается объект вертолета, который определяет используемые звуки и набор агрегатов, на борту.
2.	Через общую память компонент получает набор параметров.
3.	Если набор параметров удовлетворят условию создания объекта звука, и он присутствует на борту – он создается.
4.	Объект инициализируется по логике описанной для конкретного агрегата и параметрами заданными для выбранного борта – звук передается на устройства вывода.
5.	При выполнении условий уничтожения объекта звука – память высвобождается.
\image html seqD.png "Диаграмма последовательности"
</pre>

\section sec1 Структура "main"
<pre>
Внутри основной функции происходит инициализация аудио библиотеки - вывод звука устанавливается на устройство вывода по умолчанию,
определяются настройки «контекста» OpenAL, в начале работы программы и деинициализация в конце.
Основу кода функции составляет расчёт недостающих параметров ЛА,
которые не были получены по протоколу обмена и код обработки объектов устройств – условия их инициализации,
выполнение метода «play», условия высвобождения объектов.
После инициализации «контекста» жизненный цикл объектов устройств прогоняется в бесконечном цикле.
\code
//ПТС
if (helicopter.ptsFactor)
{
	if (localdata.p_pts)//Условие создания объекта
		if (!pts)//Если объект не создан
		{
			pts = new Sound;
		}
	if (pts)//Если объект создан - используем его
	{
		pts->play(localdata.p_pts, helicopter.fullName["pts_on"], helicopter.fullName["pts_w"], helicopter.fullName["pts_off"], helicopter.ptsFactor);//Воспроизводим звук - записываем состояние звука в play
		if (pts->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
		{
			Free(pts);//Удаляем объект
		}
	}
}
\endcode
</pre>

\section sec2 Класс "Sound" и подклассы
<pre>
Данный класс и его наследники определяет каким образом реагирует источник OpenAL на изменение входящих параметров.
Определение класса построено таким образом, что для объявления нового агрегата достаточно создать для него новый объект типа «Sound» и инициализировать функцию «play».
Конструктор класса по умолчанию создаст 3 буфера и 1 источник OpenAL, что достаточно для большинства описываемых устройств, и высвободит их в деструкторе, когда устройство завершит свою работу.
При инициализации функции «play» в нее передается признак работы устройства.
«1» соответствует включению устройства, которое по завершению переходит в фазу работы, «0» соответствует выключению.
Если объект инициализируется конструктором по умолчанию и его состояние определяется 1 параметром (включается – выключается),
то будем определять такой объект «простым» (этот параметр однако может быть результатом логических операций между несколькими параметрами).
Если объект инициализируется конструктором, с конкретным количеством необходимых источников и буферов, а также объектов эффектов,
и его состояние определяется несколькими параметрами (высота, горизонтальная скорость, вертикальная скорость, ускорение, рывок, и т. д.),
то будем определять такой объект «сложным».
Как правило для каждого «сложного» объекта создается наследник базового класса «Sound»,
где переопределяется метод «play» и добавляются новые переменные.
</pre>

\subsection subsec0 "Простые" устройства
<pre>
В ходе исследования звучания устройств ЛА удалось установить, что большинство устройств и физических эффектов имеют 3 глобальных состояния работы: запуск, режим работы, выключение.
Состоянием в данном случае будет являться характерное звучание прибора: нарастание звука, монотонное воспроизведение звука на определенном уровне и затухание звука.
Причем у некоторых устройств может отсутствовать до 2х состояний.
Исходя из этого удалось построить общую структуру для программирования логики подобных устройств и сформировать общую функцию.
Рассмотрим логику работы функции при различных входных параметрах и приведем примеры устройств.
Работа функции построена по следующему принципу:
1.	При поступлении признака работы агрегата (1) воспроизводится запись установления сигнала
2.	По ее окончанию воспроизводится в цикле звук установившегося режима сигнала
3.	При поступлении признака отключения агрегата (0) воспроизводится запись выключения
Работа функции учитывает возможность отсутствия 1й, 2х или всех фаз, в таком случает на место входного параметра устанавливается константа «NULL».
Описание различных вариаций отработки функции указаны в таблице:
<table>
<caption id="multi_row">Таблица комбинаций входных параметров функции</caption>
<tr><th>Комбинация Фаз<th>Изображение сигнала (Амплитуда/время)<th>Пояснение
<tr>
<td>Все Фазы присутствуют
<td>\image html 111.png width=250 height=150
<td>Последовательная отработка фаз при изменении признака работы.
(Трансформаторы 36 и 115, ПТС)
<tr>
<td>Запуск отсутствует
<td>\image html 011.png width=250 height=150
<td>Мгновенный переход на режим работы при признаке – 1, плавный выход при 0.
<tr>
<td>Остановка Отсутствует
<td>\image html 110.png width=250 height=150
<td>Плавный переход на режим работы, мгновенная остановка сигнала при признаке – 0
(Насосы топливных баков)
<tr>
<td>Отсутствует работа
<td>\image html 101.png width=250 height=150
<td>Предполагается что сигнал на режиме работы не слышен,
в таком случае после установления режима работы звук пропадает и появляется только при выключении
(Звук тормоза шасси)
<tr>
<td>Только работа
<td>\image html 010.png width=150 height=150
<td>Применим для сигналов не имеющих слышимой фазы нарастания и убывания (Аккумулятор, тон ПТС, НИП)
<tr>
<td>Только запуск
<td>\image html 100.png width=150 height=150
<td>Применим для воспроизведения одиночных сигналов (выстрелы, крушения, краны)
<tr>
<td>Только выключение
<td>\image html 001.png width=150 height=150
<td>Применим для сигналов звук которых появляется только при признаке 0
(Применение не найдено, возможно невозможно)
<tr>
<td>Все фазы отсутствуют
<td>0
<td>Звук не воспроизводится, высвобождается память

</table>
</pre>

\subsection subsec1 "Сложные" устройства
<pre>
Конечно, все устройства на борту ЛА можно определить, как «сложные»,
но в таком случае потребуются колоссальные вычислительные мощности.
Поэтому основное внимание при разработке комплекса уделяется наиболее важным устройствам и эффектам,
характерное звучание которых вносит основной вклад в формирование акустического образа ЛА.
К таким относятся: главный редуктор,
двигатели, звук хлопков винта, свист винта, движение по взлетной полосе.
Для установления корреляций между параметрами ЛА и звучанием устройств инженер-акустик оценивает спектр и
субъективное звучание аудио сигнала в различные моменты времени во время работы ЛА.
</pre>

\section sec3 Назначение <Helicopter>
<pre>
Данный класс агрегирует имена файлов всех звуков, которые используются при загрузке буферов OpenAL.
После создания объекта данного класса вызывается метод класса, который принимает в качестве входного параметра конкретную модель ЛА,
устанавливая пути к конкретным файлам: %модель%/%имяфайла%.wav.
Кроме адресов файлов также определяются коэффициенты громкости звуков.
Коэффициент равный «0» означает, что звук данного агрегата или эффекта отсутствует на данном ЛА.
В таком случае алгоритм не исполняется вовсе, в отличии от воспроизведения файла с 0-вой громкостью,
что экономит память и процессорное время.
И наконец определяются некоторые константы необходимые для корректного вывода звука,
например обороты двигателей и редуктора на различных режимах работы.
\code
if (model == "mi_8_mtv5")
{
	//Константы
	engTurnoverHp = 20;
	engTurnoverMg = 75;
	engTurnoverAvt = 88;
	redTurnoverMg1 = 50;
	redTurnoverMg2 = 63;
	redTurnoverAvt = 96;
	//Множители громкости
	vsuFactor = 1//всу;
	vsuHpFactor = 0.06//всу хп;
	engFactor = 1//дв;
	engHpFactor = 0.35//дв хп;
	redFactor = 1//Редуктор;
	accumFactor = 0.1//аккум;
	ptsFactor = 0.18//птс;
	ptsToneFactor = 0.25//птс тон;
	tr36Factor = 0.15//тр36;
	tr115Factor = 0.15//тр115;
	pumpLeftFactor = 0.08//подк л;
	pumpRightFactor = 0.12//подк р;
	cutoffCraneFactor = 0.18//перекр;
	ko50Factor = 0.5//КО-50;
	runwayFactor = 1//ВПП;
	rocketNar8Factor = 1//НАР8;
	rocketNar13Factor = 1//НАР13;
	rocketHitFactor = 0.8//Попадание ракеты;
	upkFactor = 1//УПК;
	vintSwishFactor = 1//ВИНТ Верх;
	vintBrakeFactor = 0.2//Винт тормоз;
	vintFlapFactor = 0.794//Винт хлопки;
	chassisBrakePumpFactor = 0.03//Тормоз шасси;
	chassisBrakeReleaseFactor = 0.03//Сброс давления;
	consumTankFactor = 0.25//Расходный бак;
	rainFactor = 0.5//Дождь;
}
\endcode
</pre>
*/

/*! \page todo TODO
<pre>
Сюда добавляется информация по предлагаемым нововведениям в будущих версиях программы

	1.	DLL основанная структура программного комплекса.
	*	Позволит добавлять логику новых бортов без перекомпиляции всего проекта
	*	Позволит разрабатывать проект одновременно большому количеству сотрудников
	*	Позволит динамически подгружать необходимый код - снизит потребление памяти

	2.	Использовать Alure вместо Alute.
	*	Более современная, поддерживаемая версия Alute
	*	Упрощает менеджмент Параметров объектов OpenAL
	*	Позволит упростить код, и отказаться от некоторых функций

	3.	Организовать массив объектов - устройств.
	*	Все созданные объекты звуков добавляются в данный массив,
	*	что позволяет в любой момент оценивать количество и качество
	*	используемых объектов - агрегатов, а также использовать глобальные стратегии по управлению
	*	vector<Sound *> Sounds;

	4.	Организовать параллельную обработку функций вывода звука "play"
	*	Улучшит производительность системы
	*	Устранит возможные задержки - Улучшит отзывчивость системы

</pre>

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

SOUNDREAD soundread;/*!< Переменная класса soundread для хранения управляющих признаков */

int Sound::maxSlots;/*!< Переменная инициализирующаяся максимальным числом источников ,которые могут проигрываться одновременно*/
int Sound::maxSources;/*!< Переменная инициализирующаяся максимальным числом источников ,которые могут проигрываться одновременно*/
double Sound::masterGain = 1;//!< общая громкость звука
int Sound::sourcesInUse = 0;
int Sound::effectSlotsInUse = 0;
double Sound::currentTime = 0;//
double Sound::deltaTime = 0;//переменная для отслеживания изменения времени
double Sound::step = 0; //шаг (временно используем параметр перегрузки)
double Sound::tangaz = 0;
double Sound::high = 0;
double Sound::velocityX = 0;//приборная скорость
double Sound::accelerationX = 0;//
double Sound::velocityY = 0;//вертикальная скорость
double Sound::dash = 0;
double Sound::accelerationVy = 0;
double Sound::derivStep = 0;
double Sound::calcA = 0;
double Sound::RedTurnAcc = 0;
int Engine::engCount = 0;

vector<double> Sound::vectorHigh, Sound::vectorVy, Sound::vectorVx, Sound::vectorAcc, Sound::vectorStep, Sound::vectorTime, Sound::vectorRedTurn;
vector<double> Sound::vectorAvrEng1Turn, Sound::vectorAvrEng2Turn, Sound::vectorAvrRedTurn, Sound::vectorAvrStep, Sound::vectorAvrAtk;//!<Массивы для рассчета среднего методом скользящего среднего
double Sound::globalWindow = 50;//!<Переменная времени для набора значений в массивы для рассчета среднего

AL_SOUND_CHANNELS Sound::channelsSetup = AL_SOUND_CHANNELS_2;//Конфигурация каналов звука

/*!\brief Основная функция программы*/
int main(int argc, char *argv[])
{
	//Получаем указатели на функции EFX
	setEFXPointers();
	vector <string> helicoptersNames = { "mi_8_mtv5","mi_8_amtsh","mi_26","mi_28","ka_226","ansat","ka_27","ka_29" };
	string model;
	Helicopter helicopter;//Переменная класса Helicopter для хранения параметров выбранного вертолета
	if (argc > 1)// если передаем аргументы, то argc будет больше 1(в зависимости от кол-ва аргументов)
	{
		for (size_t i = 0; i < helicoptersNames.size(); i++)
		{
			if (regex_match(argv[1], regex("^(" + helicoptersNames[i] + ")")))
			{
				model = argv[1];
				helicopter.setParam(model);
			}
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

	Sound::maxSources = getMaxAvaliableSources(); //получаем максимальное количество источников которое можно использовать на данном устройстве
	cout << " " << Sound::maxSources << " - Sources avaliable" << endl;
	Sound::maxSlots = MAX_AUX_SLOTS;

	device = alcOpenDevice(0);
	if (device == 0)
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		return AL_FALSE;
	}
	context = alcCreateContext(device, attribs);
	if (context == 0)
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
	if (!shaInit())// Инициализация общей памяти 
	{
		cout << "shared memory initialization error..." << endl;
		return 0;
	}
	InitRealTime(1);//инициируется "реальное" время с задержкой в 1мс

	//Указатели на объекты определяющие параметры выводимого звука
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
	Skv *skv = nullptr;
	Sound *skvUni = nullptr;
	Sound *fire1 = nullptr;
	Sound *fire2 = nullptr;
	Sound *consTank = nullptr;
	Sound *pstat = nullptr;
	Sound *runway = nullptr;
	Runway *runwayMi8 = nullptr;
	Sound *crash = nullptr;
	VintSwish *vintSwish = nullptr;
	Sound *vintSwishUni = nullptr;
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
	/*Sound *undefined0 = nullptr;*/
	Sound *undefined1 = nullptr;
	Sound *vintFlapUniversal = nullptr;
	Sound *kranKolc = nullptr;
	Sound *vpryam = nullptr;
	Sound *vadd = nullptr;

	SOUNDREAD localdata = soundread;//локальная копия общего с USPO файла
	Sound::currentTime = localdata.time;
	Sound::vectorTime.push_back(Sound::currentTime);
	Sound::vectorHigh.push_back(Sound::high);
	Sound::vectorVy.push_back(Sound::velocityY);
	Sound::vectorVx.push_back(Sound::velocityX);
	Sound::vectorAcc.push_back(Sound::accelerationX);
	Sound::vectorStep.push_back(Sound::step);
	Sound::vectorRedTurn.push_back(localdata.reduktor_gl_obor);

	double timerPodk = 0;
	double timerNar8 = 0;
	double timerNar13 = 0;
	int counterNar8 = 0;
	int counterNar13 = 0;
	double vsuDownTimer = 0;
	double vsuUpTimer = 0;
	double timerAvr = 0;
	const double window = 1;//При вычислении приближенной производной берем изменение значения за секунду 
	double periodCalc = 0;//переменная для реального значения периода вычисления, равно или немного более window
	//Опрашиваем все блоки программы в бесконечном цикле
	while (true)
	{
		//Копируем данные из общей памяти во временное хранилище
		localdata = soundread;
		if (IsProcessPresent(L"USPO.exe") && !localdata.p_model_stop)
		{
			if (Sound::currentTime == 0)
				Sound::currentTime = localdata.time;
			//Вычисляем изменение времени с прошлого цикла работы программы
			Sound::deltaTime = localdata.time - Sound::currentTime;
			Sound::currentTime = localdata.time;
			Sound::masterGain = localdata.master_gain;
			Sound::tangaz = localdata.osadki;//тангаж (временно используем параметр инт осадков)
			Sound::velocityX = localdata.v;//приборная скорость
			Sound::high = localdata.styk_hv;
			Sound::step = localdata.ny; //шаг (временно используем параметр перегрузки)

			//Если не пришел признак остановки модели - вычисляем переменные
			while (periodCalc >= window && Sound::vectorTime.size() > 2)
			{
				if (!Sound::vectorTime.empty())
				{
					Sound::vectorHigh.erase(Sound::vectorHigh.begin());
					Sound::vectorVy.erase(Sound::vectorVy.begin());
					Sound::vectorVx.erase(Sound::vectorVx.begin());
					Sound::vectorAcc.erase(Sound::vectorAcc.begin());
					Sound::vectorStep.erase(Sound::vectorStep.begin());
					Sound::vectorTime.erase(Sound::vectorTime.begin());
					Sound::vectorRedTurn.erase(Sound::vectorRedTurn.begin());
				}
				periodCalc = Sound::currentTime - Sound::vectorTime.front();
			}
			if (Sound::vectorTime.back() != Sound::currentTime)
			{
				Sound::vectorTime.push_back(Sound::currentTime);
				Sound::vectorHigh.push_back(Sound::high);
				Sound::vectorVy.push_back(Sound::velocityY);
				Sound::vectorVx.push_back(Sound::velocityX);
				Sound::vectorAcc.push_back(Sound::accelerationX);
				Sound::vectorStep.push_back(Sound::step);
				Sound::vectorRedTurn.push_back(localdata.reduktor_gl_obor);
			}
			if (!Sound::vectorTime.empty())
			{
				//Производные
				periodCalc = Sound::currentTime - Sound::vectorTime.front();
				if (periodCalc > 0)
				{
					Sound::velocityY = (Sound::high - Sound::vectorHigh.front()) / periodCalc;
					Sound::accelerationVy = (Sound::velocityY - Sound::vectorVy.front()) / periodCalc;
					Sound::accelerationX = (Sound::velocityX - Sound::vectorVx.front()) / periodCalc;
					Sound::dash = (Sound::accelerationX - Sound::vectorAcc.front()) / periodCalc;
					Sound::derivStep = (Sound::step - Sound::vectorStep.front()) / periodCalc;
					Sound::calcA = attack(Sound::velocityX, Sound::vectorVx.front(), Sound::tangaz, Sound::high - Sound::vectorHigh.front(), periodCalc);
					Sound::RedTurnAcc = (localdata.reduktor_gl_obor - Sound::vectorRedTurn.front()) / periodCalc;
				}
			}

			//Набираем массивы для среднего
			timerAvr += Sound::deltaTime;
			if (timerAvr > Sound::globalWindow)
			{
				if (!Sound::vectorAvrEng1Turn.empty())
				{
					Sound::vectorAvrEng1Turn.erase(Sound::vectorAvrEng1Turn.begin());
				}
				if (!Sound::vectorAvrEng2Turn.empty())
				{
					Sound::vectorAvrEng2Turn.erase(Sound::vectorAvrEng2Turn.begin());
				}
				if (!Sound::vectorAvrRedTurn.empty())
				{
					Sound::vectorAvrRedTurn.erase(Sound::vectorAvrRedTurn.begin());
				}
				if (!Sound::vectorAvrStep.empty())
				{
					Sound::vectorAvrStep.erase(Sound::vectorAvrStep.begin());
				}
				if (!Sound::vectorAvrAtk.empty())
				{
					Sound::vectorAvrAtk.erase(Sound::vectorAvrAtk.begin());
				}
			}

			Sound::vectorAvrEng1Turn.push_back(localdata.eng1_obor);
			Sound::vectorAvrEng2Turn.push_back(localdata.eng2_obor);
			Sound::vectorAvrRedTurn.push_back(localdata.reduktor_gl_obor);
			Sound::vectorAvrStep.push_back(Sound::step);
			Sound::vectorAvrAtk.push_back(Sound::calcA);

			//ВСУ
			if (helicopter.vsuFactor)//Если ВСУ включено в проект
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
					vsu->offsetOff = vsu->lengthOff * (1 - localdata.vsu_obor / 100.);//Включаем запись с текущего уровня оборотов
					vsu->offsetOn = vsu->lengthOn * localdata.vsu_obor / 100.;//Включаем запись с текущего уровня оборотов

					//Для 27 29 и 8
					if (helicopter.modelName == "ka_27" || helicopter.modelName == "ka_29" || helicopter.modelName == "mi_8_mtv5" || helicopter.modelName == "mi_8_amtsh")
					{
						//Подсадка ВСУ при запущенном двигателе, но до запуска редуктора
						if (localdata.reduktor_gl_obor > 0)
						{
							vsu->gain = interpolation(0, 0.7, 2.5, 0.85, 5, 1, vsuUpTimer);
							vsuUpTimer += Sound::deltaTime;
							vsuDownTimer = 0;
						}
						else
						{
							if (localdata.p_eng1_zap || localdata.p_eng1_hp || localdata.p_eng2_zap || localdata.p_eng2_hp)
							{
								vsu->gain = interpolation(0, 1, 0.25, 0.85, 0.5, 0.7, vsuDownTimer);
								vsuDownTimer += Sound::deltaTime;
								vsuUpTimer = 0;
							}
							else
							{
								vsu->gain = interpolation(0, 0.7, 2.5, 0.85, 5, 1, vsuUpTimer);
								vsuUpTimer += Sound::deltaTime;
								vsuDownTimer = 0;
							}
						}
					}
					vsu->play(localdata.p_vsu_zap, helicopter.fullName["vsu_on"], helicopter.fullName["vsu_w"], helicopter.fullName["vsu_off"], helicopter.vsuFactor);
				}
				if (vsu && vsu->sourceStatus[0] != AL_PLAYING)
				{
					Free(vsu);
				}

				//ВСУ ХП
				if (localdata.p_vsu_hp)
				{
					if (!vsuHp)
					{
						vsuHp = new Sound;
					}
				}
				if (vsuHp)
				{
					//if (localdata.vsu_obor > 0 & localdata.vsu_obor < 35.)
					//{
					//	vsuHp->lengthOff = vsuHp->getLengthWAV(helicopter.fullName["vsu_hp_off"]);
					//	vsuHp->lengthOn = vsuHp->getLengthWAV(helicopter.fullName["vsu_hp_on"]);
					//	vsuHp->offsetOff = vsuHp->lengthOff * (1 - localdata.vsu_obor / 35.);//Включаем запись с текущего уровня оборотов
					//	vsuHp->offsetOn = vsuHp->lengthOn * localdata.vsu_obor / 35.;//Включаем запись с текущего уровня оборотов
					//}
					if (localdata.p_vsu_zap | localdata.p_vsu_ostanov)//если ВСУ работает - вырубаем прокрутку
					{
						vsuHp->play(localdata.p_vsu_hp, "NULL", "NULL", "NULL", helicopter.vsuHpFactor);
					}
					else
					{
						vsuHp->play(localdata.p_vsu_hp, helicopter.fullName["vsu_hp_on"], helicopter.fullName["vsu_hp_w"], helicopter.fullName["vsu_hp_off"], helicopter.vsuHpFactor);
					}
					if (vsuHp->sourceStatus[0] != AL_PLAYING)
					{
						Free(vsuHp);
					}
				}
			}
			//Аккумулятор
			if (helicopter.accumFactor)
			{
				if (helicopter.modelName != "ansat")//226
				{
					if (localdata.rez_2)//Условие создания объекта
					{
						if (!accum)//Если объект не создан 
						{
							accum = new Sound;//Создаем объект
						}
					}
					if (accum)//Если объект создан - используем его
					{
						accum->play(localdata.rez_2, "NULL", helicopter.fullName["accum"], "NULL", helicopter.accumFactor);//Воспроизводим звук - записываем состояние звука в play
						if (accum->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						{
							Free(accum);//Удаляем объект
						}
					}
				}
				if (helicopter.modelName == "ka_226" || helicopter.modelName == "ansat")//226
				{
					if (localdata.rez_2)//Условие создания объекта
					{
						if (!accumTr)//Если объект не создан 
						{
							accumTr = new Sound;//Создаем объект
						}
					}
					if (accumTr)//Если объект создан - используем его
					{
						accumTr->play(localdata.rez_2, "NULL", helicopter.fullName["accum_tr"], "NULL", helicopter.accumFactor);//Воспроизводим звук - записываем состояние звука в play
						if (accumTr->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						{
							Free(accumTr);//Удаляем объект
						}
					}
				}
			}
			//ПТС
			if (helicopter.ptsFactor)
			{
				if (localdata.p_pts)//Условие создания объекта
				{
					if (!pts)//Если объект не создан 
					{
						pts = new Sound;
					}
				}
				if (pts)//Если объект создан - используем его
				{
					pts->play(localdata.p_pts, helicopter.fullName["pts_on"], helicopter.fullName["pts_w"], helicopter.fullName["pts_off"], helicopter.ptsFactor);//Воспроизводим звук - записываем состояние звука в play
					if (pts->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
					{
						Free(pts);//Удаляем объект
					}
				}
			}
			//ПТС тон
			if (helicopter.ptsToneFactor)
			{
				//ПТС Тон
				if (localdata.p_pts)//Условие создания объекта
				{
					if (!ptsTone)//Если объект не создан 
					{
						ptsTone = new Sound;//Создаем объект
					}
				}
				if (ptsTone)//Если объект создан - используем его
				{
					ptsTone->play(localdata.p_pts, "NULL", helicopter.fullName["pts_tone"], "NULL", helicopter.ptsToneFactor);//Воспроизводим звук - записываем состояние звука в play
					if (ptsTone->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
					{
						Free(ptsTone);//Удаляем объект
					}
				}
			}
			//ВУ
			if (helicopter.vpryam)
			{
				//ВУ
				if (localdata.p_vu1)//Условие создания объекта
				{
					if (!vpryam)//Если объект не создан 
					{
						vpryam = new Sound;//Создаем объект
					}
				}
				if (vpryam)//Если объект создан - используем его
				{
					vpryam->play(localdata.p_vu1, "NULL", helicopter.fullName["vpryam"], "NULL", helicopter.vpryam);//Воспроизводим звук - записываем состояние звука в play
					if (vpryam->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
					{
						Free(vpryam);//Удаляем объект
					}
				}
			}
			//Если Трансформатор 36В присутствует на Борту
			if (helicopter.tr36Factor)
			{
				if (localdata.p_trans_36_osn || localdata.p_trans_36_rez)//Условие создания объекта
				{
					if (!tr36)//Если объект не создан 
					{
						tr36 = new Sound;//Создаем объект
					}
				}
				if (tr36)//Если объект создан - используем его
				{
					tr36->play(localdata.p_trans_36_osn || localdata.p_trans_36_rez, helicopter.fullName["tr36_on"], helicopter.fullName["tr36_w"], helicopter.fullName["tr36_off"], helicopter.tr36Factor);//Воспроизводим звук - записываем состояние звука в play
					if (tr36->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
					{
						Free(tr36);//Удаляем объект
					}
				}
			}
			//Если Трансформатор 115В присутствует на Борту
			if (helicopter.tr115Factor)
			{
				if (localdata.p_po500)//Условие создания объекта
				{
					if (!tr115)//Если объект не создан 
					{
						tr115 = new Sound;//Создаем объект
					}
				}
				if (tr115)//Если объект создан - используем его
				{
					tr115->play(localdata.p_po500, helicopter.fullName["tr115_on"], helicopter.fullName["tr115_w"], helicopter.fullName["tr115_off"], helicopter.tr115Factor);//Воспроизводим звук - записываем состояние звука в play
					if (tr115->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
					{
						Free(tr115);//Удаляем объект
					}
				}
			}
			//НИП
			if (helicopter.nipFactor)
			{
				if (localdata.rez_3)//Условие создания объекта
				{
					if (!nip)//Если объект не создан 
					{
						nip = new Sound;//Создаем объект
					}
				}
				if (nip)//Если объект создан - используем его
				{
					nip->play(localdata.rez_3, "NULL", helicopter.fullName["nip_tone"], "NULL", helicopter.nipFactor);
					if (nip->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
					{
						Free(nip);//Удаляем объект
					}
					else
					{
						double p1 = 1, p2 = 1;
						//Если включено ВУ, то высота тона повышается на 1.5%
						if (helicopter.modelName == "mi_26")
						{
							if (localdata.p_vu1)
							{
								p1 = 1.015;
							}
							else
							{
								p1 = 1;
							}
						}
						//Прокрутка и запуск ВСУ просаживают НИП по тону
						if (localdata.p_vsu_hp || localdata.p_vsu_zap)
						{

							nip->offsetOn += Sound::deltaTime;

							//_____  
							//  1  \____   
							//     0.985\  ______
							//		0.977\/
							//		 0.946
							//	    |1c |1c|

							//Сразу провал
							p2 = interpolation(0, 1, 1.5, 0.946, 2, 0.977, nip->offsetOn);

						}
						//Прокрутка и запуск ВСУ просаживают НИП по тону
						if (!localdata.p_vsu_hp && !localdata.p_vsu_zap)
						{
							nip->offsetOn = 0;
							p2 = 1;
						}

						nip->pitch = 1 * p1 * p2;//Вычисляем результирующий тон
					}

				}
			}
			//Гировертикаль
			if (helicopter.girovertFactor)
			{
				if (localdata.rez_3)//Условие создания объекта
				{
					if (!girovert)//Если объект не создан 
					{
						girovert = new Sound;//Создаем объект
					}
				}
				if (girovert)//Если объект создан - используем его
				{
					girovert->lengthOn = girovert->getLengthWAV(helicopter.fullName["girovert_on"]);
					if (girovert->pitch < 1)
					{
						girovert->offsetOn = girovert->lengthOn * girovert->pitch;//Включаем запись с текущего уровня оборотов
					}
					girovert->play(localdata.rez_3, helicopter.fullName["girovert_on"], helicopter.fullName["girovert_w"], helicopter.fullName["girovert_w"], helicopter.ptsFactor);
					if (girovert->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
					{
						Free(girovert);//Удаляем объект
					}
					else
					{
						if (girovert->soundOn)
						{
							girovert->pitch = 1;
						}
						if (girovert->soundOff)
						{
							alSourcei(girovert->source[0], AL_LOOPING, AL_TRUE);
							girovert->pitch -= Sound::deltaTime*0.0024;//уменьшаем питч по 0.0024 в сек
																	   //если Pitch == 0 - звук пропал, источник можно отключить
							if (girovert->pitch <= 0.8)
								girovert->play(localdata.rez_3, "NULL", "NULL", "NULL", helicopter.ptsFactor);

						}
					}
				}
			}
			//Топливна система
			//Если насосы подкачки присутствуют на Борту
			if (helicopter.pumpLeftFactor)
			{
				if (localdata.p_nasos_podk_1)//Условие создания объекта
				{
					if (!podk1)//Если объект не создан 
					{
						podk1 = new Sound;//Создаем объект
					}
				}
				if (podk1)//Если объект создан - используем его
				{
					podk1->channel[0] = 1;//L
					podk1->channel[1] = 0;
					if ("ka_226" == helicopter.modelName)
					{
						if (localdata.p_eng1_zap | localdata.p_eng1_hp | localdata.p_eng2_hp | localdata.p_eng2_zap)
						{
							timerPodk += Sound::deltaTime;
							podk1->pitch = interpolation(0, 1, 1, 0.85, 5, 0.88, timerPodk);
						}
						else
						{
							timerPodk = 0;
							podk1->pitch = 1;
						}
						podk1->play(localdata.p_nasos_podk_1, helicopter.fullName["podk_l_on"], helicopter.fullName["podk_l_w"], helicopter.fullName["podk_l_off"], helicopter.pumpLeftFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					else
					{
						podk1->play(localdata.p_nasos_podk_1, helicopter.fullName["podk_l_on"], helicopter.fullName["podk_l_w"], "NULL", helicopter.pumpLeftFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					if (podk1->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
					{
						Free(podk1);//Удаляем объект
					}
				}
			}
			if (helicopter.pumpRightFactor)
			{
				if (localdata.p_nasos_podk_2)//Условие создания объекта
				{
					if (!podk2)//Если объект не создан 
					{
						podk2 = new Sound;//Создаем объект
					}
				}
				if (podk2)//Если объект создан - используем его
				{
					podk2->channel[0] = 0;
					podk2->channel[1] = 1;//R
					if ("ka_226" == helicopter.modelName)
					{
						if (localdata.p_eng1_zap | localdata.p_eng1_hp | localdata.p_eng2_hp | localdata.p_eng2_zap)
						{
							timerPodk += Sound::deltaTime;
							podk2->pitch = interpolation(0, 1, 1, 0.85, 5, 0.88, timerPodk);
						}
						else
						{
							timerPodk = 0;
							podk2->pitch = 1;
						}
						podk2->play(localdata.p_nasos_podk_2, helicopter.fullName["podk_r_on"], helicopter.fullName["podk_r_w"], helicopter.fullName["podk_r_off"], helicopter.pumpRightFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					else
					{
						podk2->play(localdata.p_nasos_podk_2, helicopter.fullName["podk_r_on"], helicopter.fullName["podk_r_w"], "NULL", helicopter.pumpRightFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					if (podk2->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
					{
						Free(podk2);//Удаляем объект
					}
				}
			}
			//Если кран кольцевания присутствует на Борту
			if (helicopter.circlingCraneFactor)
			{
				if (localdata.p_kran_kolcev)//Условие создания объекта
				{
					if (!kranKolc)//Если объект не создан 
					{
						kranKolc = new Sound;//Создаем объект
					}
				}
				if (kranKolc)//Если объект создан - используем его
				{
					kranKolc->channel[0] = 1;
					kranKolc->channel[1] = 1;
					kranKolc->play(localdata.p_kran_kolcev, helicopter.fullName["kran_circle"], "NULL", "NULL", helicopter.circlingCraneFactor);//Воспроизводим звук - записываем состояние звука в play
					if (kranKolc->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_kolcev)//Условие удаления объекта
					{
						Free(kranKolc);//Удаляем объект
					}
				}
			}
			//Если перекрывные краны присутствуют на Борту
			if (helicopter.cutoffCraneFactor)
			{
				if (localdata.p_kran_perekr_1)//Условие создания объекта
				{
					if (!perek1)//Если объект не создан 
					{
						perek1 = new Sound;//Создаем объект
					}
				}
				if (perek1)//Если объект создан - используем его
				{
					perek1->channel[0] = 1;//L
					perek1->channel[1] = 0;
					perek1->play(localdata.p_kran_perekr_1, helicopter.fullName["perekr_l"], "NULL", "NULL", helicopter.cutoffCraneFactor);//Воспроизводим звук - записываем состояние звука в play
					if (perek1->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_perekr_1)//Условие удаления объекта
					{
						Free(perek1);//Удаляем объект
					}
				}

				if (localdata.p_kran_perekr_2)//Условие создания объекта
				{
					if (!perek2)//Если объект не создан 
					{
						perek2 = new Sound;//Создаем объект
					}
				}
				if (perek2)//Если объект создан - используем его
				{
					perek2->channel[0] = 0;
					perek2->channel[1] = 1;//R
					perek2->play(localdata.p_kran_perekr_2, helicopter.fullName["perekr_r"], "NULL", "NULL", helicopter.cutoffCraneFactor);//Воспроизводим звук - записываем состояние звука в play
					if (perek2->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_perekr_2)//Условие удаления объекта
					{
						Free(perek2);//Удаляем объект
					}
				}
			}
			//Если СКВ присутствует на Борту
			if (helicopter.skvFactor)
			{

				if (helicopter.modelName == "mi_28")
				{
					if (localdata.p_skv_on)//Условие создания объекта
						if (!skv)//Если объект не создан 
							skv = new Skv;//Создаем объект
					if (skv)//Если объект создан - используем его
					{
						skv->play(helicopter, localdata);//Воспроизводим звук - записываем состояние звука в play
						if (skv->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
							Free(skv);//Удаляем объект
					}
				}
				else
				{
					if (localdata.p_skv_on)//Условие создания объекта
						if (!skvUni)//Если объект не создан 
							skvUni = new Sound;//Создаем объект
					if (skvUni)//Если объект создан - используем его
					{
						skvUni->play(localdata.p_skv_on, helicopter.fullName["skv_on"], helicopter.fullName["skv_w"], helicopter.fullName["skv_off"], helicopter.skvFactor);//Воспроизводим звук - записываем состояние звука в play
						if (skvUni->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
							Free(skvUni);//Удаляем объект
					}
				}
			}
			//Если КО-50 присутствует на Борту
			if (helicopter.ko50Factor)
			{
				if (localdata.rez_9)//Условие создания объекта
					if (!ko50)//Если объект не создан 
						ko50 = new Sound;//Создаем объект
				if (ko50)//Если объект создан - используем его
				{
					ko50->play(localdata.rez_9, helicopter.fullName["ko50_on"], helicopter.fullName["ko50_w"], helicopter.fullName["ko50_off"], helicopter.ko50Factor);//Воспроизводим звук - записываем состояние звука в play
					if (ko50->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						Free(ko50);//Удаляем объект
				}
			}
			//Если насос расходного бака
			if (helicopter.consumTankFactor)
			{
				if (localdata.rez_4)//Условие создания объекта
					if (!consTank)//Если объект не создан 
						consTank = new Sound;//Создаем объект
				if (consTank)//Если объект создан - используем его
				{
					consTank->play(localdata.rez_4, helicopter.fullName["cons_tank_on"], helicopter.fullName["cons_tank_w"], "NULL", helicopter.consumTankFactor);//Воспроизводим звук - записываем состояние звука в play
					if (consTank->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						Free(consTank);//Удаляем объект
				}
			}
			//Пожарный кран
			if (helicopter.fireCraneFactor)
			{
				if (localdata.p_kran_poj_l)//Условие создания объекта
					if (!fire1)//Если объект не создан 
						fire1 = new Sound;//Создаем объект
				if (fire1)//Если объект создан - используем его
				{
					fire1->channel[0] = 1;//L
					fire1->channel[1] = 0;
					fire1->play(localdata.p_kran_poj_l, helicopter.fullName["kran_fire_l"], "NULL", "NULL", helicopter.fireCraneFactor);//Воспроизводим звук - записываем состояние звука в play
					if (fire1->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_poj_l)//Условие удаления объекта
						Free(fire1);//Удаляем объект
				}

				if (localdata.p_kran_poj_r)//Условие создания объекта
					if (!fire2)//Если объект не создан 
						fire2 = new Sound;//Создаем объект
				if (fire2)//Если объект создан - используем его
				{
					fire2->channel[0] = 0;
					fire2->channel[1] = 1;//R
					fire2->play(localdata.p_kran_poj_r, helicopter.fullName["kran_fire_r"], "NULL", "NULL", helicopter.fireCraneFactor);//Воспроизводим звук - записываем состояние звука в play
					if (fire2->sourceStatus[0] != AL_PLAYING && !localdata.p_kran_poj_r)//Условие удаления объекта
						Free(fire2);//Удаляем объект
				}
			}
			//Насосная станция
			if (helicopter.pumpStationFactor)
			{
				if (localdata.p_nasos)//Условие создания объекта
					if (!pstat)//Если объект не создан 
						pstat = new Sound;//Создаем объект
				if (pstat)//Если объект создан - используем его
				{
					pstat->play(localdata.p_nasos, helicopter.fullName["pstat_on"], helicopter.fullName["pstat_w"], helicopter.fullName["pstat_off"], helicopter.ko50Factor);//Воспроизводим звук - записываем состояние звука в play
					if (pstat->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						Free(pstat);//Удаляем объект
				}
			}
			//Движение по ВПП и РД
			//Если звуки движения по ВПП включены в проект борта
			if (helicopter.runwayFactor)
			{
				if (helicopter.modelName == "mi_8_amtsh" || helicopter.modelName == "mi_8_mtv5" || helicopter.modelName == "mi_26")
				{
					if (localdata.v != 0 && Sound::high <= 0)//Условие создания объекта
						if (!runwayMi8)//Если объект не создан 
							runwayMi8 = new Runway;//Создаем объект
					if (runwayMi8)//Если объект создан - используем его
					{
						runwayMi8->play(helicopter, localdata);//Воспроизводим звук - записываем состояние звука в play
						if (localdata.v == 0 || Sound::high > 0)//Условие удаления объекта
							Free(runwayMi8);//Удаляем объект
					}
				}
				else
				{
					if (localdata.v != 0 && Sound::high <= 0)//Условие создания объекта
						if (!runway)//Если объект не создан 
							runway = new Sound;//Создаем объект
					if (runway)//Если объект создан - используем его
					{
						runway->play(localdata.p_na_vpp && localdata.v != 0, "NULL", helicopter.fullName["runway"], "NULL", helicopter.runwayFactor);//Воспроизводим звук - записываем состояние звука в play
						if (localdata.v == 0 || Sound::high > 0)//Условие удаления объекта
							Free(runway);//Удаляем объект
						else
						{
							runway->gain = Sound::masterGain * interpolation(0, 0, 13.8, 1, abs(localdata.v)) * helicopter.runwayFactor;
						}
					}
				}
			}
			//Крушение
			//Если звук столкновения с препятствием включен в проект
			if (true)
			{
				if (localdata.p_crash)//Условие создания объекта
					if (!crash)//Если объект не создан 
						crash = new Sound;//Создаем объект
				if (crash)//Если объект создан - используем его
				{
					crash->play(localdata.p_crash, helicopter.fullName["crash"], "NULL", "NULL", 1);//Воспроизводим звук - записываем состояние звука в play
					if (crash->sourceStatus[0] != AL_PLAYING && !localdata.p_crash)//Условие удаления объекта
						Free(crash);//Удаляем объект
				}
			}
			//Винт
			if (helicopter.vintSwishFactor)
			{

				if (helicopter.modelName == "mi_28")
				{
					if (localdata.reduktor_gl_obor != 0 && localdata.p_vu3)//Условие создания объекта
						if (!vintSwish)//Если объект не создан 
							vintSwish = new VintSwish;//Создаем объект
					if (vintSwish)//Если объект создан - используем его
					{
						vintSwish->play(helicopter, localdata);
						if (localdata.reduktor_gl_obor == 0 || !localdata.p_vu3)//Условие удаления объекта
							Free(vintSwish);//Удаляем объект
					}
				}
				else
				{
					if (localdata.reduktor_gl_obor >= helicopter.redTurnoverMg2 && localdata.p_vu3)//Условие создания объекта
						if (!vintSwishUni)//Если объект не создан 
							vintSwishUni = new Sound;//Создаем объект
					if (vintSwishUni)//Если объект создан - используем его
					{
						vintSwishUni->play(localdata.reduktor_gl_obor >= helicopter.redTurnoverMg2, "NULL", helicopter.fullName["vint_hi"], "NULL", helicopter.vintSwishFactor);//Воспроизводим звук - записываем состояние звука в play
						if (localdata.reduktor_gl_obor < helicopter.redTurnoverMg2 || !localdata.p_vu3)//Условие удаления объекта
							Free(vintSwishUni);//Удаляем объект
						else
						{
							//Выбираем высоту тона в зависимости от оборотов редуктора в данный момент
							vintSwishUni->pitch = localdata.reduktor_gl_obor / helicopter.redTurnoverAvt;
							//Выключаем шелест винта на оборотах редуктора ниже оборотов малого газа редуктора
							//Линейно гасим звук в течении 3х оборотов
							if (localdata.reduktor_gl_obor <= helicopter.redTurnoverAvt)
								vintSwishUni->gain = interpolation(helicopter.redTurnoverAvt, 1, helicopter.redTurnoverMg2, 0, localdata.reduktor_gl_obor);
						}
					}
				}
			}
			//Если звук тормоза винта включен в проект
			if (helicopter.vintBrakeFactor)
			{
				if (localdata.tormoz_vint)//Условие создания объекта
					if (!vintBrake)//Если объект не создан 
						vintBrake = new Sound;//Создаем объект
				if (vintBrake)//Если объект создан - используем его
				{
					vintBrake->play(localdata.tormoz_vint, "NULL", helicopter.fullName["vint_torm"], "NULL", helicopter.vintBrakeFactor);//Воспроизводим звук - записываем состояние звука в play
					if (vintBrake->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						Free(vintBrake);//Удаляем объект
					else
					{
						vintBrake->pitch = localdata.reduktor_gl_obor / 30.;
					}
				}
			}
			//Если звук хлопков винта включен в проект
			if (helicopter.vintFlapFactor)
			{
				if (helicopter.modelName == "ka_29" || helicopter.modelName == "mi_8_mtv5" || helicopter.modelName == "mi_8_amtsh" || helicopter.modelName == "ka_27" || helicopter.modelName == "mi_28" || helicopter.modelName == "mi_26")
				{
					if (localdata.styk_hv > 0)//Условие создания объекта
						if (!vintFlap)//Если объект не создан 
							vintFlap = new VintFlap;//Создаем объект
					if (vintFlap)//Если объект создан - используем его
					{
						vintFlap->play(helicopter, localdata);//Воспроизводим звук - записываем состояние звука в play
						if (localdata.styk_hv <= 0)//Условие удаления объекта
							Free(vintFlap);//Удаляем объект
					}
				}
				else
				{
					if (localdata.rez_10)//Условие создания объекта
						if (!vintFlapUniversal)//Если объект не создан 
							vintFlapUniversal = new Sound;//Создаем объект
					if (vintFlapUniversal)//Если объект создан - используем его
					{
						vintFlapUniversal->play(localdata.rez_10, "NULL", helicopter.fullName["vint_flap"], "NULL", helicopter.vintFlapFactor);//Воспроизводим звук - записываем состояние звука в play
						if (vintFlapUniversal->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
							Free(vintFlapUniversal);//Удаляем объект
					}
				}

			}
			//Если тормоза включены на борту
			if (helicopter.chassisBrakePumpFactor)
			{
				if (localdata.p_tormoz)//Условие создания объекта
					if (!brake)//Если объект не создан 
						brake = new Sound;//Создаем объект
				if (brake)//Если объект создан - используем его
				{
					if (helicopter.modelName == "ka_27" | helicopter.modelName == "ka_29")
					{
						brake->play(localdata.p_tormoz, "NULL", helicopter.fullName["brake"], "NULL", helicopter.chassisBrakePumpFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					else
					{
						brake->play(localdata.p_tormoz, helicopter.fullName["brake"], "NULL", helicopter.fullName["poff"], helicopter.chassisBrakePumpFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					if (brake->sourceStatus[0] != AL_PLAYING && !localdata.p_tormoz)//Условие удаления объекта
						Free(brake);//Удаляем объект
				}
			}
			//Дождь
			if (true)
			{
				if (localdata.p_rain)//Условие создания объекта
					if (!rain)//Если объект не создан 
						rain = new Sound;//Создаем объект
				if (rain)//Если объект создан - используем его
				{
					rain->play(localdata.p_rain, "NULL", helicopter.fullName["rain"], "NULL", helicopter.rainFactor);//Воспроизводим звук - записываем состояние звука в play
					if (rain->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						Free(rain);//Удаляем объект
				}
			}
			//Если звук попадания ракеты включен в проект
			if (helicopter.rocketHitFactor)
			{
				if (localdata.p_rocket_hit)//Условие создания объекта
					if (!rocket)//Если объект не создан 
						rocket = new Sound;//Создаем объект
				if (rocket)//Если объект создан - используем его
				{
					rocket->play(localdata.p_rocket_hit, helicopter.fullName["rocket"], "NULL", "NULL", helicopter.rocketHitFactor);//Воспроизводим звук - записываем состояние звука в play
					if (rocket->sourceStatus[0] != AL_PLAYING && !localdata.p_rocket_hit)//Условие удаления объекта
						Free(rocket);//Удаляем объект
				}
			}
			//Если НАР8 имеется на борту
			if (helicopter.rocketNar8Factor)
			{
				if (localdata.p_nar_s8)//Условие создания объекта
				{
					timerNar8 += Sound::deltaTime;
					for (int i = 0; i < 20; i++)
					{
						//воспроизводим звук выстрела 1го НАР8 каждые 0.05с - 20 раз
						//каждые 20 выпусков процесс можно повторить без потери заднего фронта
						//количество необходимых каналов равно количеству пусков, которое в свою очередь
						//зависит от промежуточного интервала и длинны записи звука выстрела НАР8
						if (timerNar8 >= 0.05 * i & counterNar8 < 20 & i >= counterNar8)
						{
							if (!nar8[i])//Если объект не создан 
								nar8[i] = new Sound;//Создаем объект
							if (nar8[i])//Если объект создан - используем его
							{
								nar8[i]->play(localdata.p_nar_s8, helicopter.fullName["nar8"], "NULL", "NULL", helicopter.rocketNar8Factor);//Воспроизводим звук - записываем состояние звука в play
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
							if (nar8[i]->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
								Free(nar8[i]);//Удаляем объект
						}
					}
					timerNar8 = 0;
					counterNar8 = 0;
				}
			}
			//Если НАР13 имеется на борту
			if (helicopter.rocketNar13Factor)
			{
				if (localdata.p_nar_c13)//Условие создания объекта
				{
					timerNar13 += Sound::deltaTime;
					for (int i = 0; i < 5; i++)
					{
						//воспроизводим звук выстрела 1го НАР13 каждые 0.12с - 5 раз
						//каждые 5 выпусков процесс можно повторить без потери заднего фронта
						//количество необходимых каналов равно количеству пусков, которое в свою очередь
						//зависит от промежуточного интервала и длинны записи звука выстрела НАР13
						if (timerNar13 >= 0.12 * i & counterNar13 < 5 & i >= counterNar13)
						{
							if (!nar13[i])//Если объект не создан 
								nar13[i] = new Sound;//Создаем объект
							if (nar13[i])//Если объект создан - используем его
							{
								nar13[i]->play(localdata.p_nar_c13, helicopter.fullName["nar13"], "NULL", "NULL", helicopter.rocketNar13Factor);//Воспроизводим звук - записываем состояние звука в play
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
							if (nar13[i]->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
								Free(nar13[i]);//Удаляем объект
						}
					}
					timerNar13 = 0;
					counterNar13 = 0;

				}
			}
			//Если ППУ имеется на борту
			if (helicopter.ppuFactor)
			{
				if (localdata.p_spo_ppu)//Условие создания объекта
					if (!ppu)//Если объект не создан 
						ppu = new Sound;//Создаем объект
				if (ppu)//Если объект создан - используем его
				{
					ppu->play(localdata.p_spo_ppu, "NULL", helicopter.fullName["ppu"], "NULL", helicopter.ppuFactor);//Воспроизводим звук - записываем состояние звука в play
					if (ppu->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						Free(ppu);//Удаляем объект
				}
			}
			//Если УПК имеется на борту
			if (helicopter.upkFactor)
			{
				if (localdata.p_spo_upk)//Условие создания объекта
					if (!upk)//Если объект не создан 
						upk = new Sound;//Создаем объект
				if (upk)//Если объект создан - используем его
				{
					upk->play(localdata.p_spo_upk, "NULL", helicopter.fullName["upk"], "NULL", helicopter.upkFactor);//Воспроизводим звук - записываем состояние звука в play
					if (upk->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						Free(upk);//Удаляем объект
				}
			}
			//Если УР ШТУРМ имеется на борту
			if (helicopter.rocketSturmFactor)
			{
				if (localdata.p_ur_ataka)//Условие создания объекта
					if (!sturm)//Если объект не создан 
						sturm = new Sound;//Создаем объект
				if (sturm)//Если объект создан - используем его
				{
					sturm->play(localdata.p_ur_ataka, helicopter.fullName["sturm"], "NULL", "NULL", helicopter.rocketSturmFactor);//Воспроизводим звук - записываем состояние звука в play
					if (sturm->sourceStatus[0] != AL_PLAYING && !localdata.p_ur_ataka)//Условие удаления объекта
						Free(sturm);//Удаляем объект
				}
			}
			//Если УР ИГЛА имеется на борту
			if (helicopter.rocketIglaFactor)
			{
				if (localdata.p_ur_igla)//Условие создания объекта
					if (!igla)//Если объект не создан 
						igla = new Sound;//Создаем объект
				if (igla)//Если объект создан - используем его
				{
					igla->play(localdata.p_ur_igla, helicopter.fullName["igla"], "NULL", "NULL", helicopter.rocketIglaFactor);//Воспроизводим звук - записываем состояние звука в play
					if (igla->sourceStatus[0] != AL_PLAYING && !localdata.p_ur_igla)//Условие удаления объекта
						Free(igla);//Удаляем объект
				}
			}
			//Редуктор
			if (helicopter.redFactor)
			{
				if (localdata.p_reduktor_gl_crash)//Условие создания объекта
					if (!redCrash)//Если объект не создан 
						redCrash = new Sound;//Создаем объект
				if (redCrash)//Если объект создан - используем его
				{
					redCrash->play(localdata.p_reduktor_gl_crash, "NULL", helicopter.fullName["red_crash"], "NULL", helicopter.redFactor);//Воспроизводим звук - записываем состояние звука в play
					if (redCrash->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						Free(redCrash);//Удаляем объект
					else
						redCrash->pitch = localdata.reduktor_gl_obor / helicopter.redTurnoverAvt / 2.0;
				}

				if (localdata.p_eng1_zap | localdata.p_eng2_zap | localdata.reduktor_gl_obor > 0)//условие создания объекта редуктора
					if (!red)//Если объект не создан 
						red = new Reductor;//Создаем объект
				if (red)//Если объект создан - используем его
				{
					red->channel[0] = 1;
					red->channel[1] = 1;
					red->play(helicopter, localdata);//
					if (red->sourceStatus[0] != AL_PLAYING && red->sourceStatus[1] != AL_PLAYING && !(localdata.p_eng1_zap | localdata.p_eng2_zap | localdata.reduktor_gl_obor > 0))//Условие удаления объекта
						Free(red);//Удаляем объект
				}

			}
			//Скоростная добавка
			if (helicopter.vadd)
			{
				if (localdata.v != 0)//Условие создания объекта
				{
					if (!vadd)//Если объект не создан 
					{
						vadd = new Sound;
					}
				}
				if (vadd)//Если объект создан - используем его
				{
					double g = 0;
					if (helicopter.modelName == "mi_28")
					{
						if (abs(localdata.v) < 60)
						{
							g = interpolation(0, -60, 50, -14, 60, -6, abs(localdata.v));
						}
						else
						{
							g = interpolation(60, -6, 70, -3, 80, 0, abs(localdata.v));
						}
					}
					else if (helicopter.modelName == "mi_8_amtsh" || helicopter.modelName == "mi_8_mtv5")
					{
						g = interpolation(0, 0, 42, 0.125, 70, 1, abs(localdata.v));
					}
					else if (helicopter.modelName == "ka_29")
					{
						g = (69.4 - abs(localdata.v)) * (-0.86);
					}
					else if (helicopter.modelName == "ka_27")
					{
						g = (69.4 - abs(localdata.v)) * (-0.86);
					}
					else if (helicopter.modelName == "mi_26")
					{
						g = abs(localdata.v) * 0.428 - 36;
					}
					else
					{
						g = -60;
					}
					vadd->gain = pow(10, g * 0.05);

					vadd->play(localdata.v != 0, "NULL", helicopter.fullName["pinkNoise"], "NULL", helicopter.vadd);//Воспроизводим звук - записываем состояние звука в play
					if (localdata.v == 0)//Условие удаления объекта
					{
						Free(vadd);//Удаляем объект
					}
				}
			}
			//Двигатель
			if (helicopter.engFactor)
			{
				if (localdata.p_eng1_hp & localdata.eng1_obor != 0)//Условие создания объекта
				{
					if (!engHp[0])//Если объект не создан 
					{
						engHp[0] = new Sound;//Создаем объект
					}
				}
				if (engHp[0])//Если объект создан - используем его
				{
					engHp[0]->channel[0] = 1;//L
					engHp[0]->channel[1] = 0;
					if (localdata.p_eng1_zap | localdata.p_eng1_ostanov)
					{
						engHp[0]->play(localdata.p_eng1_hp & localdata.eng1_obor != 0, "NULL", "NULL", "NULL", helicopter.engHpFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					else
						engHp[0]->play(localdata.p_eng1_hp & localdata.eng1_obor != 0, helicopter.fullName["eng_on_hp_w"], helicopter.fullName["eng_w_hp_w"], helicopter.fullName["eng_off_hp_w"], helicopter.engHpFactor);//Воспроизводим звук - записываем состояние звука в play
					if (engHp[0]->sourceStatus[0] != AL_PLAYING && !localdata.p_eng1_hp)//Условие удаления объекта
						Free(engHp[0]);//Удаляем объект
				}

				if (localdata.p_eng2_hp & localdata.eng2_obor != 0)//Условие создания объекта
				{
					if (!engHp[1])//Если объект не создан 
					{
						engHp[1] = new Sound;//Создаем объект
					}
				}
				if (engHp[1])//Если объект создан - используем его
				{
					engHp[1]->channel[0] = 0;
					engHp[1]->channel[1] = 1;//R
					if (localdata.p_eng2_zap | localdata.p_eng2_ostanov)
					{
						engHp[1]->play(localdata.p_eng2_hp & localdata.eng2_obor != 0, "NULL", "NULL", "NULL", helicopter.engHpFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					else
						engHp[1]->play(localdata.p_eng2_hp & localdata.eng2_obor != 0, helicopter.fullName["eng_on_hp_w"], helicopter.fullName["eng_w_hp_w"], helicopter.fullName["eng_off_hp_w"], helicopter.engHpFactor);//Воспроизводим звук - записываем состояние звука в play
					if (engHp[1]->sourceStatus[0] != AL_PLAYING && !localdata.p_eng2_hp)//Условие удаления объекта
						Free(engHp[1]);//Удаляем объект
				}

				if (localdata.p_eng1_pomp)//Условие создания объекта
				{
					if (!engPomp[0])//Если объект не создан 
					{
						engPomp[0] = new Sound;//Создаем объект
					}
				}
				if (engPomp[0])//Если объект создан - используем его
				{
					engPomp[0]->channel[0] = 1;//L
					engPomp[0]->channel[1] = 0;
					engPomp[0]->play(localdata.p_eng1_pomp, helicopter.fullName["eng_pomp_on"], helicopter.fullName["eng_pomp_w"], "NULL", helicopter.engFactor);//Воспроизводим звук - записываем состояние звука в play
					if (engPomp[0]->sourceStatus[0] != AL_PLAYING && !localdata.p_eng1_pomp)//Условие удаления объекта
						Free(engPomp[0]);//Удаляем объект
				}

				if (localdata.p_eng2_pomp)//Условие создания объекта
				{
					if (!engPomp[1])//Если объект не создан 
					{
						engPomp[1] = new Sound;//Создаем объект
					}
				}
				if (engPomp[1])//Если объект создан - используем его
				{
					engPomp[1]->channel[0] = 0;
					engPomp[1]->channel[1] = 1;//R
					engPomp[1]->play(localdata.p_eng2_pomp, helicopter.fullName["eng_pomp_on"], helicopter.fullName["eng_pomp_w"], "NULL", helicopter.engFactor);//Воспроизводим звук - записываем состояние звука в play
					if (engPomp[1]->sourceStatus[0] != AL_PLAYING && !localdata.p_eng2_pomp)//Условие удаления объекта
						Free(engPomp[1]);//Удаляем объект
				}

				if (localdata.p_eng1_zap | localdata.eng1_obor > 0)
				{
					if (!eng[0])//Если объект не создан 
					{
						eng[0] = new Engine;//Создаем объект
					}
				}
				if (eng[0])//Если объект создан - используем его
				{
					eng[0]->channel[0] = 1;//magic numbers//1//L
					eng[0]->channel[1] = 1;//-1
					eng[0]->play(localdata.p_eng1_zap, localdata.p_eng1_ostanov, localdata.eng1_obor, helicopter);


					if (localdata.eng1_obor == 0)//Условие удаления объекта
						Free(eng[0]);//Удаляем объект
				}

				if (localdata.p_eng2_zap | localdata.eng2_obor > 0)
				{
					if (!eng[1])//Если объект не создан 
					{
						eng[1] = new Engine;//Создаем объект
					}
				}
				if (eng[1])//Если объект создан - используем его
				{
					eng[1]->channel[0] = 1;//magic numbers//0
					eng[1]->channel[1] = 1;//2//R
					eng[1]->play(localdata.p_eng2_zap, localdata.p_eng2_ostanov, localdata.eng2_obor, helicopter);


					if (localdata.eng2_obor == 0)//Условие удаления объекта
						Free(eng[1]);//Удаляем объект
				}
			}
			//Перекрывной кран ВСУ
			if (helicopter.vsuCraneFactor)
			{
				if (localdata.rez_1)//Условие создания объекта
					if (!vsuKran)//Если объект не создан 
						vsuKran = new Sound;//Создаем объект
				if (vsuKran)//Если объект создан - используем его
				{
					if (helicopter.modelName == "ka_27" | helicopter.modelName == "ka_29")
					{
						vsuKran->play(localdata.rez_1, helicopter.fullName["vsu_kran_on"], "NULL", "NULL", helicopter.vsuCraneFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					else
					{
						vsuKran->play(localdata.rez_1, helicopter.fullName["vsu_kran_on"], helicopter.fullName["vsu_kran_w"], "NULL", helicopter.vsuCraneFactor);//Воспроизводим звук - записываем состояние звука в play
					}
					if (vsuKran->sourceStatus[0] != AL_PLAYING && !localdata.rez_1)//Условие удаления объекта
						Free(vsuKran);//Удаляем объект
				}
			}
			//Зуммер
			if (helicopter.buzzerFactor)
			{
				if (localdata.rez_7)//Условие создания объекта
					if (!beep)//Если объект не создан 
						beep = new Sound;//Создаем объект
				if (beep)//Если объект создан - используем его
				{
					beep->play(localdata.rez_7, "NULL", helicopter.fullName["beep"], "NULL", helicopter.buzzerFactor);//Воспроизводим звук - записываем состояние звука в play
					if (beep->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
						Free(beep);//Удаляем объект
				}
			}
			//Неопределенный 1
			if (helicopter.undefinedFactor)
			{
				if (helicopter.modelName == "mi_28")
				{
					if (localdata.rez_6)//Условие создания объекта
						if (!undefined1)//Если объект не создан 
							undefined1 = new Sound;//Создаем объект
					if (undefined1)//Если объект создан - используем его
					{
						undefined1->play(localdata.rez_6, "NULL", helicopter.fullName["undefined1_w"], "NULL", helicopter.undefinedFactor);//Воспроизводим звук - записываем состояние звука в play
						if (undefined1->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
							Free(undefined1);//Удаляем объект
					}
				}
				else if (helicopter.modelName == "ka_29")
				{
					if (localdata.rez_6)//Условие создания объекта
						if (!undefined1)//Если объект не создан 
							undefined1 = new Sound;//Создаем объект
					if (undefined1)//Если объект создан - используем его
					{
						undefined1->play(localdata.rez_6, helicopter.fullName["undefined1_on"], helicopter.fullName["undefined1_w"], "NULL", helicopter.undefinedFactor);//Воспроизводим звук - записываем состояние звука в play
						if (undefined1->sourceStatus[0] != AL_PLAYING)//Условие удаления объекта
							Free(undefined1);//Удаляем объект
					}
				}
			}
		}
		else
		{
			if (!Sound::vectorHigh.empty())
			{
				Sound::vectorHigh.clear();
			}
			if (!Sound::vectorVy.empty())
			{
				Sound::vectorVy.clear();
			}
			if (!Sound::vectorVx.empty())
			{
				Sound::vectorVx.clear();
			}
			if (!Sound::vectorAcc.empty())
			{
				Sound::vectorAcc.clear();
			}
			if (!Sound::vectorStep.empty())
			{
				Sound::vectorStep.clear();
			}
			if (!Sound::vectorTime.empty())
			{
				Sound::vectorTime.clear();
			}
			if (!Sound::vectorRedTurn.empty())
			{
				Sound::vectorRedTurn.clear();
			}
			if (!Sound::vectorAvrAtk.empty())
			{
				Sound::vectorAvrAtk.clear();
			}
			if (!Sound::vectorAvrEng1Turn.empty())
			{
				Sound::vectorAvrEng1Turn.clear();
			}
			if (!Sound::vectorAvrEng2Turn.empty())
			{
				Sound::vectorAvrEng2Turn.clear();
			}
			if (!Sound::vectorAvrRedTurn.empty())
			{
				Sound::vectorAvrRedTurn.clear();
			}
			if (!Sound::vectorAvrStep.empty())
			{
				Sound::vectorAvrStep.clear();
			}

			if (eng[0])
			{
				Free(eng[0]);
			}
			if (eng[1])
			{
				Free(eng[1]);
			}
			if (red)
			{
				Free(red);
			}
			if (vintFlap)
			{
				Free(vintFlap);
			}
			timerAvr = 0;
			periodCalc = 0;
			Sound::vectorTime.push_back(Sound::currentTime);
			Sound::currentTime = localdata.time;
		}
	}
	atexit(freeOpenAL);//Выполнение функции KillALData на завершении программы
	StopRealTime();
	return 0;
}

double interpolation(double x0, double fx0, double x1, double fx1, double x)
{
	if (x0<x1 && x>x1)
	{
		return fx1;
	}
	if (x0 < x1 && x < x0)
	{
		return fx0;
	}
	if (x0 > x1 && x < x1)
	{
		return fx1;
	}
	if (x0 > x1 && x > x0)
	{
		return fx0;
	}

	return	fx0 + ((fx1 - fx0) / (x1 - x0))*(x - x0);
}

double interpolation(double x0, double fx0, double x1, double fx1, double x2, double fx2, double x)
{
	if (x0<x2 && x>x2)
	{
		return fx2;
	}
	if (x0 < x2 && x < x0)
	{
		return fx0;
	}
	if (x0 > x2 && x < x2)
	{
		return fx2;
	}
	if (x0 > x2 && x > x0)
	{
		return fx0;
	}

	//если квадратичная интерполяция не работает - берем линейную
	if (x1 == x0 | x2 == x1)
	{
		return	interpolation(x0, fx0, x1, fx1, x);
	}
	else
	{
		double fx, a0, a1, a2;
		a2 = ((fx2 - fx0) / ((x2 - x0)*(x2 - x1))) - ((fx1 - fx0) / ((x1 - x0)*(x2 - x1)));
		a1 = ((fx1 - fx0) / (x1 - x0)) - (a2*(x1 + x0));
		a0 = fx0 - a1 * x0 - a2 * x0*x0;
		return fx = a0 + a1 * x + a2*x*x;

	}

}

double getPitch(double offset, string filename, double parameter)
{
	double turn = 0;
	double t = 0;
	double v = 0;
	vector <double> time, value;

	//данные в базе должны храниться в строках парами, по паре в каждой строке (не больше)
	string str;
	ifstream base(filename);
	while (!base.eof())
	{
		getline(base, str);
		sscanf(str.c_str(), "%lf %lf", &t, &v);
		time.push_back(t);
		value.push_back(v);
	}
	base.close();

	double x, x0, x1, x2, fx, fx0, fx1, fx2, a0, a1, a2;
	int n = time.size();

	for (int i = 0; i < n; i++)
	{
		if (offset < time[0])
		{
			turn = value[0];//достаем обороты из базы
			break;
		}
		if (offset == time[i])//реальная отметка времени совпала с отметкой из бд
		{
			turn = value[i];//достаем обороты из базы
			break;
		}
		if (offset > time[n - 1])//отметка не совпала с базой
		{
			turn = value[n - 1];//достаем обороты из базы
			break;
		}
		if (offset > time[i] && offset < time[i + 1])//отметка не совпала с базой
		{

			//квадратичная интерполяция
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
			//если квадратичная интерполяция не работает - берем линейную
			if (x1 == x0 | x2 == x1)
			{
				turn = interpolation(x0, fx0, x1, fx1, x);
			}
			else
			{
				turn = interpolation(x0, fx0, x1, fx1, x2, fx2, x);
			}
		}
	}
	double new_pitch = 0;
	if (turn <= 0)
	{
		new_pitch = 1;
	}
	else
	{
		new_pitch = parameter / turn;	//вычисляем результирующий Pitch на основе отношениz настоящего уровня оборотов к базовому (получен при записи аудио-файла)
	}
	return new_pitch;
}

double getOffset(double pitch, string filename, double parameter)
{
	//данные в базе должны храниться в строках парами, по паре в каждой строке (не больше)
	double t = 0;
	double v = 0;
	vector <double> time, value;
	string str;
	ifstream base(filename);
	while (!base.eof())
	{
		getline(base, str);
		sscanf(str.c_str(), "%lf %lf", &t, &v);
		time.push_back(t);
		value.push_back(v);
	}
	base.close();
	int n = time.size();

	double turn = 0;
	if (parameter < 0)
		turn = 0;
	else
		turn = parameter / pitch;

	double x, x0, x1, x2, fx, fx0, fx1, fx2, a0, a1, a2;
	double newOffset = 0;

	if (value[0] <= value[n - 1])
	{
		for (int i = 0; i < n; i++)
		{
			if (turn < value[i] && i == 0)
			{
				newOffset = time[i];//достаем обороты из базы
				break;
			}
			if (turn == value[i])//реальная отметка времени совпала с отметкой из бд
			{
				newOffset = time[i];//достаем обороты из базы
				break;
			}
			if (turn > value[i] && i == n - 1)//отметка не совпала с базой
			{
				newOffset = time[i];//достаем обороты из базы
				break;
			}
			if (turn > value[i] && turn < value[i + 1])//отметка не совпала с базой
			{

				//квадратичная интерполяция
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

				newOffset = interpolation(x0, fx0, x1, fx1, x2, fx2, x);
			}

		}

	}
	else
	{

		for (int i = 0; i < n; i++)
		{
			if (turn > value[0])
			{
				newOffset = time[0];//достаем обороты из базы
				break;
			}
			if (turn == value[i])//реальная отметка времени совпала с отметкой из бд
			{
				newOffset = time[i];//достаем обороты из базы
				break;
			}
			if (turn < value[n - 1])//отметка не совпала с базой
			{
				newOffset = time[n - 1];//достаем обороты из базы
				break;
			}
			if (turn < value[i] && turn > value[i + 1])//отметка не совпала с базой
			{

				//квадратичная интерполяция
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

				newOffset = interpolation(x0, fx0, x1, fx1, x2, fx2, x);
			}

		}
	}

	if (newOffset <= 0)
		newOffset = 0;

	return newOffset;

}

int crossFade(double *gf, double *gr, double parameter, double limit1, double limit2, double mult)//source - fade , source2 - rise
{
	//limit1 - значение параметра в начале кросс-фэйда
	//limit2 - значение параметра в конце
	//parameter - итератор кроссфейда - должен стремится от limit1 к limit2
	//source - источник, громкость которого убывает
	//source2 - источник, громкость которого нарастает
	//double gf, gr;

	if ((limit2 > limit1 && parameter < limit1) || (limit2 < limit1 && parameter > limit1))
	{
		*gf = 1 * mult;
		*gr = 0;
	}
	if ((limit2 > limit1 && parameter > limit2) || (limit2 < limit1 && parameter < limit2))
	{
		*gf = 0;
		*gr = 1 * mult;
	}
	if ((limit2 > limit1 && parameter <= limit2 && parameter >= limit1) || (limit2 < limit1 && parameter >= limit2 && parameter <= limit1))
	{
		*gf = interpolation(limit1, mult, abs(limit2 - limit1) / 2, 0.5*mult, limit2, 0, parameter);//TYPE A
		*gr = interpolation(limit1, 0, abs(limit2 - limit1) / 2, 0.5*mult, limit2, mult, parameter);
		//double x = interpolation(limit1, 0, abs(limit2 - limit1) / 2, 0.5, limit2, 1, parameter);//TYPE B
		//*gf = cos(x * 0.5 * M_PI);
		//*gr = cos((1.0 - x) * 0.5 * M_PI);
		//*gf = interpolation(limit1, 0, abs(limit2 - limit1) / 2, -6, limit2, -40, parameter);//TYPE C
		//*gr = interpolation(limit1, -40, abs(limit2 - limit1) / 2, -6, limit2, 0, parameter);
		//*gf = pow(10, *gf*0.05) * mult;
		//*gr = pow(10, *gr*0.05) * mult;
		*gf = (*gf < 0) ? 0 : *gf;
		*gr = (*gf < 0) ? 0 : *gr;
		*gf = (*gf > 1) ? 1 : *gf;
		*gr = (*gf > 1) ? 1 : *gr;
	}

	//alSourcef(source, AL_GAIN, gf);
	//alSourcef(source2, AL_GAIN, gr);

	if (limit2 > limit1 && parameter > limit2)
		return 1;//возвращаем 1 когда кроссфэйд закончен

	if (limit2 < limit1 && parameter < limit2)
		return 1;

	return 0;//возвращаем 0 когда кроссфэйд не закончен
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
	//Определяем коррекцию (для отладки)
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

	device = alcOpenDevice(0);
	if (device == 0)
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		return AL_FALSE;
	}

	context = alcCreateContext(device, 0);
	if (context == 0)
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

	int maxmono = 0;
	int maxstereo = 0;
	for (size_t i = 0; i < size; ++i)
	{
		/*if (attrs[i] == ALC_SYNC)
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
		}*/
		if (attrs[i] == ALC_MONO_SOURCES)
		{
			//std::cout << " A hint indicating how many sources should be capable of supporting mono data: " << attrs[i + 1] << std::endl;
			maxmono = attrs[i + 1];
		}
		if (attrs[i] == ALC_STEREO_SOURCES)
		{
			//	std::cout << " A hint indicating how many sources should be capable of supporting stereo data: " << attrs[i + 1] << std::endl;
			maxstereo = attrs[i + 1];
		}
	}
	alcDestroyContext(context);
	alcCloseDevice(device);
	return maxmono + maxstereo;
}

int Sound::play(bool status, string path_on, string path_w, string path_off, double gain_mult)
{
	bool start;
	bool work;
	bool end;
	bool free;

	//Заполняем все 3 буфера файлами, сразу
	if (load != "set")
	{
		if (path_on != "NULL")
			if (!setBuffer(buffer[0], path_on, channelsSetup, channel))
				return 0;
		if (path_w != "NULL")
			if (!setBuffer(buffer[1], path_w, channelsSetup, channel))
				return 0;
		if (path_off != "NULL")
			if (!setBuffer(buffer[2], path_off, channelsSetup, channel))
				return 0;
		load = "set";
	}
	//Узнаем длинну файлов запуска и остановки
	if (path_on != "NULL")
		lengthOn = getLengthWAV(path_on);
	if (path_off != "NULL")
		lengthOff = getLengthWAV(path_off);
	alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);
	//условие запуска когда все звуки присутствуют
	if (path_on != "NULL" & path_w != "NULL" & path_off != "NULL")
	{
		start = status & !soundOn & !soundWork;
		work = status & soundOn & !soundWork & sourceStatus[0] != AL_PLAYING;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[0] != AL_PLAYING;
	}
	//условие запуска когда отсутствует остановка
	if (path_on != "NULL" & path_w != "NULL" & path_off == "NULL")
	{
		start = status & !soundOn & !soundWork;
		work = status & soundOn & !soundWork & sourceStatus[0] != AL_PLAYING;
		end = 0;
		free = !status;
	}
	//условие запуска когда отсутствует работа
	if (path_on != "NULL" & path_w == "NULL" & path_off != "NULL")
	{
		start = status & !soundOn;
		work = 0;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[0] != AL_PLAYING;
	}
	//условие запуска когда отсутствует запуск
	if (path_on == "NULL" & path_w != "NULL" & path_off != "NULL")
	{
		start = 0;
		work = status & !soundWork;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[0] != AL_PLAYING;
	}
	//условие запуска когда отсутствует запуск и работа
	if (path_on == "NULL" & path_w == "NULL" & path_off != "NULL")
	{
		start = 0;
		work = 0;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[0] != AL_PLAYING;
	}
	//условие запуска когда отсутствует запуск и выключение
	if (path_on == "NULL" & path_w != "NULL" & path_off == "NULL")
	{
		start = 0;
		work = status & !soundWork;
		end = 0;
		free = !status;
	}
	//условие запуска когда отсутствует работа и выключение
	if (path_on != "NULL" & path_w == "NULL" & path_off == "NULL")
	{
		start = status & !soundOn;
		work = 0;
		end = 0;
		free = !status & sourceStatus[0] != AL_PLAYING;
	}
	//все 0
	if (path_on == "NULL" & path_w == "NULL" & path_off == "NULL")
	{
		start = 0;
		work = 0;
		end = 0;
		free = 1;
	}
	//Включение (если path_o указывает на пустую область -> у агрегата отсутствует звук запуска)
	if (start)
	{
		soundOn = 1;
		soundWork = 0;
		soundOff = 0;

		sourceStatus[0] = switchBufferAndPlay(&buffer[0], &source[0], offsetOn);
		alSourcei(source[0], AL_LOOPING, AL_FALSE);
	}
	//Работа (если path_w указывает на пустую область -> у агрегата отсутствует звук режима работы)
	if (work)
	{
		soundOn = 0;
		soundWork = 1;
		soundOff = 0;

		sourceStatus[0] = switchBufferAndPlay(&buffer[1], &source[0], 0);
		alSourcei(source[0], AL_LOOPING, AL_TRUE);
	}
	//Выключение (если path_off указывает на пустую область -> у агрегата отсутствует звук выключения)
	if (end)
	{
		soundOn = 0;
		soundWork = 0;
		soundOff = 1;

		sourceStatus[0] = switchBufferAndPlay(&buffer[2], &source[0], offsetOff);
		alSourcei(source[0], AL_LOOPING, AL_FALSE);
	}
	//Освобождение памяти
	if (free)
	{
		soundOn = 0;
		soundWork = 0;
		soundOff = 0;
		alSourceStop(source[0]);
	}

	alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);

	//Обновляем параметры по ходу воспроизведения
	if (sourceStatus[0] == AL_PLAYING)
	{
		alSourcef(source[0], AL_PITCH, pitch);
		alSourcef(source[0], AL_GAIN, gain*gain_mult*masterGain);
		//Пока идет запуск - высчитываем точку остановки
		if (soundOn)
		{
			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
			offsetOff = lengthOff * (1 - (offsetOn / lengthOn));
		}
		//Пока идет остановка - высчитываем точку запуска
		if (soundOff)
		{
			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);
			offsetOn = lengthOn * (1 - (offsetOff / lengthOff));
		}
	}
	return sourceStatus[0];
}

int Sound::setBuffer(ALuint Buffer, string path, AL_SOUND_CHANNELS channelsCount, vector<double>channels)
{
	int format;
	int size;
	int freq;
	void *iData;
	void *rData;
	int rSize;
	int bitsPerSample = 0;
	ALboolean loop;
	FILE *check;

	check = fopen(path.c_str(), "r");
	if (!check)
	{
		cout << "\n file [" << path << "] is missing\t\t\t\t\t\t\r" << endl;
		return 0;
	}
	fclose(check);
	alutLoadWAVFile((ALbyte*)path.c_str(), &format, &iData, &size, &freq, &loop);

	//Если файл стерео - просто загружаем буфер
	if (format == AL_FORMAT_STEREO8 || format == AL_FORMAT_STEREO16)
	{
		alBufferData(Buffer, format, iData, size, freq);
		alutUnloadWAV(format, iData, size, freq);
		return 1;
	}
	//Если файл моно - загружаем буфер в соответствии с конфигурацией динамиков
	if (channelsCount != 0)//
	{
		if (format == AL_FORMAT_MONO8)//8бит
		{
			bitsPerSample = 8;
			unsigned char *monodata0 = ((unsigned char*)iData);
			mono2channels(monodata0, size, channelsCount, channels.data(), &rData, &rSize);
		}
		if (format == AL_FORMAT_MONO16)//16бит
		{
			bitsPerSample = 16;
			short *monodata1 = ((short*)iData);
			mono2channels(monodata1, size, channelsCount, channels.data(), &rData, &rSize);
		}
		if (format != AL_FORMAT_MONO16 && format != AL_FORMAT_MONO8)//неподдерживаемый формат
			return 0;

		format = getFormat(channelsCount, bitsPerSample);
		alBufferData(Buffer, format, rData, rSize, freq);
		free(rData);
	}
	//Если конфигурация с 1им динамиком
	else
	{
		alBufferData(Buffer, format, iData, size, freq);
	}
	//Высвобождаем память
	alutUnloadWAV(format, iData, size, freq);
	return 1;
}

int Sound::setAndDeploySound(ALuint *Buffer, ALuint *Source, double offset, string file_path)
{
	int play = 0;
	alSourceStop(*Source);
	alSourcei(*Source, AL_BUFFER, NULL);
	alDeleteBuffers(1, &*Buffer);
	alGenBuffers(1, &*Buffer);
	if (!setBuffer(*Buffer, file_path, channelsSetup, channel))
		return 0;
	alSourcei(*Source, AL_BUFFER, *Buffer);
	alSourcef(*Source, AL_SEC_OFFSET, offset);
	alSourcePlay(*Source);
	alGetSourcei(*Source, AL_SOURCE_STATE, &play);
	return play;
}

int Sound::switchBufferAndPlay(ALuint *Buffer, ALuint *Source, double offset)
{
	int play = 0;
	alSourceStop(*Source);
	alSourcei(*Source, AL_BUFFER, NULL);
	alSourcei(*Source, AL_BUFFER, *Buffer);
	alSourcef(*Source, AL_SEC_OFFSET, offset);
	alSourcePlay(*Source);
	alGetSourcei(*Source, AL_SOURCE_STATE, &play);
	return play;
}

double Sound::getAverange(string parameter, double seconds)
{
	double averange = 0;
	if (parameter == "step")
	{
		int size = vectorAvrStep.size();
		double window = size / globalWindow * seconds;
		for (size_t i = size - window; i < size; i++)
		{
			averange += vectorAvrStep[i] / window;
		}
	}
	else if (parameter == "eng1Turns")
	{
		int size = vectorAvrEng1Turn.size();
		double window = size / globalWindow * seconds;
		for (size_t i = size - window; i < size; i++)
		{
			averange += vectorAvrEng1Turn[i] / window;
		}
	}
	else if (parameter == "eng2Turns")
	{
		int size = vectorAvrEng1Turn.size();
		double window = size / globalWindow * seconds;
		for (size_t i = size - window; i < size; i++)
		{
			averange += vectorAvrEng2Turn[i] / window;
		}
	}
	else if (parameter == "redTurns")
	{
		int size = vectorAvrRedTurn.size();
		double window = size / globalWindow * seconds;
		for (size_t i = size - window; i < size; i++)
		{
			averange += vectorAvrRedTurn[i] / window;
		}
	}
	else if (parameter == "attack")
	{
		int size = vectorAvrAtk.size();
		double window = size / globalWindow * seconds;
		for (size_t i = size - window; i < size; i++)
		{
			averange += vectorAvrAtk[i] / window;
		}
	}
	else
	{
		return 0;
	}

	return averange;
}

Reductor::Reductor() : Sound(3, 3, 3)
{
	//Не самый умный ход менять конструктор по 
	//наибольшему количеству требуемых источников
	//при условии что редуктор просто вызывает конструктор Sound
	//можно писать так if(modelName == mi_26){reductor = new Sound(4,4,2)}else{reductor = new Sound(3,3,2)}
}

int Reductor::play(Helicopter h, SOUNDREAD sr)
{
	for (size_t i = 0; i < 2; i++)
	{
		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);
		//При оборотах редуктора = 0 и незапущенных двигателях, останавиваем источники
		if (sr.reduktor_gl_obor == 0 && !sr.p_eng1_zap && !sr.p_eng2_zap)
		{
			alSourceStop(source[i]);
			alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);		// перезабили признак 
			fileBuffered[i] = filetoBuffer[i] = "NULL";
		}
		//Подключаем эквалайзер
		if (eq[i] != "set")
		{
			alEffecti(effect[i], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
			alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
			alFilteri(filter[i], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[i], AL_LOWPASS_GAIN, 0);
			alSource3i(source[i], AL_AUXILIARY_SEND_FILTER, effectSlot[i], 0, NULL);
			alSourcei(source[i], AL_DIRECT_FILTER, filter[i]);
			eq[i] = "set";
		}
		//Загружаем буферы и запускам источники
		if (fileBuffered[i] != filetoBuffer[i])
		{
			setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}
	}
	//РЕДУКТОР АНСАТА
	if (h.modelName == "ansat")
	{
		//0 -> мг
		if (sr.reduktor_gl_obor <= h.redTurnoverMg1 && (sr.p_eng1_zap | sr.p_eng2_zap))
		{
			filetoBuffer[0] = h.fullName["red_on_w"];
			filetoBuffer[1] = h.fullName["red_w_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_FALSE);
			offset[0] = getOffset(1, h.fullName["red_on"], sr.reduktor_gl_obor);
			offset[1] = 0;

			double fade = 0;
			double rise = 0;
			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);

			//Если запуск проигрывается - плавно переходим на цикл
			if (sourceStatus[0] == AL_PLAYING)
			{
				//Оцениваем выходную громкость источников по параметрам оборотов и громкости,
				//чтобы обеспечить нормальное звучание, если обороты по какимто причинам не вышли на малый газ,
				//до окончания проигрывания записи запуска
				double fadeTurn = 0;
				double fadeTime = 0;
				double riseTurn = 0;
				double riseTime = 0;
				lengthOn = getLengthWAV(h.fullName["red_on_w"]);
				lengthOff = getLengthWAV(h.fullName["red_w_w"]);
				crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1 - 10, h.redTurnoverMg1, masterGain*h.redFactor);
				if (offsetOn != 0)
					crossFade(&fadeTime, &riseTime, offsetOn, lengthOn - lengthOff, lengthOn, masterGain*h.redFactor);
				fade = (fadeTurn < fadeTime) ? fadeTurn : fadeTime;
				rise = 1 - fade;
			}
			//Иначе используем цикл
			else
			{
				fade = 0;
				rise = 1;
			}
			alSourcef(source[0], AL_GAIN, fade);//0
			alSourcef(source[1], AL_GAIN, rise);//1


			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
			if (sr.reduktor_gl_obor >= 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["red_on"], sr.reduktor_gl_obor));
			else
				alSourcef(source[0], AL_PITCH, 1);
			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//меняем pitch (дает нисходящую прямую при остановке второго дв)
		}
		//мг <-> авт
		if (sr.reduktor_gl_obor > h.redTurnoverMg2)
		{
			filetoBuffer[1] = h.fullName["red_w_w"];
			filetoBuffer[0] = h.fullName["red_w_avt_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			offset[0] = 0;
			offset[1] = 0;

			double fade, rise;
			crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg2, h.redTurnoverAvt, masterGain*h.redFactor);
			alSourcef(source[1], AL_GAIN, fade);//
			alSourcef(source[0], AL_GAIN, rise);//
			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);//меняем pitch (дает нисходящую прямую при остановке второго дв)
			alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//меняем pitch (дает нисходящую прямую при остановке второго дв)
		}
		//мг -> 0
		if (!sr.p_eng1_zap && !sr.p_eng2_zap && sr.reduktor_gl_obor > 0 && sr.reduktor_gl_obor < h.redTurnoverMg1)
		{
			filetoBuffer[0] = h.fullName["red_off_w"];
			filetoBuffer[1] = h.fullName["red_w_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_FALSE);
			offset[0] = getOffset(1, h.fullName["red_off"], sr.reduktor_gl_obor);
			offset[1] = 0;

			double fade, rise;
			crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1, h.redTurnoverMg1 - 10, masterGain*h.redFactor);
			alSourcef(source[1], AL_GAIN, fade);//
			alSourcef(source[0], AL_GAIN, rise);//

			alSourcei(source[0], AL_LOOPING, AL_FALSE);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);

			if (sr.reduktor_gl_obor > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["red_off"], sr.reduktor_gl_obor));
			else
				alSourcef(source[0], AL_PITCH, 1);
		}
	}
	else
	{
		//0 -> мг 1дв
		if (sr.reduktor_gl_obor <= h.redTurnoverMg1 && (sr.p_eng1_zap | sr.p_eng2_zap))
		{
			filetoBuffer[0] = h.fullName["red_on_w"];
			filetoBuffer[1] = h.fullName["red_w_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_FALSE);
			offset[0] = getOffset(1, h.fullName["red_on"], sr.reduktor_gl_obor);
			offset[1] = 0;

			double fade = 0;
			double rise = 0;
			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);

			//Если запуск проигрывается - плавно переходим на цикл
			if (sourceStatus[0] == AL_PLAYING)
			{
				//Оцениваем выходную громкость источников по параметрам оборотов и громкости,
				//чтобы обеспечить нормальное звучание, если обороты по какимто причинам не вышли на малый газ,
				//до окончания проигрывания записи запуска
				double fadeTurn = 0;
				double fadeTime = 0;
				double riseTurn = 0;
				double riseTime = 0;
				lengthOn = getLengthWAV(h.fullName["red_on_w"]);
				lengthOff = getLengthWAV(h.fullName["red_w_w"]);
				crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1 - 10, h.redTurnoverMg1, masterGain*h.redFactor);
				if (offsetOn != 0)
					crossFade(&fadeTime, &riseTime, offsetOn, lengthOn - lengthOff, lengthOn, masterGain*h.redFactor);
				fade = (fadeTurn < fadeTime) ? fadeTurn : fadeTime;
				rise = 1 - fade;
			}
			//Иначе используем цикл
			else
			{
				fade = 0;
				rise = 1;
			}

			alSourcef(source[0], AL_GAIN, fade);//0
			alSourcef(source[1], AL_GAIN, rise);//1

			if (sr.reduktor_gl_obor >= 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["red_on"], sr.reduktor_gl_obor));
			else
				alSourcef(source[0], AL_PITCH, 1);
			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//меняем pitch (дает нисходящую прямую при остановке второго дв)
		}
		//мг1дв <-> мг2дв
		if (sr.reduktor_gl_obor > h.redTurnoverMg1 && sr.reduktor_gl_obor <= h.redTurnoverMg2)
		{
			filetoBuffer[1] = h.fullName["red_w_w"];
			filetoBuffer[0] = h.fullName["red_w_mg_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			offset[0] = 0;
			offset[1] = 0;

			double fade, rise;
			crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1, h.redTurnoverMg2, masterGain*h.redFactor);
			alSourcef(source[1], AL_GAIN, fade);//0
			alSourcef(source[0], AL_GAIN, rise);//1

			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);
			alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);//меняем pitch (дает нисходящую прямую при остановке второго дв)
		}
		//мг2дв <-> авт
		if (h.modelName == "mi_28")
		{
			if (sr.reduktor_gl_obor > h.redTurnoverMg2 && sr.reduktor_gl_obor <= h.redTurnoverAvt - 2)
			{
				filetoBuffer[1] = h.fullName["red_w_avt_w"];
				filetoBuffer[0] = h.fullName["red_w_mg_w"];
				alSourcei(source[1], AL_LOOPING, AL_TRUE);
				alSourcei(source[0], AL_LOOPING, AL_TRUE);
				offset[0] = 0;
				offset[1] = 0;

				double fade, rise;
				crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg2, h.redTurnoverAvt, masterGain*h.redFactor);
				alSourcef(source[0], AL_GAIN, fade);//
				alSourcef(source[1], AL_GAIN, rise);//

				alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);
				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//меняем pitch (дает нисходящую прямую при остановке второго дв)
			}
			//авт <-> авт полет
			if (sr.reduktor_gl_obor > h.redTurnoverAvt - 2)
			{
				filetoBuffer[1] = h.fullName["red_w_avt_w"];
				filetoBuffer[0] = h.fullName["red_w_avt_fly"];
				alSourcei(source[1], AL_LOOPING, AL_TRUE);
				alSourcei(source[0], AL_LOOPING, AL_TRUE);
				offset[0] = 0;
				offset[1] = 0;

				double fade = 0, rise = 0;
				crossFade(&fade, &rise, step, 5.00, 5.01, masterGain*h.redFactor);
				alSourcef(source[1], AL_GAIN, fade);//
				alSourcef(source[0], AL_GAIN, rise);//

				alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);
				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//меняем pitch (дает нисходящую прямую при остановке второго дв)

			}
		}
		else
		{
			if (sr.reduktor_gl_obor > h.redTurnoverMg2)
			{
				filetoBuffer[1] = h.fullName["red_w_avt_w"];
				filetoBuffer[0] = h.fullName["red_w_mg_w"];
				alSourcei(source[1], AL_LOOPING, AL_TRUE);
				alSourcei(source[0], AL_LOOPING, AL_TRUE);
				offset[0] = 0;
				offset[1] = 0;

				double fade, rise;
				crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg2, h.redTurnoverAvt, masterGain*h.redFactor);
				alSourcef(source[0], AL_GAIN, fade);//
				alSourcef(source[1], AL_GAIN, rise);//

				alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);
				alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//меняем pitch (дает нисходящую прямую при остановке второго дв)
			}
		}
		//мг1дв -> 0
		if (!sr.p_eng1_zap && !sr.p_eng2_zap && sr.reduktor_gl_obor > 0 && sr.reduktor_gl_obor < h.redTurnoverMg1)
		{
			filetoBuffer[0] = h.fullName["red_off_w"];
			filetoBuffer[1] = h.fullName["red_w_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_FALSE);
			offset[0] = getOffset(1, h.fullName["red_off"], sr.reduktor_gl_obor);
			offset[1] = 0;

			double fade, rise;
			crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1, h.redTurnoverMg1 - 10, masterGain*h.redFactor);
			alSourcef(source[1], AL_GAIN, fade);//
			alSourcef(source[0], AL_GAIN, rise);//

			alSourcei(source[0], AL_LOOPING, AL_FALSE);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);

			if (sr.reduktor_gl_obor > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["red_off"], sr.reduktor_gl_obor));
			else
				alSourcef(source[0], AL_PITCH, 1);
		}
	}

	double lowFreqGain = AL_EQUALIZER_DEFAULT_LOW_GAIN;
	double mid1FreqGain = AL_EQUALIZER_DEFAULT_MID1_GAIN;
	double mid2FreqGain = AL_EQUALIZER_DEFAULT_MID2_GAIN;
	double highFreqGain = AL_EQUALIZER_DEFAULT_HIGH_GAIN;

	double lowCutoffFreq = AL_EQUALIZER_DEFAULT_LOW_CUTOFF;//НЧ 50-800
	double mid1CutoffFreq = AL_EQUALIZER_DEFAULT_MID1_CENTER;//купол 1 200-3000
	double mid2CutoffFreq = AL_EQUALIZER_DEFAULT_MID2_CENTER;//купол 2 1000-8000
	double highCutoffFreq = AL_EQUALIZER_DEFAULT_HIGH_CUTOFF;//ВЧ 4000-16000

	//Полеты ми 28
	if (h.modelName == "mi_28")
	{
		//Вычисляем средние обороты за последние 30с
		double averangeTurn = getAverange("redTurns", 30 * interpolation(0, 0.01, h.redTurnoverAvt, 1, sr.reduktor_gl_obor));

		//Усиление по атаке
		double atkXvel = calcA * interpolation(0, 0, 16.67, 1, abs(velocityX));

		double averangeAtk = getAverange("attack", 20) * interpolation(0, 0, 16.67, 1, abs(velocityX));

		double atkGain = (atkXvel - averangeAtk) * -0.4;
		atkGain = (atkGain < -2) ? -2 : atkGain;
		atkGain = (atkGain > 3) ? 3 : atkGain;

		//Общее усиление от скорости 
		double velocityGain = 0;
		if (velocityX < 70)
		{
			velocityGain = interpolation(20, 0, 50, 2, 70, 5, velocityX) / 5;
		}
		else
		{
			velocityGain = (velocityX  * 0.15 - 5.5) / 5;
		}

		//Общее усиление от шага
		double stepGain = 0;
		if (step < 16)
		{
			stepGain = interpolation(0, 0, 11, 3, 16, 5, step) / 3;
		}
		else
		{
			stepGain = (step * 0.4 - 1.4) / 3;
		}

		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75;

		//усиление НЧ когда нет хлопков на границе 2го условия
		if (abs(velocityX) <= 16.67) //ниже 60ти висение
		{
			if (vectorVx.size() > 2)
			{
				if (vectorVx.at(vectorVx.size() - 2) > 16.67) //предыдущая точка была не на висении - не висение
				{
					hovering = 0;
				}
				else //предыдущая точка была на висениии - висение
				{
					if (abs(accelerationX) < 0.56) // условие висение возникает когда ускорение падает ниже условия
					{
						hovering = 1;
					}
				}
			}
		}
		else //выше 60ти не висение
		{
			hovering = 0;
		}

		//Если рывок слишком большой и бьет по ушам
		double flapCGain = 0;
		if (((velocityX < 0 && accelerationX > 0.56) || (velocityX > 0 && accelerationX < -0.56)) && abs(velocityX) <= 16.67 /*&& velocityY < 4*/)
		{
			flapCGain = ((abs(accelerationX) - 0.56) * 4) * interpolation(-0.25, 0, 0.5, 0.5, 0.25, 1, velocityY) * hovering;//переходит в усиление нч по vy
			flapCGain = (flapCGain > 4) ? 4 : flapCGain;
		}

		lowFreqGain = pow(10, (turnGain + velocityGain * 3 + atkGain + flapCGain)*0.05);
		mid1FreqGain = pow(10, (turnGain + stepGain * 2 + velocityGain * 5)*0.05);
		mid2FreqGain = pow(10, (turnGain + stepGain * 4)*0.05);
		highFreqGain = pow(10, (turnGain + stepGain * 4)*0.05);

		lowFreqGain = (lowFreqGain <= 1) ? 1 : lowFreqGain;
		mid1FreqGain = (mid1FreqGain <= 1) ? 1 : mid1FreqGain;
		mid2FreqGain = (mid2FreqGain <= 1) ? 1 : mid2FreqGain;
		highFreqGain = (highFreqGain <= 1) ? 1 : highFreqGain;

		lowCutoffFreq = 200;//НЧ 50-800
		mid1CutoffFreq = 1000;//купол 1 200-3000
		mid2CutoffFreq = 3000;//купол 2 1000-8000
		highCutoffFreq = 10000;//ВЧ 4000-16000
	}
	//Полеты 8 мтв5, 8 амтш
	if (h.modelName == "mi_8_amtsh" || h.modelName == "mi_8_mtv5")
	{
		//Вычисляем средние обороты за последние 30с
		double averangeTurn = getAverange("redTurns", 30 * interpolation(0, 0.01, h.redTurnoverAvt, 1, sr.reduktor_gl_obor));

		//усиление от оборотов выше 10000
		double highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1.5;
		highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;

		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75;

		//Общее усиление от скорости выше 28м/с
		double velocityGain = (abs(velocityX) >= 28) ? (abs(velocityX) - 28)* 0.1 : 0;//0.1дб на 1 м/с

		//Вычисляем средний шаг за 35с
		double averangeStep = getAverange("step", 35);

		//Усиление от шага
		double stepGain = 0.75 * (step - averangeStep) * interpolation(0, 0, 1, 1, high);//

		//усиление по шагу в НЧ
		double mid2FreqStepGain = step * 0.6 * interpolation(0, 1, 10, 0, high);

		//Висение
		//усиление НЧ когда нет хлопков на границе 2го условия
		if (abs(velocityX) <= 16.67) //ниже 60ти висение
		{
			if (vectorVx.size() > 2)
			{
				if (vectorVx.at(vectorVx.size() - 2) > 16.67) //предыдущая точка была не на висении - не висение
				{
					hovering = 0;
				}
				else //предыдущая точка была на висениии - висение
				{
					if (abs(accelerationX) < 0.56) // условие висение возникает когда ускорение падает ниже условия
					{
						hovering = 1;
					}
				}
			}
		}
		else //выше 60ти не висение
		{
			hovering = 0;
		}
		//Если рывок слишком большой и бьет по ушам
		double flapCGain = 0;
		if (((velocityX < 0 && accelerationX > 0.56) || (velocityX > 0 && accelerationX < -0.56)) && abs(velocityX) <= 16.67 /*&& velocityY < 4*/)
		{
			flapCGain = ((abs(accelerationX) - 0.56) * 4) * interpolation(-0.25, 0, 0.25, 1, velocityY) * hovering;//переходит в усиление нч по vy
			flapCGain = (flapCGain > 4) ? 4 : flapCGain;
		}

		//Страгивание
		//Усиление редуктора в НЧ в начале движения по ВПП
		double stalkingGain = (accelerationX > 0) ? accelerationX * 5 * interpolation(0, 1, 8.3, 0, velocityX) * !high : 0;

		lowFreqGain = pow(10, (mid2FreqStepGain + flapCGain + stalkingGain)*0.05);
		mid1FreqGain = pow(10, (turnGain + stepGain + velocityGain + mid2FreqStepGain + flapCGain + stalkingGain)*0.05);
		mid2FreqGain = pow(10, (turnGain + stepGain + velocityGain)*0.05);
		highFreqGain = pow(10, (turnGain + stepGain + velocityGain + highFreqTurnGain)*0.05);

		lowCutoffFreq = 250;//НЧ 50-800
		mid1CutoffFreq = 400;//купол 1 200-3000
		mid2CutoffFreq = 3000;//купол 2 1000-8000
		highCutoffFreq = 10000;//ВЧ 4000-16000
	}
	//Полеты ка 29
	if (h.modelName == "ka_29")
	{
		//Набираем массив для рассчета усиления от среднего значения оборотов редуктора за 30с
		double averangeTurn = getAverange("redTurns", 30 * interpolation(0, 0.01, h.redTurnoverAvt, 1, sr.reduktor_gl_obor));

		//Общее усиление от скорости выше 50м/с
		double velocityGain = (abs(velocityX) >= 50) ? (abs(velocityX) - 50)* 0.2 : 0;//0.1дб на 1 м/с

		//Набираем массив для рассчета усиления от среднего значения шага за 50с
		double averangeStep = getAverange("step", 50);

		//Рассчитываем усиление от среднего
		double stepGain = (step - averangeStep) * interpolation(0, 0, 1, 1, high);

		//усиление по шагу в НЧ
		double mid2FreqStepGain = step * interpolation(0, 1, 5, 0, high);

		//усиление по шагу в Средних чатотах
		double absStepGain = step * interpolation(0, 1, 10.5, 0.5, 27.78, 0, abs(velocityX));

		//усиление от оборотов выше 10000
		double highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1;
		highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;
		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75;

		//усиление НЧ когда нет хлопков на границе 2го условия
		if (abs(velocityX) <= 16.67) //ниже 60ти висение
		{
			if (vectorVx.size() > 2)
			{
				if (vectorVx.at(vectorVx.size() - 2) > 16.67) //предыдущая точка была не на висении - не висение
				{
					hovering = 0;
				}
				else //предыдущая точка была на висениии - висение
				{
					if (abs(accelerationX) < 0.56) // условие висение возникает когда ускорение падает ниже условия
					{
						hovering = 1;
					}
				}
			}
		}
		else //выше 60ти не висение
		{
			hovering = 0;
		}
		//Если рывок слишком большой и бьет по ушам
		double flapCGain = 0;
		if (((velocityX < 0 && accelerationX > 0.56) || (velocityX > 0 && accelerationX < -0.56)) && abs(velocityX) <= 16.67 /*&& velocityY < 4*/)
		{
			flapCGain = ((abs(accelerationX) - 0.56) * 4) * interpolation(-0.25, 0, 0.5, 0.5, 0.25, 1, velocityY) * hovering;//переходит в усиление нч по vy
			flapCGain = (flapCGain > 4) ? 4 : flapCGain;
		}

		lowFreqGain = pow(10, (turnGain + stepGain * 0.15 + absStepGain * 0.1 + mid2FreqStepGain * 0.3 + flapCGain + velocityGain) * 0.05); //0.15 -> 0.15
		mid1FreqGain = pow(10, (turnGain + stepGain * 0.2 + absStepGain * 0.1 + mid2FreqStepGain * 0.2 + flapCGain) * 0.05);//0.3 -> 0.2
		mid2FreqGain = pow(10, (turnGain + stepGain * 0.3 + absStepGain * 0.1 + velocityGain * 0.75) * 0.05);//0.4 -> 0.3
		highFreqGain = pow(10, (turnGain + stepGain * 0.5 + absStepGain * 0.3 + highFreqTurnGain) * 0.05);//

		lowFreqGain = (lowFreqGain <= 1) ? 1 : lowFreqGain;
		mid1FreqGain = (mid1FreqGain <= 1) ? 1 : mid1FreqGain;
		mid2FreqGain = (mid2FreqGain <= 1) ? 1 : mid2FreqGain;
		highFreqGain = (highFreqGain <= 1) ? 1 : highFreqGain;

		lowCutoffFreq = 400;//НЧ 50-800
		mid1CutoffFreq = 1000;//купол 1 200-3000
		mid2CutoffFreq = 3000;//купол 2 1000-8000
		highCutoffFreq = 10000;//ВЧ 4000-16000
	}
	//Полеты ка 27
	if (h.modelName == "ka_27")
	{
		//Набираем массив для рассчета усиления от среднего значения оборотов редуктора за 30с
		double averangeTurn = getAverange("redTurns", 30 * interpolation(0, 0.01, h.redTurnoverAvt, 1, sr.reduktor_gl_obor));

		//усиление от скорости выше 50км/ч (14м/c)
		double velocityGain = 0;
		if (abs(velocityX) < 42)
		{
			velocityGain = interpolation(14, 0, 42, 3, abs(velocityX));
		}
		else if (abs(velocityX) >= 42 && abs(velocityX) < 56)
		{
			velocityGain = interpolation(42, 3, 56, 7, abs(velocityX));
		}
		else
		{
			velocityGain = 7 + (abs(velocityX) - 56) * 0.071;
			velocityGain = (velocityGain > 9) ? 9 : velocityGain;
		}

		//Набираем массив для рассчета усиления от среднего значения шага за 50с
		double averangeStep = getAverange("step", 50);

		//Рассчитываем усиление от среднего
		double stepGain = (step - averangeStep) * interpolation(0, 0, 1, 1, high);

		//усиление по шагу в НЧ
		double mid2FreqStepGain = step * interpolation(0, 1, 5, 0, high);

		//усиление по шагу в Средних чатотах
		double absStepGain = step * interpolation(0, 1, 10.5, 0.5, 27.78, 0, abs(velocityX));

		//усиление от оборотов выше 10000
		double highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1;
		highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;
		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75;

		//усиление НЧ когда нет хлопков на границе 2го условия
		if (abs(velocityX) <= 16.67) //ниже 60ти висение
		{
			if (vectorVx.size() > 2)
			{
				if (vectorVx.at(vectorVx.size() - 2) > 16.67) //предыдущая точка была не на висении - не висение
				{
					hovering = 0;
				}
				else //предыдущая точка была на висениии - висение
				{
					if (abs(accelerationX) < 0.56) // условие висение возникает когда ускорение падает ниже условия
					{
						hovering = 1;
					}
				}
			}
		}
		else //выше 60ти не висение
		{
			hovering = 0;
		}
		//Если рывок слишком большой и бьет по ушам
		double flapCGain = 0;
		if (((velocityX < 0 && accelerationX > 0.56) || (velocityX > 0 && accelerationX < -0.56)) && abs(velocityX) <= 16.67 /*&& velocityY < 4*/)
		{
			flapCGain = ((abs(accelerationX) - 0.56) * 4) * interpolation(-0.25, 0, 0.5, 0.5, 0.25, 1, velocityY) * hovering;//переходит в усиление нч по vy
			flapCGain = (flapCGain > 4) ? 4 : flapCGain;
		}

		lowFreqGain = pow(10, (turnGain + stepGain * 0.15 + absStepGain * 0.1 + mid2FreqStepGain * 0.3 + flapCGain + velocityGain) * 0.05); //0.15 -> 0.15
		mid1FreqGain = pow(10, (turnGain + stepGain * 0.2 + absStepGain * 0.1 + mid2FreqStepGain * 0.2 + flapCGain) * 0.05);//0.3 -> 0.2
		mid2FreqGain = pow(10, (turnGain + stepGain * 0.3 + absStepGain * 0.1 + velocityGain * 0.75) * 0.05);//0.4 -> 0.3
		highFreqGain = pow(10, (turnGain + stepGain * 0.5 + absStepGain * 0.3 + highFreqTurnGain) * 0.05);//

		lowFreqGain = (lowFreqGain <= 1) ? 1 : lowFreqGain;
		mid1FreqGain = (mid1FreqGain <= 1) ? 1 : mid1FreqGain;
		mid2FreqGain = (mid2FreqGain <= 1) ? 1 : mid2FreqGain;
		highFreqGain = (highFreqGain <= 1) ? 1 : highFreqGain;

		lowCutoffFreq = 400;//НЧ 50-800
		mid1CutoffFreq = 1000;//купол 1 200-3000
		mid2CutoffFreq = 3000;//купол 2 1000-8000
		highCutoffFreq = 10000;//ВЧ 4000-16000
	}
	//Полеты ми 26
	if (h.modelName == "mi_26")
	{
		//добавляем шум биений
		if (beats != h.fullName["beats"])
		{
			setAndDeploySound(&buffer[2], &source[2], 0, h.fullName["beats"]);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);
			beats = h.fullName["beats"];

			alEffecti(effect[2], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
			alAuxiliaryEffectSloti(effectSlot[2], AL_EFFECTSLOT_EFFECT, effect[2]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
			alFilteri(filter[2], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[2], AL_LOWPASS_GAIN, 0);
			alSource3i(source[2], AL_AUXILIARY_SEND_FILTER, effectSlot[2], 0, NULL);
			alSourcei(source[2], AL_DIRECT_FILTER, filter[2]);
		}

		//регулируем громкость шума
		double beatsGain = pow(10, (interpolation(70, -12, 78, -8, 90, -2, sr.reduktor_gl_obor)) * 0.05);
		alSourcef(source[2], AL_GAIN, beatsGain);

		double averangeTurn = getAverange("redTurns", 30 * interpolation(0, 0.01, h.redTurnoverAvt, 1, sr.reduktor_gl_obor));

		//усиление от оборотов выше 10000
		double highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1.5;
		highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;

		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75;

		double averangeStep = getAverange("step", 20);

		//Усиление от шага
		double stepGain = (step - averangeStep) * interpolation(0, 0, 1, 1, high);

		//усиление по шагу в НЧ
		double mid2FreqStepGain = step * 2 * interpolation(0, 1, 10, 0, high);

		//Усиление по Vy
		double velocityYGain = -velocityY * interpolation(0, 1, 22.4, 0, abs(velocityX));

		//Страгивание
		//Усиление редуктора в НЧ в начале движения по ВПП
		double stalkingGain = (accelerationX > 0) ? accelerationX * 5 * interpolation(0, 1, 8.3, 0, velocityX) * !high : 0;

		lowFreqGain = pow(10, (velocityYGain + stepGain * 0.25 + stalkingGain + mid2FreqStepGain) * 0.05);
		mid1FreqGain = pow(10, (turnGain + stepGain * 0.75 + stalkingGain + mid2FreqStepGain) * 0.05);
		mid2FreqGain = pow(10, (turnGain + stepGain * 0.75) * 0.05);
		highFreqGain = pow(10, (turnGain + stepGain * 0.75 + highFreqTurnGain) * 0.05);

		lowFreqGain = (lowFreqGain <= 1) ? 1 : lowFreqGain;
		mid1FreqGain = (mid1FreqGain <= 1) ? 1 : mid1FreqGain;
		mid2FreqGain = (mid2FreqGain <= 1) ? 1 : mid2FreqGain;
		highFreqGain = (highFreqGain <= 1) ? 1 : highFreqGain;

		lowCutoffFreq = 200;//НЧ 50-800
		mid1CutoffFreq = 1000;//купол 1 200-3000
		mid2CutoffFreq = 3000;//купол 2 1000-8000
		highCutoffFreq = 10000;//ВЧ 4000-16000

		//Усиление биений
		alEffectf(effect[2], AL_EQUALIZER_LOW_CUTOFF, lowCutoffFreq);
		alEffectf(effect[2], AL_EQUALIZER_MID1_CENTER, mid1CutoffFreq);
		alEffectf(effect[2], AL_EQUALIZER_MID2_CENTER, mid2CutoffFreq);
		alEffectf(effect[2], AL_EQUALIZER_HIGH_CUTOFF, highCutoffFreq);

		alEffectf(effect[2], AL_EQUALIZER_LOW_GAIN, pow(10, (stepGain * 0.25)*0.05));//

		alAuxiliaryEffectSloti(effectSlot[2], AL_EFFECTSLOT_EFFECT, effect[2]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
	}

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

		alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
	}

	return 1;
}

Engine::Engine() : Sound(2, 2, 2)
{
	//При одновременном запуске двигателей возможен эффект наложения, дающий искажение звука
	//Поэтому каждый объект двигателя имеет свой параметр фазы запуска
	engCount++;
	phase = (engCount - 1) * 0.33;
	engNum = engCount;
}

Engine::~Engine()
{
	engCount--;
}

int Engine::play(bool status_on, bool status_off, double parameter, Helicopter h)
{
	for (size_t i = 0; i < 2; i++)
	{
		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);
		//Подключаем эквалайзер
		if (eq[i] != "set")
		{
			alEffecti(effect[i], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
			alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
			alFilteri(filter[i], AL_FILTER_TYPE, AL_FILTER_LOWPASS);//определяем фильтр как НЧ
			alFilterf(filter[i], AL_LOWPASS_GAIN, 0);//убираем звук чистого источника
			alSource3i(source[i], AL_AUXILIARY_SEND_FILTER, effectSlot[i], 0, 0);//посылаем выход источника через слот с эффектом
			alSourcei(source[i], AL_DIRECT_FILTER, filter[i]);//подключаем фильтр к источнику
			eq[i] = "set";
		}
		//Загружаем буферы и запускам источники
		if (fileBuffered[i] != filetoBuffer[i])
		{
			setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}
		//Выключаем источники если обороты равны 0 и двигатель не запускается
		if (!status_on && parameter == 0)
		{
			alSourceStop(source[i]);
			alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);		// перезабили признак 
			fileBuffered[i] = filetoBuffer[i] = "NULL";
		}
	}

	//ДВИГАТЕЛЬ АНСАТА
	if (h.modelName == "ansat")
	{
		//0 -> мг
		if (parameter <= h.engTurnoverMg && status_on)
		{
			filetoBuffer[0] = h.fullName["eng_on_w"];
			filetoBuffer[1] = h.fullName["eng_w_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_FALSE);
			offset[0] = getOffset(1, h.fullName["eng_on"], parameter);
			offset[1] = getLengthWAV(h.fullName["eng_w_w"]) * phase;

			double fade = 0;
			double rise = 0;
			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);

			//Если запуск проигрывается - плавно переходим на цикл
			if (sourceStatus[0] == AL_PLAYING)
			{
				//Оцениваем выходную громкость источников по параметрам оборотов и громкости,
				//чтобы обеспечить нормальное звучание, если обороты по какимто причинам не вышли на малый газ,
				//до окончания проигрывания записи запуска
				double fadeTurn = 0;
				double fadeTime = 0;
				double riseTurn = 0;
				double riseTime = 0;
				lengthOn = getLengthWAV(h.fullName["eng_on_w"]);
				lengthOff = getLengthWAV(h.fullName["eng_w_w"]);
				crossFade(&fadeTurn, &riseTurn, parameter, h.engTurnoverMg - 10, h.engTurnoverMg, masterGain*h.engFactor);
				if (offsetOn != 0)
					crossFade(&fadeTime, &riseTime, offsetOn, lengthOn - lengthOff, lengthOn, masterGain*h.engFactor);
				fade = (fadeTurn < fadeTime) ? fadeTurn : fadeTime;
				rise = 1 - fade;
			}
			//Иначе используем цикл
			else
			{
				fade = 0;
				rise = 1;
			}
			alSourcef(source[0], AL_GAIN, fade);
			alSourcef(source[1], AL_GAIN, rise);

			if (parameter > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["eng_on"], parameter));
			else
				alSourcef(source[0], AL_PITCH, 1);

			alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//меняем pitch (дает нисходящую прямую при остановке второго дв)


		}
		//мг -> 0
		if (status_off && parameter > 0 && parameter < h.engTurnoverMg)
		{
			filetoBuffer[0] = h.fullName["eng_off_w"];
			filetoBuffer[1] = h.fullName["eng_w_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_FALSE);
			offset[0] = getOffset(1, h.fullName["eng_off"], parameter);
			offset[1] = getLengthWAV(h.fullName["eng_w_w"]) * phase;

			double fade, rise;
			crossFade(&fade, &rise, parameter, h.engTurnoverMg, h.engTurnoverMg - 10, masterGain*h.engFactor);
			alSourcef(source[1], AL_GAIN, fade);
			alSourcef(source[0], AL_GAIN, rise);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);

			if (parameter > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["eng_off"], parameter));
			else
				alSourcef(source[0], AL_PITCH, 1);
		}
	}
	else
	{
		//0 -> мг
		if (parameter <= h.engTurnoverMg && status_on)
		{
			filetoBuffer[0] = h.fullName["eng_on_w"];
			filetoBuffer[1] = h.fullName["eng_w_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_FALSE);
			offset[0] = getOffset(1, h.fullName["eng_on"], parameter);
			offset[1] = getLengthWAV(h.fullName["eng_w_w"]) * phase;

			double fade = 0;
			double rise = 0;
			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);

			//Если запуск проигрывается - плавно переходим на цикл
			if (sourceStatus[0] == AL_PLAYING)
			{
				//Оцениваем выходную громкость источников по параметрам оборотов и громкости,
				//чтобы обеспечить нормальное звучание, если обороты по какимто причинам не вышли на малый газ,
				//до окончания проигрывания записи запуска
				double fadeTurn = 0;
				double fadeTime = 0;
				double riseTurn = 0;
				double riseTime = 0;
				lengthOn = getLengthWAV(h.fullName["eng_on_w"]);
				lengthOff = getLengthWAV(h.fullName["eng_w_w"]);
				crossFade(&fadeTurn, &riseTurn, parameter, h.engTurnoverMg - 10, h.engTurnoverMg, masterGain*h.engFactor);
				if (offsetOn != 0)
					crossFade(&fadeTime, &riseTime, offsetOn, lengthOn - lengthOff, lengthOn, masterGain*h.engFactor);
				fade = (fadeTurn < fadeTime) ? fadeTurn : fadeTime;
				rise = 1 - fade;
			}
			//Иначе используем цикл
			else
			{
				fade = 0;
				rise = 1;
			}

			alSourcef(source[0], AL_GAIN, fade);
			alSourcef(source[1], AL_GAIN, rise);

			//alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);

			if (parameter > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["eng_on"], parameter));
			else
				alSourcef(source[0], AL_PITCH, 1);
			alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);//меняем pitch (дает нисходящую прямую при остановке второго дв)
		}
		//мг <-> авт
		if (parameter > h.engTurnoverMg)
		{
			filetoBuffer[0] = h.fullName["eng_w_avt_w"];
			filetoBuffer[1] = h.fullName["eng_w_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			offset[0] = getLengthWAV(h.fullName["eng_w_avt_w"]) * phase;
			offset[1] = getLengthWAV(h.fullName["eng_w_w"]) * phase;

			double fade, rise;
			crossFade(&fade, &rise, parameter, h.engTurnoverMg, h.engTurnoverAvt, masterGain*h.engFactor);
			alSourcef(source[1], AL_GAIN, fade);
			alSourcef(source[0], AL_GAIN, rise);

			alSourcef(source[1], AL_PITCH, parameter / h.engTurnoverMg);
			alSourcef(source[0], AL_PITCH, parameter / h.engTurnoverAvt);//меняем pitch (дает нисходящую прямую при остановке второго дв)
		}
		//мг -> 0
		if (status_off && parameter > 0 && parameter < h.engTurnoverMg)
		{
			filetoBuffer[0] = h.fullName["eng_off_w"];
			filetoBuffer[1] = h.fullName["eng_w_w"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_FALSE);
			offset[0] = getOffset(1, h.fullName["eng_off"], parameter);
			offset[1] = getLengthWAV(h.fullName["eng_w_w"]) * phase;

			double fade, rise;
			crossFade(&fade, &rise, parameter, h.engTurnoverMg, h.engTurnoverMg - 10, masterGain*h.engFactor);
			alSourcef(source[1], AL_GAIN, fade);
			alSourcef(source[0], AL_GAIN, rise);

			alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOff);

			if (parameter > 5)
				alSourcef(source[0], AL_PITCH, getPitch(offsetOff, h.fullName["eng_off"], parameter));
			else
				alSourcef(source[0], AL_PITCH, 1);
		}
	}

	double lowFreqGain = AL_EQUALIZER_DEFAULT_LOW_GAIN;
	double mid1FreqGain = AL_EQUALIZER_DEFAULT_MID1_GAIN;
	double mid2FreqGain = AL_EQUALIZER_DEFAULT_MID2_GAIN;
	double highFreqGain = AL_EQUALIZER_DEFAULT_HIGH_GAIN;

	double lowCutoffFreq = AL_EQUALIZER_DEFAULT_LOW_CUTOFF;//НЧ 50-800
	double mid1CutoffFreq = AL_EQUALIZER_DEFAULT_MID1_CENTER;//купол 1 200-3000
	double mid2CutoffFreq = AL_EQUALIZER_DEFAULT_MID2_CENTER;//купол 2 1000-8000
	double highCutoffFreq = AL_EQUALIZER_DEFAULT_HIGH_CUTOFF;//ВЧ 4000-16000

	//Полеты 8 мтв5, 8 амтш, ка 27м, ка 29
	if (h.modelName == "mi_8_amtsh" || h.modelName == "mi_8_mtv5" || h.modelName == "mi_28" || h.modelName == "mi_26" || h.modelName == "ka_27" || h.modelName == "ka_29")
	{
		double averangeTurn = 0;
		if (engNum == 1)
		{
			averangeTurn = getAverange("eng1Turns", 25 * interpolation(0, 0.01, h.engTurnoverAvt, 1, parameter));
		}
		else
		{
			averangeTurn = getAverange("eng2Turns", 25 * interpolation(0, 0.01, h.engTurnoverAvt, 1, parameter));
		}

		////усиление от оборотов выше 10000
		//double highFreqTurnGain = (parameter - averangeTurn) * 0.35;
		//highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;

		//усиление от оборотов
		double turnGain = (parameter - averangeTurn) * 0.35;

		lowFreqGain = pow(10, (turnGain)*0.05);
		mid1FreqGain = pow(10, (turnGain)*0.05);
		mid2FreqGain = pow(10, (turnGain)*0.05);
		highFreqGain = pow(10, (turnGain /*+ highFreqTurnGain*/)*0.05);
	}

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

		alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
	}

	return 1;
}

VintFlap::VintFlap() : Sound(3, 3, 2)
{
	remove("test.txt");
}

int VintFlap::play(Helicopter h, SOUNDREAD sr)
{
	//Условия хлопков
	//для отрицательной скорости условия зеркальные
	const double accelerationXBorder = 0.28;//!<мс/с*с
	const double velocityYBorder = -2;//!<мс/с
	const double dashBorder = -0.672;
	int flapIndicator = 0;
	if (velocityX < 0)
	{
		//dv>1 и vy<-2
		if (accelerationX >= -accelerationXBorder && velocityY <= velocityYBorder)//1
		{
			if ((calcA > 0) & (dash < -dashBorder) & (accelerationVy < 3))
			{
				flapIndicator = 1;
			}
		}
		//dv>1 и vy>-2
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
		//dv<1 и vy<-2
		if (accelerationX <= accelerationXBorder && velocityY <= velocityYBorder)//1
		{
			if ((calcA > 0) & (dash > dashBorder) & (accelerationVy < 3))
			{
				flapIndicator = 1;
			}
		}
		//dv<1 и vy>-2
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

	//Полеты 8 мтв5
	if (h.modelName == "mi_8_mtv5")
	{
		if (key[0] != h.fullName["vint_flap"])
		{
			sourceStatus[0] = setAndDeploySound(&buffer[0], &source[0], 0, h.fullName["vint_flap"]);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			key[0] = h.fullName["vint_flap"];

		}

		double atkXvel = calcA * interpolation(0, 0, 16.67, 1, abs(velocityX));

		double averangeTurn = getAverange("redTurns", 30 * interpolation(0, 0.01, h.redTurnoverAvt, 1, sr.reduktor_gl_obor));

		//Усиление от оборотов
		//только если атака больше 2х
		//делает огибающую атаки более динамичной
		double turnsGain = 0;
		if (atkXvel >= 2)
		{
			//усиление от оборотов
			turnsGain = (sr.reduktor_gl_obor - averangeTurn) * 2;
		}

		//Хлопки плавно появляются
		double h_g = interpolation(0, 0, 0.5, 0.5, 1, 1, high);//К 1 метру по высоте
		double v_g = interpolation(10, 0, 14, 0.25, 28, 1, abs(velocityX));//К 28 м/с

		//Громкость хлопков зависит от атаки
		double gain_a = interpolation(-1, -15, 1, -9, 3, -3, atkXvel);

		double atkFls = pow(10, (turnsGain + gain_a)*0.05) * h_g * v_g;

		//При втором условии, на висении, используем ускорение в качестве переходной функции хлопков
		//При а = 6 м/с^2 громкость хлопков номинальная, при условии, что вертолет летит вниз, а скорость ниже 60 км/ч (16.67)
		double flapCGainAccX = interpolation(0.56, 0, 6, 1, abs(accelerationX)) * interpolation(-0.25, 1, 0.25, 0, velocityY) * interpolation(0, 1, 16.67, 0, velocityX);//переходит в усиление нч по vy

		if (flapIndicator == 2)//хлопаем
		{
			offsetOn += deltaTime;
			offsetOn = (offsetOn > 1) ? 1 : offsetOn;//плавно наводим громкость за 1с
		}
		else
		{
			offsetOn -= deltaTime;
			offsetOn = (offsetOn < 0) ? 0 : offsetOn;
		}

		double gain = (atkFls > flapCGainAccX) ? atkFls : flapCGainAccX * offsetOn;

		//Уменьшаем громкость хлопков на 6дб за каждый градус шага ниже 6
		double stepDamping = pow(10, ((step < 3.5) ? ((3.5 - step) * -6) : 0)*0.05);

		alSourcef(source[0], AL_GAIN, gain * h.vintFlapFactor * masterGain * stepDamping);

	}
	//Полеты 8 амтш
	if (h.modelName == "mi_8_amtsh")
	{
		if (key[0] != h.fullName["vint_flap"])
		{
			sourceStatus[0] = setAndDeploySound(&buffer[0], &source[0], 0, h.fullName["vint_flap"]);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			key[0] = h.fullName["vint_flap"];

		}
		if (key[1] != h.fullName["vint_flap_low"])
		{
			sourceStatus[1] = setAndDeploySound(&buffer[1], &source[1], 0, h.fullName["vint_flap_low"]);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			key[1] = h.fullName["vint_flap_low"];

		}

		double averangeTurn = getAverange("redTurns", 30 * interpolation(0, 0.01, h.redTurnoverAvt, 1, sr.reduktor_gl_obor));

		double atkXvel = calcA * interpolation(0, 0, 16.67, 1, abs(velocityX));

		//Осуществляем переход в НЧ хлопки, при малых значениях шага (4-5)
		double flap_h = 0;
		double flap_lo = 0;
		crossFade(&flap_lo, &flap_h, step, 4, 5, 1);

		//Осуществляем переход в НЧ хлопки, при малых значениях скорости (14-16.67м/с)
		double flap_hv = 0;
		double flap_lov = 0;
		crossFade(&flap_lov, &flap_hv, abs(velocityX), 14, 16.67, 1);

		//Из 2х условия НЧ хлопков выбираем преобладающий
		double low = max(flap_lo, flap_lov);

		//Усиление от оборотов
		//только если атака больше 2х
		//делает огибающую атаки более динамичной
		double turnsGain = 0;
		if (atkXvel >= 2)
		{
			//усиление от оборотов
			turnsGain = (sr.reduktor_gl_obor - averangeTurn) * 2;
		}

		double hG = interpolation(0, 0, 0.5, 0.5, 1, 1, high);

		//Передаточные функции по скорости для хлопков по атаки
		//vL для НЧ
		//vH для ВЧ
		double vL = 0;
		double vH = 0;
		if (abs(velocityX) < 11)
		{
			vL = interpolation(10, 0, 11, 0.5, abs(velocityX));//
			vH = interpolation(10, 0, 11, 0.25, abs(velocityX));//
		}
		else
		{
			vL = interpolation(11, 0.5, 21, 1, abs(velocityX));//
			vH = interpolation(11, 0.25, 21, 1, abs(velocityX));//
		}
		double vG = vL * low + vH * (1 - low);

		double gainAtk = interpolation(-1, -15, 1, -9, 3, -3, atkXvel);

		//Вычисляем громкость хлопков по атаке
		double atkFls = pow(10, (turnsGain + gainAtk)*0.05) * vG * hG;

		//Хлопки на висении возникают при 2ом условии хлопков
		if (flapIndicator == 2)
		{
			offsetOn += deltaTime;
			offsetOn = (offsetOn > 1) ? 1 : offsetOn;//плавно наводим громкость за 1с
		}
		else
		{
			offsetOn -= deltaTime;
			offsetOn = (offsetOn < 0) ? 0 : offsetOn;
		}

		//При втором условии, на висении, используем ускорение в качестве переходной функции хлопков
		//переходит в усиление НЧ редуктора по vy
		double flapCGainAccX = interpolation(0.56, 0, 3, 1, abs(accelerationX)) * interpolation(-0.25, 1, 0.25, 0, velocityY) * offsetOn  * interpolation(0, 1, 16.67, 0, velocityX);

		//Из 2х видов хлопков выбираем более громкие
		double gain = max(atkFls, flapCGainAccX);

		//Устанавливаем громкость НЧ и ВЧ хлопков
		alSourcef(source[0], AL_GAIN, gain * h.vintFlapFactor * masterGain * (1 - low));
		alSourcef(source[1], AL_GAIN, gain * h.vintFlapFactor * masterGain * low);

	}
	//Полеты 28
	if (h.modelName == "mi_28")
	{
		if (key[0] != h.fullName["vint_flap"])
		{
			setAndDeploySound(&buffer[0], &source[0], 0, h.fullName["vint_flap"]);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			key[0] = h.fullName["vint_flap"];
		}
		if (key[1] != h.fullName["vint_flap_low"])
		{
			setAndDeploySound(&buffer[1], &source[1], 0, h.fullName["vint_flap_low"]);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			key[1] = h.fullName["vint_flap_low"];
		}

		double averangeTurn = getAverange("redTurns", 30 * interpolation(0, 0.01, h.redTurnoverAvt, 1, sr.reduktor_gl_obor));

		double gain_a = 0;
		double h_g = 0;
		double v_g = 0;
		const double floor = 5;//сдвигаем передаточную функцию атаки на floor вправо

		//Игнорируем некорректные значения атаки, при маленьких значениях скоростей
		double atkXvel = calcA * interpolation(0, 0, 16.67, 1, abs(velocityX));

		h_g = interpolation(0, 0, 0.5, 0.5, 1, 1, high);
		v_g = interpolation(14, 0, 17, 0.5, 20, 1, abs(velocityX));//72(1) - 50(0)
		gain_a = interpolation(-1 + floor, -18, 1 + floor, -12, 3 + floor, -6, atkXvel);

		//При втором условии, на висении, используем ускорение в качестве переходной функции хлопков
		double flapCGainAccX = interpolation(0.56, 0, 1, 1, abs(accelerationX)) * interpolation(-0.25, 1, 0.5, 0.5, 0.25, 0, velocityY) * interpolation(0, 1, 16.67, 0, velocityX);//переходит в усиление нч по vy

		if (flapIndicator == 2)//хлопаем
		{
			offsetOn += deltaTime;
			offsetOn = (offsetOn > 1) ? 1 : offsetOn;//плавно наводим громкость за 1с
		}
		else
		{
			offsetOn -= deltaTime;
			offsetOn = (offsetOn < 0) ? 0 : offsetOn;
		}

		//усиление от оборотов
		double turnsGain = 0;
		if (atkXvel >= 2)
		{
			//усиление от оборотов
			turnsGain = (sr.reduktor_gl_obor - averangeTurn) * 2;
		}
		double atkFls = pow(10, (turnsGain + gain_a)*0.05) * h_g * v_g;

		double gain = (atkFls > flapCGainAccX) ? atkFls : flapCGainAccX * offsetOn;

		double flap_h = 0;
		double flap_lo = 0;
		double flap_hv = 0;
		double flap_lov = 0;
		crossFade(&flap_lo, &flap_h, step, 5, 6, 1);
		crossFade(&flap_lov, &flap_hv, velocityX, 14, 16.67, 1);
		double low = max(flap_lo, flap_lov);
		alSourcef(source[0], AL_GAIN, gain * h.vintFlapFactor *masterGain *(h.vintFlapFactor*masterGain - low));
		alSourcef(source[1], AL_GAIN, gain * h.vintFlapFactor *masterGain * low);


	}
	//Полеты ка 27 - 29
	if (h.modelName == "ka_27" || h.modelName == "ka_29")
	{
		if (key[0] != h.fullName["vint_flap_A"] || key[1] != h.fullName["vint_flap_B"] || key[2] != h.fullName["vint_flap_C"])
		{
			//загружаем НЧ хлопки
			setAndDeploySound(&buffer[2], &source[2], 0, h.fullName["vint_flap_C"]);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);
			//загружаем неравномерные ВЧ хлопки
			setAndDeploySound(&buffer[1], &source[1], 0, h.fullName["vint_flap_B"]);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);

			alEffecti(effect[1], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
			alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[1]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
			alFilteri(filter[1], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[1], AL_LOWPASS_GAIN, 0);
			alSource3i(source[1], AL_AUXILIARY_SEND_FILTER, effectSlot[1], 0, 0);
			alSourcei(source[1], AL_DIRECT_FILTER, filter[1]);
			//загружаем равномерные ВЧ хлопки
			setAndDeploySound(&buffer[0], &source[0], 0, h.fullName["vint_flap_A"]);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);

			alEffecti(effect[0], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
			alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
			alFilteri(filter[0], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[0], AL_LOWPASS_GAIN, 0);
			alSource3i(source[0], AL_AUXILIARY_SEND_FILTER, effectSlot[0], 0, 0);
			alSourcei(source[0], AL_DIRECT_FILTER, filter[0]);

			alSourceStop(source[0]);
			alSourceStop(source[1]);
			alSourceStop(source[2]);
			//Запускаем одновременно (практически)
			alSourcePlay(source[0]);
			alSourcePlay(source[1]);
			alSourcePlay(source[2]);
			alGetSourcei(source[0], AL_SOURCE_STATE, &sourceStatus[0]);		// перезабили признак 
			alGetSourcei(source[1], AL_SOURCE_STATE, &sourceStatus[1]);		// перезабили признак
			alGetSourcei(source[2], AL_SOURCE_STATE, &sourceStatus[2]);		// перезабили признак
			key[0] = h.fullName["vint_flap_A"];
			key[1] = h.fullName["vint_flap_B"];
			key[2] = h.fullName["vint_flap_C"];
		}

		//Управляем частотой среза хлопков от оборотов редуктора
		double lowerFreqLimit = log10(1000);//переходим в линейную шкалу
		double highterFreqLimit = log10(6000);
		double freqCutoffFromTurns = 0;
		if (sr.reduktor_gl_obor >= 89 & sr.reduktor_gl_obor <= 90)
		{
			freqCutoffFromTurns = interpolation(89, lowerFreqLimit, 90, highterFreqLimit, sr.reduktor_gl_obor);
		}
		else if (sr.reduktor_gl_obor > 90)
		{
			freqCutoffFromTurns = (sr.reduktor_gl_obor / (90. / 100.))*highterFreqLimit / 100.;
		}
		else
		{
			freqCutoffFromTurns = lowerFreqLimit;
		}
		freqCutoffFromTurns = pow(10, freqCutoffFromTurns);//Возвращаемся к герцам

		//Если выполняется условие хлопков, выводим хлопки
		if (flapIndicator)//хлопаем
		{
			offsetOn += deltaTime;
			offsetOn = (offsetOn > 1) ? 1 : offsetOn;//плавно наводим громкость за 1с
		}
		else
		{
			offsetOn -= deltaTime;
			offsetOn = (offsetOn < 0) ? 0 : offsetOn;
		}
		double lowFreqGain = 1;
		double mid1FreqGain = interpolation(log10(1000), 0.126, log10(2000), 0.5, log10(3000), 1, log10(freqCutoffFromTurns));
		double mid2FreqGain = interpolation(log10(2000), 0.126, log10(3000), 0.5, log10(4000), 1, log10(freqCutoffFromTurns));//ниже 4к изменяем усиление куполов
		double highFreqGain = 0.126;

		double lowCutoff = 800;
		double mid1Cutoff = 2000;//1-3
		double mid2Cutoff = 3000;//4-6
		double highCutoff = (freqCutoffFromTurns > AL_EQUALIZER_MIN_HIGH_CUTOFF) ? freqCutoffFromTurns : AL_EQUALIZER_MIN_HIGH_CUTOFF;//Выше 4к двигаем верхний шельф

		alEffectf(effect[0], AL_EQUALIZER_LOW_CUTOFF, lowCutoff);
		alEffectf(effect[0], AL_EQUALIZER_MID1_CENTER, mid1Cutoff);
		alEffectf(effect[0], AL_EQUALIZER_MID2_CENTER, mid2Cutoff);
		alEffectf(effect[0], AL_EQUALIZER_HIGH_CUTOFF, highCutoff);

		alEffectf(effect[0], AL_EQUALIZER_LOW_GAIN, lowFreqGain);//
		alEffectf(effect[0], AL_EQUALIZER_MID1_GAIN, mid1FreqGain);//
		alEffectf(effect[0], AL_EQUALIZER_MID2_GAIN, mid2FreqGain);//
		alEffectf(effect[0], AL_EQUALIZER_HIGH_GAIN, highFreqGain);//

		alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
		alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[0]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)

		//Условие и громкость НЧ хлопков в некоторых случаях определяется ускорением и высокой скоростью
		double accelerationGain = (3 * abs(accelerationX)) - 15;
		accelerationGain = (accelerationGain > 5) ? -accelerationGain : accelerationGain;//дб
		accelerationGain = pow(10, accelerationGain * 0.05);//коэф

		double hiSpeedGain = 0;
		if (abs(velocityX) <= 61.6)
		{
			hiSpeedGain = interpolation(50, 0, 61.6, 0.67, abs(velocityX));
		}
		else
		{
			hiSpeedGain = interpolation(61.6, 0.67, 75, 1, abs(velocityX));
		}
		double resFlapCGain = max(hiSpeedGain, accelerationGain);//из двух показателей громкости НЧ хлопков выбираем преобладающий

		double turnsGain = 0;
		if (sr.reduktor_gl_obor <= 91)
		{
			turnsGain = (91 - sr.reduktor_gl_obor) * (-3);//рассчитываем усиление от оборотов винта - 3дб на оборот
		}

		//Условие затухания хлопков
		double off = interpolation(14, 1, 0, 0, abs(velocityX));
		//Условие выбора равномерных-неравномерных хлопков
		double flapA = 0;
		double flapB = 0;
		if (velocityX < 0)
		{
			flapA = (accelerationX >= -accelerationXBorder) ? 1 : 0;//условие равномерных хлопков
			flapB = (accelerationX < -accelerationXBorder) ? 1 : 0;//условие неравномерных хлопков
		}
		else
		{
			flapA = (accelerationX <= accelerationXBorder) ? 1 : 0;//условие равномерных хлопков
			flapB = (accelerationX > accelerationXBorder) ? 1 : 0;//условие неравномерных хлопков
		}

		//Плавный переход между НЧ и ВЧ хлопками по шагу
		double flapABStep = 0;
		double flapCStep = 0;
		crossFade(&flapCStep, &flapABStep, step, 8, 12, 1);
		//убираем эффект шага на вид хлопков,определяем по скорости, так как этот параметр более приоритетный
		if (abs(velocityX) < 16.67)
		{
			flapABStep = 1;
			flapCStep = 1;
		}
		//Плавный переход между НЧ и ВЧ хлопками по скорости
		double flapABVX = 0;
		double flapCVX = 0;
		crossFade(&flapCVX, &flapABVX, abs(velocityX), 15.28, 16.67, 1);

		//При втором условии, на висении, используем ускорение в качестве переходной функции хлопков
		double flapCGainAccX = 1;
		if (flapIndicator == 2 && abs(velocityX) < 16.67)
		{
			flapCGainAccX = interpolation(0.56, 0, 1, 1, abs(accelerationX)) * interpolation(-0.25, 1, 0.5, 0.5, 0.25, 0, velocityY);//переходит в усиление нч по vy
		}
		//рассчитываем результирующую громкость хлопков в каждый момент времени
		double flapAGain = flapA * offsetOn * off * masterGain * h.vintFlapFactor * flapABStep * flapABVX * pow(10, turnsGain*0.05);
		double flapBGain = flapB * offsetOn * off * masterGain * h.vintFlapFactor * flapABStep * flapABVX * pow(10, turnsGain*0.05);
		double flapCGain = ((flapIndicator) ? (flapCGainAccX * flapCStep * flapCVX * offsetOn * off * pow(10, turnsGain*0.05) * masterGain * (h.vintFlapFactor + (1 - h.vintFlapFactor)*0.5)) : (masterGain * (h.vintFlapFactor + (1 - h.vintFlapFactor)*0.5) * (1 - offsetOn) * resFlapCGain * off));

		alSourcef(source[0], AL_GAIN, flapAGain);//равномерные
		alSourcef(source[1], AL_GAIN, flapBGain);//неравномерные
		alSourcef(source[2], AL_GAIN, flapCGain);//тупые

	}
	//Полеты ми 26
	if (h.modelName == "mi_26")
	{
		if (key[0] != h.fullName["vint_flap"])
		{
			setAndDeploySound(&buffer[0], &source[0], 0, h.fullName["vint_flap"]);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			key[0] = h.fullName["vint_flap"];
		}
		if (key[1] != h.fullName["flapping"])
		{
			setAndDeploySound(&buffer[1], &source[1], 0, h.fullName["flapping"]);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			key[1] = h.fullName["flapping"];
		}

		//Используем модифицированную атаку
		double attack = calcA * interpolation(0, 0, 22.4, 1, velocityX) + (step - getAverange("step", 25)) * 5 * ((velocityY < 0) ? 1 : 0);

		//Передаточная финкция усиления хлопков от атаки
		double atkGain = pow(10, (attack - 18) * 0.05);

		//Усиление хлопков при малом шаге
		double flapStepGain = pow(10, interpolation(2, 2, 4, 0, step)*0.05);

		//Ослабление хлопк(ания)(ования)(очения)(???) при малом шаге
		double flappingStepGain = pow(10, interpolation(2, -3, 4, 0, step)*0.05);

		//Усиление по Vy при низких скоростях
		double flappingVyGain = pow(10, interpolation(-3, 0, 0, -10, velocityY)*0.05) * interpolation(0, 1, 22.4, 0, abs(velocityX));

		//Усиление по Vx
		double flappingVxGain = 0;
		if (abs(velocityX) > 56)
		{
			flappingVxGain = pow(10, interpolation(56, -3, 70, 0, abs(velocityX))*0.05);
		}
		else
		{
			flappingVxGain = pow(10, (abs(velocityX) * 1.07 - 66.924)*0.05);
		}

		//Берем большее из 3х показателей для flapping
		double flappingGainUnhover = max(max(flappingVyGain, atkGain), flappingVxGain);

		double accGain = pow(10, (abs(accelerationX) * 8.92 - 10)*0.05);
		accGain = (accGain > 1) ? 1 : accGain;

		//При втором условии, на висении, используем ускорение в качестве переходной функции хлопков
		double hoveringGain = ((velocityX * accelerationX < 0) ? accGain : accGain * interpolation(0, 1, 0.5, 0, abs(velocityX))) * interpolation(0, 1, 22.4, 0, velocityX);

		//Усиление висения по vy
		double velocityYGainFlap = pow(10, velocityY * 1 * 0.05);
		double velocityYGainFlapping = pow(10, velocityY * -2 * 0.05);

		//Выбираем между хлопками на висении и нет
		double flappingFinalGain = max(hoveringGain * velocityYGainFlapping, flappingGainUnhover * flappingStepGain);
		double flapFinalGain = max(hoveringGain * velocityYGainFlap, atkGain * flapStepGain);

		alSourcef(source[0], AL_GAIN, flapFinalGain * h.vintFlapFactor * masterGain * interpolation(0, 0, 0.3, 1, high));
		alSourcef(source[1], AL_GAIN, flappingFinalGain * masterGain * interpolation(0, 0, 0.3, 1, high));

		cout << " attack "<< attack <<" = "<< calcA * interpolation(0, 0, 22.4, 1, velocityX) <<" + "<< (step - getAverange("step", 25)) * 5 * ((velocityY < 0) ? 1 : 0) <<"\t"<< flappingFinalGain <<"\t\t\t\r";
		static double p;
		p += deltaTime;
		if (p >0.1)
		{
			FILE *f = fopen("test.txt", "at");
			fprintf(f, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", atkGain, flappingVyGain, flappingVxGain, hoveringGain, flapFinalGain * h.vintFlapFactor * masterGain, flappingFinalGain * masterGain, attack , currentTime);
			fclose(f);
			p = 0;
		}
	}
	return 1;
}

VintSwish::VintSwish() : Sound(2, 2, 0)
{

}

int VintSwish::play(Helicopter h, SOUNDREAD sr)
{
	for (size_t i = 0; i < 2; i++)
	{
		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);

		//Загружаем буферы и запускам источники
		if (fileBuffered[i] != filetoBuffer[i])
		{
			setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}
		//Выключаем источники если обороты равны 0 и двигатель не запускается
		if (sr.reduktor_gl_obor == 0 && !sr.p_eng1_zap && !sr.p_eng2_zap)
		{
			alSourceStop(source[i]);
			alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);		// перезабили признак 
			fileBuffered[i] = filetoBuffer[i] = "NULL";
		}
	}

	//0 -> мг 1дв
	if (sr.reduktor_gl_obor <= h.redTurnoverMg1 && (sr.p_eng1_zap | sr.p_eng2_zap))
	{
		filetoBuffer[0] = h.fullName["vint_swish_on"];
		filetoBuffer[1] = h.fullName["vint_swish_w"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		alSourcei(source[0], AL_LOOPING, AL_FALSE);
		offset[0] = getOffset(1, h.fullName["red_on"], sr.reduktor_gl_obor);
		offset[1] = 0;

		double fade, rise;
		crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1 - 10, h.redTurnoverMg1, masterGain*h.vintSwishFactor);
		alSourcef(source[0], AL_GAIN, fade);//0
		alSourcef(source[1], AL_GAIN, rise);//1

		alGetSourcef(source[0], AL_SEC_OFFSET, &offsetOn);
		if (sr.reduktor_gl_obor >= 5)
			alSourcef(source[0], AL_PITCH, getPitch(offsetOn, h.fullName["red_on"], sr.reduktor_gl_obor));
		else
			alSourcef(source[0], AL_PITCH, 1);
		alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);//меняем pitch (дает нисходящую прямую при остановке второго дв)


	}
	//мг1дв <-> мг2дв
	if (sr.reduktor_gl_obor > h.redTurnoverMg1 && sr.reduktor_gl_obor <= h.redTurnoverMg2)
	{
		filetoBuffer[1] = h.fullName["vint_swish_w"];
		filetoBuffer[0] = h.fullName["vint_swish_w_mg"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		alSourcei(source[0], AL_LOOPING, AL_TRUE);
		offset[0] = 0;
		offset[1] = 0;

		double fade = 0, rise = 0;
		crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg1, h.redTurnoverMg2, masterGain*h.vintSwishFactor);
		alSourcef(source[1], AL_GAIN, fade);//0
		alSourcef(source[0], AL_GAIN, rise);//1

		alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);
		alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);//меняем pitch (дает нисходящую прямую при остановке второго дв)
	}
	//мг2дв <-> авт
	if (sr.reduktor_gl_obor > h.redTurnoverMg2)
	{
		filetoBuffer[1] = h.fullName["vint_swish_w_avt"];
		filetoBuffer[0] = h.fullName["vint_swish_w_mg"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		alSourcei(source[0], AL_LOOPING, AL_TRUE);
		offset[0] = 0;
		offset[1] = 0;

		double fade, rise;
		crossFade(&fade, &rise, sr.reduktor_gl_obor, h.redTurnoverMg2, h.redTurnoverAvt, masterGain*h.vintSwishFactor);
		alSourcef(source[0], AL_GAIN, fade);//
		alSourcef(source[1], AL_GAIN, rise);//

		alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg2);
		alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//меняем pitch (дает нисходящую прямую при остановке второго дв)
	}
	//мг1дв -> 0
	if (!sr.p_eng1_zap && !sr.p_eng2_zap && sr.reduktor_gl_obor > 0 && sr.reduktor_gl_obor < h.redTurnoverMg1 - 1)
	{
		filetoBuffer[1] = h.fullName["vint_swish_w"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		offset[1] = 0;

		alSourcef(source[0], AL_GAIN, 0);
		alSourcef(source[1], AL_GAIN, interpolation(h.redTurnoverMg1*0.69, 0, h.redTurnoverMg1, 1, sr.reduktor_gl_obor));//
		alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverMg1);
	}
	return 1;
}

Skv::Skv() : Sound(1, 3, 1)
{

}

int Skv::play(Helicopter h, SOUNDREAD sr)
{
	if (eq != "set")
	{
		alEffecti(effect[0], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
		alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
		alFilteri(filter[0], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
		alFilterf(filter[0], AL_LOWPASS_GAIN, 0);
		alSource3i(source[0], AL_AUXILIARY_SEND_FILTER, effectSlot[0], 0, 0);
		alSourcei(source[0], AL_DIRECT_FILTER, filter[0]);
		eq = "set";
	}

	if (sr.eng1_obor > sr.eng2_obor)
	{
		pitch = 0.029 * sr.eng1_obor - 1.484;
	}
	else
	{
		pitch = 0.029 * sr.eng2_obor - 1.484;
	}

	Sound::play(sr.p_skv_on, h.fullName["skv_on"], h.fullName["skv_w"], h.fullName["skv_off"], h.skvFactor);//Воспроизводим звук - записываем состояние звука в play

	//Набираем массив для рассчета усиления от среднего значения оборотов редуктора за 30с
	double averangeTurn = getAverange("redTurns", 25);
	/*double averangeTurn = 0;
	averangeCalcPeriod += deltaTime;
	if (averangeCalcPeriod >= 25 && !vector.empty())
		vector.erase(vector.begin());
	vector.push_back(sr.reduktor_gl_obor);
	for (auto& x : vector)
		averangeTurn += x / vector.size();*/

	double avrTurngain = (-5) + (sr.reduktor_gl_obor - averangeTurn) * 4;
	avrTurngain = (avrTurngain > 0) ? 0 : avrTurngain;

	double highFreqGain = pow(10, avrTurngain * 0.05);

	double highCutoffFreq = 4000;//ВЧ 4000-16000

	alEffectf(effect[0], AL_EQUALIZER_HIGH_CUTOFF, highCutoffFreq);

	alEffectf(effect[0], AL_EQUALIZER_HIGH_GAIN, highFreqGain);//

	alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)

	return 1;
}

Runway::Runway() : Sound(2, 2, 1)
{

}

int Runway::play(Helicopter h, SOUNDREAD sr)
{
	//Блок настройки эффекта эквалайзер
	//прямой выход заглушается, остается только звук прошедший через блок эквалайзера
	if (eq != "set")
	{
		alEffecti(effect[0], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
		alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
		alFilteri(filter[0], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
		alFilterf(filter[0], AL_LOWPASS_GAIN, 0);
		alSource3i(source[0], AL_AUXILIARY_SEND_FILTER, effectSlot[0], 0, 0);
		alSourcei(source[0], AL_DIRECT_FILTER, filter[0]);
		eq = "set";
	}

	for (size_t i = 0; i < 2; i++)
	{
		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);

		//Загружаем буферы и запускам источники
		if (fileBuffered[i] != filetoBuffer[i])
		{
			setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}
	}

	if (h.modelName == "mi_8_amtsh" || h.modelName == "mi_8_mtv5")
	{
		alEffectf(effect[0], AL_EQUALIZER_HIGH_CUTOFF, 4000);

		alEffectf(effect[0], AL_EQUALIZER_HIGH_GAIN, interpolation(11.2, 0.126, 14, 1, velocityX));//

		alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)

		filetoBuffer[1] = h.fullName["runway"];
		filetoBuffer[0] = h.fullName["landing"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		alSourcei(source[0], AL_LOOPING, AL_TRUE);

		alSourcef(source[1], AL_GAIN, interpolation(0, 0, 8.3, 1, 11.2, 0, abs(velocityX)) * !high * h.runwayFactor * 0.25/*Уменьшаем движение по полосе*/);//
																																													  //alSourcef(source[1], AL_GAIN, 0);//
		alSourcef(source[0], AL_GAIN, interpolation(8.3, 0, 11.2, 1, abs(velocityX)) * !high * h.runwayFactor * 0.854);//
	}
	else if (h.modelName == "mi_26")
	{
		filetoBuffer[1] = h.fullName["flapping"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		//78 -3 84 0 поправка на обороты редуктора
		//180гр - 14 с разворот - нужно усиление при развороте (12гр - 1с -> -3дб; 6гр - 1с -> -6дб) (убавить флаппинг на 3 дб)
		alSourcef(source[1], AL_GAIN, interpolation(0, 0, 8.3, 1, 14, 0, abs(velocityX)) * !high * interpolation(78, 0.71, 84, 1, sr.reduktor_gl_obor) * h.runwayFactor /** 0.25*//*Уменьшаем движение по полосе*/);//
																																													  //alSourcef(source[1], AL_GAIN, 0);//

	}

	return 1;
}

double attack(double velocityX, double velocityXPrevious, double tangaz, double deltaHigh, double periodCalc)
{
	double calcA = 0;
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

Sound::Sound() : sourceStatus(new int), source(new ALuint), buffer(new ALuint[3]), effectSlot(new ALuint), effect(new ALuint), filter(new ALuint)
{
	sourceStatus[0] = 0;
	try
	{
		if (sourcesInUse == maxSources)
			throw 256;
	}
	catch (int e)
	{
		cout << " [" << e << "] Sources exist...cant gen more sources...\n" << endl;
	}
	alGenSources(1, source.get());
	alGenEffects(1, effect.get());
	alGenBuffers(3, buffer.get());
	alGenFilters(1, filter.get());
	sourcesInUse++;
}

Sound::Sound(int sources, int buffers, int effectslots) : sourceStatus(new int[sources]), source(new ALuint[sources]), buffer(new ALuint[buffers]), effectSlot(new ALuint[effectslots]), effect(new ALuint[sources]), filter(new ALuint[sources])
{
	try
	{
		if (sourcesInUse == maxSources)
			throw 256;
		if (sourcesInUse == maxSlots)
			throw 16;
	}
	catch (int z)
	{
		switch (z)
		{
		case 256: cout << " Cant gen more sources...\n" << endl; break;
		case 16: cout << " Cant gen more aux slots...\n" << endl; break;
		}
	}
	sourceNumber = sources;
	bufferNumber = buffers;
	effectSlotNumber = effectslots;

	for (size_t i = 0; i < sourceNumber; i++)
	{
		sourceStatus[i] = 0;
	}

	if (sourcesInUse + sourceNumber > 256)
		cout << "Caution: only [" << sourcesInUse + sourceNumber - 256 << "] sources can be generated" << endl;
	alGenSources(sourceNumber, source.get());
	alGenEffects(sourceNumber, effect.get());
	alGenFilters(sourceNumber, filter.get());
	alGenBuffers(bufferNumber, buffer.get());
	alGenAuxiliaryEffectSlots(effectSlotNumber, effectSlot.get());

	sourcesInUse += sourceNumber;
	effectSlotsInUse += effectSlotNumber;
}

Sound::Sound(const Sound &copy) : Sound(copy.sourceNumber, copy.bufferNumber, copy.effectSlotNumber)
{
	load = copy.load;
	soundOn = copy.soundOn;
	soundWork = copy.soundWork;
	soundOff = copy.soundOff;
	offsetOn = copy.offsetOn;
	offsetOff = copy.offsetOff;
	lengthOn = copy.lengthOn;
	lengthOff = copy.lengthOff;
	pitch = copy.pitch;
	gain = copy.gain;
	channel = copy.channel;
	sourceNumber = copy.sourceNumber;
	bufferNumber = copy.bufferNumber;
	effectSlotNumber = copy.effectSlotNumber;
}

Sound::~Sound()
{
	alDeleteSources(sourceNumber, source.get());
	alDeleteEffects(sourceNumber, effect.get());
	alDeleteFilters(sourceNumber, filter.get());
	alDeleteBuffers(bufferNumber, buffer.get());
	alDeleteAuxiliaryEffectSlots(effectSlotNumber, effectSlot.get());

	sourcesInUse -= sourceNumber;
	effectSlotsInUse -= effectSlotNumber;
}

double Sound::getLengthWAV(string filename)
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
	fread(header, sizeof(WAVEHEADER), 1, in);//считываем заголовочную информацию
	double length = ((double)header->lDataSize / ((double)header->wfex.wBitsPerSample * (double)header->wfex.nSamplesPerSec)) * 8;//вычисляем длинну в секундах
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
		CloseHandle(hSnapshot);
		return true;
	}

	while (Process32Next(hSnapshot, &pe))
	{
		if (!_wcsicmp((wchar_t *)&pe.szExeFile, szExe))
		{
			CloseHandle(hSnapshot);
			return true;
		}
	}

	CloseHandle(hSnapshot);

	return false;
}