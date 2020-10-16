/*
 * G00Handler.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#include "G00Handler.h"

static G00Handler *instance;

G00Handler *G00Handler::getInstance()
{
  if (!instance)
  {
    instance = new G00Handler();
  };
  return instance;
};

G00Handler::G00Handler()
{
}

int G00Handler::execute(Command *command , System::IO::Ports::SerialPort^ m_serialPort)
{
  
  	m_serialPort->Write("G00 was here\r\n");

    m_serialPort->Write("R99");
    m_serialPort->Write(" X ");
    m_serialPort->Write(System::Convert::ToString(command->getX()));
    m_serialPort->Write(" Y ");
    m_serialPort->Write(System::Convert::ToString(command->getY()));
    m_serialPort->Write(" Z ");
    m_serialPort->Write(System::Convert::ToString(command->getZ()));
    m_serialPort->Write(" A ");
    m_serialPort->Write(System::Convert::ToString(command->getA()));
    m_serialPort->Write(" B ");
    m_serialPort->Write(System::Convert::ToString(command->getB()));
    m_serialPort->Write(" C ");
    m_serialPort->Write(System::Convert::ToString(command->getC()));
    m_serialPort->Write("\r\n");
    //Movement *mov = Movement::getInstance();
    //std::thread t{mov->moveToCoords, command->getX(), command->getY(), command->getZ(),
    //    command->getA(), command->getB(), command->getC(),
    //    false, false, false, m_serialPort };
  Movement::getInstance()->moveToCoords(
      command->getX(), command->getY(), command->getZ(),
      command->getA(), command->getB(), command->getC(),
      false, false, false,m_serialPort);

  if (LOGGING)
  {
    //CurrentState::getInstance()->print();
  }
  return 0;
}
