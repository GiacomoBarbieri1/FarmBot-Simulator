/*
 * CurrentState.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#include "CurrentState.h"

static CurrentState *instance;
long x = 0;
long y = 0;
long z = 0;
unsigned int speed = 0;
bool endStopState[3][2];
long Q = 0;
int lastError = 0;

CurrentState *CurrentState::getInstance()
{
  if (!instance)
  {
    instance = new CurrentState();
  };
  return instance;
};

CurrentState::CurrentState()
{
  x = 0;
  y = 0;
  z = 0;
  speed = 0;
  Q = 0;
  lastError = 0;
}

long CurrentState::getX()
{
  return x;
}

long CurrentState::getY()
{
  return y;
}

long CurrentState::getZ()
{
  return z;
}

long *CurrentState::getPoint()
{
  static long currentPoint[3] = {x, y, z};
  return currentPoint;
}

void CurrentState::setX(long newX)
{
  x = newX;
}

void CurrentState::setY(long newY)
{
  y = newY;
}

void CurrentState::setZ(long newZ)
{
  z = newZ;
}

int CurrentState::getLastError()
{
  return lastError;
}

void CurrentState::setLastError(int error)
{
  lastError = error;
}

void CurrentState::setEndStopState(unsigned int axis, unsigned int position, bool state)
{
  endStopState[axis][position] = state;
}

void CurrentState::setStepsPerMm(long stepsX, long stepsY, long stepsZ)
{
  stepsPerMmX = stepsX;
  stepsPerMmY = stepsY;
  stepsPerMmZ = stepsZ;
}

void CurrentState::storeEndStops()
{
  //CurrentState::getInstance()->setEndStopState(0, 0, digitalRead(X_MIN_PIN));
  //CurrentState::getInstance()->setEndStopState(0, 1, digitalRead(X_MAX_PIN));
  //CurrentState::getInstance()->setEndStopState(1, 0, digitalRead(Y_MIN_PIN));
  //CurrentState::getInstance()->setEndStopState(1, 1, digitalRead(Y_MAX_PIN));
  //CurrentState::getInstance()->setEndStopState(2, 0, digitalRead(Z_MIN_PIN));
  //CurrentState::getInstance()->setEndStopState(2, 1, digitalRead(Z_MAX_PIN));
}

void CurrentState::printPosition(System::IO::Ports::SerialPort^ p_serialPort)
{

  if (stepsPerMmX <= 0) { stepsPerMmX = 1; }
  if (stepsPerMmY <= 0) { stepsPerMmY = 1; }
  if (stepsPerMmZ <= 0) { stepsPerMmZ = 1; }

  p_serialPort->Write("R82");
  p_serialPort->Write(" X");
  std::string pos = std::to_string((float)x / (float)stepsPerMmX);
  p_serialPort->Write(gcnew System::String(pos.c_str()));
  p_serialPort->Write(" Y");
  pos = std::to_string((float)y / (float)stepsPerMmY);
  p_serialPort->Write(gcnew System::String(pos.c_str()));
  p_serialPort->Write(" Z");
  pos = std::to_string((float)z / (float)stepsPerMmZ);
  p_serialPort->Write(gcnew System::String(pos.c_str()));
  printQAndNewLine(p_serialPort);
}

System::String^ CurrentState::getPosition()
{

  if (stepsPerMmX <= 0) { stepsPerMmX = 1; }
  if (stepsPerMmY <= 0) { stepsPerMmY = 1; }
  if (stepsPerMmZ <= 0) { stepsPerMmZ = 1; }

  std::string output = "";

  output += "R82";
  output += " X";
  output += std::to_string((float)x / (float)stepsPerMmX * 1.0);
  output += " Y";
  output += std::to_string((float)y / (float)stepsPerMmY * 1.0);
  output += " Z";
  output += std::to_string((float)z / (float)stepsPerMmZ * 1.0);

  return gcnew System::String(output.c_str())+getQAndNewLine();
}

void CurrentState::printBool(bool value)
{
  //if (value)
  //{
  //  Serial.print("1");
  //}
  //else
  //{
  //  Serial.print("0");
  //}
}

void CurrentState::printEndStops()
{
  //Serial.print("R81");
  //Serial.print(" XA");
  //printBool(endStopState[0][0]);
  //Serial.print(" XB");
  //printBool(endStopState[0][1]);
  //Serial.print(" YA");
  //printBool(endStopState[1][0]);
  //Serial.print(" YB");
  //printBool(endStopState[1][1]);
  //Serial.print(" ZA");
  //printBool(endStopState[2][0]);
  //Serial.print(" ZB");
  //printBool(endStopState[2][1]);
  ////Serial.print("\r\n");
  //printQAndNewLine();
}

void CurrentState::print(System::IO::Ports::SerialPort^ p_serialPort)
{
  printPosition(p_serialPort);
  printEndStops();
}

void CurrentState::setQ(long q)
{
  Q = q;
}

void CurrentState::resetQ()
{
  Q = 0;
}

void CurrentState::printQAndNewLine(System::IO::Ports::SerialPort^ p_serialPort)
{
    std::string mensaje = " Q";
    mensaje+= std::to_string(Q);
    mensaje += "\r\n";
    p_serialPort->Write(gcnew System::String(mensaje.c_str()));
}

System::String^ CurrentState::getQAndNewLine()
{
    System::String^ output = gcnew System::String("");

  output += " Q";
  output += Q;
  output += "\r\n";

  return output;
}

void CurrentState::setEmergencyStop()
{
  emergencyStop = true;
}

void CurrentState::resetEmergencyStop()
{
  emergencyStop = false;
}

bool CurrentState::isEmergencyStop()
{
  return emergencyStop;
}
System::String^ CurrentState::conv2Str(const char* mensaje)
{
    System::String^ conversion = gcnew System::String(mensaje);
    return  conversion;
}
System::String^ CurrentState::conv2Str(char* mensaje)
{
    System::String^ conversion = gcnew System::String(mensaje);
    return  conversion;
}

float CurrentState::getPosX()
{ 
    if (stepsPerMmX <= 0) { stepsPerMmX = 1; }
    return (float)x / (float)stepsPerMmX;
}

float CurrentState::getPosY()
{
    if (stepsPerMmY <= 0) { stepsPerMmY = 1; }
    return (float)y / (float)stepsPerMmY;
}

float CurrentState::getPosZ()
{
    if (stepsPerMmZ <= 0) { stepsPerMmZ = 1; }
    return (float)z / (float)stepsPerMmZ;
}
