//#######################################################################################################
//######################### Plugin 132: INA219 DC Voltage/Current sensor ################################
//#######################################################################################################

#define PLUGIN_132
#define PLUGIN_ID_132         132
#define PLUGIN_NAME_132       "Fuel Gauge max1720x"
#define PLUGIN_VALUENAME1_132 "Voltage"
#define PLUGIN_VALUENAME2_132 "Current"
#define PLUGIN_VALUENAME3_132 "Temperature"
#define PLUGIN_VALUENAME4_132 "Capacity"
#define PLUGIN_VALUENAME5_132 "SOC"

#include <max1720x.h>

max1720x PLUGIN_132_gauge;


boolean Plugin_132(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_132;
        Device[deviceCount].Type = DEVICE_TYPE_I2C;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 4;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_132);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_132));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_132));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_132));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[3], PSTR(PLUGIN_VALUENAME4_132));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        String plugin1 = WebServer.arg("PLUGIN_132_value");
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = plugin1.toInt();
        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        PLUGIN_132_begin();
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        UserVar[event->BaseVarIndex] = PLUGIN_132_gauge.getVoltage();
        UserVar[event->BaseVarIndex + 1] = PLUGIN_132_gauge.getCurrent();
        UserVar[event->BaseVarIndex + 2] = PLUGIN_132_gauge.getTemperature();
        UserVar[event->BaseVarIndex + 3] = PLUGIN_132_gauge.getSOC();
        String log = F("max1720x read ");
        //log += value;
        addLog(LOG_LEVEL_INFO,log);
        success = true;
        break;
      }
  }
  return success;
}


//**************************************************************************/
//  Setups the HW
//**************************************************************************/

void PLUGIN_132_begin(void) {
  PLUGIN_132_gauge.reset();  // Resets MAX1720x
}
