/*
 * GCodeProcessor.h
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#ifndef GCODEPROCESSOR_H_
#define GCODEPROCESSOR_H_

#include "Command.h"
#include "Config.h"

#include "Debug.h"

#include <thread>
#include "CurrentState.h"

#include "GCodeHandler.h"
#include "G00Handler.h"
#include "G28Handler.h"

#include "F09Handler.h"

#include "F11Handler.h"
#include "F12Handler.h"
#include "F13Handler.h"

#include "F14Handler.h"
#include "F15Handler.h"
#include "F16Handler.h"

#include "F20Handler.h"
#include "F21Handler.h"
#include "F22Handler.h"

//#include "F31Handler.h"
//#include "F32Handler.h"

#include "F41Handler.h"
#include "F42Handler.h"
#include "F43Handler.h"
#include "F44Handler.h"

#include "F61Handler.h"

#include "F81Handler.h"
#include "F82Handler.h"
#include "F83Handler.h"
#include "F84Handler.h"

class GCodeProcessor
{
private:
    //System::IO::Ports::SerialPort m_serial;
public:
  GCodeProcessor();
  virtual ~GCodeProcessor();
  int execute(Command *command, System::IO::Ports::SerialPort^ m_serialPort);

protected:
  GCodeHandler *getGCodeHandler(CommandCodeEnum);

  System::String^ conv2Str(const char* mensaje);

private:
  void printCommandLog(Command *, System::IO::Ports::SerialPort^ m_serialPort);
  void executeHandler();
};

#endif /* GCODEPROCESSOR_H_ */
