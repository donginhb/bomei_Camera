/**
 * @FILENAME: camera_ov9665.c
 * @BRIEF camera driver file
 * Copyright (C) 2008 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @AUTHOR 
 * @DATE 2008-07-28
 * @VERSION 1.0
 * @REF
 */ 
#include "akdefine.h"
#include "platform_hd_config.h"
#include "drv_api.h"
#include "drv_gpio.h"
#include "drv_camera.h"
#include "gpio_config.h"
#include "drv_sccb.h"
#include "camera_ov9665.h"



#ifdef USE_CAMERA_OV9665

#define CAM_EN_LEVEL		0	
#define CAM_RESET_LEVEL		0

#define CAMERA_SCCB_ADDR        0x60
#define CAMERA_OV9665_ID        0x9663

#if (defined(CHIP_AK7801) || defined(CHIP_AK7802))
	#define CAMERA_MCLK_DIV         3              //192Mhz/(2*(3+1))=24Mhz
#elif defined(CHIP_AK322L)
	#define CAMERA_MCLK_DIV         4             //90Mhz/4=22.5Mhz
#else
	#define CAMERA_MCLK_DIV         7             //84Mhz/7=12Mhz
#endif
#define OV9665_CAMERA_MCLK  24

static T_CAMERA_TYPE camera_ov9665_type = CAMERA_1P3M;
static T_NIGHT_MODE night_mode = CAMERA_DAY_MODE;
static T_CAMERA_MODE s_ov9665_CurMode = CAMERA_MODE_VGA;

static T_VOID camera_setbit(T_U8 reg, T_U8 bit, T_U8 value)
{
	T_U8 tmp;

	tmp = sccb_read_data(CAMERA_SCCB_ADDR, reg);
	if (value == 1)
		tmp |= 0x1<<bit;
	else
		tmp &= ~(0x1<<bit);
	sccb_write_data(CAMERA_SCCB_ADDR, reg, &tmp, 1);
}

static T_BOOL camera_set_param(const T_U8 tabParameter[])
{	
	int i = 0;
	T_U8 temp_value;

	while (1)
	{
		if ((END_FLAG == tabParameter[i]) && (END_FLAG == tabParameter[i + 1])) 
		{
			break;
		}
		else if (DELAY_FLAG == tabParameter[i])
		{
			mini_delay(tabParameter[i + 1]);
		}
		else
		{
			sccb_write_data(CAMERA_SCCB_ADDR, tabParameter[i], (T_U8 *)(&tabParameter[i + 1]), 1);

			if (!((tabParameter[i] == 0x12) && (tabParameter[i + 1] & 0x80)))
			{				
				temp_value = sccb_read_data(CAMERA_SCCB_ADDR, tabParameter[i]);
				if (temp_value != tabParameter[i + 1])
				{
					akprintf(C1, M_DRVSYS, "set parameter error!\n");
					akprintf(C1, M_DRVSYS, "reg 0x%x write data is 0x%x, read data is 0x%x!\n", tabParameter[i], tabParameter[i + 1], temp_value);

					return AK_FALSE;
				}
			}
		}
		i += 2;
	}

	return AK_TRUE;
}

static T_VOID camera_setup(const T_U8 tabParameter[])
{
	int i = 0;

	while (1)
	{
		if ((END_FLAG == tabParameter[i]) && (END_FLAG == tabParameter[i + 1])) 
		{
			break;
		}
		else if (DELAY_FLAG == tabParameter[i])
		{
			mini_delay(tabParameter[i + 1]);
		}
		else
		{
			sccb_write_data(CAMERA_SCCB_ADDR, tabParameter[i], (T_U8 *)(&tabParameter[i + 1]), 1);
		}
		i += 2;
	}
}

static T_VOID cam_ov9665_open(T_VOID)
{
	gpio_set_pin_dir(GPIO_CAMERA_AVDD, GPIO_DIR_OUTPUT);
	gpio_set_pin_level(GPIO_CAMERA_AVDD, gpio_pin_get_ActiveLevel(GPIO_CAMERA_AVDD));   
	
	gpio_set_pin_dir(GPIO_CAMERA_CHIP_ENABLE, GPIO_DIR_OUTPUT);
	gpio_set_pin_level(GPIO_CAMERA_CHIP_ENABLE, CAM_EN_LEVEL);	
	mini_delay(10);
	
	gpio_set_pin_dir(GPIO_CAMERA_RESET, GPIO_DIR_OUTPUT);
	gpio_set_pin_level(GPIO_CAMERA_RESET, CAM_RESET_LEVEL);
	mini_delay(10);
	gpio_set_pin_level(GPIO_CAMERA_RESET, !CAM_RESET_LEVEL);

	mini_delay(20);
}

static T_BOOL cam_ov9665_close(T_VOID)
{	
	gpio_set_pin_level(GPIO_CAMERA_CHIP_ENABLE, !CAM_EN_LEVEL);
	gpio_set_pin_level(GPIO_CAMERA_AVDD, !gpio_pin_get_ActiveLevel(GPIO_CAMERA_AVDD));    
	
	return AK_TRUE;
}

static T_U32 cam_ov9665_read_id(T_VOID)
{	
	T_U8 value = 0x00;
	T_U32 id = 0;

	sccb_init(GPIO_I2C_SCL, GPIO_I2C_SDA);        //init sccb first here!!
	
	value = sccb_read_data(CAMERA_SCCB_ADDR, 0x0a);
	id = value << 8;
	value = sccb_read_data(CAMERA_SCCB_ADDR, 0x0b);
//	if ((value >= 0x60) && (value <= 0x63))	//set all to 0x9660
//		value = 0x60;
	id |= value;
            
	return id;
}

/**
 * @brief: initialize the parameters of camera, should be done after reset and open camera to initialize   
 * @author: 
 * @date 2007-09-14
 * @return int
 * @retval 0 success; 1 fail
 */
static T_BOOL cam_ov9665_init(T_VOID)
{
	if (!camera_set_param(INIT_TAB))
	{
		return AK_FALSE;
	}
	else
	{		
		night_mode = CAMERA_DAY_MODE;
		return AK_TRUE;
	}		
}

/**
 * @brief: Set camera mode to specify image quality, SXGA/VGA/CIF/ etc 
 * @author: 
 * @date 2007-09-14
 * @param[in] mode: mode value
 * @return T_VOID
 * @retval
 */
static T_VOID cam_ov9665_set_mode(T_CAMERA_MODE mode)
{
    s_ov9665_CurMode = mode;
	switch(mode)
	{
		case CAMERA_MODE_SXGA:              //capture mode
			camera_setup(SXGA_MODE_TAB);
			break;			
		case CAMERA_MODE_VGA:               //capture mode
			camera_setup(VGA_MODE_TAB);
			break;
		case CAMERA_MODE_CIF:               //capture mode
			camera_setup(CIF_MODE_TAB);
			break;
		case CAMERA_MODE_QVGA:              //capture mode
			camera_setup(QVGA_MODE_TAB);
			break;
		case CAMERA_MODE_QCIF:              //capture mode
			camera_setup(QCIF_MODE_TAB);
			break;
		case CAMERA_MODE_QQVGA:             //capture mode
			camera_setup(QQVGA_MODE_TAB);
			break;
		case CAMERA_MODE_PREV:                              //preview mode
			camera_setup(PREV_MODE_TAB);
			
			if (CAMERA_NIGHT_MODE == night_mode)
			{
				camera_setup(NIGHT_MODE_TAB);
			}
			else
			{
				camera_setup(DAY_MODE_TAB);
			}
			break;
		case CAMERA_MODE_REC:                              //record mode
			camera_setup(RECORD_MODE_TAB);
			break;
		default:
            s_ov9665_CurMode = CAMERA_MODE_VGA;
			akprintf(C1, M_DRVSYS, "set camera mode parameter error!\n");
			break;
	}
}

/**
 * @brief: Set camera exposure mode 
 * @author: 
 * @date 2007-09-14
 * @param[in] exposure: exposure mode
 * @return T_VOID
 * @retval
 */
static T_VOID cam_ov9665_set_exposure(T_CAMERA_EXPOSURE exposure)
{
	switch(exposure)
	{
		case EXPOSURE_WHOLE:
			camera_setup(EXPOSURE_WHOLE_TAB);
			break;
		case EXPOSURE_CENTER:
			camera_setup(EXPOSURE_CENTER_TAB);
			break;
		case EXPOSURE_MIDDLE:
			camera_setup(EXPOSURE_MIDDLE_TAB);
			break;
		default:
			akprintf(C1, M_DRVSYS, "set exposure parameter error!\n");
			break;
	}
}

/**
 * @brief: Set camera brightness level 
 * @author: 
 * @date 2007-09-14
 * @param[in] brightness: brightness value
 * @return T_VOID
 * @retval
 */
static T_VOID cam_ov9665_set_brightness(T_CAMERA_BRIGHTNESS brightness)
{
	switch(brightness)
	{
		case CAMERA_BRIGHTNESS_0:
			camera_setup(BRIGHTNESS_0_TAB);
			break;
		case CAMERA_BRIGHTNESS_1:
			camera_setup(BRIGHTNESS_1_TAB);
			break;
		case CAMERA_BRIGHTNESS_2:
			camera_setup(BRIGHTNESS_2_TAB);
			break;
		case CAMERA_BRIGHTNESS_3:
			camera_setup(BRIGHTNESS_3_TAB);
			break;
		case CAMERA_BRIGHTNESS_4:
			camera_setup(BRIGHTNESS_4_TAB);
			break;
		case CAMERA_BRIGHTNESS_5:
			camera_setup(BRIGHTNESS_5_TAB);
			break;
		case CAMERA_BRIGHTNESS_6:
			camera_setup(BRIGHTNESS_6_TAB);
			break;
		default:
			akprintf(C1, M_DRVSYS, "set brightness parameter error!\n");
			break;
	}
}

/**
 * @brief: Set camera contrast level 
 * @author: 
 * @date 2007-09-14
 * @param[in] contrast: contrast value
 * @return T_VOID
 * @retval
 */
static T_VOID cam_ov9665_set_contrast(T_CAMERA_CONTRAST contrast)
{
	switch(contrast)
	{
		case CAMERA_CONTRAST_1:
			camera_setup(CONTRAST_1_TAB);
			break;
		case CAMERA_CONTRAST_2:
			camera_setup(CONTRAST_2_TAB);
			break;
		case CAMERA_CONTRAST_3:
			camera_setup(CONTRAST_3_TAB);
			break;
		case CAMERA_CONTRAST_4:
			camera_setup(CONTRAST_4_TAB);
			break;
		case CAMERA_CONTRAST_5:
			camera_setup(CONTRAST_5_TAB);
			break;
		case CAMERA_CONTRAST_6:
			camera_setup(CONTRAST_6_TAB);
			break;
		case CAMERA_CONTRAST_7:
			camera_setup(CONTRAST_7_TAB);
			break;
		default:
			akprintf(C1, M_DRVSYS, "set contrast parameter error!\n");
			break;
	}
}

/**
 * @brief: Set camera saturation level 
 * @author: 
 * @date 2007-09-14
 * @param[in] saturation: saturation value
 * @return T_VOID
 * @retval
 */
static T_VOID cam_ov9665_set_saturation(T_CAMERA_SATURATION saturation)
{
	switch(saturation)
	{
		case CAMERA_SATURATION_1:
			camera_setup(SATURATION_1_TAB);
			break;
		case CAMERA_SATURATION_2:
			camera_setup(SATURATION_2_TAB);
			break;
		case CAMERA_SATURATION_3:
			camera_setup(SATURATION_3_TAB);
			break;
		case CAMERA_SATURATION_4:
			camera_setup(SATURATION_4_TAB);
			break;
		case CAMERA_SATURATION_5:
			camera_setup(SATURATION_5_TAB);
			break;
		default:
			akprintf(C1, M_DRVSYS, "set saturation parameter error!\n");
			break;
	}
}

/**
 * @brief: Set camera sharpness level 
 * @author: 
 * @date 2007-09-14
 * @param[in] sharpness: sharpness value
 * @return T_VOID
 * @retval
 */
static T_VOID cam_ov9665_set_sharpness(T_CAMERA_SHARPNESS sharpness)
{
	switch(sharpness)
	{
		case CAMERA_SHARPNESS_0:
			camera_setup(SHARPNESS_0_TAB);
			break;
		case CAMERA_SHARPNESS_1:
			camera_setup(SHARPNESS_1_TAB);
			break;
		case CAMERA_SHARPNESS_2:
			camera_setup(SHARPNESS_2_TAB);
			break;
		case CAMERA_SHARPNESS_3:
			camera_setup(SHARPNESS_3_TAB);
			break;
		case CAMERA_SHARPNESS_4:
			camera_setup(SHARPNESS_4_TAB);
			break;
		case CAMERA_SHARPNESS_5:
			camera_setup(SHARPNESS_5_TAB);
			break;
		case CAMERA_SHARPNESS_6:
			camera_setup(SHARPNESS_6_TAB);
			break;
		default:
			akprintf(C1, M_DRVSYS, "set sharpness parameter error!\n");
			break;
	}
}

/**
 * @brief: Set camera AWB mode 
 * @author: 
 * @date 2007-09-14
 * @param[in] awb: AWB mode
 * @return T_VOID
 * @retval
 */
static T_VOID cam_ov9665_set_AWB(T_CAMERA_AWB awb)
{
	switch(awb)
	{
		case AWB_AUTO:
			camera_setup(AWB_AUTO_TAB);
			break;
		case AWB_SUNNY:
			camera_setup(AWB_SUNNY_TAB);
			break;
		case AWB_CLOUDY:
			camera_setup(AWB_CLOUDY_TAB);
			break;
		case AWB_OFFICE:
			camera_setup(AWB_OFFICE_TAB);
			break;
		case AWB_HOME:
			camera_setup(AWB_HOME_TAB);
			break;
		case AWB_NIGHT:
			camera_setup(AWB_NIGHT_TAB);
			break;
		default:
			akprintf(C1, M_DRVSYS, "set AWB mode parameter error!\n");
			break;			
	}
}

/**
 * @brief: Set camera mirror mode 
 * @author: 
 * @date 2007-09-14
 * @param[in] mirror: mirror mode
 * @return T_VOID
 * @retval
 */
static T_VOID cam_ov9665_set_mirror(T_CAMERA_MIRROR mirror)
{
	switch(mirror)
	{
		case CAMERA_MIRROR_V:
				//camera_setup(MIRROR_V_TAB);
				camera_setbit(0x04, 6, 1);
				camera_setbit(0x04, 7, 0);
				break;	
		case CAMERA_MIRROR_H:
				//camera_setup(MIRROR_H_TAB);
				camera_setbit(0x04, 6, 0);
				camera_setbit(0x04, 7, 1);
				break;
		case CAMERA_MIRROR_NORMAL:
				//camera_setup(MIRROR_NORMAL_TAB);
				camera_setbit(0x04, 6, 0);
				camera_setbit(0x04, 7, 0);
				break;	
		case CAMERA_MIRROR_FLIP:
				//camera_setup(MIRROR_FLIP_TAB);
				camera_setbit(0x04, 6, 1);
				camera_setbit(0x04, 7, 1);
				break;					
		default:
			akprintf(C1, M_DRVSYS, "set mirror parameter error!\n");
			break;
	}
}

/**
 * @brief: Set camera effect mode 
 * @author: 
 * @date 2004-09-22
 * @param[in] effect: effect mode
 * @return T_VOID
 * @retval
 */
static T_VOID cam_ov9665_set_effect(T_CAMERA_EFFECT effect)
{
	switch(effect)
	{
		case CAMERA_EFFECT_NORMAL:
			camera_setup(EFFECT_NORMAL_TAB);
			break;
		case CAMERA_EFFECT_SEPIA:
			camera_setup(EFFECT_SEPIA_TAB);
			break;
		case CAMERA_EFFECT_ANTIQUE:
			camera_setup(EFFECT_ANTIQUE_TAB);
			break;
		case CAMERA_EFFECT_BLUE:
			camera_setup(EFFECT_BLUISH_TAB);
			break;			
		case CAMERA_EFFECT_GREEN:
			camera_setup(EFFECT_GREENISH_TAB);
			break;
		case CAMERA_EFFECT_RED:
			camera_setup(EFFECT_REDDISH_TAB);
			break;
		case CAMERA_EFFECT_NEGATIVE:
			camera_setup(EFFECT_NEGATIVE_TAB);
			break;	
		case CAMERA_EFFECT_BW:
			camera_setup(EFFECT_BW_TAB);
			break;
		case CAMERA_EFFECT_BWN:
			camera_setup(EFFECT_BWN_TAB);
			break;	
		default:
			akprintf(C1, M_DRVSYS, "set camer effect parameter error!\n");
			break;
	}
}

/**
 * @brief: Set camera windows  
 * @author: 
 * @date 2007-09-14
 * @param[in] width: window width
 * @param[in] height: window height
 * @return int
 * @retval 0 error mode; 1 success; -1 fail
 */
static T_S32 cam_ov9665_set_digital_zoom(T_U32 width, T_U32 height)
{	
	T_U16 hrefstart = 0, hrefstop = 0, vrefstart = 0, vrefstop = 0;
	T_U8 hbit = 0, lstartbit = 0, lstopbit = 0;

	T_U8 value;

	T_U8 Camera_window_table[] =
	{
		0x17, 0x00,
		0x18, 0x00,
		0x32, 0x00,
		0x19, 0x00,
		0x1a, 0x00,
		0x03, 0x00,

		0xb9, 0x00,
		0xba, 0x00,
		0xbb, 0x00,
		0xbc, 0x00,
		
		END_FLAG, END_FLAG
	};

	switch(s_ov9665_CurMode)
	{			
		case CAMERA_MODE_SXGA:
			hrefstart = 100 + (1280 - width) / 4;
			hrefstop = 742 - (1280 - width) / 4;

			vrefstart = 7 + (1024 - height) / 4;
			vrefstop = 523 - (1024 - height) / 4;

			Camera_window_table[13] = width / 8;
			Camera_window_table[15] = height / 8;
			Camera_window_table[17] = width / 8;
			Camera_window_table[19] = height / 8;
			break;

		case CAMERA_MODE_VGA:
			hrefstart = 100 + (640 - width) / 2;
			hrefstop = 742 - (640 - width) / 2;

			vrefstart = 11 + (480 - height) / 4;
			vrefstop = 252 - (480 - height) / 4;

			Camera_window_table[13] = width / 8;
			Camera_window_table[15] = height / 8;
			Camera_window_table[17] = width / 8;
			Camera_window_table[19] = height / 8;
			break;

		case CAMERA_MODE_CIF:
			hrefstart = 100 + (1280 - 1280*width/352) / 4;
			hrefstop = 742 - (1280 - 1280*width/352) / 4;

			vrefstart = 7 + (1024 - 1024*height/288) / 4;
			vrefstop = 523 - (1024 - 1024*height/288) / 4;

			Camera_window_table[13] = 1280*width/352 / 8;
			Camera_window_table[15] = 1024*height/288 / 8;
			Camera_window_table[17] = width / 8;
			Camera_window_table[19] = height / 8;
			break;
			
		case CAMERA_MODE_QVGA:
/*			hrefstart = 101 + (320 - width) / 4;
			hrefstop = 749 - (320 - width) / 4;

			vrefstart = 11 + (240 - height) / 2;
			vrefstop = 252 - (240 - height) / 2;

			Camera_window_table[13] = width / 8;
			Camera_window_table[15] = height / 8;
			Camera_window_table[17] = width / 8;
			Camera_window_table[19] = height / 8;
*/
			hrefstart = 101 + (640 - 640*width/320) / 2;
			hrefstop = 749 - (640 - 640*width/320) / 2;

			vrefstart = 11 + (480 - 480*height/240) / 4;
			vrefstop = 252 - (480 - 480*height/240) / 4;

			Camera_window_table[13] = 640*width/320 / 8;
			Camera_window_table[15] = 480*height/240 / 8;
			Camera_window_table[17] = width / 8;
			Camera_window_table[19] = height / 8;
			break;
			
		case CAMERA_MODE_QCIF:
			hrefstart = 100 + (1280 - 1280*width/176) / 4;
			hrefstop = 742 - (1280 - 1280*width/176) / 4;

			vrefstart = 7 + (1024 - 1024*height/144) / 4;
			vrefstop = 523 - (1024 - 1024*height/144) / 4;

			Camera_window_table[13] = 1280*width/176 / 8;
			Camera_window_table[15] = 1024*height/144 / 8;
			Camera_window_table[17] = width / 8;
			Camera_window_table[19] = height / 8;
			break;
			
		case CAMERA_MODE_QQVGA:
			hrefstart = 100 + (1280 - 1280*width/160) / 4;
			hrefstop = 742 - (1280 - 1280*width/160) / 4;

			vrefstart = 7 + (1024 - 1024*height/120) / 4;
			vrefstop = 523 - (1024 - 1024*height/120) / 4;

			Camera_window_table[13] = 1280*width/160 / 8;
			Camera_window_table[15] = 1024*height/120 / 8;
			Camera_window_table[17] = width / 8;
			Camera_window_table[19] = height / 8;
			break;
			
		case CAMERA_MODE_PREV:
			hrefstart = 100 + (640 - width) / 2;
			hrefstop = 742 - (640 - width) / 2;

			vrefstart = 11 + (480 - height) / 4;
			vrefstop = 252 - (480 - height) / 4;

			Camera_window_table[13] = width / 8;
			Camera_window_table[15] = height / 8;
			Camera_window_table[17] = width / 8;
			Camera_window_table[19] = height / 8;
			break;

		case CAMERA_MODE_REC:
			hrefstart = 100 + (640 - width) / 2;
			hrefstop = 742 - (640 - width) / 2;

			vrefstart = 11 + (480 - height) / 4;
			vrefstop = 252 - (480 - height) / 4;

			Camera_window_table[13] = width / 8;
			Camera_window_table[15] = height / 8;
			Camera_window_table[17] = width / 8;
			Camera_window_table[19] = height / 8;
			break;
			
		default:
			//akprintf(C1, M_DRVSYS, "unsupported WINDOWING in mode %d!!\n", s_ov9665_CurMode);
			return 0;
	}

	akprintf(C1, M_DRVSYS, "set window size %d, %d, %d\r\n", s_ov9665_CurMode, width, height);

	hbit = hrefstart >> 3;         //Horizontal Frame start high 8-bit
	lstartbit = hrefstart & 0x7;   //Horizontal Frame start low 3-bit
	
	Camera_window_table[1] = hbit;
		
	hbit = hrefstop >> 3;          //Horizontal Frame end high 8-bit
	lstopbit = hrefstop & 0x7;     //Horizontal Frame end low 3-bit
	
	Camera_window_table[3] = hbit;

	value = sccb_read_data(CAMERA_SCCB_ADDR, Camera_window_table[4]);
	Camera_window_table[5] = (value & 0xc0) | lstartbit | (lstopbit << 3);
	
	hbit = vrefstart >> 2;         //Vertical Frame start high 8-bit
	lstartbit = vrefstart & 0x3;   //Vertical Frame start low 2-bit
	
	Camera_window_table[7] = hbit;
	
	hbit = vrefstop >> 2;          //Vertical Frame end high 8-bit
	lstopbit = vrefstop & 0x3;     //Vertical Frame end low 2-bit
	
	Camera_window_table[9] = hbit;

	value = sccb_read_data(CAMERA_SCCB_ADDR, Camera_window_table[10]);
	Camera_window_table[11] = (value & 0xc0) | lstartbit | (lstopbit << 2);

	if (camera_set_param(Camera_window_table) == AK_TRUE)
		return 1;
	else
		return -1;
}

static T_VOID cam_ov9665_set_night_mode(T_NIGHT_MODE mode)
{
	switch(mode)
	{
		case CAMERA_DAY_MODE:
			camera_setup(DAY_MODE_TAB);
			night_mode = CAMERA_DAY_MODE;
			break;
		case CAMERA_NIGHT_MODE:
			camera_setup(NIGHT_MODE_TAB);
			night_mode = CAMERA_NIGHT_MODE;
			break;
		default:
			akprintf(C1, M_DRVSYS, "set night mode parameter error!\n");
			break;
	}
}

static T_VOID start_preview(T_CAMERA_MODE Cammode)
{
	T_U8 Reg0x13;

	//Change to preview mode
	cam_ov9665_set_mode(Cammode);
	
	//Start AG/AE
	Reg0x13 = sccb_read_data(CAMERA_SCCB_ADDR, 0x13);
	Reg0x13 = Reg0x13 | 0x05;
	sccb_write_data(CAMERA_SCCB_ADDR, 0x13, &Reg0x13, 1);
}

static T_VOID start_capture(T_CAMERA_MODE Cammode)
{
	T_U16 Capture_dummy_pixel;
	T_U16 Capture_dummy_line;	
	T_CAMERA_MODE mode = CAMERA_MODE_VGA;	
	T_U8 capture_max_gain = 15;	
	T_U16 Default_VGA_Line_Width = 1190;
	T_U16 Default_SXGA_Line_Width = 1922;	
	T_U16 Default_VGA_maximum_shutter = 525;
	T_U16 Default_SXGA_maximum_shutter = 1051;

	T_U8 Reg0x13;
	T_U8 Reg0x45;
	T_U8 Reg0x10;
	T_U8 Reg0x04;
	T_U8 Reg0x2d;
	T_U8 Reg0x2e;
	T_U8 Reg0x00;
	T_U8 Reg0x2a;
	T_U8 Reg0x2b;
	T_U8 Reg0x46;
	T_U8 Reg0x47;

	T_U32 Shutter;
	T_U32 Extra_lines;
	T_U32 Preview_Exposure;	
	T_U32 Preview_Gain16;
	T_U32 Preview_dummy_pixel;
//	T_U32 Preview_PCLK_frequency;
//	T_U32 Capture_PCLK_frequency;	
	float Preview_PCLK_frequency;
	float Capture_PCLK_frequency;	
	T_U32 Capture_Max_Gain16;
	T_U32 Preview_line_width;
	T_U32 Capture_line_width;	
	T_U32 Capture_maximum_shutter;
	T_U32 Capture_banding_Filter;
	T_U32 Gain_Exposure;
	T_U32 Capture_dummy_pixel_reg;
	T_U32 Capture_Exposure;
	T_U32 Capture_Gain16;
	T_U32 Gain;
	
	//Stop AE/AG
	Reg0x13 = sccb_read_data(CAMERA_SCCB_ADDR, 0x13);
	Reg0x13 = Reg0x13 & 0xfa;
	sccb_write_data(CAMERA_SCCB_ADDR, 0x13, &Reg0x13, 1);
	
//	akprintf(C3, M_DRVSYS, "Reg0x13 = 0x%-8x\n", Reg0x13);

	//Read back preview shutter
	Reg0x45 = sccb_read_data(CAMERA_SCCB_ADDR, 0x45);
	Reg0x10 = sccb_read_data(CAMERA_SCCB_ADDR, 0x10);
	Reg0x04 = sccb_read_data(CAMERA_SCCB_ADDR, 0x04);
	Shutter = ((Reg0x45 & 0x3f) << 10) + (Reg0x10 << 2) + (Reg0x04 & 0x03);
	
//	akprintf(C3, M_DRVSYS, "Reg0x45 = 0x%-8x\n", Reg0x45);
//	akprintf(C3, M_DRVSYS, "Reg0x10 = 0x%-8x\n", Reg0x10);
//	akprintf(C3, M_DRVSYS, "Reg0x04 = 0x%-8x\n", Reg0x04);
//	akprintf(C3, M_DRVSYS, "Shutter = 0x%-8x\n", Shutter);

	//Read back extra line
	Reg0x2d = sccb_read_data(CAMERA_SCCB_ADDR, 0x2d);
	Reg0x2e = sccb_read_data(CAMERA_SCCB_ADDR, 0x2e);
	
	Extra_lines = Reg0x2d + (Reg0x2e << 8);
	Preview_Exposure = Shutter + Extra_lines;
	
//	akprintf(C3, M_DRVSYS, "Reg0x2d = 0x%-8x\n", Reg0x2d);
//	akprintf(C3, M_DRVSYS, "Reg0x2e = 0x%-8x\n", Reg0x2e);
//	akprintf(C3, M_DRVSYS, "Extra_lines = 0x%-8x\n", Extra_lines);	
//	akprintf(C3, M_DRVSYS, "Preview_Exposure = 0x%-8x\n", Preview_Exposure);

	//Read Back Gain for preview
	Reg0x00 = sccb_read_data(CAMERA_SCCB_ADDR, 0x00);
	Preview_Gain16 = (((Reg0x00 & 0xf0) >> 4) + 1) * (16 + (Reg0x00 & 0x0f));
	
//	akprintf(C3, M_DRVSYS, "reg0x00 = 0x%-8x\n", Reg0x00);
//	akprintf(C3, M_DRVSYS, "Preview_Gain16 = 0x%-8x\n", Preview_Gain16);

	//Read back dummy pixels
	Reg0x2a = sccb_read_data(CAMERA_SCCB_ADDR, 0x2a);
	Reg0x2b = sccb_read_data(CAMERA_SCCB_ADDR, 0x2b);	
	Preview_dummy_pixel = ((Reg0x2a & 0xf0) << 4) + Reg0x2b;

//	akprintf(C3, M_DRVSYS, "Reg0x2a = 0x%-8x\n", Reg0x2a);
//	akprintf(C3, M_DRVSYS, "Reg0x2b = 0x%-8x\n", Reg0x2b);
//	akprintf(C3, M_DRVSYS, "Preview_dummy_pixel = 0x%-8x\n", Preview_dummy_pixel);

	//capture setting	
	Capture_dummy_pixel = 0x00;
	Capture_dummy_line = 0x00;
	mode = CAMERA_MODE_SXGA;

#if (defined(CHIP_AK7801) || defined(CHIP_AK7802))
	Preview_PCLK_frequency = 24;
	Capture_PCLK_frequency = 36;
#elif defined(CHIP_AK322L)
	Preview_PCLK_frequency = 22.5;
	Capture_PCLK_frequency = 33.75;
#else
	Preview_PCLK_frequency = 24;
	Capture_PCLK_frequency = 36;
#endif	
	
	//Capture maximum gain could be defined.
	Capture_Max_Gain16 = capture_max_gain * 16;
//	akprintf(C3, M_DRVSYS, "Capture_Max_Gain16 = 0x%-8x\n", Capture_Max_Gain16);

	Preview_line_width = Default_VGA_Line_Width + Preview_dummy_pixel;
	
	if (CAMERA_MODE_VGA == mode)
	{
		Capture_line_width = Default_VGA_Line_Width + Capture_dummy_pixel;
	}
	else 
	{
		Capture_line_width = Default_SXGA_Line_Width + Capture_dummy_pixel;
	}

//	akprintf(C3, M_DRVSYS, "Preview_line_width = 0x%-8x\n", Preview_line_width);
//	akprintf(C3, M_DRVSYS, "Capture_line_width = 0x%-8x\n", Capture_line_width);

	if (CAMERA_MODE_VGA == mode) 
	{
		Capture_maximum_shutter = Default_VGA_maximum_shutter + Capture_dummy_line;
	}
	else 
	{
		Capture_maximum_shutter = Default_SXGA_maximum_shutter + Capture_dummy_line;
	}
//	akprintf(C3, M_DRVSYS, "Capture_maximum_shutter = 0x%-8x\n", Capture_maximum_shutter);

	Capture_Exposure = Preview_Exposure * 2 * Capture_PCLK_frequency / Preview_PCLK_frequency * Preview_line_width / Capture_line_width;
//	akprintf(C3, M_DRVSYS, "Capture_Exposure = 0x%-8x\n", Capture_Exposure);

	//Calculate banding filter value
	if (1)  //50Hz
	{
		if (0) 
		{//RGB indicates raw RGB
			Capture_banding_Filter = Capture_PCLK_frequency * 1000000 / 100 / Capture_line_width;
		}
		else 
		{
			Capture_banding_Filter = Capture_PCLK_frequency * 1000000 / 100 / (2 * Capture_line_width);
		}
	}
	else
	{
		if (0) 
		{
			Capture_banding_Filter = Capture_PCLK_frequency * 1000000 / 120 / Capture_line_width;
		}
		else 
		{
			Capture_banding_Filter = Capture_PCLK_frequency * 1000000 / 120 / (2 * Capture_line_width);
		}
	}
//	akprintf(C3, M_DRVSYS, "Capture_banding_Filter = 0x%-8x\n", Capture_banding_Filter);
	
	//redistribute gain and exposure
	Gain_Exposure = Preview_Gain16 * Capture_Exposure;
//	akprintf(C3, M_DRVSYS, "Gain_Exposure = 0x%-8x\n", Gain_Exposure);
	if (Gain_Exposure < Capture_banding_Filter * 16)
	{
		// Exposure < 1/100
		Capture_Exposure = Gain_Exposure / 16;
		Capture_Gain16 = (Gain_Exposure * 2 + 1) / Capture_Exposure / 2;
	}
	else 
	{
		if (Gain_Exposure > Capture_maximum_shutter * 16) 
		{
			// Exposure > Capture_Maximum_Shutter
			Capture_Exposure = Capture_maximum_shutter;
			Capture_Gain16 = (Gain_Exposure * 2 + 1) / Capture_maximum_shutter / 2;

			if (Capture_Gain16 > Capture_Max_Gain16) 
			{
				// gain reach maximum, insert extra line
				Capture_Exposure = Gain_Exposure * 1.1 / Capture_Max_Gain16;
				// For 50Hz, Exposure = n/100; For 60Hz, Exposure = n/120
				Capture_Exposure = Gain_Exposure / 16 / Capture_banding_Filter;
				Capture_Exposure = Capture_Exposure * Capture_banding_Filter;
				Capture_Gain16 = (Gain_Exposure * 2 + 1) / Capture_Exposure / 2;
			}
		}
		else
		{
			// 1/100(120) < Exposure < Capture_Maximum_Shutter, Exposure = n/100(120)
			Capture_Exposure = Gain_Exposure / 16 / Capture_banding_Filter;
			Capture_Exposure =  Capture_Exposure * Capture_banding_Filter;
			Capture_Gain16 = (Gain_Exposure * 2 + 1) / Capture_Exposure / 2;
		}
	}
//	akprintf(C3, M_DRVSYS, "Capture_Exposure = 0x%-8x\n", Capture_Exposure);
//	akprintf(C3, M_DRVSYS, "Capture_Gain16 = 0x%-8x\n", Capture_Gain16);
	
	//Write registers, change to QXGA resolution.
	cam_ov9665_set_mode(Cammode);
	
	//write dummy pixels
	if (CAMERA_MODE_VGA == mode)
	{
		Capture_dummy_pixel_reg = Capture_dummy_pixel * 2;
	}
	else
	{
		Capture_dummy_pixel_reg = Capture_dummy_pixel;
	}	
	Reg0x2b = Capture_dummy_pixel_reg & 0x00ff;
	Reg0x2a = sccb_read_data(CAMERA_SCCB_ADDR, 0x2a);	
	Reg0x2a = (Reg0x2a & 0x0f) | ((Capture_dummy_pixel_reg & 0x0f00) >> 4);
	sccb_write_data(CAMERA_SCCB_ADDR, 0x2a, &Reg0x2a, 1);
	sccb_write_data(CAMERA_SCCB_ADDR, 0x2b, &Reg0x2b, 1);
	
//	akprintf(C3, M_DRVSYS, "Reg0x2a = 0x%-8x\n", Reg0x2a);
//	akprintf(C3, M_DRVSYS, "Reg0x2b = 0x%-8x\n", Reg0x2b);
	
	//Write Dummy Lines
	Reg0x46 = Capture_dummy_line & 0x00ff;
	Reg0x47 = Capture_dummy_line >> 8;
	sccb_write_data(CAMERA_SCCB_ADDR, 0x46, &Reg0x46, 1);
	sccb_write_data(CAMERA_SCCB_ADDR, 0x47, &Reg0x47, 1);
	
//	akprintf(C3, M_DRVSYS, "Reg0x46 = 0x%-8x\n", Reg0x46);
//	akprintf(C3, M_DRVSYS, "Reg0x47 = 0x%-8x\n", Reg0x47);
	
	//Write Exposure
	if (Capture_Exposure > Capture_maximum_shutter) 
	{
		Shutter = Capture_maximum_shutter;
		Extra_lines = Capture_Exposure - Capture_maximum_shutter;
	}
	else 
	{
		Shutter = Capture_Exposure;
		Extra_lines = 0;
	}
	
//	akprintf(C3, M_DRVSYS, "Shutter = 0x%-8x\n", Shutter);
//	akprintf(C3, M_DRVSYS, "Extra_lines = 0x%-8x\n", Extra_lines);	

	Reg0x04 = sccb_read_data(CAMERA_SCCB_ADDR, 0x04);
	Reg0x04 = (Reg0x04 & 0xfc) | (Shutter & 0x000003);
	Reg0x10 = (Shutter >> 2) & 0x00ff;
	Reg0x45 = sccb_read_data(CAMERA_SCCB_ADDR, 0xa1);
	Reg0x45 = (Reg0x45 & 0xc0) | ((Shutter >> 10) & 0x3f);

	sccb_write_data(CAMERA_SCCB_ADDR, 0x45, &Reg0x45, 1);
	sccb_write_data(CAMERA_SCCB_ADDR, 0x10, &Reg0x10, 1);
	sccb_write_data(CAMERA_SCCB_ADDR, 0x04, &Reg0x04, 1);	

//	akprintf(C3, M_DRVSYS, "Reg0x45 = 0x%-8x\n", Reg0x45);
//	akprintf(C3, M_DRVSYS, "Reg0x10 = 0x%-8x\n", Reg0x10);
//	akprintf(C3, M_DRVSYS, "Reg0x04 = 0x%-8x\n", Reg0x04);
	
	//Write extra line
	Reg0x2d = Extra_lines & 0x00ff;
	Reg0x2e = Extra_lines >> 8;
	sccb_write_data(CAMERA_SCCB_ADDR, 0x2d, &Reg0x2d, 1);
	sccb_write_data(CAMERA_SCCB_ADDR, 0x2e, &Reg0x2e, 1);
	
//	akprintf(C3, M_DRVSYS, "cap reg0x2d = 0x%-8x\n", Reg0x2d);
//	akprintf(C3, M_DRVSYS, "cap reg0x2e = 0x%-8x\n", Reg0x2e);

	//Write Gain
	Gain = 0;
	if (Capture_Gain16 > 31) 
	{
		Capture_Gain16 = Capture_Gain16 / 2;
		Gain = 0x10;
	}
	if (Capture_Gain16 > 31)
	{
		Capture_Gain16 = Capture_Gain16 / 2;
		Gain = Gain | 0x20;
	}
	if (Capture_Gain16 > 31)
	{
		Capture_Gain16 = Capture_Gain16 / 2;
		Gain = Gain | 0x40;
	}
	if (Capture_Gain16 > 31) 
	{
		Capture_Gain16 = Capture_Gain16 / 2;
		Gain = Gain | 0x80;
	}
	if (Capture_Gain16 > 16) 
	{
		Gain = Gain | ((Capture_Gain16 -16) & 0x0f);
	}
//	akprintf(C3, M_DRVSYS, "Capture_Gain16 = 0x%-8x, Gain = 0x%-8x\n", Capture_Gain16, Gain);
	
	sccb_write_data(CAMERA_SCCB_ADDR, 0x00, (T_U8 *)&Gain, 1);
}

static T_VOID start_record(T_CAMERA_MODE Cammode)
{
	T_U8 Reg0x13;

	//Change to record mode
	cam_ov9665_set_mode(Cammode);
	
	//Start AG/AE
	Reg0x13 = sccb_read_data(CAMERA_SCCB_ADDR, 0x13);
	Reg0x13 = Reg0x13 | 0x05;
	sccb_write_data(CAMERA_SCCB_ADDR, 0x13, &Reg0x13, 1);	
}

static T_BOOL cam_ov9665_set_to_cap(T_U32 width, T_U32 height)
{
	float Mclk;
    T_CAMERA_MODE Cammode;
	Mclk = (float)(get_asic_freq() / 1000000) / CAMERA_MCLK_DIV;

    if ((width <= 160) && (height <= 120))
    {
        Cammode = CAMERA_MODE_QQVGA;
    }
    else if ((width <= 176) && (height <= 144))
    {
        Cammode = CAMERA_MODE_QCIF;
    }
    else if ((width <= 320) && (height <= 240))
    {
        Cammode = CAMERA_MODE_QVGA;
    }
    else if ((width <= 352) && (height <= 288))
    {
        Cammode = CAMERA_MODE_CIF;
    }
    else if ((width <= 640) && (height <= 480))
    {
        Cammode = CAMERA_MODE_VGA;
    }
    else if ((width <= 1280) && (height <= 1024))
    {
        Cammode = CAMERA_MODE_SXGA;
    }
    else
    {
        akprintf(C1, M_DRVSYS, "ov9665 unsupport %d & %d mode!\n", width, height);
        return AK_FALSE;
    }
    




	if (CAMERA_MODE_SXGA == Cammode)
	{
		start_capture(Cammode);	
		cam_ov9665_set_digital_zoom(width, height);
		mini_delay(300);
	}
	else
	{
		cam_ov9665_set_mode(Cammode);
		cam_ov9665_set_digital_zoom(width, height);	
		mini_delay(300);		
	}

	return AK_TRUE;
}

static T_BOOL cam_ov9665_set_to_prev(T_U32 width, T_U32 height)
{
	start_preview(CAMERA_MODE_PREV);
	cam_ov9665_set_digital_zoom(width, height);
	mini_delay(300);
	return AK_TRUE;
}

static T_BOOL cam_ov9665_set_to_record(T_U32 width, T_U32 height)
{	
	start_record(CAMERA_MODE_REC);
	cam_ov9665_set_digital_zoom(width, height);
	mini_delay(300);
	return AK_TRUE;
}

static T_CAMERA_TYPE cam_ov9665_get_type(T_VOID)
{
	return camera_ov9665_type;
}

static T_CAMERA_FUNCTION_HANDLER ov9665_function_handler = 
{
	OV9665_CAMERA_MCLK,
	cam_ov9665_open,
	cam_ov9665_close,
	cam_ov9665_read_id,
	cam_ov9665_init,
	cam_ov9665_set_mode,
	cam_ov9665_set_exposure,
	cam_ov9665_set_brightness,
	cam_ov9665_set_contrast,
	cam_ov9665_set_saturation,
	cam_ov9665_set_sharpness,
	cam_ov9665_set_AWB,
	cam_ov9665_set_mirror,
	cam_ov9665_set_effect,
	cam_ov9665_set_digital_zoom,
	cam_ov9665_set_night_mode,
	cam_ov9665_set_to_cap,
	cam_ov9665_set_to_prev,
	cam_ov9665_set_to_record,
	cam_ov9665_get_type
};

static int camera_ov9665_reg(void)
{
	camera_reg_dev(CAMERA_OV9665_ID, &ov9665_function_handler);
	return 0;
}

#ifdef __CC_ARM
#pragma arm section rwdata = "__initcall_", zidata = "__initcall_"
#endif
module_init(camera_ov9665_reg)
#ifdef __CC_ARM
#pragma arm section
#endif

#endif

