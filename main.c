#include <ds18b20.h>

sfr ISP_CONTR = 0xE7;

sbit DQ = P2^2;
USIGN8 gucTxBuf[20];
USIGN8 gucTxLength = 0;
USIGN8 gucTxIndex = 1;
OBJ_DS18B20 gsds;

void delay(USIGN16 count)    
{
  USIGN16 i;
  while(count)
  {
    i=200;
    while(i>0)
      i--;
    count--;
  }
}

USIGN8 DS_Init(void)
{
  USIGN16 i;

  DQ = 0;
  i=103;
  while(i)
    i--;
	DQ = 1;

  i = 4;
	while(i)
	  i--; 
  delay(1);
//	if(DQ == 0)
	  return 1;     /*初始化成功*/
//  else
//	  return 0;
}


bit DS_ReadBit(void)
{
  USIGN16 i;
	bit ds_bit;

	DQ = 0;
	i++;

	DQ = 1;
	i++;
	i++;

	ds_bit = DQ;
	i = 8;
	while(i)
	  i--;

  return ds_bit;
}

void DS_WriteBit(USIGN8 b)
{
  USIGN16 i;
	if(b == 1)
	{
	  DQ = 0;
		i++;
		i++;
		DQ = 1;
		i = 7;
		while(i)
		  i--;
	}
	else if(b == 0)
	{
	  DQ = 0;
		i = 7;
		while(i)
		  i--;
		DQ = 1;
		i++;
		i++;
	}
}

USIGN8 DS_ReadByte(void)
{
  USIGN8 ucData, i, b;

  ucData = 0;
	for(i = 1; i <= 8; i++)
	{
	  b = DS_ReadBit();
		ucData = (b << 7) | (ucData >> 1);
	}
  return ucData;
}

void DS_WriteByte(USIGN8 ucData)
{
  USIGN8 i, b;

	for(i = 0; i < 8; i++)
	{
	  b = (ucData & 0x01);
	  DS_WriteBit(b);
		ucData = ucData >> 1;
	}
}

USIGN8 DS_ReadRomCmd(void)
{
  USIGN8 i;
  DS_WriteByte(READ_ROM_COMMAND);
	for(i = 0; i < 8; i++)
	{
	  ((USIGN8*)&gsds)[i] = DS_ReadByte();
	}
	return SUCCESS;
}

USIGN8 DS_MatchRomCmd(void)
{
  USIGN8 i;
  DS_WriteByte(MATCH_ROM_COMMAND);
  for(i = 0; i < 8; i++)
  {
    DS_WriteByte(((USIGN8*)&gsds)[i]);
  }
  return SUCCESS;
}

USIGN8 DS_SkipRomCmd(void)
{
  DS_WriteByte(SKIP_ROM_COMMAND);
	return SUCCESS;
}

USIGN8 DS_ProcessRomCmd(USIGN8 ucRomCmd)
{
  USIGN8 status = SUCCESS;
  switch(ucRomCmd)
	{
	case READ_ROM_COMMAND:
	  status = DS_ReadRomCmd();
		break;

  case MATCH_ROM_COMMAND:
    status = DS_MatchRomCmd();
		break;

  case SEARCH_ROM_COMMAND:
		break;

  case ALARM_SEARCH_COMMAND:
		break;

  case SKIP_ROM_COMMAND:
	  DS_SkipRomCmd();
		break;

  default:
	  status = FAILED;
		break;
	}

	return status;
}

USIGN8 DS_IsParasitePower(void)
{
  USIGN8 ucPwrMode;	
  DS_Init();
	DS_ProcessRomCmd(SKIP_ROM_COMMAND);
	ucPwrMode = DS_ProcessFuncCmd(READ_POWER_SUPPLY_COMMAND);
  if(ucPwrMode == 1)/* 返回1:表示DS18B20是外供电模式 */
	  return 0;
  else if(ucPwrMode == 0)/* 返回0:表示DS18B20是寄生供电模式 */
    return 1;
  else return 0xFF; 
}

USIGN8 DS_ConvertTempCmd(void)
{
  USIGN8 status = SUCCESS;
	DS_WriteByte(CONVERT_TEMPERATURE_COMMAND);
  if(DS_IsParasitePower())
	{
	  DQ = 1;
		delay(1);
		DQ = 0;
	}
	else
	{
	}
	return status;
}

USIGN8 DS_CopyScratchpadCmd(void)
{
  USIGN8 status = SUCCESS;
  DS_WriteByte(COPY_SCRATCHPAD_COMMAND);
  if(DS_IsParasitePower())
	{
	  DQ = 1;
		delay(1);
		DQ = 0;
	}
	else
	{
	}
	return status;
}

USIGN8 DS_ReadPwrSupplyCmd(void)
{
  //USIGN8 status = SUCCESS;
  USIGN8 ucIsParasitePowered = 0; /*'0'表示是寄生供电*/
  DS_WriteByte(READ_POWER_SUPPLY_COMMAND);
  ucIsParasitePowered = DS_ReadBit();
  return ucIsParasitePowered;
}

USIGN8 DS_RecallEECmd(void)
{
  USIGN8 i = 10;
  USIGN8 status = SUCCESS;

  DS_WriteByte(RECALL_EE_COMMAND);
  while(i--)
  {
      status = DS_ReadBit();
      if(status == 1) /*如果读回的是1，说明拷贝完成*/
        break;
  }
  return status;
}

USIGN8 DS_ReadScratchpadCmd(USIGN8 ucLength)
{
  USIGN8 status = SUCCESS;
	USIGN8 i;
					 
	if(ucLength > 9 || ucLength == 0)
	{
	  status = FAILED;
	}
	else
	{
		DS_WriteByte(READ_SCRATCHPAD_COMMAND);
		for(i = 0; i < ucLength; i++)
		{
		  ((USIGN8*)&gsds)[i+8] = DS_ReadByte();
		}
	}
	return status;
}

USIGN8 DS_WriteScratchpadCmd(OBJ_DS18B20* p)
{
  USIGN8 status = SUCCESS;
  DS_WriteByte(WRITE_SCRATCHPAD_COMMAND);
  DS_WriteByte(p->ucTH);
  DS_WriteByte(p->ucTL);
  DS_WriteByte(p->ucCfg);
  return status;
}

USIGN8 DS_ProcessFuncCmd(USIGN8 ucFuncCmd)
{
  USIGN8 status = SUCCESS;
	switch(ucFuncCmd)
	{
	case CONVERT_TEMPERATURE_COMMAND:
	  status = DS_ConvertTempCmd();
	  break;

  case COPY_SCRATCHPAD_COMMAND:
    status = DS_CopyScratchpadCmd();
	  break;

  case WRITE_SCRATCHPAD_COMMAND:
    status = DS_WriteScratchpadCmd(&gsds);
    break;

  case READ_SCRATCHPAD_COMMAND:
    status = DS_ReadScratchpadCmd(9);
    break;

  case RECALL_EE_COMMAND:
    status = DS_RecallEECmd();
    break;

  case READ_POWER_SUPPLY_COMMAND:
    status = DS_ReadPwrSupplyCmd();
    break;

	default:
	  status = FAILED;
		break;
	}
	return status;
}

void ComInit(void)
{
  TMOD = 0x20;
	TH1 = 0xFD;
	TL1 = 0xFD;
	TR1 = 1;

	SM0 = 0;
	SM1 = 1;
	REN = 1;
	ES = 1;
	EA = 1;
}

void main(void)
{
  USIGN8 i;
  ComInit();

	while(1)
	{			
    P1 = 0xaa;

    DS_Init();
    DS_ProcessRomCmd(SKIP_ROM_COMMAND);
    gsds.ucTH = 0x12;
    gsds.ucTL = 0x34;
    gsds.ucCfg = 0x7F;
    DS_ProcessFuncCmd(WRITE_SCRATCHPAD_COMMAND);

		DS_Init();
		DS_ProcessRomCmd(SKIP_ROM_COMMAND);
		DS_ProcessFuncCmd(CONVERT_TEMPERATURE_COMMAND);
		DS_Init();
		DS_ProcessRomCmd(SKIP_ROM_COMMAND);
		DS_ProcessFuncCmd(READ_SCRATCHPAD_COMMAND);
		delay(0xFF);
    for(i = 0; i < 9; i++)
      gucTxBuf[i] = ((USIGN8*)&gsds)[i+8];
	  SBUF = gucTxBuf[0];
		gucTxLength = 9;
		gucTxIndex = 1;
    delay(0xFFF); 

		DS_Init();
		DS_ProcessRomCmd(READ_ROM_COMMAND);
    for(i = 0; i < 8; i++)
      gucTxBuf[i] = ((USIGN8*)&gsds)[i];
    SBUF = gucTxBuf[0];
    gucTxLength = 8;
    gucTxIndex = 1;
    delay(0xFFF);

		if(DS_IsParasitePower()) /*如果是寄生供电模式*/
      SBUF = 0x01;            
    else
      SBUF = 0x00;
    delay(0xFFF);

	}
}

void soft_reset(void)
{
  ((void(code*)(void))0x0000)();
}

void ISP_Check(USIGN8 ucChkData)
{
  USIGN8 code ucISPCommand = 0x81;
  if(ucChkData == ucISPCommand)
  {
    EA = 0;
    delay(0xFF);
    ISP_CONTR = 0x60; /*软件复位进入ISP下载模式*/
  }
}

USIGN8 ucRxData;
void Serial(void) interrupt 4
{
  if(RI)
	{
  	ucRxData = SBUF;
    ISP_Check(ucRxData);
		RI = 0;

		SBUF = ucRxData;
		gucTxLength = 0;
		while(!TI);
	}
	if(TI)
	{
	  TI = 0;
		if(gucTxIndex != gucTxLength && gucTxLength != 0)
		{
		  SBUF = gucTxBuf[gucTxIndex++];
			while(!TI);
		}
		else
		{
		  gucTxIndex = 1;
      gucTxLength = 0;
		}
	}
}
