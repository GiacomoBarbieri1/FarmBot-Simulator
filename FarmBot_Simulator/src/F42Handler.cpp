/*
 * F42Handler.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#include "F42Handler.h"

static F42Handler *instance;

F42Handler *F42Handler::getInstance()
{
  if (!instance)
  {
    instance = new F42Handler();
  };
  return instance;
};

F42Handler::F42Handler()
{
}

int F42Handler::execute(Command *command, System::IO::Ports::SerialPort^ m_serialPort)
{

  //PinControl::getInstance()->readValue(command->getP(), command->getM());
   int val= ArduinoPins::getInstance()->searchPin(command->getP()).getValue();
   m_serialPort->Write("R41");
   m_serialPort->Write(" ");
   m_serialPort->Write("P");
   m_serialPort->Write(System::Convert::ToString(command->getP()));
   m_serialPort->Write(" ");
   m_serialPort->Write("V");
   m_serialPort->Write(System::Convert::ToString(val));
  //serial.print("\r\n");
  CurrentState::getInstance()->printQAndNewLine(m_serialPort);
  return 0;
}
