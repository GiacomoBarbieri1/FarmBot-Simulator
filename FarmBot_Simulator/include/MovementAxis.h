/*
 * MovementAxis.h
 *
 *  Created on: 18 juli 2015
 *      Author: Tim Evers
 */

#ifndef MovementAXIS_H_
#define MovementAXIS_H_


#include "CurrentState.h"
#include <chrono>
#include "ParameterList.h"
#include "pins.h"
//#include "MovementEncoder.h"//NOT IMPLEMENTED YET
#include "Config.h"
#include <stdio.h>
#include <stdlib.h>
#include "Board.h"
#include "ArduinoPins.h"
#include <chrono>



class MovementAxis
{

public:
  MovementAxis();

#if defined(FARMDUINO_EXP_V20)
  TMC2130_Basics *TMC2130A;
  TMC2130_Basics *TMC2130B;
#endif

  void loadPinNumbers(int step, int dir, int enable, int min, int max, int step2, int dir2, int enable2);
  void loadMotorSettings(long speedMax, long speedMin, long speedHome, long stepsAcc, long timeOut, bool homeIsUp, bool motorInv, bool endStInv, bool endStInv2, long interruptSpeed, bool motor2Enbl, bool motor2Inv, bool endStEnbl, bool stopAtHome, long maxSize, bool stopAtMax);
  bool loadCoordinates(long sourcePoint, long destinationPoint, bool home);
  void setMaxSpeed(long speed);

  void enableMotor();
  void disableMotor();
  void checkMovement(std::chrono::time_point<std::chrono::system_clock> p_CurrentTime);
  void incrementTick();
  void checkTiming();
  void setTicks();

  bool isAxisActive();
  void deactivateAxis();
  bool isAccelerating();
  bool isDecelerating();
  bool isCruising();
  bool isCrawling();
  bool isMotorActive();
  //bool isMoving();

  bool endStopMin();
  bool endStopMax();
  bool endStopsReached();
  bool endStopAxisReached(bool movement_forward);

  long currentPosition();
  void setCurrentPosition(long newPos);
  long destinationPosition();

  void setStepAxis();
  void setMotorStep();
  void resetMotorStep();

  void setDirectionUp();
  void setDirectionDown();
  void setDirectionHome();
  void setDirectionAway();
  void setDirectionAxis();

  void setMovementUp();
  void setMovementDown();

  long getLastPosition();
  void setLastPosition(long position);

  bool movingToHome();
  bool movingUp();

  bool isStepDone();
  void resetStepDone();

  void activateDebugPrint();
  void test();

  void setStartTime(std::chrono::time_point<std::chrono::system_clock> p_tStart);

  char channelLabel;
  bool movementStarted;

#if defined(FARMDUINO_EXP_V20)
  void initTMC2130();
  void loadSettingsTMC2130(int motorCurrent, int  stallSensitivity, int microSteps);
  bool stallDetected();
  uint16_t getLoad();
#endif

#if defined(FARMDUINO_EXP_V20)
  void setMotorStepWriteTMC2130();
  void setMotorStepWriteTMC2130_2();
  void resetMotorStepWriteTMC2130();
  void resetMotorStepWriteTMC2130_2();

  unsigned int getLostSteps();

  bool tmcStep = true;
  bool tmcStep2 = true;
#endif

private:
    //Time Managers
    std::chrono::time_point<std::chrono::system_clock> MovStart;
    std::chrono::time_point<std::chrono::system_clock> CurrentTime;
    std::chrono::duration<double> elapsed_seconds;

  int lastCalcLog = 0;
  bool debugPrint = false;

  // pin settings primary motor
  int pinStep;
  int pinDirection;
  int pinEnable;

  // pin settings primary motor
  int pin2Step;
  int pin2Direction;
  int pin2Enable;

  // pin settings primary motor
  int pinMin;
  int pinMax;

  // motor settings
  bool motorEndStopInv;  // switch places of end stops
  bool motorEndStopInv2; // invert input (true/normal open, falce/normal closed) of end stops
  bool motorEndStopEnbl; // enable the use of the end stops

  // motor settings
  long motorSpeedMax        = 300;    // maximum speed in steps per second
  long motorSpeedMin        = 50;     // minimum speed in steps per second
  long motorSpeedHome       = 50;     // homing speed in steps per second
  long motorStepsAcc        = 300;    // number of steps used for acceleration
  long motorTimeOut         = 120;    // timeout in seconds
  bool motorHomeIsUp        = false;  // direction to move when reached 0 on axis but no end stop detected while homing
  bool motorMotorInv        = false;  // invert motor direction
  bool motorMotor2Enl       = false;  // enable secondary motor
  bool motorMotor2Inv       = false;  // invert secondary motor direction
  long motorInterruptSpeed  = 100;    // period of interrupt in micro seconds
  bool motorStopAtHome      = false;  // stop at home position or also use other side of the axis
  long motorMaxSize         = 0;      // maximum size of the axis in steps
  bool motorStopAtMax       = false;  // stop at the maximum size

  // coordinates
  long coordSourcePoint     = 0;      // all coordinated in steps
  long coordCurrentPoint    = 0;
  long coordDestinationPoint = 0;
  bool coordHomeAxis        = false; // homing command

  long axisLastPosition     = 0;

  // movement handling
  unsigned long movementLength  = 0;
  unsigned long maxLenth        = 0;
  unsigned long moveTicks       = 0;
  unsigned long stepOnTick      = 0;
  unsigned long stepOffTick     = 0;
  unsigned long axisSpeed       = 0;

  bool axisActive           = false;
  bool movementUp           = false;
  bool movementToHome       = false;
  bool movementStepDone     = false;
  bool movementAccelerating = false;
  bool movementDecelerating = false;
  bool movementCruising     = false;
  bool movementCrawling     = false;
  bool movementMotorActive  = false;
  bool movementMoving       = false;
  bool stepIsOn             = false;

  void step(long &currentPoint, unsigned long steps, long destinationPoint);
  bool pointReached(long currentPoint, long destinationPoint);
  unsigned int calculateSpeed(long sourcePosition, long currentPosition, long destinationPosition, long minSpeed, long maxSpeed, long stepsAccDec);
  unsigned long getLength(long l1, long l2);
  void checkAxisDirection();


  void (MovementAxis::*setMotorStepWrite)();
  void (MovementAxis::*setMotorStepWrite2)();
  void (MovementAxis::*resetMotorStepWrite)();
  void (MovementAxis::*resetMotorStepWrite2)();

  void setMotorStepWriteDefault();
  void setMotorStepWriteDefault2();
  void resetMotorStepWriteDefault();
  void resetMotorStepWriteDefault2();
  //void setMotorStepWrite54();
  //void resetMotorStepWrite54();
  //void setMotorStepWrite26();
  //void resetMotorStepWrite26();
  //void setMotorStepWrite60();
  //void resetMotorStepWrite60();
  //void setMotorStepWrite46();
  //void resetMotorStepWrite46();

};

#endif /* MovementAXIS_H_ */
