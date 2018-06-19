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
Пример:
\code{.cpp}
		../BASE/BASE.exe mi_8_mtv5
\endcode

</pre>
*/

/*! \page advanced Основные сведения
\tableofcontents
<pre>
Ниже будут пояснены все элементы программы, которые ,по мнению разработчика,
могут быть непонятны будущему разработчику или тестировщику
</pre>

\section sec0 Общая структура и диаграмма последовательности
<pre>
Данное ПО можно разбить на 4 части:
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
Внутри основной функции происходит инициализация аудио библиотеки: вывод звука устанавливается на устройство вывода по умолчанию,
определяются настройки «контекста» OpenAL в начале работы программы и деинициализация - в конце.
Основу кода функции составляет расчёт недостающих параметров ЛА,
которые не были получены по протоколу обмена и обработка объектов устройств: условия их инициализации,
выполнение метода «play», условия высвобождения объектов.
После инициализации «контекста» жизненный цикл объектов устройств прогоняется в бесконечном цикле.
\code{.cpp}
{AL_init}//Инициализация OpenAL
while(true)//Бесконечный цикл
{
	{Calc_par1}//рассчет параметров, например Атака, 1ые и 2ые производные
	{CAlc_par2}

	{easy_sound0}//Инициализация объекта - звука устройства, c универсальной логикой, например ПТС
	{easy_sound1}
	{easy_sound2}
	{hard_sound0}//Инициализация объекта - звука устройства, с уникальной логикой, например редуктор
	{easy_sound2}
	{hard_sound1}

	//Выход из цикла происходит только при закрытии приложения
}
{AL_deinit}//Деинициализация OpenAL
\endcode
</pre>

\subsection subSecCalculating Рассчет недостающих параметров
<pre>
К недостающим параметрам относятся:
•	Средние значения входящих параметров (средний шаг, средние обороты);
•	Первые и вторые производные (ускорение, рывок);
•	Специальные - разработанные для определенной задачи - параметры (атака)
Для рассчета среднего значения используется метод скользящего среднего:
значения в каждой точке определения равны среднему значению исходной функции за предыдущий период.
Для этого для каждого необходимого параметра собираются массивы значений за время Sound::globalWindow,
если необходимо значение среднего за меньший промежуток времени, то среднее расчитывается теоретически
(то есть берется часть собранных значений).
\see Sound::getAverange();

Набор массивов для расчета среднего:
\code{.cpp}
timerAvr += Sound::deltaTime;
//Если необходимый размер окна достигнут - выбрасываем значения в начале массива
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
//Кладем значения в конец массива каждую итерацию цикла
Sound::vectorAvrEng1Turn.push_back(localdata.eng1_obor);
Sound::vectorAvrEng2Turn.push_back(localdata.eng2_obor);
Sound::vectorAvrRedTurn.push_back(localdata.reduktor_gl_obor);
Sound::vectorAvrStep.push_back(Sound::step);
Sound::vectorAvrAtk.push_back(Sound::calcA);
\endcode

Для рассчета производных используются численные методы приближенного вычисления производной.
Так как будущее значение производной неизвестно вычиляется только значение левой производной( (Y_текущее - Y_прошлое)/(dt) ).
Где dt = Sound::window.
Пример расчета вертикальной скорости как производной от высоты:
\code{.cpp}
//Если не пришел признак остановки модели - вычисляем переменные
//Если необходимый размер окна достигнут - выбрасываем значения в начале массива
while (periodCalc >= window && Sound::vectorTime.size() > 2)
{
	if (!Sound::vectorTime.empty())
	{
		Sound::vectorHight.erase(Sound::vectorHight.begin());
		Sound::vectorTime.erase(Sound::vectorTime.begin());
	}
	periodCalc = Sound::currentTime - Sound::vectorTime.front();
}
//Собираем значения только если время изменяется
if (Sound::vectorTime.back() != Sound::currentTime)
{
	Sound::vectorTime.push_back(Sound::currentTime);
	Sound::vectorHight.push_back(Sound::hight);
}
//Если вектор времени не пуст - время изменяется, можно вычислить производные
if (!Sound::vectorTime.empty())
{
	//Производные
	periodCalc = Sound::currentTime - Sound::vectorTime.front();
	if (periodCalc > 0)
	{
		Sound::velocityY = (Sound::hight - Sound::vectorHight.front()) / periodCalc;
	}
}
\endcode
</pre>

\section sec2 Класс "Sound" и подклассы
<pre>
Данный класс и его наследники определяет, каким образом реагирует источник OpenAL на изменение входящих параметров.
Определение класса построено таким образом, что для объявления нового агрегата достаточно создать для него новый объект типа «Sound» и инициализировать функцию «play».
Конструктор класса по умолчанию создаст 3 буфера и 1 источник OpenAL, что достаточно для большинства описываемых устройств, и высвободит их в деструкторе, когда устройство завершит свою работу.
При инициализации функции «play» в нее передается признак работы устройства.
«1» соответствует включению устройства, которое по завершению переходит в фазу работы, «0» соответствует выключению.
Если объект инициализируется конструктором по умолчанию и его состояние определяется 1 параметром (включается – выключается),
то будем определять такой объект «простым» (этот параметр, однако, может быть результатом логических операций между несколькими параметрами).
Если объект инициализируется конструктором с конкретным количеством необходимых источников и буферов, а также объектов эффектов,
и его состояние определяется несколькими параметрами (высота, горизонтальная скорость, вертикальная скорость, ускорение, рывок, и т. д.),
то будем определять такой объект «сложным».
Как правило, для каждого «сложного» объекта создается наследник базового класса «Sound»,
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
Пример инициализации "Простого" устройства:
\code{.cpp}
Sound *pts;
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

\subsection subsec1 "Сложные" устройства
<pre>
Конечно, все устройства на борту ЛА можно определить, как «сложные»,
но в таком случае потребуются колоссальные вычислительные мощности.
Поэтому основное внимание при разработке комплекса уделяется наиболее важным устройствам и эффектам,
характерное звучание которых вносит основной вклад в формирование акустического образа ЛА.
К таким относятся: главный редуктор,
двигатели, звук хлопков винта, свист лопастей винта, движение по взлетной полосе.
Для установления корреляций между параметрами ЛА и звучанием устройств инженер-акустик оценивает спектр и
субъективное звучание аудио сигнала в различные моменты времени во время работы ЛА.
Пример инициализации "Сложного" устройства:
\code{.cpp}
Engine *eng[2];
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
\endcode
\sa
Reductor::play()
Engine::play()
VintFlap::play()

</pre>

\section sec3 Назначение "Helicopter"
<pre>
Секция описывает работу класса "Helicopter"
</pre>
\subsection subSecNames Определение путей
<pre>
Данный класс агрегирует имена файлов всех звуков, которые используются при загрузке буферов OpenAL.
После создания объекта данного класса вызывается метод класса, который принимает в качестве входного параметра конкретную модель ЛА,
устанавливая пути к конкретным файлам: %модель%/%имяфайла%.wav.
Пример работы метода для mi_26:
\code
setPath("mi_26")
{
	...
	//Вооружение
	shortName["sturm"] = "sturm.wav" --> fullName["sturm"] = ../mi_26/"sturm.wav"
	"s8.wav"						 -->					 ../mi_26/"s8.wav"
	"s13.wav"						 -->					 ../mi_26/"s13.wav"
	"rocket.wav"					 -->					 ../mi_26/"rocket.wav"
	"gun_loop.wav"					 -->					 ../mi_26/"gun_loop.wav"
	"shoot.wav"						 -->					 ../mi_26/"shoot.wav"
	"sturm.wav"						 -->					 ../mi_26/"sturm.wav"
	//Винт
	...
}
\endcode
</pre>

\subsection subSecFactors Определение состава устройств
<pre>
Кроме адресов файлов также определяются коэффициенты громкости звуков.
Коэффициент равный «0» означает, что звук данного агрегата или эффекта отсутствует на данном ЛА.
В таком случае алгоритм не исполняется вовсе, в отличие от воспроизведения файла с 0-вой громкостью,
что экономит память и процессорное время.
И наконец определяются некоторые константы необходимые для корректного вывода звука,
например: обороты двигателей и редуктора на различных режимах работы.
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
\sa
Helicopter

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

#include "BASE.h"

using namespace std;

#define MAX_AUX_SLOTS 16

SOUNDREAD soundread;/*!< Переменная класса soundread для хранения управляющих признаков */

int Sound::maxSlots;/*!< Переменная инициализирующаяся максимальным числом источников ,которые могут проигрываться одновременно*/
int Sound::maxSources;/*!< Переменная инициализирующаяся максимальным числом источников ,которые могут проигрываться одновременно*/
double Sound::masterGain = 1;//!< общая громкость звука
int Sound::sourcesInUse = 0;
int Sound::effectSlotsInUse = 0;
double Sound::currentTime = 0;
double Sound::deltaTime = 0;
double Sound::step = 0;
double Sound::tangaz = 0;
double Sound::derivTangaz = 0;
double Sound::hight = 0;
double Sound::velocityVectorXZ = 0;
double Sound::accelerationVectorXZ = 0;
double Sound::velocityY = 0;
double Sound::dashVectorXZ = 0;
double Sound::accelerationVy = 0;
double Sound::derivStep = 0;
double Sound::calcA = 0;
double Sound::redTurnAcc = 0;
double Sound::groundTouch = 0;
int Engine::engCount = 0;

vector<double> Sound::vectorVy;
vector<double> Sound::vectorVXZ;
vector<double> Sound::vectorAccXZ;
vector<double> Sound::vectorStep;
vector<double> Sound::vectorTangaz;
vector<double> Sound::vectorTime;
vector<double> Sound::vectorRedTurn;
vector<double> Sound::vectorAvrEng1Turn;
vector<double> Sound::vectorAvrEng2Turn;
vector<double> Sound::vectorAvrRedTurn;
vector<double> Sound::vectorAvrStep;
vector<double> Sound::vectorAvrAtk;
double Sound::globalWindow = 50;

AL_SOUND_CHANNELS Sound::channelsSetup = AL_SOUND_CHANNELS_6;//Конфигурация каналов звука
//channels[0];//front left
//channels[1];//front right
//channels[2];//front center
//channels[3];//low freq
//channels[4];//back left
//channels[5];//back right

/*!\brief Основная функция программы*/
int main(int argc, char *argv[])
{
	//Только 1на копия приложения может быть запущена одновременно
	HANDLE hMutex = OpenMutex(
		MUTEX_ALL_ACCESS, 0, L"BASE");

	if (!hMutex)
		// Mutex doesn’t exist. This is
		// the first instance so create
		// the mutex.
		hMutex =
		CreateMutex(0, 0, L"BASE");
	else
		// The mutex exists so this is the
		// the second instance so return.
	{
		cout << "Application already exist!" << endl;
		return 1;
	}

	//Получаем указатели на функции EFX
	setEFXPointers();
	vector <string> helicoptersNames = { "mi_8_mtv5","mi_8_amtsh","mi_26","mi_28","ka_226","ansat","ka_27","ka_29" };//Возможные комманды 
	string model = "ka_29";//по умолчанию
	Helicopter helicopter;//Переменная класса Helicopter для хранения параметров выбранного вертолета
	if (argc > 1)// если передаем аргументы, то argc будет больше 1(в зависимости от кол-ва аргументов)
	{
		for (size_t i = 0; i < helicoptersNames.size(); i++)
		{
			if (regex_match(argv[1], regex("^(" + helicoptersNames[i] + ")")))
			{
				model = argv[1];
			}
		}
	}

	helicopter.setParam(model);//Инициализируем параметры вертолета
	system("cls");
	cout << " Using " << helicopter.modelName << endl;

	ALCdevice *device;
	ALCcontext *context;

	ALint attribs[4] = { 0 };
	attribs[0] = ALC_MAX_AUXILIARY_SENDS;
	attribs[1] = MAX_AUX_SLOTS;

	Sound::maxSources = getMaxAvaliableSources(); //получаем максимальное количество источников которое можно использовать на данном устройстве
	cout << " " << Sound::maxSources << " - Sources avaliable" << endl;
	Sound::maxSlots = MAX_AUX_SLOTS;

	device = alcOpenDevice(0);//Инициализируем устройство
	if (device == 0)
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		return AL_FALSE;
	}
	context = alcCreateContext(device, attribs);//Инициализируем контекст (т.е. окружение - кабина вертолета)
	if (context == 0)
	{
		cout << alcGetString(device, alcGetError(device)) << endl;
		alcCloseDevice(device);
		return AL_FALSE;
	}
	if (!alcMakeContextCurrent(context))//Выбираем контекст текущим
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
	InitRealTime(1);//инициируется "реальное" время с задержкой в 1мс (чтение распределенной памяти раз в 0.001 с)

	//Указатели на объекты определяющие параметры выводимого звука
	Vsu *vsu = nullptr;
	Sound *vsuHp = nullptr;
	Crane *vsuKran = nullptr;
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
	Crane *perek1 = nullptr;
	Crane *perek2 = nullptr;
	Sound *ko50 = nullptr;
	Skv *skv = nullptr;
	Crane *fire1 = nullptr;
	Crane *fire2 = nullptr;
	Sound *consTank = nullptr;
	Sound *pstat = nullptr;
	Runway *runway = nullptr;
	Sound *crash = nullptr;
	VintSwish *vintSwish = nullptr;
	Sound *vintBrake = nullptr;
	VintFlap *vintFlap = nullptr;
	Sound *brake = nullptr;
	Sound *rain = nullptr;
	Sound *sturm = nullptr;
	Sound *igla = nullptr;
	Sound *rocket = nullptr;
	Sound *ppu = nullptr;
	Sound *upk = nullptr;
	Sound *nar8[50] = { nullptr };
	Sound *nar13[50] = { nullptr };
	Engine *eng[2] = { nullptr };
	Reductor *red = nullptr;
	Sound *beep = nullptr;
	Sound *undefined1 = nullptr;
	Crane *kranKolc = nullptr;
	Sound *vpryam = nullptr;
	Sound *vadd = nullptr;
	Sound *shock[4][50] = { nullptr };
	Sound *trim = nullptr;
	Sound *frict = nullptr;

	SOUNDREAD localdata = soundread;//локальная копия общего с USPO файла
	Sound::currentTime = localdata.time;
	Sound::vectorTime.push_back(Sound::currentTime);
	Sound::vectorVy.push_back(Sound::velocityY);
	Sound::vectorVXZ.push_back(Sound::velocityVectorXZ);
	Sound::vectorAccXZ.push_back(Sound::accelerationVectorXZ);
	Sound::vectorStep.push_back(Sound::step);
	Sound::vectorTangaz.push_back(Sound::tangaz);
	Sound::vectorRedTurn.push_back(localdata.reduktor_gl_obor);

	double timerPodk = 0;
	double timerNar8 = 0;//период выстрела нар8
	double timerNar13 = 0;//период выстрела нар8
	int counterNar8 = 0;
	int counterNar13 = 0;
	bool check8 = 0;
	bool check13 = 0;
	double timerAvr = 0;
	const double window = 1;//При вычислении приближенной производной берем изменение значения за секунду 
	double periodCalc = 0;//переменная для реального значения периода вычисления, равно или немного более window
	bool shockLocker[4] = { 0,0,0,0 };
	int counterShock[4] = { 0,0,0,0 };
	double avrDeltaTime = 0;
	vector<double> adt;

	//Резервируем память
	adt.reserve(600);
	Sound::vectorVy.reserve(1000);
	Sound::vectorVXZ.reserve(1000);
	Sound::vectorAccXZ.reserve(1000);
	Sound::vectorStep.reserve(1000);
	Sound::vectorTangaz.reserve(1000);
	Sound::vectorTime.reserve(1000);
	Sound::vectorRedTurn.reserve(20000);
	Sound::vectorAvrEng1Turn.reserve(20000);
	Sound::vectorAvrEng2Turn.reserve(20000);
	Sound::vectorAvrRedTurn.reserve(20000);
	Sound::vectorAvrStep.reserve(20000);
	Sound::vectorAvrAtk.reserve(20000);

	//Опрашиваем все блоки программы в бесконечном цикле
	while (true)
	{
		//Копируем данные из общей памяти во временное хранилище
		localdata = soundread;
		if (IsProcessPresent(L"USPO.exe") && !localdata.p_model_stop)//цикл работает пока uspo активно, и признак остановки модели не активен
		{
			/*cout << fixed
				<< " SOIU: " << Sound::sourcesInUse
				<< " ESIU: " << Sound::effectSlotsInUse
				<< " CUTI: " << Sound::currentTime
				<< "\t\t\r";*/

			//printf(" DT__: %.4lf\tENG1: %.3f\tENG2: %.3f\tRED_: %.3f\tVSU: %.3f\t\r", avrDeltaTime, soundread.eng1_obor, soundread.eng2_obor, soundread.reduktor_gl_obor, soundread.vsu_obor);

			if (Sound::currentTime == 0)
				Sound::currentTime = localdata.time;
			//Вычисляем изменение времени с прошлого цикла работы программы
			Sound::deltaTime = localdata.time - Sound::currentTime;

			//Среднее время цикла
			avrDeltaTime = 0;
			adt.push_back(Sound::deltaTime);
			if (adt.size() > 500)
			{
				adt.erase(adt.begin());
			}
			for (auto tg : adt)
			{
				avrDeltaTime += tg / adt.size();
			}

			Sound::currentTime = localdata.time;
			Sound::masterGain = localdata.master_gain;
			Sound::tangaz = localdata.tangaz;//тангаж (временно используем параметр инт осадков)
			Sound::velocityVectorXZ = sqrt(pow(localdata.v_atm_x, 2) + pow(localdata.v_atm_z, 2));//приборная скорость
			Sound::step = localdata.step; //шаг (временно используем параметр перегрузки)
			Sound::hight = localdata.hight;
			Sound::velocityY = localdata.vy;
			Sound::groundTouch = (localdata.obj_hv + localdata.obj_l + localdata.obj_nos + localdata.obj_r) / 4;//Признак касания

			//Если не пришел признак остановки модели - вычисляем переменные
			//Если необходимый размер окна достигнут - выбрасываем значения в начале массива
			while (periodCalc >= window && Sound::vectorTime.size() > 2)
			{
				if (!Sound::vectorTime.empty())
				{
					Sound::vectorVy.erase(Sound::vectorVy.begin());
					Sound::vectorVXZ.erase(Sound::vectorVXZ.begin());
					Sound::vectorAccXZ.erase(Sound::vectorAccXZ.begin());
					Sound::vectorTangaz.erase(Sound::vectorTangaz.begin());
					Sound::vectorStep.erase(Sound::vectorStep.begin());
					Sound::vectorTime.erase(Sound::vectorTime.begin());
					Sound::vectorRedTurn.erase(Sound::vectorRedTurn.begin());
				}
				periodCalc = Sound::currentTime - Sound::vectorTime.front();
			}
			//Собираем значения только если время изменяется
			if (Sound::vectorTime.back() != Sound::currentTime)
			{
				Sound::vectorTime.push_back(Sound::currentTime);
				Sound::vectorVy.push_back(Sound::velocityY);
				Sound::vectorVXZ.push_back(Sound::velocityVectorXZ);
				Sound::vectorAccXZ.push_back(Sound::accelerationVectorXZ);
				Sound::vectorTangaz.push_back(Sound::tangaz);
				Sound::vectorStep.push_back(Sound::step);
				Sound::vectorRedTurn.push_back(localdata.reduktor_gl_obor);
			}
			//Если вектор времени не пуст - время изменяется, можно вычилить производные
			if (!Sound::vectorTime.empty())
			{
				//Производные
				periodCalc = Sound::currentTime - Sound::vectorTime.front();
				if (periodCalc > 0)
				{
					Sound::accelerationVy = (Sound::velocityY - Sound::vectorVy.front()) / periodCalc;
					Sound::accelerationVectorXZ = (Sound::velocityVectorXZ - Sound::vectorVXZ.front()) / periodCalc;
					Sound::dashVectorXZ = (Sound::accelerationVectorXZ - Sound::vectorAccXZ.front()) / periodCalc;
					Sound::derivStep = (Sound::step - Sound::vectorStep.front()) / periodCalc;
					Sound::derivTangaz = (Sound::tangaz - Sound::vectorTangaz.front()) / periodCalc;
					Sound::calcA = attack(Sound::velocityVectorXZ, Sound::vectorVXZ.front(), Sound::tangaz, Sound::velocityY);
					Sound::redTurnAcc = (localdata.reduktor_gl_obor - Sound::vectorRedTurn.front()) / periodCalc;
				}
			}

			//Набираем массивы для среднего
			timerAvr += Sound::deltaTime;
			//Если необходимый размер окна достигнут - выбрасываем значения в начале массива
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
			//Кладем значения в конец массива
			Sound::vectorAvrEng1Turn.push_back(localdata.eng1_obor);
			Sound::vectorAvrEng2Turn.push_back(localdata.eng2_obor);
			Sound::vectorAvrRedTurn.push_back(localdata.reduktor_gl_obor);
			Sound::vectorAvrStep.push_back(Sound::step);
			Sound::vectorAvrAtk.push_back(Sound::calcA);

			//ВСУ
			if (helicopter.vsuFactor)//Если ВСУ включено в проект
			{
				if (localdata.vsu_obor > 0 || localdata.p_vsu_hp || localdata.p_vsu_ostanov || localdata.p_vsu_zap)
				{
					if (!vsu)
					{
						vsu = new Vsu;
					}
				}
				if (vsu)
				{
					//Поканальный вывод в задние динамики с номинальной громкостью
					//Мультипликатор "1" применяется непосредственно к сэмплам буфера, при загрузке
					if (helicopter.modelName == "mi_26")
					{
						vsu->channel[0] = 0;//FL
						vsu->channel[1] = 0;//FR
						vsu->channel[4] = 1;//BL
						vsu->channel[5] = 1;//BR
					}

					if (vsu->play(localdata, helicopter))
					{

					}
					else
					{
						Free(vsu);
					}
				}
			}
			//Аккумулятор
			if (helicopter.accumFactor)
			{
				if (helicopter.modelName != "ansat")//226
				{
					if (localdata.accumulator)//Условие создания объекта
					{
						if (!accum)//Если объект не создан 
						{
							accum = new Sound;//Создаем объект
						}
					}
					if (accum)//Если объект создан - используем его
					{
						if (accum->play(localdata.accumulator, "NULL", helicopter.fullName["accum"], "NULL", helicopter.accumFactor))
						{

						}
						else
						{
							Free(accum);//Удаляем объект
						}
					}
				}
				if (helicopter.modelName == "ka_226" || helicopter.modelName == "ansat")//226
				{
					if (localdata.accumulator)//Условие создания объекта
					{
						if (!accumTr)//Если объект не создан 
						{
							accumTr = new Sound;//Создаем объект
						}
					}
					if (accumTr)//Если объект создан - используем его
					{
						if (accumTr->play(localdata.accumulator, "NULL", helicopter.fullName["accum_tr"], "NULL", helicopter.accumFactor))
						{

						}
						else
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
					if (pts->play(localdata.p_pts, helicopter.fullName["pts_on"], helicopter.fullName["pts_w"], helicopter.fullName["pts_off"], helicopter.ptsFactor))
					{

					}
					else
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
					if (ptsTone->play(localdata.p_pts, "NULL", helicopter.fullName["pts_tone"], "NULL", helicopter.ptsToneFactor))
					{

					}
					else
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
					if (vpryam->play(localdata.p_vu1, "NULL", helicopter.fullName["vpryam"], "NULL", helicopter.vpryam))
					{

					}
					else
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
					if (tr36->play(localdata.p_trans_36_osn || localdata.p_trans_36_rez, helicopter.fullName["tr36_on"], helicopter.fullName["tr36_w"], helicopter.fullName["tr36_off"], helicopter.tr36Factor))
					{

					}
					else
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
					if (tr115->play(localdata.p_po500, helicopter.fullName["tr115_on"], helicopter.fullName["tr115_w"], helicopter.fullName["tr115_off"], helicopter.tr115Factor))
					{

					}
					else
					{
						Free(tr115);//Удаляем объект
					}
				}
			}
			//НИП
			if (helicopter.nipFactor)
			{
				if (localdata.ground_power_supply)//Условие создания объекта
				{
					if (!nip)//Если объект не создан 
					{
						nip = new Sound;//Создаем объект
					}
				}
				if (nip)//Если объект создан - используем его
				{
					if (nip->play(localdata.ground_power_supply, "NULL", helicopter.fullName["nip_tone"], "NULL", helicopter.nipFactor))
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

							nip->offset[0] += Sound::deltaTime;

							//_____  
							//  1  \____   
							//     0.985\  ______
							//		0.977\/
							//		 0.946
							//	    |1c |1c|

							//Сразу провал
							p2 = interpolation({ 0, 1 }, { 1.5, 0.946 }, { 2, 0.977 }, nip->offset[0]);

						}
						//Прокрутка и запуск ВСУ просаживают НИП по тону
						if (!localdata.p_vsu_hp && !localdata.p_vsu_zap)
						{
							nip->offset[0] = 0;
							p2 = 1;
						}

						for (size_t i = 0; i < 2; i++)
						{
							nip->pitch[i] = 1 * p1 * p2;//Вычисляем результирующий тон
						}
					}
					else
					{
						Free(nip);//Удаляем объект
					}
				}
			}
			//Гировертикаль
			if (helicopter.girovertFactor)
			{
				if (nip)//Условие создания объекта
				{
					if (!girovert)//Если объект не создан 
					{
						girovert = new Sound;//Создаем объект
					}
				}
				if (girovert)//Если объект создан - используем его
				{
					girovert->lengthOn = getLengthWAV(helicopter.fullName["girovert_on"]);
					if (girovert->pitch[girovert->id] < 1 && girovert->soundOn)
					{
						girovert->offset[girovert->id] = girovert->lengthOn * girovert->pitch[girovert->id];//Включаем запись с текущего уровня оборотов
					}

					if (girovert->play(nip, helicopter.fullName["girovert_on"], helicopter.fullName["girovert_w"], helicopter.fullName["girovert_w"], helicopter.ptsFactor))
					{
						if (girovert->soundOn)
						{
							girovert->pitch[girovert->id] = 1;
						}
						if (girovert->soundOff)
						{
							alSourcei(girovert->source[girovert->id], AL_LOOPING, AL_TRUE);
							girovert->pitch[girovert->id] -= Sound::deltaTime*0.0024;//уменьшаем питч по 0.0024 в сек

							//если pitch <= 0.8 - звук пропал, источник можно отключить
							if (girovert->pitch[girovert->id] <= 0.8)
							{
								if (!girovert->play(localdata.ground_power_supply, "NULL", "NULL", "NULL", helicopter.ptsFactor))
								{
									Free(girovert);//Удаляем объект
								}
							}
						}
					}
					else
					{
						Free(girovert);//Удаляем объект
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
						for (size_t i = 0; i < 2; i++)
						{
							if (localdata.p_eng1_zap | localdata.p_eng1_hp | localdata.p_eng2_hp | localdata.p_eng2_zap)
							{
								timerPodk += Sound::deltaTime;

								podk1->pitch[i] = interpolation({ 0, 1 }, { 1, 0.85 }, { 5, 0.88 }, timerPodk);

							}
							else
							{
								timerPodk = 0;
								podk1->pitch[0] = 1;
							}
						}

						if (podk1->play(localdata.p_nasos_podk_1, helicopter.fullName["podk_l_on"], helicopter.fullName["podk_l_w"], helicopter.fullName["podk_l_off"], helicopter.pumpLeftFactor))//Воспроизводим звук - записываем состояние звука в play
						{

						}
						else
						{
							Free(podk1);//Удаляем объект
						}
					}
					else
					{
						if (podk1->play(localdata.p_nasos_podk_1, helicopter.fullName["podk_l_on"], helicopter.fullName["podk_l_w"], "NULL", helicopter.pumpLeftFactor))
						{

						}
						else
						{
							Free(podk1);//Удаляем объект
						}
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
					podk2->channel[0] = 0;//R
					podk2->channel[1] = 1;
					if ("ka_226" == helicopter.modelName)
					{
						for (size_t i = 0; i < 2; i++)
						{
							if (localdata.p_eng1_zap | localdata.p_eng1_hp | localdata.p_eng2_hp | localdata.p_eng2_zap)
							{
								timerPodk += Sound::deltaTime;

								podk2->pitch[i] = interpolation({ 0, 1 }, { 1, 0.85 }, { 5, 0.88 }, timerPodk);

							}
							else
							{
								timerPodk = 0;
								podk2->pitch[0] = 1;
							}
						}

						if (podk2->play(localdata.p_nasos_podk_2, helicopter.fullName["podk_l_on"], helicopter.fullName["podk_l_w"], helicopter.fullName["podk_l_off"], helicopter.pumpRightFactor))//Воспроизводим звук - записываем состояние звука в play
						{

						}
						else
						{
							Free(podk2);//Удаляем объект
						}
					}
					else
					{
						if (podk2->play(localdata.p_nasos_podk_2, helicopter.fullName["podk_l_on"], helicopter.fullName["podk_l_w"], "NULL", helicopter.pumpRightFactor))
						{

						}
						else
						{
							Free(podk2);//Удаляем объект
						}
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
						kranKolc = new Crane;//Создаем объект
					}
				}
				if (kranKolc)//Если объект создан - используем его
				{
					kranKolc->channel[0] = 1;
					kranKolc->channel[1] = 1;

					if (kranKolc->play(localdata.p_kran_kolcev, helicopter.fullName["kran_circle"], "NULL", helicopter.fullName["kran_circle"], helicopter.circlingCraneFactor))
					{

					}
					else
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
						perek1 = new Crane;//Создаем объект
					}
				}
				if (perek1)//Если объект создан - используем его
				{
					perek1->channel[0] = 1;//L
					perek1->channel[1] = 0;
					if (perek1->play(localdata.p_kran_perekr_1, helicopter.fullName["perekr_l"], "NULL", helicopter.fullName["perekr_l"], helicopter.cutoffCraneFactor))
					{

					}
					else
					{
						Free(perek1);//Удаляем объект
					}
				}

				if (localdata.p_kran_perekr_2)//Условие создания объекта
				{
					if (!perek2)//Если объект не создан 
					{
						perek2 = new Crane;//Создаем объект
					}
				}
				if (perek2)//Если объект создан - используем его
				{
					perek2->channel[0] = 0;
					perek2->channel[1] = 1;//R
					if (perek2->play(localdata.p_kran_perekr_2, helicopter.fullName["perekr_r"], "NULL", helicopter.fullName["perekr_r"], helicopter.cutoffCraneFactor))
					{

					}
					else
					{
						Free(perek2);//Удаляем объект
					}
				}
			}
			//Если СКВ присутствует на Борту
			if (helicopter.skvFactor)
			{
				if (localdata.p_skv_on)//Условие создания объекта
					if (!skv)//Если объект не создан 
						skv = new Skv;//Создаем объект
				if (skv)//Если объект создан - используем его
				{
					if (skv->play(helicopter, localdata))
					{

					}
					else
					{
						Free(skv);//Удаляем объект
					}

				}
			}
			//Если КО-50 присутствует на Борту
			if (helicopter.ko50Factor)
			{
				if (localdata.stove)//Условие создания объекта
					if (!ko50)//Если объект не создан 
						ko50 = new Sound;//Создаем объект
				if (ko50)//Если объект создан - используем его
				{
					if (ko50->play(localdata.stove, helicopter.fullName["ko50_on"], helicopter.fullName["ko50_w"], helicopter.fullName["ko50_off"], helicopter.ko50Factor))
					{

					}
					else
					{
						Free(ko50);//Удаляем объект
					}
				}
			}
			//Если насос расходного бака
			if (helicopter.consumTankFactor)
			{
				if (localdata.dis_tank_pump)//Условие создания объекта
					if (!consTank)//Если объект не создан 
						consTank = new Sound;//Создаем объект
				if (consTank)//Если объект создан - используем его
				{
					if (consTank->play(localdata.dis_tank_pump, helicopter.fullName["cons_tank_on"], helicopter.fullName["cons_tank_w"], "NULL", helicopter.consumTankFactor))
					{

					}
					else
					{
						Free(consTank);//Удаляем объект
					}
				}
			}
			//Пожарный кран
			if (helicopter.fireCraneFactor)
			{
				if (localdata.p_kran_poj_l)//Условие создания объекта
					if (!fire1)//Если объект не создан 
						fire1 = new Crane;//Создаем объект
				if (fire1)//Если объект создан - используем его
				{
					fire1->channel[0] = 1;//L
					fire1->channel[1] = 0;
					if (fire1->play(localdata.p_kran_poj_l, helicopter.fullName["kran_fire_l"], "NULL", helicopter.fullName["kran_fire_l"], helicopter.fireCraneFactor))
					{

					}
					else
					{
						Free(fire1);//Удаляем объект
					}
				}

				if (localdata.p_kran_poj_r)//Условие создания объекта
					if (!fire2)//Если объект не создан 
						fire2 = new Crane;//Создаем объект
				if (fire2)//Если объект создан - используем его
				{
					fire2->channel[0] = 0;
					fire2->channel[1] = 1;//R
					if (fire2->play(localdata.p_kran_poj_r, helicopter.fullName["kran_fire_r"], "NULL", helicopter.fullName["kran_fire_r"], helicopter.fireCraneFactor))
					{

					}
					else
					{
						Free(fire2);//Удаляем объект
					}
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
					if (pstat->play(localdata.p_nasos, helicopter.fullName["pstat_on"], helicopter.fullName["pstat_w"], helicopter.fullName["pstat_off"], helicopter.ko50Factor))
					{

					}
					else
					{
						Free(pstat);//Удаляем объект
					}
				}
			}

			//Движение по ВПП и РД

			//Если звуки движения по ВПП включены в проект борта
			if (helicopter.runwayFactor)
			{
				if (localdata.v_surf_x != 0 && Sound::groundTouch > 0)//Условие создания объекта
					if (!runway)//Если объект не создан 
						runway = new Runway;//Создаем объект
				if (runway)//Если объект создан - используем его
				{
					runway->play(helicopter, localdata);//Воспроизводим звук - записываем состояние звука в play
					if (localdata.v_surf_x == 0 || !(Sound::groundTouch > 0))//Условие удаления объекта
						Free(runway);//Удаляем объект
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
					if (crash->play(localdata.p_crash, helicopter.fullName["crash"], "NULL", "NULL", 1))
					{

					}
					else
					{
						Free(crash);//Удаляем объект
					}
				}
			}

			//Винт
			if (helicopter.vintSwishFactor)
			{
				if (localdata.reduktor_gl_obor != 0)//Условие создания объекта
					if (!vintSwish)//Если объект не создан 
						vintSwish = new VintSwish;//Создаем объект
				if (vintSwish)//Если объект создан - используем его
				{
					vintSwish->play(helicopter, localdata);
					if (localdata.reduktor_gl_obor == 0)//Условие удаления объекта
						Free(vintSwish);//Удаляем объект
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
					if (vintBrake->play(localdata.tormoz_vint, helicopter.fullName["vint_torm"], helicopter.fullName["vint_torm"], helicopter.fullName["vint_torm"], helicopter.vintBrakeFactor))
					{
						double soundOff = getParameterFromVector(vector<point>{ { 12, 1 }, { 1, 0 } }, localdata.reduktor_gl_obor);
						for (size_t i = 0; i < 2; i++)
						{
							vintBrake->pitch[i] = localdata.reduktor_gl_obor / 12.0;
							if (vintBrake->soundOn)
							{
								vintBrake->gain[i] = ((vintBrake->offset[vintBrake->id] > 1) ? 1 : vintBrake->offset[vintBrake->id] / vintBrake->lengthOn) * soundOff;
							}
							else if (vintBrake->soundOff)
							{
								vintBrake->gain[i] = ((vintBrake->offset[vintBrake->id] > 1) ? 0 : 1 - (vintBrake->offset[vintBrake->id] / vintBrake->lengthOff)) * soundOff;
							}
							else
							{
								vintBrake->gain[i] = soundOff;
							}
						}
					}
					else
					{
						Free(vintBrake);//Удаляем объект
					}
				}
			}
			//Если звук хлопков винта включен в проект
			if (helicopter.vintFlapFactor)
			{
				if (Sound::groundTouch == 0)//Условие создания объекта
					if (!vintFlap)//Если объект не создан 
						vintFlap = new VintFlap;//Создаем объект
				if (vintFlap)//Если объект создан - используем его
				{
					vintFlap->play(helicopter, localdata);//Воспроизводим звук - записываем состояние звука в play
					if (Sound::groundTouch > 0.625)//Условие удаления объекта
						Free(vintFlap);//Удаляем объект
				}
			}
			//Если тормоза включены на борту
			if (helicopter.chassisBrakePumpFactor)
			{
				if (localdata.p_tormoz_press || localdata.frict)//Условие создания объекта
					if (!brake)//Если объект не создан 
						brake = new Sound;//Создаем объект
				if (brake)//Если объект создан - используем его
				{
					if (helicopter.modelName == "ka_27" | helicopter.modelName == "ka_29")
					{
						if (brake->play(localdata.p_tormoz_press || localdata.frict, "NULL", helicopter.fullName["brake"], "NULL", helicopter.chassisBrakePumpFactor))
						{
							brake->gain[brake->id] = localdata.tormoz;
							brake->pitch[brake->id] = getParameterFromVector(vector<point>{ { 5 * 0.277, 0.67 }, { 15 * 0.277, 1 }, { 25 * 0.277, 1.33 }, { 30 * 0.277, 1.4 }}, abs(localdata.v_surf_x));
						}
						else
						{
							Free(brake);//Удаляем объект
						}

					}
					else
					{
						if (brake->play(localdata.p_tormoz_press, helicopter.fullName["brake"], "NULL", helicopter.fullName["poff"], helicopter.chassisBrakePumpFactor))
						{

						}
						else
						{
							Free(brake);//Удаляем объект
						}
					}
				}
			}
			//Если триммер включен на борту
			if (helicopter.trim)
			{
				if (localdata.trim)//Условие создания объекта
					if (!trim)//Если объект не создан 
						trim = new Sound;//Создаем объект
				if (trim)//Если объект создан - используем его
				{
					if (trim->play(localdata.trim, helicopter.fullName["trim_on"], "NULL", helicopter.fullName["trim_off"], helicopter.trim))
					{

					}
					else
					{
						Free(trim);//Удаляем объект
					}
				}
			}

			////Если фрикцион включен на борту

			//if (helicopter.frict)
			//{
			//	if (localdata.frict)//Условие создания объекта
			//		if (!frict)//Если объект не создан 
			//			frict = new Sound;//Создаем объект
			//	if (frict)//Если объект создан - используем его
			//	{
			//		if (frict->play(localdata.frict, "NULL", helicopter.fullName["frict"], "NULL", helicopter.frict))
			//		{
			//		}
			//		else
			//		{
			//			Free(frict);//Удаляем объект
			//		}
			//	}
			//}

			//Дождь
			if (true)
			{
				if (localdata.p_rain)//Условие создания объекта
					if (!rain)//Если объект не создан 
						rain = new Sound;//Создаем объект
				if (rain)//Если объект создан - используем его
				{
					if (rain->play(localdata.p_rain, "NULL", helicopter.fullName["rain"], "NULL", helicopter.rainFactor))
					{

					}
					else
					{
						Free(rain);//Удаляем объект
					}
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
					if (rocket->play(localdata.p_rocket_hit, helicopter.fullName["rocket"], "NULL", "NULL", helicopter.rocketHitFactor))
					{

					}
					else
					{
						Free(rocket);//Удаляем объект
					}
				}
			}

			//Если НАР8 имеется на борту
			if (helicopter.rocketNar8Factor)
			{
				timerNar8 -= Sound::deltaTime;
				if (timerNar8 <= 0)
				{
					timerNar8 = 0;
					check8 = 0;
				}
				if (localdata.p_nar_s8)//Условие создания объекта
				{
					check8 = 1;
				}
				if (check8)//Условие создания объекта
				{
					if (!timerNar8)
					{
						if (!nar8[counterNar8])
						{
							nar8[counterNar8] = new Sound;//Создаем объект
							counterNar8++;
						}
						if (counterNar8 >= 50)
						{
							counterNar8 = 0;
						}
						timerNar8 = 0.125;
					}
				}
				for (int i = 0; i < 50; i++)
				{
					//воспроизводим звук выстрела 1го НАР8 каждые 0.125с
					if (nar8[i])
					{
						if (localdata.p_nar_s8 == 1)
						{
							nar8[i]->channel[0] = 1;//L
							nar8[i]->channel[1] = 0;
						}
						else if (localdata.p_nar_s8 == 2)
						{
							nar8[i]->channel[0] = 0;//R
							nar8[i]->channel[1] = 1;
						}
						else if (localdata.p_nar_s8 == 3)
						{
							nar8[i]->channel[0] = 1;//Both
							nar8[i]->channel[1] = 1;
						}

						nar8[i]->play(check8, helicopter.fullName["nar8"], "NULL", "NULL", helicopter.rocketNar8Factor);

						if (nar8[i]->sourceStatus[nar8[i]->id] != AL_PLAYING)
						{
							Free(nar8[i]);
						}
					}
				}
			}

			//Если НАР13 имеется на борту
			if (helicopter.rocketNar13Factor)
			{
				timerNar13 -= Sound::deltaTime;
				if (timerNar13 <= 0)
				{
					timerNar13 = 0;
					check13 = 0;
				}
				if (localdata.p_nar_s13)//Условие создания объекта
				{
					check13 = 1;
				}
				if (check13)//Условие создания объекта
				{
					if (!timerNar13)
					{
						if (!nar13[counterNar13])
						{
							nar13[counterNar13] = new Sound;//Создаем объект
							counterNar13++;
						}

						if (counterNar13 >= 50)
						{
							counterNar13 = 0;
						}
						timerNar13 = 0.15;
					}
				}
				for (int i = 0; i < 50; i++)
				{
					//воспроизводим звук выстрела 1го НАР13 каждые 0.15с
					if (nar13[i])
					{
						if (localdata.p_nar_s13 == 1)
						{
							nar13[i]->channel[0] = 1;//L
							nar13[i]->channel[1] = 0;
						}
						else if (localdata.p_nar_s13 == 2)
						{
							nar13[i]->channel[0] = 0;//R
							nar13[i]->channel[1] = 1;
						}
						else if (localdata.p_nar_s13 == 3)
						{
							nar13[i]->channel[0] = 1;//Both
							nar13[i]->channel[1] = 1;
						}
						nar13[i]->play(check13, helicopter.fullName["nar13"], "NULL", "NULL", helicopter.rocketNar13Factor);

						if (nar13[i]->sourceStatus[nar13[i]->id] != AL_PLAYING)
						{
							Free(nar13[i]);
						}
					}
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
					if (localdata.p_spo_ppu == 1)
					{
						ppu->channel[0] = 1;//L
						ppu->channel[1] = 0;
					}
					else if (localdata.p_spo_ppu == 2)
					{
						ppu->channel[0] = 0;//R
						ppu->channel[1] = 1;
					}
					else if (localdata.p_spo_ppu == 3)
					{
						ppu->channel[0] = 1;//centre
						ppu->channel[1] = 1;
					}
					if (ppu->play(localdata.p_spo_ppu, "NULL", helicopter.fullName["ppu"], "NULL", helicopter.ppuFactor))
					{

					}
					else
					{
						Free(ppu);//Удаляем объект
					}
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
					if (localdata.p_spo_upk == 1)
					{
						upk->channel[0] = 1;//L
						upk->channel[1] = 0;
					}
					else if (localdata.p_spo_upk == 2)
					{
						upk->channel[0] = 0;//R
						upk->channel[1] = 1;
					}
					else if (localdata.p_spo_upk == 3)
					{
						upk->channel[0] = 1;//centre
						upk->channel[1] = 1;
					}

					if (upk->play(localdata.p_spo_upk, "NULL", helicopter.fullName["upk"], "NULL", helicopter.upkFactor))
					{

					}
					else
					{
						Free(upk);//Удаляем объект
					}
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
					if (localdata.p_ur_ataka == 1)
					{
						sturm->channel[0] = 1;//L
						sturm->channel[1] = 0;
					}
					else if (localdata.p_ur_ataka == 2)
					{
						sturm->channel[0] = 0;//R
						sturm->channel[1] = 1;
					}
					else if (localdata.p_ur_ataka == 3)
					{
						sturm->channel[0] = 1;//centre
						sturm->channel[1] = 1;
					}

					if (sturm->play(localdata.p_ur_ataka, helicopter.fullName["sturm"], "NULL", "NULL", helicopter.rocketSturmFactor))
					{

					}
					else
					{
						Free(sturm);//Удаляем объект
					}
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
					if (localdata.p_ur_igla == 1)
					{
						igla->channel[0] = 1;//L
						igla->channel[1] = 0;
					}
					else if (localdata.p_ur_igla == 2)
					{
						igla->channel[0] = 0;//R
						igla->channel[1] = 1;
					}
					else if (localdata.p_ur_igla == 3)
					{
						igla->channel[0] = 1;//centre
						igla->channel[1] = 1;
					}
					if (igla->play(localdata.p_ur_igla, helicopter.fullName["igla"], "NULL", "NULL", helicopter.rocketIglaFactor))
					{

					}
					else
					{
						Free(igla);//Удаляем объект
					}
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
					if (redCrash->play(localdata.p_reduktor_gl_crash, "NULL", helicopter.fullName["red_crash"], "NULL", helicopter.redFactor))
					{
						redCrash->pitch[redCrash->id] = localdata.reduktor_gl_obor / helicopter.redTurnoverAvt / 2.0;
					}
					else
					{
						Free(redCrash);//Удаляем объект
					}
				}


				if (localdata.reduktor_gl_obor > 0)//условие создания объекта редуктора
					if (!red)//Если объект не создан 
						red = new Reductor;//Создаем объект
				if (red)//Если объект создан - используем его
				{
					red->channel[0] = 1;
					red->channel[1] = 1;
					red->play(helicopter, localdata);//
					if (red && localdata.reduktor_gl_obor == 0)//Условие удаления объекта
						Free(red);//Удаляем объект
				}

			}

			//Удар при проходе плит
			double shockInten[4] = {
					localdata.styk_l,
					localdata.styk_hv,
					localdata.styk_nos,
					localdata.styk_r };

			//Удар при проходе стыка плит
			if (helicopter.shock)
			{
				for (size_t i = 0; i < 4; i++)
				{
					if (shockInten[i])//Условие создания объекта
					{
						if (!shock[i][counterShock[i]] && !shockLocker[i])
						{
							shock[i][counterShock[i]] = new Sound;//Создаем объект
							shock[i][counterShock[i]]->gain[0] = shockInten[i];
							shock[i][counterShock[i]]->gain[1] = shockInten[i];
							counterShock[i]++;
							shockLocker[i] = 1;
						}
						if (counterShock[i] >= 50)
						{
							counterShock[i] = 0;
						}
					}
					else
					{
						shockLocker[i] = 0;
					}

					for (size_t j = 0; j < 50; j++)
					{
						if (shock[i][j])
						{
							if ((i + 1) % 2 == 0)
							{
								shock[i][j]->channel[0] = 0;//R
								shock[i][j]->channel[1] = 1;
							}
							else
							{
								shock[i][j]->channel[0] = 1;//L
								shock[i][j]->channel[1] = 0;
							}

							shock[i][j]->play(shockLocker[i], helicopter.fullName["shock"], "NULL", "NULL", helicopter.shock);

							if (shock[i][j]->sourceStatus[shock[i][j]->id] != AL_PLAYING)
							{
								Free(shock[i][j]);
							}
						}
					}
				}
			}

			//Скоростная добавка
			if (helicopter.vadd)
			{
				if (localdata.v_atm_x != 0)//Условие создания объекта
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
						if (abs(localdata.v_atm_x) < 60)
						{
							g = interpolation({ 0, -60 }, { 50, -14 }, { 60, -6 }, abs(localdata.v_atm_x));
						}
						else
						{
							g = interpolation({ 60, -6 }, { 70, -3 }, { 80, 0 }, abs(localdata.v_atm_x));
						}
					}
					else if (helicopter.modelName == "mi_8_amtsh" || helicopter.modelName == "mi_8_mtv5")
					{
						g = interpolation({ 0, -60 }, { 42, -18 }, { 70, 0 }, abs(localdata.v_atm_x));
					}
					else if (helicopter.modelName == "ka_29")
					{
						g = (69.4 - abs(localdata.v_atm_x)) * (-0.86);
					}
					else if (helicopter.modelName == "ka_27")
					{
						g = (69.4 - abs(localdata.v_atm_x)) * (-0.86);
					}
					/*else if (helicopter.modelName == "mi_26")
					{
						g = abs(localdata.v_atm_x) * 0.428 - 36;
					}*/
					else if (helicopter.modelName == "ka_226")
					{
						g = getParameterFromVector(vector<point>{ { 36.11111, -60 }, { 50, 0 } }, abs(localdata.v_atm_x));
					}
					else if (helicopter.modelName == "ansat")
					{
						g = getParameterFromVector(vector<point>{ { 28, -60 }, { 44.44, -15 }, { 56, -6 }, { 61.11, -1 }, { 70, 0 } }, abs(localdata.v_atm_x));
					}
					else
					{
						g = -60;
					}
					vadd->gain[vadd->id] = toCoef(g);

					vadd->play(localdata.v_atm_x != 0, "NULL", helicopter.fullName["pinkNoise"], "NULL", helicopter.vadd);
					if (localdata.v_atm_x == 0)//Условие удаления объекта
					{
						Free(vadd);//Удаляем объект
					}
				}
			}
			//Двигатель
			if (helicopter.engFactor)
			{
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
					if (engPomp[0]->play(localdata.p_eng1_pomp, helicopter.fullName["eng_pomp_on"], helicopter.fullName["eng_pomp_w"], "NULL", helicopter.engFactor))
					{

					}
					else
					{
						Free(engPomp[0]);//Удаляем объект
					}
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
					if (engPomp[1]->play(localdata.p_eng2_pomp, helicopter.fullName["eng_pomp_on"], helicopter.fullName["eng_pomp_w"], "NULL", helicopter.engFactor))
					{

					}
					else
					{
						Free(engPomp[1]);//Удаляем объект
					}
				}

				if (localdata.eng1_obor > 0)
				{
					if (!eng[0])//Если объект не создан 
					{
						eng[0] = new Engine;//Создаем объект
					}
				}
				if (eng[0])//Если объект создан - используем его
				{
					eng[0]->channel[0] = 1;//magic numbers//1//L
					eng[0]->channel[1] = -1;//-1
					eng[0]->play(localdata.p_eng1_zap, localdata.p_eng1_ostanov, localdata.p_eng1_hp, localdata.eng1_obor, helicopter);


					if (localdata.eng1_obor == 0)//Условие удаления объекта
						Free(eng[0]);//Удаляем объект
				}

				if (localdata.eng2_obor > 0)
				{
					if (!eng[1])//Если объект не создан 
					{
						eng[1] = new Engine;//Создаем объект
					}
				}
				if (eng[1])//Если объект создан - используем его
				{
					eng[1]->channel[0] = 0;//magic numbers//0
					eng[1]->channel[1] = 2;//2//R
					eng[1]->play(localdata.p_eng2_zap, localdata.p_eng2_ostanov, localdata.p_eng2_hp, localdata.eng2_obor, helicopter);


					if (localdata.eng2_obor == 0)//Условие удаления объекта
						Free(eng[1]);//Удаляем объект
				}
			}
			//Перекрывной кран ВСУ
			if (helicopter.vsuCraneFactor)
			{
				if (localdata.p_kran_perekr_vsu)//Условие создания объекта
					if (!vsuKran)//Если объект не создан 
						vsuKran = new Crane;//Создаем объект
				if (vsuKran)//Если объект создан - используем его
				{
					if (helicopter.modelName == "ka_27" | helicopter.modelName == "ka_29")
					{
						if (vsuKran->play(localdata.p_kran_perekr_vsu, helicopter.fullName["vsu_kran_on"], "NULL", helicopter.fullName["vsu_kran_on"], helicopter.vsuCraneFactor))
						{

						}
						else
						{
							Free(vsuKran);//Удаляем объект
						}
					}
					else
					{
						if (vsuKran->play(localdata.p_kran_perekr_vsu, helicopter.fullName["vsu_kran_on"], helicopter.fullName["vsu_kran_w"], helicopter.fullName["vsu_kran_on"], helicopter.vsuCraneFactor))
						{

						}
						else
						{
							Free(vsuKran);//Удаляем объект
						}
					}
				}
			}
			//Зуммер
			if (helicopter.buzzerFactor)
			{
				if (localdata.zoomer)//Условие создания объекта
					if (!beep)//Если объект не создан 
						beep = new Sound;//Создаем объект
				if (beep)//Если объект создан - используем его
				{
					if (beep->play(localdata.zoomer, "NULL", helicopter.fullName["beep"], "NULL", helicopter.buzzerFactor))
					{

					}
					else
					{
						Free(beep);//Удаляем объект
					}
				}
			}
			//Неопределенный 1
			if (helicopter.undefinedFactor)
			{
				if (helicopter.modelName == "mi_28")
				{
					if (localdata.undefined)//Условие создания объекта
						if (!undefined1)//Если объект не создан 
							undefined1 = new Sound;//Создаем объект
					if (undefined1)//Если объект создан - используем его
					{
						if (undefined1->play(localdata.undefined, "NULL", helicopter.fullName["undefined1_w"], "NULL", helicopter.undefinedFactor))
						{

						}
						else
						{
							Free(undefined1);//Удаляем объект
						}
					}
				}
				else if (helicopter.modelName == "ka_29")
				{
					if (localdata.undefined)//Условие создания объекта
						if (!undefined1)//Если объект не создан 
							undefined1 = new Sound;//Создаем объект
					if (undefined1)//Если объект создан - используем его
					{
						if (undefined1->play(localdata.undefined, helicopter.fullName["undefined1_on"], helicopter.fullName["undefined1_w"], "NULL", helicopter.undefinedFactor))
						{

						}
						else
						{
							Free(undefined1);//Удаляем объект
						}
					}
				}
				else if (helicopter.modelName == "ansat")
				{
					if (localdata.undefined)//Условие создания объекта
						if (!undefined1)//Если объект не создан 
							undefined1 = new Sound;//Создаем объект
					if (undefined1)//Если объект создан - используем его
					{
						if (undefined1->play(localdata.undefined, helicopter.fullName["undefined1_on"], helicopter.fullName["undefined1_w"], helicopter.fullName["undefined1_off"], helicopter.undefinedFactor))
						{

						}
						else
						{
							Free(undefined1);//Удаляем объект
						}
					}
				}
			}
		}
		else
		{
			//Убираем звук при зависании модели
			Sound::masterGain -= 0.01;
			Sound::masterGain = (Sound::masterGain < 0) ? 0 : Sound::masterGain;

			if (!Sound::vectorVXZ.empty())
			{
				Sound::vectorVXZ.clear();
			}
			if (!Sound::vectorAccXZ.empty())
			{
				Sound::vectorAccXZ.clear();
			}
			if (!Sound::vectorStep.empty())
			{
				Sound::vectorStep.clear();
			}
			if (!Sound::vectorTangaz.empty())
			{
				Sound::vectorTangaz.clear();
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

			for (size_t i = 0; i < 2; i++)
			{
				if (eng[i])
				{
					Free(eng[i]);
				}
			}
			if (red)
			{
				Free(red);
			}
			if (vintFlap)
			{
				Free(vintFlap);
			}
			if (vintSwish)
			{
				Free(vintSwish);
			}
			if (vadd)
			{
				Free(vadd);
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

int parametricalCrossfade(double *fadeGain, double *riseGain, double parameter, double fadeLimit, double riseLimit)
{
	if ((riseLimit > fadeLimit && parameter < fadeLimit) || (riseLimit < fadeLimit && parameter > fadeLimit))
	{
		*fadeGain = 1;
		*riseGain = 0;
	}
	if ((riseLimit > fadeLimit && parameter > riseLimit) || (riseLimit < fadeLimit && parameter < riseLimit))
	{
		*fadeGain = 0;
		*riseGain = 1;
	}
	if ((riseLimit > fadeLimit && parameter <= riseLimit && parameter >= fadeLimit) || (riseLimit < fadeLimit && parameter >= riseLimit && parameter <= fadeLimit))
	{
		double crossfader = getParameterFromVector(vector<point>{ {fadeLimit, -1}, { riseLimit, 1 }}, parameter);
		*fadeGain = sqrt(0.5*(1 - crossfader));
		*riseGain = sqrt(0.5*(1 + crossfader));
	}

	if (riseLimit > fadeLimit && parameter > riseLimit)
		return 1;//возвращаем 1 когда кроссфэйд закончен

	if (riseLimit < fadeLimit && parameter < riseLimit)
		return 1;

	return 0;//возвращаем 0 когда кроссфэйд не закончен
}

int timeCrossfade(double *fadeGain, double *riseGain, double crossFadeDuration, double timer)
{
	double crossfader = 0;
	if (crossFadeDuration == 0)
	{
		crossfader = 1;
	}
	else
	{
		crossfader = (timer * (2 / crossFadeDuration)) - 1;
	}
	crossfader = (crossfader > 1) ? 1 : crossfader;
	crossfader = (crossfader < -1) ? -1 : crossfader;
	*fadeGain = sqrt(0.5*(1 - crossfader));
	*riseGain = sqrt(0.5*(1 + crossfader));

	return 0;
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

int Sound::play(bool status, string pathOn, string pathW, string pathOff, double gainMult)
{
	bool start;	//Переменная для смены фазы на запуск
	bool work;	//Переменная для смены фазы на работу
	bool end;	//Переменная для смены фазы на выключение
	bool free;	//Переменная для смены фазы на высвобождение памяти

	//Узнаем длинну файлов запуска и остановки
	if (pathOn != "NULL")
		lengthOn = getLengthWAV(pathOn);
	if (pathOff != "NULL")
		lengthOff = getLengthWAV(pathOff);

	alGetSourcei(source[id], AL_SOURCE_STATE, &sourceStatus[id]);//Обновляем статус источника

	//условие запуска когда все звуки присутствуют
	if (pathOn != "NULL" & pathW != "NULL" & pathOff != "NULL")
	{
		start = status & !soundOn & !soundWork;
		work = status & soundOn & !soundWork & ((lengthOn - offset[id]) <= crossFadeDuration);
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[id] != AL_PLAYING;
	}
	//условие запуска когда отсутствует остановка
	if (pathOn != "NULL" & pathW != "NULL" & pathOff == "NULL")
	{
		start = status & !soundOn & !soundWork;
		work = status & soundOn & !soundWork & ((lengthOn - offset[id]) <= crossFadeDuration);
		end = 0;
		free = !status;
	}
	//условие запуска когда отсутствует работа
	if (pathOn != "NULL" & pathW == "NULL" & pathOff != "NULL")
	{
		start = status & !soundOn;
		work = 0;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[id] != AL_PLAYING;
	}
	//условие запуска когда отсутствует запуск
	if (pathOn == "NULL" & pathW != "NULL" & pathOff != "NULL")
	{
		start = 0;
		work = status & !soundWork;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[id] != AL_PLAYING;
	}
	//условие запуска когда отсутствует запуск и работа
	if (pathOn == "NULL" & pathW == "NULL" & pathOff != "NULL")
	{
		start = 0;
		work = 0;
		end = !status & !soundOff;
		free = !status & soundOff & sourceStatus[id] != AL_PLAYING;
	}
	//условие запуска когда отсутствует запуск и выключение
	if (pathOn == "NULL" & pathW != "NULL" & pathOff == "NULL")
	{
		start = 0;
		work = status & !soundWork;
		end = 0;
		free = !status;
	}
	//условие запуска когда отсутствует работа и выключение
	if (pathOn != "NULL" & pathW == "NULL" & pathOff == "NULL")
	{
		start = status & !soundOn;
		work = 0;
		end = 0;
		free = !status & sourceStatus[id] != AL_PLAYING;
	}
	//все 0
	if (pathOn == "NULL" & pathW == "NULL" & pathOff == "NULL")
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
		mode = "on";
	}
	//Работа (если pathW указывает на пустую область -> у агрегата отсутствует звук режима работы)
	if (work)
	{
		soundOn = 0;
		soundWork = 1;
		soundOff = 0;
		mode = "w";
	}
	//Выключение (если pathOff указывает на пустую область -> у агрегата отсутствует звук выключения)
	if (end)
	{
		soundOn = 0;
		soundWork = 0;
		soundOff = 1;
		mode = "off";
	}
	//Освобождение памяти
	if (free)
	{
		soundOn = 0;
		soundWork = 0;
		soundOff = 0;

		return 0; //Возвращаем 0 - признак окончания работы объекта
	}

	if (ModeSequence.back() != mode)//Если произошла смена режима
	{
		switcher = 0;//Обнуляем таймер кроссфеда
		id = !id;//Меняем номер активного источника
		if (mode == "on" || mode == "off")//Если текущий режим запуск или выключение - перезапускаем с нужного времени
		{
			fileBuffered[id] = "NULL";
		}
		ModeSequence.push_back(mode);//Сохраняем режим в историю
		if (ModeSequence.size() > 3)//Если если история режимов превышает 3 - очищаем лишние режимы
		{
			ModeSequence.erase(ModeSequence.begin());
		}
	}

	//Инициализируем переменные в зависимости от режима
	if (mode == "w")
	{
		filetoBuffer[id] = pathW;
		alSourcef(source[id], AL_LOOPING, AL_TRUE);
	}
	else if (mode == "on")
	{
		filetoBuffer[id] = pathOn;
		alSourcef(source[id], AL_LOOPING, AL_FALSE);
	}
	else if (mode == "off")
	{
		filetoBuffer[id] = pathOff;
		alSourcef(source[id], AL_LOOPING, AL_FALSE);
	}

	double finalGain = gain[id] * gainMult * masterGain;//Вычисляем максимальную громкость 
	double rise = 0;
	double fade = 0;
	switcher += deltaTime;
	timeCrossfade(&fade, &rise, crossFadeDuration, switcher);

	if (fileBuffered[id] == "NULL" && filetoBuffer[id] == "NULL")
	{
		rise = 0;
		fade = 1;
	}
	else if ((fileBuffered[!id] == "NULL" && filetoBuffer[!id] == "NULL") || pathW == "NULL")
	{
		rise = 1;
		fade = 0;
	}

	//Применяем результирующую громкость
	alSourcef(source[!id], AL_GAIN, fade * finalGain);
	alSourcef(source[id], AL_GAIN, rise * finalGain);

	//float a;
	//float b;
	//alGetSourcef(source[0], AL_GAIN, &a);
	//alGetSourcef(source[1], AL_GAIN, &b);
	//cout.precision(3);
	//cout << fixed
	//	<< " FADE: " << a
	//	<< " RISE: " << b
	//	<< " " << ModeSequence[0]
	//	<< " " << ModeSequence[1]
	//	<< " " << ModeSequence[2]
	//	<< "\t\t\r";

	/*static double p = 0;
	p += deltaTime;
	if (p >= 0.01)
	{
		FILE *f = fopen("cf.txt", "at");
		fprintf(f, "%.3lf\t%.3lf\t%.3lf\n", currentTime, a, b);
		p = 0;
		fclose(f);
	}*/

	for (size_t i = 0; i < 2; i++)
	{
		//Загружаем буферы и запускам источники
		if (fileBuffered[i] != filetoBuffer[i])
		{
			sourceStatus[i] = setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}

		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);

		//Обновляем высоту тона
		alSourcef(source[i], AL_PITCH, pitch[i]);
	}

	//Пока идет запуск - высчитываем точку остановки
	if (soundOn)
	{
		alGetSourcef(source[id], AL_SEC_OFFSET, &offset[id]);
		offsetOn = offset[id];
		if (offset[id] != 0)
		{
			offset[!id] = lengthOff * (1 - (offset[id] / lengthOn));
		}
	}
	//Пока идет остановка - высчитываем точку запуска
	if (soundOff)
	{
		alGetSourcef(source[id], AL_SEC_OFFSET, &offset[id]);
		offsetOff = offset[id];
		if (offset[id] != 0)
		{
			offset[!id] = lengthOn * (1 - (offset[id] / lengthOff));
		}
	}

	return 1;
}

int Sound::setBuffer(ALuint Buffer, string path, AL_SOUND_CHANNELS channelsCount, vector<double>&channels)
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
	alSourceStop(*Source);//Останавливаем воспроизведение
	alSourcei(*Source, AL_BUFFER, NULL);//Отсоединяем буфер
	alDeleteBuffers(1, &*Buffer);//Удаляем буфер
	alGenBuffers(1, &*Buffer);//Пересоздаем буфер
	if (file_path == "NULL")//Если путь равен "нулю" выходим
	{
		return 0;
	}
	if (!setBuffer(*Buffer, file_path, channelsSetup, channel))//Заполняем буфер
		return 0;
	alSourcei(*Source, AL_BUFFER, *Buffer);//подключаем буфер к источнику
	alSourcef(*Source, AL_SEC_OFFSET, offset);//Устанавливаем отступ в сек
	alSourcef(*Source, AL_GAIN, 0);//Устанавливаем громкость в 0
	alSourcePlay(*Source);//Запускаем вспроизведение
	alGetSourcei(*Source, AL_SOURCE_STATE, &play);//Получаем статус воспроизведения
	return play;//Возвращаем статус воспроизведения
}

double Sound::getAverange(vector<double> &vectorOfParameters, double seconds)
{
	double averange = 0;
	int size = vectorOfParameters.size();
	double window = size / globalWindow * seconds;
	for (size_t i = size - window; i < size; i++)
	{
		averange += vectorOfParameters[i] / window;
	}
	return averange;
}

Reductor::Reductor() : Sound(4, 4, 3)
{
	//Не самый умный ход менять конструктор по 
	//наибольшему количеству требуемых источников
	//при условии что редуктор просто вызывает конструктор Sound
	//можно писать так if(modelName == mi_26){reductor = new Sound(4,4,2)}else{reductor = new Sound(3,3,2)}
}

Reductor::~Reductor()
{
}

int Reductor::play(Helicopter &h, SOUNDREAD &sr)
{
	//0 -> мг1дв
	if (sr.reduktor_gl_obor < h.redTurnoverMg1 && (sr.p_eng1_zap | sr.p_eng2_zap) && !(mode == "mg1" && (sr.reduktor_gl_obor > h.redTurnoverMg1 * 0.9)))
	{
		if (h.redLengthOn - offsetOn <= crossFadeDuration)
		{
			mode = "mg1";
		}
		else
		{
			mode = "on";
		}
	}
	//мг1дв
	else if (sr.reduktor_gl_obor <= h.redTurnoverMg1 && !sr.p_eng1_zap && !sr.p_eng2_zap && !(sr.p_eng1_ostanov && sr.p_eng2_ostanov))
	{
		mode = "mg1";
	}
	//мг1дв <-> мг2дв
	else if (sr.reduktor_gl_obor > h.redTurnoverMg1 && sr.reduktor_gl_obor <= h.redTurnoverMg2 && !(sr.p_eng1_ostanov && sr.p_eng2_ostanov))
	{
		mode = "mg2";
	}
	//мг2дв <-> авт
	else if (sr.reduktor_gl_obor > h.redTurnoverMg2 && !sr.p_eng1_ostanov && !sr.p_eng2_ostanov)
	{
		mode = "avt";
	}
	//мг2дв -> 0
	else if (sr.p_eng1_ostanov && sr.p_eng2_ostanov && sr.reduktor_gl_obor > 0)
	{
		if (sr.reduktor_gl_obor > h.redTurnoverMg2)
		{
			mode = "avtOff";
		}
		else
		{
			mode = "off";
		}
	}

	if (ModeSequence.back() != mode)
	{
		switcher = 0;
		id = !id;
		if (mode == "on" || mode == "off")
		{
			fileBuffered[id] = "NULL";
		}
		ModeSequence.push_back(mode);
		if (ModeSequence.size() >= 4)
		{
			ModeSequence.erase(ModeSequence.begin());
		}
	}

	if (mode == "on")
	{
		filetoBuffer[id] = h.fullName["red_on_w"];
	}
	else if (mode == "mg1")
	{
		filetoBuffer[id] = h.fullName["red_w_w"];
	}
	else if (mode == "mg2")
	{
		filetoBuffer[id] = h.fullName["red_w_mg_w"];
		filetoBuffer[!id] = h.fullName["red_w_w"];
	}
	else if (mode == "avt")
	{
		filetoBuffer[id] = h.fullName["red_w_avt_w"];
		filetoBuffer[!id] = h.fullName["red_w_mg_w"];
	}
	else if (mode == "avtOff")
	{
		filetoBuffer[!id] = h.fullName["red_w_avt_w"];
		filetoBuffer[id] = h.fullName["red_w_mg_w"];
	}
	else if (mode == "off")
	{
		if (ModeSequence[1] == "avt")
		{
			fileBuffered[id] = h.fullName["red_w_mg_w"];
			ModeSequence[1] = "mg2";
			id = !id;
			filetoBuffer[id] = h.fullName["red_off_w"];
		}
		else
		{
			filetoBuffer[id] = h.fullName["red_off_w"];
		}
	}

	//28
	/*
	if (h.modelName == "mi_28")
	{
		//авт <-> авт полет
		if (sr.reduktor_gl_obor > h.redTurnoverAvt)
		{
			filetoBuffer[1] = h.fullName["red_w_avt_w"];
			filetoBuffer[0] = h.fullName["red_w_avt_fly"];
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			offset[0] = 0;
			offset[1] = 0;

			double fade = 0, rise = 0;
			parametricalCrossfade(&fade, &rise, step, 5.00, 5.01, masterGain*h.redFactor);
			alSourcef(source[1], AL_GAIN, fade);//
			alSourcef(source[0], AL_GAIN, rise);//

			alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);
			alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);//меняем pitch (дает нисходящую прямую при остановке второго дв)
		}
	}
	*/

	double finalGain = h.redFactor * masterGain;
	double rise = 0;
	double fade = 0;

	if (fileBuffered[!id] == h.fullName["red_on_w"] && fileBuffered[id] == h.fullName["red_w_w"])
	{
		if (reperSet != "set")
		{
			reperTurn = sr.reduktor_gl_obor;
			reperSet = "set";
		}

		double timeGain[2];
		double turnGain[2];

		if (sr.reduktor_gl_obor <= h.redTurnoverMg1)
		{
			parametricalCrossfade(&turnGain[!id], &turnGain[id], sr.reduktor_gl_obor, reperTurn, h.redTurnoverMg1);
		}
		else
		{
			turnGain[id] = 1;
			turnGain[!id] = 0;
		}

		if (offsetOn != 0)
		{
			timeCrossfade(&timeGain[!id], &timeGain[id], crossFadeDuration, crossFadeDuration - (h.redLengthOn - offsetOn));
		}
		else
		{
			timeGain[id] = 1;
			timeGain[!id] = 0;
		}

		rise = min(timeGain[id], turnGain[id]);
		fade = max(timeGain[!id], turnGain[!id]);
	}
	else if (fileBuffered[id] == h.fullName["red_on_w"] || fileBuffered[id] == h.fullName["red_off_w"] || fileBuffered[!id] == h.fullName["red_on_w"] || fileBuffered[!id] == h.fullName["red_off_w"])
	{
		switcher += deltaTime;
		timeCrossfade(&fade, &rise, crossFadeDuration, switcher);
	}
	else
	{
		rise = 1;
		fade = 1;
	}

	//глушим редуктор на хп двигателей
	double reductorHpRestrict = 0;
	if (sr.p_eng1_zap || sr.p_eng2_zap)
	{
		reductorHpRestrict = 1;
	}
	else
	{
		reductorHpRestrict = getParameterFromVector(vector<point>{ { 10, 0 }, { 12, 1 }}, sr.reduktor_gl_obor);
	}

	alSourcef(source[id], AL_GAIN, gain[id] * rise * finalGain * reductorHpRestrict);
	alSourcef(source[id], AL_PITCH, pitch[id]);
	alSourcef(source[!id], AL_GAIN, gain[!id] * fade * finalGain * reductorHpRestrict);
	alSourcef(source[!id], AL_PITCH, pitch[!id]);

	/*string modes = "[" + ModeSequence[0] + " " + ModeSequence[1] + " " + ModeSequence[2] + "]";
	cout.precision(3);
	cout << fixed
		<< " ID__: " << id
		<< " gain[id]: " << gain[id] * rise * finalGain
		<< " gain[!id]: " << gain[!id] * fade * finalGain
		<< " pitch[id]: " << pitch[id]
		<< " pitch[!id]: " << pitch[!id]
		<< " rise: " << rise
		<< " fade: " << fade
		<< " FIB0: " << fileBuffered[0]
		<< " FIB1: " << fileBuffered[1]
		<< "\t\t\r";*/

		/*static double period = 0;
				period += deltaTime;
				if (period>=0.05)
				{
					FILE *f = fopen("abr.txt", "at");
					fprintf(f, "%lf\t%lf\t%lf\t%lf\t%s\t%s\t%s\n", gain[id] * rise * finalGain, gain[!id] * fade * finalGain,pitch[id],pitch[!id], fileBuffered[0].data(), fileBuffered[1].data(), mode.data());
					fclose(f);
					period = 0;
			}*/

	for (size_t i = 0; i < 2; i++)
	{
		//При оборотах редуктора = 0 и незапущенных двигателях, останавиваем источники
		if (sr.reduktor_gl_obor == 0 && !sr.p_eng1_zap && !sr.p_eng2_zap)
		{
			alSourceStop(source[i]);
			alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);		// перезабили признак 
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

		if (filetoBuffer[i] == h.fullName["red_on_w"])
		{
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
			offset[i] = getParameterFromVector(h.redFunctionOnSwap, sr.reduktor_gl_obor);
			crossFadeDuration = 0.5;
		}
		else if (filetoBuffer[i] == h.fullName["red_w_w"])
		{
			alSourcei(source[i], AL_LOOPING, AL_TRUE);
			offset[i] = 0;
		}
		else if (filetoBuffer[i] == h.fullName["red_w_mg_w"])
		{
			alSourcei(source[i], AL_LOOPING, AL_TRUE);
			offset[i] = 0;
		}
		else if (filetoBuffer[i] == h.fullName["red_w_avt_w"])
		{
			alSourcei(source[i], AL_LOOPING, AL_TRUE);
			offset[i] = 0;
		}
		else if (filetoBuffer[i] == h.fullName["red_off_w"])
		{
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
			offset[i] = getParameterFromVector(h.redFunctionOffSwap, sr.reduktor_gl_obor);
			crossFadeDuration = 1;
		}

		//Загружаем буферы и запускам источники
		if (fileBuffered[i] != filetoBuffer[i])
		{
			setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}

		if (fileBuffered[i] == h.fullName["red_on_w"])
		{
			alGetSourcef(source[i], AL_SEC_OFFSET, &offsetOn);
			pitch[i] = sr.reduktor_gl_obor / roundFloat(getParameterFromVector(h.redFunctionOn, offsetOn), 0.001);
			gain[i] = 1;
		}
		else if (fileBuffered[i] == h.fullName["red_w_w"])
		{
			pitch[i] = roundFloat((sr.reduktor_gl_obor / h.redTurnoverMg1), 0.001);
			gain[i] = getParameterFromVector(vector<point>{ {h.redTurnoverMg1, 1}, { h.redTurnoverMg2, 0 } }, sr.reduktor_gl_obor);
		}
		else if (fileBuffered[i] == h.fullName["red_w_mg_w"])
		{
			pitch[i] = roundFloat((sr.reduktor_gl_obor / h.redTurnoverMg2), 0.001);
			gain[i] = getParameterFromVector(vector<point>{ {h.redTurnoverMg1, 0}, { h.redTurnoverMg2, 1 }, { h.redTurnoverAvt, 0 } }, sr.reduktor_gl_obor);
		}
		else if (fileBuffered[i] == h.fullName["red_w_avt_w"])
		{
			pitch[i] = roundFloat((sr.reduktor_gl_obor / h.redTurnoverAvt), 0.001);
			gain[i] = getParameterFromVector(vector<point>{ {h.redTurnoverMg2, 0}, { h.redTurnoverAvt, 1 } }, sr.reduktor_gl_obor);
		}
		else if (fileBuffered[i] == h.fullName["red_off_w"])
		{
			alGetSourcef(source[i], AL_SEC_OFFSET, &offsetOff);
			pitch[i] = sr.reduktor_gl_obor / roundFloat(getParameterFromVector(h.redFunctionOff, offsetOff), 0.001);
			gain[i] = 1;
		}
		if (fileBuffered[id] == "NULL")
		{
			gain[id] = 0;
		}

		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);
	}

	double lowFreqGain = AL_EQUALIZER_DEFAULT_LOW_GAIN;
	double mid1FreqGain = AL_EQUALIZER_DEFAULT_MID1_GAIN;
	double mid2FreqGain = AL_EQUALIZER_DEFAULT_MID2_GAIN;
	double highFreqGain = AL_EQUALIZER_DEFAULT_HIGH_GAIN;

	double lowCutoffFreq = AL_EQUALIZER_DEFAULT_LOW_CUTOFF;//НЧ 50-800
	double mid1CutoffFreq = AL_EQUALIZER_DEFAULT_MID1_CENTER;//купол 1 200-3000
	double mid2CutoffFreq = AL_EQUALIZER_DEFAULT_MID2_CENTER;//купол 2 1000-8000
	double highCutoffFreq = AL_EQUALIZER_DEFAULT_HIGH_CUTOFF;//ВЧ 4000-16000

	double avrTurnRestrict = getParameterFromVector(vector<point>{ { 0, 0 }, { 0.5, 1 }}, hight);

	//Полеты ми 28
	if (h.modelName == "mi_28")
	{
		//Вычисляем средние обороты за последние 30с
		double averangeTurn = getAverange(vectorAvrRedTurn, 30);

		//Усиление по атаке
		double atkXvel = calcA * interpolation({ 0, 0 }, { 16.67, 1 }, abs(velocityVectorXZ));

		double averangeAtk = getAverange(vectorAvrAtk, 20) * interpolation({ 0, 0 }, { 16.67, 1 }, abs(velocityVectorXZ));

		double atkGain = (atkXvel - averangeAtk) * -0.4;
		atkGain = (atkGain < -2) ? -2 : atkGain;
		atkGain = (atkGain > 3) ? 3 : atkGain;

		//Общее усиление от скорости 
		double velocityGain = 0;
		if (velocityVectorXZ < 70)
		{
			velocityGain = interpolation({ 20, 0 }, { 50, 2 }, { 70, 5 }, velocityVectorXZ) / 5;
		}
		else
		{
			velocityGain = (velocityVectorXZ  * 0.15 - 5.5) / 5;
		}

		//Общее усиление от шага
		double stepGain = 0;
		if (step < 16)
		{
			stepGain = interpolation({ 0, 0 }, { 11, 3 }, { 16, 5 }, step) / 3;
		}
		else
		{
			stepGain = (step * 0.4 - 1.4) / 3;
		}

		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75 * avrTurnRestrict;

		//усиление НЧ когда нет хлопков на границе 2го условия
		if (abs(velocityVectorXZ) <= 16.67) //ниже 60ти висение
		{
			if (vectorVXZ.size() > 2)
			{
				if (vectorVXZ.at(vectorVXZ.size() - 2) > 16.67) //предыдущая точка была не на висении - не висение
				{
					hovering = 0;
				}
				else //предыдущая точка была на висениии - висение
				{
					if (abs(accelerationVectorXZ) < 0.56) // условие висение возникает когда ускорение падает ниже условия
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
		if (((velocityVectorXZ < 0 && accelerationVectorXZ > 0.56) || (velocityVectorXZ > 0 && accelerationVectorXZ < -0.56)) && abs(velocityVectorXZ) <= 16.67 /*&& velocityY < 4*/)
		{
			flapCGain = ((abs(accelerationVectorXZ) - 0.56) * 4) * interpolation({ -0.25, 0 }, { 0, 0.5 }, { 0.25, 1 }, velocityY) * hovering;//переходит в усиление нч по vy
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
	else if (h.modelName == "mi_8_amtsh" || h.modelName == "mi_8_mtv5")
	{
		//Вычисляем средние обороты за последние 30с
		double averangeTurn = getAverange(vectorAvrRedTurn, 30);

		//усиление от оборотов выше 10000
		double highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1.5 * avrTurnRestrict;
		highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;

		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75 * avrTurnRestrict;

		//Общее усиление от скорости выше 28м/с
		double velocityGain = (abs(velocityVectorXZ) >= 28) ? (abs(velocityVectorXZ) - 28)* 0.1 : 0;//0.1дб на 1 м/с

		//Вычисляем средний шаг за 35с
		double averangeStep = getAverange(vectorAvrStep, 35);

		//Усиление от шага
		double stepGain = 0.75 * (step - averangeStep) * interpolation({ 0, 0 }, { 1, 1 }, hight);//

		//усиление по шагу в НЧ
		double mid2FreqStepGain = step * 0.6 * interpolation({ 0, 1 }, { 10, 0 }, hight);

		//Висение
		//усиление НЧ когда нет хлопков на границе 2го условия
		if (abs(velocityVectorXZ) <= 16.67) //ниже 60ти висение
		{
			if (vectorVXZ.size() > 2)
			{
				if (vectorVXZ.at(vectorVXZ.size() - 2) > 16.67) //предыдущая точка была не на висении - не висение
				{
					hovering = 0;
				}
				else //предыдущая точка была на висениии - висение
				{
					if (abs(accelerationVectorXZ) < 0.56) // условие висение возникает когда ускорение падает ниже условия
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
		if (((velocityVectorXZ < 0 && accelerationVectorXZ > 0.56) || (velocityVectorXZ > 0 && accelerationVectorXZ < -0.56)) && abs(velocityVectorXZ) <= 16.67 /*&& velocityY < 4*/)
		{
			flapCGain = ((abs(accelerationVectorXZ) - 0.56) * 4) * interpolation({ -0.25, 0 }, { 0.25, 1 }, velocityY) * hovering;//переходит в усиление нч по vy
			flapCGain = (flapCGain > 4) ? 4 : flapCGain;
		}

		//Страгивание
		//Усиление редуктора в НЧ в начале движения по ВПП
		double stalkingGain = (accelerationVectorXZ > 0) ? accelerationVectorXZ * 5 * interpolation({ 0, 1 }, { 8.3, 0 }, velocityVectorXZ) * !hight : 0;

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
	else if (h.modelName == "ka_29")
	{
		//добавляем отдельный звук взлета
		if (takeOff != h.fullName["takeOff"])
		{
			setAndDeploySound(&buffer[2], &source[2], 0, h.fullName["takeOff"]);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);
			takeOff = h.fullName["takeOff"];
		}

		double takeOffGain = toCoef(min(getParameterFromVector(vector<point>{ { 0, -12 }, { 8, -5 }, { 16, 0 }}, step),
			getParameterFromVector(vector<point>{ { 0, 0 }/*, { 4, -2.5 }*/, { 8, -12 } }, hight)))
			* getParameterFromVector(vector<point>{ { 0, 0 }, { h.redTurnoverAvt, 1 } }, sr.reduktor_gl_obor);

		alSourcef(source[2], AL_GAIN, sm.delay(takeOffGain, deltaTime) * masterGain);

		/*float toff;
		alGetSourcef(source[2], AL_GAIN, &toff);
		cout.precision(3);
		cout << fixed
			<< " TOFD: " << toff
			<< " TOFG: " << takeOffGain
			<< "\t\t\r";*/

			//Набираем массив для рассчета усиления от среднего значения оборотов редуктора за 30с
		double averangeTurn = getAverange(vectorAvrRedTurn, 30);

		//Общее усиление от скорости выше 50м/с
		double velocityGain = (abs(velocityVectorXZ) >= 50) ? (abs(velocityVectorXZ) - 50)* 0.2 : 0;//0.1дб на 1 м/с

		//Набираем массив для рассчета усиления от среднего значения шага за 50с
		double averangeStep = getAverange(vectorAvrStep, 50);

		//Рассчитываем усиление от среднего
		double stepGain = (step - averangeStep) * getParameterFromVector(vector<point>{ { 0, 0 }, { 1, 1 } }, hight);

		//усиление по шагу в НЧ
		//double mid2FreqStepGain = step * interpolation(0, 1, 5, 0, hight);

		//усиление по шагу в Средних чатотах
		double absStepGain = step * getParameterFromVector(vector<point>{ { 0, 1 }, { 10.5, 0.5 }, { 27.78, 0 } }, abs(velocityVectorXZ));

		//усиление от оборотов выше 10000
		double highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1 * avrTurnRestrict;
		highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;
		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75 * avrTurnRestrict;

		//усиление НЧ когда нет хлопков на границе 2го условия
		if (abs(velocityVectorXZ) <= 16.67) //ниже 60ти висение
		{
			if (vectorVXZ.size() > 2)
			{
				if (vectorVXZ.at(vectorVXZ.size() - 2) > 16.67) //предыдущая точка была не на висении - не висение
				{
					hovering = 0;
				}
				else //предыдущая точка была на висениии - висение
				{
					if (abs(accelerationVectorXZ) < 0.56) // условие висение возникает когда ускорение падает ниже условия
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
		if (((velocityVectorXZ < 0 && accelerationVectorXZ > 0.56) || (velocityVectorXZ > 0 && accelerationVectorXZ < -0.56)) && abs(velocityVectorXZ) <= 16.67 /*&& velocityY < 4*/)
		{
			flapCGain = ((abs(accelerationVectorXZ) - 0.56) * 4) * interpolation({ -0.25, 0 }, { 0, 0.5 }, { 0.25, 1 }, velocityY) * hovering;//переходит в усиление нч по vy
			flapCGain = (flapCGain > 4) ? 4 : flapCGain;
		}

		lowFreqGain = toCoef(turnGain + stepGain * 0.15 + absStepGain * 0.1 /*+ mid2FreqStepGain * 0.3*/ + flapCGain + velocityGain); //0.15 -> 0.15
		mid1FreqGain = toCoef(turnGain + stepGain * 0.2 + absStepGain * 0.1 /*+ mid2FreqStepGain * 0.2*/ + flapCGain);//0.3 -> 0.2
		mid2FreqGain = toCoef(turnGain + stepGain * 0.3 + absStepGain * 0.1 + velocityGain * 0.75);//0.4 -> 0.3
		highFreqGain = toCoef(turnGain + stepGain * 0.5 + absStepGain * 0.3 + highFreqTurnGain);//

		lowFreqGain = (lowFreqGain <= 1) ? 1 : lowFreqGain;
		mid1FreqGain = (mid1FreqGain <= 1) ? 1 : mid1FreqGain;
		mid2FreqGain = (mid2FreqGain <= 1) ? 1 : mid2FreqGain;
		highFreqGain = (highFreqGain <= 1) ? 1 : highFreqGain;

		lowCutoffFreq = 400;//НЧ 50-800
		mid1CutoffFreq = 1000;//купол 1 200-3000
		mid2CutoffFreq = 3000;//купол 2 1000-8000
		highCutoffFreq = 10000;//ВЧ 4000-16000

		/*float a;
		float b;
		alGetSourcef(source[0], AL_GAIN, &a);
		alGetSourcef(source[1], AL_GAIN, &b);
		float pich;
		alGetSourcef(source[id], AL_PITCH, &pich);
		cout.precision(3);
		cout << fixed
			<< " FADE: " << a
			<< " RISE: " << b
			<< " " << ModeSequence[0]
			<< " " << ModeSequence[1]
			<< " " << ModeSequence[2]
			<< " STGN: " << stepGain
			<< " ABSG: " << absStepGain
			<< " AVRS: " << averangeStep
			<< "\t\t\r";*/
	}
	//Полеты ка 27
	else if (h.modelName == "ka_27")
	{
		//добавляем отдельный звук взлета
		if (takeOff != h.fullName["takeOff"])
		{
			setAndDeploySound(&buffer[2], &source[2], 0, h.fullName["takeOff"]);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);
			takeOff = h.fullName["takeOff"];
		}

		double takeOffGain = toCoef(min(getParameterFromVector(vector<point>{ { 0, -12 }, { 8, -5 }, { 16, 0 }}, step),//db
			getParameterFromVector(vector<point>{ { 0, 0 }/*, { 4, -2.5 }*/, { 8, -12 } }, hight)))//db
			* getParameterFromVector(vector<point>{ { 0, 0 }, { h.redTurnoverAvt, 1 } }, sr.reduktor_gl_obor);//coef

		alSourcef(source[2], AL_GAIN, sm.delay(takeOffGain, deltaTime) * masterGain);

		//Набираем массив для рассчета усиления от среднего значения оборотов редуктора за 30с
		double averangeTurn = getAverange(vectorAvrRedTurn, 30);

		//усиление от скорости выше 50км/ч (14м/c)
		double velocityGain = 0;
		if (abs(velocityVectorXZ) < 42)
		{
			velocityGain = interpolation({ 14, 0 }, { 42, 3 }, abs(velocityVectorXZ));
		}
		else if (abs(velocityVectorXZ) >= 42 && abs(velocityVectorXZ) < 56)
		{
			velocityGain = interpolation({ 42, 3 }, { 56, 7 }, abs(velocityVectorXZ));
		}
		else
		{
			velocityGain = 7 + (abs(velocityVectorXZ) - 56) * 0.071;
			velocityGain = (velocityGain > 9) ? 9 : velocityGain;
		}

		//Набираем массив для рассчета усиления от среднего значения шага за 50с
		double averangeStep = getAverange(vectorAvrStep, 50);

		//Рассчитываем усиление от среднего
		double stepGain = (step - averangeStep) * interpolation({ 0, 0 }, { 1, 1 }, hight);

		//усиление по шагу в НЧ
		double mid2FreqStepGain = step * interpolation({ 0, 1 }, { 5, 0 }, hight);

		//усиление по шагу в Средних чатотах
		double absStepGain = step * interpolation({ 0, 1 }, { 10.5, 0.5 }, { 27.78, 0 }, abs(velocityVectorXZ));

		//усиление от оборотов выше 10000
		double highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1 * avrTurnRestrict;
		highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;
		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75 * avrTurnRestrict;

		//усиление НЧ когда нет хлопков на границе 2го условия
		if (abs(velocityVectorXZ) <= 16.67) //ниже 60ти висение
		{
			if (vectorVXZ.size() > 2)
			{
				if (vectorVXZ.at(vectorVXZ.size() - 2) > 16.67) //предыдущая точка была не на висении - не висение
				{
					hovering = 0;
				}
				else //предыдущая точка была на висениии - висение
				{
					if (abs(accelerationVectorXZ) < 0.56) // условие висение возникает когда ускорение падает ниже условия
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
		if (((velocityVectorXZ < 0 && accelerationVectorXZ > 0.56) || (velocityVectorXZ > 0 && accelerationVectorXZ < -0.56)) && abs(velocityVectorXZ) <= 16.67 /*&& velocityY < 4*/)
		{
			flapCGain = ((abs(accelerationVectorXZ) - 0.56) * 4) * interpolation({ -0.25 , 0 }, { 0, 0.5 }, { 0.25, 1 }, velocityY) * hovering;//переходит в усиление нч по vy
			flapCGain = (flapCGain > 4) ? 4 : flapCGain;
		}

		lowFreqGain = pow(10, (turnGain + stepGain * 0.15 + absStepGain * 0.1 /*+ mid2FreqStepGain * 0.3*/ + flapCGain + velocityGain) * 0.05); //0.15 -> 0.15
		mid1FreqGain = pow(10, (turnGain + stepGain * 0.2 + absStepGain * 0.1 /*+ mid2FreqStepGain * 0.2*/ + flapCGain) * 0.05);//0.3 -> 0.2
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
	else if (h.modelName == "mi_26")
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
		double beatsGain = pow(10, (getParameterFromVector(vector<point>{ { 60, -40 }, { 70, -12 }, { 78, -8 }, { 90, -2 }}, sr.reduktor_gl_obor)) * 0.05);

		gain[2] = beatsGain * masterGain;

		alSourcef(source[2], AL_GAIN, gain[2]);

		//добавляем отдельный звук взлета
		if (takeOff != h.fullName["takeOff"])
		{
			setAndDeploySound(&buffer[3], &source[3], 0, h.fullName["takeOff"]);
			alSourcei(source[3], AL_LOOPING, AL_TRUE);
			takeOff = h.fullName["takeOff"];
		}

		double takeOffGain = toCoef(min(getParameterFromVector(vector<point>{ { 0, -30 }, { 6, 0 } }, step),
			getParameterFromVector(vector<point>{ { 0, 0 }/*, { 4, -2.5 }*/, { 10, -30 } }, hight)))
			* getParameterFromVector(vector<point>{ { 0, 0 }, { h.redTurnoverAvt, 1 } }, sr.reduktor_gl_obor);

		gain[3] = sm.delay(takeOffGain, deltaTime) * masterGain;

		alSourcef(source[3], AL_GAIN, gain[3]);

		double averangeTurn = getAverange(vectorAvrRedTurn, 30);

		//усиление от оборотов выше 10000
		double highFreqTurnGain = (sr.reduktor_gl_obor - averangeTurn) * 1.5 * avrTurnRestrict;
		highFreqTurnGain = (highFreqTurnGain > 3) ? 3 : highFreqTurnGain;

		//усиление от оборотов
		double turnGain = (sr.reduktor_gl_obor - averangeTurn) * 0.75 * avrTurnRestrict;

		double averangeStep = getAverange(vectorAvrStep, 20);

		//Усиление от шага
		double stepGain = (step - averangeStep) * interpolation({ 0, 0 }, { 1, 1 }, hight);

		//усиление по шагу в НЧ
		//double mid2FreqStepGain = step * 1 * interpolation(0, 1, 10, 0, hight);

		//Усиление по Vy
		double vyG = (velocityY * (-2) - 10) - 4.43/*коэф 0.6*/;
		vyG = (vyG > 0) ? 0 : vyG;
		double velocityYGain = vyG * interpolation({ 0, 1 }, { 22.22, 0 }, abs(velocityVectorXZ)) * ((velocityY < 0) ? 1 : 0);

		//Страгивание
		//Усиление редуктора в НЧ в начале движения по ВПП
		double stalkingGain = (accelerationVectorXZ > 0) ? accelerationVectorXZ * 5 * interpolation({ 0, 1 }, { 8.3, 0 }, velocityVectorXZ) * (groundTouch > 0) : 0;

		lowFreqGain = pow(10, (velocityYGain + stepGain * 0.25 + stalkingGain /*+ mid2FreqStepGain*/) * 0.05);
		mid1FreqGain = pow(10, (turnGain + stepGain * 1 + stalkingGain /*+ mid2FreqStepGain*/) * 0.05);
		mid2FreqGain = pow(10, (turnGain + stepGain * 1) * 0.05);
		highFreqGain = pow(10, (turnGain + stepGain * 1 + highFreqTurnGain) * 0.05);

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
	//Полеты ка 226
	else if (h.modelName == "ka_226")
	{
		//добавляем отдельную гармонику редуктора
		if (beats != h.fullName["red_harm"])
		{
			setAndDeploySound(&buffer[2], &source[2], 0, h.fullName["red_harm"]);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);
			beats = h.fullName["red_harm"];

			alEffecti(effect[2], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
			alAuxiliaryEffectSloti(effectSlot[2], AL_EFFECTSLOT_EFFECT, effect[2]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
			alFilteri(filter[2], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[2], AL_LOWPASS_GAIN, 0);
			alSource3i(source[2], AL_AUXILIARY_SEND_FILTER, effectSlot[2], 0, NULL);
			alSourcei(source[2], AL_DIRECT_FILTER, filter[2]);
		}

		//громкость гармоники
		double harmGain = getParameterFromVector(vector<point>{ { 0, -40 }, { 5, -15 }, { 9, 0 }}, step);
		alSourcef(source[2], AL_GAIN, toCoef(harmGain) * 0.56 * masterGain);

		//высота тона гармоники 0.008(5) -> 1% Оборота редуктора
		double harmPitch = sr.reduktor_gl_obor / h.redTurnoverAvt /*1 + (sr.reduktor_gl_obor - h.redTurnoverAvt) * 0.0085556*/;
		alSourcef(source[2], AL_PITCH, harmPitch);

		//
		double avrEngTurns = (sr.eng1_obor > sr.eng2_obor) ? sr.eng1_obor - getAverange(vectorAvrEng1Turn, 2) : sr.eng2_obor - getAverange(vectorAvrEng2Turn, 2);

		//усиление 1го купола
		double harmMid1Gain = getParameterFromVector(vector<point>{ { 9, 0 }, { 10, 6 }, { 12, 12 }}, step) * getParameterFromVector(vector<point>{ { 0, 1.6 }, { 50, 0.7 }}, velocityVectorXZ);

		//усиление 2го купола
		double harmMid2Gain = getParameterFromVector(vector<point>{ { -2, -12 }, { -1, -6 }, { 0, 0 }, { 1, 6 }, { 2, 12 }}, avrEngTurns);

		//Усиление гармоники
		alEffectf(effect[2], AL_EQUALIZER_MID1_CENTER, 500);
		alEffectf(effect[2], AL_EQUALIZER_MID2_CENTER, 4300);

		alEffectf(effect[2], AL_EQUALIZER_MID1_WIDTH, 0.33);
		alEffectf(effect[2], AL_EQUALIZER_MID2_WIDTH, 0.33);//сделай 0.33

		alEffectf(effect[2], AL_EQUALIZER_MID1_GAIN, pow(10, (harmMid1Gain)*0.05));//
		alEffectf(effect[2], AL_EQUALIZER_MID2_GAIN, pow(10, (harmMid2Gain)*0.05));//

		alAuxiliaryEffectSloti(effectSlot[2], AL_EFFECTSLOT_EFFECT, effect[2]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)

		//усиление по шагу в ВЧ
		double highFreqStepGain = getParameterFromVector(vector<point>{ { 0, 0 }, { 5, 2 }, { 9, 3 }, { 12, 5 } }, step);

		//усиление по шагу в НЧ (1) (призма)
		double lowFreqStepGain1 = getParameterFromVector(vector<point>{ { 0, 0 }, { 9, 15 }, { 16, 26.66 } }, step) * getParameterFromVector(vector<point>{ { 0, 1 }, { 8, 0 } }, hight);

		//усиление по шагу в НЧ (2)
		double lowFreqStepGain2 = getParameterFromVector(vector<point>{ { 0, 0 }, { 12, 14 } }, step) * getParameterFromVector(vector<point>{ { 0, 0 }, { 28, 1 } }, abs(velocityVectorXZ));

		//Усиление на висении
		double hoveringGain = (hight > 0) ? abs(accelerationVectorXZ) * 6 * getParameterFromVector(vector<point>{ { 0, 1 }, { 28, 0 } }, velocityVectorXZ) : 0;

		//
		if (velocityY < 0)
		{
			flapOn += deltaTime;
			flapOn = (flapOn > 1) ? 1 : flapOn;//плавно наводим громкость за 1с
		}
		else
		{
			flapOn -= deltaTime;
			flapOn = (flapOn < 0) ? 0 : flapOn;
		}

		//0.1 -> 2дб
		double lowFreqAccGain = (accelerationVectorXZ <= -0.56) ? (((abs(accelerationVectorXZ) - 0.56) * 20) * getParameterFromVector(vector<point>{ { 4, 1 }, { 9, 0 } }, step) * getParameterFromVector(vector<point>{ { 8, 0 }, { 16, 1 } }, hight) * flapOn) : 0;

		lowFreqGain = toCoef(lowFreqStepGain1 + lowFreqStepGain2 + hoveringGain + lowFreqAccGain);
		highFreqGain = toCoef(highFreqStepGain);

		lowFreqGain = (lowFreqGain <= 1) ? 1 : lowFreqGain;
		highFreqGain = (highFreqGain <= 1) ? 1 : highFreqGain;

		lowCutoffFreq = pow(10, getParameterFromVector(vector<point>{ { 8, log10(80) }, { 16, log10(120) } }, hight));//НЧ 50-800
		highCutoffFreq = 4000;//ВЧ 4000-16000
	}
	//Полеты ансат
	else if (h.modelName == "ansat")
	{
		//добавляем отдельную гармонику бумбума
		if (beats != h.fullName["BumBum"])
		{
			setAndDeploySound(&buffer[2], &source[2], 0, h.fullName["BumBum"]);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);
			beats = h.fullName["BumBum"];
		}
		//добавляем отдельный звук взлета
		if (takeOff != h.fullName["takeOff"])
		{
			setAndDeploySound(&buffer[3], &source[3], 0, h.fullName["takeOff"]);
			alSourcei(source[3], AL_LOOPING, AL_TRUE);
			takeOff = h.fullName["takeOff"];
		}

		double takeOffStep = toCoef(getParameterFromVector(vector<point>{ { 0, -60 }, { 7, -18 }, { 60, 0 }}, step))
			* getParameterFromVector(vector<point>{ { 0, 1 }, { 11.11, 0 } }, abs(velocityVectorXZ))
			* getParameterFromVector(vector<point>{ { 0, 1 }, { 8, 0 } }, hight);

		double takeOffVelY = toCoef(getParameterFromVector(vector<point>{ { -9, -3 }, { -4, -15 }}, velocityY))
			* getParameterFromVector(vector<point>{ { 5, 0 }, { 10, 1 }}, calcA)
			* getParameterFromVector(vector<point>{ { 34, 1 }, { 37, 0.5 }, { 40, 0 }}, step);

		double takeOffGain = max(takeOffStep, takeOffVelY);

		alSourcef(source[3], AL_GAIN, sm.delay(takeOffGain, deltaTime) * masterGain);

		//Громкость бумбума
		double bumBumGain = toCoef(getParameterFromVector(vector<point>{ { -15, 0 }, { -10, -6 }, { -7, -15 }, { -5, -20 }, { 0, -60 }}, calcA))
			* getParameterFromVector(vector<point>{ { 8.3, 0 }, { 14, 1 }}, abs(velocityVectorXZ));

		alSourcef(source[2], AL_GAIN, bumBumGain * masterGain);
		//
		double avrEngTurns = (sr.eng1_obor > sr.eng2_obor) ? sr.eng1_obor - getAverange(vectorAvrEng1Turn, 2) : sr.eng2_obor - getAverange(vectorAvrEng2Turn, 2);

		//усиление 2го купола
		double mid2FreqGainEngTurns = getParameterFromVector(vector<point>{ { -2, -8 }, { -1, -4 }, { 0, 0 }, { 2, 4 }}, avrEngTurns);

		double trueStep = (step < 7) ? 0 : step - 7;//7->0 шаг // очень странные единицы шага у ансата

		double lowFreqGainStep = trueStep * 0.0755;

		double lowFreqGainVelX = getParameterFromVector(vector<point>{ { 33.33, 0 }, { 56, 6 }, { 70, 3 }}, abs(velocityVectorXZ));

		double mid2GainStep = trueStep * 0.13;

		double mid2FreqGainVelX = getParameterFromVector(vector<point>{ { 33.33, 0 }, { 44.44, 6 }, { 47.2, 6 }, { 56, 3 }}, abs(velocityVectorXZ));

		double highFreqGainVelX = getParameterFromVector(vector<point>{ { 44.44, 0 }, { 56, 4 }, { 70, 6 }}, abs(velocityVectorXZ));

		//НЧ во время треска винта
		double lowFreqCrunchGain = getParameterFromVector(vector<point>{ { 5, 0 }, { 10, 6 }}, calcA)
			* getParameterFromVector(vector<point>{ { 5.55, 0 }, { 11.11, 1 }, { 19.4, 1 }, { 25, 0 }}, abs(velocityVectorXZ))
			* getParameterFromVector(vector<point>{ { 34, 1 }, { 37, 0.5 }, { 40, 0 }}, step)
			* getParameterFromVector(vector<point>{ { -1.3, 0 }, { -0.7, 1 }}, velocityY);

		//НЧ треск на висении
		double lowFreqCrunchHoveringGain = getParameterFromVector(vector<point>{ { 4, 0 }, { 6, 4 }}, tangaz)
			* getParameterFromVector(vector<point>{ { 0, 1 }, { 8.3, 0 }}, abs(velocityVectorXZ));

		lowFreqGain = toCoef(lowFreqGainStep + lowFreqGainVelX + lowFreqCrunchGain + lowFreqCrunchHoveringGain);
		mid2FreqGain = toCoef(mid2GainStep + mid2FreqGainVelX + mid2FreqGainEngTurns);
		highFreqGain = toCoef(highFreqGainVelX);

		alEffectf(effect[0], AL_EQUALIZER_MID2_WIDTH, 0.3);//Ширина купола в октавах
		alEffectf(effect[1], AL_EQUALIZER_MID2_WIDTH, 0.3);//

		lowCutoffFreq = 120;//НЧ 50-800
		mid2CutoffFreq = 3700;//купол 2 1000-8000
		highCutoffFreq = 4000;//ВЧ 4000-16000
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

int Engine::play(bool status_on, bool status_off, bool status_hp, double parameter, Helicopter &h)
{
	if (status_hp && mode != "mg")
	{
		if (h.engLengthHpOn - offsetOn <= crossFadeDuration)
		{
			mode = "mg";
		}
		else
		{
			mode = "hp";
		}
	}
	else if (status_on && mode != "mg")
	{
		if (h.engLengthOn - offsetOn <= crossFadeDuration)
		{
			mode = "mg";
		}
		else
		{
			mode = "on";
		}
	}
	else if (status_off)
	{
		if (parameter <= h.engTurnoverMg)
		{
			mode = "off";
		}
		else
		{
			mode = "avtOff";
		}
	}
	else if (!status_on && !status_off && !status_hp)
	{
		if (parameter <= h.engTurnoverMg)
		{
			mode = "mg";
		}
		else
		{
			mode = "avt";
		}
	}

	if (ModeSequence.back() != mode)
	{
		switcher = 0;
		id = !id;
		if (mode == "on" || mode == "off" || mode == "hp")
		{
			fileBuffered[id] = "NULL";
		}
		ModeSequence.push_back(mode);
		if (ModeSequence.size() >= 4)
		{
			ModeSequence.erase(ModeSequence.begin());
		}
	}

	bool modeHp = 0;
	for (size_t i = 0; i < ModeSequence.size(); i++)
	{
		if (ModeSequence[i] == "hp")
		{
			modeHp = 1;
		}
	}
	bool modeOn = !modeHp;

	//0 -> мг
	if (mode == "on")
	{
		filetoBuffer[id] = h.fullName["eng_on_w"];
	}
	//0 -> хп
	else if (mode == "hp")
	{
		filetoBuffer[id] = h.fullName["eng_on_hp_w"];
	}
	//мг || хп
	else if (mode == "mg")
	{
		if (modeOn)
		{
			filetoBuffer[id] = h.fullName["eng_w_w"];
		}
		else if (modeHp)
		{
			filetoBuffer[id] = h.fullName["eng_w_hp_w"];
		}
	}
	//мг <-> авт
	else if (mode == "avt")
	{
		if (h.modelName == "ansat")
		{
			filetoBuffer[id] = h.fullName["eng_w_w"];
		}
		else
		{
			filetoBuffer[id] = h.fullName["eng_w_avt_w"];
			filetoBuffer[!id] = h.fullName["eng_w_w"];
		}
	}
	//мг -> 0 || хп -> 0
	else if (mode == "off")
	{
		if (modeOn)
		{
			//если каким-то неимоверным образом промежуточный этап
			//был пропущен и после автомата последовало выключение
			if (ModeSequence[1] == "avt")
			{
				fileBuffered[id] = h.fullName["eng_w_w"];
				ModeSequence[1] = "mg";
				id = !id;
				filetoBuffer[id] = h.fullName["eng_off_w"];
			}
			else
			{
				filetoBuffer[id] = h.fullName["eng_off_w"];
			}
		}
		else if (modeHp)
		{
			filetoBuffer[id] = h.fullName["eng_off_hp_w"];
		}
	}
	//авт -> 0
	else if (mode == "avtOff")
	{
		filetoBuffer[!id] = h.fullName["eng_w_avt_w"];
		filetoBuffer[id] = h.fullName["eng_w_w"];
	}

	double finalGain = 0;
	if (modeOn)
	{
		finalGain = masterGain * h.engFactor;
	}
	else if (modeHp)
	{
		finalGain = masterGain * h.engHpFactor;
	}

	if (fileBuffered[id] == h.fullName["eng_w_avt_w"])
	{
		gain[id] = getParameterFromVector(vector<point>{ {h.engTurnoverMg, 0}, { h.engTurnoverAvt, 1 } }, parameter);
		gain[!id] = 1 - gain[id];
	}
	else if (fileBuffered[!id] == h.fullName["eng_w_avt_w"])
	{
		gain[id] = getParameterFromVector(vector<point>{ {h.engTurnoverMg, 1}, { h.engTurnoverAvt, 0 } }, parameter);
		gain[!id] = 1 - gain[id];
	}
	else if (fileBuffered[!id] == h.fullName["eng_on_w"] && fileBuffered[id] == h.fullName["eng_w_w"])
	{
		if (reperSet != "set")
		{
			reperTurn = parameter;
			reperSet = "set";
		}

		double timeGain[2];
		double turnGain[2];

		if (parameter <= h.engTurnoverMg)
		{
			parametricalCrossfade(&turnGain[!id], &turnGain[id], parameter, reperTurn, h.engTurnoverMg);
		}
		else
		{
			turnGain[id] = 1;
			turnGain[!id] = 0;
		}

		if (offsetOn != 0)
		{
			timeCrossfade(&timeGain[!id], &timeGain[id], crossFadeDuration, crossFadeDuration - (h.engLengthOn - offsetOn));
		}
		else
		{
			timeGain[id] = 1;
			timeGain[!id] = 0;
		}

		gain[id] = max(timeGain[id], turnGain[id]);
		gain[!id] = min(timeGain[!id], turnGain[!id]);
	}
	else if (fileBuffered[id] == "NULL" && filetoBuffer[id] == "NULL")
	{
		gain[id] = 0;
		gain[!id] = 1;
	}
	else if (fileBuffered[!id] == "NULL" && filetoBuffer[!id] == "NULL")
	{
		gain[id] = 1;
		gain[!id] = 0;
	}
	else
	{
		switcher += deltaTime;
		timeCrossfade(&gain[!id], &gain[id], crossFadeDuration, switcher);
	}

	alSourcef(source[!id], AL_GAIN, gain[!id] * finalGain);
	alSourcef(source[id], AL_GAIN, gain[id] * finalGain);
	alSourcef(source[!id], AL_PITCH, pitch[!id]);
	alSourcef(source[id], AL_PITCH, pitch[id]);

	//Для первых 2х источников
	for (size_t i = 0; i < 2; i++)
	{
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

		//Подготавливаем файл к загрузке
		if (filetoBuffer[i] == h.fullName["eng_on_w"])
		{
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
			offset[i] = getParameterFromVector(h.engFunctionOnSwap, parameter);
		}
		else if (filetoBuffer[i] == h.fullName["eng_on_hp_w"])
		{
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
			offset[i] = h.engLengthHpOn / h.engTurnoverHp * parameter;
		}
		else if (filetoBuffer[i] == h.fullName["eng_w_w"])
		{
			offset[i] = h.engLengthMg * phase;
			alSourcei(source[i], AL_LOOPING, AL_TRUE);
		}
		else if (filetoBuffer[i] == h.fullName["eng_w_hp_w"])
		{
			offset[i] = h.engLengthHpW * phase;
			alSourcei(source[i], AL_LOOPING, AL_TRUE);
		}
		else if (filetoBuffer[i] == h.fullName["eng_w_avt_w"])
		{
			alSourcei(source[i], AL_LOOPING, AL_TRUE);
			offset[i] = h.engLengthWAavt * phase;
		}
		else if (filetoBuffer[i] == h.fullName["eng_off_w"])
		{
			offset[i] = getParameterFromVector(h.engFunctionOffSwap, parameter);
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
		}
		else if (filetoBuffer[i] == h.fullName["eng_off_hp_w"])
		{
			offset[i] = h.engLengthHpOff / h.engTurnoverHp * abs(h.engTurnoverHp - parameter);
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
		}

		string modes = "[" + ModeSequence[0] + " " + ModeSequence[1] + " " + ModeSequence[2] + "]";
		cout.precision(3);
		cout << fixed
			<< " ID__: " << id
			<< " gain[" << id << "]: " << gain[id]
			<< " gain[" << !id << "]: " << gain[!id]
			<< " pitch[" << id << "]: " << pitch[id]
			<< " pitch[" << !id << "]: " << pitch[!id]
			<< " offset[" << id << "]: " << offset[id]
			<< " offset[" << !id << "]: " << offset[!id]
			<< " FIB[" << id << "]: " << fileBuffered[id]
			<< " FIB[" << !id << "]: " << fileBuffered[!id]
			<< "\t\t\r";

		//Загружаем буферы и запускам источники
		if (fileBuffered[i] != filetoBuffer[i])
		{
			setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}

		//Следим за параметрами загруженного файла
		if (fileBuffered[i] == h.fullName["eng_on_w"])
		{
			alGetSourcef(source[i], AL_SEC_OFFSET, &offsetOn);
			pitch[i] = parameter / roundFloat(getParameterFromVector(h.engFunctionOn, offsetOn), 0.001);
		}
		else if (fileBuffered[i] == h.fullName["eng_on_hp_w"])
		{
			pitch[i] = 1;
			alGetSourcef(source[i], AL_SEC_OFFSET, &offset[i]);
		}
		else if (fileBuffered[i] == h.fullName["eng_w_w"])
		{
			if (h.modelName == "ansat")
			{
				pitch[i] = roundFloat((parameter / h.engTurnoverAvt), 0.001);
			}
			else
			{
				pitch[i] = roundFloat((parameter / h.engTurnoverMg), 0.001);
			}
		}
		else if (fileBuffered[i] == h.fullName["eng_w_hp_w"])
		{
			pitch[i] = 1/*parameter / h.engTurnoverHp*/;
		}
		else if (fileBuffered[i] == h.fullName["eng_w_avt_w"])
		{
			pitch[i] = roundFloat((parameter / h.engTurnoverAvt), 0.001);//меняем pitch (дает нисходящую прямую при остановке второго дв)
		}
		else if (fileBuffered[i] == h.fullName["eng_off_w"])
		{
			alGetSourcef(source[i], AL_SEC_OFFSET, &offsetOff);
			pitch[i] = parameter / roundFloat(getParameterFromVector(h.engFunctionOff, offsetOff), 0.001);
		}
		else if (fileBuffered[i] == h.fullName["eng_off_hp_w"])
		{
			pitch[i] = 1;
			alGetSourcef(source[i], AL_SEC_OFFSET, &offsetOff);
		}

		//Выключаем источники если обороты равны 0 и двигатель не запускается
		if (!status_on && !status_off && !status_hp && parameter == 0)
		{
			alSourceStop(source[i]);
		}

		//Обновляем статус записи
		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);
	}

	double lowFreqGain = AL_EQUALIZER_DEFAULT_LOW_GAIN;
	double mid1FreqGain = AL_EQUALIZER_DEFAULT_MID1_GAIN;
	double mid2FreqGain = AL_EQUALIZER_DEFAULT_MID2_GAIN;
	double highFreqGain = AL_EQUALIZER_DEFAULT_HIGH_GAIN;

	double lowCutoffFreq = AL_EQUALIZER_DEFAULT_LOW_CUTOFF;//НЧ 50-800
	double mid1CutoffFreq = AL_EQUALIZER_DEFAULT_MID1_CENTER;//купол 1 200-3000
	double mid2CutoffFreq = AL_EQUALIZER_DEFAULT_MID2_CENTER;//купол 2 1000-8000
	double highCutoffFreq = AL_EQUALIZER_DEFAULT_HIGH_CUTOFF;//ВЧ 4000-16000

	double avrTurnRestrict = getParameterFromVector(vector<point>{ { 0, 0 }, { 0.5, 1 }}, hight);

	//Полеты 8 мтв5, 8 амтш, ка 27м, ка 29
	if (h.modelName == "mi_8_amtsh" || h.modelName == "mi_8_mtv5" || h.modelName == "mi_28" || h.modelName == "mi_26" || h.modelName == "ka_27" || h.modelName == "ka_29")
	{
		double averangeTurn;
		if (engNum == 1)
		{
			averangeTurn = getAverange(vectorAvrEng1Turn, 25);
		}
		else if (engNum == 2)
		{
			averangeTurn = getAverange(vectorAvrEng2Turn, 25);
		}

		//усиление от оборотов
		double turnGain = toCoef((parameter - averangeTurn) * 0.35 * avrTurnRestrict);

		lowFreqGain = turnGain;
		mid1FreqGain = turnGain;
		mid2FreqGain = turnGain;
		highFreqGain = turnGain;
	}
	else if (h.modelName == "ansat")
	{
		//Громкость двигателей в зависимости от оборотов
		double turnsGainControl = toCoef(getParameterFromVector(vector<point>{ { 60, -6 }, { 80, -4 }, { 100, 0 }}, parameter)
		);

		lowFreqGain = turnsGainControl;
		mid1FreqGain = turnsGainControl;
		mid2FreqGain = turnsGainControl;
		highFreqGain = turnsGainControl;
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

	/*static double pr = 0;
	pr+= deltaTime;
	if (pr >= 0.01)
	{
		FILE *f = fopen("eng.txt", "at");
		fprintf(f, "%lf\t%f\t%f\t%f\t%f\t%lf\n", currentTime, g[0], g[1], p[0], p[1],parameter);
		fclose(f);
		pr = 0;
	}*/

	

	return 1;
}

Vsu::Vsu() : Sound(2, 2, 0)
{

}

Vsu::~Vsu()
{

}

int Vsu::play(SOUNDREAD &sr, Helicopter &h)
{
	if (init != "set")
	{
		vsuTurnover = sr.vsu_obor;
		init = "set";
	}

	//Для 27 29 и 8
	if (h.modelName == "ka_27" || h.modelName == "ka_29" || h.modelName == "mi_8_mtv5" || h.modelName == "mi_8_amtsh")
	{
		//Подсадка ВСУ при запущенном двигателе, но до запуска редуктора
		if (sr.reduktor_gl_obor > 0)
		{
			gain[0] = interpolation({ 0, 0.7 }, { 2.5, 0.85 }, { 5, 1 }, vsuUpTimer);
			vsuUpTimer += Sound::deltaTime;
			vsuDownTimer = 0;
		}
		else
		{
			if (sr.p_eng1_zap || sr.p_eng1_hp || sr.p_eng2_zap || sr.p_eng2_hp)
			{
				gain[0] = interpolation({ 0, 1 }, { 0.25, 0.85 }, { 0.5, 0.7 }, vsuDownTimer);
				vsuDownTimer += Sound::deltaTime;
				vsuUpTimer = 0;
			}
			else
			{
				gain[0] = interpolation({ 0, 0.7 }, { 2.5, 0.85 }, { 5, 1 }, vsuUpTimer);
				vsuUpTimer += Sound::deltaTime;
				vsuDownTimer = 0;
			}
		}
	}

	if (sr.p_vsu_hp && mode != "w")
	{
		if (h.vsuLengthHpOn - offsetOn <= crossFadeDuration)
		{
			mode = "w";
		}
		else
		{
			mode = "hp";
		}
	}
	else if (sr.p_vsu_zap && mode != "w")
	{
		if (h.vsuLengthOn - offsetOn <= crossFadeDuration)
		{
			mode = "w";
		}
		else
		{
			mode = "on";
		}
	}
	else if (sr.p_vsu_ostanov)
	{
		mode = "off";
	}
	else if (!sr.p_vsu_hp && !sr.p_vsu_zap && !sr.p_vsu_ostanov && sr.vsu_obor != 0)
	{
		mode = "w";
	}

	if (ModeSequence.back() != mode)
	{
		switcher = 0;
		id = !id;
		if (mode == "on" || mode == "off" || mode == "hp")
		{
			fileBuffered[id] = "NULL";
		}
		ModeSequence.push_back(mode);
		if (ModeSequence.size() >= 4)
		{
			ModeSequence.erase(ModeSequence.begin());
		}
	}

	bool modeHp = 0;
	for (size_t i = 0; i < ModeSequence.size(); i++)
	{
		if (ModeSequence[i] == "hp")
		{
			modeHp = 1;
		}
	}
	bool modeOn = !modeHp;

	//0 -> 100
	if (mode == "hp")
	{
		filetoBuffer[id] = h.fullName["vsu_hp_on"];
		vsuTurnover += 35.0 / h.vsuLengthHpOn * deltaTime;
		vsuTurnover = (vsuTurnover > 35) ? 35 : vsuTurnover;
	}
	//0 -> хп
	else if (mode == "on")
	{
		filetoBuffer[id] = h.fullName["vsu_on"];
		vsuTurnover += 100.0 / h.vsuLengthOn * deltaTime;
		vsuTurnover = (vsuTurnover > 100) ? 100 : vsuTurnover;
	}
	//100
	else if (mode == "w")
	{
		if (modeOn)
		{
			filetoBuffer[id] = h.fullName["vsu_w"];
			vsuTurnover += 100.0 / h.vsuLengthOn * deltaTime;
			vsuTurnover = (vsuTurnover > 100) ? 100 : vsuTurnover;
		}
		else if (modeHp)
		{
			filetoBuffer[id] = h.fullName["vsu_hp_w"];
			vsuTurnover += 35.0 / h.vsuLengthHpOn * deltaTime;
			vsuTurnover = (vsuTurnover > 35) ? 35 : vsuTurnover;
		}
	}
	//100 -> 0 || хп -> 0
	else if (mode == "off")
	{
		if (modeOn)
		{
			filetoBuffer[id] = h.fullName["vsu_off"];
			vsuTurnover -= 100.0 / h.vsuLengthOff * deltaTime;
			//Обороты ВСУ не должны падать ниже 0
			vsuTurnover = (vsuTurnover < 0) ? 0 : vsuTurnover;
		}
		else if (modeHp)
		{
			filetoBuffer[id] = h.fullName["vsu_hp_off"];
			vsuTurnover -= 35.0 / h.vsuLengthHpOff * deltaTime;
			vsuTurnover = (vsuTurnover < 0) ? 0 : vsuTurnover;
		}
	}

	double finalGain = 0;
	if (modeOn)
	{
		finalGain = masterGain * h.vsuFactor * gain[id];
	}
	else if (modeHp)
	{
		finalGain = masterGain * h.vsuHpFactor * gain[id];
	}

	double fade = 0;
	double rise = 0;

	switcher += deltaTime;
	timeCrossfade(&fade, &rise, crossFadeDuration, switcher);
	if (filetoBuffer[id] == "NULL" && filetoBuffer[id] == "NULL")
	{
		rise = 0;
		fade = 1;
	}
	else if (filetoBuffer[!id] == "NULL" && filetoBuffer[!id] == "NULL")
	{
		rise = 1;
		fade = 0;
	}

	alSourcef(source[!id], AL_GAIN, fade * finalGain);
	alSourcef(source[id], AL_GAIN, rise * finalGain);

	for (size_t i = 0; i < 2; i++)
	{
		if (filetoBuffer[i] == h.fullName["vsu_on"])
		{
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
			offset[i] = h.vsuLengthOn / h.vsuTurnoverFull * vsuTurnover;
			alSourcef(source[i], AL_PITCH, 1);
			alGetSourcef(source[i], AL_SEC_OFFSET, &offsetOn);
		}
		else if (filetoBuffer[i] == h.fullName["vsu_hp_on"])
		{
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
			offset[i] = h.vsuLengthHpOn / h.vsuTurnoverHp * vsuTurnover;
			alSourcef(source[i], AL_PITCH, 1);
			alGetSourcef(source[i], AL_SEC_OFFSET, &offsetOn);
		}
		else if (filetoBuffer[i] == h.fullName["vsu_w"])
		{
			alSourcef(source[i], AL_PITCH, 1);
			alSourcei(source[i], AL_LOOPING, AL_TRUE);
			offset[i] = 0;
		}
		else if (filetoBuffer[i] == h.fullName["vsu_hp_w"])
		{
			alSourcef(source[i], AL_PITCH, 1);
			alSourcei(source[i], AL_LOOPING, AL_TRUE);
			offset[i] = 0;
		}
		else if (filetoBuffer[i] == h.fullName["vsu_off"])
		{
			offset[i] = h.vsuLengthOff / h.vsuTurnoverFull * abs(h.vsuTurnoverFull - vsuTurnover);
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
			alSourcef(source[i], AL_PITCH, 1);
		}
		else if (filetoBuffer[i] == h.fullName["vsu_hp_off"])
		{
			offset[i] = h.vsuLengthHpOff / h.vsuTurnoverHp * abs(h.vsuTurnoverHp - vsuTurnover);
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
			alSourcef(source[i], AL_PITCH, 1);
		}

		//Загружаем буферы и запускам источники
		if (fileBuffered[i] != filetoBuffer[i])
		{
			setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}

		//Выключаем источники если обороты равны 0 и не запускается
		if (!sr.p_vsu_zap && !sr.p_vsu_ostanov && !sr.p_vsu_hp && vsuTurnover == 0)
		{
			alSourceStop(source[i]);
			alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);		// перезабили признак 
			return 0;
		}

		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);
	}

	/*float a;
	float b;
	alGetSourcef(source[0], AL_GAIN, &a);
	alGetSourcef(source[1], AL_GAIN, &b);
	cout.precision(3);
	cout << fixed
		<< " FADE: " << a
		<< " RISE: " << b
		<< " " << ModeSequence[0]
		<< " " << ModeSequence[1]
		<< " " << ModeSequence[2]
		<< " VSTO: " << vsuTurnover
		<< "\t\t\r";*/

	return 1;
}

VintFlap::VintFlap() : Sound(3, 3, 2)
{

}

int VintFlap::play(Helicopter &h, SOUNDREAD &sr)
{
	//Условия хлопков
	//для отрицательной скорости условия зеркальные
	const double accelerationXBorder = 0.28;//!<мс/с*с
	const double velocityYBorder = -2;//!<мс/с
	const double dashBorder = -0.672;
	int flapIndicator = 0;
	if (velocityVectorXZ < 0)
	{
		//dv>1 и vy<-2
		if (accelerationVectorXZ >= -accelerationXBorder && velocityY <= velocityYBorder)//1
		{
			if ((calcA > 0) & (dashVectorXZ < -dashBorder) & (accelerationVy < 3))
			{
				flapIndicator = 1;
			}
			else
			{
				flapIndicator = 0;
			}
		}
		//dv>1 и vy>-2
		if (accelerationVectorXZ >= -accelerationXBorder && velocityY > velocityYBorder)//2
		{

			if ((accelerationVectorXZ > 0.56 & velocityY < 0) || ((velocityY > 4) & (dashVectorXZ > 0.5) & (accelerationVy > 3)))
			{
				flapIndicator = 2;
			}
			else
			{
				flapIndicator = 0;
			}
		}
		//
		if (accelerationVectorXZ < -accelerationXBorder && velocityY <= velocityYBorder)//3
		{
			if ((accelerationVectorXZ < -1.4) & (abs(derivStep) > 2))
			{
				flapIndicator = 3;
			}
			else
			{
				flapIndicator = 0;
			}
		}
		//
		if (accelerationVectorXZ < -accelerationXBorder && velocityY > velocityYBorder)//4
		{
			if ((accelerationVectorXZ < -0.56) & ((velocityY > 2 && abs(velocityVectorXZ) > 28) | ((abs(velocityVectorXZ) > 50) & (step > 20))))
			{
				flapIndicator = 4;
			}
			else
			{
				flapIndicator = 0;
			}
		}
	}
	else
	{
		//dv<1 и vy<-2
		if (accelerationVectorXZ <= accelerationXBorder && velocityY <= velocityYBorder)//1
		{
			if ((calcA > 0) & (dashVectorXZ > dashBorder) & (accelerationVy < 3))
			{
				flapIndicator = 1;
			}
			else
			{
				flapIndicator = 0;
			}
		}
		//dv<1 и vy>-2
		if (accelerationVectorXZ <= accelerationXBorder && velocityY > velocityYBorder)//2
		{

			if ((accelerationVectorXZ < -0.56 & velocityY < 0) || ((velocityY > 4) & (dashVectorXZ < -0.5) & (accelerationVy > 3)))
			{
				flapIndicator = 2;
			}
			else
			{
				flapIndicator = 0;
			}
		}
		//
		if (accelerationVectorXZ > accelerationXBorder && velocityY <= velocityYBorder)//3
		{
			if ((accelerationVectorXZ > 1.4) & (abs(derivStep) > 2))
			{
				flapIndicator = 3;
			}
			else
			{
				flapIndicator = 0;
			}
		}
		//
		if (accelerationVectorXZ > accelerationXBorder && velocityY > velocityYBorder)//4
		{
			if ((accelerationVectorXZ > 0.56) & ((velocityY > 2 && abs(velocityVectorXZ) > 28) | ((abs(velocityVectorXZ) > 50) & (step > 20))))
			{
				flapIndicator = 4;
			}
			else
			{
				flapIndicator = 0;
			}
		}
	}

	double avrTurnRestrict = getParameterFromVector(vector<point>{ { 0, 0 }, { 0.5, 1 }}, hight);

	//Хлопки 8 мтв5
	if (h.modelName == "mi_8_mtv5")
	{
		if (key[0] != h.fullName["vint_flap"])
		{
			sourceStatus[0] = setAndDeploySound(&buffer[0], &source[0], 0, h.fullName["vint_flap"]);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			key[0] = h.fullName["vint_flap"];
		}

		double atkXvel = calcA * interpolation({ 0, 0 }, { 16.67, 1 }, abs(velocityVectorXZ));

		double averangeTurn = getAverange(vectorAvrRedTurn, 30);

		//Усиление от оборотов
		//только если атака больше 2х
		//делает огибающую атаки более динамичной
		double turnsGain = 0;
		if (atkXvel >= 2)
		{
			//усиление от оборотов
			turnsGain = (sr.reduktor_gl_obor - averangeTurn) * 2 * avrTurnRestrict;
		}

		//Хлопки плавно появляются
		double h_g = interpolation({ 0, 0 }, { 0.5, 0.5 }, { 1, 1 }, hight);//К 1 метру по высоте
		double v_g = interpolation({ 10, 0 }, { 14, 0.25 }, { 28, 1 }, abs(velocityVectorXZ));//К 28 м/с

		//Громкость хлопков зависит от атаки
		double gain_a = interpolation({ -1, -15 }, { 1, -9 }, { 3, -3 }, atkXvel);

		double atkFls = pow(10, (turnsGain + gain_a)*0.05) * h_g * v_g;

		// на висении, используем ускорение в качестве переходной функции хлопков
		//При а = 6 м/с^2 громкость хлопков номинальная, при условии, что вертолет летит вниз, а скорость ниже 60 км/ч (16.67)
		double flapCGainAccX = interpolation({ 0.56, 0 }, { 6, 1 }, abs(accelerationVectorXZ))
			* interpolation({ -0.25, 1 }, { 0.25, 0 }, velocityY)
			* interpolation({ 0, 1 }, { 16.67, 0 }, velocityVectorXZ);//переходит в усиление нч по vy

		if (((velocityVectorXZ < 0 && accelerationVectorXZ > 0.56) || (velocityVectorXZ > 0 && accelerationVectorXZ < -0.56)))//хлопаем
		{
			flapOn += deltaTime;
			flapOn = (flapOn > 1) ? 1 : flapOn;//плавно наводим громкость за 1с
		}
		else
		{
			flapOn -= deltaTime;
			flapOn = (flapOn < 0) ? 0 : flapOn;
		}

		double gainC = (atkFls > flapCGainAccX) ? atkFls : flapCGainAccX * flapOn;

		//Уменьшаем громкость хлопков на 6дб за каждый градус шага ниже 3.5
		double stepDamping = pow(10, ((step < 3.5) ? ((3.5 - step) * (-6)) : 0)*0.05);

		alSourcef(source[0], AL_GAIN, gainC * h.vintFlapFactor * masterGain * stepDamping);

	}
	//Хлопки 8 амтш
	else if (h.modelName == "mi_8_amtsh")
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

		double averangeTurn = getAverange(vectorAvrRedTurn, 30);

		double atkXvel = calcA * interpolation({ 0, 0 }, { 16.67, 1 }, abs(velocityVectorXZ));

		//Осуществляем переход в НЧ хлопки, при малых значениях шага (4-5)
		double flap_h = 0;
		double flap_lo = 0;
		parametricalCrossfade(&flap_lo, &flap_h, step, 4, 5);

		//Осуществляем переход в НЧ хлопки, при малых значениях скорости (14-16.67м/с)
		double flap_hv = 0;
		double flap_lov = 0;
		parametricalCrossfade(&flap_lov, &flap_hv, abs(velocityVectorXZ), 14, 16.67);

		//Из 2х условия НЧ хлопков выбираем преобладающий
		double low = max(flap_lo, flap_lov);

		//Усиление от оборотов
		//только если атака больше 2х
		//делает огибающую атаки более динамичной
		double turnsGain = 0;
		if (atkXvel >= 2)
		{
			//усиление от оборотов
			turnsGain = (sr.reduktor_gl_obor - averangeTurn) * 2 * avrTurnRestrict;
		}

		//Передаточные функции по скорости для хлопков по атаки
		//vL для НЧ
		//vH для ВЧ
		double vL = 0;
		double vH = 0;
		if (abs(velocityVectorXZ) < 11)
		{
			vL = interpolation({ 10, 0 }, { 11, 0.5 }, abs(velocityVectorXZ));//
			vH = interpolation({ 10, 0 }, { 11, 0.25 }, abs(velocityVectorXZ));//
		}
		else
		{
			vL = interpolation({ 11, 0.5 }, { 21, 1 }, abs(velocityVectorXZ));//
			vH = interpolation({ 11, 0.25 }, { 21, 1 }, abs(velocityVectorXZ));//
		}
		double vG = vL * low + vH * (1 - low);//Ограничение по скорости 

		double hG = interpolation({ 0, 0 }, { 0.5, 0.5 }, { 1, 1 }, hight);//Ограничение по высоте 

		double gainAtk = interpolation({ -1, -15 }, { 1, -9 }, { 3, -3 }, atkXvel);

		//Вычисляем громкость хлопков по атаке
		double atkFls = toCoef(turnsGain + gainAtk) * vG * hG;

		//Хлопки на висении возникают при 2ом условии хлопков
		if (((velocityVectorXZ < 0 && accelerationVectorXZ > 0.56) || (velocityVectorXZ > 0 && accelerationVectorXZ < -0.56)))
		{
			flapOn += deltaTime;
			flapOn = (flapOn > 1) ? 1 : flapOn;//плавно наводим громкость за 1с
		}
		else
		{
			flapOn -= deltaTime;
			flapOn = (flapOn < 0) ? 0 : flapOn;
		}

		//При втором условии, на висении, используем ускорение в качестве переходной функции хлопков
		//переходит в усиление НЧ редуктора по vy
		double flapCGainAccX = interpolation({ 0.56, 0 }, { 3, 1 }, abs(accelerationVectorXZ))
			* interpolation({ -0.25 , 1 }, { 0.25, 0 }, velocityY)
			* flapOn
			* interpolation({ 0, 1 }, { 16.67, 0 }, velocityVectorXZ);

		//Из 2х видов хлопков выбираем более громкие
		double gainF = max(atkFls, flapCGainAccX);

		//Устанавливаем громкость НЧ и ВЧ хлопков
		alSourcef(source[0], AL_GAIN, gainF * h.vintFlapFactor * masterGain * (1 - low));
		alSourcef(source[1], AL_GAIN, gainF * h.vintFlapFactor * masterGain * low);

	}
	//Хлопки 28
	else if (h.modelName == "mi_28")
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

		double averangeTurn = getAverange(vectorAvrRedTurn, 30);

		double gain_a = 0;
		double h_g = 0;
		double v_g = 0;
		const double floor = 5;//сдвигаем передаточную функцию атаки на floor вправо

		//Игнорируем некорректные значения атаки, при маленьких значениях скоростей
		double atkXvel = calcA * interpolation({ 0, 0 }, { 16.67, 1 }, abs(velocityVectorXZ));

		h_g = interpolation({ 0, 0 }, { 0.5, 0.5 }, { 1, 1 }, hight);
		v_g = interpolation({ 14, 0 }, { 17, 0.5 }, { 20, 1 }, abs(velocityVectorXZ));//72(1) - 50(0)
		gain_a = interpolation({ -1 + floor ,  -18 }, { 1 + floor, -12 }, { 3 + floor, -6 }, atkXvel);

		//При втором условии, на висении, используем ускорение в качестве переходной функции хлопков
		double flapCGainAccX = interpolation({ 0.56, 0 }, { 1, 1 }, abs(accelerationVectorXZ))
			* interpolation({ -0.25, 1 }, { 0, 0.5 }, { 0.25, 0 }, velocityY)
			* interpolation({ 0, 1 }, { 16.67, 0 }, velocityVectorXZ);//переходит в усиление нч по vy

		if (((velocityVectorXZ < 0 && accelerationVectorXZ > 0.56) || (velocityVectorXZ > 0 && accelerationVectorXZ < -0.56)))//хлопаем
		{
			flapOn += deltaTime;
			flapOn = (flapOn > 1) ? 1 : flapOn;//плавно наводим громкость за 1с
		}
		else
		{
			flapOn -= deltaTime;
			flapOn = (flapOn < 0) ? 0 : flapOn;
		}

		//усиление от оборотов
		double turnsGain = 0;
		if (atkXvel >= 2)
		{
			//усиление от оборотов
			turnsGain = (sr.reduktor_gl_obor - averangeTurn) * 2 * avrTurnRestrict;
		}
		double atkFls = pow(10, (turnsGain + gain_a)*0.05) * h_g * v_g;

		double gainAac = (atkFls > flapCGainAccX) ? atkFls : flapCGainAccX * flapOn;

		double flap_h = 0;
		double flap_lo = 0;
		double flap_hv = 0;
		double flap_lov = 0;
		parametricalCrossfade(&flap_lo, &flap_h, step, 5, 6);
		parametricalCrossfade(&flap_lov, &flap_hv, velocityVectorXZ, 14, 16.67);
		double low = max(flap_lo, flap_lov);
		alSourcef(source[0], AL_GAIN, gainAac * h.vintFlapFactor *masterGain *(h.vintFlapFactor*masterGain - low));
		alSourcef(source[1], AL_GAIN, gainAac * h.vintFlapFactor *masterGain * low);


	}
	//Хлопки ка 27 - 29
	else if (h.modelName == "ka_27" || h.modelName == "ka_29")
	{
		if (key[0] != h.fullName["vint_flap_A"] || key[1] != h.fullName["vint_flap_B"] || key[2] != h.fullName["vint_flap_C"])
		{
			//загружаем равномерные ВЧ хлопки
			setAndDeploySound(&buffer[0], &source[0], 0, h.fullName["vint_flap_A"]);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			//загружаем неравномерные ВЧ хлопки
			setAndDeploySound(&buffer[1], &source[1], 0, h.fullName["vint_flap_B"]);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			//загружаем НЧ хлопки
			setAndDeploySound(&buffer[2], &source[2], 0, h.fullName["vint_flap_C"]);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);

			alEffecti(effect[1], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
			alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[1]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
			alFilteri(filter[1], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[1], AL_LOWPASS_GAIN, 0);
			alSource3i(source[1], AL_AUXILIARY_SEND_FILTER, effectSlot[1], 0, 0);
			alSourcei(source[1], AL_DIRECT_FILTER, filter[1]);

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
			freqCutoffFromTurns = interpolation({ 89, lowerFreqLimit }, { 90, highterFreqLimit }, sr.reduktor_gl_obor);
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
			flapOn += deltaTime;
			flapOn = (flapOn > 1) ? 1 : flapOn;//плавно наводим громкость за 1с
		}
		else
		{
			flapOn -= deltaTime;
			flapOn = (flapOn < 0) ? 0 : flapOn;
		}
		double lowFreqGain = 1;
		double mid1FreqGain = interpolation({ log10(1000), 0.126 }, { log10(2000), 0.5 }, { log10(3000), 1 }, log10(freqCutoffFromTurns));
		double mid2FreqGain = interpolation({ log10(2000), 0.126 }, { log10(3000), 0.5 }, { log10(4000), 1 }, log10(freqCutoffFromTurns));//ниже 4к изменяем усиление куполов
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
		double mirror = 1.385;//5кмч
		double accMirrored = (accelerationVectorXZ >= mirror) ? mirror - (accelerationVectorXZ - mirror) : accelerationVectorXZ;
		double accelerationGain = 10.83 * accMirrored - 15;
		accelerationGain = toCoef(accelerationGain);//коэф

		//Хлопки по высокой скорости
		double hiSpeedGain = 0;
		if (abs(velocityVectorXZ) <= 61.6)
		{
			hiSpeedGain = interpolation({ 50, 0 }, { 61.6, 0.67 }, abs(velocityVectorXZ));
		}
		else
		{
			hiSpeedGain = interpolation({ 61.6, 0.67 }, { 75, 1 }, abs(velocityVectorXZ));
		}
		double resFlapCGain = max(hiSpeedGain, accelerationGain);//из двух показателей громкости НЧ хлопков выбираем преобладающий

		double turnsGain = 0;
		if (sr.reduktor_gl_obor <= 91)
		{
			turnsGain = (91 - sr.reduktor_gl_obor) * (-3);//расчитываем усиление от оборотов винта - 3дб на оборот
		}

		//Условие затухания хлопков
		double off = interpolation({ 14, 1 }, { 0, 0 }, abs(velocityVectorXZ));
		//Условие выбора равномерных-неравномерных хлопков
		double flapA = 0;
		double flapB = 0;
		if (velocityVectorXZ < 0)
		{
			flapA = (accelerationVectorXZ >= -accelerationXBorder) ? 1 : 0;//условие равномерных хлопков
			flapB = (accelerationVectorXZ < -accelerationXBorder) ? 1 : 0;//условие неравномерных хлопков
		}
		else
		{
			flapA = (accelerationVectorXZ <= accelerationXBorder) ? 1 : 0;//условие равномерных хлопков
			flapB = (accelerationVectorXZ > accelerationXBorder) ? 1 : 0;//условие неравномерных хлопков
		}

		//Плавный переход между НЧ и ВЧ хлопками по шагу
		double flapABStep = 0;
		double flapCStep = 0;
		parametricalCrossfade(&flapCStep, &flapABStep, step, 8, 12);
		//убираем эффект шага на вид хлопков,определяем по скорости, так как этот параметр более приоритетный
		if (abs(velocityVectorXZ) < 16.67)
		{
			flapABStep = 1;
			flapCStep = 1;
		}
		//Плавный переход между НЧ и ВЧ хлопками по скорости
		double flapABVX = 0;
		double flapCVX = 0;
		parametricalCrossfade(&flapCVX, &flapABVX, abs(velocityVectorXZ), 15.28, 16.67);

		//на висении, используем ускорение в качестве переходной функции хлопков
		double flapCGainAccX = 1;
		if (((velocityVectorXZ < 0 && accelerationVectorXZ > 0.56) || (velocityVectorXZ > 0 && accelerationVectorXZ < -0.56)) && abs(velocityVectorXZ) < 16.67)
		{
			flapCGainAccX = interpolation({ 0.56, 0 }, { 1, 1 }, abs(accelerationVectorXZ))
				* interpolation({ -0.25, 1 }, { 0, 0.5 }, { 0.25, 0 }, velocityY);//переходит в усиление нч по vy
		}
		//рассчитываем результирующую громкость хлопков в каждый момент времени
		double flapAGain = flapA * flapOn * off * masterGain * h.vintFlapFactor * flapABStep * flapABVX * pow(10, turnsGain*0.05);
		double flapBGain = flapB * flapOn * off * masterGain * h.vintFlapFactor * flapABStep * flapABVX * pow(10, turnsGain*0.05);
		double flapCGain = ((flapIndicator) ?
			(flapCGainAccX * flapCStep * flapCVX * flapOn * off * pow(10, turnsGain*0.05)
				* masterGain
				* (h.vintFlapFactor + (1 - h.vintFlapFactor)*0.5))
			: (masterGain
				* (h.vintFlapFactor + (1 - h.vintFlapFactor)*0.5) * (1 - flapOn) * resFlapCGain * off));

		alSourcef(source[0], AL_GAIN, flapAGain);//равномерные
		alSourcef(source[1], AL_GAIN, flapBGain);//неравномерные
		alSourcef(source[2], AL_GAIN, flapCGain);//тупые

		/*cout.precision(3);
		cout << fixed
			<< " FLAG: " << flapAGain
			<< " FLBG: " << flapBGain
			<< " FLCG: " << flapCGain
			<< " AVXZ: " << accelerationVectorXZ
			<< " DASH: " << dashVectorXZ
			<< " ACCY: " << accelerationVy
			<< " VELY: " << velocityY
			<< " VELX: " << velocityVectorXZ
			<< "\t\t\r";*/
	}
	//Хлопки ми 26
	else if (h.modelName == "mi_26")
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
		double attack = calcA * getValue({ 0, 0 }, { 22.22, 1 }, velocityVectorXZ, 1, "H")
			+ (step - getAverange(vectorAvrStep, 25)) * 5 * ((velocityY < 0) ? 1 : 0)
			* getValue({ 0, 0 }, { 13.89, 1 }, velocityVectorXZ, 0, 1)
			* getValue({ 13.89, 1 }, { 22.22, 0 }, velocityVectorXZ, 0, 1);

		//Передаточная финкция усиления хлопков от атаки
		double atkGain = pow(10, (getValue({ 0, -18 }, { 12, 0 }, attack, 0, "H")) * 0.05);

		//Усиление хлопков при малом шаге
		double flapStepGain = pow(10, interpolation({ 2, 2 }, { 4, 0 }, step)*0.05);

		//Ослабление хлопк(ания)(ования)(очения)(???) при малом шаге
		double flappingStepGain = pow(10, interpolation({ 2, -3 }, { 4, 0 }, step)*0.05);

		//Усиление по Vy при низких скоростях
		double vyG = getParameterFromVector(vector<point>{ {-6, 0}, { 0,-11 }, { 6,-24 }}, velocityY);
		vyG = (vyG > -3) ? -3 : vyG;
		double flappingVyGain = pow(10, vyG *0.05) * interpolation({ 0, 1 }, { 22.22, 0 }, abs(velocityVectorXZ));

		//Усиление по Vx
		double flappingVxGain = pow(10, getParameterFromVector(vector<point>{ {0, -60}, { 49,-12 }, { 56,-6 }, { 70,-3 }, { 84,0 } }, abs(velocityVectorXZ))*0.05);

		//Берем большее из 3х показателей для flapping
		double flappingGainUnhover = max(max(flappingVyGain, atkGain), flappingVxGain);

		//Усиление по dvx
		double accGain = pow(10, (getParameterFromVector(vector<point>{ {2, 0}, { 0, -12 } }, abs(accelerationVectorXZ)))*0.05)
			* interpolation({ 0, 1 }, { 22.22, 0 }, abs(velocityVectorXZ));

		//на висении, используем ускорение в качестве переходной функции хлопков
		double hoveringGain = (velocityVectorXZ * accelerationVectorXZ <= 0) ? accGain : 0;

		//Сглаживаем появление и исчезание хлопков
		double smoothHoveringGain = sm.delay(hoveringGain, deltaTime);

		//printf(" DT__: %.3lf\tFLP: %.3lf\tFLPP: %.3lf\tATK: %.3lf\t\r", Sound::deltaTime, gain[0], gain[1], attack);

		/*static double period = 0;
		if (period == 0)
		{
			remove("flap.txt");
		}
		period += deltaTime;
		if (period > 0.05)
		{
			FILE *f=fopen("flap.txt","at");
			fprintf(f, "%.3lf\t%.3lf\t%.3lf\t%.3lf\t\n", Sound::currentTime, gain[0], gain[1], attack);
			fclose(f);
			period = 0.01;
		}*/

		//Ослабление звука при падении шага до 1
		double lowStepMuting = interpolation({ 1,0.5 }, { 2,1 }, step) * getParameterFromVector(vector<point>{ { 0.625, 0 }, { 0, 1 }}, groundTouch);

		//Усиление висения по vy
		double velocityYGainFlap = pow(10, velocityY * 1 * 0.05) * (accelerationVectorXZ != 0);
		double velocityYGainFlapping = pow(10, velocityY * -1 * 0.05) * (accelerationVectorXZ != 0);

		//Выбираем между хлопками на висении и нет
		double flappingFinalGain = max(smoothHoveringGain * velocityYGainFlapping, flappingGainUnhover * flappingStepGain);
		double flapFinalGain = max(smoothHoveringGain * velocityYGainFlap, atkGain * flapStepGain);

		gain[0] = flapFinalGain * h.vintFlapFactor * masterGain * lowStepMuting * getParameterFromVector(vector<point>{ { 0.625, 0 }, { 0, 1 }}, groundTouch);
		gain[1] = flappingFinalGain * masterGain * lowStepMuting * getParameterFromVector(vector<point>{ { 0.625, 0 }, { 0, 1 }}, groundTouch);

		alSourcef(source[0], AL_GAIN, gain[0]);
		alSourcef(source[1], AL_GAIN, gain[1]);
	}
	//Хлопки ка 226
	else if (h.modelName == "ka_226")
	{
		//Условия наступления хлопков
		if (velocityVectorXZ > 0)
		{
			if (velocityVectorXZ > 28 && accelerationVectorXZ > -0.28 && velocityY > 2)
			{
				flapIndicator = 1;
			}
			else
			{
				flapIndicator = 0;
			}
		}
		else
		{
			if (velocityVectorXZ < -28 && accelerationVectorXZ < 0.28 && velocityY > 2)
			{
				flapIndicator = 1;
			}
			else
			{
				flapIndicator = 0;
			}
		}

		//Если выполняется условие хлопков, выводим хлопки
		if (flapIndicator)//хлопаем
		{
			flapOn += deltaTime;
			flapOn = (flapOn > 1) ? 1 : flapOn;//плавно наводим громкость за 1с
		}
		else
		{
			flapOn -= deltaTime;
			flapOn = (flapOn < 0) ? 0 : flapOn;
		}

		if (key[1] != h.fullName["flapping"])
		{
			setAndDeploySound(&buffer[1], &source[1], 0, h.fullName["flapping"]);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			key[1] = h.fullName["flapping"];
		}

		//Базовый уровень звука хлопков от скорости
		double flappingVelxGain = toCoef(getParameterFromVector(vector<point>{ { 28, -18 }, { 50, -13 }}, velocityVectorXZ)) * getParameterFromVector(vector<point>{ { 0, 0 }, { h.redTurnoverAvt, 1 }}, sr.reduktor_gl_obor);

		//Усиливаем хлопки при условии наступлении хлопков
		double dTangazGain = (derivTangaz > 0) ? derivTangaz * 10 * flapOn : 0;

		//Результирующая громкость хлопков ( не более -5 )
		double flappingGain = (toDb(flappingVelxGain) + dTangazGain > -5) ? -5 : toDb(flappingVelxGain) + dTangazGain;

		alSourcef(source[1], AL_GAIN, toCoef(flappingGain) * masterGain * h.vintFlapFactor);

		alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);
	}
	//Хлопки ансат
	else if (h.modelName == "ansat")
	{
		if (key[0] != h.fullName["vint_flap_hi"])
		{
			setAndDeploySound(&buffer[0], &source[0], 0, h.fullName["vint_flap_hi"]);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			key[0] = h.fullName["vint_flap_hi"];
		}
		if (key[1] != h.fullName["vint_flap"])
		{
			setAndDeploySound(&buffer[1], &source[1], 0, h.fullName["vint_flap"]);
			alSourcei(source[1], AL_LOOPING, AL_TRUE);
			key[1] = h.fullName["vint_flap"];

			alEffecti(effect[1], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
			alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[1]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
			alFilteri(filter[1], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(filter[1], AL_LOWPASS_GAIN, 0);
			alSource3i(source[1], AL_AUXILIARY_SEND_FILTER, effectSlot[1], 0, NULL);
			alSourcei(source[1], AL_DIRECT_FILTER, filter[1]);
		}
		if (key[2] != h.fullName["pinkNoise"])
		{
			setAndDeploySound(&buffer[2], &source[2], 0, h.fullName["pinkNoise"]);
			alSourcei(source[2], AL_LOOPING, AL_TRUE);
			key[2] = h.fullName["pinkNoise"];
		}

		double lowShelf = getParameterFromVector(vector<point>{ { 44.44, -15 }, { 56, -12 }, { 61.1, -8 }, { 70, -7 }}, abs(velocityVectorXZ));

		double atkMiddlePoint = getParameterFromVector(vector<point>{ { -15, -2 }, { -12, 0 }, { -8, 3 }, { -6, 4.99 }}, lowShelf);

		double attackFlapGain = toCoef(getParameterFromVector(vector<point>{ { -10, lowShelf }, { atkMiddlePoint, lowShelf }, { 5, -6 }, { 10, -4 }}, calcA))
			* getParameterFromVector(vector<point>{ { 5.55, 0 }, { 8.33, 0.5 }, { 11.11, 1 }}, abs(velocityVectorXZ))
			* getParameterFromVector(vector<point>{ { 34, 0 }, { 37, 0.5 }, { 40, 1 }}, step)
			* getParameterFromVector(vector<point>{ { 0.625, 0 }, { 0, 1 }}, groundTouch);

		//Ослабление хлопков при треске
		double crunchGainMod = getParameterFromVector(vector<point>{ { -1.3, 0 }, { -0.7, -3 }}, velocityY)
			* getParameterFromVector(vector<point>{ { 5.55, 0 }, { 11.11, 1 }, { 19.4, 1 }, { 25, 0 }}, abs(velocityVectorXZ))
			* getParameterFromVector(vector<point>{ { 34, 1 }, { 37, 0.5 }, { 40, 0 }}, step)
			* getParameterFromVector(vector<point>{ { 0.625, 0 }, { 0, 1 }}, groundTouch)
			* getParameterFromVector(vector<point>{ { 3, 0 }, { 5, 1 }}, calcA);

		//Ослабление хлопков при верт скор
		double velYGainMod = getParameterFromVector(vector<point>{ { -8, -3 }, { -4, 0 }}, velocityY)
			* getParameterFromVector(vector<point>{ { 34, 1 }, { 37, 0.5 }, { 40, 0 }}, step)
			* getParameterFromVector(vector<point>{ { 5, 0 }, { 10, 1 }}, calcA);

		double velYFlapHiGain = toCoef(getParameterFromVector(vector<point>{ { -1.3, -13 }, { -0.7, 0 }}, velocityY))
			* getParameterFromVector(vector<point>{ { 5.55, 0 }, { 11.11, 1 }, { 19.4, 1 }, { 25, 0 }}, abs(velocityVectorXZ))
			* getParameterFromVector(vector<point>{ { 34, 0 }, { 37, 0.5 }, { 40, 1 }}, step)
			* getParameterFromVector(vector<point>{ { 3, 0 }, { 5, 1 }}, calcA);

		//треск на висении
		double vintFlapCrunchHoveringGain = getParameterFromVector(vector<point>{ { 3, 0 }, { 6, 8 }}, tangaz)
			* getParameterFromVector(vector<point>{ { 0, 1 }, { 8.3, 0 }}, abs(velocityVectorXZ));

		//Результирующая громкость хлопков 
		double vintFlapGain = max(toCoef(lowShelf), attackFlapGain) * toCoef(crunchGainMod + velYGainMod + vintFlapCrunchHoveringGain);

		//треск на висении
		double vintFlapHiCrunchHoveringGain = toCoef(getParameterFromVector(vector<point>{ { 4, -13 }, { 6, 0 }}, tangaz))
			* getParameterFromVector(vector<point>{ { 0, 1 }, { 8.3, 0 }}, abs(velocityVectorXZ));

		//Результирующая громкость хлопков 
		double vintFlapHiGain = max(velYFlapHiGain, vintFlapHiCrunchHoveringGain);

		double vaddGain = toCoef(getParameterFromVector(vector<point>{ { -2, -18 }, { -1.3, -15 }, { -0.7, -6 }}, velocityY))
			* getParameterFromVector(vector<point>{ { 5.55, 0 }, { 11.11, 1 }, { 19.4, 1 }, { 25, 0 }}, abs(velocityVectorXZ))
			* getParameterFromVector(vector<point>{ { 34, 0 }, { 37, 0.5 }, { 40, 1 }}, step)
			* getParameterFromVector(vector<point>{ { 3, 0 }, { 5, 1 }}, calcA);

		double highFreqVintFlapGain = toCoef(getParameterFromVector(vector<point>{ { -0.6, -18 }, { -0.5, -15 }, { 0, 0 }}, derivTangaz))
			* getParameterFromVector(vector<point>{ { 3, 0 }, { 7, 1 }}, calcA);

		alSourcef(source[1], AL_GAIN, vintFlapGain * masterGain * h.vintFlapFactor);
		alSourcef(source[1], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);
		alEffectf(effect[1], AL_EQUALIZER_HIGH_CUTOFF, 4000);
		alEffectf(effect[1], AL_EQUALIZER_HIGH_GAIN, highFreqVintFlapGain);//
		alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[1]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)

		alSourcef(source[0], AL_GAIN, vintFlapHiGain * masterGain * h.vintFlapFactor);
		alSourcef(source[0], AL_PITCH, sr.reduktor_gl_obor / h.redTurnoverAvt);

		alSourcef(source[2], AL_GAIN, vaddGain * masterGain * h.vintFlapFactor);
	}
	//Остальные борты
	else
	{
		if (key[0] != h.fullName["vint_flap"])
		{
			sourceStatus[0] = setAndDeploySound(&buffer[0], &source[0], 0, h.fullName["vint_flap"]);
			alSourcei(source[0], AL_LOOPING, AL_TRUE);
			key[0] = h.fullName["vint_flap"];
		}
		alSourcef(source[0], AL_GAIN, gain[0] * h.vintFlapFactor * masterGain);
	}

	return 1;
}

VintSwish::VintSwish() : Sound(2, 2, 0)
{

}

int VintSwish::play(Helicopter &h, SOUNDREAD &sr)
{
	if (h.modelName == "mi_28")
	{
		lengthOn = getLengthWAV(h.fullName["vint_swish_on"]);
		lengthOff = getLengthWAV(h.fullName["vint_swish_off"]);

		//0 -> мг1дв
		if (sr.reduktor_gl_obor < h.redTurnoverMg1 && (sr.p_eng1_zap | sr.p_eng2_zap) && mode != "mg1")
		{
			if (lengthOn - offsetOn <= crossFadeDuration)
			{
				mode = "mg1";
			}
			else
			{
				mode = "on";
			}
		}
		//мг1дв
		else if (sr.reduktor_gl_obor < h.redTurnoverMg1 && !(sr.p_eng1_zap | sr.p_eng2_zap) && !(sr.p_eng1_ostanov && sr.p_eng2_ostanov))
		{
			mode = "mg1";
		}
		//мг1дв <-> мг2дв
		else if (sr.reduktor_gl_obor >= h.redTurnoverMg1 && sr.reduktor_gl_obor <= h.redTurnoverMg2)
		{
			mode = "mg2";
		}
		//мг2дв <-> авт
		else if (sr.reduktor_gl_obor > h.redTurnoverMg2)
		{
			mode = "avt";
		}
		//мг2дв -> 0
		else if (sr.p_eng1_ostanov && sr.p_eng2_ostanov && sr.reduktor_gl_obor > 0 && sr.reduktor_gl_obor < h.redTurnoverMg2)
		{
			mode = "off";
		}

		if (ModeSequence.back() != mode)
		{
			switcher = 0;
			id = !id;
			if (mode == "on" || mode == "off")
			{
				fileBuffered[id] = "NULL";
			}
			ModeSequence.push_back(mode);
			if (ModeSequence.size() >= 4)
			{
				ModeSequence.erase(ModeSequence.begin());
			}
		}

		if (mode == "on")
		{
			filetoBuffer[id] = h.fullName["vint_swish_on"];
		}
		else if (mode == "mg1")
		{
			filetoBuffer[id] = h.fullName["vint_swish_w"];
		}
		else if (mode == "mg2")
		{
			filetoBuffer[id] = h.fullName["vint_swish_w_mg"];
			filetoBuffer[!id] = h.fullName["vint_swish_w"];
		}
		else if (mode == "avt")
		{
			filetoBuffer[id] = h.fullName["vint_swish_w_avt"];
			filetoBuffer[!id] = h.fullName["vint_swish_w_mg"];
		}
		else if (mode == "off")
		{
			if (ModeSequence[1] == "avt")
			{
				fileBuffered[id] = h.fullName["vint_swish_w_mg"];
				ModeSequence[1] = "mg2";
				id = !id;
				filetoBuffer[id] = h.fullName["vint_swish_off"];
			}
			else
			{
				filetoBuffer[id] = h.fullName["vint_swish_off"];
			}
		}

		double finalGain = h.redFactor * masterGain;

		if (fileBuffered[id] == h.fullName["vint_swish_w_avt"])
		{
			gain[id] = getParameterFromVector(vector<point>{ {h.redTurnoverMg2, 0}, { h.redTurnoverAvt, 1 } }, sr.reduktor_gl_obor);
			gain[!id] = 1 - gain[id];
		}
		else if (fileBuffered[!id] == h.fullName["vint_swish_w_avt"])
		{
			gain[id] = 1 - getParameterFromVector(vector<point>{ {h.redTurnoverMg2, 0}, { h.redTurnoverAvt, 1 } }, sr.reduktor_gl_obor);
			gain[!id] = 1 - gain[id];
		}
		else if (fileBuffered[id] == h.fullName["vint_swish_w_mg"] && fileBuffered[!id] == h.fullName["vint_swish_w"])
		{
			gain[id] = getParameterFromVector(vector<point>{ {h.redTurnoverMg1, 0}, { h.redTurnoverMg2, 1 } }, sr.reduktor_gl_obor);
			gain[!id] = 1 - gain[id];
		}
		else if (fileBuffered[!id] == h.fullName["vint_swish_w_mg"] && fileBuffered[id] == h.fullName["vint_swish_w"])
		{
			gain[id] = 1 - getParameterFromVector(vector<point>{ {h.redTurnoverMg1, 0}, { h.redTurnoverMg2, 1 } }, sr.reduktor_gl_obor);
			gain[!id] = 1 - gain[id];
		}
		else if (fileBuffered[id] == "NULL" && filetoBuffer[id] == "NULL")
		{
			gain[id] = 0;
			gain[!id] = 1;
		}
		else if (fileBuffered[!id] == "NULL" && filetoBuffer[!id] == "NULL")
		{
			gain[id] = 1;
			gain[!id] = 0;
		}
		else if (fileBuffered[!id] == h.fullName["vint_swish_on"] && fileBuffered[id] == h.fullName["vint_swish_w"])
		{
			if (reperSet != "set")
			{
				reperTurn = sr.reduktor_gl_obor;
				reperSet = "set";
			}

			double timeGain[2];
			double turnGain[2];

			if (sr.reduktor_gl_obor <= h.redTurnoverMg1)
			{
				parametricalCrossfade(&turnGain[!id], &turnGain[id], sr.reduktor_gl_obor, reperTurn, h.redTurnoverMg1);
			}
			else
			{
				turnGain[id] = 1;
				turnGain[!id] = 0;
			}

			if (offsetOn != 0)
			{
				timeCrossfade(&timeGain[!id], &timeGain[id], crossFadeDuration, crossFadeDuration - (lengthOn - offsetOn));
			}
			else
			{
				timeGain[id] = 1;
				timeGain[!id] = 0;
			}

			gain[id] = max(timeGain[id], turnGain[id]);
			gain[!id] = min(timeGain[!id], turnGain[!id]);
		}
		else
		{
			switcher += deltaTime;
			timeCrossfade(&gain[!id], &gain[id], crossFadeDuration, switcher);
		}

		alSourcef(source[!id], AL_GAIN, gain[!id] * finalGain);
		alSourcef(source[id], AL_GAIN, gain[id] * finalGain);

		//свист просто затихает на выключении
		if (mode == "off")
		{
			alSourcef(source[id], AL_GAIN, interpolation({ h.redTurnoverMg1*0.69, 0 }, { h.redTurnoverMg1, 1 }, sr.reduktor_gl_obor));//
			alSourcef(source[!id], AL_GAIN, interpolation({ h.redTurnoverMg1*0.69, 0 }, { h.redTurnoverMg1, 1 }, sr.reduktor_gl_obor));//
		}

		for (size_t i = 0; i < 2; i++)
		{
			if (filetoBuffer[i] == h.fullName["vint_swish_on"])
			{
				alGetSourcef(source[i], AL_SEC_OFFSET, &offsetOn);
				alSourcei(source[i], AL_LOOPING, AL_FALSE);
				offset[i] = getParameterFromVector(h.redFunctionOnSwap, sr.reduktor_gl_obor);
				alSourcef(source[i], AL_PITCH, sr.reduktor_gl_obor / getParameterFromVector(h.redFunctionOn, offsetOn));
			}
			else if (filetoBuffer[i] == h.fullName["vint_swish_w"])
			{
				alSourcei(source[i], AL_LOOPING, AL_TRUE);
				offset[i] = 0;
				alSourcef(source[i], AL_PITCH, roundFloat((sr.reduktor_gl_obor / h.redTurnoverMg1), 0.001));
			}
			else if (filetoBuffer[i] == h.fullName["vint_swish_w_mg"])
			{
				alSourcei(source[i], AL_LOOPING, AL_TRUE);
				offset[i] = 0;
				alSourcef(source[i], AL_PITCH, roundFloat((sr.reduktor_gl_obor / h.redTurnoverMg2), 0.001));//меняем pitch (дает нисходящую прямую при остановке второго дв)
			}
			else if (filetoBuffer[i] == h.fullName["vint_swish_w_avt"])
			{
				alSourcei(source[i], AL_LOOPING, AL_TRUE);
				offset[i] = 0;
				alSourcef(source[i], AL_PITCH, roundFloat((sr.reduktor_gl_obor / h.redTurnoverAvt), 0.001));//меняем pitch[0] (дает нисходящую прямую при остановке второго дв)
			}
		}
	}
	else if (h.modelName == "ka_226")
	{
		filetoBuffer[0] = h.fullName["vint_hi"];
		alSourcei(source[0], AL_LOOPING, AL_TRUE);

		if (sr.reduktor_gl_obor >= h.redTurnoverMg2)
		{
			//Выбираем высоту тона в зависимости от оборотов редуктора в данный момент
			pitch[0] = sr.reduktor_gl_obor / h.redTurnoverAvt;
			//Выключаем шелест винта на оборотах редуктора ниже оборотов малого газа редуктора
			//Делаем поправку на шаг
			gain[0] = interpolation({ h.redTurnoverAvt, 1 }, { h.redTurnoverMg2, 0 }, sr.reduktor_gl_obor)
				* toCoef(getParameterFromVector(vector<point>{ { 0, -11 }, { 12, -7 }}, step));
		}
		else
		{
			pitch[0] = 1;
			gain[0] = 0;
		}

		alSourcef(source[0], AL_GAIN, masterGain * gain[0] * h.vintSwishFactor);
		alSourcef(source[0], AL_PITCH, pitch[0]);
	}
	else if (h.modelName == "ansat")
	{
		filetoBuffer[0] = h.fullName["vint_hi"];
		alSourcei(source[0], AL_LOOPING, AL_TRUE);


		if (sr.reduktor_gl_obor >= h.redTurnoverMg2)
		{
			//Выбираем высоту тона в зависимости от оборотов редуктора в данный момент
			pitch[0] = sr.reduktor_gl_obor / h.redTurnoverAvt;
			//Выключаем шелест винта на оборотах редуктора ниже оборотов малого газа редуктора
			gain[0] = interpolation({ h.redTurnoverAvt, 1 }, { h.redTurnoverMg2, 0 }, sr.reduktor_gl_obor);
		}
		else
		{
			pitch[0] = 1;
			gain[0] = 0;
		}

		double velXGainMod = toCoef(getParameterFromVector(vector<point>{ { 0, -7 }, { 14, -1 }}, abs(velocityVectorXZ)));

		alSourcef(source[0], AL_PITCH, pitch[0]);

		alSourcef(source[0], AL_GAIN, gain[0] * velXGainMod * masterGain * h.vintSwishFactor);
	}
	else
	{
		filetoBuffer[0] = h.fullName["vint_hi"];
		alSourcei(source[0], AL_LOOPING, AL_TRUE);

		if (sr.reduktor_gl_obor >= h.redTurnoverMg2)
		{
			//Выбираем высоту тона в зависимости от оборотов редуктора в данный момент
			pitch[0] = sr.reduktor_gl_obor / h.redTurnoverAvt;
			//Выключаем шелест винта на оборотах редуктора ниже оборотов малого газа редуктора
			gain[0] = interpolation({ h.redTurnoverAvt, 1 }, { h.redTurnoverMg2, 0 }, sr.reduktor_gl_obor);
		}
		else
		{
			pitch[0] = 1;
			gain[0] = 0;
		}

		alSourcef(source[0], AL_GAIN, masterGain * gain[0] * h.vintSwishFactor);
		alSourcef(source[0], AL_PITCH, pitch[0]);
	}

	for (size_t i = 0; i < 2; i++)
	{
		//При оборотах редуктора = 0 и незапущенных двигателях, останавиваем источники
		if (sr.reduktor_gl_obor == 0 && !sr.p_eng1_zap && !sr.p_eng2_zap)
		{
			alSourceStop(source[i]);
			alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);		// перезабили признак 
		}

		if (fileBuffered[i] != filetoBuffer[i])
		{
			setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}
		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);
	}

	return 1;
}

Skv::Skv() : Sound(3, 3, 2)
{

}

int Skv::play(Helicopter &h, SOUNDREAD &sr)
{
	int status = 0;

	if (h.modelName == "mi_28")
	{
		for (size_t i = 0; i < 2; i++)
		{
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
		}

		pitch[id] = 0.029 * max(sr.eng1_obor, sr.eng2_obor) - 1.484;

		status = Sound::play(sr.p_skv_on, h.fullName["skv_on"], h.fullName["skv_w"], h.fullName["skv_off"], h.skvFactor);//Воспроизводим звук - записываем состояние звука в play

		double averangeTurn = getAverange(vectorAvrRedTurn, 25);

		double avrTurngain = (-5) + (sr.reduktor_gl_obor - averangeTurn) * 4;
		avrTurngain = (avrTurngain > 0) ? 0 : avrTurngain;

		double highFreqGain = pow(10, avrTurngain * 0.05);

		double highCutoffFreq = 4000;//ВЧ 4000-16000

		for (size_t i = 0; i < 2; i++)
		{
			alEffectf(effect[i], AL_EQUALIZER_HIGH_CUTOFF, highCutoffFreq);

			alEffectf(effect[i], AL_EQUALIZER_HIGH_GAIN, highFreqGain);//

			alAuxiliaryEffectSloti(effectSlot[i], AL_EFFECTSLOT_EFFECT, effect[i]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
		}
	}
	else if (h.modelName == "mi_26")
	{
		pitch[id] = 1 + 0.03245 * abs(max(sr.eng1_obor, sr.eng2_obor) - h.engTurnoverAvt);

		status = Sound::play(sr.p_skv_on, h.fullName["skv_on"], h.fullName["skv_w"], h.fullName["skv_off"], h.skvFactor);//Воспроизводим звук - записываем состояние звука в play
	}
	else if (h.modelName == "ka_29" || h.modelName == "ka_27")
	{
		//не работает без двигателя
		if (sr.eng1_obor > 0 || sr.eng2_obor > 0)
		{
			//добавляем шум биений
			if (harm != h.fullName["skv_harm_29"])
			{
				setAndDeploySound(&buffer[2], &source[2], 0, h.fullName["skv_harm_29"]);
				alSourcei(source[2], AL_LOOPING, AL_TRUE);
				harm = h.fullName["skv_harm_29"];

				alEffecti(effect[1], AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);//определяем эффект как эквалайзер
				alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[1]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)
				alFilteri(filter[1], AL_FILTER_TYPE, AL_FILTER_LOWPASS);//определяем фильтр как НЧ
				alFilterf(filter[1], AL_LOWPASS_GAIN, 0);//убираем звук чистого источника
				alSource3i(source[2], AL_AUXILIARY_SEND_FILTER, effectSlot[1], 0, 0);//посылаем выход источника через слот с эффектом
				alSourcei(source[2], AL_DIRECT_FILTER, filter[1]);//подключаем фильтр к источнику
			}

			double harmOn = 0;
			if (soundOn)
			{
				harmOn = offset[id] / lengthOn;
			}
			else if (soundOff)
			{
				harmOn = 1 - (offset[id] / lengthOff);
			}
			else if (soundWork)
			{
				harmOn = 1;
			}

			//регулируем громкость шума
			double harmGain = getParameterFromVector(vector<point>{ { 0, -60 }, { 40, -35 }, { 85, -15 }, { 90, -10 }, { 100, -5 } }, sr.reduktor_gl_obor);
			alSourcef(source[2], AL_GAIN, toCoef(harmGain) * harmOn);

			//Высота тона гармоники
			double harmPitch = 1 + (0.01892 * (max(sr.eng1_obor, sr.eng2_obor) - h.engTurnoverAvt));
			alSourcef(source[2], AL_PITCH, harmPitch);

			//Средние обороты дв за 2 с
			double avrEngTurns = (sr.eng1_obor > sr.eng2_obor) ? sr.eng1_obor - getAverange(vectorAvrEng1Turn, 2) : sr.eng2_obor - getAverange(vectorAvrEng2Turn, 2);

			//Усиление полосы эквалайзера
			double harmEqMid1Gain = getParameterFromVector(vector<point>{ {0, 0}, { 4, 18 } }, avrEngTurns);

			// Усиление полосы эквалайзера
			double harmEqMid2Gain = getParameterFromVector(vector<point>{ {0, 0}, { 4, 12 } }, avrEngTurns);

			alEffectf(effect[1], AL_EQUALIZER_MID1_CENTER, 1000);
			alEffectf(effect[1], AL_EQUALIZER_MID1_WIDTH, 0.2);
			alEffectf(effect[1], AL_EQUALIZER_MID1_GAIN, toCoef(harmEqMid1Gain));//

			alEffectf(effect[1], AL_EQUALIZER_MID2_CENTER, 2400);
			alEffectf(effect[1], AL_EQUALIZER_MID2_WIDTH, 0.2);
			alEffectf(effect[1], AL_EQUALIZER_MID2_GAIN, toCoef(harmEqMid2Gain));//

			alAuxiliaryEffectSloti(effectSlot[1], AL_EFFECTSLOT_EFFECT, effect[1]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)

			gain[id] = toCoef(harmGain);
			pitch[id] = harmPitch;
			status = Sound::play(sr.p_skv_on, h.fullName["skv_on"], h.fullName["skv_w"], h.fullName["skv_off"], h.skvFactor);//Воспроизводим звук - записываем состояние звука в play

			/*cout.precision(3);
			cout << fixed
				<< " HAPI: " << harmPitch
				<< " HAGA: " << harmGain
				<< " HE1G: " << harmEqMid1Gain
				<< " HE2G: " << harmEqMid2Gain
				<< " AVRT: " << avrEngTurns
				<< " AVR1: " << getAverange("eng1Turns", 2)
				<< " AVR2: " << getAverange("eng2Turns", 2)
				<< "\t\t\r"; */
		}
	}
	else
	{
		status = Sound::play(sr.p_skv_on, h.fullName["skv_on"], h.fullName["skv_w"], h.fullName["skv_off"], h.skvFactor);//Воспроизводим звук - записываем состояние звука в play
	}

	return status;
}

Runway::Runway() : Sound(2, 2, 1)
{

}

int Runway::play(Helicopter &h, SOUNDREAD &sr)
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
	}

	if (h.modelName == "mi_8_amtsh" || h.modelName == "mi_8_mtv5")
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

		alEffectf(effect[0], AL_EQUALIZER_HIGH_CUTOFF, 4000);

		alEffectf(effect[0], AL_EQUALIZER_HIGH_GAIN, interpolation({ 11.2, 0.126 }, { 14, 1 }, sr.v_surf_x));//

		alAuxiliaryEffectSloti(effectSlot[0], AL_EFFECTSLOT_EFFECT, effect[0]);//помещаем эффект в слот (в 1 слот можно поместить 1 эффект)

		filetoBuffer[1] = h.fullName["runway"];
		filetoBuffer[0] = h.fullName["landing"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		alSourcei(source[0], AL_LOOPING, AL_TRUE);

		gain[1] = interpolation({ 0, 0 }, { 8.3, 1 }, { 11.2, 0 }, abs(sr.v_surf_x))
			* 0.25
			* getParameterFromVector(vector<point>{ { 0, 0 }, { 0.625, 1 }}, groundTouch);/*Уменьшаем движение по полосе*/
																						  //alSourcef(source[1], AL_GAIN, 0);//
		gain[0] = interpolation({ 8.3, 0 }, { 11.2, 1 }, abs(sr.v_surf_x))
			* 0.854
			* getParameterFromVector(vector<point>{ { 0, 0 }, { 0.625, 1 }}, groundTouch);
	}
	else if (h.modelName == "mi_26")
	{
		filetoBuffer[0] = h.fullName["runway"];
		alSourcei(source[0], AL_LOOPING, AL_TRUE);
		filetoBuffer[1] = h.fullName["flapping"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);

		double drivingGain = getParameterFromVector(vector<point>{ { 0, -60 }, { 1, -30 }, { 2.77, -8 }, { 5, -5 }, { 8.3, -2 }, { 9, 0 } }, abs(sr.v_surf_x));

		gain[0] = toCoef(drivingGain) * getParameterFromVector(vector<point>{ { 0, 0 }, { 0.625, 1 }}, groundTouch) * 0.707;
		gain[1] = interpolation({ 0, 0 }, { 8.3, 1 }, { 14, 0 }, abs(sr.v_surf_x))
			* interpolation({ 78, 0.71 }, { 84, 1 }, sr.reduktor_gl_obor);

		/*static double period = 0;
		if (period == 0)
		{
			remove("driv.txt");
		}
		period += deltaTime;
		if (period > 0.05)
		{
			FILE *f = fopen("driv.txt", "at");
			fprintf(f, "%.3lf\t%.3lf\t%.3lf\t%.3lf\t\n", Sound::currentTime, gain[0], gain[1], groundTouch);
			fclose(f);
			period = 0.01;
		}*/
	}
	else if (h.modelName == "ka_226")
	{
		double drivingGain = getParameterFromVector(vector<point>{ { 0, -18 }, { 8.4, -12 }, { 14, -18 }}, abs(sr.v_surf_x));

		filetoBuffer[1] = h.fullName["runway"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		gain[1] = toCoef(drivingGain) * getParameterFromVector(vector<point>{ { 0, 0 }, { 0.625, 1 }}, groundTouch);
	}
	else if (h.modelName == "ansat")
	{
		double drivingGain = getParameterFromVector(vector<point>{ { 0, -18 }, { 8.3, -3 }, { 14, 0 } }, abs(sr.v_surf_x));

		filetoBuffer[1] = h.fullName["runway"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		gain[1] = toCoef(drivingGain) * getParameterFromVector(vector<point>{ { 0, 0 }, { 0.625, 1 }}, groundTouch);
	}
	else if (h.modelName == "ka_29" || h.modelName == "ka_27")
	{
		double drivingGain = getParameterFromVector(vector<point>{ { 0, -60 }, { 1, -30 }, { 2.77, -8 }, { 5, -5 }, { 8.3, -2 }, { 9, 0 } }, abs(sr.v_surf_x));

		filetoBuffer[1] = h.fullName["runway"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		gain[1] = toCoef(drivingGain) * getParameterFromVector(vector<point>{ { 0, 0 }, { 0.625, 1 }}, groundTouch);

		/*cout.precision(3);
		cout << fixed
			<< " SMDS: " << sm[1].dbPerSec
			<< " SMGA: " << sm[1].newDbGain
			<< " GAN1: " << toDb(gain[1])
			<< "\t\t\r";*/
	}
	else
	{
		filetoBuffer[1] = h.fullName["runway"];
		alSourcei(source[1], AL_LOOPING, AL_TRUE);
		gain[1] = interpolation({ 0, 0 }, { 13.8, 1 }, abs(sr.v_surf_x)) * groundTouch;
	}

	for (size_t i = 0; i < sourceNumber; i++)
	{
		alSourcef(source[i], AL_GAIN, masterGain * h.runwayFactor * sm[i].delay(gain[i], deltaTime));
	}

	/*float g[3];
	for (size_t i = 0; i < sourceNumber; i++)
	{
		alGetSourcef(source[i], AL_GAIN, &g[i]);
	}
	printf("%.3lf\t%.3lf\t%.3lf\t%.3lf\r", g[1], g[2], g[3]);*/

	return 1;
}

double attack(double velocityVectorXZ, double velocityXPrevious, double tangaz, double velocityY)
{
	double calcA = 0;
	if ((velocityVectorXZ + velocityXPrevious) == 0)
	{
		if (velocityY < 0)
		{
			calcA = 90;
		}
		else if (velocityY > 0)
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
		if (velocityY != 0)
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
		calcA = atan(tan(tangaz) - (2 * velocityY) / ((velocityXPrevious + velocityVectorXZ) * cos(tangaz)));
		calcA = calcA * 180 / M_PI;
	}
	return calcA;
}

Sound::Sound() : sourceStatus(new int[2]), gain(new double[2]), pitch(new double[2]), offset(new float[2]), source(new ALuint[2]), buffer(new ALuint[2]), filetoBuffer(new string[2]), fileBuffered(new string[2])
{
	for (size_t i = 0; i < sourceNumber; i++)
	{
		sourceStatus[i] = 0;
		gain[i] = 1;
		pitch[i] = 1;
		offset[i] = 0;
		fileBuffered[i] = "NULL";
		filetoBuffer[i] = "NULL";
	}

	try
	{
		if (sourcesInUse == maxSources)
			throw 256;
	}
	catch (int e)
	{
		cout << " [" << e << "] Sources exist...cant gen more sources...\n" << endl;
	}
	alGenSources(sourceNumber, source.get());
	alGenBuffers(bufferNumber, buffer.get());

	sourcesInUse += sourceNumber;
}

Sound::Sound(int sources, int buffers, int effectslots) : sourceStatus(new int[sources]), gain(new double[sources]), pitch(new double[sources]), offset(new float[sources]), source(new ALuint[sources]), buffer(new ALuint[buffers]), effectSlot(new ALuint[effectslots]), effect(new ALuint[sources]), filter(new ALuint[sources]), filetoBuffer(new string[sources]), fileBuffered(new string[sources])
{
	try
	{
		if (sourcesInUse == maxSources)
			throw 256;
		if (effectSlotsInUse == maxSlots)
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
		gain[i] = 1;
		pitch[i] = 1;
		offset[i] = 0;
		fileBuffered[i] = "NULL";
		filetoBuffer[i] = "NULL";
	}

	if (sourcesInUse + sourceNumber > 256)
		cout << "Caution: only [" << sourcesInUse + sourceNumber - 256 << "] sources can be generated" << endl;
	alGenSources(sourceNumber, source.get());
	alGenBuffers(bufferNumber, buffer.get());
	alGenEffects(sourceNumber, effect.get());
	alGenFilters(sourceNumber, filter.get());
	alGenAuxiliaryEffectSlots(effectSlotNumber, effectSlot.get());

	sourcesInUse += sourceNumber;
	effectSlotsInUse += effectSlotNumber;
}

Sound::Sound(const Sound &copy) : Sound()
{
	/*soundOn = 0;
	soundWork = 0;
	soundOff = 0;*/
	soundOn = copy.soundOn;
	soundWork = copy.soundWork;
	soundOff = copy.soundOff;
	mode = copy.mode;//"0";
					 //ModeSequence.clear();
	ModeSequence = copy.ModeSequence;//{ "0","0","0" };
	lengthOn = copy.lengthOn;
	lengthOff = copy.lengthOff;
	offsetOn = copy.offsetOn;
	offsetOff = copy.offsetOff;
	crossFadeDuration = copy.crossFadeDuration;
	id = copy.id;
	for (size_t i = 0; i < copy.sourceNumber; i++)
	{
		pitch[i] = copy.pitch[i];
		gain[i] = copy.gain[i];
		offset[i] = copy.offset[i];
		source[i] = copy.source[i];
		if (effectSlot)
		{
			effectSlot[i] = copy.effectSlot[i];
		}
		if (effect)
		{
			effect[i] = copy.effect[i];
		}
		if (filter)
		{
			filter[i] = copy.filter[i];
		}
		filetoBuffer[i] = copy.filetoBuffer[i];
		fileBuffered[i] = copy.fileBuffered[i];
		sourceStatus[i] = copy.sourceStatus[i];
	}
	for (size_t i = 0; i < copy.bufferNumber; i++)
	{
		buffer[i] = copy.buffer[i];
	}
	channel = copy.channel;
	sourceNumber = copy.sourceNumber;
	bufferNumber = copy.bufferNumber;
	effectSlotNumber = copy.effectSlotNumber;
}

Sound & Sound::operator =(const Sound & copy)
{
	/*soundOn = 0;
	soundWork = 0;
	soundOff = 0;*/
	soundOn = copy.soundOn;
	soundWork = copy.soundWork;
	soundOff = copy.soundOff;
	mode = copy.mode;//"0";
	//ModeSequence.clear();
	ModeSequence = copy.ModeSequence;//{ "0","0","0" };
	lengthOn = copy.lengthOn;
	lengthOff = copy.lengthOff;
	offsetOn = copy.offsetOn;
	offsetOff = copy.offsetOff;
	crossFadeDuration = copy.crossFadeDuration;
	id = copy.id;
	for (size_t i = 0; i < copy.sourceNumber; i++)
	{
		pitch[i] = copy.pitch[i];
		gain[i] = copy.gain[i];
		offset[i] = copy.offset[i];
		source[i] = copy.source[i];
		if (effectSlot)
		{
			effectSlot[i] = copy.effectSlot[i];
		}
		if (effect)
		{
			effect[i] = copy.effect[i];
		}
		if (filter)
		{
			filter[i] = copy.filter[i];
		}
		filetoBuffer[i] = copy.filetoBuffer[i];
		fileBuffered[i] = copy.fileBuffered[i];
		sourceStatus[i] = copy.sourceStatus[i];
	}
	for (size_t i = 0; i < copy.bufferNumber; i++)
	{
		buffer[i] = copy.buffer[i];
	}
	channel = copy.channel;
	sourceNumber = copy.sourceNumber;
	bufferNumber = copy.bufferNumber;
	effectSlotNumber = copy.effectSlotNumber;
	return *this;
}

Sound::~Sound()
{
	alDeleteSources(sourceNumber, source.get());
	alDeleteBuffers(bufferNumber, buffer.get());
	if (effect)
	{
		alDeleteEffects(sourceNumber, effect.get());
	}
	if (filter)
	{
		alDeleteFilters(sourceNumber, filter.get());
	}
	if (effectSlot)
	{
		alDeleteAuxiliaryEffectSlots(effectSlotNumber, effectSlot.get());
		effectSlotsInUse -= effectSlotNumber;
	}
	sourcesInUse -= sourceNumber;
}

int Crane::play(char status, string pathOn, string pathW, string pathOff, double gainMult)
{
	bool start;
	bool work;
	bool end;
	bool free;

	//Узнаем длинну файлов запуска и остановки
	if (pathOn != "NULL")
		lengthOn = getLengthWAV(pathOn);
	if (pathOff != "NULL")
		lengthOff = getLengthWAV(pathOff);

	alGetSourcei(source[id], AL_SOURCE_STATE, &sourceStatus[id]);

	//условие запуска когда все звуки присутствуют
	if (pathOn != "NULL" & pathW != "NULL" & pathOff != "NULL")
	{
		start = ((int)status == 1) & !soundOn & !soundWork;
		work = ((int)status == 0) & soundOn & !soundWork & ((lengthOn - offset[id]) <= crossFadeDuration);
		end = ((int)status == -1) & !soundOff;
		free = ((int)status == 0) & soundOff & sourceStatus[id] != AL_PLAYING;
	}
	//условие запуска когда отсутствует остановка
	if (pathOn != "NULL" & pathW != "NULL" & pathOff == "NULL")
	{
		start = ((int)status == 1) & !soundOn & !soundWork;
		work = ((int)status == 0)& soundOn & !soundWork & ((lengthOn - offset[id]) <= crossFadeDuration);
		end = 0;
		free = ((int)status == 0);
	}
	//условие запуска когда отсутствует работа
	if (pathOn != "NULL" & pathW == "NULL" & pathOff != "NULL")
	{
		start = ((int)status == 1) & !soundOn;
		work = 0;
		end = ((int)status == -1) & !soundOff;
		free = ((int)status == 0) & soundOff & sourceStatus[id] != AL_PLAYING;
	}
	//условие запуска когда отсутствует запуск
	if (pathOn == "NULL" & pathW != "NULL" & pathOff != "NULL")
	{
		start = 0;
		work = ((int)status == 0) & !soundWork;
		end = ((int)status == -1) & !soundOff;
		free = ((int)status == 0) & soundOff & sourceStatus[id] != AL_PLAYING;
	}
	//условие запуска когда отсутствует запуск и работа
	if (pathOn == "NULL" & pathW == "NULL" & pathOff != "NULL")
	{
		start = 0;
		work = 0;
		end = ((int)status == -1) & !soundOff;
		free = ((int)status == 0) & soundOff & sourceStatus[id] != AL_PLAYING;
	}
	//условие запуска когда отсутствует запуск и выключение
	if (pathOn == "NULL" & pathW != "NULL" & pathOff == "NULL")
	{
		start = 0;
		work = ((int)status == 0) & !soundWork;
		end = 0;
		free = ((int)status == 0);
	}
	//условие запуска когда отсутствует работа и выключение
	if (pathOn != "NULL" & pathW == "NULL" & pathOff == "NULL")
	{
		start = ((int)status == 1) & !soundOn;
		work = 0;
		end = 0;
		free = ((int)status == 0) & sourceStatus[id] != AL_PLAYING;
	}
	//все 0
	if (pathOn == "NULL" & pathW == "NULL" & pathOff == "NULL")
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
		mode = "on";
	}
	//Работа (если pathW указывает на пустую область -> у агрегата отсутствует звук режима работы)
	if (work)
	{
		soundOn = 0;
		soundWork = 1;
		soundOff = 0;
		mode = "w";
	}
	//Выключение (если pathOff указывает на пустую область -> у агрегата отсутствует звук выключения)
	if (end)
	{
		soundOn = 0;
		soundWork = 0;
		soundOff = 1;
		mode = "off";
	}
	//Освобождение памяти
	if (free)
	{
		soundOn = 0;
		soundWork = 0;
		soundOff = 0;

		return 0;
	}

	if (ModeSequence.back() != mode)
	{
		switcher = 0;
		id = !id;
		if (mode == "on" || mode == "off")
		{
			fileBuffered[id] = "NULL";
		}
		ModeSequence.push_back(mode);
		if (ModeSequence.size() >= 4)
		{
			ModeSequence.erase(ModeSequence.begin());
		}
	}


	//cout.precision(3);
	//cout << fixed
	//	<< " ID__: " << id
	//	<< " OFF0: " << offset[0]
	//	<< " OFF1: " << offset[1]
	//	<< " " << ModeSequence[0]
	//	<< " " << ModeSequence[1]
	//	<< " " << ModeSequence[2]
	//	<< "\t\t\r";

	if (mode == "w")
	{
		filetoBuffer[id] = pathW;
		alSourcef(source[id], AL_LOOPING, AL_TRUE);
	}
	else if (mode == "on")
	{
		filetoBuffer[id] = pathOn;
		alSourcef(source[id], AL_LOOPING, AL_FALSE);
	}
	else if (mode == "off")
	{
		filetoBuffer[id] = pathOff;
		alSourcef(source[id], AL_LOOPING, AL_FALSE);
	}

	double finalGain = gain[id] * gainMult * masterGain;
	double rise = 0;
	double fade = 0;
	switcher += deltaTime;
	timeCrossfade(&fade, &rise, crossFadeDuration, switcher);

	if (fileBuffered[id] == "NULL" && filetoBuffer[id] == "NULL")
	{
		rise = 0;
		fade = 1;
	}
	else if ((fileBuffered[!id] == "NULL" && filetoBuffer[!id] == "NULL") || pathW == "NULL")
	{
		rise = 1;
		fade = 0;
	}
	alSourcef(source[!id], AL_GAIN, fade * finalGain);
	alSourcef(source[id], AL_GAIN, rise * finalGain);

	for (size_t i = 0; i < 2; i++)
	{
		//Загружаем буферы и запускам источники
		if (fileBuffered[i] != filetoBuffer[i])
		{
			sourceStatus[i] = setAndDeploySound(&buffer[i], &source[i], offset[i], filetoBuffer[i]);
			fileBuffered[i] = filetoBuffer[i];
		}

		alGetSourcei(source[i], AL_SOURCE_STATE, &sourceStatus[i]);

		alSourcef(source[i], AL_PITCH, pitch[i]);
	}

	//Пока идет запуск - высчитываем точку остановки
	if (soundOn)
	{
		alGetSourcef(source[id], AL_SEC_OFFSET, &offset[id]);
		offsetOn = offset[id];
		if (offset[id] != 0)
		{
			offset[!id] = lengthOff * (1 - (offset[id] / lengthOn));
		}
	}
	//Пока идет остановка - высчитываем точку запуска
	if (soundOff)
	{
		alGetSourcef(source[id], AL_SEC_OFFSET, &offset[id]);
		offsetOff = offset[id];
		if (offset[id] != 0)
		{
			offset[!id] = lengthOn * (1 - (offset[id] / lengthOff));
		}
	}

	return 1;
}
