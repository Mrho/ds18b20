//��װĿ¼�µ�EXE�ļ��򿪺���ڵ�������ʾ��ǰ�¶�ֵ
#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit DS=P2^2;           //define interface
uint temp;             // variable of temperature

void delay(uint count)      //delay
{
  uint i;
  while(count)
  {
    i=200;
    while(i>0)
    i--;
    count--;
  }
}
///////����:���ڳ�ʼ��,������9600����ʽ1///////
void Init_Com(void)
{
     TMOD = 0x20;
     PCON = 0x00;
     SCON = 0x50;
     TH1 = 0xFd;
     TL1 = 0xFd;
     TR1 = 1;
}

void dsreset(void)       //send reset and initialization command
{
  uint i;
  DS=0;
  i=103;
  while(i>0)i--;
  DS=1;
  i=4;
  while(i>0)i--;
}

bit tmpreadbit(void)       //read a bit
{
   uint i;
   bit dat;
   DS=0;i++;          //i++ for delay
   DS=1;i++;i++;
   dat=DS;
   i=8;while(i>0)i--;
   return (dat);
}

uchar tmpread(void)   //read a byte date
{
  uchar i,j,dat;
  dat=0;
  for(i=1;i<=8;i++)
  {
    j=tmpreadbit();
    dat=(j<<7)|(dat>>1);   //�������������λ����ǰ�棬�����պ�һ���ֽ���DAT��
  }
  return(dat);
}

void tmpwritebyte(uchar dat)   //write a byte to ds18b20
{
  uint i;
  uchar j;
  bit testb;
  for(j=1;j<=8;j++)
  {
    testb=dat&0x01;
    dat=dat>>1;
    if(testb)     //write 1
    {
      DS=0;
      i++;i++;
      DS=1;
      i=8;while(i>0)i--;
    }
    else
    {
      DS=0;       //write 0
      i=8;while(i>0)i--;
      DS=1;
      i++;i++;
    }

  }
}

void tmpchange(void)  //DS18B20 begin change
{
  dsreset();
  delay(1);
  tmpwritebyte(0xcc);  // address all drivers on bus
  tmpwritebyte(0x44);  //  initiates a single temperature conversion
}

uint tmp()               //get the temperature
{
  float tt;
  uchar a,b;
  dsreset();
  delay(1);
  tmpwritebyte(0xcc);
  tmpwritebyte(0xbe);
  a=tmpread();
  b=tmpread();

  SBUF = a;
  delay(0xF);
  SBUF = b;
  delay(0x7FF);

  temp=b;
  temp<<=8;             //two byte  compose a int variable
  temp=temp|a;
  tt=temp*0.0625;
  temp=tt*10+0.5;
  return temp;
}


void main()
{
 uchar a;
 Init_Com();
  do
  {
    tmpchange();  
    for(a=10;a>0;a--)
    {   
      tmp();
    }
  }while(1);
}
  