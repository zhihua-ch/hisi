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
#include "dvr/i2c/i2c.h"
#include "dvr/sysreg/sys_reg_public.h"
#include "initial.h"
#include "nvp6124_reg.h"
#include "power_control_public.h"

extern enum video_standard_t  g_videostd;
extern enum capture_size_t g_videosize;
static unsigned int nvp6124_slave_addr[4] = {0x60, 0x62, 0x64, 0x66};

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
	audio_source_control_oper(AHD_MODE);
	//获取视频制式
	video_stand = nvp6124_videoGetVstd(0,0);
	video_size = nvp6124_videoGetSize(0);
	
	//s32Ret = pthread_mutex_init(&nvp6124_lock, NULL);
	assert( 0 == s32Ret);
	printf("nvp6124:video_stand=%d,g_videostd=%d\n",video_stand,g_videostd);
	printf("nvp6124:video_size=%d,g_videosize=%d\n",video_size,g_videosize);
	if(video_stand != g_videostd)
	{
		g_videostd = video_stand;
		SystemSetVideoMode(video_stand);
	}
	if(video_size != g_videosize)
	{
		g_videosize = video_size;
		SystemSetVideoSize(g_videosize);
	}
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
	
	audio_init(nvp6124_slave_addr[i],16,0,0);
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
        norm = SystemGetVideoMode();
      //  ERR_PRINT("\n\n===>NVP6124VideoGetVstd invalid, use old[%d]!\n\n", norm);
    }
	//pthread_mutex_unlock(&nvp6124_lock);
	return norm;	
}
