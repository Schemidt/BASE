/*!
\file
\brief Функция преобразования буфера

Данный файл описывает функцию преобразования буфера
*/
#pragma once

#include "string"

#include "AL\al.h"
#include "AL\alc.h"
#include "AL\alut.h"
#include "AL\alext.h"
#include "AL\efx.h"

using namespace std;

enum AL_SOUND_CHANNELS_FLAGS
{
	AL_CHANNEL_DEFAULT = 0x0000,   // to all available channels
	AL_CHANNEL_FRONT_LEFT = 0x0001,
	AL_CHANNEL_FRONT_RIGHT = 0x0002,
	AL_CHANNEL_FRONT_CENTER = 0x0004,
	AL_CHANNEL_LOW_FREQ = 0x0008,
	AL_CHANNEL_BACK_LEFT = 0x0010,
	AL_CHANNEL_BACK_RIGHT = 0x0020,
	AL_CHANNEL_BACK_CENTER = 0x0040,
	AL_CHANNEL_SIDE_LEFT = 0x0080,
	AL_CHANNEL_SIDE_RIGHT = 0x0100
};

enum AL_SOUND_CHANNELS
{
	AL_SOUND_CHANNELS_DEFAULT = 0,   // load as is
	AL_SOUND_CHANNELS_2 = 2,   // two
	AL_SOUND_CHANNELS_4 = 4,   // four
	AL_SOUND_CHANNELS_6 = 6,   // 5.1
	AL_SOUND_CHANNELS_7 = 7,   // 6.1
	AL_SOUND_CHANNELS_8 = 8,   // 7.1
};

/* template define rule:
 * if
 * ( bitsPerSample == 16 bit ) => we should convert data as [short monodata = ((short*)loadedData)]
 * ( bitsPerSample == 8  bit ) => we should convert data as [uchar monodata = ((uchar*)loadedData)]
 * ( bitsPerSample == 32 bit ) => we should convert data as [float monodata = ((float*)loadedData)]
 */
template <class T >
bool mono2channels(T                         *monodata,        /* in  */
	int                        sizeBytes,       /* in  */
	AL_SOUND_CHANNELS          channelsCount,   /* in  */
	double						*channels,        /* in (enum AL_SOUND_CHANNELS)  */
	void                      **returnedData,   /* out */
	int                       *returnedSize)   /* out */
{
	/* check out */
	if ((channelsCount == AL_SOUND_CHANNELS_DEFAULT))
		return false;

	/* samples count calculating */
	int samples = sizeBytes / sizeof(T);

	/* allocate new sound data */
	T *newSoundData = new T[samples * channelsCount];
	int newSoundDataSize = samples * channelsCount * sizeof(T);

	/* all channels to silent */
	memset(newSoundData, 0, newSoundDataSize);

	/* fill needed sound channels */
	for (int i = 0; i < samples; i++)
	{
		/* according channelsCount */
		if (channelsCount == AL_SOUND_CHANNELS_2)
		{
			newSoundData[i*channelsCount + 0] = monodata[i] * channels[0];//front left
			newSoundData[i*channelsCount + 1] = monodata[i] * channels[1];//front right
		}
		if (channelsCount == AL_SOUND_CHANNELS_4)
		{
			newSoundData[i*channelsCount + 0] = monodata[i] * channels[0];//front left
			newSoundData[i*channelsCount + 1] = monodata[i] * channels[1];//front right
			newSoundData[i*channelsCount + 2] = monodata[i] * channels[2];//back left
			newSoundData[i*channelsCount + 3] = monodata[i] * channels[3];//back rigth
		}
		else if (channelsCount == AL_SOUND_CHANNELS_6)
		{
			newSoundData[i*channelsCount + 0] = monodata[i] * channels[0];//front left
			newSoundData[i*channelsCount + 1] = monodata[i] * channels[1];//front right
			newSoundData[i*channelsCount + 2] = monodata[i] * channels[2];//front center
			newSoundData[i*channelsCount + 3] = monodata[i] * channels[3];//low freq
			newSoundData[i*channelsCount + 4] = monodata[i] * channels[4];//back left
			newSoundData[i*channelsCount + 5] = monodata[i] * channels[5];//back right
		}
		else if (channelsCount == AL_SOUND_CHANNELS_7)
		{
			newSoundData[i*channelsCount + 0] = monodata[i] * channels[0];//front left
			newSoundData[i*channelsCount + 1] = monodata[i] * channels[1];//front right
			newSoundData[i*channelsCount + 2] = monodata[i] * channels[2];//front center
			newSoundData[i*channelsCount + 3] = monodata[i] * channels[3];//low freq
			newSoundData[i*channelsCount + 4] = monodata[i] * channels[4];//back left
			newSoundData[i*channelsCount + 5] = monodata[i] * channels[5];//back right
			newSoundData[i*channelsCount + 6] = monodata[i] * channels[6];//back center
		}
		else if (channelsCount == AL_SOUND_CHANNELS_8)
		{
			newSoundData[i*channelsCount + 0] = monodata[i] * channels[0];//front left
			newSoundData[i*channelsCount + 1] = monodata[i] * channels[1];//front right
			newSoundData[i*channelsCount + 2] = monodata[i] * channels[2];//front center
			newSoundData[i*channelsCount + 3] = monodata[i] * channels[3];//low freq
			newSoundData[i*channelsCount + 4] = monodata[i] * channels[4];//back left
			newSoundData[i*channelsCount + 5] = monodata[i] * channels[5];//back right
			newSoundData[i*channelsCount + 6] = monodata[i] * channels[6];//side left 
			newSoundData[i*channelsCount + 7] = monodata[i] * channels[7];//side right
		}
	}  /* end for( i = 0;....samples ) */

	*returnedData = newSoundData;
	*returnedSize = newSoundDataSize;
	return true;
}

ALenum getFormat(AL_SOUND_CHANNELS chan, int bitsPerSample)
{
	/* готовим строковый вид формата */
	string str = "AL_FORMAT_";

	// does sound card support such a format
	switch (chan)
	{
	case AL_SOUND_CHANNELS_2: str += "STEREO"; break;
	case AL_SOUND_CHANNELS_4: str += "QUAD"; break;
	case AL_SOUND_CHANNELS_6: str += "51CHN"; break;
	case AL_SOUND_CHANNELS_7: str += "61CHN"; break;
	case AL_SOUND_CHANNELS_8: str += "71CHN"; break;
	};

	str += to_string(bitsPerSample);

	// пытаемся получить у OpenAL формат...
	ALenum rFormat = alGetEnumValue(str.c_str());

	return rFormat;
}
