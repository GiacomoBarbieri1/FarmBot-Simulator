/*
 * GCodeProcessor.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 *      Author: Tim Evers
 */

#include "GCodeProcessor.h"


GCodeProcessor::GCodeProcessor()
{
}

GCodeProcessor::~GCodeProcessor()
{
}

void GCodeProcessor::printCommandLog(Command *command, System::IO::Ports::SerialPort^ m_serialPort)
{
  m_serialPort->Write("command == NULL: ");
  m_serialPort->Write("\r\n");
}

void GCodeProcessor::executeHandler()
{
}

int GCodeProcessor::execute(Command *command, System::IO::Ports::SerialPort^ m_serialPort)
{

  int execution = 0;
  bool isMovement = false;

  bool emergencyStop = false;
  emergencyStop = CurrentState::getInstance()->isEmergencyStop();

  int attempt = 0;
  int maximumAttempts = ParameterList::getInstance()->getValue(PARAM_MOV_NR_RETRY);

  long Q = command->getQ();
  CurrentState::getInstance()->setQ(Q);

  if
  (
    command->getCodeEnum() == G00 ||//Move To Home creo que falta g28
    command->getCodeEnum() == G01 ||
    command->getCodeEnum() == F11 ||//Find Home x
    command->getCodeEnum() == F12 ||//Find Home y
    command->getCodeEnum() == F13 ||//Find Home z
    command->getCodeEnum() == F14 ||//Calibrate x
    command->getCodeEnum() == F15 ||//Calibrate y
    command->getCodeEnum() == F16 //Calibrate z
  )
  {
    isMovement = true;
  }



  //Only allow reset of emergency stop when emergency stop is engaged

  if (emergencyStop)
  {
    if (!(
      command->getCodeEnum() == F09 ||
      command->getCodeEnum() == F20 ||
      command->getCodeEnum() == F21 ||
      command->getCodeEnum() == F22 ||
      command->getCodeEnum() == F81 ||
      command->getCodeEnum() == F82 ||
      command->getCodeEnum() == F83 ))
    {

    m_serialPort->Write(conv2Str(COMM_REPORT_EMERGENCY_STOP));
    CurrentState::getInstance()->printQAndNewLine(m_serialPort);
    return -1;
    }
  }

  // Do not execute the command when the config complete parameter is not
  // set by the raspberry pi and it's asked to do a move command

  if (ParameterList::getInstance()->getValue(PARAM_CONFIG_OK) != 1)
  {
    if (isMovement)
    {
      m_serialPort->Write(conv2Str(COMM_REPORT_NO_CONFIG));
      CurrentState::getInstance()->printQAndNewLine(m_serialPort);
      CurrentState::getInstance()->setLastError(ERR_PARAMS_NOT_OK);
      return -1;
    }
  }

  // Return error when no command or invalid command is found

  if (command == NULL)
  {

    m_serialPort->Write(conv2Str(COMM_REPORT_BAD_CMD));
    CurrentState::getInstance()->printQAndNewLine(m_serialPort);
    CurrentState::getInstance()->setLastError(ERR_INVALID_COMMAND);

    if (LOGGING)
    {
      printCommandLog(command,m_serialPort);
    }
    return -1;
  }

  if (command->getCodeEnum() == CODE_UNDEFINED)
  {
    m_serialPort->Write(conv2Str(COMM_REPORT_BAD_CMD));
    CurrentState::getInstance()->printQAndNewLine(m_serialPort);
    CurrentState::getInstance()->setLastError(ERR_INVALID_COMMAND);

    if (LOGGING)
    {
      printCommandLog(command,m_serialPort);
    }
    return -1;
  }

  // Get the right handler for this command

  GCodeHandler *handler = getGCodeHandler(command->getCodeEnum());

  if (handler == NULL)
  {
    m_serialPort->Write(conv2Str(COMM_REPORT_BAD_CMD));
    CurrentState::getInstance()->printQAndNewLine(m_serialPort);
    CurrentState::getInstance()->setLastError(ERR_INVALID_COMMAND);

    m_serialPort->Write("R99 handler == NULL\r\n");
    return -1;
  }

  // Report start of command

  m_serialPort->Write(conv2Str((COMM_REPORT_CMD_START)));
  CurrentState::getInstance()->printQAndNewLine(m_serialPort);

  // Execute command with retry
  CurrentState::getInstance()->setLastError(0);
  while (attempt < 1 || (!emergencyStop && attempt < maximumAttempts && execution != 0 && execution != 2))
  // error 2 is timeout error: stop movement retries
  {

    if (LOGGING || debugMessages)
    {
      //m_serialPort->Write("R99 attempt ");
      //m_serialPort->Write(attempt);
      //Serial.print(" from ");
      //Serial.print(maximumAttempts);
      //Serial.print("\r\n");
    }

    attempt++;
    if (attempt > 1)
    {
        m_serialPort->Write(conv2Str(COMM_REPORT_CMD_RETRY));
      CurrentState::getInstance()->printQAndNewLine(m_serialPort);
    }
    //std::thread t(&GCodeHandler::execute,handler, command, m_serialPort);
    handler->execute(command,m_serialPort);
    execution = CurrentState::getInstance()->getLastError();
    emergencyStop = CurrentState::getInstance()->isEmergencyStop();

    if (LOGGING || debugMessages)
    {
      //Serial.print("R99 execution ");
      //Serial.print(execution);
      //Serial.print("\r\n");
    }
  }

  // Clean serial buffer
  while (m_serialPort->BytesToRead>0)
  {
      char mensaje=m_serialPort->ReadChar();
  }

  // if movemement failed after retry
  // and parameter for emergency stop is set
  // set the emergency stop

  if (execution != 0)
  {
    if (isMovement)
    {
      if (ParameterList::getInstance()->getValue(PARAM_E_STOP_ON_MOV_ERR) == 1)
      {
        CurrentState::getInstance()->setEmergencyStop();
      }
    }
  }

  // Report back result of execution
  if (execution == 0)
  {
      m_serialPort->Write(conv2Str(COMM_REPORT_CMD_DONE));
    CurrentState::getInstance()->printQAndNewLine(m_serialPort);
  }
  else
  {
      m_serialPort->Write(conv2Str(COMM_REPORT_CMD_ERROR));
      m_serialPort->Write(" V");
      m_serialPort->Write(conv2Str((const char*) CurrentState::getInstance()->getLastError()));
    CurrentState::getInstance()->printQAndNewLine(m_serialPort);
  }

  CurrentState::getInstance()->resetQ();
  return execution;
};

GCodeHandler *GCodeProcessor::getGCodeHandler(CommandCodeEnum codeEnum)
{

  GCodeHandler *handler = NULL;

  // These are if statements so they can be disabled as test
  // Usefull when running into memory issues again

  if (codeEnum == G00)
  {
    handler = G00Handler::getInstance();
  }

  if (codeEnum == G28)
  {
    handler = G28Handler::getInstance();
  }

  if (codeEnum == F09)
  {
    handler = F09Handler::getInstance();
  }

  if (codeEnum == F11)
  {
    handler = F11Handler::getInstance();
  }
  if (codeEnum == F12)
  {
    handler = F12Handler::getInstance();
  }
  if (codeEnum == F13)
  {
    handler = F13Handler::getInstance();
  }

  if (codeEnum == F14)
  {
    handler = F14Handler::getInstance();
  }
  if (codeEnum == F15)
  {
    handler = F15Handler::getInstance();
  }
  if (codeEnum == F16)
  {
    handler = F16Handler::getInstance();
  }

  if (codeEnum == F20)
  {
    handler = F20Handler::getInstance();
  }
  if (codeEnum == F21)
  {
    handler = F21Handler::getInstance();
  }
  if (codeEnum == F22)
  {
    handler = F22Handler::getInstance();
  }

  //	if (codeEnum == F31) {handler = F31Handler::getInstance();}
  //	if (codeEnum == F32) {handler = F32Handler::getInstance();}

  if (codeEnum == F41)
  {
    handler = F41Handler::getInstance();
  }
  if (codeEnum == F42)
  {
    handler = F42Handler::getInstance();
  }
  if (codeEnum == F43)
  {
    handler = F43Handler::getInstance();
  }
  if (codeEnum == F44)
  {
    handler = F44Handler::getInstance();
  }

  if (codeEnum == F61)
  {
    handler = F61Handler::getInstance();
  }

  if (codeEnum == F81)
  {
    handler = F81Handler::getInstance();
  }
  if (codeEnum == F82)
  {
    handler = F82Handler::getInstance();
  }
  if (codeEnum == F83)
  {
    handler = F83Handler::getInstance();
  }
  if (codeEnum == F84)
  {
    handler = F84Handler::getInstance();
  }

  return handler;
}
System::String^ GCodeProcessor::conv2Str(const char* mensaje)
{
    System::String^ conversion = gcnew System::String(mensaje);
    return  conversion;
}
