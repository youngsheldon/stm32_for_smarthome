#include "ui.h"
DEV dev1,dev2,dev3,dev4,dev5;
int DeviceOnlineState[5] = {0};
u8 SendRequestBuf[8]={0x07,0xef,0xfe,0x00,0x00,0x00,0x00,0xef};//3:节点id（1,2,3） 4：传感器id（DHT11 1，LIGHTED 2，DS18B20 3，Motor 4）  5：动作 （0，1） 
int Lamp[6] = {0};
int SendCmdFlag = 1;
int ModeCouter = 0;

static int BalconyControlBuf[2] = {0};
static int BedroomControlBuf[3] = {0};

uint16_t VirtAddVarTab[57] = {
    0xAA00, 0xAA01, 0xAA02, 0xAA03, 0xAA04, 0xAA05, 0xAA06, 0xAA07, 0xAA08, 0xAA09,
    0xAA0A, 0xAA0B, 0xAA0C, 0xAA0D, 0xAA0E, 0xAA0F,0xAA10, 0xAA11, 0xAA12, 0xAA13, 
    0xAA14, 0xAA15, 0xAA16, 0xAA17, 0xAA18, 0xAA19, 0xAA1A, 0xAA1B, 0xAA1C, 0xAA1D,
    0xAA1E, 0xAA1F,0xAA20, 0xAA21, 0xAA22, 0xAA23, 0xAA24, 0xAA25, 0xAA26, 0xAA27, 
    0xAA28, 0xAA29, 0xAA2A, 0xAA2B, 0xAA2C, 0xAA2D, 0xAA2E, 0xAA2F,0xAA30, 0xAA31,
    0xAA32, 0xAA33, 0xAA34, 0xAA35, 0xAA36, 0xAA37, 0xAA38,};

void EEPROM_Init(void)
{
    FLASH_Unlock();
    EE_Init();
}

void ReadLedStatusFromFlash(void)
{
    int i =0;

    for(;i < 5; i++)
    EE_ReadVariable(VirtAddVarTab[i],(uint16_t *)(Lamp+i));
}

/**************************************************************************************************************/
// static int BalconyControlBuf[2] = {0};
// static int BedroomControlBuf[3] = {0};
//     int AddWaterFlag;       15
//     int AddLightFlag;       16
//     int OpenWindowFlag;     17
//     int OpenFanFlag;        18
void ChangeComand(int DeviceId,int SensorId,int Motion)
{
    SendRequestBuf[3]=DeviceId;
    SendRequestBuf[4]=SensorId;
    SendRequestBuf[5]=Motion;
}


void DetectCommand(int TagCouter,int OperatationCouter)
{
    if(TagCouter == 0)
    {
        switch(OperatationCouter)
        {
            case 0:
            if(BalconyControlBuf[0])
            {
                ChangeComand(1,16,1);
            }
            else
            {
                ChangeComand(1,16,0);
            }
            break;

            case 1:
            if(BalconyControlBuf[1])
            {
                ChangeComand(1,15,1);
            }
            else
            {
                ChangeComand(1,15,0);
            }
            break;
        }
    }

    if(TagCouter == 2)
    {
        switch(OperatationCouter)
        {
            case 0:
            if(BedroomControlBuf[0] == 0)
            {
                ChangeComand(3,17,0);
            }
            if(BedroomControlBuf[0] == 1)
            {
                ChangeComand(3,17,1);
            }
            if(BedroomControlBuf[0] == 2)
            {
                ChangeComand(3,17,2);
            }
            break;

            case 1:
            if(BedroomControlBuf[1])
            {
                ChangeComand(3,16,1);
            }
            else
            {
                ChangeComand(3,16,0);
            }
            break;

            case 2:
            if(BedroomControlBuf[2])
            {
                ChangeComand(3,18,1);
            }
            else
            {
                ChangeComand(3,18,0);
            }
            break;
        }
    }
}

void SendCommand(void)
{
    u8 i;
    for(i=0;i<8;i++)
    {
        USART_SendData(USART1, SendRequestBuf[i]);
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
    }
}

/**************************************************************************************************************/
void DeviceMemberInit(DEV*dev)
{
    memset(dev->ShorAddr,0,4);
    dev->Tempeture = 0;
    dev->Humity = 0;
    dev->FireFlag = 2;
    dev->HumanFlag = 2;
    // dev->GasFlag = 2;
    // dev->LampFlag = 2;
    dev->RainFlag = 2;
}

void AllDeviceInit(void)
{
    DeviceMemberInit(&dev1);
    DeviceMemberInit(&dev2);
    DeviceMemberInit(&dev3);
    DeviceMemberInit(&dev4);
    DeviceMemberInit(&dev5);
} 
/**************************************************************************************************************/

void MainMenuItemShow()
{
    OLED_ShowCHinese(0,0,0);
    OLED_ShowCHinese(16,0,1);
    OLED_ShowCHinese(32,0,2);
    OLED_ShowCHinese(48,0,3);
    OLED_ShowCHinese(64,0,4);
    OLED_ShowCHinese(80,0,5);
    OLED_ShowCHinese(96,0,6);
    OLED_ShowCHinese(112,0,7);

    OLED_ShowCHinese(0,2,8); //阳台1
    OLED_ShowCHinese(16,2,9);
    OLED_ShowNum(32,2,1,1,16);
    OLED_ShowChar(40,2,':');
    
   
    OLED_ShowCHinese(64,2,10); //厨房2
    OLED_ShowCHinese(80,2,11);
    OLED_ShowNum(96,2,2,1,16);
    OLED_ShowChar(104,2,':');
   
    OLED_ShowCHinese(0,4,12); //卧室3
    OLED_ShowCHinese(16,4,13);
    OLED_ShowNum(32,4,3,1,16);
    OLED_ShowChar(40,4,':');
   
    OLED_ShowCHinese(64,4,14); //户外4
    OLED_ShowCHinese(80,4,15);
    OLED_ShowNum(96,4,4,1,16);
    OLED_ShowChar(104,4,':');

    OLED_ShowCHinese(0,6,16); //门外5
    OLED_ShowCHinese(16,6,15);
    OLED_ShowNum(32,6,5,1,16);
    OLED_ShowChar(40,6,':');

    OLED_ShowCHinese(64,6,56);//模式:
    OLED_ShowCHinese(80,6,57);
    OLED_ShowChar(96,6,':');
}


void ControlModeItemShow(int mode)
{
    switch(mode)
    {
        case 0:
        OLED_ShowCHinese(104,6,54);
        break;

        case 1:
        OLED_ShowCHinese(104,6,58);
        break;
    }
}

void EndDeviceOnlineStateShow(int EndDeviceId,int status)
{
    if(status)
    {
        switch(EndDeviceId)
        {
            case 1:
            OLED_ShowChar(48,2,'+');
            break;

            case 2:
            OLED_ShowChar(112,2,'+');
            break;

            case 3:
            OLED_ShowChar(48,4,'+');
            break;

            case 4:
            OLED_ShowChar(112,4,'+');
            break;

            case 5:
            OLED_ShowChar(48,6,'+');
            break;

            case 6:
            OLED_ShowChar(112,6,'+');
            break;
        }
    }
    else
    {
        switch(EndDeviceId)
        {
            case 1:
            OLED_ShowChar(48,2,'-');
            break;

            case 2:
            OLED_ShowChar(112,2,'-');
            break;

            case 3:
            OLED_ShowChar(48,4,'-');
            break;

            case 4:
            OLED_ShowChar(112,4,'-');
            break;

            case 5:
            OLED_ShowChar(48,6,'-');
            break;

            case 6:
            OLED_ShowChar(112,6,'-');
            break;
        }
    }
}

void DeviceOnlineShow(void)
{
    int i = 1;
    for(;i<6;i++)
    EndDeviceOnlineStateShow(i,DeviceOnlineState[i - 1]);
}

void MainMenuMoveTag(int MoveTagCouter)
{
    switch(MoveTagCouter)
    {
        case 0:
        OLED_ShowChar(56,2,'<');
        OLED_ShowChar(120,2,' ');
        OLED_ShowChar(56,4,' ');
        OLED_ShowChar(120,4,' ');
        OLED_ShowChar(56,6,' ');
        break;

        case 1:
        OLED_ShowChar(56,2,' ');
        OLED_ShowChar(120,2,'<');
        OLED_ShowChar(56,4,' ');
        OLED_ShowChar(120,4,' ');
        OLED_ShowChar(56,6,' ');
        break;

        case 2:
        OLED_ShowChar(56,2,' ');
        OLED_ShowChar(120,2,' ');
        OLED_ShowChar(56,4,'<');
        OLED_ShowChar(120,4,' ');
        OLED_ShowChar(56,6,' ');
        break;

        case 3:
        OLED_ShowChar(56,2,' ');
        OLED_ShowChar(120,2,' ');
        OLED_ShowChar(56,4,' ');
        OLED_ShowChar(120,4,'<');
        OLED_ShowChar(56,6,' ');
        break;

        case 4:
        OLED_ShowChar(56,2,' ');
        OLED_ShowChar(120,2,' ');
        OLED_ShowChar(56,4,' ');
        OLED_ShowChar(120,4,' ');
        OLED_ShowChar(56,6,'<');
        break;
    }
}

void EndDeviceAddrItemShow(char *ShortAddr)
{
    OLED_ShowCHinese(0,0,44); //地址:xxxx
    OLED_ShowCHinese(16,0,45);
    OLED_ShowChar(32,0,'>');
    OLED_ShowChar(40,0,'0');
    OLED_ShowChar(48,0,'X');
    OLED_ShowString(56,0,ShortAddr,4); 
}

void TempetureSensorItemShow(int Tempeture)
{
    OLED_ShowCHinese(0,2,17); //温度
    OLED_ShowCHinese(16,2,18);
    OLED_ShowChar(32,2,'>');
    OLED_ShowNum(40,2,Tempeture,2,16);
}

void HumitySensorItemShow(int Humity)
{
    OLED_ShowCHinese(64,2,19); //湿度
    OLED_ShowCHinese(80,2,18);
    OLED_ShowChar(96,2,'>');
    OLED_ShowNum(104,2,Humity,2,16);    
}

void SoilHumidityItemShow(int SoilHumidity)
{
    OLED_ShowCHinese(0,4,20); //土湿
    OLED_ShowCHinese(16,4,19);
    OLED_ShowChar(32,4,'>');
    OLED_ShowNum(40,4,SoilHumidity,3,16);
}

void LightIlluminationItemShow(int LightIllumination)
{
    OLED_ShowCHinese(64,4,21); //光强
    OLED_ShowCHinese(80,4,22);
    OLED_ShowChar(96,4,'>');
    OLED_ShowNum(104,4,LightIllumination,3,16);    
}           

void GasSensorItemShow(int GasConcentration)
{
    OLED_ShowCHinese(0,4,26); //有害气体浓度
    OLED_ShowCHinese(16,4,27);
    OLED_ShowCHinese(32,4,28);
    OLED_ShowCHinese(48,4,29);
    OLED_ShowCHinese(64,4,30);
    OLED_ShowCHinese(80,4,31);
    OLED_ShowChar(96,4,'>');
    OLED_ShowNum(104,4,GasConcentration,3,16);
}

void FireSensorItemShow(int flag)
{
    OLED_ShowCHinese(0,6,32); //火灾情况
    OLED_ShowCHinese(16,6,33);
    OLED_ShowCHinese(32,6,34);
    OLED_ShowCHinese(48,6,35);
    OLED_ShowChar(64,6,'>');
    switch(flag)
    {
        case 1:
        OLED_ShowCHinese(72,6,46);
        OLED_ShowCHinese(88,6,47);
        break;

        case 0:
        OLED_ShowCHinese(72,6,48);
        OLED_ShowCHinese(88,6,47);
        break;
    }
}

void AddWaterItemShow(int flag)
{
    OLED_ShowCHinese(64,6,23); //浇水
    OLED_ShowCHinese(80,6,24);
    OLED_ShowChar(96,6,'>');
    switch(flag)
    {
        case 0:
        OLED_ShowCHinese(104,6,50);
        break;

        case 1:
        OLED_ShowCHinese(104,6,49);
        break;
    }
}

void AddLightItemShow(int flag)
{
    OLED_ShowCHinese(0,6,25); //加光
    OLED_ShowCHinese(16,6,21);
    OLED_ShowChar(32,6,'>');
    switch(flag)
    {
        case 0:
        OLED_ShowCHinese(40,6,50);
        break;

        case 1:
        OLED_ShowCHinese(40,6,49);
        break;
    }
}

void WindowOperationItemShow(int flag)
{
    OLED_ShowCHinese(0,4,36); //开窗
    OLED_ShowCHinese(16,4,37);
    OLED_ShowChar(32,4,'>');
     switch(flag)
    {
        case 0:
        OLED_ShowCHinese(40,4,53);//停
        break;

        case 1:
        OLED_ShowCHinese(40,4,51);//开
        break;

        case 2:
        OLED_ShowCHinese(40,4,52);//关
        break;
    }
}

void FanOperationItemShow(int flag)
{
    OLED_ShowCHinese(64,6,38); //风扇
    OLED_ShowCHinese(80,6,39);
    OLED_ShowChar(96,6,'>');
    switch(flag)
    {
        case 0:
        OLED_ShowCHinese(104,6,52);
        break;

        case 1:
        OLED_ShowCHinese(104,6,51);
        break;
    }
}

void HumanSensorItemShow(int flag)
{
    OLED_ShowCHinese(0,4,42); //有人
    OLED_ShowCHinese(16,4,43);
    OLED_ShowChar(32,4,'>');
    switch(flag)
    {
        case 0:
        OLED_ShowCHinese(40,4,50);
        break;

        case 1:
        OLED_ShowCHinese(40,4,49);
        break;
    }
}

void RainSensorItemShow(int flag)
{
    OLED_ShowCHinese(0,4,40); //下雨
    OLED_ShowCHinese(16,4,41);
    OLED_ShowChar(32,4,'>');
     switch(flag)
    {
        case 1:
        OLED_ShowCHinese(40,4,50);
        break;

        case 0:
        OLED_ShowCHinese(40,4,49);
        break;
    }
}

void BalconyShow()//1
{
    EndDeviceAddrItemShow(dev1.ShorAddr);
    OLED_ShowCHinese(96,0,8); //阳台1
    OLED_ShowCHinese(112,0,9);
    TempetureSensorItemShow(dev1.Tempeture);
    HumitySensorItemShow(dev1.Humity);
    SoilHumidityItemShow(dev1.SoilHumidity);
    LightIlluminationItemShow(dev1.LightIllumination);
    AddWaterItemShow(dev1.AddWaterFlag);
    AddLightItemShow(dev1.AddLightFlag);
}

void KitchenShow()//2
{
    EndDeviceAddrItemShow(dev2.ShorAddr);
    OLED_ShowCHinese(96,0,10); //厨房2
    OLED_ShowCHinese(112,0,11);
    TempetureSensorItemShow(dev2.Tempeture);
    HumitySensorItemShow(dev2.Humity);
    GasSensorItemShow(dev2.GasConcentration);
    FireSensorItemShow(dev2.FireFlag);
}

void BedroomShow()//3
{
    EndDeviceAddrItemShow(dev3.ShorAddr);
    OLED_ShowCHinese(96,0,12); //卧室3
    OLED_ShowCHinese(112,0,13);
    TempetureSensorItemShow(dev3.Tempeture);
    HumitySensorItemShow(dev3.Humity);
    WindowOperationItemShow(dev3.OpenWindowFlag);
    LightIlluminationItemShow(dev3.LightIllumination);
    AddLightItemShow(dev3.AddLightFlag);
    FanOperationItemShow(dev3.OpenFanFlag);
}

void OutdoorShow()//4
{
    EndDeviceAddrItemShow(dev4.ShorAddr);
    OLED_ShowCHinese(96,0,14); //户外4
    OLED_ShowCHinese(112,0,15);
    TempetureSensorItemShow(dev4.Tempeture);
    HumitySensorItemShow(dev4.Humity);
    RainSensorItemShow(dev4.RainFlag);
    LightIlluminationItemShow(dev4.LightIllumination);
} 

void DoorShow()//5
{
    EndDeviceAddrItemShow(dev5.ShorAddr);
    OLED_ShowCHinese(96,0,16); //门外5
    OLED_ShowCHinese(112,0,15);
    TempetureSensorItemShow(dev5.Tempeture);
    HumitySensorItemShow(dev5.Humity);
    HumanSensorItemShow(dev5.HumanFlag);
}

void EndDeviceTag(int flag)
{
    switch(flag)
    {
        case 0:
        BalconyShow();
        break;

        case 1:
        KitchenShow();
        break;

        case 2:
        BedroomShow();
        break;

        case 3:
        OutdoorShow();
        break;

        case 4:
        DoorShow();
        break;  
    }
}

void BalconyOptionTag(int tag)
{
    switch(tag)
    {
        case 0:
        OLED_ShowChar(56,6,'<');
        OLED_ShowChar(120,6,' ');
        break;

        case 1:
        OLED_ShowChar(56,6,' ');
        OLED_ShowChar(120,6,'<');
        break;
    }
}

void BedroomOptionTag(int tag)
{
    switch(tag)
    {
        case 0:
        OLED_ShowChar(56,4,'<');
        OLED_ShowChar(56,6,' ');
        OLED_ShowChar(120,6,' ');
        break;

        case 1:
        OLED_ShowChar(56,4,' ');
        OLED_ShowChar(56,6,'<');
        OLED_ShowChar(120,6,' ');
        break;

        case 2:
        OLED_ShowChar(56,4,' ');
        OLED_ShowChar(56,6,' ');
        OLED_ShowChar(120,6,'<');
        break;
    }
}

void MainMenuIntoSubMenu()
{
    static int TagCouter = 0;
    static int EnterCouter = 0;
    static int OperatationCouter = 0;
    static int ValueChangeFlag = 0;
    
    if(TagCouter>4)
        TagCouter = 0;
    if(TagCouter < 0)
        TagCouter = 4;
    if(EnterCouter > 1)
        EnterCouter = 0;
    if(ValueChangeFlag> 2)
        ValueChangeFlag = 0;
    switch(KEY_Scan())
    {
        case 2 ://key2
        if(EnterCouter == 1&&(TagCouter == 0 || TagCouter == 2))
        {
            if(EnterCouter == 1&&TagCouter == 0)
            {
                switch(OperatationCouter)
                {
                    case 0:
                        BalconyControlBuf[0] = !BalconyControlBuf[0];
                        dev1.AddLightFlag = BalconyControlBuf[0];
                    break;

                    case 1:
                        BalconyControlBuf[1] = !BalconyControlBuf[1];
                        dev1.AddWaterFlag = BalconyControlBuf[1];
                    break;                    
                }
                DetectCommand(TagCouter,OperatationCouter);
            }

            if(EnterCouter == 1&&TagCouter == 2)
            {
                switch(OperatationCouter)
                {
                    case 0:
                        ++ BedroomControlBuf[0];
                        if(BedroomControlBuf[0] > 2)
                            BedroomControlBuf[0] = 0;
                        dev3.OpenWindowFlag = BedroomControlBuf[0];
                    break;

                    case 1:
                        BedroomControlBuf[1] = !BedroomControlBuf[1];
                        dev3.AddLightFlag =BedroomControlBuf[1];
                    break;     

                    case 2:
                        BedroomControlBuf[2] = !BedroomControlBuf[2];
                        dev3.OpenFanFlag = BedroomControlBuf[2];
                    break;               
                }
                DetectCommand(TagCouter,OperatationCouter);
            }

        }
        else
        {
            TagCouter--;
            OLED_Clear();
        }

        break;

        case 3 ://key3
        TagCouter ++;
        OLED_Clear();
        break;

        case 4 ://key4
        EnterCouter++;
        OLED_Clear();
        break;

        case 5 ://key1
        OperatationCouter ++;
        if(EnterCouter == 0)
        {
            SendRequestBuf[6] = !ModeCouter;
            ModeCouter = !ModeCouter;
        }
        break;
    }

    if(EnterCouter == 0)
    {
        MainMenuItemShow();
        MainMenuMoveTag(TagCouter);
        DeviceOnlineShow();
        ControlModeItemShow(ModeCouter);
    }

    if(EnterCouter == 1 )
    { 
        EndDeviceTag(TagCouter);
    }
    
      //阳台控制选项
    if(EnterCouter == 1&&TagCouter == 0)
    {
        if(OperatationCouter > 1)
            OperatationCouter = 0;
        BalconyOptionTag(OperatationCouter);
    }

    //卧室控制选项 
    if(EnterCouter == 1&&TagCouter == 2)
    {
        if(OperatationCouter > 2)
            OperatationCouter = 0;
        BedroomOptionTag(OperatationCouter);
    }    

}

/**************************************************************************************************************/
