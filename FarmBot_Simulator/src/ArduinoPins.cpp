#include "ArduinoPins.h"
static ArduinoPins* instanceParam;
ArduinoPins* ArduinoPins::getInstance()
{
	if (!instanceParam)
	{
		instanceParam = new ArduinoPins();
	};
	return instanceParam;
}
ArduinoPins::ArduinoPins() {
	//Create the lis of Pins
	for (int i= 1; i<=97;i ++) {
		//For the special case of Analog Pins
		if(i>=82 && i<=97)
			pines.push_back(PinObj(i, ANALOG_PIN));
		else
			pines.push_back(PinObj(i, DIGITAL_PIN));
	}
	for (int i = 0; i < 97; i++) {
		pines[i].setValue(0);
	}
	//Missing the special cases of PWM Pines

}

void ArduinoPins::setMode(int p_pinNr, const char* p_pinMode)
{
	searchPin(p_pinNr).setType(p_pinMode);
	//Missing code here
}

void ArduinoPins::digitalWrite(int p_pinNr, int p_value)
{
	for (int i = 0; i < 97; i++) {
		int pinActual = pines[i].getPinNr();
		if (pinActual == p_pinNr)
			pines[i].setValue(p_value);
	}
	//Missing the code here
}

PinObj ArduinoPins::searchPin(int p_pinNr)
{
	for (int i = 0; i < 97; i++) {
		int pinActual = pines[i].getPinNr();
		if (pinActual == p_pinNr)
			return pines[i];
	}
	}
