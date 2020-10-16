#pragma once
#include "PinClass.h"
#include <iostream>
#include <vector>

#define OUTPUT "Output"
#define INPUT "Input"
#define INPUT_PULLUP "Input Pull Up"
#define ANALOG_PIN "Analog Pin"
#define DIGITAL_PIN "Digital Pin"
#define PWM_DIGITAL_PIN "Pwm Digital Pin"
#define HIGH 1
#define LOW 0

public class ArduinoPins
{
private:
	std::vector<PinObj> pines;
public:
	
	static ArduinoPins* getInstance();
	ArduinoPins();
	void setMode(int p_pinNr,const char* p_pinMode);
	void digitalWrite(int p_pinNr, int p_value);
	PinObj searchPin(int p_pinNr);


};

