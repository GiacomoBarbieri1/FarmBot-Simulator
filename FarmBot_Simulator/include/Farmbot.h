#pragma once
#include <iostream>
#include <chrono> 
#include <thread> 
#include "Board.h"
#include "Config.h"
#include "pins.h"
#include "ParameterList.h"
#include <iostream>
#include "ArduinoPins.h"
#include "Movement.h"
#include "GCodeProcessor.h"

using namespace std;
using namespace System;

public ref class Farmbot
{
private:
	ArduinoPins* PinsList;
	Command* command;
public:
	System::IO::Ports::SerialPort^ m_serialPort;
public:
	Farmbot(System::IO::Ports::SerialPort^ p_serialPort);
	void setPinInputOutput();
	void readParameters();
	void loadMovementSetting();
	void startMotor();
	void startPinGuard();
	void startServo();
	void startInterrupt();
	void initLastAction();
	void homeOnBoot();
	void loop();
	//void checkEncoders();
	void checkPinGuard();
	void checkSerialInputs();
	void checkEmergencyStop();
	void checkParamsChanged();
	void periodicChecksAndReport();

	System::String^ conv2Str(const char* mensaje);
	System::String^ conv2Str(char* mensaje);
};

