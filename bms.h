/*
*********************************************************************************************************
*
*	模块名称 : BMS模拟模块
*	文件名称 : bms.h
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
 
#ifndef    __BMS_H
#define    __BMS_H
 
 
#ifdef     __cplusplus
extern "C" {
#endif

#include "pgn.h"
#include "PE_Types.h"
#include "ringbuffer.h"
#include "ucos_ii.h"

#define     ADDR_BMS    0xF4
#define     ADDR_ECU    0x56

#define     BIT1    0x00U
#define     BIT2    0x01U
#define     BIT3    0x02U
#define     BIT4    0x03U
#define     BIT5    0x04U
#define     BIT6    0x05U
#define     BIT7    0x06U
#define     BIT8    0x07U
     

typedef union U{
    
    uint32_t byte;
    
    struct{
        unsigned         :3;
        unsigned         P       :3;
        unsigned         R       :1;
        unsigned         DP      :1;
        unsigned         PF      :8;
        unsigned         PS      :8;
        unsigned         SA      :8;
    }bit;
    
}SAEJ1939_TypeDef;



typedef struct
{
  
    ringbuffer          rb;
    
    //uint32_t            TimeStamp;
    
}bms_CANTypeDef;


typedef struct
{
    uint32_t            *id;
    
    uint8_t             *data;
    
    uint8_t             len;
    
}bms_TxBuff;



typedef enum
{
    ST_bms_Shake = 1,
    
    ST_bms_Setvar = 2,
    
    ST_bms_Deliver = 3,
    
    ST_bms_Stop = 4,
    
}bms_StatusHandleType;



typedef struct
{
    bms_StatusHandleType    state;
    
    uint8_t                 CanName;
    
    bms_CANTypeDef          Rxframe;
    
    bms_TxBuff              Txframe;
        
    uint32_t                TimeStamp;
    
}bms_HandleTypeDef;



typedef ringbuffer_cangroup *canframe;


#define BMS_TimeInterval_ms(n)  OSTimeDlyHMSM(0,0,0,n)



/* prototype declaration */
canframe bms_read(bms_HandleTypeDef *hbms);
uint8_t *bms_getData(canframe pcf);
uint8_t bms_getPF(canframe pcf);
void bms_Task(bms_HandleTypeDef *hbms);
void bms_can_send_t(uint32_t *id,uint8_t *data,uint8_t len,uint8_t type);
uint32_t bms_Identifier_build(uint8_t P, uint8_t PF, uint8_t PS, uint8_t SA);

#ifdef      __cplusplus
}
#endif

#endif

/*************************** vccw (END OF FILE)*********************************************************/    