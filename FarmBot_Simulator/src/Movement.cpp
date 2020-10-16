#include "Movement.h"


static Movement *instance;

Movement *Movement::getInstance()
{
  if (!instance)
  {
    instance = new Movement();
  };
  return instance;
};



void Movement::reportStatus(MovementAxis *axis, int axisStatus)
{  
  //serialBuffer += COMM_REPORT_CMD_STATUS;
  //serialBuffer += " ";
  //serialBuffer += axis->channelLabel;
  //serialBuffer += axisStatus;
  //serialBuffer += CurrentState::getInstance()->getQAndNewLine();

  //Serial.print(COMM_REPORT_CMD_STATUS);
  //Serial.print(" ");
  //Serial.print(axis->label);
  //Serial.print(axisStatus);
  //CurrentState::getInstance()->printQAndNewLine();
}//TODO

void Movement::reportCalib(MovementAxis *axis, int calibStatus)
{
/*  Serial.print(COMM_REPORT_CALIB_STATUS);
  Serial.print(" ");
  Serial.print(axis->channelLabel);
  Serial.print(calibStatus);
  CurrentState::getInstance()->printQAndNewLine();*/
}

void Movement::checkAxisSubStatus(MovementAxis *axis, int *axisSubStatus)
{
  int newStatus = 0;
  bool statusChanged = false;

  if (axis->isAccelerating())
  {
    newStatus = COMM_REPORT_MOVE_STATUS_ACCELERATING;
  }

  if (axis->isCruising())
  {
    newStatus = COMM_REPORT_MOVE_STATUS_CRUISING;
  }

  if (axis->isDecelerating())
  {
    newStatus = COMM_REPORT_MOVE_STATUS_DECELERATING;
  }

  if (axis->isCrawling())
  {
    newStatus = COMM_REPORT_MOVE_STATUS_CRAWLING;
  }

  // if the status changes, send out a status report
  if (*axisSubStatus != newStatus && newStatus > 0)
  {
    statusChanged = true;
  }
  *axisSubStatus = newStatus;

  if (statusChanged)
  {
    reportStatus(axis, *axisSubStatus);
  }
}

//const int MOVEMENT_INTERRUPT_SPEED = 100; // Interrupt cycle in micro seconds

Movement::Movement()
{

  // Initialize some variables for testing

  motorMotorsEnabled = false;

  motorConsMissedSteps[0] = 0;
  motorConsMissedSteps[1] = 0;
  motorConsMissedSteps[2] = 0;

  motorLastPosition[0] = 0;
  motorLastPosition[1] = 0;
  motorLastPosition[2] = 0;

  motorConsEncoderLastPosition[0] = 0;
  motorConsEncoderLastPosition[1] = 0;
  motorConsEncoderLastPosition[2] = 0;

  // Create the axis controllers

  axisX = MovementAxis();
  axisY = MovementAxis();
  axisZ = MovementAxis();

  axisX.channelLabel = 'X';
  axisY.channelLabel = 'Y';
  axisZ.channelLabel = 'Z';

  // Load settings

  loadSettings();

  motorMotorsEnabled = false;
}

void Movement::loadSettings()
{

  /**/ //Serial.println("== load pin numbers ==");

  // Load motor settings

  axisX.loadPinNumbers(X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, X_MIN_PIN, X_MAX_PIN, E_STEP_PIN, E_DIR_PIN, E_ENABLE_PIN);
  axisY.loadPinNumbers(Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, Y_MIN_PIN, Y_MAX_PIN, 0, 0, 0);
  axisZ.loadPinNumbers(Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, Z_MIN_PIN, Z_MAX_PIN, 0, 0, 0);

  axisSubStep[0] = COMM_REPORT_MOVE_STATUS_IDLE;
  axisSubStep[1] = COMM_REPORT_MOVE_STATUS_IDLE;
  axisSubStep[2] = COMM_REPORT_MOVE_STATUS_IDLE;

  /**/ //Serial.println("== load motor settings ==");

  loadMotorSettings();


}


void Movement::test()
{
  axisX.enableMotor();

  //axisY.test();

  //axisX.enableMotor();
  //axisX.setMotorStep();
  //delayMicroseconds(10);
  //axisX.setMotorStep();
  //delayMicroseconds(10);

  //digitalWrite(X_STEP_PIN, HIGH);
  //delayMicroseconds(10);
  //digitalWrite(X_STEP_PIN, LOW);
  //delayMicroseconds(10);

  //axisX.setMotorStepWriteTMC2130();
  //axisX.test();

  //Serial.print("R99");
  //Serial.print(" mot x = ");
  //Serial.print(axisX.currentPosition());
  //Serial.print(" enc x = ");
  //Serial.print(encoderX.currentPosition());
  //Serial.print("\r\n");

  //Serial.print("R99");
  //Serial.print(" mot y = ");
  //Serial.print(axisY.currentPosition());
  //Serial.print(" enc y = ");
  //Serial.print(encoderY.currentPosition());
  //Serial.print("\r\n");

  //Serial.print("R99");
  //Serial.print(" mot z = ");
  //Serial.print(axisZ.currentPosition());
  //Serial.print(" enc z = ");
  //Serial.print(encoderZ.currentPosition());
  //Serial.print("\r\n");

  // read changes in encoder
  //encoderX.readEncoder();
  //encoderY.readEncoder();
  //encoderZ.readEncoder();
  //reportPosition();

  //bool test = axisX.endStopMin();
  //Serial.print("R99");
  //Serial.print(" test = ");
  //Serial.print(test);
  //Serial.print("\r\n");
}

void Movement::test2()
{
  
  axisX.setMotorStep();
  //CurrentState::getInstance()->printPosition();
  //encoderX.test();
  //encoderY.test();
  //encoderZ.test();
}

/**
 * xDest - destination X in steps
 * yDest - destination Y in steps
 * zDest - destination Z in steps
 * maxStepsPerSecond - maximum number of steps per second
 * maxAccelerationStepsPerSecond - maximum number of acceleration in steps per second
 */
int Movement::moveToCoords(double xDestScaled, double yDestScaled, double zDestScaled,
                                 unsigned int xMaxSpd, unsigned int yMaxSpd, unsigned int zMaxSpd,
                                 bool xHome, bool yHome, bool zHome, System::IO::Ports::SerialPort^ serial)
{

  /**/ //test();
  
 
 //Turns The destination positions to Steps
  long xDest = xDestScaled * stepsPerMm[0];
  long yDest = yDestScaled * stepsPerMm[1];
  long zDest = zDestScaled * stepsPerMm[2];

  //Start the time variables
  unsigned long currentMillis = 0;
  std::chrono::time_point<std::chrono::system_clock> timeStart;

  //Parameters of serial
  serialMessageNr = 0;
  serialMessageDelay = 0;

  int incomingByte = 0;
  error = 0;
  emergencyStop = false;
  unsigned int commandSpeed[3];

  // if a speed is given in the command, use that instead of the config speed
  //For each axis
  if (xMaxSpd > 0 && xMaxSpd < speedMax[0])
  {
    commandSpeed[0] = xMaxSpd;
  }
  else
  {
    commandSpeed[0] = speedMax[0];
  }

  if (yMaxSpd > 0 && yMaxSpd < speedMax[1])
  {
    commandSpeed[1] = yMaxSpd;
  }
    else
  {
    commandSpeed[1] = speedMax[1];
  }

  if (zMaxSpd > 0 && zMaxSpd < speedMax[2])
  {
    commandSpeed[2] = zMaxSpd;
  }
    else
  {
    commandSpeed[2] = speedMax[2];
  }

  //Asigns the speeds to each axis
  axisX.setMaxSpeed(commandSpeed[0]);
  axisY.setMaxSpeed(commandSpeed[1]);
  axisZ.setMaxSpeed(commandSpeed[2]);

  // Load coordinates into axis class
  //Source point is the origin point
  //Current point is the position in the actual time
  long sourcePoint[3] = {0, 0, 0};

  sourcePoint[0] = CurrentState::getInstance()->getX();
  sourcePoint[1] = CurrentState::getInstance()->getY();
  sourcePoint[2] = CurrentState::getInstance()->getZ();

  //currentPoint = {0, 0, 0};
  currentPoint[0] = CurrentState::getInstance()->getX();
  currentPoint[1] = CurrentState::getInstance()->getY();
  currentPoint[2] = CurrentState::getInstance()->getZ();

  long destinationPoint[3] = {0, 0, 0};
  destinationPoint[0] = xDest;
  destinationPoint[1] = yDest;
  destinationPoint[2] = zDest;
  std::cout << "Destination point is:"<<xDest<<","<<yDest<<","<<zDest << std::endl;

  motorConsMissedSteps[0] = 0;
  motorConsMissedSteps[1] = 0;
  motorConsMissedSteps[2] = 0;

  motorConsMissedStepsPrev[0] = 0;
  motorConsMissedStepsPrev[1] = 0;
  motorConsMissedStepsPrev[2] = 0;

  motorLastPosition[0] = currentPoint[0];
  motorLastPosition[1] = currentPoint[1];
  motorLastPosition[2] = currentPoint[2];

  // Load coordinates into motor control

  // Report back coordinates if target coordinates changed

  if (axisX.loadCoordinates(currentPoint[0], destinationPoint[0], xHome))
  {
    serial->Write(System::Convert::ToString(COMM_REPORT_COORD_CHANGED_X));
    serial->Write(" X");
    serial->Write(System::Convert::ToString(axisX.destinationPosition() / stepsPerMm[0]));
    CurrentState::getInstance()->printQAndNewLine(serial);
  }

  if (axisY.loadCoordinates(currentPoint[1], destinationPoint[1], yHome))
  {
      serial->Write(System::Convert::ToString(COMM_REPORT_COORD_CHANGED_X));
      serial->Write(" Y");
      serial->Write(System::Convert::ToString(axisY.destinationPosition() / stepsPerMm[1]));
      CurrentState::getInstance()->printQAndNewLine(serial);
  }

  if (axisZ.loadCoordinates(currentPoint[2], destinationPoint[2], zHome))
  {
      serial->Write(System::Convert::ToString(COMM_REPORT_COORD_CHANGED_X));
      serial->Write(" Z");
      serial->Write(System::Convert::ToString(axisZ.destinationPosition() / stepsPerMm[2]));
      CurrentState::getInstance()->printQAndNewLine(serial);;
  }

  // Prepare for movement

  axisX.movementStarted = false;
  axisY.movementStarted = false;
  axisZ.movementStarted = false;

  storeEndStops();
  reportEndStops();

  axisX.setDirectionAxis();
  axisY.setDirectionAxis();
  axisZ.setDirectionAxis();

  // Enable motors

  axisSubStep[0] = COMM_REPORT_MOVE_STATUS_START_MOTOR;
  axisSubStep[1] = COMM_REPORT_MOVE_STATUS_START_MOTOR;
  axisSubStep[2] = COMM_REPORT_MOVE_STATUS_START_MOTOR;

  reportStatus(&axisX, axisSubStep[0]);
  reportStatus(&axisY, axisSubStep[1]);
  reportStatus(&axisZ, axisSubStep[2]);

  enableMotors();

  // Start movement

  axisActive[0] = true;
  axisActive[1] = true;
  axisActive[2] = true;

  if (xHome || yHome || zHome)
  {
    if (!xHome) { axisX.deactivateAxis(); }
    if (!yHome) { axisY.deactivateAxis(); }
    if (!zHome) { axisZ.deactivateAxis(); }

    axisActive[0] = xHome;
    axisActive[1] = yHome;
    axisActive[2] = zHome;
  }

  //axisX.checkMovement();
  //axisY.checkMovement();
  //axisZ.checkMovement();

  axisX.setTicks();
  axisY.setTicks();
  axisZ.setTicks();

  emergencyStop = CurrentState::getInstance()->isEmergencyStop();
  
  unsigned long loopCounts = 0;
  timeStart = std::chrono::system_clock::now();
  axisX.setStartTime(timeStart);
  axisY.setStartTime(timeStart);
  axisZ.setStartTime(timeStart);
  std::cout << "StartedMoving" << std::endl;
  // Let the interrupt handle all the movements
  //std::thread t(&Movement::handleMovementInterrupt,this);
  while ((axisActive[0] || axisActive[1] || axisActive[2]) && !emergencyStop)
  {
      
    /**/
    if (loopCounts % 1000 == 0)
    {

      //serial->Write("R99");
      //serial->Write(" missed step ");
      //serial->Write(motorConsMissedSteps[1]);
      //serial->Write(" axis pos ");
      //serial->Write(axisY.currentPosition());
      //Serial.print("\r\n");

      //Serial.print("X - ");
      //axisX.test();

      //Serial.print("Y - ");
      //axisY.test();

    }
    loopCounts++;
    

    checkAxisSubStatus(&axisX, &axisSubStep[0]);
    checkAxisSubStatus(&axisY, &axisSubStep[1]);
    checkAxisSubStatus(&axisZ, &axisSubStep[2]);

    //axisX.checkTiming();
    //axisY.checkTiming();
    //axisZ.checkTiming();

    //if (axisX.isStepDone())
    //{
      axisX.checkMovement(std::chrono::system_clock::now());
  /*    axisX.resetStepDone();
    }*/

    //if (axisY.isStepDone())
    //{
        axisY.checkMovement(std::chrono::system_clock::now());
    //  axisY.resetStepDone();
    //}

    //if (axisZ.isStepDone())
    //{
      axisZ.checkMovement(std::chrono::system_clock::now());
    //  axisZ.resetStepDone();
    //}

    if (axisX.isAxisActive() && motorConsMissedSteps[0] > motorConsMissedStepsMax[0])
    {
      axisX.deactivateAxis();
      serialBuffer += "R99";
      serialBuffer += " deactivate motor X due to missed steps";
      serialBuffer += "\r\n";

      if (xHome)
      {
        axisX.setCurrentPosition(0);
      }
      else
      {
        error = ERR_STALL_DETECTED;
      }
    }

    if (axisY.isAxisActive() && motorConsMissedSteps[1] > motorConsMissedStepsMax[1])
    {
      axisY.deactivateAxis();
      serialBuffer += "R99";
      serialBuffer += " deactivate motor Y due to missed steps";
      serialBuffer += "\r\n";
      
      if (yHome)
      {
        axisY.setCurrentPosition(0);
      }
      else
      {
        error = ERR_STALL_DETECTED;
      }
    }

    if (axisZ.isAxisActive() && motorConsMissedSteps[2] > motorConsMissedStepsMax[2])
    {
      axisZ.deactivateAxis();

      serialBuffer += "R99";
      serialBuffer += " deactivate motor Z due to missed steps";
      serialBuffer += "\r\n";

      if (zHome)
      {
        //encoderZ.setPosition(0);
        axisZ.setCurrentPosition(0);        
      }
      else
      {
        error = ERR_STALL_DETECTED;
      }
    }

    if (axisX.endStopAxisReached(false))
    {
      axisX.setCurrentPosition(0);
    }

    if (axisY.endStopAxisReached(false))
    {
      axisY.setCurrentPosition(0);
    }

    if (axisZ.endStopAxisReached(false))
    {
      axisZ.setCurrentPosition(0);
    }

    axisActive[0] = axisX.isAxisActive();
    axisActive[1] = axisY.isAxisActive();
    axisActive[2] = axisZ.isAxisActive();

    currentPoint[0] = axisX.currentPosition();
    currentPoint[1] = axisY.currentPosition();
    currentPoint[2] = axisZ.currentPosition();

    CurrentState::getInstance()->setX(currentPoint[0]);
    CurrentState::getInstance()->setY(currentPoint[1]);
    CurrentState::getInstance()->setZ(currentPoint[2]);
    std::cout << currentPoint[0] << " ," << currentPoint[1] << " ," << currentPoint[2] << std::endl;

    storeEndStops();

    // Check timeouts
    /*if (axisActive[0] == true && ((millis() >= timeStart && millis() - timeStart > timeOut[0] * 1000) || (millis() < timeStart && millis() > timeOut[0] * 1000)))
    {
      serialBuffer += COMM_REPORT_TIMEOUT_X;
      serialBuffer += "\r\n";
      serialBuffer += "R99 timeout X axis\r\n";
      error = ERR_TIMEOUT;
    }*/
   /* if (axisActive[1] == true && ((millis() >= timeStart && millis() - timeStart > timeOut[1] * 1000) || (millis() < timeStart && millis() > timeOut[1] * 1000)))
    {
      serialBuffer += COMM_REPORT_TIMEOUT_Y;
      serialBuffer += "\r\n";
      serialBuffer += "R99 timeout Y axis\r\n";
      error = ERR_TIMEOUT;
    }*/
    /*if (axisActive[2] == true && ((millis() >= timeStart && millis() - timeStart > timeOut[2] * 1000) || (millis() < timeStart && millis() > timeOut[2] * 1000)))
    {
      serialBuffer += COMM_REPORT_TIMEOUT_Z;
      serialBuffer += "\r\n";
      serialBuffer += "R99 timeout Z axis\r\n";
      error = ERR_TIMEOUT;
    }*/

    // Check if there is an emergency stop command
    //if (Serial.available() > 0)
    //{
    //  incomingByte = Serial.read();
    //  if (incomingByte == 69 || incomingByte == 101)
    //  {
    //    serialBuffer += "R99 emergency stop\r\n";

    //    Serial.print(COMM_REPORT_EMERGENCY_STOP);
    //    //CurrentState::getInstance()->printQAndNewLine();

    //    emergencyStop = true;

    //    axisX.deactivateAxis();
    //    axisY.deactivateAxis();
    //    axisZ.deactivateAxis();

    //    axisActive[0] = false;
    //    axisActive[1] = false;
    //    axisActive[2] = false;

    //    error = ERR_EMERGENCY_STOP;
    //  }
    //}

    if (error != 0)
    {
      serialBuffer += "R99 error\r\n";

      axisActive[0] = false;
      axisActive[1] = false;
      axisActive[2] = false;
    }

    // Send the serial buffer one character per cycle to keep motor timing more accuracte
    serialBufferSendNext();

    // Periodically send message still active
    currentMillis++;
    serialMessageDelay++;

    if (serialMessageDelay > 300 && serialBuffer.length() == 0 && serialBufferSending == 0)
    {

      //Serial.print("Y-");
      //axisY.test();/**/

      serialMessageDelay = 0;

      switch(serialMessageNr)
      {
        case 0:
          /*serialBuffer += COMM_REPORT_CMD_BUSY;
          serialBuffer += CurrentState::getInstance()->getQAndNewLine();*/
          break;
        case 1:
          /*serialBuffer += CurrentState::getInstance()->getPosition();
          serialBuffer += CurrentState::getInstance()->getQAndNewLine();*/
          break;

        default:
          break;
      }

      serialMessageNr++;

      #if !defined(FARMDUINO_EXP_V20)
      if (serialMessageNr > 1)
      {
        serialMessageNr = 0;
      }
      #endif

      serialBufferSending = 0;
      
    //  if (debugMessages /*&& debugInterrupt*/)
    //  {

				//Serial.print("R99");
				//Serial.print(" missed step ");
				//Serial.print(motorConsMissedSteps[1]);
				//Serial.print(" encoder pos ");
				//Serial.print(encoderY.currentPosition());
				//Serial.print(" axis pos ");
				//Serial.print(axisY.currentPosition());
				//Serial.print("\r\n");
    //  }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  }
  std::cout << "Finished Moving" << std::endl;
  serialBufferEmpty();
  //Serial.print("R99 stopped\r\n");

  // Send feedback for homing

  if (xHome && !error && !emergencyStop)
  {
    //Serial.print(COMM_REPORT_HOMED_X);
    //CurrentState::getInstance()->printQAndNewLine();
  }

  if (yHome && !error && !emergencyStop)
  {
    //Serial.print(COMM_REPORT_HOMED_Y);
    //CurrentState::getInstance()->printQAndNewLine();
  }

  if (zHome && !error && !emergencyStop)
  {
    //Serial.print(COMM_REPORT_HOMED_Z);
    //CurrentState::getInstance()->printQAndNewLine();
  }

  // Stop motors

  axisSubStep[0] = COMM_REPORT_MOVE_STATUS_STOP_MOTOR;
  axisSubStep[1] = COMM_REPORT_MOVE_STATUS_STOP_MOTOR;
  axisSubStep[2] = COMM_REPORT_MOVE_STATUS_STOP_MOTOR;

  reportStatus(&axisX, axisSubStep[0]);
  reportStatus(&axisY, axisSubStep[1]);
  reportStatus(&axisZ, axisSubStep[2]);

  disableMotors();

  // Report end statuses

  currentPoint[0] = axisX.currentPosition();
  currentPoint[1] = axisY.currentPosition();
  currentPoint[2] = axisZ.currentPosition();

  CurrentState::getInstance()->setX(currentPoint[0]);
  CurrentState::getInstance()->setY(currentPoint[1]);
  CurrentState::getInstance()->setZ(currentPoint[2]);
  

  storeEndStops();
  reportEndStops();
  reportPosition();

  // Report axis idle

  axisSubStep[0] = COMM_REPORT_MOVE_STATUS_IDLE;
  axisSubStep[1] = COMM_REPORT_MOVE_STATUS_IDLE;
  axisSubStep[2] = COMM_REPORT_MOVE_STATUS_IDLE;

  reportStatus(&axisX, axisSubStep[0]);
  reportStatus(&axisY, axisSubStep[1]);
  reportStatus(&axisZ, axisSubStep[2]);

  disableMotors();

  if (emergencyStop)
  {
    CurrentState::getInstance()->setEmergencyStop();
    error = ERR_EMERGENCY_STOP;
  }

  //Serial.print("R99 error ");
  //Serial.print(error);
  //Serial.print("\r\n");


  // Return error
  CurrentState::getInstance()->setLastError(error);

  return error;
}

void Movement::serialBufferEmpty()
{
  while (serialBuffer.length() > 0)
  {
    serialBufferSendNext();
  }
}

void Movement::serialBufferSendNext()//TODO
{
  //// Send the next char in the serialBuffer
  //if (serialBuffer.length() > 0)
  //{

  //  if (serialBufferSending < serialBuffer.length())
  //  {
  //    //Serial.print("-");
  //    switch (serialBuffer.charAt(serialBufferSending))
  //    {
  //    case 13:
  //      Serial.print("\r\n");
  //      break;
  //    case 10:
  //      break;
  //    default:
  //      Serial.print(serialBuffer.charAt(serialBufferSending));
  //      break;
  //    }

  //    serialBufferSending++;
  //  }
  //  else
  //  {
  //    // Reset length of buffer when done
  //    serialBuffer = "";
  //    serialBufferSending = 0;
  //  }
  //}
}

//
// Calibration
//
//Falta por hacer este metodo
int Movement::calibrateAxis(int axis)
{

  // Load motor and encoder settings

  loadMotorSettings();
  //loadEncoderSettings();

  //unsigned long timeStart             = millis();

  bool movementDone = false;

  int paramValueInt = 0;
  int stepsCount = 0;
  int incomingByte = 0;
  int error = 0;

  bool invertEndStops = false;
  int parEndInv;
  int parNbrStp;

  float *missedSteps;
  int *missedStepsMax;
  long *lastPosition;
  float *encoderStepDecay;
  bool *encoderEnabled;
  int *axisStatus;
  long *axisStepsPerMm;

  // Prepare for movement

  storeEndStops();
  reportEndStops();

  // Select the right axis
  MovementAxis *calibAxis;
  //MovementEncoder *calibEncoder;

  switch (axis)
  {
  case 0:
    calibAxis = &axisX;
    //calibEncoder = &encoderX;
    parEndInv = MOVEMENT_INVERT_ENDPOINTS_X;
    parNbrStp = MOVEMENT_AXIS_NR_STEPS_X;
    invertEndStops = endStInv[0];
    missedSteps = &motorConsMissedSteps[0];
    missedStepsMax = &motorConsMissedStepsMax[0];
    lastPosition = &motorLastPosition[0];
    encoderStepDecay = &motorConsMissedStepsDecay[0];
    encoderEnabled = &motorConsEncoderEnabled[0];
    axisStatus = &axisSubStep[0];
    axisStepsPerMm = &stepsPerMm[0];
    break;
  case 1:
    calibAxis = &axisY;
    //calibEncoder = &encoderY;
    parEndInv = MOVEMENT_INVERT_ENDPOINTS_Y;
    parNbrStp = MOVEMENT_AXIS_NR_STEPS_Y;
    invertEndStops = endStInv[1];
    missedSteps = &motorConsMissedSteps[1];
    missedStepsMax = &motorConsMissedStepsMax[1];
    lastPosition = &motorLastPosition[1];
    encoderStepDecay = &motorConsMissedStepsDecay[1];
    encoderEnabled = &motorConsEncoderEnabled[1];
    axisStatus = &axisSubStep[1];
    axisStepsPerMm = &stepsPerMm[1];
    break;
  case 2:
    calibAxis = &axisZ;
    //calibEncoder = &encoderZ;
    parEndInv = MOVEMENT_INVERT_ENDPOINTS_Z;
    parNbrStp = MOVEMENT_AXIS_NR_STEPS_Z;
    invertEndStops = endStInv[2];
    missedSteps = &motorConsMissedSteps[2];
    missedStepsMax = &motorConsMissedStepsMax[2];
    lastPosition = &motorLastPosition[2];
    encoderStepDecay = &motorConsMissedStepsDecay[2];
    encoderEnabled = &motorConsEncoderEnabled[2];
    axisStatus = &axisSubStep[2];
    axisStepsPerMm = &stepsPerMm[2];
    break;
  default:
    //Serial.print("R99 Calibration error: invalid axis selected\r\n");
    error = 1;
    CurrentState::getInstance()->setLastError(error);
    return error;
  }

  // Preliminary checks

  if (calibAxis->endStopMin() || calibAxis->endStopMax())
  {
    //Serial.print("R99 Calibration error: end stop active before start\r\n");
    error = 1;
    CurrentState::getInstance()->setLastError(error);
    return error;
  }

  /*Serial.print("R99");
  Serial.print(" axis ");
  Serial.print(calibAxis->channelLabel);
  Serial.print(" move to start for calibration");
  Serial.print("\r\n");*/

  *axisStatus = COMM_REPORT_MOVE_STATUS_START_MOTOR;
  reportStatus(calibAxis, axisStatus[0]);

  // Move towards home
  calibAxis->enableMotor();
  
  /**/
  //calibAxis->setDirectionHome();
  calibAxis->setDirectionAway();

  //calibAxis->setCurrentPosition(calibEncoder->currentPosition());

  stepsCount = 0;
  *missedSteps = 0;
  movementDone = false;

  motorConsMissedSteps[0] = 0;
  motorConsMissedSteps[1] = 0;
  motorConsMissedSteps[2] = 0;

  *axisStatus = COMM_REPORT_MOVE_STATUS_CRAWLING;
  reportStatus(calibAxis, axisStatus[0]);

  reportCalib(calibAxis, COMM_REPORT_CALIBRATE_STATUS_TO_HOME);

  while (!movementDone && error == 0)
  {

    #if defined(FARMDUINO_V14)
     //checkEncoders();
    #endif

    //checkAxisVsEncoder(calibAxis, calibEncoder, &motorConsMissedSteps[axis], &motorLastPosition[axis], &motorConsEncoderLastPosition[axis], &motorConsEncoderUseForPos[axis], &motorConsMissedStepsDecay[axis], &motorConsEncoderEnabled[axis]);

    // Check if there is an emergency stop command
    /*if (Serial.available() > 0)
    {
      incomingByte = Serial.read();
      if (incomingByte == 69 || incomingByte == 101)
      {
        Serial.print("R99 emergency stop\r\n");
        movementDone = true;
        CurrentState::getInstance()->setEmergencyStop();
        Serial.print(COMM_REPORT_EMERGENCY_STOP);
        CurrentState::getInstance()->printQAndNewLine();
        error = 1;
      }
    }*/

    // Move until any end stop is reached or the motor is skipping. That end should be the far end stop. First, ram the end at high speed.

    /**/
    //if (((!invertEndStops && !calibAxis->endStopMax()) || (invertEndStops && !calibAxis->endStopMin())) && !movementDone && (*missedSteps < *missedStepsMax))
    //if ((!calibAxis->endStopMin() && !calibAxis->endStopMax()) && !movementDone && (*missedSteps < *missedStepsMax))
    if ((!calibAxis->endStopMin() && !calibAxis->endStopMax()) && !movementDone && (*missedSteps < *missedStepsMax))
    {

      calibAxis->setStepAxis();
      

      //delayMicroseconds(100000 / speedHome[axis] / 2);

      stepsCount++;
      if (stepsCount % (speedHome[axis] * 3) == 0)
      {
        // Periodically send message still active
        //Serial.print(COMM_REPORT_CMD_BUSY);
        //CurrentState::getInstance()->printQAndNewLine();
      }

      if (debugMessages)
      {
        if (stepsCount % (speedHome[axis] / 6) == 0 /*|| *missedSteps > 3*/)
        {
         /* Serial.print("R99");
          Serial.print(" step count ");
          Serial.print(stepsCount);
          Serial.print(" missed steps ");
          Serial.print(*missedSteps);
          Serial.print(" max steps ");
          Serial.print(*missedStepsMax);
          Serial.print(" cur pos mtr ");
          Serial.print(calibAxis->currentPosition());
          Serial.print(" cur pos enc ");
          Serial.print(calibEncoder->currentPosition());
          Serial.print("\r\n");*/
        }
      }

      calibAxis->resetMotorStep();
     // delayMicroseconds(100000 / speedHome[axis] / 2);
    }
    else
    {
      movementDone = true;
      //Serial.print("R99 movement done\r\n");

      // If end stop for home is active, set the position to zero
      if (calibAxis->endStopMin())
      {
        invertEndStops = true;
      }
    }
  }

  reportCalib(calibAxis, COMM_REPORT_CALIBRATE_STATUS_TO_END);

  //Serial.print("R99");
  //Serial.print(" axis ");
  //Serial.print(calibAxis->channelLabel);
  //Serial.print(" at starting point");
  //Serial.print("\r\n");

  // Report back the end stop setting

  if (error == 0)
  {
    if (invertEndStops)
    {
      paramValueInt = 1;
    }
    else
    {
      paramValueInt = 0;
    }

    //Serial.print("R23");
    //Serial.print(" ");
    //Serial.print("P");
    //Serial.print(parEndInv);
    //Serial.print(" ");
    //Serial.print("V");
    //Serial.print(paramValueInt);
    ////Serial.print("\r\n");
    //CurrentState::getInstance()->printQAndNewLine();
  }

  // Store the status of the system

  storeEndStops();
  reportEndStops();

  // Move into the other direction now, and measure the number of steps

  //Serial.print("R99");
  //Serial.print(" axis ");
  //Serial.print(calibAxis->channelLabel);
  //Serial.print(" calibrating length");
  //Serial.print("\r\n");

  stepsCount = 0;
  movementDone = false;
  *missedSteps = 0;

  /**/
  //calibAxis->setDirectionAway();
  calibAxis->setDirectionHome();

 // calibAxis->setCurrentPosition(calibEncoder->currentPosition());

  motorConsMissedSteps[0] = 0;
  motorConsMissedSteps[1] = 0;
  motorConsMissedSteps[2] = 0;

  //long encoderStartPoint = calibEncoder->currentPosition();
  //long encoderEndPoint = calibEncoder->currentPosition();

  while (!movementDone && error == 0)
  {

    #if defined(FARMDUINO_V14)
       //checkEncoders();
    #endif

    //checkAxisVsEncoder(calibAxis, calibEncoder, &motorConsMissedSteps[axis], &motorLastPosition[axis], &motorConsEncoderLastPosition[axis], &motorConsEncoderUseForPos[axis], &motorConsMissedStepsDecay[axis], &motorConsEncoderEnabled[axis]);

    // Check if there is an emergency stop command
    //if (Serial.available() > 0)
    //{
    //  incomingByte = Serial.read();
    //  if (incomingByte == 69 || incomingByte == 101)
    //  {
    //    Serial.print("R99 emergency stop\r\n");
    //    movementDone = true;
    //    CurrentState::getInstance()->setEmergencyStop();
    //    Serial.print(COMM_REPORT_EMERGENCY_STOP);
    //    CurrentState::getInstance()->printQAndNewLine();
    //    error = 1;
    //  }
    //}

    // Ignore the missed steps at startup time
    if (stepsCount < 10)
    {
      *missedSteps = 0;
    }

    // Move until the end stop is at the home position by detecting the other end stop or missed steps are detected
    /**/
    //if ((!calibAxis->endStopMin() && !calibAxis->endStopMax()) && !movementDone && (*missedSteps < *missedStepsMax))
    //if (((!invertEndStops && !calibAxis->endStopMax()) || (invertEndStops && !calibAxis->endStopMin())) && !movementDone && (*missedSteps < *missedStepsMax))
    if (((!invertEndStops && !calibAxis->endStopMin()) || (invertEndStops && !calibAxis->endStopMax())) && !movementDone && (*missedSteps < *missedStepsMax))

    {

      calibAxis->setStepAxis();
      stepsCount++;

      //checkAxisVsEncoder(&axisX, &encoderX, &motorConsMissedSteps[0], &motorLastPosition[0], &motorConsMissedStepsDecay[0], &motorConsEncoderEnabled[0]);

      //delayMicroseconds(100000 / speedHome[axis] / 2);

      if (stepsCount % (speedHome[axis] * 3) == 0)
      {
        // Periodically send message still active
       // Serial.print(COMM_REPORT_CMD_BUSY);
        //Serial.print("\r\n");
        //CurrentState::getInstance()->printQAndNewLine();

        //Serial.print("R99");
        //Serial.print(" step count: ");
        //Serial.print(stepsCount);
        //Serial.print("\r\n");
      }

      calibAxis->resetMotorStep();
      //delayMicroseconds(100000 / speedHome[axis] / 2);
    }
    else
    {
      //Serial.print("R99 movement done\r\n");
      movementDone = true;
    }
  }

  //Serial.print("R99");
  //Serial.print(" axis ");
  //Serial.print(calibAxis->channelLabel);
  //Serial.print(" at end point");
  //Serial.print("\r\n");

  //encoderEndPoint = calibEncoder->currentPosition();

  // if the encoder is enabled, use the encoder data instead of the step count

  if (encoderEnabled)
  {
    //stepsCount = abs(encoderEndPoint - encoderStartPoint);
  }

  // Report back the end stop setting

  if (error == 0)
  {
    //Serial.print("R23");
    //Serial.print(" ");
    //Serial.print("P");
    //Serial.print(parNbrStp);
    //Serial.print(" ");
    //Serial.print("V");
    //Serial.print((float)stepsCount);
    //CurrentState::getInstance()->printQAndNewLine();
  }

  *axisStatus = COMM_REPORT_MOVE_STATUS_STOP_MOTOR;
  reportStatus(calibAxis, axisStatus[0]);

  calibAxis->disableMotor();

  storeEndStops();
  reportEndStops();

  switch (axis)
  {
  case 0:
    CurrentState::getInstance()->setX(stepsCount);
    break;
  case 1:
    CurrentState::getInstance()->setY(stepsCount);
    break;
  case 2:
    CurrentState::getInstance()->setZ(stepsCount);
    break;
  }

  reportPosition();

  *axisStatus = COMM_REPORT_MOVE_STATUS_IDLE;
  reportStatus(calibAxis, axisStatus[0]);

  reportCalib(calibAxis, COMM_REPORT_CALIBRATE_STATUS_IDLE);

  CurrentState::getInstance()->setLastError(error);
  return error;
}

int debugPrintCount = 0;



void Movement::loadMotorSettings()
{

  // Load settings

  homeIsUp[0] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_UP_X);
  homeIsUp[1] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_UP_Y);
  homeIsUp[2] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_UP_Z);

  speedMax[0] = ParameterList::getInstance()->getValue(MOVEMENT_MAX_SPD_X);
  speedMax[1] = ParameterList::getInstance()->getValue(MOVEMENT_MAX_SPD_Y);
  speedMax[2] = ParameterList::getInstance()->getValue(MOVEMENT_MAX_SPD_Z);

  speedMin[0] = ParameterList::getInstance()->getValue(MOVEMENT_MIN_SPD_X);
  speedMin[1] = ParameterList::getInstance()->getValue(MOVEMENT_MIN_SPD_Y);
  speedMin[2] = ParameterList::getInstance()->getValue(MOVEMENT_MIN_SPD_Z);

  speedHome[0] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_SPEED_X);
  speedHome[1] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_SPEED_Y);
  speedHome[2] = ParameterList::getInstance()->getValue(MOVEMENT_HOME_SPEED_Z);

  stepsAcc[0] = ParameterList::getInstance()->getValue(MOVEMENT_STEPS_ACC_DEC_X);
  stepsAcc[1] = ParameterList::getInstance()->getValue(MOVEMENT_STEPS_ACC_DEC_Y);
  stepsAcc[2] = ParameterList::getInstance()->getValue(MOVEMENT_STEPS_ACC_DEC_Z);

  motorInv[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_INVERT_MOTOR_X));
  motorInv[1] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_INVERT_MOTOR_Y));
  motorInv[2] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_INVERT_MOTOR_Z));

  endStInv[0] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_ENDPOINTS_X);
  endStInv[1] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_ENDPOINTS_Y);
  endStInv[2] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_ENDPOINTS_Z);

  endStInv2[0] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_2_ENDPOINTS_X);
  endStInv2[1] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_2_ENDPOINTS_Y);
  endStInv2[2] = ParameterList::getInstance()->getValue(MOVEMENT_INVERT_2_ENDPOINTS_Z);

  endStEnbl[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_ENABLE_ENDPOINTS_X));
  endStEnbl[1] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_ENABLE_ENDPOINTS_Y));
  endStEnbl[2] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_ENABLE_ENDPOINTS_Z));

  timeOut[0] = ParameterList::getInstance()->getValue(MOVEMENT_TIMEOUT_X);
  timeOut[1] = ParameterList::getInstance()->getValue(MOVEMENT_TIMEOUT_Y);
  timeOut[2] = ParameterList::getInstance()->getValue(MOVEMENT_TIMEOUT_Z);

  motorKeepActive[0] = ParameterList::getInstance()->getValue(MOVEMENT_KEEP_ACTIVE_X);
  motorKeepActive[1] = ParameterList::getInstance()->getValue(MOVEMENT_KEEP_ACTIVE_Y);
  motorKeepActive[2] = ParameterList::getInstance()->getValue(MOVEMENT_KEEP_ACTIVE_Z);

  motorMaxSize[0] = ParameterList::getInstance()->getValue(MOVEMENT_AXIS_NR_STEPS_X);
  motorMaxSize[1] = ParameterList::getInstance()->getValue(MOVEMENT_AXIS_NR_STEPS_Y);
  motorMaxSize[2] = ParameterList::getInstance()->getValue(MOVEMENT_AXIS_NR_STEPS_Z);

  motor2Inv[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_SECONDARY_MOTOR_INVERT_X));
  motor2Inv[1] = false;
  motor2Inv[2] = false;

  motor2Enbl[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_SECONDARY_MOTOR_X));
  motor2Enbl[1] = false;
  motor2Enbl[2] = false;

  motorStopAtHome[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_HOME_X));
  motorStopAtHome[1] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_HOME_Y));
  motorStopAtHome[2] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_HOME_Z));

  motorStopAtMax[0] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_MAX_X));
  motorStopAtMax[1] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_MAX_Y));
  motorStopAtMax[2] = intToBool(ParameterList::getInstance()->getValue(MOVEMENT_STOP_AT_MAX_Z));

  stepsPerMm[0] = ParameterList::getInstance()->getValue(MOVEMENT_STEP_PER_MM_X);
  stepsPerMm[1] = ParameterList::getInstance()->getValue(MOVEMENT_STEP_PER_MM_Y);
  stepsPerMm[2] = ParameterList::getInstance()->getValue(MOVEMENT_STEP_PER_MM_Z);

  if (stepsPerMm[0] < 1) 
  {
    stepsPerMm[0] = 1;
  }

  if (stepsPerMm[1] < 1)
  {
    stepsPerMm[1] = 1;
  }

  if (stepsPerMm[2] < 1)
  {
    stepsPerMm[2] = 1;
  }

  CurrentState::getInstance()->setStepsPerMm(stepsPerMm[0], stepsPerMm[1], stepsPerMm[2]);

  axisX.loadMotorSettings(speedMax[0], speedMin[0], speedHome[0], stepsAcc[0], timeOut[0], homeIsUp[0], motorInv[0], endStInv[0], endStInv2[0], MOVEMENT_INTERRUPT_SPEED, motor2Enbl[0], motor2Inv[0], endStEnbl[0], motorStopAtHome[0], motorMaxSize[0], motorStopAtMax[0]);
  axisY.loadMotorSettings(speedMax[1], speedMin[1], speedHome[1], stepsAcc[1], timeOut[1], homeIsUp[1], motorInv[1], endStInv[1], endStInv2[1], MOVEMENT_INTERRUPT_SPEED, motor2Enbl[1], motor2Inv[1], endStEnbl[1], motorStopAtHome[1], motorMaxSize[1], motorStopAtMax[1]);
  axisZ.loadMotorSettings(speedMax[2], speedMin[2], speedHome[2], stepsAcc[2], timeOut[2], homeIsUp[2], motorInv[2], endStInv[2], endStInv2[2], MOVEMENT_INTERRUPT_SPEED, motor2Enbl[2], motor2Inv[2], endStEnbl[2], motorStopAtHome[2], motorMaxSize[2], motorStopAtMax[2]);

  primeMotors();
}

bool Movement::intToBool(int value)
{
  if (value == 1)
  {
    return true;
  }
  return false;
}

unsigned long Movement::getMaxLength(unsigned long lengths[3])
{
  unsigned long max = lengths[0];
  for (int i = 1; i < 3; i++)
  {
    if (lengths[i] > max)
    {
      max = lengths[i];
    }
  }
  return max;
}

void Movement::enableMotors()
{
  motorMotorsEnabled = true;

  axisX.enableMotor();
  axisY.enableMotor();
  axisZ.enableMotor();

  //delay(100);
}

void Movement::disableMotorsEmergency()
{
  motorMotorsEnabled = false;

  axisX.disableMotor();
  axisY.disableMotor();
  axisZ.disableMotor();
}

void Movement::disableMotors()
{
  motorMotorsEnabled = false;

  if (motorKeepActive[0] == 0) { axisX.disableMotor(); }
  if (motorKeepActive[1] == 0) { axisY.disableMotor(); }
  if (motorKeepActive[2] == 0) { axisZ.disableMotor(); }

  //delay(100);
}

void Movement::primeMotors()
{
  if (motorKeepActive[0] == 1) { axisX.enableMotor(); } else { axisX.disableMotor(); }
  if (motorKeepActive[1] == 1) { axisY.enableMotor(); } else { axisY.disableMotor(); }
  if (motorKeepActive[2] == 1) { axisZ.enableMotor(); } else { axisZ.disableMotor(); }
}

bool Movement::motorsEnabled()
{
  return motorMotorsEnabled;
}

bool Movement::endStopsReached()
{

  if (axisX.endStopsReached() ||
      axisY.endStopsReached() ||
      axisZ.endStopsReached())
  {
    return true;
  }
  return false;
}

void Movement::storePosition()
{

#if !defined(FARMDUINO_EXP_V20)

    CurrentState::getInstance()->setX(axisX.currentPosition());
    CurrentState::getInstance()->setY(axisY.currentPosition());
    CurrentState::getInstance()->setZ(axisZ.currentPosition());
 
#endif

}

void Movement::reportEndStops()
{
  CurrentState::getInstance()->printEndStops();
}

void Movement::reportPosition()
{
  CurrentState::getInstance()->getPosition();
}

void Movement::storeEndStops()
{
  CurrentState::getInstance()->storeEndStops();
}

void Movement::setPositionX(long pos)
{
  axisX.setCurrentPosition(pos);
}

void Movement::setPositionY(long pos)
{
  axisY.setCurrentPosition(pos);
}

void Movement::setPositionZ(long pos)
{
  axisZ.setCurrentPosition(pos);
}

// Handle movement by checking each axis
void Movement::handleMovementInterrupt(void)
{
    // No need to check the encoders for Farmduino 1.4
//#if defined(RAMPS_V14) || defined(FARMDUINO_V10)
//    checkEncoders();
//#endif
//
//    // handle motor timing
//
//    axisX.incrementTick();
//    axisY.incrementTick();
//    axisZ.incrementTick();
    unsigned long loopCounts = 0;
    while ((axisActive[0] || axisActive[1] || axisActive[2]) && !emergencyStop)
    {

        /**/
        if (loopCounts % 1000 == 0)
        {

            //serial->Write("R99");
            //serial->Write(" missed step ");
            //serial->Write(motorConsMissedSteps[1]);
            //serial->Write(" axis pos ");
            //serial->Write(axisY.currentPosition());
            //Serial.print("\r\n");

            //Serial.print("X - ");
            //axisX.test();

            //Serial.print("Y - ");
            //axisY.test();

        }
        loopCounts++;


        checkAxisSubStatus(&axisX, &axisSubStep[0]);
        checkAxisSubStatus(&axisY, &axisSubStep[1]);
        checkAxisSubStatus(&axisZ, &axisSubStep[2]);

        //axisX.checkTiming();
        //axisY.checkTiming();
        //axisZ.checkTiming();

        //if (axisX.isStepDone())
        //{
        axisX.checkMovement(std::chrono::system_clock::now());
        /*    axisX.resetStepDone();
          }*/

          //if (axisY.isStepDone())
          //{
        axisY.checkMovement(std::chrono::system_clock::now());
        //  axisY.resetStepDone();
        //}

        //if (axisZ.isStepDone())
        //{
        axisZ.checkMovement(std::chrono::system_clock::now());
        //  axisZ.resetStepDone();
        //}

        if (axisX.isAxisActive() && motorConsMissedSteps[0] > motorConsMissedStepsMax[0])
        {
            axisX.deactivateAxis();
            serialBuffer += "R99";
            serialBuffer += " deactivate motor X due to missed steps";
            serialBuffer += "\r\n";

            if (xHome)
            {
                axisX.setCurrentPosition(0);
            }
            else
            {
                error = ERR_STALL_DETECTED;
            }
        }

        if (axisY.isAxisActive() && motorConsMissedSteps[1] > motorConsMissedStepsMax[1])
        {
            axisY.deactivateAxis();
            serialBuffer += "R99";
            serialBuffer += " deactivate motor Y due to missed steps";
            serialBuffer += "\r\n";

            if (yHome)
            {
                axisY.setCurrentPosition(0);
            }
            else
            {
                error = ERR_STALL_DETECTED;
            }
        }

        if (axisZ.isAxisActive() && motorConsMissedSteps[2] > motorConsMissedStepsMax[2])
        {
            axisZ.deactivateAxis();

            serialBuffer += "R99";
            serialBuffer += " deactivate motor Z due to missed steps";
            serialBuffer += "\r\n";

            if (zHome)
            {
                //encoderZ.setPosition(0);
                axisZ.setCurrentPosition(0);
            }
            else
            {
                error = ERR_STALL_DETECTED;
            }
        }

        if (axisX.endStopAxisReached(false))
        {
            axisX.setCurrentPosition(0);
        }

        if (axisY.endStopAxisReached(false))
        {
            axisY.setCurrentPosition(0);
        }

        if (axisZ.endStopAxisReached(false))
        {
            axisZ.setCurrentPosition(0);
        }

        axisActive[0] = axisX.isAxisActive();
        axisActive[1] = axisY.isAxisActive();
        axisActive[2] = axisZ.isAxisActive();

        currentPoint[0] = axisX.currentPosition();
        currentPoint[1] = axisY.currentPosition();
        currentPoint[2] = axisZ.currentPosition();
        

        CurrentState::getInstance()->setX(currentPoint[0]);
        CurrentState::getInstance()->setY(currentPoint[1]);
        CurrentState::getInstance()->setZ(currentPoint[2]);
        std::cout << currentPoint[0] << " ," << currentPoint[1] << " ," << currentPoint[2] << std::endl;

        storeEndStops();

        // Check timeouts
        /*if (axisActive[0] == true && ((millis() >= timeStart && millis() - timeStart > timeOut[0] * 1000) || (millis() < timeStart && millis() > timeOut[0] * 1000)))
        {
          serialBuffer += COMM_REPORT_TIMEOUT_X;
          serialBuffer += "\r\n";
          serialBuffer += "R99 timeout X axis\r\n";
          error = ERR_TIMEOUT;
        }*/
        /* if (axisActive[1] == true && ((millis() >= timeStart && millis() - timeStart > timeOut[1] * 1000) || (millis() < timeStart && millis() > timeOut[1] * 1000)))
         {
           serialBuffer += COMM_REPORT_TIMEOUT_Y;
           serialBuffer += "\r\n";
           serialBuffer += "R99 timeout Y axis\r\n";
           error = ERR_TIMEOUT;
         }*/
         /*if (axisActive[2] == true && ((millis() >= timeStart && millis() - timeStart > timeOut[2] * 1000) || (millis() < timeStart && millis() > timeOut[2] * 1000)))
         {
           serialBuffer += COMM_REPORT_TIMEOUT_Z;
           serialBuffer += "\r\n";
           serialBuffer += "R99 timeout Z axis\r\n";
           error = ERR_TIMEOUT;
         }*/

         // Check if there is an emergency stop command
         //if (Serial.available() > 0)
         //{
         //  incomingByte = Serial.read();
         //  if (incomingByte == 69 || incomingByte == 101)
         //  {
         //    serialBuffer += "R99 emergency stop\r\n";

         //    Serial.print(COMM_REPORT_EMERGENCY_STOP);
         //    //CurrentState::getInstance()->printQAndNewLine();

         //    emergencyStop = true;

         //    axisX.deactivateAxis();
         //    axisY.deactivateAxis();
         //    axisZ.deactivateAxis();

         //    axisActive[0] = false;
         //    axisActive[1] = false;
         //    axisActive[2] = false;

         //    error = ERR_EMERGENCY_STOP;
         //  }
         //}

        if (error != 0)
        {
            serialBuffer += "R99 error\r\n";

            axisActive[0] = false;
            axisActive[1] = false;
            axisActive[2] = false;
        }

        // Send the serial buffer one character per cycle to keep motor timing more accuracte
        serialBufferSendNext();

        // Periodically send message still active
        //currentMillis++;
        serialMessageDelay++;

        if (serialMessageDelay > 300 && serialBuffer.length() == 0 && serialBufferSending == 0)
        {

            //Serial.print("Y-");
            //axisY.test();/**/

            serialMessageDelay = 0;

            switch (serialMessageNr)
            {
            case 0:
                /*serialBuffer += COMM_REPORT_CMD_BUSY;
                serialBuffer += CurrentState::getInstance()->getQAndNewLine();*/
                break;
            case 1:
                /*serialBuffer += CurrentState::getInstance()->getPosition();
                serialBuffer += CurrentState::getInstance()->getQAndNewLine();*/
                break;

            default:
                break;
            }

            serialMessageNr++;

#if !defined(FARMDUINO_EXP_V20)
            if (serialMessageNr > 1)
            {
                serialMessageNr = 0;
            }
#endif

            serialBufferSending = 0;

            //  if (debugMessages /*&& debugInterrupt*/)
            //  {

                        //Serial.print("R99");
                        //Serial.print(" missed step ");
                        //Serial.print(motorConsMissedSteps[1]);
                        //Serial.print(" encoder pos ");
                        //Serial.print(encoderY.currentPosition());
                        //Serial.print(" axis pos ");
                        //Serial.print(axisY.currentPosition());
                        //Serial.print("\r\n");
            //  }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

}