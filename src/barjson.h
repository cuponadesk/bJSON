

#ifndef _BARJSON_
#define _BARJSON_

#define QUOTE '\"'
#define COLON ':'

#include <string>
#include <Arduino.h>
#include "ESP8266WiFi.h"

class barjson {
public:
barjson();

bool setURL(const std::string &s);
bool setHost(const std::string &h, const uint16_t & p = 80);
void readJSON();

private:

void handleJSON();//parse a json object {"keyID": [ (INT,FLOAT,STRING,JSON OBJECT) ] }
void handleValue();
char eat();
char eat(const char &c);
char r();

char p();
bool nextChar(const char & c);
char nextChar();
std::string _json_host, _json_url;
uint16_t _json_port;
WiFiClient client;
std::string keyNames[10];
uint8_t currentKeyLevel;
};

#endif
