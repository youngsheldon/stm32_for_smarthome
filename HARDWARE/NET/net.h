#ifndef __NET_H
#define __NET_H  
#include "sys.h"
#include "edp.h"
#include "usart2.h"
#include "led.h"
#include "delay.h"
#include "ui.h"
#include "stm32f10x_it.h" 
#include "eeprom.h"

#define KEY  "zwkfOrveVdV7eAq4xJpomVDdyUs="    //APIkey
#define ID   "1085544"                          //设备ID
#define PUSH_ID NULL

void NET_Init(void);
void packetSend(edp_pkt* pkt);
void ConnectToDevice(void);
void UpLoadZigbeeEndDeviceSensorDataToCloud(void);
void UpLoadZigbeeEndDeviceOtherSensorDataToCloud(void);
void SwitchDetect( );
extern uint16_t VirtAddVarTab[57];
extern int ServerConnectState;
#endif

/*
10 30 00 03 45 44 50 01 40 00 80 00 07 31 30 38 35 35 34 34 00 1C 7A 77 6B 66 4F 72 76 65 56 64 56 37 65 41 71 34 78 4A 70 6F 6D 56 44 64 79 55 73 3D 
*/

/*
80 18 00 05 00 14 2C 3B 6C 69 67 68 74 20 69 6E 74 65 6E 73 69 74 79 2C 33 34 
*/
//AT+CIPSTART="TCP","183.230.40.39",876
//AT+CIPMODE=1
//AT+CIPSEND
//
////80 10 80 1C 60 62 6C 9C 02 70 9E 90 E2 7C 9F EE 90 1E 9E 1E
///80 10 80 1C 60 62 6C 9C 02 70 9E 90 E2 7C 9E EE 90 1E 9E 1E
///80 1E 00 02 60 02 7C 9E F0 82 70 F3 FE
///80 1E 00 02 60 02 7C 9F F0 82 70 F3 FE
///
///A0 32 00 24 30 38 37 37 64 33 38 35 2D 65 30 35 32 2D 35 38 39 35 2D 61 38 35 64 2D 37 33 35 30 65 66 36 33 39 31 36 33 00 00 00 08 73 77
///
///A0 32 00 24 61 65 62 62 64 35 62 35 2D 35 35 36 66 2D 35 39 31 37 2D 62 65 32 65 2D 63 64 38 65 30 30 39 66 36 31 36 36 00 00 00 08 73 77
///
///
/// A0 32 00 24 63 36 61 35 65 34 64 38 2D 31 38 36 37 2D 35 30 62 32 2D 38 37 36 32 2D 64 38 65 38 37 37 31 36 36 34 39 64 00 00 00 08 73 77 
/// 
/// 
//40 01 3C 40 01 3C 
//解析存储转发标志消息标志位时出错。
//
////40 01 3C    48+12
//在发送连接请求以前发送了SAVE_DATA消息。
//再发送连接请求
