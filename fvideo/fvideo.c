#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <memory.h>
#include <sys/io.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "video_nvp6124.h"
#include "system.h"
#include "../dvr/i2c/i2c.h"
#include "../dvr/sysreg/sys_reg_public.h"
#include "initial.h"
#include "nvp6124_reg.h"
#include "power_control_public.h"

#define NTSC   1
#define PAL    0
#define NVP6124_R0_ID 	0x84
#define NVP6114A_R0_ID 	0x85

/*nvp6124 1080P 色彩推荐配置*/
#define BRI_CENTER_VAL_NTSC 0xF4
#define BRI_CENTER_VAL_PAL  0xF4
#define CON_CENTER_VAL_NTSC 0x90
#define CON_CENTER_VAL_PAL  0x90
#define SAT_CENTER_VAL_NTSC 0x80
#define SAT_CENTER_VAL_PAL  0x80                       
#define HUE_CENTER_VAL_NTSC 0x00
#define HUE_CENTER_VAL_PAL  0x00


/*nvp6124 720P 色彩推荐配置*/
#define BRI_CENTER_VAL_NTSC_720P 0x08
#define BRI_CENTER_VAL_PAL_720P  0x08
#define CON_CENTER_VAL_NTSC_720P 0x88
#define CON_CENTER_VAL_PAL_720P  0x88
#define SAT_CENTER_VAL_NTSC_720P 0x90
#define SAT_CENTER_VAL_PAL_720P  0x90                       
#define HUE_CENTER_VAL_NTSC_720P 0xFD
#define HUE_CENTER_VAL_PAL_720P  0x00
/*nvp6124 960H 色彩推荐配置*/
#define BRI_CENTER_VAL_NTSC_960H 0xF8
#define BRI_CENTER_VAL_PAL_960H  0xF5
#define CON_CENTER_VAL_NTSC_960H 0x80
#define CON_CENTER_VAL_PAL_960H  0x79
#define SAT_CENTER_VAL_NTSC_960H 0xA0
#define SAT_CENTER_VAL_PAL_960H  0x80                       
#define HUE_CENTER_VAL_NTSC_960H 0x01
#define HUE_CENTER_VAL_PAL_960H  0x00

unsigned int nvp6124_con_tbl[2]  = {CON_CENTER_VAL_NTSC, CON_CENTER_VAL_PAL};
unsigned int nvp6124_hue_tbl[2]  = {HUE_CENTER_VAL_NTSC, HUE_CENTER_VAL_PAL};
unsigned int nvp6124_sat_tbl[2]  = {SAT_CENTER_VAL_NTSC, SAT_CENTER_VAL_PAL};
unsigned int nvp6124_bri_tbl[2]  = {BRI_CENTER_VAL_NTSC, BRI_CENTER_VAL_PAL};

unsigned int nvp6124_con_tbl_720P[2]  = {CON_CENTER_VAL_NTSC_720P, CON_CENTER_VAL_PAL_720P};
unsigned int nvp6124_hue_tbl_720P[2]  = {HUE_CENTER_VAL_NTSC_720P, HUE_CENTER_VAL_PAL_720P};
unsigned int nvp6124_sat_tbl_720P[2]  = {SAT_CENTER_VAL_NTSC_720P, SAT_CENTER_VAL_PAL_720P};
unsigned int nvp6124_bri_tbl_720P[2]  = {BRI_CENTER_VAL_NTSC_720P, BRI_CENTER_VAL_PAL_720P};


unsigned int nvp6124_con_tbl_960H[2]  = {CON_CENTER_VAL_NTSC_960H, CON_CENTER_VAL_PAL_960H};
unsigned int nvp6124_hue_tbl_960H[2]  = {HUE_CENTER_VAL_NTSC_960H, HUE_CENTER_VAL_PAL_960H};
unsigned int nvp6124_sat_tbl_960H[2]  = {SAT_CENTER_VAL_NTSC_960H, SAT_CENTER_VAL_PAL_960H};
unsigned int nvp6124_bri_tbl_960H[2]  = {BRI_CENTER_VAL_NTSC_960H, BRI_CENTER_VAL_PAL_960H};

unsigned char nvp6124_motion_sens_tbl[8]= {0xe0,0xc8,0xa0,0x98,0x78,0x68,0x50,0x48};
unsigned char ch_mode_status[16]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char ch_mode_status_tmp[16]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};


unsigned int nvp6124_slave_addr[4] = {0x60, 0x62, 0x64, 0x66};
unsigned char ch_seq[4]={0,1,2,3};
unsigned int nvp6124_cnt = 1;
unsigned int nvp6124_mode = 1;  //0:ntsc, 1: pal
int chip_id[4];

extern enum video_standard_t  g_videostd;
extern enum capture_size_t g_videosize;

void nvp6124_write_table(unsigned char chip_addr, unsigned char addr, unsigned char *tbl_ptr, unsigned char tbl_cnt)
{
	unsigned char i = 0;
	
	for(i = 0; i < tbl_cnt; i ++)
	{
		GPIO_I2C_Write0(chip_addr, (addr + i), *(tbl_ptr + i));
	}
}
void nvp6124_outport_1mux_chseq(void)
{
	int i = 0;                               
	for(i=0;i<nvp6124_cnt;i++)
	{
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x01);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC0, ch_seq[0]<<4|ch_seq[0]);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC1, ch_seq[0]<<4|ch_seq[0]);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC2, ch_seq[1]<<4|ch_seq[1]);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC3, ch_seq[1]<<4|ch_seq[1]);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC4, ch_seq[2]<<4|ch_seq[2]);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC5, ch_seq[2]<<4|ch_seq[2]);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC6, ch_seq[3]<<4|ch_seq[3]);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC7, ch_seq[3]<<4|ch_seq[3]);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC8, 0x00);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xC9, 0x00);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xCA, 0xFF);
	}
	printf("nvp6124_outport_1mux_chseq\n");
}

void nvp6124_system_init(void)
{
    int i = 0;                               
	for(i=0;i<nvp6124_cnt;i++)
	{
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x01); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x82, 0x14); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x83, 0x2C); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x3e, 0x10); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x80, 0x60); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x80, 0x61); 
		//usleep(100); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x80, 0x40); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x81, 0x02); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x97, 0x00); 
		//usleep(10); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x80, 0x60); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x81, 0x00); 
		//usleep(10); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x97, 0x0F); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x38, 0x18); 
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0x38, 0x08); 
		//usleep(10); 
		//printf("~~~~~~~~~~~~~~~~20150227-2 nvp6124_system_init~~~~~~~~~~~~~~~~\n");
		//usleep(100);
		GPIO_I2C_Write0( nvp6124_slave_addr[i], 0xCA, 0xFF);
	}
	nvp6124_outport_1mux_chseq();
}
static void nvp6124_init()
{
	nvp6124_video_mode vmode;
	static int nvp6124_init = 0;
	int i;
	if(nvp6124_init==0)
	{
		chip_id[0] = NVP6124_R0_ID ;
		nvp6124_ntsc_common_init();
		for(i=0;i<4;i++)
		{
			vmode.vformat[i] = 0;
			vmode.chmode[i] = NVP6124_VI_1080P_2530;
		}
		nvp6124_each_mode_setting(&vmode);
		nvp6124_init=1;
	}

}

/***************************************************************************
Function: nvp6124_videoCreate
Description: 视频输入初始化;

Input: 	void;
Output: void;

Return: 成功: VIM_SUCCESS;
        失败: VIM_FAILURE;
******************************************************************************/
int nvp6124_videoCreate(void)
{
	int i = 0;
	int s32Ret = 0;
	nvp6124_video_mode vmode;
	enum video_standard_t video_stand=0;
	enum capture_size_t	  video_size=0;
	//audio_source_control_oper(AHD_MODE);
	//获取视频制式
	video_stand = nvp6124_videoGetVstd(0,0);
	video_size = nvp6124_videoGetSize(0);
	
	//s32Ret = pthread_mutex_init(&nvp6124_lock, NULL);
	assert( 0 == s32Ret);
	printf("nvp6124:video_stand=%d,g_videostd=%d\n",video_stand,g_videostd);
	printf("nvp6124:video_size=%d,g_videosize=%d\n",video_size,g_videosize);
	/*if(video_stand != g_videostd)
	{
		g_videostd = video_stand;
		SystemSetVideoMode(video_stand);
	}
	if(video_size != g_videosize)
	{
		g_videosize = video_size;
		SystemSetVideoSize(g_videosize);
	}*/
	switch(g_videostd)
	{
		case	VIDEO_STANDARD_PAL:
			nvp6124_pal_common_init();
			break;
		case	VIDEO_STANDARD_NTSC:
			nvp6124_ntsc_common_init();
			break;
		default:
			break;
	}
	switch(g_videosize)
	{
		case	CAPTURE_SIZE_D1:
			
				
					vmode.vformat[0] = g_videostd%2;
					vmode.chmode[i] = NVP6124_VI_SD;  //默认设置为1080P模式.
				
				break;
		case	CAPTURE_SIZE_720P:
				
				
					vmode.vformat[0] = g_videostd%2;
					vmode.chmode[i] = NVP6124_VI_720P_2530;  //默认设置为1080P模式.
				
				break;
		case	CAPTURE_SIZE_1080P:
				
				
					vmode.vformat[0] = g_videostd%2;
					vmode.chmode[i] = NVP6124_VI_1080P_2530;  //默认设置为1080P模式.
				
				break;
		default:
				break;
	}
	nvp6124_each_mode_setting(&vmode);
	
	//audio_init(nvp6124_slave_addr[i],16,0,0);
	return 0;
}

int nvp6124_videoGetVstd(int channel,uint dwStandard)
{
	int norm = -1;
	int i =0;
	int getvideofmt[4]={-1};
	//pthread_mutex_lock(&nvp6124_lock);
	nvp6124_init();
	
	
		GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x00);
		getvideofmt[i] = GPIO_I2C_Read0(nvp6124_slave_addr[i/4], 0xD0+i%4);
		if(getvideofmt[i] == 0x40 || getvideofmt[i] == 0x10)
		{
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x00);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0x23+4*(i%4), 0x41);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x05+i%4);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0x47, 0xee);
		}
		else
		{
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x00);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0x23+4*(i%4), 0x43);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x05+i%4);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0x47, 0x04);
		}
	//	printf("getvideofmt[%d]=%#x\n",i,getvideofmt[i]);
	
	norm=getvideofmt[channel];
	switch(norm)
	{
		case 0x01:
		case 0x04:
		case 0x10:
		case 0x40:
			norm=VIDEO_STANDARD_NTSC;
			break;
		case 0x02:
		case 0x08:
		case 0x20:
		case 0x80:
			norm=VIDEO_STANDARD_PAL;
			break;
		default:
			norm=-1;
			break;
	}
    //!norm 检测无效，使用以前那一个
    if( norm < 0 )
    {
		//norm = SystemGetVideoMode();
		enum video_standard_t videost;
		videost = VIDEO_STANDARD_PAL;
		norm = videost;//指定
      //  ERR_PRINT("\n\n===>NVP6124VideoGetVstd invalid, use old[%d]!\n\n", norm);
    }
	//pthread_mutex_unlock(&nvp6124_lock);
	return norm;	
}
unsigned char nvp6124_videoGetSize(int channel)
{
	enum capture_size_t norm = CAPTURE_SIZE_NR;
	int i =0;
	int getvideofmt[4]={-1};
	nvp6124_init();
	for(i=0; i<nvp6124_cnt*4; i++)
	{
		GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x00);
		getvideofmt[i] = GPIO_I2C_Read0(nvp6124_slave_addr[i/4], 0xD0+i%4);
		if(getvideofmt[i] == 0x40 || getvideofmt[i] == 0x10)
		{
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x00);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0x23+4*(i%4), 0x41);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x05+i%4);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0x47, 0xee);
		}
		else
		{
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x00);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0x23+4*(i%4), 0x43);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0xFF, 0x05+i%4);
			GPIO_I2C_Write0(nvp6124_slave_addr[i/4], 0x47, 0x04);
		}
	//	printf("getvideofmt[%d]=%#x\n",i,getvideofmt[i]);
	}
	norm=getvideofmt[channel];
	switch(norm)
	{
		case 0x01:
		case 0x02:
			norm=CAPTURE_SIZE_D1;
			break;
		case 0x04:
		case 0x08:
		case 0x10:
		case 0x20:
			norm=CAPTURE_SIZE_720P;
			break;
		case 0x40:
		case 0x80:
			norm=CAPTURE_SIZE_1080P;
			break;
		default:
			norm=CAPTURE_SIZE_NR;
			break;
	}
    //!norm 检测无效，使用以前那一个
    if(norm == CAPTURE_SIZE_NR)
    {
		//norm = SystemGetVideoSize();
		enum capture_size_t videosize = CAPTURE_SIZE_1080P;
      //  ERR_PRINT("\n\n===>NVP6124VideoGetSize invalid, use old[%d]!\n\n", norm);
    }
	return norm;	
}
void nvp6124_pal_common_init(void)
{
	int i = 0;

	for(i=0;i<nvp6124_cnt;i++)
	{
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x00);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B0_25P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x01);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B1_25P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x02);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B2_25P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x03);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B3_25P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x04);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B4_25P_Buf,254);
		//GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x05);
        //nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B5_25P_Buf,254);
		//GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x06);
        //nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B6_25P_Buf,254);
		//GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x07);
        //nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B7_25P_Buf,254);
		//GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x08);
        //nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B8_25P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x09);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B9_25P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x0A);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_BA_25P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x0B);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_BB_25P_Buf,254);
	}

	nvp6124_system_init();
}

void nvp6124_ntsc_common_init(void)
{
	int i = 0;

	for(i=0;i<nvp6124_cnt;i++)
	{
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x00);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B0_30P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x01);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B1_30P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x02);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B2_30P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x03);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B3_30P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x04);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B4_30P_Buf,254);
		//GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x05);
        //nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B5_30P_Buf,254);
		//GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x06);
        //nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B6_30P_Buf,254);
		//GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x07);
        //nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B7_30P_Buf,254);
		//GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x08);
        //nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B8_30P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x09);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_B9_30P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x0A);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_BA_30P_Buf,254);
		GPIO_I2C_Write0(nvp6124_slave_addr[i], 0xFF, 0x0B);
        nvp6124_write_table(nvp6124_slave_addr[i],0x00,NVP6124_BB_30P_Buf,254);
	}

	nvp6124_system_init();
}
void nvp6124_each_mode_setting(nvp6124_video_mode *pvmode )
{  
	int i;
	unsigned char tmp;
	unsigned char ch, chmode[16];
		unsigned char pn_value_sd_nt_comet[4] = {0x4D,0x0E,0x88,0x6C};
	unsigned char pn_value_720p_30[4] = 	{0xEE,0x00,0xE5,0x4E};
	unsigned char pn_value_720p_60[4] = 	{0x78,0x6E,0x7C,0x27};
	unsigned char pn_value_fhd_nt[4] = 		{0x2C,0xF0,0xCA,0x52};
	unsigned char pn_value_sd_pal_comet[4] = {0x75,0x35,0xB4,0x6C};
	unsigned char pn_value_720p_25[4] = 	{0x46,0x08,0x10,0x4F};
	unsigned char pn_value_720p_50[4] = 	{0xCD,0xBD,0x7F,0x27};
	unsigned char pn_value_fhd_pal[4] = 	{0xC8,0x7D,0xC3,0x52};
	unsigned char vformat = pvmode->vformat[0];
	
  	for(ch=0;ch<(nvp6124_cnt*1);ch++)
  	{
		chmode[ch] = pvmode->chmode[ch];
  	}
	for(i=0;i<nvp6124_cnt;i++)
  	{
		GPIO_I2C_Write0( nvp6124_slave_addr[i], 0xFF, 0);
		GPIO_I2C_Write0( nvp6124_slave_addr[i], 0x80, 0x0f);
		GPIO_I2C_Write0( nvp6124_slave_addr[i], 0xFF, 1);
		GPIO_I2C_Write0( nvp6124_slave_addr[i], 0x93, 0x80);
  	}
	printf("\n\nchmode[0] %d chmode[1] %d chmode[2] %d chmode[3] %d \n\n", chmode[0],chmode[1],chmode[2],chmode[3]);
	
	for(ch=0;ch<(nvp6124_cnt*1);ch++)
	{
		//if((chmode[ch] != ch_mode_status[ch]) && (chmode[ch] < NVP6124_VI_BUTT)) 
		if(chmode[ch] < NVP6124_VI_BUTT) 
		{
			switch(chmode[ch])
			{		
				case NVP6124_VI_SD:
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x08+ch%4, vformat==PAL?0xDD:0xA0);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x0c+ch%4, nvp6124_bri_tbl_960H[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x10+ch%4, nvp6124_con_tbl_960H[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x14+ch%4, vformat==PAL?0x80:0x80);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x18+ch%4, vformat==PAL?0x18:0x18);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x21+4*(ch%4), vformat==PAL?0x12:0x92);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x23+4*(ch%4), 0x43);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x30+ch%4, vformat==PAL?0x12:0x11);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x3c+ch%4, nvp6124_sat_tbl_960H[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x40+ch%4, nvp6124_hue_tbl_960H[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x44+ch%4, vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x48+ch%4, vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x4c+ch%4, vformat==PAL?0x04:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x50+ch%4, vformat==PAL?0x04:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x58+ch%4, vformat==PAL?0x80:0x90);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x5c+ch%4, vformat==PAL?0x1e:0x1e);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x64+ch%4, vformat==PAL?0x0d:0x08);
	 				GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x81+ch%4, vformat==PAL?0x00:0x00);	
	 				GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x85+ch%4, vformat==PAL?0x11:0x11);
	 				GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x89+ch%4, vformat==PAL?0x10:0x00);
	 				GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], ch%4+0x8E, vformat==PAL?0x08:0x07);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x93+ch%4, 0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x98+ch%4, vformat==PAL?0x07:0x04);
	 				GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xa0+ch%4, vformat==PAL?0x00:0x10);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xa4+ch%4, vformat==PAL?0x00:0x01);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x01);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x88+ch%4, vformat==PAL?0x7e:0x7e);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x8c+ch%4, vformat==PAL?0x26:0x26);
					if(chip_id[0] == NVP6124_R0_ID)
						GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xcc+ch_seq[ch%4], 0x36);
					else if(chip_id[0] == NVP6114A_R0_ID && ch_seq[ch]!=0xFF)
						GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xcd+(ch_seq[ch%4]%2)*2, 0x36);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xd7, GPIO_I2C_Read0( nvp6124_slave_addr[ch/4], 0xd7)&(~(1<<(ch%4))));
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x02);  //motion
					tmp = GPIO_I2C_Read0( nvp6124_slave_addr[ch/4], 0x16+(ch%4)/2);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x16+(ch%4)/2, (tmp&(ch%2==0?0xF0:0x0F))|(0x00<<((ch%2)*4)));
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x05+ch%4);
					nvp6124_write_table( nvp6124_slave_addr[ch/4], 0x00, vformat==PAL?NVP6124_B5_PAL_Buf:NVP6124_B5_NTSC_Buf, 254 );
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x06,0x40);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x09);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x40+(ch%4), 0x60);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x44, GPIO_I2C_Read0( nvp6124_slave_addr[ch/4], 0x44)&(~(1<<(ch%4))));
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x50+4*(ch%4),vformat==PAL?pn_value_sd_pal_comet[0]:pn_value_sd_nt_comet[0]);	//ch%41 960H	
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x51+4*(ch%4),vformat==PAL?pn_value_sd_pal_comet[1]:pn_value_sd_nt_comet[1]);		
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x52+4*(ch%4),vformat==PAL?pn_value_sd_pal_comet[2]:pn_value_sd_nt_comet[2]);		
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x53+4*(ch%4),vformat==PAL?pn_value_sd_pal_comet[3]:pn_value_sd_nt_comet[3]);
					printf("ch %d setted to SD %s\n", ch, vformat==PAL?"PAL":"NTSC");
				break;
				case NVP6124_VI_720P_2530:
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x08+ch%4,vformat==PAL?0x60:0x60);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x0c+ch%4,nvp6124_bri_tbl_720P[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x10+ch%4,nvp6124_con_tbl_720P[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x14+ch%4,vformat==PAL?0x90:0x90);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x18+ch%4,vformat==PAL?0x30:0x30);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x21+4*(ch%4), 0x92);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x22+4*(ch%4), 0x0A);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x23+4*(ch%4), 0x43);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x30+ch%4,vformat==PAL?0x12:0x12);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x3c+ch%4,nvp6124_sat_tbl_720P[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x40+ch%4,nvp6124_hue_tbl_720P[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x44+ch%4,vformat==PAL?0x30:0x30);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x48+ch%4,vformat==PAL?0x30:0x30);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x4c+ch%4,vformat==PAL?0x04:0x04);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x50+ch%4,vformat==PAL?0x04:0x04);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x58+ch%4,vformat==PAL?0x80:0x90);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x5c+ch%4,vformat==PAL?0x9e:0x9e);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x64+ch%4,vformat==PAL?0xb1:0xb2);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x81+ch%4,vformat==PAL?0x07:0x06);	
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x85+ch%4,vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x89+ch%4,vformat==PAL?0x10:0x10);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], ch%4+0x8E,vformat==PAL?0x0d:0x0d);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x93+ch%4,0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x98+ch%4,vformat==PAL?0x07:0x04);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xa0+ch%4,vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xa4+ch%4,vformat==PAL?0x00:0x01);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF,0x01);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x88+ch%4,vformat==PAL?0x5C:0x5C);	
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x8c+ch%4,vformat==PAL?0x40:0x40);
					//GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xcc+ch_seq[ch%4],vformat==PAL?0x46:0x46);
					if(chip_id[0] == NVP6124_R0_ID)
						GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xcc+ch_seq[ch%4], 0x46);
					else if(chip_id[0] == NVP6114A_R0_ID && ch_seq[ch]!=0xFF)
						GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xcd+(ch_seq[ch%4]%2)*2, 0x46);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xd7,GPIO_I2C_Read0( nvp6124_slave_addr[ch/4], 0xd7)|(1<<(ch%4)));
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x02);  //motion
					tmp = GPIO_I2C_Read0( nvp6124_slave_addr[ch/4], 0x16+(ch%4)/2);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x16+(ch%4)/2, (tmp&(ch%2==0?0xF0:0x0F))|(0x05<<((ch%2)*4)));
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x05+ch%4);
					nvp6124_write_table( nvp6124_slave_addr[ch/4], 0x00, vformat==PAL?NVP6124_B5_25P_Buf:NVP6124_B5_30P_Buf,254 );
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x01,0x0D); 
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x06,0x40); 
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x2B,0x78); 
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x59,0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x58,0x13);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xC0,0x16);
					//eq_init for 720P_2530
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xD8,0x0C);    
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xD9,0x0E); 
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDA,0x12);    
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDB,0x14); 
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDC,0x1C);    
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDD,0x2C);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDE,0x34);
					
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x09);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x40+(ch%4),0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x44, GPIO_I2C_Read0( nvp6124_slave_addr[ch/4], 0x44)|(1<<(ch%4)));
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x50+4*(ch%4),vformat==PAL?pn_value_720p_25[0]:pn_value_720p_30[0]);	//ch%41 960H	
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x51+4*(ch%4),vformat==PAL?pn_value_720p_25[1]:pn_value_720p_30[1]);		
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x52+4*(ch%4),vformat==PAL?pn_value_720p_25[2]:pn_value_720p_30[2]);		
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x53+4*(ch%4),vformat==PAL?pn_value_720p_25[3]:pn_value_720p_30[3]);
					printf("ch %d setted to 720P %s\n", ch, vformat==PAL?"PAL":"NTSC");
				break;
				case NVP6124_VI_1080P_2530:
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x08+ch%4,vformat==PAL?0x60:0x60);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x0c+ch%4,nvp6124_bri_tbl[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x10+ch%4,nvp6124_con_tbl[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x14+ch%4,vformat==PAL?0x90:0x90);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x18+ch%4,vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x21+4*(ch%4), 0x92);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x22+4*(ch%4), 0x0A);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x23+4*(ch%4), vformat==PAL?0x43:0x43);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x30+ch%4,vformat==PAL?0x12:0x12);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x3c+ch%4,nvp6124_sat_tbl[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x40+ch%4,nvp6124_hue_tbl[vformat%2]);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x44+ch%4,vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x48+ch%4,vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x4c+ch%4,vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x50+ch%4,vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x58+ch%4,vformat==PAL?0x6a:0x49);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x5c+ch%4,vformat==PAL?0x9e:0x9e);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x64+ch%4,vformat==PAL?0xbf:0x8d);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x81+ch%4,vformat==PAL?0x03:0x02);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x85+ch%4,vformat==PAL?0x00:0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x89+ch%4,vformat==PAL?0x10:0x10);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], ch%4+0x8E,vformat==PAL?0x0a:0x09);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x93+ch%4,0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x98+ch%4,vformat==PAL?0x07:0x04);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xa0+ch%4,vformat==PAL?0x00:0x00);	
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xa4+ch%4,vformat==PAL?0x00:0x01);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x01);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x88+ch%4,vformat==PAL?0x4c:0x4c);	
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x8c+ch%4,vformat==PAL?0x84:0x84);							 		
					//GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xcc+ch_seq[ch%4],vformat==PAL?0x66:0x66);
					if(chip_id[0] == NVP6124_R0_ID)
						GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xcc+ch_seq[ch%4], 0x66);
					else if(chip_id[0] == NVP6114A_R0_ID && ch_seq[ch]!=0xFF)
						GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xcd+(ch_seq[ch%4]%2)*2, 0x66);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xd7, GPIO_I2C_Read0( nvp6124_slave_addr[ch/4], 0xd7)|(1<<(ch%4)));
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x02);  //motion
					tmp = GPIO_I2C_Read0( nvp6124_slave_addr[ch/4], 0x16+(ch%4)/2);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x16+(ch%4)/2, (tmp&(ch%2==0?0xF0:0x0F))|(0x05<<((ch%2)*4)));
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x05+ch%4);
					nvp6124_write_table( nvp6124_slave_addr[ch/4], 0x00, vformat==PAL?NVP6124_B5_25P_Buf:NVP6124_B5_30P_Buf,254 );
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x01,0x0C);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x06,0x40);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x2A,0x72);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x2B,0xA8); 
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x58,0x13);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x59,0x01);
					//eq_init for 1080P_2530
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xD8,0x10);    
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xD9,0x1F); 
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDA,0x2B);    
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDB,0x7F); 
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDC,0xFF);    
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDD,0xFF);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xDE,0xFF);
					
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x24,vformat==PAL?0x2A:0x1A);  //sync changed
					//GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x47,vformat==PAL?0x04:0xEE); 
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x50,vformat==PAL?0x84:0x86);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xBB,vformat==PAL?0x00:0xE4);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0xFF, 0x09);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x40+(ch%4), 0x00);
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x44,GPIO_I2C_Read0( nvp6124_slave_addr[ch/4], 0x44)|(1<<(ch%4)));
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x50+4*(ch%4),vformat==PAL?pn_value_fhd_pal[0]:pn_value_fhd_nt[0]);	//ch%41 960H	
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x51+4*(ch%4),vformat==PAL?pn_value_fhd_pal[1]:pn_value_fhd_nt[1]);		
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x52+4*(ch%4),vformat==PAL?pn_value_fhd_pal[2]:pn_value_fhd_nt[2]);		
					GPIO_I2C_Write0( nvp6124_slave_addr[ch/4], 0x53+4*(ch%4),vformat==PAL?pn_value_fhd_pal[3]:pn_value_fhd_nt[3]);
					printf("ch %d setted to 1080P %s\n", ch, vformat==PAL?"PAL":"NTSC");
				break;
				default:
					printf("ch%d wrong mode detected!!!\n", ch);
					break;
			}
			
			ch_mode_status[ch] = chmode[ch];
		}	
	}
}

