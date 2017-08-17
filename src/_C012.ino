#include <SPI.h>
#include <LoRa.h>

#define CPLUGIN_012
#define CPLUGIN_ID_012         12
#define CPLUGIN_NAME_012       "LoRa device"

boolean lora_begin = false;

boolean CPlugin_012(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
      {
        Protocol[++protocolCount].Number = CPLUGIN_ID_012;
        break;
      }

    case CPLUGIN_GET_DEVICENAME:
      {
        string = F(CPLUGIN_NAME_012);
        break;
      }

    case CPLUGIN_WEBFORM_LOAD:
      {

        break;
      }
    case CPLUGIN_PROTOCOL_SEND:
      {

        String data = "";

        setup_lora();

        switch(event->sensorType) {
          case SENSOR_TYPE_SINGLE:
            data += F("value=");
            data += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            break;
          case SENSOR_TYPE_LONG:
            data += F("value=");
            data += (unsigned long)UserVar[event->BaseVarIndex] + ((unsigned long)UserVar[event->BaseVarIndex + 1] << 16);
            break;
          case SENSOR_TYPE_DUAL:
            data += F("value=");
            data += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            data += (";");
            data += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            break;
          case SENSOR_TYPE_TEMP_HUM:
            data += F("value=");
            data += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            data += F(";");
            data += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            data += F(";");
            data += humStat(UserVar[event->BaseVarIndex + 1]);
            break;
          case SENSOR_TYPE_TEMP_BARO:                      // temp + hum + hum_stat + bar + bar_fore, used for BMP085
            data += F("value=");
            data += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            data += F(";0;0;");
            data += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            data += F(";0");
            break;
          case SENSOR_TYPE_TRIPLE:
            data += F("value=");
            data += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            data += F(";");
            data += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            data += F(";");
            data += toString(UserVar[event->BaseVarIndex + 2], ExtraTaskSettings.TaskDeviceValueDecimals[2]);
            break;
          case SENSOR_TYPE_TEMP_HUM_BARO:                      // temp + hum + hum_stat + bar + bar_fore, used for BME280
            data += F("value=");
            data += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            data += F(";");
            data += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            data += F(";");
            data += humStat(UserVar[event->BaseVarIndex + 1]);
            data += F(";");
            data += toString(UserVar[event->BaseVarIndex + 2], ExtraTaskSettings.TaskDeviceValueDecimals[2]);
            data += F(";0");
            break;
          case SENSOR_TYPE_QUAD:
            data += F("value=");
            data += toString(UserVar[event->BaseVarIndex], ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            data += F(";");
            data += toString(UserVar[event->BaseVarIndex + 1], ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            data += F(";");
            data += toString(UserVar[event->BaseVarIndex + 2], ExtraTaskSettings.TaskDeviceValueDecimals[2]);
            data += F(";");
            data += toString(UserVar[event->BaseVarIndex + 3], ExtraTaskSettings.TaskDeviceValueDecimals[3]);
            break;
          case SENSOR_TYPE_SWITCH:
            data += F("value=");
            if (UserVar[event->BaseVarIndex] == 0)
              data += F("Off");
            else
              data += F("On");
            break;
          case SENSOR_TYPE_DIMMER:
            data += F("value=");
            if (UserVar[event->BaseVarIndex] == 0)
              data += ("Off");
            else
            {
              data += UserVar[event->BaseVarIndex];
            }
            break;
          case (SENSOR_TYPE_WIND):
            data += F("value=");                   // WindDir in degrees; WindDir as text; Wind speed average ; Wind speed gust; 0
            data += toString(UserVar[event->BaseVarIndex],ExtraTaskSettings.TaskDeviceValueDecimals[0]);
            data += ";";
            data += getBearing(UserVar[event->BaseVarIndex]);
            data += ";";
            // Domoticz expects the wind speed in (m/s * 10)
            data += toString((UserVar[event->BaseVarIndex + 1] * 10),ExtraTaskSettings.TaskDeviceValueDecimals[1]);
            data += ";";
            data += toString((UserVar[event->BaseVarIndex + 2] * 10),ExtraTaskSettings.TaskDeviceValueDecimals[2]);
            data += ";0";
            break;

        }

        if(lora_begin) {
            addLog(LOG_LEVEL_INFO, "Sending with lora");
            addLog(LOG_LEVEL_INFO, data);

            LoRa.beginPacket();
            LoRa.print(data);
            LoRa.endPacket();
          } else {
            addLog(LOG_LEVEL_ERROR, "LoRa failed to init!");
          }
        break;
      }
  }
  return success;
}

void setup_lora() {
  LoRa.setPins(2, 0);
  long lora_freq[3] = { 433E6, 866E6, 915E6};    // frequencies
  if (!LoRa.begin(lora_freq[1])) {
    lora_begin = false;
    addLog(LOG_LEVEL_ERROR, "Failed to start");
  } else {
    lora_begin = true;
    addLog(LOG_LEVEL_INFO, "LoRa started");
  }
}
