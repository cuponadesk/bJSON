#include "SD.h"
#include "ESP8266WiFi.h"
#include "barjson.h"
void setup()
{
        Serial.begin(115200);

        // We start by connecting to a WiFi network

        Serial.println();
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println("Dalek");

/* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
   would try to act as both a client and an access-point and could cause
   network-issues with your other WiFi-devices on your WiFi-network. */
        WiFi.mode(WIFI_STA);
        WiFi.begin("Dalek", "5132841174");

        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
        }

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
}

void loop()
{
        Serial.println(millis());
        barjson newJson;
        newJson.setHost("statsapi.mlb.com");
        newJson.setURL("/api/v1/schedule/games?&sportId=1&date=04/29/2018");
        newJson.readJSON();
        delay(50000);

}
