/*!
\file
\brief Функции и классы общего назначения / Определения
*/

#include "Utility.h"
#include "Mono2channels.h"

using namespace std;

double getLengthWAV(string filename)
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

double toDb(double coef)
{
	//определяем log10(0) как -60
	if (coef == 0)
	{
		return -60;
	}
	else
	{
		return log10(coef) / 0.05;
	}
}

double toCoef(double db)
{
	return pow(10, db * 0.05);
}

double roundFloat(double x, double nullsAfterInt)
{
	return round(x / nullsAfterInt) * nullsAfterInt;
}

double Smoother::delay(double nsGain, double deltaTime)
{

	double nsDbGain = toDb(nsGain);

	//В отрицательной области громкость нарастает быстрее
	dbPerSec = getParameterFromVector(vector<point>{ {-60, 40}, { -20, 16 }, { -15, 10 }, { -10, 6 }, { 0, 3 }}, newDbGain);

	//Берем текущую громкость за начальную
	if (firstAttempt)
	{
		newDbGain = nsDbGain;
		firstAttempt = 0;
	}

	if (abs(newDbGain - nsDbGain) > 0.25)
	{
		if (newDbGain < nsDbGain)
		{
			//Ползем к актуальной громкости
			newDbGain += dbPerSec * deltaTime;
			if (newDbGain > nsDbGain)
			{
				newDbGain = nsDbGain;
			}
		}
		else if (newDbGain > nsDbGain)
		{
			//Ползем к актуальной громкости
			newDbGain -= dbPerSec * deltaTime;
			if (newDbGain < nsDbGain)
			{
				newDbGain = nsDbGain;
			}
		}
		else
		{
			//Ползем к актуальной громкости 
			newDbGain = nsDbGain;
		}
	}
	else
	{
		newDbGain = nsDbGain;
	}

	return  toCoef(newDbGain);
}

double getValue(point p1, point p2, double x, double low_limit, double hi_limit)
{
	double f = ((p1.y - p2.y) / (p1.x - p2.x))*x + (p1.y - ((p1.y - p2.y) / (p1.x - p2.x))*p1.x);
	f = (f > hi_limit) ? hi_limit : f;
	f = (f < low_limit) ? low_limit : f;
	return f;
}

double getValue(point p1, point p2, double x, double limit, string w)
{
	double f = ((p1.y - p2.y) / (p1.x - p2.x))*x + (p1.y - ((p1.y - p2.y) / (p1.x - p2.x))*p1.x);
	if (w == "L")
	{
		f = (f < limit) ? limit : f;
	}
	else if (w == "H")
	{
		f = (f > limit) ? limit : f;
	}
	return f;
}

double getValue(double parameter, int n, point p, ...)
{
	va_list points;
	va_start(points, n);
	vector<point> vectPoint;
	for (size_t i = 0; i < n; i++)
	{
		vectPoint.push_back(va_arg(points, point));
	}
	va_end(points);
	return getParameterFromVector(vectPoint, parameter);
}

double getValue(point p1, point p2, double x)
{
	double f = ((p1.y - p2.y) / (p1.x - p2.x))*x + (p1.y - ((p1.y - p2.y) / (p1.x - p2.x))*p1.x);
	return f;
}

double getParameterFromVector(vector<point> &value, double offset)
{
	point p1, p2, p3;
	double x, a0, a1, a2;
	int n = value.size();
	//если вектор из 1ой точки - возвращаем "y" этой точки
	if (n == 1)
	{
		return value[0].y;
	}
	else if (n == 2)
	{
		p1 = value[0];
		p2 = value[1];
		p3 = p2;
	}
	else if (n == 3)
	{
		p1 = value[0];
		p2 = value[1];
		p3 = value[2];
	}
	//если вектор состоит из малого числа значений - перебираем их
	else if (n < 8)
	{
		if (value[0].x <= value[n - 1].x)
		{
			for (int i = 0; i < n; i++)
			{
				if (offset < value[0].x)
				{
					return value[i].y;//достаем обороты из базы
				}
				if (offset == value[i].x)//реальная отметка времени совпала с отметкой из бд
				{
					return value[i].y;//достаем обороты из базы
				}
				if (offset > value[n - 1].x)//отметка не совпала с базой
				{
					return value[n - 1].y;//достаем обороты из базы
				}
				if (offset > value[i].x && offset < value[i + 1].x)//отметка не совпала с базой
				{
					if (value.size() > 2)
					{
						//Выбираем 3 точки (вариант -1 0 +1)
						if (i - 1 == -1)
						{
							p1 = value[i]; p2 = value[i + 1]; p3 = value[i + 2];
						}
						else if (i + 1 == value.size())
						{
							p1 = value[i - 2]; p2 = value[i - 1]; p3 = value[i];
						}
						else
						{
							p1 = value[i - 1]; p2 = value[i]; p3 = value[i + 1];
						}
					}
					else
					{
						return interpolation(value[0], value[1], offset);
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < n; i++)
			{
				if (offset > value[0].x)
				{
					return value[0].y;//достаем обороты из базы
				}
				if (offset == value[i].x)//реальная отметка времени совпала с отметкой из бд
				{
					return value[i].y;//достаем обороты из базы
				}
				if (offset < value[n - 1].x)//отметка не совпала с базой
				{
					return value[n - 1].y;//достаем обороты из базы
				}
				if (offset < value[i].x && offset > value[i + 1].x)//отметка не совпала с базой
				{
					if (value.size() > 2)
					{
						//Выбираем 3 точки (вариант -1 0 +1)
						if (i - 1 == -1)
						{
							p1 = value[i]; p2 = value[i + 1]; p3 = value[i + 2];
						}
						else if (i + 1 == value.size())
						{
							p1 = value[i - 2]; p2 = value[i - 1]; p3 = value[i];
						}
						else
						{
							p1 = value[i - 1]; p2 = value[i]; p3 = value[i + 1];
						}
					}
					else
					{
						return interpolation(value[0], value[1], offset);
					}
				}
			}
		}
	}
	//если вектор длинный и сортирован! (вектора должны быть подготовлены заранее) - используем бинарный поиск
	//TODO: алгоритм сортировки
	else
	{
		int num = binSer(value, offset);
		//Выбираем 3 точки (вариант -1 0 +1)
		if (num - 1 == -1)
		{
			p1 = value[num]; p2 = value[num + 1]; p3 = value[num + 2];
		}
		else if (num + 1 == value.size())
		{
			p1 = value[num - 2]; p2 = value[num - 1]; p3 = value[num];
		}
		else
		{
			p1 = value[num - 1]; p2 = value[num]; p3 = value[num + 1];
		}
	}

	return interpolation(p1, p2, p3, offset);
}

vector<point> getVectorFromFile(string filename)
{
	vector<point> vect;

	//данные в базе должны храниться в строках парами, по паре в каждой строке (не больше)
	ifstream base(filename);
	while (!base.eof())
	{
		string str;
		double t = 0;
		double v = 0;
		getline(base, str);
		sscanf(str.c_str(), "%lf %lf", &t, &v);
		vect.push_back({ t,v });
	}
	base.close();

	return vect;
}

int binSer(vector<point> &points, double offset)
{
	int l = 0;
	int n = points.size() / 2;
	int r = points.size() - 1;

	if (points.front().x < points.back().x)
	{
		while (abs(l - r) >= 2)
		{
			if (offset == points[n].x)
			{
				return n;
			}
			else if (offset < points[n].x)
			{
				r = n;
			}
			else
			{
				l = n;
			}
			n = trunc((float)(l + r) / 2.0);
		}
	}
	else
	{
		while (abs(l - r) >= 2)
		{
			if (offset == points[n].x)
			{
				return n;
			}
			else if (offset > points[n].x)
			{
				r = n;
			}
			else
			{
				l = n;
			}
			n = trunc((float)(l + r) / 2.0);
		}
	}
	return n;
}

double interpolation(point p1, point p2, double x)
{
	if (p1.x < p2.x && x > p2.x)
	{
		return p2.y;
	}
	if (p1.x < p2.x && x < p1.x)
	{
		return p1.y;
	}
	if (p1.x > p2.x && x < p2.x)
	{
		return p2.y;
	}
	if (p1.x > p2.x && x > p1.x)
	{
		return p1.y;
	}

	if ((p2.x - p1.x) == 0)
	{
		return p1.y;
	}
	else
	{
		return	p1.y + ((p2.y - p1.y) / (p2.x - p1.x))*(x - p1.x);
	}
}

double interpolation(point p1, point p2, point p3, double x)
{
	if (p1.x < p3.x && x > p3.x)
	{
		return p3.y;
	}
	if (p1.x < p3.x && x < p1.x)
	{
		return p1.y;
	}
	if (p1.x > p3.x && x < p3.x)
	{
		return p3.y;
	}
	if (p1.x > p3.x && x > p1.x)
	{
		return p1.y;
	}

	//если квадратичная интерполяция не работает - берем линейную
	if (p1.x == p2.x)
	{
		return	interpolation(p1, p3, x);
	}
	else if (p1.x == p3.x)
	{
		return	interpolation(p1, p2, x);
	}
	else if (p2.x == p3.x)
	{
		return	interpolation(p1, p3, x);
	}
	else if (p1.x == p2.x && p2.x == p3.x)
	{
		return p3.y;
	}
	else
	{
		double a0, a1, a2;
		a2 = ((p3.y - p1.y) / ((p3.x - p1.x)*(p3.x - p2.x))) - ((p2.y - p1.y) / ((p2.x - p1.x)*(p3.x - p2.x)));
		a1 = ((p2.y - p1.y) / (p2.x - p1.x)) - (a2*(p2.x + p1.x));
		a0 = p1.y - a1 * p1.x - a2 * p1.x*p1.x;
		return a0 + a1 * x + a2*x*x;
	}
}

point & point::operator=(const point & copy)
{
	this->x = copy.x;
	this->y = copy.y;

	return *this;
}

int setBuffer(ALuint Buffer, string path, vector<double>channels)
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
	AL_SOUND_CHANNELS channelsCount = AL_SOUND_CHANNELS_6;// Конфигурация каналов - устройств вывода (2.1,4.1,5.1,6.1,7.1)

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