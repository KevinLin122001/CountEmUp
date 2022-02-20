/*
 * Speaker.h
 * Subroutine header file for initializing the speaker and playing a note
 * Last Edited Date: February 19, 2022
 * Author: Kevin Lin and Robert Walker
 */

#ifndef SPEAKER_H_
#define SPEAKER_H_

#endif /* SPEAKER_H_ */

#include <msp.h>
#define SpeakerPort P2
#define Speaker BIT4  //P2.4

void SpeakerInit(void);

void PlayNote(unsigned int CurrentNote);
