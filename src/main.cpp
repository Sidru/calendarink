

#include <Arduino.h>
#include <WiFi.h>
#include "time.h"

#include "display.h"
#include "display_wrapper.h"


#define LED_BUILTIN 2

const char* ssid = "DoReMiFaSolLaSi";
const char* password = "BigMilkRat12";

displayWrapper eDisplay;

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


dayEntry dayData;

void loop()
{
    char buf[50];
    char text[100];

    static int day = 1;
    static bool bold = 1;  

    delay(1000);

    getlocaltime();
    // Sprintf 02d doesnt work :S
    //displayPrintTextFast(buf, 3);
    eDisplay.printDay(day);


    //dayData.put((char*)"Dentista", 10);
    //dayData.put((char*)"Birres", 18);
    //dayData.put((char*)"Porros", 22);
    //dayData.print(1); 

    eDisplay.appointment.put((char*)"Dentista", 10);
    eDisplay.appointment.put((char*)"Birres", 18);
    eDisplay.appointment.put((char*)"Porros", 22);
    
    eDisplay.printAppointments();

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    day++;
    if(day > 31) day = 1;
    bold = !bold;
}



