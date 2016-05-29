#include "net.h"

char temperature[][18] = {"dev1->temperature","dev2->temperature","dev3->temperature","dev4->temperature","dev5->temperature"};
char humidity[][15] = {"dev1->humidity","dev2->humidity","dev3->humidity","dev4->humidity","dev5->humidity"};

void NET_Init(void)
{
    delay_ms(400);
    Uart2_Put_String("AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n");
    delay_ms(200);
    Uart2_Put_String("AT+CIPMODE=1\r\n");
    delay_ms(200);
    Uart2_Put_String("AT+CIPSEND\r\n");
    delay_ms(200);
    Uart2_Put_String("AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n");
    delay_ms(200);
    Uart2_Put_String("AT+CIPMODE=1\r\n");
    delay_ms(100);
    Uart2_Put_String("AT+CIPSEND\r\n");
    delay_ms(200);
    LED3 = 0;
}

void packetSend(edp_pkt* pkt)
{
    if (pkt != NULL)
    {
        Uart2_SendPackt(pkt->data,pkt->len);
        free(pkt);              //回收内存
    }
}


void ConnectToDevice(void)
{
    packetSend(packetConnect(ID, KEY));
}

void UpLoadData(char *Name,int Data)
{
    char temp[2];
    sprintf( temp, "%d", Data );
    packetSend(packetDataSaveTrans(PUSH_ID, Name, temp)); 
}

// typedef struct 
// {
//     //data
//     char ShorAddr[4];
//     int Tempeture;
//     int Humity;
//     int SoilHumidity;
//     int LightIllumination;
//     int GasConcentration;
//     int FireFlag;
//     int HumanFlag;
//     int RainFlag;
//     //command
//     int AddWaterFlag;
//     int AddLightFlag;
//     int OpenWindowFlag;
//     int OpenFanFlag;
// }DEV;

void UpLoadZigbeeEndDeviceOtherSensorDataToCloud(void)
{
    //节点1(阳台)
    if(DeviceOnlineState[0])
    UpLoadData("dev1->SoilHumidity",dev1.SoilHumidity);
    delay_us(10);
    if(DeviceOnlineState[0])
    UpLoadData("dev1->LightIllumination",dev1.LightIllumination);
    delay_us(10);
    //节点2(厨房)
    if(DeviceOnlineState[1])
    UpLoadData("dev2->GasConcentration",dev2.GasConcentration);
    delay_us(10);
    if(DeviceOnlineState[1])
    UpLoadData("dev2->FireFlag",!dev2.FireFlag);
    delay_us(10);
    //节点3(卧室)
    if(DeviceOnlineState[2])
    UpLoadData("dev3->LightIllumination",dev3.LightIllumination);
    delay_us(10);
    //节点4(户外)
    if(DeviceOnlineState[3])
    UpLoadData("dev4->LightIllumination",dev4.LightIllumination);
    delay_us(10);
    if(DeviceOnlineState[3])
    UpLoadData("dev4->RainFlag",!dev4.RainFlag);
    delay_us(10);
    //节点5(门外)
    if(DeviceOnlineState[4])
    UpLoadData("dev5->HumanFlag",dev5.HumanFlag);
    delay_us(10);
}

void UpLoadZigbeeEndDeviceSensorDataToCloud(void)
{
    //温度数据
    if(DeviceOnlineState[0])
    UpLoadData(temperature[0],dev1.Tempeture);
    delay_us(10);
    if(DeviceOnlineState[1])
    UpLoadData(temperature[1],dev2.Tempeture);
    delay_us(10);
    if(DeviceOnlineState[2])
    UpLoadData(temperature[2],dev3.Tempeture);
    delay_us(10);
    if(DeviceOnlineState[3])
    UpLoadData(temperature[3],dev4.Tempeture);
    delay_us(10);
    if(DeviceOnlineState[4])
    UpLoadData(temperature[4],dev5.Tempeture);
    delay_us(10);
    //湿度数据
    if(DeviceOnlineState[0])
    UpLoadData(humidity[0],dev1.Humity);
    delay_us(10);
    if(DeviceOnlineState[1])
    UpLoadData(humidity[1],dev2.Humity);
    delay_us(10);
    if(DeviceOnlineState[2])
    UpLoadData(humidity[2],dev3.Humity);
    delay_us(10);
    if(DeviceOnlineState[3])
    UpLoadData(humidity[3],dev4.Humity);
    delay_us(10);
    if(DeviceOnlineState[4])
    UpLoadData(humidity[4],dev5.Humity);
    delay_us(10);
}

void SwitchDetect( )
{
    if(RxCompeleteFlag == 1)
    {
        RxCompeleteFlag = 0;
        LED2 = 0;
        delay_ms(20);
        LED2 = 1;
        if(SaveRxBuffer[44]=='A' && SaveRxBuffer[45]=='1')
        {
            ChangeComand(3,17,1);
            dev3.OpenWindowFlag = 1;
            SendCmdFlag = 1;
            UpLoadData("BedroomWindowSwitch",1);
            // EE_WriteVariable(VirtAddVarTab[0], 1);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='A' && SaveRxBuffer[45]=='0')
        {
            ChangeComand(3,17,0);
            dev3.OpenWindowFlag = 0;
            SendCmdFlag = 1;
            UpLoadData("BedroomWindowSwitch",0);
            // EE_WriteVariable(VirtAddVarTab[0], 0);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }

        if(SaveRxBuffer[44]=='A' && SaveRxBuffer[45]=='2')
        {
            ChangeComand(3,17,2);
            dev3.OpenWindowFlag = 2;
            SendCmdFlag = 1;
            UpLoadData("BedroomWindowSwitch",2);
            // EE_WriteVariable(VirtAddVarTab[0], 0);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }        

        if(SaveRxBuffer[44]=='B' && SaveRxBuffer[45]=='1')
        {
            ChangeComand(3,16,1);
            dev3.AddLightFlag = 1;
            SendCmdFlag = 1;
            UpLoadData("BedroomLightSwitch",1);
            // EE_WriteVariable(VirtAddVarTab[1], 1);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='B' && SaveRxBuffer[45]=='0')
        {
            ChangeComand(3,16,0);
            dev3.AddLightFlag = 0;
            SendCmdFlag = 1;
            UpLoadData("BedroomLightSwitch",0);
            // EE_WriteVariable(VirtAddVarTab[1], 0);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='C' && SaveRxBuffer[45]=='1')
        {
            ChangeComand(3,18,1);
            dev3.OpenFanFlag = 1;
            SendCmdFlag = 1;
            UpLoadData("BedroomFanSwitch",1);
            // EE_WriteVariable(VirtAddVarTab[2], 1);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='C' && SaveRxBuffer[45]=='0')
        {
            ChangeComand(3,18,0);
            dev3.OpenFanFlag = 0;
            SendCmdFlag = 1;
            UpLoadData("BedroomFanSwitch",0);
            // EE_WriteVariable(VirtAddVarTab[2], 0);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='D' && SaveRxBuffer[45]=='1')
        {
            ChangeComand(1,16,1);
            dev1.AddLightFlag = 1;
            SendCmdFlag = 1;
            UpLoadData("BalconyLightSwitch",1);
            // EE_WriteVariable(VirtAddVarTab[3], 1);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='D' && SaveRxBuffer[45]=='0')
        {
            ChangeComand(1,16,0);
            dev1.AddLightFlag = 0;
            SendCmdFlag = 1;
            UpLoadData("BalconyLightSwitch",0);
            // EE_WriteVariable(VirtAddVarTab[3], 0);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='E' && SaveRxBuffer[45]=='1')
        {
            ChangeComand(1,15,1);
            dev1.AddWaterFlag = 1;
            SendCmdFlag = 1;
            UpLoadData("BalconyWaterSwitch",1);
            // EE_WriteVariable(VirtAddVarTab[4], 1);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='E' && SaveRxBuffer[45]=='0')
        {
            ChangeComand(1,15,0);
            dev1.AddWaterFlag = 0;
            SendCmdFlag = 1;
            UpLoadData("BalconyWaterSwitch",0);
            // EE_WriteVariable(VirtAddVarTab[4], 0);//将LED(继电器的状态值写入FLASH)
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='F' && SaveRxBuffer[45]=='1')
        {
            ModeCouter = 1;
            SendRequestBuf[6] = 1;
            SendCmdFlag = 1;
            UpLoadData("ControlMode",1);
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        if(SaveRxBuffer[44]=='F' && SaveRxBuffer[45]=='0')
        {
            ModeCouter = 0;
            SendRequestBuf[6] = 0;
            SendCmdFlag = 1;
            UpLoadData("ControlMode",0);
            LED2 = 0;
            delay_ms(10);
            LED2 = 1;
        }
        //处理异常掉线
        if(SaveRxBuffer[0] == 0x40 && SaveRxBuffer[1] == 0x01 && SaveRxBuffer[2] == 0x3C )
        {
            StartToUploadFlag = 0;
            delay_ms(100);
            ConnectToDevice( );
            delay_ms(100);
            StartToUploadFlag = 1;
            LED2 = 0;
            delay_ms(100);
            LED2 = 1;
        }
        //esp8266异常重启处理
        if(strstr(SaveRxBuffer,"Ai-Thinker Technology") || strstr(SaveRxBuffer,"wdt reset") || strstr(SaveRxBuffer,"wdt ready"))
        {
            StartToUploadFlag = 0;
            delay_ms(1000);
            NET_Init();
            ConnectToDevice();
            delay_ms(100);
            StartToUploadFlag = 1;
            LED2 = 0;
            delay_ms(100);
            LED2 = 1;
        }

        memset(SaveRxBuffer,0,600);
        RXCouter = 0;
    }
}


//  ets Jan  8 2013,rst cause:4, boot mode:(3,6)

// wdt reset
// load 0x40100000, len 1396, room 16 
// tail 4
// chksum 0x89
// load 0x3ffe8000, len 776, room 4 
// tail 4
// chksum 0xe8
// load 0x3ffe8308, len 540, room 4 
// tail 8
// chksum 0xc0
// csum 0xc0

// 2nd boot version : 1.4(b1)
//   SPI Speed      : 40MHz
//   SPI Mode       : DIO
//   SPI Flash Size & Map: 8Mbit(512KB+512KB)
// jump to run user1 @ 1000

// rl莾;dd?
// Ai-Thinker Technology Co.,Ltd.

// ready

//  ets Jan  8 2013,rst cause:4, boot mode:(3,6)

// wdt reset
// load 0x40100000, len 1396, room 16 
// tail 4
// chksum 0x89
// load 0x3ffe8000, len 776, room 4 
// tail 4
// chksum 0xe8
// load 0x3ffe8308, len 540, room 4 
// tail 8
// chksum 0xc0
// csum 0xc0

// 2nd boot version : 1.4(b1)
//   SPI Speed      : 40MHz
//   SPI Mode       : DIO
//   SPI Flash Size & Map: 8Mbit(512KB+512KB)
// jump to run user1 @ 1000

// ;l噧{dd?
// Ai-Thinker Technology Co.,Ltd.

// ready
// WIFI CONNECTED

