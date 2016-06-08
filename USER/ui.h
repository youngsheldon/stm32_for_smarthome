#ifndef _UI_H_
#define _UI_H_
#include "sys.h"
#include "oled.h"
#include "led.h"
#include "key.h"
#include "eeprom.h"
#include "stm32f10x_flash.h"

#define LEFTDIR 1
#define RIGHTDIR 2

typedef struct 
{
    //data
    char ShorAddr[4];
    int Tempeture;
    int Humity;
    int SoilHumidity;
    int LightIllumination;
    int GasConcentration;
    int FireFlag;
    int HumanFlag;
    int RainFlag;
    //command
    int AddWaterFlag;
    int AddLightFlag;
    int OpenWindowFlag;
    int OpenFanFlag;
}DEV;

extern DEV dev1,dev2,dev3,dev4,dev5;

extern int DeviceOnlineState[5];
extern int Lamp[6];
extern int SendCmdFlag;
extern int ModeCouter;
extern u8 SendRequestBuf[8];

void EEPROM_Init(void);
void ReadLedStatusFromFlash(void);

void AllDeviceInit(void);

void DetectCommand(int TagCouter,int OperatationCouter);
void SendCommand(void);
void ChangeComand(int DeviceId,int SensorId,int Motion);

void MainMenuItemShow();
void EndDeviceOnlineStateShow(int EndDeviceId,int status);
void MainMenuMoveTag(int MoveTagCouter);
void EndDeviceAddrItemShow(char *ShortAddr);
void TempetureSensorItemShow(int Tempeture);
void HumitySensorItemShow(int Humity);
void SoilHumidityItemShow(int SoilHumidity);
void LightIlluminationItemShow(int LightIllumination);
void GasSensorItemShow(int GasConcentration);
void FireSensorItemShow(int flag);
void AddWaterItemShow(int flag);
void AddLightItemShow(int flag);
void WindowOperationItemShow(int flag);
void FanOperationItemShow(int flag);
void HumanSensorItemShow(int flag);
void RainSensorItemShow(int flag);

void EndDeviceTag(int flag);
void MainMenuIntoSubMenu();

void FireAlarm();
#endif

