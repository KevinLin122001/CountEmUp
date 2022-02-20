//File Name: HC_SF04InputCapture.h
//Author: Kevin Lin and Robert Walker
//Last Edit Date: February 19, 2022
//ECE230 Winter 2021-2022

#include <msp.h>
//P1.7
#define InputCaptureTriggerPort P1
#define InputCaptureTrigger BIT7

//P2.5 and P2.6
#define InputCapturePort P2
#define InputCapturePin BIT5 | BIT6

//P1.0
#define LED1port P1
#define LED1Pin BIT0

void InputCaptureConfiguration(void);
