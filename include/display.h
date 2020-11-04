#ifndef DISPLAY_H 
#define DISPLAY_H 


#define DAY_WINDOW_WIDTH    200
#define DAY_WINDOW_HEIGHT   100

#define TEXT_WINDOW_WIDTH   200
#define TEXT_WINDOW_HEIGH   


#define MAX_DAY_ENTRIES 5


typedef struct {
    char* text;
    int time;
}entry;

class dayEntry {

    public:
    void put(entry* data); 
    void put(char* text, int time);
    void print(uint8_t nr);
    entry* _entries[5];
    uint8_t _entryCnt = 0;
};


class displayWrapper {

    public:
    void init(void);
    void printDay(uint8_t day);
    void printAppointments(void);
    dayEntry appointment; 
    private:
    uint8_t _day;
};

#endif
