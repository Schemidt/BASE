/*!
\file
\brief Реализует общую память / Объявление
*/
#ifndef __SHARED__
#define __SHARED__

#include "Sound.h"

struct pSharedData {
   SOUNDREAD sr;	// Данные

};

extern pSharedData *shaData;

char shaInit ( void );
void shaClose( void );

#endif