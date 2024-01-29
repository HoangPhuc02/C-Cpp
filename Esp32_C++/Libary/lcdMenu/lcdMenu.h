#ifndef __LCD_MENU_H
#define __LCD_MENU_H

#include "LiquidCrystal_I2C.h"
#include "stdio.h"
#define MAX_TITLE_LENGTH 16   //Max chacracter on a row here example on 16x2 LCD
struct Button
{
    /* data */
};

typedef struct Linker
{
    Linker *pre;
    char Title_1[MAX_TITLE_LENGTH];
    char MenuList1[MAX_TITLE_LENGTH];
    Linker *MenuList1;
    char MenuList2[MAX_TITLE_LENGTH];
    Linker *MenuList2;
    char MenuList3[MAX_TITLE_LENGTH];
    Linker *MenuList3;

}Menu;

Menu MainMenu = {
    NULL,
    "   MAIN MENU    ",NULL,
    "  SENSOR        ",NULL,
    "  ACTUATOR      ",NULL,
    "  SETTING       ",NULL
};

/* SENSOR*/
Menu SensorMenu = {
    NULL,
    "  SENSOR MENU   ",
    "  WATER         ",NULL,
    "  LAND          ",NULL,
    "                ",NULL
};
Menu WaterSensorMenu = {
    NULL,
    "   WATER MENU   ",
    "  TEMP          ",NULL,
    "  WATER LEVEL   ",NULL,
    "                ",NULL
};
Menu LandSensorMenu = {
    NULL,
    "   LAND MENU    ",
    "  TEMP HUMI     ",NULL,
    "  PH            ",NULL,
    "  MOSTURE       ",NULL
};

/* ACTUAROR */
Menu ActuatorMenu = {
    NULL,
    "  ACTUATOR MENU ",
    "  SERVO         ",NULL,
    "  LED           ",NULL,
    "  RELAY         ",NULL
};
Menu ServoActuator = {
    NULL,
    "   WATER MENU   ",
    "  TEMP          ",NULL,
    "  WATER LEVEL   ",NULL,
    "                ",NULL
};
Menu LedActuator = {
    NULL,
    "   LAND MENU    ",
    "  TEMP HUMI     ",NULL,
    "  PH            ",NULL,
    "  MOSTURE       ",NULL
};
Menu RelayActuator = {
    NULL,
    "   LAND MENU    ",
    "  TEMP HUMI     ",NULL,
    "  PH            ",NULL,
    "  MOSTURE       ",NULL
};

void MenuDisplay(Menu *menu);



#endif