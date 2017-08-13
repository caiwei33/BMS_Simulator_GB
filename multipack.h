/*
*********************************************************************************************************
*
*	模块名称 : BMS模拟分包模块
*	文件名称 : multipack.h
*	版    本 : V1.0
*	说    明 :
*	修改记录 :
*		版本号   日期       作者     说明
*		v1.0    2017-07-11  vccw     首发
*                                  
*                                   
*
*	Copyright (C), 2017  vccw
*
*********************************************************************************************************
*/

#ifndef     __MULTIPACK_H
#define     __MULTIPACK_H

#ifdef      __cplusplus
extern "C"{
#endif



#define     CM          0xECU
#define     DT          0xEBU

#define     CRITICAL_SECTION(n)     OSTimeDlyHMSM(0,0,0,n)      

/* BRM  49 Bytes 缓冲区 */

typedef struct
{
    uint8_t     brmVer[3];
    
    uint8_t     brmType;
    
    uint8_t     brmCapacity[2];
    
    uint8_t     brmVol[2];
    
    uint8_t     brmfactory[4];
    
    uint8_t     brmSerial[4];
    
    uint8_t     brmYear;
    
    uint8_t     brmMonth;
    
    uint8_t     brmDate;
    
    uint8_t     brmCycle[3];
    
    uint8_t     brmRight;
    
    uint8_t     brmReserve;
    
    uint8_t     brmVIN[17];
    
    uint8_t     brmHardVer[8];
    
}BRM_BufferTypeDef;


/* BCP  13 Bytes 缓冲区 */
typedef struct
{
    uint8_t     bcpSinglelBatteryVol[2];
    
    uint8_t     bcpCur[2];
    
    uint8_t     bcpEnergy[2];
    
    uint8_t     bcpVol[2];
    
    uint8_t     bcpTemp;
    
    uint8_t     bcpSoc[2];
    
    uint8_t     bcpbatVol[2];
    
}BCP_BufferTypeDef;


/* BCS  9 Bytes 缓冲区 */
typedef struct
{
    uint8_t     bcsVol[2];
    
    uint8_t     bcsCur[2];
    
    uint8_t     bcsSpecial[2];
    
    uint8_t     bcsSoc;
    
    uint8_t     bcsTime[2];
    
}BCS_BufferTypeDef;




typedef enum
{
    TP_CM_RTS = 0x01U,      /* 请求包 */
    
    TP_CM_CTS = 0x02U,      /* 响应包 */
    
    TP_DT = 0x03U,          /* 数据包 */
    
    TP_CM_ACK = 0x04U,
    
    TP_CM_Abort = 0x05,
    
}multipack_StatusTypeDef;



typedef struct 
{
    multipack_StatusTypeDef     state;  /* 状态 */
    
    bms_HandleTypeDef           *hbms;  /* bms句柄 */
    
    uint32_t                    TimeStamp;
    
}multipack_HandleTypeDef;



uint8_t multipack_Task(uint8_t para,uint8_t type,bms_HandleTypeDef *hbms);

#ifdef      __cplusplus
}
#endif

#endif

/*************************** vccw (END OF FILE)*********************************************************/     