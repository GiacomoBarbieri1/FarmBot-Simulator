#pragma once




public class PinObj
{
private:
	int pinNr;
	const char* pinMode;
	const char* pinType;
	int analogPinValue;
	int pinValue;
	bool digitalPinValue;
	
public:
	PinObj(int p_pinNr,const char* pinType);
	int getPinNr();
	const char* getPinType();
	void setMode(const char* p_pinMode);
	void setType(const char* p_pinType);
	void setValue(int p_pinValue);

	int getValue();


};

