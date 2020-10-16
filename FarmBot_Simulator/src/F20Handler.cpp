/*
 * F20Handler.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#include "F20Handler.h"

static F20Handler *instance;

F20Handler *F20Handler::getInstance()
{
  if (!instance)
  {
    instance = new F20Handler();
  };
  return instance;
};

F20Handler::F20Handler()
{
}

int F20Handler::execute(Command *command, System::IO::Ports::SerialPort^ p_serialPort)
{

  ParameterList::getInstance()->readAllValues(p_serialPort);
  //Serial.print("R20");
  CurrentState::getInstance()->printQAndNewLine(p_serialPort);

  return 0;
}
