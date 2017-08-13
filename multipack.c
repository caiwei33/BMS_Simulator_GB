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
#include "bms.h"
#include "multipack.h"
#include "string.h"
#include "timer.h"


/*
*********************************************************************************************************
*	函 数 名: multipack_send
*	功能说明: 多包数据发送
*	形    参： data 发送数据
*              len 数据长度 
*	返 回 值: 无
*********************************************************************************************************
*/
void multipack_send(uint8_t *data,uint8_t len,uint8_t type)
{
    uint8_t edata[8];
    uint8_t i = 0;
    uint8_t divide;
    uint8_t remainder;
    // SAEJ1939_TypeDef Id = {0};
    // uint32_t *Id_t;
    uint32_t Id = 0;
    
    // Id.bit.P = 0x07U;
    // Id.bit.PF = 0xEBU;
    // Id.bit.PS = ADDR_ECU;
    // Id.bit.SA = ADDR_BMS;
    // Id_t = (uint32_t *)&Id;
    
    Id = bms_Identifier_build(7, 0xEBU, ADDR_ECU, ADDR_BMS);
    
    
    remainder = len % 8;
    
    if(remainder == 0)
    {
       divide = len / 8;
    }
    else
    {
        divide = len / 8;
        divide ++;
    }  
    

    for(i = 1; i <= divide; i++)
    {
        edata[0] = i;
        memcpy(&edata[1],&data[(i-1) * 7],7);
        bms_can_send_t(&Id,edata,8,type);
        CRITICAL_SECTION(10);
    }
    
}

/*
*********************************************************************************************************
*	函 数 名: multipack_read
*	功能说明: 从缓冲区读取一个can报文数据帧
*	形    参：hbms 句柄
*	返 回 值: canframe
*********************************************************************************************************
*/
uint8_t multipack_read(multipack_HandleTypeDef *hmul)
{
    canframe pcf;
    uint8_t *data;
    
    pcf = bms_read(hmul->hbms);
    
    if(pcf->id == 0)
    {
        CRITICAL_SECTION(10);
        return 0;
    }
    
    if(bms_getPF(pcf) == 0xEC)
    {
        data = bms_getData(pcf);
        
        switch(data[BIT1])
        {
            case 0x11:
            {
                /* 发送数据包 */
                return 1;
            }
            break;
            
            case 0x13:
            {
                /* 接收完毕确认 */
                return 2;
            }
            break;
            
            case 0xFF:
            {
                /* 放弃 */
                return 3;
            }
            break;
        }
        
    }
    
    return 0;

}

/*
*********************************************************************************************************
*	函 数 名: multipack_BRM_RTS
*	功能说明: BRM RTS 数据帧
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void multipack_BRM_RTS(uint8_t type)
{
    //SAEJ1939_TypeDef Id = {0};
    uint8_t data[8] = {0};
    //uint32_t *Id_t; 
    uint32_t Id = 0;
    
    // Id.bit.P = 0x07U;
    // Id.bit.PF = 0xECU;
    // Id.bit.PS = ADDR_ECU;
    // Id.bit.SA = ADDR_BMS;
    
    Id = bms_Identifier_build(7, 0xECU, ADDR_ECU, ADDR_BMS);
    
    // Id_t = (uint32_t *)&Id;
    
    data[0] = 0x10U;    /* Special ID*/
    data[1] = 0x31U;    /* 49个Bytes数据 */
    data[2] = 0x00U;
    data[3] = 0x07U;    /* 发送7个数据包 */
    data[4] = 0xFFU;    /* 没有限制 */
    data[5] = 0x00U;
    data[6] = BRM;      /* PGN */
    data[7] = 0x00U;
    
    bms_can_send_t(&Id,data,8,type);
}

/*
*********************************************************************************************************
*	函 数 名: multipack_BCP_RTS
*	功能说明: BCP RTS 数据帧
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void multipack_BCP_RTS(uint8_t type)
{
    //SAEJ1939_TypeDef Id = {0};
    uint8_t data[8] = {0};
    //uint32_t *Id_t; 
    uint32_t Id = 0;
    
    // Id.bit.P = 0x07U;
    // Id.bit.PF = 0xECU;
    // Id.bit.PS = ADDR_ECU;
    // Id.bit.SA = ADDR_BMS;
    
    Id = bms_Identifier_build(7, 0xECU, ADDR_ECU, ADDR_BMS);
    
    data[0] = 0x10U;    /* Special ID*/
    data[1] = 0x0DU;    /* 13个Bytes数据 */
    data[2] = 0x00U;
    data[3] = 0x02U;    /* 发送2个数据包 */
    data[4] = 0xFFU;    /* 没有限制 */
    data[5] = 0x00U;
    data[6] = BCP;      /* PGN */
    data[7] = 0x00U;
    
    //Id_t = (uint32_t *)&Id;
    
    bms_can_send_t(&Id,data,8,type);
    
}

/*
*********************************************************************************************************
*	函 数 名: multipack_BCS_RTS
*	功能说明: BCS RTS 数据帧
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void multipack_BCS_RTS(uint8_t type)
{
    //SAEJ1939_TypeDef Id = {0};
    uint8_t data[8] = {0};
   // uint32_t *Id_t; 
    uint32_t Id = 0;
    
    // Id.bit.P = 0x07U;
    // Id.bit.PF = 0xECU;
    // Id.bit.PS = ADDR_ECU;
    // Id.bit.SA = ADDR_BMS;
    
    Id = bms_Identifier_build(7, 0xECU, ADDR_ECU, ADDR_BMS);
        
    data[0] = 0x10U;    /* Special ID*/
    data[1] = 0x09U;    /* 9个Bytes数据 */
    data[2] = 0x00U;
    data[3] = 0x02U;    /* 发送2个数据包 */
    data[4] = 0xFFU;    /* 没有限制 */
    data[5] = 0x00U;
    data[6] = BCS;      /* PGN */
    data[7] = 0x00U;
    
    // Id_t = (uint32_t *)&Id;
    
    bms_can_send_t(&Id,data,8,type);
    
}

/*
*********************************************************************************************************
*	函 数 名: multipack_BRM_DT
*	功能说明: BRM 多包数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void multipack_BRM_DT(uint8_t type)
{
    /* 接收到CTS*/
    BRM_BufferTypeDef hbrm;
    
    uint8_t data[56];

    memset(&hbrm,0,sizeof(hbrm));
    memset(data,0xFF,sizeof(data));
    
    hbrm.brmVer[0] = 0x00U;
    hbrm.brmVer[1] = 0x01U;
    hbrm.brmVer[2] = 0x00U;
    
    hbrm.brmType = 0x03U; 
     
    hbrm.brmCapacity[0] = 0xE8U;
    hbrm.brmCapacity[1] = 0x03U;
    
    hbrm.brmVol[0] = 0x88U;
    hbrm.brmVol[1] = 0x13U;
    
    memcpy(data,(uint8_t *)&hbrm,sizeof(hbrm));
    
    multipack_send(data,sizeof(data),type);
}

/*
*********************************************************************************************************
*	函 数 名: multipack_BCP_DT
*	功能说明: BCP 多包数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void multipack_BCP_DT(uint8_t type)
{
    BCP_BufferTypeDef hbcp;
    uint8_t data[16];
    
    memset(&hbcp,0,sizeof(hbcp));
    memset(data,0xFF,sizeof(data));
    
    hbcp.bcpSinglelBatteryVol[0] = 0xF4;
    hbcp.bcpSinglelBatteryVol[1] = 0x01;
    
    //100A
    hbcp.bcpCur[0] = 0xB8;
    hbcp.bcpCur[1] = 0x0B;
    
    //100
    hbcp.bcpEnergy[0] = 0xE8;
    hbcp.bcpEnergy[1] = 0x03;
    
    //750
    hbcp.bcpVol[0] = 0x4C;
    hbcp.bcpVol[1] = 0x1D;
    
    //50
    hbcp.bcpTemp = 0x64;
    
    //70
    hbcp.bcpSoc[0] = 0xBC;
    hbcp.bcpSoc[1] = 0x02;
    
    //500
    hbcp.bcpbatVol[0] = 0x88;
    hbcp.bcpbatVol[1] = 0x13;
    
    memcpy(data,&hbcp,sizeof(hbcp));
    
    multipack_send(data,sizeof(data),type);
}


/*
*********************************************************************************************************
*	函 数 名: multipack_BCS_DT
*	功能说明: BCS 多包数据
*	形    参：无 
*	返 回 值: 无
*********************************************************************************************************
*/
void multipack_BCS_DT(uint8_t type)
{
    BCS_BufferTypeDef hbcs;
    uint8_t data[16];
    
    memset(&hbcs,0,sizeof(hbcs));
    memset(data,0xFF,sizeof(data));
    
    //500
    hbcs.bcsVol[0] = 0x88;
    hbcs.bcsVol[1] = 0x13;
    
    //100
    hbcs.bcsCur[0] = 0xE8;
    hbcs.bcsCur[1] = 0x03;
    
    //
    hbcs.bcsSpecial[0] = 0xF4;
    hbcs.bcsSpecial[1] = 0x01;
    
    //70
    hbcs.bcsSoc = 0x46;
    
    //100
    hbcs.bcsTime[0] = 0x64;
    hbcs.bcsTime[1] = 0x00;
    
    memcpy(data,&hbcs,sizeof(hbcs));
    
    multipack_send(data,sizeof(data),type);
}


/*
*********************************************************************************************************
*	函 数 名: multipack_Timeout
*	功能说明: BCS 多包数据
*	形    参：无 
*	返 回 值: 无
*********************************************************************************************************
*/
void multipack_Timeout(uint8_t type)
{
    uint8_t data[8] = {0};
    uint32_t Id = 0;
    
    Id = bms_Identifier_build(7, 0xECU, ADDR_ECU, ADDR_BMS);
    
    data[0] = 0xFF;    
    data[1] = 0xFF;    
    data[2] = 0xFF;
    data[3] = 0xFF;    
    data[4] = 0xFF;    
    data[5] = 0xFF;
    data[6] = 0xFF;      
    data[7] = 0xFF;
    
    bms_can_send_t(&Id,data,8,type);
}


/*
*********************************************************************************************************
*	函 数 名: multipack_Task
*	功能说明: 分包函数  OS系统
*	形    参：para
*	返 回 值: uint8_t
*********************************************************************************************************
*/
uint8_t multipack_Task(uint8_t para,uint8_t type,bms_HandleTypeDef *hbms)
{
    multipack_HandleTypeDef   hMP;
    
    hMP.state = TP_CM_RTS;
    
    hMP.TimeStamp = timer_GetTick();
    
    hMP.hbms = hbms;
    
    for(;;)
    {
        switch(para)
        {
            case BRM:
            {
                switch(hMP.state)
                {
                    case TP_CM_RTS:
                    {
                        multipack_BRM_RTS(type);
                    }
                    break;
                    
                    case TP_DT:
                    {
                        multipack_BRM_DT(type);
                    }
                    break;
                }
            }
            break;
            
            case BCP:
            {
                switch(hMP.state)
                {
                    case TP_CM_RTS:
                    {
                        multipack_BCP_RTS(type);
                    }
                    break;
                    
                    case TP_DT:
                    {
                        multipack_BCP_DT(type);
                    }
                    break;
                }
            }
            break;
            
            case BCS:
            {
                switch(hMP.state)
                {
                    case TP_CM_RTS:
                    {
                        multipack_BCS_RTS(type);
                    }
                    break;
                    
                    case TP_DT:
                    {
                        multipack_BCS_DT(type);
                    }
                    break;
                }
            }
            break;
        }
        
        
      loop:
      
      CRITICAL_SECTION(10);
      
      switch(multipack_read(&hMP))
      {
          case 1:  /* 发送数据 */
          {
            hMP.state = TP_DT;  
          }
          break;
          
          case 2:   /* 接收完毕 */
          {
            return 1;
          }
          break;
          
          case 3:   /* 放弃 */
          {
            return 0;
          }
          break;
          
          default:
          {
              if(timer_GetTick() - hMP.TimeStamp > 5000)
              {
                    hMP.TimeStamp = timer_GetTick();
                    
                    multipack_Timeout(type); 
                    
                    return 0; 
              }
              
              goto loop;
          }
          break;
      }
      
      
    }
}


/*************************** vccw (END OF FILE)*********************************************************/     