#include "PinClass.h"

PinObj::PinObj(int p_pinNr, const char* p_pinType)
{
	pinNr = p_pinNr;
	pinType = p_pinType;
	pinMode = "";

}

int PinObj::getPinNr()
{
	return pinNr;
}

const char* PinObj::getPinType()
{
	return pinType;
}

void PinObj::setMode(const char* p_pinMode)
{
	pinMode = p_pinMode;
}

void PinObj::setType(const char* p_pinType)
{
	pinType = p_pinType;
}

void PinObj::setValue(int p_pinValue)
{
	pinValue = p_pinValue;
}

int PinObj::getValue() {
	return pinValue;
}
