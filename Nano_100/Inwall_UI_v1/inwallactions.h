#ifndef INWALLACTIONS_H_INCLUDED
#define INWALLACTIONS_H_INCLUDED

#include "inwallconstants.h"
extern void parseUart(void);
extern void switchPressAction(uint8_t switchID);
extern void inwallInit(void);
extern void inwallWakeUp(void);

struct switchLayout
{
    uint8_t state;
    uint8_t config;
};

typedef struct
{
    uint8_t sceneID[MAX_NO_OF_SCENES];
    struct switchLayout arr_switch[MAX_NO_OF_SWITCHES];
    uint8_t fanID;
    uint8_t fanState;
    uint8_t fanLevel;

} inwallMemoryLayout;


#endif