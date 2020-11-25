
#include <Arduino.h>
#include <string.h>

#include "display_includes.h"
#include "display.h"

GxEPD2_BW<GxEPD2_213_B72, GxEPD2_213_B72::HEIGHT> ePaper(GxEPD2_213_B72(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25)); // GDEH0213B72


void dayEntry::put(entry* data)
{
    if(MAX_DAY_ENTRIES - 1 > _entryCnt) 
    {
        _entries[_entryCnt] = data;
        _entryCnt ++;
    }
    
}

void dayEntry::put(char* text, int time)
{
    entry* data;
    char* buffer; 

    Serial.println("Putting ...");
    Serial.println(text);

    if(MAX_DAY_ENTRIES - 1 > _entryCnt) 
    {
        data = (entry*) malloc(sizeof(entry));
        //buffer = (char*) malloc(sizeof(text));
        buffer = (char*) malloc(strlen(text));
        strcpy(buffer, text);
        
        if((NULL != buffer) && (NULL != data))
        {
            data->text = buffer;
            data->time = time;
            _entries[_entryCnt] = data;
            _entryCnt ++;
        }
             
    }

    //TODO FREE te malloc
}

void dayEntry::deleteAll(void) {
    _entryCnt = 0;
    for(int i = 0; i < MAX_DAY_ENTRIES; i++) {
        free(_entries[i]);
        //_entries[i]->time = 0;
        _entries[i] = 0;
    }
}

void dayEntry::print(uint8_t nr) 
{
    entry* pEntry = NULL;

    if(_entryCnt >= nr) 
    {
        if(NULL != _entries[nr])
        {
            pEntry = _entries[nr];

            Serial.println(pEntry->text);
            Serial.println(pEntry->time);
        }
    }
}


void displayWrapper::init(void) {

  ePaper.init(115200); // uses standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)

  //// *** special handling for Waveshare ESP32 Driver board *** //
  //// ********************************************************* //
  SPI.end(); // release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
  //SPI: void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
  SPI.begin(13, 12, 14, 15); // map and init SPI pins SCK(13), MISO(12), MOSI(14), SS(15)
  //// *** end of special handling for Waveshare ESP32 Driver board *** //
  //// **************************************************************** //
  //
  ePaper.setRotation(3);
}

void displayWrapper::printDay(uint8_t day) {

    char text[2];
    if(day > 31) return;
    
    sprintf(text, "%02d", day);

    ePaper.setPartialWindow(0, 0, ePaper.width()/3, ePaper.height());
    ePaper.setTextColor(GxEPD_BLACK);
    //ePaper.setFont(&FreeSerifBold24pt7b);
    //ePaper.setFont(&OpenSansCondensed_Light48pt7b);
    ePaper.setFont(&OpenSansCondensed_Bold48pt7b);
    ePaper.setTextSize(1, 1);

    
    int16_t tbx, tby; uint16_t tbw, tbh;
    ePaper.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
    // center bounding box by transposition of origin:
    uint16_t x = ((ePaper.width()/3 - tbw) / 2) - tbx;
    uint16_t y = ((ePaper.height() - tbh) / 2) - tby;
    //uint16_t x = 5;
    //uint16_t y = 70;

    ePaper.firstPage();

    do
    {
      //ePaper.writeFillRect(0, 0, ePaper.width()/3, ePaper.height(), GxEPD_WHITE);
      ePaper.fillScreen(GxEPD_WHITE);
      ePaper.setCursor(x, y);
      ePaper.print(text);
    }
    while (ePaper.nextPage());
}

void displayWrapper::printAppointments(void) {
    uint8_t i = 0;
    int16_t x = 0;
    int16_t y = 0;

    ePaper.setPartialWindow(ePaper.width()/3, 0, ePaper.width()*2/3, ePaper.height());
    ePaper.setTextColor(GxEPD_BLACK);
    ePaper.setFont(&FreeMono9pt7b);
    ePaper.setTextSize(1, 1);

    x = ePaper.width()/3 + 10;

    Serial.println(ePaper.width());
    Serial.println(ePaper.height());
    Serial.println(ePaper.width()*2/3);

    ePaper.firstPage();

    Serial.println(appointment._entryCnt);
    for(i = 0; i < appointment._entryCnt; i ++)
    {
        Serial.println(appointment._entries[i]->text);
        ePaper.setCursor(x, 15 + 25*i);
        ePaper.printf("%d-%.10s", appointment._entries[i]->time, appointment._entries[i]->text);
    }

    ePaper.nextPage();


}
