/*
 * Speaker.h
 *
 *  Created on: Feb 18, 2022
 *      Author: link
 */

#ifndef SPEAKER_H_
#define SPEAKER_H_



#endif /* SPEAKER_H_ */

#include <msp.h>
#define SpeakerPort P2
#define Speaker BIT4  //P2.6

void SpeakerInit(void);

void PlayNote(unsigned int CurrentNote);
