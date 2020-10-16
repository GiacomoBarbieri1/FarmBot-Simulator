#include "ParameterList.h"

static ParameterList *instanceParam;
long paramValues[PARAM_NR_OF_PARAMS];

ParameterList *ParameterList::getInstance()
{
  if (!instanceParam)
  {
    instanceParam = new ParameterList();
  };
  return instanceParam;
}

ParameterList::ParameterList()
{
    
    std::ifstream exists("./files/EepromSimulated.csv");

    if (exists.fail())
    {
        //Creates the simulation file
        std::ofstream myfile("./files/EepromSimulated.csv", std::ios::out);
        //myfile << "Number Param,Value" << std::endl;
        for (int i = 0; i <= PARAM_NR_OF_PARAMS; i++)
            myfile << i << ",-1" << std::endl;
        myfile.close();

    }
   
  // at the first boot, load default parameters and set the parameter version
  // so during subsequent boots the values are just loaded from eeprom
  // unless the eeprom is disabled with a parameter

  int paramChangeNr = 0;

  int paramVersion = readValueEeprom(0);
  if (paramVersion <= 0)
  {
    setAllValuesToDefault();
    writeAllValuesToEeprom();
  }
  else
  {
    if (readValueEeprom(PARAM_USE_EEPROM) == 1)
    {
      readAllValuesFromEeprom();
    } else {
      setAllValuesToDefault();
    }
  }
}

// ===== Interface functions for the raspberry pi =====

System::String^ ParameterList::readValue(int id)
{
    System::String^ mensaje;
  // Check if the value is an existing parameter
  if (validParam(id))
  {
    // Retrieve the value from memory
    long value = paramValues[id];

    // Send to the raspberry pi
    mensaje = gcnew System::String("R21 P" + id + " V" + value);

   mensaje+= CurrentState::getInstance()->getQAndNewLine();
  }
  else
  {
      mensaje = gcnew System::String("R99 Error: invalid parameter id\r\n");
  }

  return mensaje;
}

System::String^ ParameterList::writeValue(int id, long value)
{
    System::String^ mensaje="";
  if (paramChangeNr < 9999)
  {
    paramChangeNr++;
  }
  else
  {
    paramChangeNr = 0;
  }

  // Check if the value is a valid parameter
  if (validParam(id))
  {
    // Store the value in memory
    paramValues[id] = value;
    writeValueEeprom(id, value);
    readValue(id);
  }
  else
  {
      mensaje = gcnew System::String("R99 Error: invalid parameter id\r\n");
  }

  /*

	CurrentState::getInstance()->printQAndNewLine();
*/

  // If any value is written,
  // trigger the loading of the new configuration in all other modules
  sendConfigToModules();

  return mensaje;
}

void ParameterList::sendConfigToModules()
{
  // Trigger other modules to load the new values
  //PinGuard::getInstance()->loadConfig();
}

int ParameterList::readAllValues(System::IO::Ports::SerialPort^ p_serialPort)
{

  // Make a dump of all values
  // Check if it's a valid value to keep the junk out of the list
  for (int i = 0; i < PARAM_NR_OF_PARAMS; i++)
  {
    if (validParam(i))
    {
      p_serialPort->Write(readValue(i));
    }
  }
  return 0;
}

long ParameterList::getValue(int id)
{
  return paramValues[id];
}

int ParameterList::paramChangeNumber()
{
  return paramChangeNr;
}

// ===== eeprom handling ====
long ParameterList::readValueEeprom(int p_id)
{
    std::fstream fin;
    fin.open("./files/EepromSimulated.csv", std::ios::in);
    if (!fin.fail()) {
        std::string line;
        while (std::getline(fin, line)) {
            if (line.rfind(std::to_string(p_id), 0) == 0) {
                std::size_t found = line.find(",");
                long value = std::stol(line.substr(found+1));
                return value;
            }
        }
        fin.close();
        
    }
    return -1;
}

int ParameterList::writeValueEeprom(int p_id, long p_value)
{
    // File pointer 
    std::fstream fin, fout;

    // Open an existing record 
    fin.open("./files/EepromSimulated.csv", std::ios::in);
    if (!fin.fail()) {
        fout.open("./files/EepromSimulatedTemp.csv", std::ios::out);
        std::string line;
        while (std::getline(fin, line)) {
            if(line.rfind(std::to_string(p_id),0)==0)
                fout << p_id << "," << p_value << std::endl;
            else
                fout << line << std::endl;
        }
        fout.close();
        fin.close();
        std::remove("./files/EepromSimulated.csv");
        std::rename("./files/EepromSimulatedTemp.csv", "./files/EepromSimulated.csv");
    }

  
    
  return 0;
}

int ParameterList::readAllValuesFromEeprom()
{
  // Write all existing values to eeprom
  for (int i = 0; i < PARAM_NR_OF_PARAMS; i++)
  {
    if (validParam(i))
    {
      paramValues[i] = readValueEeprom(i);
      if (paramValues[i] == -1)
      {
        // When parameters are still on default,
        // load a good value and save it
        loadDefaultValue(i);
        writeValueEeprom(i, paramValues[i]);
      }
    }
  }
  return 0;
}

int ParameterList::writeAllValuesToEeprom()
{
  // Write all existing values to eeprom
  for (int i = 0; i < PARAM_NR_OF_PARAMS; i++)
  {
    if (validParam(i))
    {
      writeValueEeprom(i, paramValues[i]);
    }
  }
  return 0;
}

// ==== parameter valdation and defaults

int ParameterList::setAllValuesToDefault()
{
  // Copy default values to the memory values
  for (int i = 0; i < PARAM_NR_OF_PARAMS; i++)
  {
    if (validParam(i))
    {
      loadDefaultValue(i);
    }
  }
  return 0;
}

void ParameterList::loadDefaultValue(int id)
{

  switch (id)
  {
  case PARAM_VERSION:
    paramValues[id] = PARAM_VERSION_DEFAULT;
    break;
  case PARAM_TEST:
    paramValues[id] = PARAM_TEST_DEFAULT;
    break;
  case PARAM_CONFIG_OK:
    paramValues[id] = PARAM_CONFIG_OK_DEFAULT;
    break;
  case PARAM_USE_EEPROM:
    paramValues[id] = PARAM_USE_EEPROM_DEFAULT;
    break;
  case PARAM_E_STOP_ON_MOV_ERR:
    paramValues[id] = PARAM_E_STOP_ON_MOV_ERR_DEFAULT;
    break;
  case PARAM_MOV_NR_RETRY:
    paramValues[id] = PARAM_MOV_NR_RETRY_DEFAULT;
    break;
  case MOVEMENT_TIMEOUT_X:
    paramValues[id] = MOVEMENT_TIMEOUT_X_DEFAULT;
    break;
  case MOVEMENT_TIMEOUT_Y:
    paramValues[id] = MOVEMENT_TIMEOUT_Y_DEFAULT;
    break;
  case MOVEMENT_TIMEOUT_Z:
    paramValues[id] = MOVEMENT_TIMEOUT_Z_DEFAULT;
    break;

  case MOVEMENT_KEEP_ACTIVE_X:
    paramValues[id] = MOVEMENT_KEEP_ACTIVE_X_DEFAULT;
    break;
  case MOVEMENT_KEEP_ACTIVE_Y:
    paramValues[id] = MOVEMENT_KEEP_ACTIVE_Y_DEFAULT;
    break;
  case MOVEMENT_KEEP_ACTIVE_Z:
    paramValues[id] = MOVEMENT_KEEP_ACTIVE_Z_DEFAULT;
    break;

  case MOVEMENT_HOME_AT_BOOT_X:
    paramValues[id] = MOVEMENT_HOME_AT_BOOT_X_DEFAULT;
    break;
  case MOVEMENT_HOME_AT_BOOT_Y:
    paramValues[id] = MOVEMENT_HOME_AT_BOOT_Y_DEFAULT;
    break;
  case MOVEMENT_HOME_AT_BOOT_Z:
    paramValues[id] = MOVEMENT_HOME_AT_BOOT_Z_DEFAULT;
    break;

  case MOVEMENT_INVERT_ENDPOINTS_X:
    paramValues[id] = MOVEMENT_INVERT_ENDPOINTS_X_DEFAULT;
    break;
  case MOVEMENT_INVERT_ENDPOINTS_Y:
    paramValues[id] = MOVEMENT_INVERT_ENDPOINTS_Y_DEFAULT;
    break;
  case MOVEMENT_INVERT_ENDPOINTS_Z:
    paramValues[id] = MOVEMENT_INVERT_ENDPOINTS_Z_DEFAULT;
    break;

  case MOVEMENT_ENABLE_ENDPOINTS_X:
    paramValues[id] = MOVEMENT_ENABLE_ENDPOINTS_X_DEFAULT;
    break;
  case MOVEMENT_ENABLE_ENDPOINTS_Y:
    paramValues[id] = MOVEMENT_ENABLE_ENDPOINTS_Y_DEFAULT;
    break;
  case MOVEMENT_ENABLE_ENDPOINTS_Z:
    paramValues[id] = MOVEMENT_ENABLE_ENDPOINTS_Z_DEFAULT;
    break;

  case MOVEMENT_INVERT_MOTOR_X:
    paramValues[id] = MOVEMENT_INVERT_MOTOR_X_DEFAULT;
    break;
  case MOVEMENT_INVERT_MOTOR_Y:
    paramValues[id] = MOVEMENT_INVERT_MOTOR_Y_DEFAULT;
    break;
  case MOVEMENT_INVERT_MOTOR_Z:
    paramValues[id] = MOVEMENT_INVERT_MOTOR_Z_DEFAULT;
    break;

  case MOVEMENT_SECONDARY_MOTOR_X:
    paramValues[id] = MOVEMENT_SECONDARY_MOTOR_X_DEFAULT;
    break;
  case MOVEMENT_SECONDARY_MOTOR_INVERT_X:
    paramValues[id] = MOVEMENT_SECONDARY_MOTOR_INVERT_X_DEFAULT;
    break;

  case MOVEMENT_STEPS_ACC_DEC_X:
    paramValues[id] = MOVEMENT_STEPS_ACC_DEC_X_DEFAULT;
    break;
  case MOVEMENT_STEPS_ACC_DEC_Y:
    paramValues[id] = MOVEMENT_STEPS_ACC_DEC_Y_DEFAULT;
    break;
  case MOVEMENT_STEPS_ACC_DEC_Z:
    paramValues[id] = MOVEMENT_STEPS_ACC_DEC_Z_DEFAULT;
    break;

  case MOVEMENT_STOP_AT_HOME_X:
    paramValues[id] = MOVEMENT_STOP_AT_HOME_X_DEFAULT;
    break;
  case MOVEMENT_STOP_AT_HOME_Y:
    paramValues[id] = MOVEMENT_STOP_AT_HOME_Y_DEFAULT;
    break;
  case MOVEMENT_STOP_AT_HOME_Z:
    paramValues[id] = MOVEMENT_STOP_AT_HOME_Z_DEFAULT;
    break;

  case MOVEMENT_HOME_UP_X:
    paramValues[id] = MOVEMENT_HOME_UP_X_DEFAULT;
    break;
  case MOVEMENT_HOME_UP_Y:
    paramValues[id] = MOVEMENT_HOME_UP_Y_DEFAULT;
    break;
  case MOVEMENT_HOME_UP_Z:
    paramValues[id] = MOVEMENT_HOME_UP_Z_DEFAULT;
    break;

  case MOVEMENT_STEP_PER_MM_X:
    paramValues[id] = MOVEMENT_STEP_PER_MM_X_DEFAULT;
    break;
  case MOVEMENT_STEP_PER_MM_Y:
    paramValues[id] = MOVEMENT_STEP_PER_MM_Y_DEFAULT;
    break;
  case MOVEMENT_STEP_PER_MM_Z:
    paramValues[id] = MOVEMENT_STEP_PER_MM_Z_DEFAULT;
    break;

  case MOVEMENT_MIN_SPD_X:
    paramValues[id] = MOVEMENT_MIN_SPD_X_DEFAULT;
    break;
  case MOVEMENT_MIN_SPD_Y:
    paramValues[id] = MOVEMENT_MIN_SPD_Y_DEFAULT;
    break;
  case MOVEMENT_MIN_SPD_Z:
    paramValues[id] = MOVEMENT_MIN_SPD_Z_DEFAULT;
    break;

  case MOVEMENT_HOME_SPEED_X:
    paramValues[id] = MOVEMENT_HOME_SPEED_X_DEFAULT;
    break;
  case MOVEMENT_HOME_SPEED_Y:
    paramValues[id] = MOVEMENT_HOME_SPEED_Y_DEFAULT;
    break;
  case MOVEMENT_HOME_SPEED_Z:
    paramValues[id] = MOVEMENT_HOME_SPEED_Z_DEFAULT;
    break;

  case MOVEMENT_MAX_SPD_X:
    paramValues[id] = MOVEMENT_MAX_SPD_X_DEFAULT;
    break;
  case MOVEMENT_MAX_SPD_Y:
    paramValues[id] = MOVEMENT_MAX_SPD_Y_DEFAULT;
    break;
  case MOVEMENT_MAX_SPD_Z:
    paramValues[id] = MOVEMENT_MAX_SPD_Z_DEFAULT;
    break;

  case MOVEMENT_INVERT_2_ENDPOINTS_X:
    paramValues[id] = MOVEMENT_INVERT_2_ENDPOINTS_X_DEFAULT;
    break;
  case MOVEMENT_INVERT_2_ENDPOINTS_Y:
    paramValues[id] = MOVEMENT_INVERT_2_ENDPOINTS_Y_DEFAULT;
    break;
  case MOVEMENT_INVERT_2_ENDPOINTS_Z:
    paramValues[id] = MOVEMENT_INVERT_2_ENDPOINTS_Z_DEFAULT;
    break;

  case MOVEMENT_STOP_AT_MAX_X:
    paramValues[id] = MOVEMENT_STOP_AT_MAX_X_DEFAULT;
    break;
  case MOVEMENT_STOP_AT_MAX_Y:
    paramValues[id] = MOVEMENT_STOP_AT_MAX_Y_DEFAULT;
    break;
  case MOVEMENT_STOP_AT_MAX_Z:
    paramValues[id] = MOVEMENT_STOP_AT_MAX_Z_DEFAULT;
    break;

  case MOVEMENT_MOTOR_CURRENT_X:
    paramValues[id] = MOVEMENT_MOTOR_CURRENT_X_DEFAULT;
    break;
  case MOVEMENT_MOTOR_CURRENT_Y:
    paramValues[id] = MOVEMENT_MOTOR_CURRENT_Y_DEFAULT;
    break;
  case MOVEMENT_MOTOR_CURRENT_Z:
    paramValues[id] = MOVEMENT_MOTOR_CURRENT_Z_DEFAULT;
    break;

  case MOVEMENT_STALL_SENSITIVITY_X:
    paramValues[id] = MOVEMENT_STALL_SENSITIVITY_X_DEFAULT;
    break;
  case MOVEMENT_STALL_SENSITIVITY_Y:
    paramValues[id] = MOVEMENT_STALL_SENSITIVITY_Y_DEFAULT;
    break;
  case MOVEMENT_STALL_SENSITIVITY_Z:
    paramValues[id] = MOVEMENT_STALL_SENSITIVITY_Z_DEFAULT;
    break;

  case MOVEMENT_MICROSTEPS_X:
    paramValues[id] = MOVEMENT_MICROSTEPS_X_DEFAULT;
    break;
  case MOVEMENT_MICROSTEPS_Y:
    paramValues[id] = MOVEMENT_MICROSTEPS_Y_DEFAULT;
    break;
  case MOVEMENT_MICROSTEPS_Z:
    paramValues[id] = MOVEMENT_MICROSTEPS_Z_DEFAULT;
    break;

  case ENCODER_ENABLED_X:
    paramValues[id] = ENCODER_ENABLED_X_DEFAULT;
    break;
  case ENCODER_ENABLED_Y:
    paramValues[id] = ENCODER_ENABLED_Y_DEFAULT;
    break;
  case ENCODER_ENABLED_Z:
    paramValues[id] = ENCODER_ENABLED_Z_DEFAULT;
    break;

  case ENCODER_TYPE_X:
    paramValues[id] = ENCODER_TYPE_X_DEFAULT;
    break;
  case ENCODER_TYPE_Y:
    paramValues[id] = ENCODER_TYPE_Y_DEFAULT;
    break;
  case ENCODER_TYPE_Z:
    paramValues[id] = ENCODER_TYPE_Z_DEFAULT;
    break;

  case ENCODER_MISSED_STEPS_MAX_X:
    paramValues[id] = ENCODER_MISSED_STEPS_MAX_X_DEFAULT;
    break;
  case ENCODER_MISSED_STEPS_MAX_Y:
    paramValues[id] = ENCODER_MISSED_STEPS_MAX_Y_DEFAULT;
    break;
  case ENCODER_MISSED_STEPS_MAX_Z:
    paramValues[id] = ENCODER_MISSED_STEPS_MAX_Z_DEFAULT;
    break;

  case ENCODER_SCALING_X:
    paramValues[id] = ENCODER_SCALING_X_DEFAULT;
    break;
  case ENCODER_SCALING_Y:
    paramValues[id] = ENCODER_SCALING_Y_DEFAULT;
    break;
  case ENCODER_SCALING_Z:
    paramValues[id] = ENCODER_SCALING_Z_DEFAULT;
    break;

  case ENCODER_MISSED_STEPS_DECAY_X:
    paramValues[id] = ENCODER_MISSED_STEPS_DECAY_X_DEFAULT;
    break;
  case ENCODER_MISSED_STEPS_DECAY_Y:
    paramValues[id] = ENCODER_MISSED_STEPS_DECAY_Y_DEFAULT;
    break;
  case ENCODER_MISSED_STEPS_DECAY_Z:
    paramValues[id] = ENCODER_MISSED_STEPS_DECAY_Z_DEFAULT;
    break;

  case ENCODER_USE_FOR_POS_X:
    paramValues[id] = ENCODER_USE_FOR_POS_X_DEFAULT;
    break;
  case ENCODER_USE_FOR_POS_Y:
    paramValues[id] = ENCODER_USE_FOR_POS_Y_DEFAULT;
    break;
  case ENCODER_USE_FOR_POS_Z:
    paramValues[id] = ENCODER_USE_FOR_POS_Z_DEFAULT;
    break;

  case ENCODER_INVERT_X:
    paramValues[id] = ENCODER_INVERT_X_DEFAULT;
    break;
  case ENCODER_INVERT_Y:
    paramValues[id] = ENCODER_INVERT_Y_DEFAULT;
    break;
  case ENCODER_INVERT_Z:
    paramValues[id] = ENCODER_INVERT_Z_DEFAULT;
    break;

  case PIN_GUARD_1_PIN_NR:
    paramValues[id] = PIN_GUARD_1_PIN_NR_DEFAULT;
    break;
  case PIN_GUARD_1_TIME_OUT:
    paramValues[id] = PIN_GUARD_1_TIME_OUT_DEFAULT;
    break;
  case PIN_GUARD_1_ACTIVE_STATE:
    paramValues[id] = PIN_GUARD_1_ACTIVE_STATE_DEFAULT;
    break;

  case PIN_GUARD_2_PIN_NR:
    paramValues[id] = PIN_GUARD_2_PIN_NR_DEFAULT;
    break;
  case PIN_GUARD_2_TIME_OUT:
    paramValues[id] = PIN_GUARD_2_TIME_OUT_DEFAULT;
    break;
  case PIN_GUARD_2_ACTIVE_STATE:
    paramValues[id] = PIN_GUARD_2_ACTIVE_STATE_DEFAULT;
    break;

  case PIN_GUARD_3_PIN_NR:
    paramValues[id] = PIN_GUARD_3_PIN_NR_DEFAULT;
    break;
  case PIN_GUARD_3_TIME_OUT:
    paramValues[id] = PIN_GUARD_3_TIME_OUT_DEFAULT;
    break;
  case PIN_GUARD_3_ACTIVE_STATE:
    paramValues[id] = PIN_GUARD_3_ACTIVE_STATE_DEFAULT;
    break;

  case PIN_GUARD_4_PIN_NR:
    paramValues[id] = PIN_GUARD_4_PIN_NR_DEFAULT;
    break;
  case PIN_GUARD_4_TIME_OUT:
    paramValues[id] = PIN_GUARD_4_TIME_OUT_DEFAULT;
    break;
  case PIN_GUARD_4_ACTIVE_STATE:
    paramValues[id] = PIN_GUARD_4_ACTIVE_STATE_DEFAULT;
    break;

  case PIN_GUARD_5_PIN_NR:
    paramValues[id] = PIN_GUARD_5_PIN_NR_DEFAULT;
    break;
  case PIN_GUARD_5_TIME_OUT:
    paramValues[id] = PIN_GUARD_5_TIME_OUT_DEFAULT;
    break;
  case PIN_GUARD_5_ACTIVE_STATE:
    paramValues[id] = PIN_GUARD_5_ACTIVE_STATE_DEFAULT;
    break;

  default:
    paramValues[id] = 0;
    break;
  }
}

bool ParameterList::validParam(int id)
{

  // Check if the id is a valid one
  switch (id)
  {
  case PARAM_VERSION:
  case PARAM_CONFIG_OK:
  case PARAM_USE_EEPROM:
  case PARAM_E_STOP_ON_MOV_ERR:
  case PARAM_MOV_NR_RETRY:
  case MOVEMENT_TIMEOUT_X:
  case MOVEMENT_TIMEOUT_Y:
  case MOVEMENT_TIMEOUT_Z:
  case MOVEMENT_KEEP_ACTIVE_X:
  case MOVEMENT_KEEP_ACTIVE_Y:
  case MOVEMENT_KEEP_ACTIVE_Z:
  case MOVEMENT_HOME_AT_BOOT_X:
  case MOVEMENT_HOME_AT_BOOT_Y:
  case MOVEMENT_HOME_AT_BOOT_Z:
  case MOVEMENT_ENABLE_ENDPOINTS_X:
  case MOVEMENT_ENABLE_ENDPOINTS_Y:
  case MOVEMENT_ENABLE_ENDPOINTS_Z:
  case MOVEMENT_INVERT_ENDPOINTS_X:
  case MOVEMENT_INVERT_ENDPOINTS_Y:
  case MOVEMENT_INVERT_ENDPOINTS_Z:
  case MOVEMENT_INVERT_2_ENDPOINTS_X:
  case MOVEMENT_INVERT_2_ENDPOINTS_Y:
  case MOVEMENT_INVERT_2_ENDPOINTS_Z:
  case MOVEMENT_INVERT_MOTOR_X:
  case MOVEMENT_INVERT_MOTOR_Y:
  case MOVEMENT_INVERT_MOTOR_Z:
  case MOVEMENT_SECONDARY_MOTOR_X:
  case MOVEMENT_SECONDARY_MOTOR_INVERT_X:
  case MOVEMENT_STEPS_ACC_DEC_X:
  case MOVEMENT_STEPS_ACC_DEC_Y:
  case MOVEMENT_STEPS_ACC_DEC_Z:
  case MOVEMENT_STOP_AT_HOME_X:
  case MOVEMENT_STOP_AT_HOME_Y:
  case MOVEMENT_STOP_AT_HOME_Z:
  case MOVEMENT_HOME_UP_X:
  case MOVEMENT_HOME_UP_Y:
  case MOVEMENT_HOME_UP_Z:
  case MOVEMENT_STEP_PER_MM_X:
  case MOVEMENT_STEP_PER_MM_Y:
  case MOVEMENT_STEP_PER_MM_Z:
  case MOVEMENT_MIN_SPD_X:
  case MOVEMENT_MIN_SPD_Y:
  case MOVEMENT_MIN_SPD_Z:
  case MOVEMENT_HOME_SPEED_X:
  case MOVEMENT_HOME_SPEED_Y:
  case MOVEMENT_HOME_SPEED_Z:
  case MOVEMENT_MAX_SPD_X:
  case MOVEMENT_MAX_SPD_Y:
  case MOVEMENT_MAX_SPD_Z:
  case MOVEMENT_MOTOR_CURRENT_X:
  case MOVEMENT_MOTOR_CURRENT_Y:
  case MOVEMENT_MOTOR_CURRENT_Z:
  case MOVEMENT_STALL_SENSITIVITY_X:
  case MOVEMENT_STALL_SENSITIVITY_Y:
  case MOVEMENT_STALL_SENSITIVITY_Z:
  case MOVEMENT_MICROSTEPS_X:
  case MOVEMENT_MICROSTEPS_Y:
  case MOVEMENT_MICROSTEPS_Z:
  case ENCODER_ENABLED_X:
  case ENCODER_ENABLED_Y:
  case ENCODER_ENABLED_Z:
  case ENCODER_TYPE_X:
  case ENCODER_TYPE_Y:
  case ENCODER_TYPE_Z:
  case ENCODER_MISSED_STEPS_MAX_X:
  case ENCODER_MISSED_STEPS_MAX_Y:
  case ENCODER_MISSED_STEPS_MAX_Z:
  case ENCODER_SCALING_X:
  case ENCODER_SCALING_Y:
  case ENCODER_SCALING_Z:
  case ENCODER_MISSED_STEPS_DECAY_X:
  case ENCODER_MISSED_STEPS_DECAY_Y:
  case ENCODER_MISSED_STEPS_DECAY_Z:
  case ENCODER_USE_FOR_POS_X:
  case ENCODER_USE_FOR_POS_Y:
  case ENCODER_USE_FOR_POS_Z:
  case ENCODER_INVERT_X:
  case ENCODER_INVERT_Y:
  case ENCODER_INVERT_Z:
  case MOVEMENT_AXIS_NR_STEPS_X:
  case MOVEMENT_AXIS_NR_STEPS_Y:
  case MOVEMENT_AXIS_NR_STEPS_Z:
  case MOVEMENT_STOP_AT_MAX_X:
  case MOVEMENT_STOP_AT_MAX_Y:
  case MOVEMENT_STOP_AT_MAX_Z:
  case PIN_GUARD_1_PIN_NR:
  case PIN_GUARD_1_TIME_OUT:
  case PIN_GUARD_1_ACTIVE_STATE:
  case PIN_GUARD_2_PIN_NR:
  case PIN_GUARD_2_TIME_OUT:
  case PIN_GUARD_2_ACTIVE_STATE:
  case PIN_GUARD_3_PIN_NR:
  case PIN_GUARD_3_TIME_OUT:
  case PIN_GUARD_3_ACTIVE_STATE:
  case PIN_GUARD_4_PIN_NR:
  case PIN_GUARD_4_TIME_OUT:
  case PIN_GUARD_4_ACTIVE_STATE:
  case PIN_GUARD_5_PIN_NR:
  case PIN_GUARD_5_TIME_OUT:
  case PIN_GUARD_5_ACTIVE_STATE:
    return true;
  default:
    return false;
  }
}
