/*
 * F22Handler.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#include "F22Handler.h"

static F22Handler *instance;

F22Handler *F22Handler::getInstance()
{
  if (!instance)
  {
    instance = new F22Handler();
  };
  return instance;
};

F22Handler::F22Handler()
{
}

int F22Handler::execute(Command *command, System::IO::Ports::SerialPort^ m_serialPort)
{


m_serialPort->Write("R99");
m_serialPort->Write(" ");
m_serialPort->Write("write value");
m_serialPort->Write(" ");
m_serialPort->Write("P");

m_serialPort->Write(" ");
m_serialPort->Write(System::Convert::ToString(command->getP()));
m_serialPort->Write(" ");

m_serialPort->Write("V");
m_serialPort->Write(" ");
m_serialPort->Write(System::Convert::ToString(command->getV()));
m_serialPort->Write(" ");

m_serialPort->Write("\r\n");


 System::String^ respuesta= ParameterList::getInstance()->writeValue(command->getP(), command->getV());
 m_serialPort->Write(respuesta);
  return 0;
}
