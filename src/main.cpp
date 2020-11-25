


#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "time.h"
#include "display.h"
#include "display_wrapper.h"
#include "secrets.h"

#define LED_BUILTIN 2


const char* ssid = SECRET_SSID;
const char* password = SECRET_SSID_PWD;
const char* host = "script.google.com";
const char* gScriptId = SECRET_GSCRIPT_ID;
const int httpsPort = 443;

WiFiClientSecure client;

displayWrapper eDisplay;


/* Extract URL from http redirect - used especially for google as they always redirect to a new temporary URL */
String getRedirectURL(String url) {
  String tagStr = "Location: ";
  String endStr = "\n";

  int indexFrom = url.indexOf(tagStr) + tagStr.length();
  int indexTo = url.indexOf(endStr, indexFrom);

  return url.substring(indexFrom, indexTo);
}

// Generic code for getting requests - doing it a bit pure, as most libraries I tried could not handle the redirects from google
bool getRequest(char *urlServer, String urlRequest) {
  
  client.stop(); // close connection before sending a new request

  if (client.connect(urlServer, 443)) { // if the connection succeeds
    Serial.println("connecting...");

////    // send the HTTP PUT request:
////    client.println("GET " + urlRequest); // " HTTP/1.1"
////    //Serial.println("GET " + urlRequest);
////    client.println("User-Agent: ESP32 Receiver/1.1");
////    client.println();

    client.println(String("GET ") + urlRequest + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "User-Agent: ESP32 Receiver/1.1\r\n" +
                          "Connection: close\r\n" + 
                          "\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 10000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        Serial.println("Connection timeout");
        return false;
      }
    }
  } else {
    Serial.println("Calendar connection did not succeed");
  }



  return true;
}


String getGcalEntries(void) 
{
    String outputStr = "{}";
    String day = "7";
    String month = "November";
    String year = "2020";

    String url ="https://script.google.com/macros/s/";
    url += gScriptId;
    url += "/exec?";
    //url += "day=7&month=November&year=2020";
    url += "period=7";


    Serial.println("Getting calendar");
    Serial.println(url);

    //httpGet(url);

    getRequest((char*)host, url);

    outputStr = client.readString();

    // If redirected - then follow redirect - google always redirect to a temporary URL by default. Note that this at times fail resulting in an empty display. Consider detecting this and retrying.
    if(outputStr.indexOf("Location:") > 0) {
        Serial.println("Redirecting...");
        String newURL = getRedirectURL(outputStr);

        getRequest((char*)host, newURL);

        outputStr = client.readString();
        Serial.println(outputStr);
    }

    int startJson = -1;
    int endJson = -1;

    startJson = outputStr.indexOf("{");
    if(startJson >= 0) endJson = outputStr.indexOf("\r", startJson);
    if(endJson >= 0) {
        outputStr = outputStr.substring(startJson, endJson);
    }

    return outputStr;
}

void wifiInit(void) 
{
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("CONNECTED");
    Serial.printf("IP address: ");
    Serial.println(WiFi.localIP());
    //END WIFI
}


void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");

    //displayInit();
    eDisplay.init();

    wifiInit();
    configTime(3600, 3600, "pool.ntp.org");

    //displayTestText("Puta Caca");
    //displayTest();
        
    pinMode(LED_BUILTIN, OUTPUT);
}

void getlocaltime(void) {
    char buffer[256];
    struct tm timeInfo;
    
    if(!getLocalTime(&timeInfo)){
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeInfo);
//tm_sec	int	seconds after the minute	0-60*
//tm_min	int	minutes after the hour	0-59
//tm_hour	int	hours since midnight	0-23
//tm_mday	int	day of the month	1-31
//tm_mon	int	months since January	0-11
//tm_year	int	years since 1900	
//tm_wday	int	days since Sunday	0-6
//tm_yday	int	days since January 1	0-365
//tm_isdst	int	Daylight Saving Time flag	
}


void displayCntTest(void) {
    int date = 0;
    while(1) {
       
        eDisplay.printDay(date);
        Serial.println(date);
        delay(1000);
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        date++;
        if(date == 30) date = 0;
    }
}

void loop()
{
    char buf[50];
    char text[100];

    static int day = 1;
    static bool bold = 1;  
    DynamicJsonDocument jsonDoc(4048);
    //StaticJsonDocument<1024> jsonDoc;

    String gcalString = "{}";
    char jsonBuffer[1024];
   
    displayCntTest();

    delay(1000);

    getlocaltime();

    gcalString = getGcalEntries();
    //gcalString = getCalendar();
    //Serial.println(gcalString);
    
    DeserializationError error = deserializeJson(jsonDoc, gcalString);
    
    if(error) {
        //Serial.println(error);
        Serial.print("deserialize error= ");
        Serial.println(error.c_str());
    }

    Serial.println(jsonDoc.size());

    JsonObject documentRoot = jsonDoc.as<JsonObject>();


    for (JsonPair keyValue : documentRoot) {
        Serial.println(keyValue.key().c_str());
    }

    const char* startDay = jsonDoc["startDay"];    
    int date;
    const char* entryTime;
    const char* entryTitle; 

    Serial.print("startDay = ");
    Serial.println(startDay);

    if(jsonDoc.containsKey("days")) {
        Serial.print("days = ");
        JsonArray daysArray = jsonDoc["days"].as<JsonArray>();
        Serial.print("Length =");
        Serial.println(daysArray.size());
        for(JsonVariant days : daysArray) {
            //Serial.println(val.as<char*>());
            JsonObject dayObj = days.as<JsonObject>(); 
            date = dayObj["date"].as<int>();
            Serial.print("date =");
            Serial.println(date);
            Serial.println(""); 

            eDisplay.printDay(date);
            
            JsonArray entriesArray = dayObj["entries"].as<JsonArray>();
            for(JsonVariant entries : entriesArray) {
                JsonObject entryObj = entries.as<JsonObject>();
                entryTime = entryObj["time"];
                entryTitle = entryObj["title"];
                Serial.print(entryTime);
                Serial.print("\t");
                Serial.println(entryTitle);
                eDisplay.appointment.put((char*)entryTitle, (int)atol(entryTime));
            }

            Serial.println("-------------------");

            eDisplay.printAppointments();
            eDisplay.appointment.deleteAll();

            delay(5000);
        }
    }

    //eDisplay.printDay(day);
    //eDisplay.appointment.put((char*)"Dentista", 10);
    //eDisplay.appointment.put((char*)"Birres", 18);
    //eDisplay.appointment.put((char*)"Porros", 22);
    //eDisplay.printAppointments();
    
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    day++;
    if(day > 31) day = 1;
    bold = !bold;

    delay(5000);
}



