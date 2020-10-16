/*
 * GCodeHandler.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#include "GCodeHandler.h"

//GCodeHandler::GCodeHandler(CommandCodeEnum codeEnum)
//{
//    GCodeHandler* handler = NULL;
//
//    // These are if statements so they can be disabled as test
//    // Usefull when running into memory issues again
//
//    if (codeEnum == G00)
//    {
//        handler = G00Handler::getInstance();
//    }
//
//    if (codeEnum == G28)
//    {
//        handler = G28Handler::getInstance();
//    }
//
//    if (codeEnum == F09)
//    {
//        handler = F09Handler::getInstance();
//    }
//
//    if (codeEnum == F11)
//    {
//        handler = F11Handler::getInstance();
//    }
//    if (codeEnum == F12)
//    {
//        handler = F12Handler::getInstance();
//    }
//    if (codeEnum == F13)
//    {
//        handler = F13Handler::getInstance();
//    }
//
//    if (codeEnum == F14)
//    {
//        handler = F14Handler::getInstance();
//    }
//    if (codeEnum == F15)
//    {
//        handler = F15Handler::getInstance();
//    }
//    if (codeEnum == F16)
//    {
//        handler = F16Handler::getInstance();
//    }
//
//    if (codeEnum == F20)
//    {
//        handler = F20Handler::getInstance();
//    }
//    if (codeEnum == F21)
//    {
//        handler = F21Handler::getInstance();
//    }
//    if (codeEnum == F22)
//    {
//        handler = F22Handler::getInstance();
//    }
//
//    //	if (codeEnum == F31) {handler = F31Handler::getInstance();}
//    //	if (codeEnum == F32) {handler = F32Handler::getInstance();}
//
//    if (codeEnum == F41)
//    {
//        handler = F41Handler::getInstance();
//    }
//    if (codeEnum == F42)
//    {
//        handler = F42Handler::getInstance();
//    }
//    if (codeEnum == F43)
//    {
//        handler = F43Handler::getInstance();
//    }
//    if (codeEnum == F44)
//    {
//        handler = F44Handler::getInstance();
//    }
//
//    if (codeEnum == F61)
//    {
//        handler = F61Handler::getInstance();
//    }
//
//    if (codeEnum == F81)
//    {
//        handler = F81Handler::getInstance();
//    }
//    if (codeEnum == F82)
//    {
//        handler = F82Handler::getInstance();
//    }
//    if (codeEnum == F83)
//    {
//        handler = F83Handler::getInstance();
//    }
//    if (codeEnum == F84)
//    {
//        handler = F84Handler::getInstance();
//    }
//}

GCodeHandler::GCodeHandler()
{
}

GCodeHandler::~GCodeHandler()
{
}

int GCodeHandler::execute(Command *, System::IO::Ports::SerialPort^ m_serialPort)
{
  return -1;
}
