#include "Farmbot.h"
std::thread exThread;
bool stepperInit = false;
bool stepperFlip = false;

static char commandEndChar = 0x0A;
char commandChar[INCOMING_CMD_BUF_SIZE + 1];
//String commandString = "";
char incomingChar = 0;
char incomingCommandArray[INCOMING_CMD_BUF_SIZE];
int incomingCommandPointer = 0;

static GCodeProcessor* gCodeProcessor = new GCodeProcessor();

int reportingPeriod = 5000;
std::chrono::time_point<std::chrono::system_clock> start;

unsigned long lastAction;
unsigned long currentTime;
unsigned long cycleCounter = 0;
bool previousEmergencyStop = false;

unsigned long pinGuardLastCheck;
unsigned long pinGuardCurrentTime;

int lastParamChangeNr = 0;


// Blink led routine used for testing
bool blink = false;


Farmbot::Farmbot(System::IO::Ports::SerialPort^ p_serialPort) {
	//Begin the clock
    start = std::chrono::system_clock::now();
    //Instance the pins
    PinsList =ArduinoPins::getInstance();
    std::cout << "Pines Creados" << std::endl;
    //Start Serial
	cout << "Iniciando Farmbot"<<endl;
	m_serialPort = p_serialPort;
	m_serialPort->Write(conv2Str(COMM_REPORT_COMMENT));
	m_serialPort->Write(conv2Str(SPACE));
	m_serialPort->Write("Serial Conection started");
	m_serialPort->Write(conv2Str(CRLF));

    setPinInputOutput();
    
    readParameters();
    loadMovementSetting();
    startMotor();
    startPinGuard();
    startServo();
    startInterrupt();
    initLastAction();
    homeOnBoot();

    m_serialPort->Write(conv2Str(COMM_REPORT_COMMENT));
    m_serialPort->Write(conv2Str(SPACE));
    m_serialPort->Write("ARDUINO STARTUP COMPLETE");
    m_serialPort->Write(conv2Str(CRLF));

}
// Set pins input output
#ifdef RAMPS_V14
void setPinInputOutput()
{

    // pin input/output settings
    pinMode(X_STEP_PIN, OUTPUT);
    pinMode(X_DIR_PIN, OUTPUT);
    pinMode(X_ENABLE_PIN, OUTPUT);
    pinMode(E_STEP_PIN, OUTPUT);
    pinMode(E_DIR_PIN, OUTPUT);
    pinMode(E_ENABLE_PIN, OUTPUT);
    pinMode(X_MIN_PIN, INPUT_PULLUP);
    pinMode(X_MAX_PIN, INPUT_PULLUP);

    pinMode(X_ENCDR_A, INPUT_PULLUP);
    pinMode(X_ENCDR_B, INPUT_PULLUP);
    pinMode(X_ENCDR_A_Q, INPUT_PULLUP);
    pinMode(X_ENCDR_B_Q, INPUT_PULLUP);

    pinMode(Y_STEP_PIN, OUTPUT);
    pinMode(Y_DIR_PIN, OUTPUT);
    pinMode(Y_ENABLE_PIN, OUTPUT);
    pinMode(Y_MIN_PIN, INPUT_PULLUP);
    pinMode(Y_MAX_PIN, INPUT_PULLUP);

    pinMode(Y_ENCDR_A, INPUT_PULLUP);
    pinMode(Y_ENCDR_B, INPUT_PULLUP);
    pinMode(Y_ENCDR_A_Q, INPUT_PULLUP);
    pinMode(Y_ENCDR_B_Q, INPUT_PULLUP);

    pinMode(Z_STEP_PIN, OUTPUT);
    pinMode(Z_DIR_PIN, OUTPUT);
    pinMode(Z_ENABLE_PIN, OUTPUT);
    pinMode(Z_MIN_PIN, INPUT_PULLUP);
    pinMode(Z_MAX_PIN, INPUT_PULLUP);

    pinMode(Z_ENCDR_A, INPUT_PULLUP);
    pinMode(Z_ENCDR_B, INPUT_PULLUP);
    pinMode(Z_ENCDR_A_Q, INPUT_PULLUP);
    pinMode(Z_ENCDR_B_Q, INPUT_PULLUP);

    pinMode(HEATER_0_PIN, OUTPUT);
    pinMode(HEATER_1_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    pinMode(UTM_C, INPUT_PULLUP);
    pinMode(UTM_D, INPUT_PULLUP);
    pinMode(UTM_E, INPUT_PULLUP);
    pinMode(UTM_F, INPUT_PULLUP);
    pinMode(UTM_G, INPUT_PULLUP);
    pinMode(UTM_H, INPUT_PULLUP);
    pinMode(UTM_I, INPUT_PULLUP);
    pinMode(UTM_J, INPUT_PULLUP);
    pinMode(UTM_K, INPUT_PULLUP);
    pinMode(UTM_L, INPUT_PULLUP);

    // Aux 1 pins to safer state
    pinMode(AUX1_00, INPUT_PULLUP);
    pinMode(AUX1_01, INPUT_PULLUP);
    pinMode(AUX1_57, INPUT_PULLUP);
    pinMode(AUX1_58, INPUT_PULLUP);

    // Aux 3 pins to safer state
    pinMode(AUX3_49, INPUT_PULLUP);
    pinMode(AUX3_50, INPUT_PULLUP);
    pinMode(AUX3_51, INPUT_PULLUP);

    // Aux 4 pins to safer state
    pinMode(AUX4_43, INPUT_PULLUP);
    pinMode(AUX4_45, INPUT_PULLUP);
    pinMode(AUX4_47, INPUT_PULLUP);
    pinMode(AUX4_32, INPUT_PULLUP);

    pinMode(SERVO_0_PIN, OUTPUT);
    pinMode(SERVO_1_PIN, OUTPUT);
    pinMode(SERVO_2_PIN, OUTPUT);
    pinMode(SERVO_3_PIN, OUTPUT);

    Serial.print(COMM_REPORT_COMMENT);
    Serial.print(SPACE);
    Serial.print("Set input/output");
    Serial.print(CRLF);
}
#endif

#if defined(FARMDUINO_V10) || defined(FARMDUINO_V14)
void Farmbot::setPinInputOutput()
{
    PinsList->setMode(X_STEP_PIN, OUTPUT);
    
   
    // pin input/output settings
    PinsList->setMode(X_STEP_PIN, OUTPUT);
    PinsList->setMode(X_DIR_PIN, OUTPUT);
    PinsList->setMode(X_ENABLE_PIN, OUTPUT);

    PinsList->setMode(E_STEP_PIN, OUTPUT);
    PinsList->setMode(E_DIR_PIN, OUTPUT);
    PinsList->setMode(E_ENABLE_PIN, OUTPUT);
    PinsList->setMode(X_MIN_PIN, INPUT_PULLUP);
    PinsList->setMode(X_MAX_PIN, INPUT_PULLUP);

    PinsList->setMode(X_ENCDR_A, INPUT_PULLUP);
    PinsList->setMode(X_ENCDR_B, INPUT_PULLUP);
    //pinMode(X_ENCDR_A_Q, INPUT_PULLUP);
    //pinMode(X_ENCDR_B_Q, INPUT_PULLUP);

    PinsList->setMode(Y_STEP_PIN, OUTPUT);
    PinsList->setMode(Y_DIR_PIN, OUTPUT);
    PinsList->setMode(Y_ENABLE_PIN, OUTPUT);
    PinsList->setMode(Y_MIN_PIN, INPUT_PULLUP);
    PinsList->setMode(Y_MAX_PIN, INPUT_PULLUP);

    PinsList->setMode(Y_ENCDR_A, INPUT_PULLUP);
    PinsList->setMode(Y_ENCDR_B, INPUT_PULLUP);
    //pinMode(Y_ENCDR_A_Q, INPUT_PULLUP);
    //pinMode(Y_ENCDR_B_Q, INPUT_PULLUP);

    PinsList->setMode(Z_STEP_PIN, OUTPUT);
    PinsList->setMode(Z_DIR_PIN, OUTPUT);
    PinsList->setMode(Z_ENABLE_PIN, OUTPUT);
    PinsList->setMode(Z_MIN_PIN, INPUT_PULLUP);
    PinsList->setMode(Z_MAX_PIN, INPUT_PULLUP);

    PinsList->setMode(Z_ENCDR_A, INPUT_PULLUP);
    PinsList->setMode(Z_ENCDR_B, INPUT_PULLUP);
    //pinMode(Z_ENCDR_A_Q, INPUT_PULLUP);
    //pinMode(Z_ENCDR_B_Q, INPUT_PULLUP);

    PinsList->setMode(AUX_STEP_PIN, OUTPUT);
    PinsList->setMode(AUX_DIR_PIN, OUTPUT);
    PinsList->setMode(AUX_ENABLE_PIN, OUTPUT);
    
    PinsList->digitalWrite(AUX_ENABLE_PIN, HIGH);

    PinsList->setMode(LED_PIN, OUTPUT);
    PinsList->setMode(VACUUM_PIN, OUTPUT);
    PinsList->setMode(WATER_PIN, OUTPUT);
    PinsList->setMode(LIGHTING_PIN, OUTPUT);
    PinsList->setMode(PERIPHERAL_4_PIN, OUTPUT);
    PinsList->setMode(PERIPHERAL_5_PIN, OUTPUT);

    PinsList->setMode(UTM_C, INPUT_PULLUP);
    PinsList->setMode(UTM_D, INPUT_PULLUP);
    if (UTM_E > 0) { PinsList->setMode(UTM_E, INPUT_PULLUP); };
    if (UTM_F > 0) { PinsList->setMode(UTM_F, INPUT_PULLUP); };
    if (UTM_G > 0) { PinsList->setMode(UTM_G, INPUT_PULLUP); };
    if (UTM_H > 0) { PinsList->setMode(UTM_H, INPUT_PULLUP); };
    if (UTM_I > 0) { PinsList->setMode(UTM_I, INPUT_PULLUP); };
    if (UTM_J > 0) { PinsList->setMode(UTM_J, INPUT_PULLUP); };
    if (UTM_K > 0) { PinsList->setMode(UTM_K, INPUT_PULLUP); };
    if (UTM_L > 0) { PinsList->setMode(UTM_L, INPUT_PULLUP); };

    PinsList->setMode(SERVO_0_PIN, OUTPUT);
    PinsList->setMode(SERVO_1_PIN, OUTPUT);
    PinsList->setMode(SERVO_2_PIN, OUTPUT);
    PinsList->setMode(SERVO_3_PIN, OUTPUT);

    m_serialPort->Write(Farmbot::conv2Str(COMM_REPORT_COMMENT));
    m_serialPort->Write(conv2Str(SPACE));
    m_serialPort->Write("Set input/output");
    m_serialPort->Write(conv2Str(CRLF));
/*
#if defined(FARMDUINO_V14)

    reportingPeriod = 500;

    pinMode(READ_ENA_PIN, INPUT_PULLUP);
    pinMode(NSS_PIN, OUTPUT);
    digitalWrite(NSS_PIN, HIGH);

    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    SPI.begin();
#endif*/
}
#endif

#if defined(FARMDUINO_EXP_V20)
void setPinInputOutput()
{
    Serial.print(COMM_REPORT_COMMENT);
    Serial.print(SPACE);
    Serial.print("Set input/output farmbot express");
    Serial.print(CRLF);

    // Motor step, direction and pin is set up using the control chip library
    // This board also does not use encoders

    pinMode(X_ENABLE_PIN, OUTPUT);
    pinMode(X_DIR_PIN, OUTPUT);
    pinMode(X_STEP_PIN, OUTPUT);
    pinMode(X_MIN_PIN, INPUT_PULLUP);
    pinMode(X_MAX_PIN, INPUT_PULLUP);

    digitalWrite(X_ENABLE_PIN, HIGH);
    digitalWrite(X_DIR_PIN, LOW);
    digitalWrite(X_STEP_PIN, LOW);

    pinMode(Y_ENABLE_PIN, OUTPUT);
    pinMode(Y_DIR_PIN, OUTPUT);
    pinMode(Y_STEP_PIN, OUTPUT);
    pinMode(Y_MIN_PIN, INPUT_PULLUP);
    pinMode(Y_MAX_PIN, INPUT_PULLUP);

    digitalWrite(Y_ENABLE_PIN, HIGH);
    digitalWrite(Y_DIR_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);

    pinMode(Z_ENABLE_PIN, OUTPUT);
    pinMode(Z_DIR_PIN, OUTPUT);
    pinMode(Z_STEP_PIN, OUTPUT);
    pinMode(Z_MIN_PIN, INPUT_PULLUP);
    pinMode(Z_MAX_PIN, INPUT_PULLUP);

    digitalWrite(Z_ENABLE_PIN, HIGH);
    digitalWrite(Z_DIR_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);

    pinMode(E_ENABLE_PIN, OUTPUT);
    pinMode(E_DIR_PIN, OUTPUT);
    pinMode(E_STEP_PIN, OUTPUT);

    digitalWrite(E_ENABLE_PIN, HIGH);
    digitalWrite(E_DIR_PIN, LOW);
    digitalWrite(E_STEP_PIN, LOW);

    pinMode(AUX_STEP_PIN, OUTPUT);
    pinMode(AUX_DIR_PIN, OUTPUT);
    pinMode(AUX_ENABLE_PIN, OUTPUT);

    pinMode(LED_PIN, OUTPUT);
    pinMode(VACUUM_PIN, OUTPUT);
    pinMode(WATER_PIN, OUTPUT);
    pinMode(LIGHTING_PIN, OUTPUT);
    pinMode(PERIPHERAL_4_PIN, OUTPUT);
    pinMode(PERIPHERAL_5_PIN, OUTPUT);

    pinMode(UTM_C, INPUT_PULLUP);
    pinMode(UTM_D, INPUT_PULLUP);
    if (UTM_E > 0) { pinMode(UTM_E, INPUT_PULLUP); };
    if (UTM_F > 0) { pinMode(UTM_F, INPUT_PULLUP); };
    if (UTM_G > 0) { pinMode(UTM_G, INPUT_PULLUP); };
    if (UTM_H > 0) { pinMode(UTM_H, INPUT_PULLUP); };
    if (UTM_I > 0) { pinMode(UTM_I, INPUT_PULLUP); };
    if (UTM_J > 0) { pinMode(UTM_J, INPUT_PULLUP); };
    if (UTM_K > 0) { pinMode(UTM_K, INPUT_PULLUP); };
    if (UTM_L > 0) { pinMode(UTM_L, INPUT_PULLUP); };

    pinMode(SERVO_0_PIN, OUTPUT);
    pinMode(SERVO_1_PIN, OUTPUT);
    pinMode(SERVO_2_PIN, OUTPUT);
    pinMode(SERVO_3_PIN, OUTPUT);

}
#endif

void Farmbot::readParameters()
{

    // Dump all values to the serial interface
    m_serialPort->Write(Farmbot::conv2Str(COMM_REPORT_COMMENT));
    m_serialPort->Write(conv2Str(SPACE));
    m_serialPort->Write("Read Parameteres");
    m_serialPort->Write(conv2Str(CRLF));

    ParameterList::getInstance()->readAllValues(m_serialPort);
}

void Farmbot::loadMovementSetting()
{
    // Load motor settings
    m_serialPort->Write(Farmbot::conv2Str(COMM_REPORT_COMMENT));
    m_serialPort->Write(conv2Str(SPACE));
    m_serialPort->Write("Load movement settings");
    m_serialPort->Write(conv2Str(CRLF));

    Movement::getInstance()->loadSettings();
}

void Farmbot::startMotor()// Aun Falta
{
    m_serialPort->Write(Farmbot::conv2Str(COMM_REPORT_COMMENT));
    m_serialPort->Write(conv2Str(SPACE));
    m_serialPort->Write("Set motor enables off");
    m_serialPort->Write(conv2Str(CRLF));

    ArduinoPins::getInstance()->digitalWrite(X_ENABLE_PIN, HIGH);
    ArduinoPins::getInstance()->digitalWrite(E_ENABLE_PIN, HIGH);
    ArduinoPins::getInstance()->digitalWrite(Y_ENABLE_PIN, HIGH);
    ArduinoPins::getInstance()->digitalWrite(Z_ENABLE_PIN, HIGH);
}

void Farmbot::startPinGuard() //Aun falta
{
    
}

void Farmbot::startServo()//Aun falta
{
    
}

void Farmbot::startInterrupt()//Aun falta
{
    
}

void Farmbot::initLastAction()//Aun falta
{
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    lastAction = elapsed_seconds.count() * 1000;
}

void Farmbot::homeOnBoot()//Aun falta
{
    
}

void Farmbot::loop()
{
    //checkPinGuard();
    checkSerialInputs();
    checkEmergencyStop();
    checkParamsChanged();
    periodicChecksAndReport();

}



void Farmbot::checkPinGuard()//Aun falta
{
    cout << "Entre al thread" << endl;
    gCodeProcessor->execute(command, m_serialPort);
}

void Farmbot::checkSerialInputs()//Aun falta
{
    if (m_serialPort->BytesToRead>0)
    {
        // Save current time stamp for timeout actions
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds= end - start;
        
        lastAction = elapsed_seconds.count()*1000;

        // Get the input and start processing on receiving 'new line'
        incomingChar = m_serialPort->ReadChar();

        // Filter out emergency stop.
        if (!(incomingChar == 69 || incomingChar == 101))// 69 == E 101==e
        {
            incomingCommandArray[incomingCommandPointer] = incomingChar;
            incomingCommandPointer++;
        }
        else
        {
            CurrentState::getInstance()->setEmergencyStop();
        }

        // If the string is getting to long, cap it off with a new line and let it process anyway
        if (incomingCommandPointer >= INCOMING_CMD_BUF_SIZE - 1)
        {
            incomingChar = '\n';
            incomingCommandArray[incomingCommandPointer] = incomingChar;
            incomingCommandPointer++;
        }

        if (incomingChar == '\n' || incomingCommandPointer >= INCOMING_CMD_BUF_SIZE)
        {

            //char commandChar[incomingCommandPointer + 1];
            for (int i = 0; i < incomingCommandPointer - 1; i++)
            {
                commandChar[i] = incomingCommandArray[i];
            }
            commandChar[incomingCommandPointer - 1] = '\0';

            if (incomingCommandPointer > 1)
            {

                // Report back the received command
                m_serialPort->Write(conv2Str(COMM_REPORT_CMD_ECHO));
                m_serialPort->Write(" ");
                m_serialPort->Write("*");
                m_serialPort->Write(conv2Str(commandChar));
                m_serialPort->Write("*");
                m_serialPort->Write("\r\n");

                // Create a command and let it execute
                command = new Command(commandChar);

                // Log the values if needed for debugging
                if (LOGGING || debugMessages)
                {
                    command->print();
                }
                //int(*CommandExecuter)(Command*,System::IO::Ports::SerialPort^)= gCodeProcessor->execute;
                //std::thread t_execute(gCodeProcessor->execute,command,m_serialPort);
                //gCodeProcessor->execute(command,m_serialPort);
                //CommandExecute^ f = gcnew MyCallback(this,&Farmbot::CommandExecute);
                System::Threading::Thread^ newThread1 = gcnew System::Threading::Thread(
                    gcnew System::Threading::ThreadStart(this,&Farmbot::checkPinGuard));
                newThread1->Start();
                //free(command);
 

            }

            incomingCommandPointer = 0;
        }
    }
}

void Farmbot::checkEmergencyStop()//Aun falta
{
   
}

void Farmbot::checkParamsChanged()//Aun falta
{
    
}

void Farmbot::periodicChecksAndReport()//Aun falta
{
    
}
System::String^ Farmbot::conv2Str(const char* mensaje)
{
	System::String ^conversion= gcnew String(mensaje);
	return  conversion;
}
System::String^ Farmbot::conv2Str(char* mensaje)
{
    System::String^ conversion = gcnew String(mensaje);
    return  conversion;
}
