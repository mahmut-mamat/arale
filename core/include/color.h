#ifndef _COLOR_H_
#define _COLOR_H_

enum 
{
    BLACK = 0,                //黑  
    LIGHT_RED,                //亮红  
    LIGHT_GREEN,              //亮绿  
    LIGHT_YELLOW,             //亮黄  
    LIGHT_BLUE,               //亮蓝  
    LIGHT_PURPLE,             //亮紫  
    LIGHT_TINT_BLUE,          //浅亮蓝  
    WHITE,                    //白  
    LIGHT_GREY,               //亮灰  
    DARK_RED,                 //暗红  
    DARK_GREEN,               //暗绿  
    DARK_YELLOW,              //暗黄  
    CYAN,                     //青
    DARK_PRUPLE,              //暗紫  
    LIGHT_DARK_BLUE,          //浅暗蓝  
    DARK_GREY,                //暗灰
    COLOR_MAX
};

static unsigned char table_rgb[] = 
{
        0x00, 0x00, 0x00,   //黑  
        0xff, 0x00, 0x00,   //亮红  
        0x00, 0xff, 0x00,   //亮绿  
        0xff, 0xff, 0x00,   //亮黄  
        0x00, 0x00, 0xff,   //亮蓝  
        0xff, 0x00, 0xff,   //亮紫  
        0x00, 0xff, 0xff,   //浅亮蓝  
        0xff, 0xff, 0xff,   //白  
        0xc6, 0xc6, 0xc6,   //亮灰  
        0x84, 0x00, 0x00,   //暗红  
        0x00, 0x84, 0x00,   //暗绿  
        0x84, 0x84, 0x00,   //暗黄  
        0x00, 0x00, 0x84,   //青
        0x84, 0x00, 0x84,   //暗紫  
        0x00, 0x84, 0x84,   //浅暗蓝  
        0x84, 0x84, 0x84    //暗灰
};


#endif

