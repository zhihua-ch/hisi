#ifndef __VIDEO_NVP6124_H__
#define __VIDEO_NVP6124_H__
#include "Video.h"
#include "LossDetect.h"

enum
{
	NVP6124_VI_SD = 0,
	NVP6124_VI_720P_2530,
	NVP6124_VI_720P_5060,
	NVP6124_VI_1080P_2530,
	NVP6124_VI_BUTT
};
typedef struct _nvp6124_video_mode
{
    unsigned int chip;
    unsigned int mode;
	unsigned char vformat[16];
	unsigned char chmode[16];
}nvp6124_video_mode;
void nvp6124_pal_common_init(void);
void nvp6124_ntsc_common_init(void);
void nvp6124_each_mode_setting(nvp6124_video_mode *pvmode );

int nvp6124_videoCreate(void);
int nvp6124_videoDestory(void);
int nvp6124_videoSetCover(int channel, int index, VIDEO_COVER_PARAM *pParam);

int nvp6124_videoSetVstd(int channel,uint dwStandard);
int nvp6124_videoWriteRegister(unsigned char Chip, unsigned char Page, 
	                           unsigned char Register, unsigned char Data);
int nvp6124_videoReadRegister(unsigned char Chip, unsigned char Page, 
	                          unsigned char Register);
int nvp6124_videoSwitchVGAMonitor(VD_BOOL open);
int nvp6124_videoGetCaps(VIDEO_CAPS * pCaps);
int nvp6124_videoSetBkColor(uint color);
int nvp6124_videoRecordSetSize(int channel, int size);
int nvp6124_videoAutoDetectStart(void);
int nvp6124_videoAutoDetectStop(void);
int nvp6124_videoGetVstd(int channel,uint dwStandard);
unsigned char nvp6124_videoGetSize(int channel);
int nvp6124_videoSetColor(int channel, VIDEO_COLOR * pColor);
int nvp6124_lossDetectCreate(void);
int nvp6124_lossDetectDestory(void);
int nvp6124_lossDetectGetCaps(LOSS_DETECT_CAPS * pCaps);
int nvp6124_lossDetectGetState(uint* pData);
int nvp6124_audioIn_SetVolume(int value);
int nvp6124_audioOut_SetVolume(int value);
int nvp6124_videoSetCropRegion(int channel,int Hoffset,
	                           int Voffset,int Width,int Height);
void nvp6124_outport_disable(void);
#endif

