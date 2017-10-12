
#ifndef __OSDTITLE_H__
#define __OSDTITLE_H__

#include "hi_comm_region.h"
#include "System.h"
#include "Capture.h"
/*
红   R:255 G:0   B:0
红橙 R:242 G:101 B:34
黄橙 R:247 G:148 B:29
黄   R:255 G:255 B:0
绿   R:0   G:255 B:0
青   R:255 G:255 B:255
蓝   R:255 G:0   B:255
紫   R:255 G:0   B:255
白   R:255 G:255 B:255
黑   R:0   G:0   B:0  
*/

#define NUMBER_DIGIT(num, bit) (((num)/(bit))%10)
#define PCIV_OSD_TITLE_PKT_LEN	(PCIV_MSG_MAXLEN - sizeof(CAPTURE_TITLE_PARAM) - 16)

typedef struct tagOSD_TIME
{
	SYSTEM_TIME sysTime;
	int         dateFmt;
	int         dateSpec;
	int         timeFmt;
} OSD_TIME;

typedef struct hiBUFFER_S
{
	char *pAddr;
	int   nSize;
} BUFFER_S;

typedef enum
{
   OSD_OVERLAY_STATE_NORMAL = 0,      //
   OSD_OVERLAY_STATE_CHANGED,         //OSD区域需要更新
   OSD_OVERLAY_STATE_NEED_DESTROY     //OSD区域需要重建
}OSD_OVERLAY_STATE;

typedef struct tagOSD_OVERLAY_PARAM
{
    int                     changed;         
	RGN_HANDLE           osdHandle;       //对应编码码流
	RGN_HANDLE           ExtosdHandle;       //扩展编码码流
	RGN_HANDLE           SnapOsdHandle;   //对应抓图码流
	CAPTURE_TITLE_PARAM     Param;           //osd叠加共用，主要提供叠加，数据等信息(没用,暂时保留)
	BUFFER_S	            StBufRGB;        //字休背景
	BUFFER_S                StBufFont;       //字体
	OSD_TIME	            osdTime;         //通道时间标题和格式
//	REGION_CTRL_PARAM_U     *osdMarker;      //水印,暂时没有用到
	ubool					bUsedVi;         //!值为1-叠加在vi, 0-叠加在venc
	pthread_mutex_t lock;
	int time_hide;
	int date_hide;
}OSD_OVERLAY_PARAM;

void InitOSD();
void FlushChnOSD( int channel );
int Osd_SetTimeFmt(int channel, const SYSTEM_TIME * pTime, int datefmt,int datespc,int timefmt);
int Osd_SetTimeFmt2(int channel, const SYSTEM_TIME * pTime, int datefmt,int datespc,int timefmt,int time_hide,int date_hide);

//int unSetSnapTitleTime(int chn,int Grp);
int SetSnapTitleTime(int chn);

//注意，时间标题需要字符转点阵，而通道标题这个工作在SOFIA中完成
int Osd_SetTime(int channel, OSD_TYPE_E type, const SYSTEM_TIME * pTime, unsigned char bDbg);
int Osd_SetTitle(int channel, OSD_TYPE_E type, CAPTURE_TITLE_PARAM * pParam);
//void Osd_PrintRegion(REGION_ATTR_S* pstRgnAttr);
void Osd_FontBgColorInit();
short ConvertRGB32TORGB1555(int rgb);

#endif	

