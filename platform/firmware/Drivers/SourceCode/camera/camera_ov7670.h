/**
 * @file camera_ov7670.h
 * @brief camera driver file
 * Copyright (C) 2010 Anyka (Guangzhou) Microelectronics Technology Co., Ltd
 * @author xia_wenting 
 * @date 2011-10-27
 * @version 1.0
 * @ref
 */
#ifndef __CAMERA_OV7670_H__
#define __CAMERA_OV7670_H__


#ifdef USE_CAMERA_OV7670

#define DELAY_FLAG        0xFE   // first parameter is 0xfe, then 2nd parameter is delay time count
#define END_FLAG          0xFF   // first parameter is 0xff, then parameter table is over 

static const T_U8 INIT_TAB[] = 
{
    //Rev 1.1
    //Input clock 24Mhz, 12.5FPS
    0x12, 0x80,
    DELAY_FLAG, 10,
    0x11, 0x81,  //0x80 to 0x81
    0x3a, 0x0c,  //0x04 to 0x08, UYVY
    0x12, 0x00,
    
    0x17, 0x13,
    0x18, 0x01,
    0x32, 0xb6,
    0x19, 0x02,
    0x1a, 0x7a,
    0x03, 0x0a,
    
    0x0c, 0x00,
    0x3e, 0x00,
    0x70, 0x3a,
    0x71, 0x35,
    0x72, 0x11,
    0x73, 0xf0,
    0xa2, 0x02,
    
    /*0x7a, 0x20,
    0x7b, 0x10,
    0x7c, 0x1e,
    0x7d, 0x35,
    0x7e, 0x5a,
    0x7f, 0x69,
    0x80, 0x76,
    0x81, 0x80,
    0x82, 0x88,
    0x83, 0x8f,
    0x84, 0x96,
    0x85, 0xa3,
    0x86, 0xaf,
    0x87, 0xc4,
    0x88, 0xd7,
    0x89, 0xe8,*/
      
    0x13, 0xe0,
    0x00, 0x00,
    0x10, 0x00,
    0x0d, 0x40, 
    0x14, 0x18,  
    0xa5, 0x07,
    0xab, 0x08,
    0x24, 0x75,///95, ///44,
    0x25, 0x64,///33, ///3c,
    0x26, 0xd3,///e3, ///82,
    0x9f, 0x78,
    0xa0, 0x68,
    0xa1, 0x03,
    0xa6, 0xd8,
    0xa7, 0xd8,
    0xa8, 0xf0,
    0xa9, 0x90,
    0xaa, 0x14,
    0x13, 0xe5,
    
    0x0e, 0x61,
    0x0f, 0x4b,
    0x16, 0x02,
    0x1e, 0x07, 
    0x21, 0x02,
    0x22, 0x91,
    0x29, 0x07,
    0x33, 0x0b, 
    0x35, 0x0b,
    0x37, 0x1d,
    0x38, 0x71,
    0x39, 0x2a, 
    0x3c, 0x78,
    0x4d, 0x40,
    0x4e, 0x20,
    0x69, 0x00, 
    0x6b, 0x0a,
    0x74, 0x10,    
    0x8d, 0x4f,
    0x8e, 0x00,
    0x8f, 0x00,
    0x90, 0x00,
    0x91, 0x00,
    0x92, 0x66,
    0x96, 0x00,
    0x9a, 0x80,
    0xb0, 0x84, 
    0xb1, 0x0c,
    0xb2, 0x0e,
    0xb3, 0x82,
    0xb8, 0x0a,
    
    0x43, 0x0a,
    0x44, 0xf0,
    0x45, 0x34,
    0x46, 0x58,
    0x47, 0x28,
    0x48, 0x3a,
    0x59, 0x88,
    0x5a, 0x88,
    0x5b, 0x44,
    0x5c, 0x67,
    0x5d, 0x49,
    0x5e, 0x0e,
    0x64, 0x04,
    0x65, 0x20,
    0x66, 0x05,
    0x94, 0x04,
    0x95, 0x08,
    0x6c, 0x0a,
    0x6d, 0x55,
    0x6e, 0x11,
    0x6f, 0x9f,  //9e for advance AWB
    
    0x6a, 0x40,
    0x01, 0x40,
    0x02, 0x40,
    0x13, 0xe7,
    
    //color matrix
    0x4f, 0x66,
    0x50, 0x66,    
    0x51, 0x00,    
    0x52, 0x22,    
    0x53, 0x5e,    
    0x54, 0x80,    
    0x58, 0x9e,
    
    0x41, 0x38,
    0x3f, 0x00,
    0x75, 0x24,  //03,
    0x76, 0xe1,
    0x4c, 0x00,
    0x77, 0x08,  //00,
    0x3d, 0xc1,  //0xc2 to 0xc1, UYVY
    0x4b, 0x09,
    0xc9, 0x60,
    0x41, 0x38,
    0x56, 0x40,
    0x55, 0x80,
   
    // LCC append lens correction setting		   
    0x62, 0x10, 
    0x63, 0x10,
    0x64, 0x08,  //green
    0x94, 0x07,  //blue
    0x95, 0x0b,  //red
    0x65, 0x00,
    0x66, 0x05,
    
    0x34, 0x11,
    0x3b, 0x1a,  //0x0a to 0x1a, auto banding filter
    0xa4, 0x88,
    0x96, 0x00,
    0x97, 0x30,
    0x98, 0x20,
    0x99, 0x30,
    0x9a, 0x84,
    0x9b, 0x29,
    0x9c, 0x03,
    0x9d, 0x98,
    0x9e, 0x3f,
    0x78, 0x04,
    
    0x79, 0x01,
    0xc8, 0xf0,
    0x79, 0x0f,
    0xc8, 0x00,
    0x79, 0x10,
    0xc8, 0x7e,
    0x79, 0x0a,
    0xc8, 0x80,
    0x79, 0x0b,
    0xc8, 0x01,
    0x79, 0x0c,
    0xc8, 0x0f,
    0x79, 0x0d,
    0xc8, 0x20,
    0x79, 0x09,
    0xc8, 0x80,
    0x79, 0x02,
    0xc8, 0xc0,
    0x79, 0x03,
    0xc8, 0x40,
    0x79, 0x05,
    0xc8, 0x30,
    0x79, 0x26,
    0x2d, 0x00,
    0x2e, 0x00,
    
    //modify by xia in 20110114
    0x7a, 0x24,
    0x7b, 0x04,
    0x7c, 0x07,
    0x7d, 0x10,
    0x7e, 0x28,
    0x7f, 0x36,
    0x80, 0x44,
    0x81, 0x52,
    0x82, 0x60,
    0x83, 0x6C,
    0x84, 0x78,
    0x85, 0x8C,
    0x86, 0x9E,
    0x87, 0xBB,
    0x88, 0xD2,
    0x89, 0xE5,
    END_FLAG, END_FLAG
};

static const T_U8 VGA_MODE_TAB[] = 
{
    //12.5fps
    0x11, 0x81,  //0x80 to 0x81
    0x3d, 0xc1,  //0xc2 to 0xc1, UYVY
    0x92, 0x66,  
    0x93, 0x00,
    0x12, 0x00,

    0x17, 0x13,
    0x18, 0x01,
    0x32, 0xb6,
    0x19, 0x02,
    0x1a, 0x7a,
    0x03, 0x0a,
    
    0x0c, 0x00,
    0x3e, 0x00,
    0x70, 0x3a,
    0x71, 0x35,
    0x72, 0x11,
    0x73, 0xf0,
    0xa2, 0x02,
    END_FLAG, END_FLAG
};

static const T_U8 CIF_MODE_TAB[] = 
{
/*    0x11, 0x81,
    0x3d, 0xc1,  //0xc2 to 0xc1, UYVY
    0x92, 0x66,
    0x93, 0x00,
    0x12, 0x00,

    0x17, 0x15,
    0x18, 0x0b,
    0x32, 0xb6,
    0x19, 0x03,  
    0x1a, 0x7a,  //0x7b to 0x7a
    0x03, 0x0a,
    
    0x0c, 0x08,
    0x3e, 0x11,
    0x70, 0x3a,
    0x71, 0x35,
    0x72, 0x11,
    0x73, 0xf1,
    0xa2, 0x02,
*/    
    END_FLAG, END_FLAG
};

static const T_U8 QVGA_MODE_TAB[] = 
{
/*
    0x11, 0x81,  //0x80 to 0x81
    0x3d, 0xc1,  
    0x92, 0x66,
    0x93, 0x00,
    0x12, 0x00,

    0x17, 0x15,
    0x18, 0x03,
    0x32, 0x36,  
    0x19, 0x02,
    0x1a, 0x7a,
    0x03, 0x0a,

    0x0c, 0x04,
    0x3e, 0x19,
    0x70, 0x3a,
    0x71, 0x35,
    0x72, 0x11,
    0x73, 0xf1,
    0xa2, 0x02,
*/    
    END_FLAG, END_FLAG
};

static const T_U8 QCIF_MODE_TAB[] = 
{
/*    0x11, 0x81,  //0x80 to 0x81
    0x3d, 0xc1,  //0xc2 to 0xc1, UYVY
    0x92, 0x66,
    0x93, 0x00,
    0x12, 0x00,

    0x17, 0x38,  
    0x18, 0x03,  
    0x32, 0x92,  
    0x19, 0x03,  
    0x1a, 0x7b,  
    0x03, 0x0b,

    0x0c, 0x0c,
    0x3e, 0x11,
    0x70, 0x3a,
    0x71, 0x35,
    0x72, 0x11,
    0x73, 0xf1,
    0xa2, 0x52,
*/    
    END_FLAG, END_FLAG
};

static const T_U8 QQVGA_MODE_TAB[] = 
{
    END_FLAG, END_FLAG
};

static const T_U8 PREV_MODE_TAB[] = 
{
    //12.5fps
    0x11, 0x81,  //0x80 to 0x81
    0x3d, 0xc1,  //0xc2 to 0xc1, UYVY
    0x92, 0x66,  
    0x93, 0x00,
    0x12, 0x00,

    0x17, 0x13,
    0x18, 0x01,
    0x32, 0xb6,
    0x19, 0x02,
    0x1a, 0x7a,
    0x03, 0x0a,
    
    0x0c, 0x00,
    0x3e, 0x00,
    0x70, 0x3a,
    0x71, 0x35,
    0x72, 0x11,
    0x73, 0xf0,
    0xa2, 0x02,

    END_FLAG, END_FLAG
};

static const T_U8 RECORD_MODE_TAB[] = 
{
    //mclk 24mhz, pclk 24mhz, 30fps
    0x11, 0x80,
    0x3d, 0xc1,  //0xc2 to 0xc1, UYVY
    0x92, 0x00,  //00,30fps //66, 25fps
    0x93, 0x00,
    0x12, 0x00,

    0x17, 0x13,
    0x18, 0x01,
    0x32, 0xb6,
    0x19, 0x02,
    0x1a, 0x7a,
    0x03, 0x0a,
    
    0x0c, 0x00,
    0x3e, 0x00,
    0x70, 0x3a,
    0x71, 0x35,
    0x72, 0x11,
    0x73, 0xf0,
    0xa2, 0x02,
   
    //Banding Filter
    0x92, 0x00,
    0x93, 0x00,
    0x9d, 0x98, //4c, 15fps;63,20fps; 98, 30fps
    0x9e, 0x7f,
    0xa5, 0x02, //06, 15fps;05, 20fps;02, 30fps
    0xab, 0x07,
    0x3b, 0x0a,
    END_FLAG, END_FLAG
};

/****************   Camera Exposure Table   ****************/
static const T_U8 EXPOSURE_WHOLE_TAB[] = 
{
    END_FLAG, END_FLAG
};

static const T_U8 EXPOSURE_CENTER_TAB[] = 
{
    END_FLAG, END_FLAG
};

static const T_U8 EXPOSURE_MIDDLE_TAB[] = 
{
    END_FLAG, END_FLAG
};

/****************   Camera Brightness Table   ****************/
static const T_U8 BRIGHTNESS_0_TAB[] = 
{
    //Brightness -3
    //0x55, 0xc0,
    END_FLAG, END_FLAG
};

static const T_U8 BRIGHTNESS_1_TAB[] = 
{
    //Brightness -2
    //0x55, 0xb0,
    END_FLAG, END_FLAG
};

static const T_U8 BRIGHTNESS_2_TAB[] = 
{
    //Brightness -1
    //0x55, 0x98,
    END_FLAG, END_FLAG
};

static const T_U8 BRIGHTNESS_3_TAB[] = 
{
    //Brightness 0
    //0x55, 0x00,
    END_FLAG, END_FLAG
};

static const T_U8 BRIGHTNESS_4_TAB[] = 
{
    //Brightness +1
    //0x55, 0x18,
    END_FLAG, END_FLAG
};

static const T_U8 BRIGHTNESS_5_TAB[] = 
{
    //Brightness +2
    //0x55, 0x30, 
    END_FLAG, END_FLAG
};

static const T_U8 BRIGHTNESS_6_TAB[] = 
{
    //Brightness +3
    //0x55, 0x40,
    END_FLAG, END_FLAG
};

/****************   Camera Contrast Table   ****************/
static const T_U8 CONTRAST_1_TAB[] = 
{
    //Contrast -3
	//0x56, 0x28,
    END_FLAG, END_FLAG
};

static const T_U8 CONTRAST_2_TAB[] = 
{
    //Contrast -2
    //0x56, 0x30,
    END_FLAG, END_FLAG
};

static const T_U8 CONTRAST_3_TAB[] = 
{
    //Contrast -1
    //0x56, 0x38,
    END_FLAG, END_FLAG
};

static const T_U8 CONTRAST_4_TAB[] = 
{
    //Contrast 0
    //0x56, 0x40,
    END_FLAG, END_FLAG
};

static const T_U8 CONTRAST_5_TAB[] = 
{
    //Contrast +1
    //0x56, 0x50,
    END_FLAG, END_FLAG
};

static const T_U8 CONTRAST_6_TAB[] = 
{
    //Contrast +2
    //0x56, 0x60,
    END_FLAG, END_FLAG
};

static const T_U8 CONTRAST_7_TAB[] = 
{
    //Contrast +3
    //0x56, 0x70,
    END_FLAG, END_FLAG
};

/****************   Camera Saturation Table   ****************/
static const T_U8 SATURATION_1_TAB[] = 
{
    /*0x4f, 0x40,        //lujie change for OV7670 @061009
    0x50, 0x40,  
    0x51, 0x00,  
    0x52, 0x11,  
    0x53, 0x2f,  
    0x54, 0x40,  
    0x58, 0x9e, */
    END_FLAG, END_FLAG
};

static const T_U8 SATURATION_2_TAB[] = 
{
    /*0x4f, 0x66,    //lujie change for OV7670 @061009
    0x50, 0x66,  
    0x51, 0x00,  
    0x52, 0x1b,  
    0x53, 0x4b,  
    0x54, 0x66,  
    0x58, 0x9e, */
    END_FLAG, END_FLAG
};

static const T_U8 SATURATION_3_TAB[] = 
{
    /*0x4f, 0x80,      //lujie change for OV7670 @061009
    0x50, 0x80,    
    0x51, 0x00,    
    0x52, 0x22,    
    0x53, 0x5e,    
    0x54, 0x80,    
    0x58, 0x9e, */ 
    END_FLAG, END_FLAG
};

static const T_U8 SATURATION_4_TAB[] = 
{
    /*0x4f, 0x99,  //lujie change for OV7670 @061009
    0x50, 0x99,  
    0x51, 0x00,  
    0x52, 0x28,  
    0x53, 0x70,  
    0x54, 0x99,  
    0x58, 0x9e,*/  
    END_FLAG, END_FLAG
};

static const T_U8 SATURATION_5_TAB[] = 
{
    /*0x4f, 0xc0 ,//lujie change for OV7670 @061009
    0x50, 0xc0,
    0x51, 0x00,
    0x52, 0x33,
    0x53, 0x8d,
    0x54, 0xc0,
    0x58, 0x9e,*/
    END_FLAG, END_FLAG
};

/****************   Camera Sharpness Table   ****************/
static const T_U8 SHARPNESS_0_TAB[] = 
{
    END_FLAG, END_FLAG
};

static const T_U8 SHARPNESS_1_TAB[] = 
{
    END_FLAG, END_FLAG
};

static const T_U8 SHARPNESS_2_TAB[] = 
{
    END_FLAG, END_FLAG
};

static const T_U8 SHARPNESS_3_TAB[] = 
{
    END_FLAG, END_FLAG
};

static const T_U8 SHARPNESS_4_TAB[] = 
{
    END_FLAG, END_FLAG
};

static const T_U8 SHARPNESS_5_TAB[] = 
{
    END_FLAG, END_FLAG
};

static const T_U8 SHARPNESS_6_TAB[] = 
{
    END_FLAG, END_FLAG
};

/****************   Camera AWB Table   ****************/
static const T_U8 AWB_AUTO_TAB[] = 
{
    //Auto
    0x13, 0xe7,  //AWB on
    0x3b, 0x1a,  //0x0a to 0x1a, auto banding filter
    0x14, 0x18, 
    0x2d, 0x00,
    0x2e, 0x00,
    END_FLAG, END_FLAG
};

static const T_U8 AWB_SUNNY_TAB[] = 
{
    //Sunny
    0x13, 0xe5,  //AWB off
    0x01, 0x5a,
    0x02, 0x5c,
    0x6a, 0x42,
    0x3b, 0x1a,  //0x0a to 0x1a, auto banding filter
    0x14, 0x08, 
    0x2d, 0x00,
    0x2e, 0x00,
    END_FLAG, END_FLAG
};

static const T_U8 AWB_CLOUDY_TAB[] = 
{
    //Cloudy
    0x13, 0xe5, //AWB off
    0x01, 0x58,
    0x02, 0x60,
    0x6a, 0x40,
    0x3b, 0x1a,  //0x0a to 0x1a, auto banding filter
    0x14, 0x18, 
    0x2d, 0x00,
    0x2e, 0x00,
    END_FLAG, END_FLAG
};

static const T_U8 AWB_OFFICE_TAB[] = 
{
    //Office
    0x13, 0xe5, //AWB off
    0x01, 0x84,
    0x02, 0x4c,
    0x6a, 0x40,
    0x3b, 0x1a,  //0x0a to 0x1a, auto banding filter
    0x14, 0x18, 
    0x2d, 0x00,
    0x2e, 0x00,
    END_FLAG, END_FLAG
};

static const T_U8 AWB_HOME_TAB[] = 
{
    //Home
    0x13, 0xe5, //AWB off
    0x01, 0x96,
    0x02, 0x40,
    0x6a, 0x4a,
    0x3b, 0x1a,  //0x0a to 0x1a, auto banding filter
    0x14, 0x18, 
    0x2d, 0x00,
    0x2e, 0x00,
    END_FLAG, END_FLAG
};

static const T_U8 AWB_NIGHT_TAB[] = 
{
    //Night
    0x13, 0xe7, //AWB on
    0x3b, 0xda,  //0xea to 0xda, 1/4 frame rate, auto banding filter
    0x14, 0x28, 
    0x2d, 0x00,
    0x2e, 0x00,
    END_FLAG, END_FLAG
};

/****************   Camera Effect Table   ****************/
static const T_U8 EFFECT_NORMAL_TAB[] = 
{
    //Normal
    0x3a, 0x08,
    0x67, 0x80,
    0x68, 0x80,
    END_FLAG, END_FLAG
};

static const T_U8 EFFECT_SEPIA_TAB[] = 
{
    0x3a, 0x18,
    0x67, 0x40,
    0x68, 0xb0,
    END_FLAG, END_FLAG
};

static const T_U8 EFFECT_ANTIQUE_TAB[] = 
{
    /* Antique */
    0x3a, 0x18,
    0x67, 0xa0,
    0x68, 0x40,
    END_FLAG, END_FLAG
};

static const T_U8 EFFECT_BLUISH_TAB[] = 
{
    /* Bluish */
    0x3a, 0x18,
    0x67, 0x80,
    0x68, 0xc0,
    END_FLAG, END_FLAG
};

static const T_U8 EFFECT_GREENISH_TAB[] = 
{
    0x3a, 0x18,
      0x67, 0x40,
      0x68, 0x40,
    END_FLAG, END_FLAG
};

static const T_U8 EFFECT_REDDISH_TAB[] = 
{
    0x3a, 0x18,
    0x67, 0xc0,
    0x68, 0x80,
    END_FLAG, END_FLAG
};

static const T_U8 EFFECT_NEGATIVE_TAB[] = 
{
    /* Negative */
    0x3a, 0x28,
      0x67, 0x80,
      0x68, 0x80,
    END_FLAG, END_FLAG
};

static const T_U8 EFFECT_BW_TAB[] = 
{
    /* Black/White */
    0x3a, 0x18,
      0x67, 0x80,
      0x68, 0x80,
    END_FLAG, END_FLAG
};

static const T_U8 EFFECT_BWN_TAB[] = 
{
    /* B/W Negative */
    0x3a, 0x38,
    0x67, 0x80,
    0x68, 0x80,
    END_FLAG, END_FLAG
};

/****************   Camera night/day mode   ****************/
static const T_U8 DAY_MODE_TAB[] = 
{
    //12.5fps
    0x13, 0xe7,  //AWB on
    0x3b, 0x0a,  //0x0a to 0x1a, auto banding filter
    0x14, 0x28,  //18, 
    0x2d, 0x00,
    0x2e, 0x00,
    END_FLAG, END_FLAG
};

static const T_U8 NIGHT_MODE_TAB[] = 
{
    //12.5~3.125fps
    0x13, 0xe7, //AWB on
    0x3b, 0xea,  //0xea to 0xda, 1/4 frame rate, auto banding filter
    0x14, 0x28, 
    0x2d, 0x00,
    0x2e, 0x00,
    END_FLAG, END_FLAG
};

#endif
#endif

