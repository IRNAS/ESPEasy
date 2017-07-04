//#######################################################################################################
//######################### Plugin 133: INA219 DC Voltage/Current sensor ################################
//#######################################################################################################

#define PLUGIN_133
#define PLUGIN_ID_133         133
#define PLUGIN_NAME_133       "USB charger bq2429x"
#define PLUGIN_VALUENAME1_133 "Status"
#define PLUGIN_VALUENAME2_133 "Faults"

#include <bq2429x.h>

bq2429x PLUGIN_133_charger;


boolean Plugin_133(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_133;
        Device[deviceCount].Type = DEVICE_TYPE_I2C;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 2;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_133);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_133));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_133));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        addFormCheckBox(string, F("Safety timer:"), F("plugin_133_safetytimer"),
        Settings.TaskDevicePluginConfig[event->TaskIndex][0]);
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = isFormItemChecked(F("plugin_133_safetytimer"));
        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        PLUGIN_133_begin();
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        UserVar[event->BaseVarIndex] = PLUGIN_133_charger.getStatus();
        UserVar[event->BaseVarIndex + 1] = PLUGIN_133_charger.getFaults();

        PLUGIN_133_charger.setSafetyTimer(Settings.TaskDevicePluginConfig[event->TaskIndex][0]);

        if(PLUGIN_133_charger.getError() == 0){
          success = true;
          String log = F("Status: ");
          log += String(UserVar[event->BaseVarIndex],HEX);
          log += F(": Faults: ");
          log += String(UserVar[event->BaseVarIndex+1],HEX);
          addLog(LOG_LEVEL_INFO,log);
        }
        else
        {
          addLog(LOG_LEVEL_ERROR, F("bq2329x: i2c error"));
        }
        break;
      }
  }
  return success;
}


//**************************************************************************/
//  Setups the HW
//**************************************************************************/

void PLUGIN_133_begin(void) {
 //
}
