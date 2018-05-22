#include "barjson.h"

barjson::barjson()
{
        _json_url="";
}

bool barjson::setHost(const std::string & h, const uint16_t & p)
{
        _json_host = h;
        _json_port = p;

        Serial.printf("Setting json host  to %s Port: %d\n", h.c_str(), p);


        if(!barjson::client.connect(_json_host.c_str(),_json_port))
        {
                Serial.println("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nConnection to json host failed!\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                return false;
        }

        Serial.println("\nConnection to json host worked!\n");
        barjson::client.stop();
        return true;
}


bool barjson::setURL(const std::string & s)
{
        Serial.print("1 ");
        _json_url = s;
        Serial.print("2 ");
        Serial.printf("Setting json url to %s\n", s.c_str());
        Serial.print("3 ");
        if(!barjson::client.connect(_json_host.c_str(),_json_port))
        {
                Serial.print("4 ");
                Serial.println("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nConnection to json host failed!\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                return false;
        }
        Serial.print("5 ");
        // This will send the request to the server
        barjson::client.print(String("GET ") + _json_url.c_str() + " HTTP/1.1\r\n" + "Host: " + _json_host.c_str() + "\r\n" + "Connection: close\r\n\r\n");
        Serial.print("6 ");
        unsigned long timeout = millis();
        Serial.print("7 ");
        while (barjson::client.available() == 0) {
                if (millis() - timeout > 5000) {
                        Serial.println(">>> Client Timeout !");
                        barjson::client.stop();
                        return false;
                }
        }

        Serial.println("\n>>>Connection to json URL worked!<<<\n");

        barjson::client.stop();

        return true;
}

void barjson::readJSON()
{
        if(!barjson::client.connect(_json_host.c_str(),_json_port))
                return;
// This will send the request to the server
        barjson::client.print(String("GET ") + _json_url.c_str() + " HTTP/1.1\r\n" + "Host: " + _json_host.c_str() + "\r\n" + "Connection: close\r\n\r\n");
        unsigned long timeout = millis();

        while (barjson::client.available() == 0) {
                if (millis() - timeout > 5000) {
                        Serial.println(">>> Client Timeout !");
                        barjson::client.stop();
                        return;
                }
        }
        Serial.println(">>>Finding first object<<<\n");
        while(client.peek()!='{')
        {
                Serial.print(char(client.read()));

        }
        Serial.println(">>>Done<<<\n");
        currentKeyLevel=0;
        handleJSON();


}

void barjson::handleJSON()
{
        while(client.available() )
        {
                char charTemp=nextChar();
                if(charTemp!='{' && charTemp!=',')
                {
                        Serial.printf("I fucked up somewhere\n");
                        Serial.println(client.peek());
                        while(client.available())
                                Serial.print(char(client.read()));
                        return;
                }
                else
                {

                        std::string temp {""};            //read variable name
                        eat('\"');
                        r();
                        //String temp2=client.readStringUntil(QUOTE).c_str();
                        keyNames[currentKeyLevel]=client.readStringUntil(QUOTE).c_str();
                        for(int x = 0; x <= currentKeyLevel; x++)
                                Serial.printf(".%s", keyNames[x].c_str());
                        client.readStringUntil(COLON);
                        Serial.print(':');
                        handleValue();
                        // Serial.println("Back from value");
                        if(nextChar('}'))
                        {
                                // Serial.println("End of JSON Object");
                                client.read();
                                return;
                        }


                }
        }
}


void barjson::handleValue()
{
        std::string temp {};
        char c = nextChar();
        switch (c)
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        case '.':
        case '-':
        {

                while(c>='-' && c<='9'  )
                {
                        temp += r();
                        c=p();
                }
                Serial.printf("%s\n",temp.c_str());
        } break;
        case 'f':
        case 'F':
        {
                Serial.println("False");
                while(!nextChar(',')&&!nextChar('}'))
                {
                        client.read();
                }

        } break;
        case 't':
        case 'T':
        {
                while(!nextChar(',')&&!nextChar('}'))
                {
                        client.read();
                }
                Serial.println("True");
        } break;
        case QUOTE:
        {
                r();
                String temp3 ="";
                while(p()!='\"')
                        temp3+=char(r());
                r();
                Serial.printf("%s\n",temp3.c_str());
        } break;
        case ']':
                return;
                break;
        case '[':
        {
                r();
                uint8_t numValues=0;
                Serial.printf("[");
                handleValue();

                while(nextChar(','))
                {
                        Serial.println("Array item done!\n\n\n\nlksadkllkadfsjklsdfljkfdsjklfdskljsfdkjl\n");
                        c=r();
                        handleValue();
                        numValues++;

                }
                if(nextChar()==']')
                {
                        Serial.println("\n\n\n\n\n\nEnd found");
                        r();
                        return;
                }
        } break;
        case '{':
        {
                currentKeyLevel++;
                handleJSON();
                currentKeyLevel--;
        } break;
        default:
                Serial.printf("Error\n");

        }


}

char barjson::eat()
{
        char t = client.peek();
        while(isWhitespace(t))
        {
                r();
                t=p();
        }
        return t;
}

char barjson::eat(const char &c)
{
        char t = p();;
        while(t!=c)
        {
                r();
                t=p();
        }

        return t;
}

char barjson::nextChar()
{
        // Serial.print("\nFinding next char...");
        char t = p();
        while((t<33 || t>126)&&client.available())
        {
                Serial.print(t);
                client.read();
                t=client.peek();
        }
        // Serial.printf("...Next char is %c\n", t);
        return t;
}
bool barjson::nextChar(const char & c) {

        return c==nextChar();

}

char barjson::p(){
        uint32_t local_time = millis()+5000;
        while(!client.available())
        {
                if(local_time<millis())
                        return 0;
                delay(1);
                Serial.print(',');
        }
        char t = client.peek();
        //Serial.print(t);
        return t;
}

char barjson::r(){
        uint32_t local_time = millis()+5000;
        while(!client.available() && local_time>millis())

        {
                delay(1);
                Serial.print(',');
        }
        char t = client.read();
        //Serial.print(t);
        return t;

}
