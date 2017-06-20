//#######################################################################################################
//########################### Controller Plugin 013: Nodewatcher ########################################
//#######################################################################################################
#ifdef PLUGIN_BUILD_TESTING

#define CPLUGIN_013
#define CPLUGIN_ID_013         13
#define CPLUGIN_NAME_013       "Slack bot"

#include "sha256.h"
#include "Base64.h"



boolean CPlugin_013(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
      {
        Protocol[++protocolCount].Number = CPLUGIN_ID_013;
        Protocol[protocolCount].usesMQTT = false;
        Protocol[protocolCount].usesAccount = true;
        Protocol[protocolCount].usesPassword = true;
        Protocol[protocolCount].defaultPort = 80;
        break;
      }

    case CPLUGIN_GET_DEVICENAME:
      {
        string = F(CPLUGIN_NAME_013);
        break;
      }

    case CPLUGIN_PROTOCOL_SEND:
      {

        ControllerSettingsStruct ControllerSettings;
        LoadControllerSettings(event->ControllerIndex, (byte*)&ControllerSettings, sizeof (ControllerSettings));

        char log[80];
        boolean success = false;
        char host[20];
        sprintf_P(host, PSTR("%u.%u.%u.%u"), ControllerSettings.IP[0], ControllerSettings.IP[1], ControllerSettings.IP[2], ControllerSettings.IP[3]); // ? not sure what it does
        sprintf_P(log, PSTR("%s%s using port %u"), "HTTP : connecting to ", host,ControllerSettings.Port);
        addLog(LOG_LEVEL_DEBUG, log);

        const char* SlackWebhookURL = "/services/T049VHKJF/B31NK8XQR/GXrWK4v4HEkmm5beAUcMew5Z";
        // SSL Certificate finngerprint for the host
        const char* fingerprint = "‎‎ab f0 5b a9 1a e0 ae 5f ce 32 2e 7c 66 67 49 ec dd 6d 6a 38";
        // create a secure connection using WiFiClientSecure
         WiFiClientSecure client;
         const int httpPort = 443;

         // verify the signature of the ssl certificate
         // if (client.verify(fingerprint, host)) {
         // Serial.println("ssl cert matches");
         // } else {
         // Serial.println("ssl cert mismatch");


        if (!client.connect(host, httpPort))
        {
          connectionFailures++;
          strcpy_P(log, PSTR("HTTP : connection failed"));
          addLog(LOG_LEVEL_ERROR, log);
          return false;
        }

        statusLED(true);
        if (connectionFailures)
          connectionFailures--;

       String channel = "#robocop";
       String username = "robocop-version1";
        // start constructing the message
        String postDataStr="payload={\"channel\": \"" + channel + "\", \"username\": \"" + username + "\", \"text\": \"";

        byte valueCount = getValueCountFromSensorType(event->sensorType); // multivalue handling required, figure out json
        for (byte x = 0; x < valueCount; x++)
        {
          postDataStr += String(ExtraTaskSettings.TaskDeviceName);
          postDataStr += F(" ");
          postDataStr += String(ExtraTaskSettings.TaskDeviceValueNames[x]);
          postDataStr += toString(UserVar[event->BaseVarIndex + x],ExtraTaskSettings.TaskDeviceValueDecimals[x]);
          postDataStr += F(" ");
        }
        postDataStr += F("\", \"icon_emoji\": \":ghost:\"}");

        client.print("POST ");
        client.print(SlackWebhookURL);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(host);
        client.println("User-Agent: ArduinoIoT/1.0");
        client.println("Connection: close");
        client.println("Content-Type: application/x-www-form-urlencoded;");
        client.print("Content-Length: ");
        client.println(postDataStr.length());
        client.println();
        client.println(postDataStr);

        unsigned long timer = millis() + 200;
        while (!client.available() && millis() < timer)
          delay(1);

        // Read all the lines of the reply from server and print them to Serial
        while (client.available()) {
          String line = client.readStringUntil('\n');
          line.toCharArray(log, 80);
          addLog(LOG_LEVEL_DEBUG_MORE, log);
          if (line.substring(0, 15) == "HTTP/1.1 200 OK")
          {
            strcpy_P(log, PSTR("HTTP : Succes!"));
            addLog(LOG_LEVEL_DEBUG, log);
            success = true;
          }
          delay(1);
        }
        strcpy_P(log, PSTR("HTTP : closing connection"));
        addLog(LOG_LEVEL_DEBUG, log);

        client.flush();
        client.stop();
        break;
      }

  }
  return success;
}
#endif
