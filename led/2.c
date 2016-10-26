/*-----------------------------------------------
  名称：IO口高低电平控制
  论坛：www.doflye.net
  编写：shifang
  日期：2009.5
  修改：无
  内容：点亮P1口的1个LED灯闪烁
        该程序是单片机学习中最简单最基础的，
		通过程序了解如何控制端口的高低电平
------------------------------------------------*/
#include<reg52.h> //包含头文件，一般情况不需要改动，
//头文件包含特殊功能寄存器的定义
sfr AUXR = 0x8e;
sfr P5=0xC8;
sfr P3M1=0xB1;
sfr P3M0=0xB2;

sfr P1M1=0x91;
sfr P1M0=0x92;

sbit KEY = P3^2;

sbit T_STR = P3^7;      //锁存引脚，一般标识位STR
sbit T_IO  = P1^5;       //数据引脚，屏上标识R1或者G1
sbit T_CLK = P3^6;      //时钟引脚，标识位CLK

unsigned char MoveBitNum,MoveBitNumtemp,IncNum;//移动位数,临时移动位数，大于一个字节增加字节数
unsigned int HzNum;//汉字个数
unsigned char buff[10];

/*-----------------------------------------------
             16x16汉字取模数据
------------------------------------------------*/
unsigned char code hztest[]=    //取模选择字节倒序 ，正序得出的字是反的，可以在软件中设置自动倒叙
{

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,


0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //前面一屏大小的空字符，不显示，这里根据屏的大小修改，这个是4个汉字屏
                                                                                 //所以写入4个汉字
///*--  文字:  上  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0xC0,0x1F,0x40,0x00,
//0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0xFF,0x7F,0x00,0x00,

///*--  文字:  海  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x80,0x00,0x84,0x00,0x88,0x3F,0x48,0x00,0xA1,0x1F,0x82,0x10,0x92,0x12,0x90,0x14,
//0xE8,0x7F,0x88,0x10,0x47,0x12,0x44,0x14,0xC4,0x3F,0x04,0x10,0x04,0x0A,0x00,0x04,

///*--  文字:  朗  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x00,0x10,0x3E,0x7E,0x22,0x42,0x22,0x42,0x22,0x7E,0x3E,0x42,0x22,0x42,0x22,
//0x7E,0x22,0x02,0x3E,0x12,0x22,0x22,0x22,0x5A,0x21,0x46,0x21,0x82,0x28,0x40,0x10,

///*--  文字:  译  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0xC4,0x3F,0x88,0x20,0x08,0x11,0x00,0x0A,0x00,0x04,0x0F,0x1B,0xC8,0x64,
//0x08,0x04,0x88,0x3F,0x08,0x04,0x28,0x04,0xD8,0x7F,0x08,0x04,0x00,0x04,0x00,0x04,

///*--  文字:  电  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x80,0x00,0x80,0x00,0x80,0x00,0xFC,0x1F,0x84,0x10,0x84,0x10,0x84,0x10,0xFC,0x1F,
//0x84,0x10,0x84,0x10,0x84,0x10,0xFC,0x1F,0x84,0x50,0x80,0x40,0x80,0x40,0x00,0x7F,

///*--  文字:  子  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0xFE,0x1F,0x00,0x08,0x00,0x04,0x00,0x02,0x80,0x01,0x80,0x00,0xFF,0x7F,
//0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xA0,0x00,0x40,0x00,

///*--  文字:  科  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x10,0x08,0xB8,0x08,0x0F,0x09,0x08,0x09,0x08,0x08,0xBF,0x08,0x08,0x09,0x1C,0x09,
//0x2C,0x08,0x0A,0x78,0xCA,0x0F,0x09,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,

///*--  文字:  技  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x04,0x08,0x04,0x08,0x04,0xC8,0x7F,0x3F,0x04,0x08,0x04,0x08,0x04,0xA8,0x3F,
//0x18,0x21,0x0C,0x11,0x0B,0x12,0x08,0x0A,0x08,0x04,0x08,0x0A,0x8A,0x11,0x64,0x60,

///*--  文字:     --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

///*--  文字:     --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

///*--  文字:  德  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x02,0x08,0x02,0xF4,0x7F,0x02,0x02,0xE9,0x3F,0x28,0x25,0x24,0x25,0xE6,0x3F,
//0x05,0x00,0xF4,0x7F,0x04,0x02,0x04,0x24,0xA4,0x44,0xA4,0x50,0x94,0x10,0x04,0x1F,

///*--  文字:  飞  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0xFF,0x03,0x00,0x02,0x00,0x22,0x00,0x12,0x00,0x0A,0x00,0x06,0x00,0x0A,
//0x00,0x12,0x00,0x22,0x00,0x04,0x00,0x04,0x00,0x48,0x00,0x50,0x00,0x60,0x00,0x40,

///*--  文字:  莱  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x10,0x04,0x10,0x04,0xFF,0x7F,0x90,0x04,0x80,0x00,0xFC,0x1F,0x80,0x00,0x88,0x08,
//0x90,0x04,0xFE,0x3F,0xC0,0x01,0xA0,0x02,0x90,0x04,0x8C,0x18,0x83,0x60,0x80,0x00,

///*--  文字:  开  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0xFE,0x3F,0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x04,0xFF,0x7F,
//0x10,0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x08,0x04,0x08,0x04,0x04,0x04,0x02,0x04,

///*--  文字:  发  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x80,0x00,0x88,0x08,0x88,0x10,0x44,0x00,0xFC,0x3F,0x40,0x00,0x20,0x00,0xE0,0x1F,
//0x50,0x10,0x90,0x10,0x88,0x08,0x08,0x05,0x04,0x02,0x02,0x05,0xC0,0x18,0x38,0x60,

///*--  文字:  板  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x10,0x08,0x38,0x88,0x07,0x88,0x00,0xBF,0x00,0x88,0x3F,0x8C,0x22,0x9C,0x22,
//0xAA,0x22,0xAA,0x14,0x89,0x14,0x88,0x08,0x88,0x08,0x48,0x14,0x48,0x22,0x28,0x41,

///*--  文字:  、  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x04,0x00,0x18,0x00,0x30,0x00,0x20,0x00,0x00,0x00,0x00,0x00,

///*--  文字:  实  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x40,0x00,0x80,0x00,0xFE,0x7F,0x02,0x40,0x11,0x21,0x20,0x01,0x20,0x01,0x08,0x01,
//0x10,0x01,0x10,0x01,0xFF,0x7F,0x80,0x02,0x40,0x04,0x20,0x08,0x18,0x10,0x06,0x20,

///*--  文字:  验  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x04,0x1F,0x04,0x10,0x0A,0x12,0x0A,0x12,0x11,0x92,0x20,0x52,0x5F,0x3E,0x00,
//0x20,0x22,0x20,0x24,0xB8,0x24,0x27,0x15,0x22,0x11,0x20,0x08,0xD4,0x7F,0x08,0x00,

///*--  文字:  仪  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x10,0x01,0x10,0x12,0x50,0x12,0x48,0x10,0x48,0x10,0x8C,0x08,0x8C,0x08,0x8A,0x08,
//0x09,0x05,0x08,0x05,0x08,0x02,0x08,0x02,0x08,0x05,0x88,0x08,0x48,0x10,0x28,0x60,

///*--  文字:  器  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x7C,0x3E,0x44,0x22,0x44,0x22,0x7C,0x3E,0x80,0x04,0x80,0x08,0xFF,0x7F,
//0x40,0x01,0x30,0x06,0x0C,0x18,0x03,0x60,0x7C,0x3E,0x44,0x22,0x44,0x22,0x7C,0x3E,

///*--  文字:  、  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x04,0x00,0x18,0x00,0x30,0x00,0x20,0x00,0x00,0x00,0x00,0x00,

///*--  文字:  元  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0xFC,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x20,0x02,0x20,0x02,
//0x20,0x02,0x20,0x02,0x10,0x02,0x10,0x42,0x08,0x42,0x04,0x42,0x02,0x7C,0x01,0x00,

///*--  文字:  器  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x7C,0x3E,0x44,0x22,0x44,0x22,0x7C,0x3E,0x80,0x04,0x80,0x08,0xFF,0x7F,
//0x40,0x01,0x30,0x06,0x0C,0x18,0x03,0x60,0x7C,0x3E,0x44,0x22,0x44,0x22,0x7C,0x3E,

///*--  文字:  件  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x10,0x04,0x10,0x04,0x90,0x04,0x88,0x04,0x88,0x3F,0x4C,0x04,0x4C,0x04,0x2A,0x04,
//0x09,0x04,0xE8,0x7F,0x08,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x08,0x04,0x08,0x04,

///*--  文字:  ，  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x0C,0x00,0x08,0x00,0x04,0x00,0x00,0x00,

///*--  文字:  更  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0xFF,0x7F,0x80,0x00,0x80,0x00,0xFC,0x1F,0x84,0x10,0x84,0x10,0xFC,0x1F,
//0x84,0x10,0x84,0x10,0xFC,0x1F,0x88,0x00,0x50,0x00,0x60,0x00,0x98,0x03,0x07,0x7C,

///*--  文字:  多  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x40,0x00,0x40,0x00,0xE0,0x0F,0x10,0x04,0x1C,0x02,0x20,0x01,0xC0,0x02,0x30,0x01,
//0x8E,0x1F,0x40,0x10,0x30,0x08,0x4C,0x04,0x80,0x02,0x80,0x01,0x70,0x00,0x0E,0x00,

///*--  文字:  信  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x10,0x02,0x10,0x04,0xD0,0x7F,0x08,0x00,0x08,0x00,0x8C,0x3F,0x0C,0x00,0x0A,0x00,
//0x89,0x3F,0x08,0x00,0x08,0x00,0x88,0x3F,0x88,0x20,0x88,0x20,0x88,0x3F,0x88,0x20,

///*--  文字:  息  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x80,0x00,0x40,0x00,0xF8,0x0F,0x08,0x08,0xF8,0x0F,0x08,0x08,0xF8,0x0F,0x08,0x08,
//0xF8,0x0F,0x08,0x08,0x80,0x00,0x10,0x21,0x12,0x49,0x12,0x48,0xE1,0x0F,0x00,0x00,

///*--  文字:  请  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x02,0x02,0x02,0xE4,0x3F,0x04,0x02,0xC0,0x1F,0x00,0x02,0xE7,0x7F,0x04,0x00,
//0xC4,0x1F,0x44,0x10,0xC4,0x1F,0x44,0x10,0xD4,0x1F,0x4C,0x10,0x44,0x14,0x40,0x08,

///*--  文字:  搜  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x05,0x88,0x34,0x88,0x24,0x88,0x24,0xBF,0x35,0x88,0x24,0x88,0x24,0xA8,0x3F,
//0x18,0x04,0x8C,0x3F,0x0B,0x11,0x08,0x0A,0x08,0x04,0x08,0x0A,0x0A,0x11,0xC4,0x60,

///*--  文字:  索  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x80,0x00,0x80,0x00,0xFC,0x1F,0x80,0x00,0x80,0x00,0xFE,0x7F,0x42,0x40,0x21,0x24,
//0xF8,0x03,0x80,0x01,0x60,0x08,0xFC,0x1F,0x80,0x10,0x88,0x04,0xA4,0x08,0x42,0x10,

///*--  文字:  “  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x00,0x11,0x80,0x08,0x40,0x04,0xC0,0x0C,0xC0,0x0C,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

///*--  文字:  德  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x02,0x08,0x02,0xF4,0x7F,0x02,0x02,0xE9,0x3F,0x28,0x25,0x24,0x25,0xE6,0x3F,
//0x05,0x00,0xF4,0x7F,0x04,0x02,0x04,0x24,0xA4,0x44,0xA4,0x50,0x94,0x10,0x04,0x1F,

///*--  文字:  飞  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0xFF,0x03,0x00,0x02,0x00,0x22,0x00,0x12,0x00,0x0A,0x00,0x06,0x00,0x0A,
//0x00,0x12,0x00,0x22,0x00,0x04,0x00,0x04,0x00,0x48,0x00,0x50,0x00,0x60,0x00,0x40,

///*--  文字:  莱  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x10,0x04,0x10,0x04,0xFF,0x7F,0x90,0x04,0x80,0x00,0xFC,0x1F,0x80,0x00,0x88,0x08,
//0x90,0x04,0xFE,0x3F,0xC0,0x01,0xA0,0x02,0x90,0x04,0x8C,0x18,0x83,0x60,0x80,0x00,

///*--  文字:  ”  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x98,0x01,0x98,0x01,0x10,0x01,0x88,0x00,0x44,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

///*--  文字:  登  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x01,0x7C,0x09,0x40,0x25,0x24,0x12,0x28,0x0A,0x10,0x04,0xE8,0x0B,0x04,0x10,
//0xFB,0x6F,0x08,0x08,0x08,0x08,0xF8,0x0F,0x10,0x04,0x20,0x02,0xFE,0x3F,0x00,0x00,

///*--  文字:  录  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0xFC,0x0F,0x00,0x08,0x00,0x08,0xF8,0x0F,0x00,0x08,0x00,0x08,0xFF,0x7F,
//0x80,0x00,0x84,0x10,0x88,0x09,0xA0,0x06,0x90,0x04,0x8C,0x18,0xA3,0x60,0x40,0x00,

///*--  文字:  单  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x08,0x10,0x04,0x20,0x02,0xFC,0x1F,0x84,0x10,0x84,0x10,0xFC,0x1F,0x84,0x10,
//0x84,0x10,0xFC,0x1F,0x80,0x00,0x80,0x00,0xFF,0x7F,0x80,0x00,0x80,0x00,0x80,0x00,

///*--  文字:  片  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0x08,0x02,0xF8,0x3F,0x08,0x00,0x08,0x00,
//0x08,0x00,0xF8,0x07,0x08,0x04,0x08,0x04,0x08,0x04,0x04,0x04,0x04,0x04,0x02,0x04,

///*--  文字:  机  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x00,0x88,0x0F,0x88,0x08,0x88,0x08,0xBF,0x08,0x88,0x08,0x8C,0x08,0x9C,0x08,
//0xAA,0x08,0xAA,0x08,0x89,0x08,0x88,0x48,0x88,0x48,0x48,0x48,0x48,0x70,0x28,0x00,

///*--  文字:  爱  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x10,0x80,0x3F,0x7E,0x08,0x44,0x08,0x88,0x04,0xFE,0x7F,0x42,0x40,0x41,0x20,
//0xFE,0x1F,0x20,0x00,0xE0,0x0F,0x50,0x08,0x88,0x04,0x04,0x03,0xC2,0x0C,0x38,0x70,

///*--  文字:  好  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x00,0x08,0x3F,0x08,0x20,0x08,0x10,0x3F,0x08,0x24,0x04,0x24,0x04,0xA4,0x7F,
//0x24,0x04,0x12,0x04,0x14,0x04,0x08,0x04,0x14,0x04,0x22,0x04,0x21,0x05,0x00,0x02,

///*--  文字:  者  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x40,0x00,0x40,0x10,0xFC,0x0B,0x40,0x04,0x40,0x02,0xFF,0x7F,0x80,0x00,0x40,0x00,
//0xF0,0x0F,0x18,0x08,0x14,0x08,0xF2,0x0F,0x11,0x08,0x10,0x08,0xF0,0x0F,0x10,0x08,

///*--  文字:  论  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x02,0x04,0x02,0x08,0x05,0x08,0x05,0x80,0x08,0x40,0x10,0x2F,0x60,0x88,0x08,
//0x88,0x04,0x88,0x02,0x88,0x01,0x88,0x00,0xA8,0x20,0x98,0x20,0x08,0x3F,0x00,0x00,

///*--  文字:  坛  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x08,0x00,0x08,0x00,0x88,0x3F,0x08,0x00,0x08,0x00,0x3F,0x00,0xC8,0x7F,0x08,0x04,
//0x08,0x04,0x08,0x02,0x08,0x02,0x38,0x11,0x87,0x20,0xC2,0x7F,0x80,0x40,0x00,0x00,

///*--  文字:  。  --*/
///*--  新宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x18,0x00,0x24,0x00,0x24,0x00,0x18,0x00,0x00,0x00,0x00,0x00,
/*---×a??×?·? ?? ---*/
0x80,0x00,0x9F,0x10,0x90,0x10,0x90,0x08,0x90,0x04,0x9E,0x02,0x82,0x00,0xE2,0x7F,
0x82,0x02,0x9E,0x04,0x90,0x04,0x90,0x08,0x90,0x10,0x90,0x22,0x8A,0x41,0x84,0x00,


/*---×a??×?·? 3? ---*/
0x00,0x02,0x02,0x02,0x04,0x02,0x04,0x02,0xE0,0x3F,0x20,0x22,0x28,0x22,0x28,0x22,
0x24,0x22,0x27,0x22,0xE4,0x3F,0x24,0x22,0x04,0x02,0x04,0x02,0x04,0x02,0x00,0x02,


/*---×a??×?·? ￡? ---*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x0C,0x00,0x08,0x00,0x04,0x00,0x00,0x00,


/*---×a??×?·? è? ---*/
0x00,0x00,0xF8,0x0F,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0xF8,0x0F,
0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0xF8,0x0F,0x08,0x08,


/*---×a??×?·? ?? ---*/
0x10,0x01,0x10,0x01,0x10,0x01,0x88,0x7F,0x88,0x40,0x4C,0x20,0x2C,0x04,0x0A,0x04,
0x89,0x14,0x88,0x24,0x48,0x24,0x48,0x44,0x28,0x44,0x08,0x04,0x08,0x05,0x08,0x02,


/*---×a??×?·? ′ó ---*/
0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xFF,0x7F,0x80,0x00,0x80,0x00,
0x40,0x01,0x40,0x01,0x20,0x02,0x20,0x02,0x10,0x04,0x08,0x08,0x04,0x10,0x03,0x60,


/*---×a??×?·? òˉ ---*/
0x20,0x02,0x10,0x04,0x18,0x0A,0x24,0x11,0xC0,0x00,0x30,0x03,0x0C,0x0C,0x03,0x70,
0xF8,0x0F,0x40,0x08,0x40,0x08,0x40,0x08,0x40,0x0A,0x40,0x04,0x40,0x00,0x40,0x00,

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //尾部的一屏的空字符，用于清屏，参数和屏大小有关。
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,


0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

};

/*-----------------------------------------------
             向595写入一个字节
------------------------------------------------*/
void InputByte(unsigned  char dat) //写一个字节
{ 
	unsigned char i;       
	for(i=0;i<8;i++)
    {     
		T_IO=!(dat&0x01);
        dat>>=1;
        T_CLK=0;
        T_CLK=1;
    }
}
/*-----------------------------------------------
                    延时程序
------------------------------------------------*/
void Delay(unsigned int t)
{
 while(--t);
}


/*------------------------------------------------
                    主函数
------------------------------------------------*/
void main (void)
{
	unsigned char count;//16行扫描数据，范围0-15
	unsigned int i,  j;
	unsigned char temp;
	P3M0=0xff;//推挽
	P1M0=0xff;
	P3M1=0;
	P1M1=0;

	AUXR |= 0x80;                 	//1T
	//AUXR &= 0x7f;                	//12T

	while (1)         //主循环
	{
		i++;
		if(i==100)//更改流动速度,1T单片机和12T单片机速度大约5-8倍，注意更改参数
		{
			i=0;
			MoveBitNum++;
			if(MoveBitNum==16)//每次移动完一个汉字大小后循环
			{
				MoveBitNum=0;
				HzNum+=32;    //调用下一个汉字
				if(HzNum>=11*32)//需要显示的汉字个数乘以32，包括前面的一屏空字符的个数，后面清屏的空字符不包含在内,这里是(汉字个数+4)x32
					HzNum=0;   //完全显示完后循环调用
			}
		}
		Delay(1);//控制扫描频率
		for(j=0;j<5;j++) //取每个汉字的前2个字节，4个汉字组成一行，数据传输完成后锁存输出即可，这里需要多取出一个汉字作为缓冲
		{                //原来j=4，现在j=5.
			buff[2*j]=hztest[HzNum+j*32+count*2];  //每次移动完一个汉字后，选择下一个汉字
			buff[2*j+1]=hztest[HzNum+j*32+count*2+1];
		}
		if(MoveBitNum<8)                   //  判读移动距离是大于一个字节还是小于一个字节，因为一个字节左移右移最大只能8位
		{ 
			IncNum=0; MoveBitNumtemp=MoveBitNum; 
		}
		else
		{ 
			IncNum=1; MoveBitNumtemp=MoveBitNum-8;
		}//大于8就减去8得到的数值还是小于8
		T_STR=0;       //锁存无效
		for(j=0;j<8;j++)          //按bit的方式移动缓冲区的内容，然后输出到595，即取出的数值每个字节左移一定的位数，
		{                                     //后面左移出的数据整合到前面的字节中，保持数据的连续性	     
			temp=(buff[j+IncNum]>>MoveBitNumtemp)|(buff[j+1+IncNum]<<(8-MoveBitNumtemp));//这句比较重要，需要自行拿出2个字节的数据模拟分析
			InputByte(temp);//输出到595
		}//8个字节传输完锁存输出
		P1=count;//用P0口的前4位控制16行，屏内部通过4-16译码器工作，循环扫描16行
		T_STR=1;      //锁存有效，此时一行的数据显示到屏上
		count++;
		if(count==16)
			count=0;
	}
}
