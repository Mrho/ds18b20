
#ifndef DS18B20_DEFS
#define DS18B20_DEFS

#include <reg52.h>
 
/* DS18B20 ROM Command Number */
#define READ_ROM_COMMAND            0x33
#define MATCH_ROM_COMMAND           0x55
#define SEARCH_ROM_COMMAND          0xF0
#define ALARM_SEARCH_COMMAND        0xEC
#define SKIP_ROM_COMMAND            0xCC

/* DS18B20 Function Command Number */
#define CONVERT_TEMPERATURE_COMMAND 0x44
#define COPY_SCRATCHPAD_COMMAND     0x48
#define WRITE_SCRATCHPAD_COMMAND    0x4E
#define READ_SCRATCHPAD_COMMAND     0xBE
#define RECALL_EE_COMMAND           0xB8
#define READ_POWER_SUPPLY_COMMAND   0XB4

#define SUCCESS    1
#define FAILED     0

typedef unsigned char USIGN8;
typedef unsigned int  USIGN16;
typedef struct DS18B20
{
  /* ROM Information */
  USIGN8 ucFCode;
  USIGN8 ucSN[6];
  USIGN8 ucRomCrc;

  /* Scratchpad */
  USIGN16 ucTemp;
  USIGN8  ucTH;
  USIGN8  ucTL;
  USIGN8  ucCfg;
  USIGN8  ucReserved[3];
  USIGN8  ucMemCrc;
}OBJ_DS18B20;

void delay(USIGN16 count);
USIGN8 DS_Init(void);
bit    DS_ReadBit(void);
void   DS_WriteBit(USIGN8 b);
USIGN8 DS_ReadByte(void);
void   DS_WriteByte(USIGN8 ucData);
USIGN8 DS_ReadRomCmd(void);
USIGN8 DS_MatchRomCmd(void);
USIGN8 DS_SkipRomCmd(void);
USIGN8 DS_ProcessRomCmd(USIGN8 ucRomCmd);
USIGN8 DS_IsParasitePower(void);
USIGN8 DS_ConvertTempCmd(void);  
USIGN8 DS_CopyScratchpadCmd(void);
USIGN8 DS_ReadPwrSupplyCmd(void); 
USIGN8 DS_RecallEECmd(void);
USIGN8 DS_ReadScratchpadCmd(USIGN8 ucLength); 
USIGN8 DS_WriteScratchpadCmd(OBJ_DS18B20* p);
USIGN8 DS_ProcessFuncCmd(USIGN8 ucFuncCmd);
void ComInit(void);
void soft_reset(void); 
void ISP_Check(USIGN8 ucChkData);







#endif