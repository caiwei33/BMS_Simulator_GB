/*
*********************************************************************************************************
*
*	模块名称 : BMS模拟模块
*	文件名称 : bms.c
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

#include "bms.h"
#include "ringbuffer.h"
#include "multipack.h"
#include "can.h"
#include "timer.h"
 
static uint16_t req_vol;
static uint16_t req_cur;

ringbuffer_cangroup s;
    
static uint8_t (*send[])(uint8_t ID_type, uint32_t ID, uint8_t RTR, uint8_t *data, uint8_t length) 
 = {
    CAN1_TxMsg,CAN1_TxMsg,CAN2_TxMsg
};

 /*
*********************************************************************************************************
*	函 数 名: bms_can_send
*	功能说明: 发送CAN报文
*	形    参：id CAN id
*             data 发送数据 
*	返 回 值: 无
*********************************************************************************************************
*/
void bms_can_send(bms_HandleTypeDef *hbms)
{
    
    send[hbms->CanName](1,*(hbms->Txframe.id),0,hbms->Txframe.data,hbms->Txframe.len);
    
}

 /*
*********************************************************************************************************
*	函 数 名: bms_can_send
*	功能说明: 发送CAN报文
*	形    参：id CAN id
*             data 发送数据 
*	返 回 值: 无
*********************************************************************************************************
*/
void bms_can_send_t(uint32_t *id,uint8_t *data,uint8_t len,uint8_t type)
{
    send[type](1,*id,0,data,len);   
}


 /*
*********************************************************************************************************
*	函 数 名: bms_Identifier_build
*	功能说明: 组成标识符
*	形    参：P 优先级
*             PF PF值
*             PS 目标地址
*             SA 源地址
*	返 回 值: ID 标识符
*********************************************************************************************************
*/
uint32_t bms_Identifier_build(uint8_t P, uint8_t PF, uint8_t PS, uint8_t SA)
{
    uint32_t id = 0;
    
    id |= ((uint32_t)P  << 26);
    id |= ((uint32_t)PF << 16);
    id |= ((uint32_t)PS << 8);
    id |= (uint32_t)SA;    
    
    return id;
}

/*
*********************************************************************************************************
*	函 数 名: bms_read
*	功能说明: 从缓冲区读取一个can报文数据帧
*	形    参：hbms 句柄
*	返 回 值: canframe
*********************************************************************************************************
*/
canframe bms_read(bms_HandleTypeDef *hbms)
{
    canframe cf;
    
    s = ringbuffer_read(&(hbms->Rxframe.rb));
    
    cf = &s;
    
    return cf;
}

/*
*********************************************************************************************************
*	函 数 名: bms_getPF
*	功能说明: 报文握手阶段
*	形    参：hbms 句柄
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bms_getPF(canframe pcf)
{
    uint8_t ps = 0;
    uint8_t pf = 0;
    
    ps = (*(pcf->id) & 0x0000FF00) >> 8;
    pf = (*(pcf->id) & 0x00FF0000) >> 16;

    
    if(ps == ADDR_BMS)
        return pf;
    else
        return 0;
}


/*
*********************************************************************************************************
*	函 数 名: bms_getPF_bitfield
*	功能说明: 报文握手阶段
*	形    参：hbms 句柄
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bms_getPF_bitfield(canframe pcf)
{
    SAEJ1939_TypeDef *id = (SAEJ1939_TypeDef *)pcf->id;
    
    if(id->bit.PS == ADDR_BMS)
        return id->bit.PF;
    else
        return 0;
}

/*
*********************************************************************************************************
*	函 数 名: bms_getData
*	功能说明: 获取can报文数据
*	形    参：hbms 句柄
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t *bms_getData(canframe pcf)
{
    return pcf->data;
}

/*
*********************************************************************************************************
*	函 数 名: bms_BRO
*	功能说明: 发送BRO数据
*	形    参：hbms 句柄
*	返 回 值: 无
*********************************************************************************************************
*/
void bms_BRO(bms_HandleTypeDef *hbms)
{
    uint32_t Id = 0;
    //SAEJ1939_TypeDef id = {0};
    uint8_t data[8] = {0};
    
    // Id.bit.P = 0x04U;
    // Id.bit.PF = BRO;
    // Id.bit.PS = ADDR_ECU;
    // Id.bit.SA = ADDR_BMS;
    
    Id = bms_Identifier_build(4,BRO,ADDR_ECU,ADDR_BMS);
    
    data[0] = 0xAA;    /* Special ID*/

    hbms->Txframe.id = &Id;
    hbms->Txframe.data = data;
    hbms->Txframe.len = 1;
    
    bms_can_send(hbms);
    
}


/*
*********************************************************************************************************
*	函 数 名: bms_BCL
*	功能说明: 发送BCL数据
*	形    参：hbms 句柄
*	返 回 值: 无
*********************************************************************************************************
*/
void bms_BCL(bms_HandleTypeDef *hbms)
{
    //SAEJ1939_TypeDef Id = {0};
    uint32_t Id = 0;
    uint8_t data[8] = {0};
    
    // Id.bit.P = 0x06U;
    // Id.bit.PF = BCL;
    // Id.bit.PS = ADDR_ECU;
    // Id.bit.SA = ADDR_BMS;
    
    Id = bms_Identifier_build(6,BCL,ADDR_ECU,ADDR_BMS);
    
    data[0] = (uint8_t)(req_vol & 0xFF) ;
    data[1] = (uint8_t)((req_vol >> 8 ) & 0xFF);
    data[2] = (uint8_t)(req_cur & 0xFF);
    data[3] = (uint8_t)((req_cur >> 8) & 0xFF);
    data[4] = 0x01;

    hbms->Txframe.id = &Id;
    hbms->Txframe.data = data;
    hbms->Txframe.len = 5;

    
    bms_can_send(hbms);
}

/*
*********************************************************************************************************
*	函 数 名: bms_BST
*	功能说明: 发送BST数据
*	形    参：hbms 句柄
*	返 回 值: 无
*********************************************************************************************************
*/
void bms_BST(bms_HandleTypeDef *hbms)
{
    //SAEJ1939_TypeDef Id = {0};
    uint8_t data[8] = {0};
    uint32_t Id = 0;
    
    // Id.bit.P = 0x04U;
    // Id.bit.PF = BST;
    // Id.bit.PS = ADDR_ECU;
    // Id.bit.SA = ADDR_BMS;
    
    Id = bms_Identifier_build(4,BST,ADDR_ECU,ADDR_BMS);
    
    data[0] = 0x64;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    
    hbms->Txframe.id = &Id;
    hbms->Txframe.data = data;
    hbms->Txframe.len = 4;
    
    bms_can_send(hbms);
}

/*
*********************************************************************************************************
*	函 数 名: bms_SetVol
*	功能说明: 设置需求电压
*	形    参：vol 电压
*             
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bms_SetVol(uint16_t vol)
{
    if(vol < 2000)
        return 0;
    else
    {
        req_vol = vol;
        return 1;
    }
}


/*
*********************************************************************************************************
*	函 数 名: bms_SetCur
*	功能说明: 设置需求电流
*	形    参：hbms 句柄
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bms_SetCur(uint16_t cur)
{
    if(4000 - cur < 3000)
        return 0;
    else
    {
        req_cur = 4000 - cur;
        return 1;
    }
}


/*
*********************************************************************************************************
*	函 数 名: bms_Shank
*	功能说明: 报文握手阶段
*	形    参：hbms 句柄
*	返 回 值: uint8_t
*********************************************************************************************************
*/
void bms_Timeout(bms_HandleTypeDef *hbms)
{
    if(timer_GetTick() - (hbms->TimeStamp) >= 5000)
    {
        hbms->TimeStamp = timer_GetTick();
        hbms->state = ST_bms_Shake;
    }
}

/*
*********************************************************************************************************
*	函 数 名: bms_Shank
*	功能说明: 报文握手阶段
*	形    参：hbms 句柄
*	返 回 值: uint8_t
*********************************************************************************************************
*/
uint8_t bms_Shake(bms_HandleTypeDef *hbms)
{
    canframe pcf;
    
    pcf = bms_read(hbms);
    
    if(pcf->id == 0)
    {
        BMS_TimeInterval_ms(200);
        return 0;
    }
        
    #if 0
    hbms->Txframe.id = pcf->id;
    hbms->Txframe.data = pcf->data;
    hbms->Txframe.len = 8;
    bms_can_send(hbms);
    #endif
    
    /* 收到充电机握手报文 */
    switch(bms_getPF(pcf))
    //switch(bms_getPF_bitfield(pcf))
    {
        case CHM:
        {
            /* 准备发送BMS握手报文 */
        }
        break;
        
        case CRM:
        {
            switch(pcf->data[BIT1])
            {
                case 0x00:
                {
                    /* BRM 分包 */
                    if(multipack_Task(BRM,hbms->CanName,hbms) == 0)
                    {
                        return 0;
                    }
                }
                break;
                
                case 0xAA:
                {
                    /* 分包发送完毕 */ /* BCP */
                    if(multipack_Task(BCP,hbms->CanName,hbms) == 0)
                        return 0;
                    
                    hbms->state = ST_bms_Setvar;  // 参数配置
                    hbms->TimeStamp = timer_GetTick();
                    return 1;
                }
                break;
            }
        }
        break;
        
        case CST:
        {
            
        }
        break;
    }
    
    BMS_TimeInterval_ms(200);
    
    return 0;
}


/*
*********************************************************************************************************
*	函 数 名: bms_Setvar
*	功能说明: bms参数设置
*	形    参：hbms句柄
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bms_Setvar(bms_HandleTypeDef *hbms)
{
    canframe pcf;
    
    pcf = bms_read(hbms);
    
    if(pcf->id == 0)
    {
        bms_Timeout(hbms);
        BMS_TimeInterval_ms(200);
        return 0;
    }
    

    /* 收到充电机握手报文 */
    switch(bms_getPF(pcf))
    {
        case CML:
        {
            /* BRO */
            bms_BRO(hbms);
        }
        break;
        
        
        case CRO:
        {
            switch(pcf->data[BIT1])
            {
                case 0x00:
                {
                    /* 发送BRO */
                    bms_BRO(hbms);
                }
                break;
                
                case 0xAA:
                {
                    /* 充电桩准备OK 准备进入充电状态 */
                    hbms->state = ST_bms_Deliver;  // 参数配置
                    hbms->TimeStamp = timer_GetTick();
                    return 1;
                }
                break;
            }
        }
        break;
        
        case CST:
        {
            
        }
        break;
        
    }
    
    bms_Timeout(hbms);
    
    BMS_TimeInterval_ms(200);
    
    return 0;
}

/*
*********************************************************************************************************
*	函 数 名: bms_Deliver
*	功能说明: bms电能传输
*	形    参：hbms句柄
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bms_Deliver(bms_HandleTypeDef *hbms)
{
    canframe pcf;
    
    pcf = bms_read(hbms);
    
    if(pcf->id == 0)
    {
        BMS_TimeInterval_ms(200);
        return 0;
    }
    
    /* 收到充电机握手报文 */
    switch(bms_getPF(pcf))
    {
        case CCS:
        {
            /* BCL BCS */
        }
        break;
        
        case CST:
        {
            
        }
        break;
        
    }
    
    multipack_Task(BCS,hbms->CanName,hbms);
    
    BMS_TimeInterval_ms(200);
    
    bms_BCL(hbms);
    
    BMS_TimeInterval_ms(200);
}


/*
*********************************************************************************************************
*	函 数 名: bms_Stop
*	功能说明: bms停止结束
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
uint8_t bms_Stop(bms_HandleTypeDef *hbms)
{
    canframe pcf;
    
    pcf = bms_read(hbms);
    
    if(pcf->id == 0)
    {
        BMS_TimeInterval_ms(200);
        return 0;
    }
    
    /* 收到充电机握手报文 */
    switch(bms_getPF(pcf))
    {
        case 1:
        {

        }
        break;
    }
    
    BMS_TimeInterval_ms(200);
}


/*
*********************************************************************************************************
*	函 数 名: bms_Error
*	功能说明: bms错误
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
uint8_t bms_Error(bms_HandleTypeDef *hbms)
{
    return 0;
}


static uint8_t  (*pFunc[])(bms_HandleTypeDef *) = {
    
    bms_Shake,bms_Shake, bms_Setvar, bms_Deliver, bms_Stop, bms_Error
    
};


/*
*********************************************************************************************************
*	函 数 名: bms_Task
*	功能说明: bms主函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/ 
void bms_Task(bms_HandleTypeDef *hbms)
{

   uint8_t res = 0;
   
   ringbuffer_Init(&(hbms->Rxframe.rb));
   
   hbms->state = ST_bms_Shake;
   hbms->TimeStamp = timer_GetTick();
   bms_SetVol(5000);
   bms_SetCur(1000);
   
   for(;;)
   {
      res = pFunc[hbms->state](hbms);
      
      if(res == 1)
      {
          
      }
      else if(res == 1)
      {
          
      }
   }
    

}
 
 
/*************************** vccw(END OF FILE)**********************************************************/