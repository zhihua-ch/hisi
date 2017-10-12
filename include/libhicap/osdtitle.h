
#ifndef __OSDTITLE_H__
#define __OSDTITLE_H__

#include "hi_comm_region.h"
#include "System.h"
#include "Capture.h"
/*
��   R:255 G:0   B:0
��� R:242 G:101 B:34
�Ƴ� R:247 G:148 B:29
��   R:255 G:255 B:0
��   R:0   G:255 B:0
��   R:255 G:255 B:255
��   R:255 G:0   B:255
��   R:255 G:0   B:255
��   R:255 G:255 B:255
��   R:0   G:0   B:0  
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
   OSD_OVERLAY_STATE_CHANGED,         //OSD������Ҫ����
   OSD_OVERLAY_STATE_NEED_DESTROY     //OSD������Ҫ�ؽ�
}OSD_OVERLAY_STATE;

typedef struct tagOSD_OVERLAY_PARAM
{
    int                     changed;         
	RGN_HANDLE           osdHandle;       //��Ӧ��������
	RGN_HANDLE           ExtosdHandle;       //��չ��������
	RGN_HANDLE           SnapOsdHandle;   //��Ӧץͼ����
	CAPTURE_TITLE_PARAM     Param;           //osd���ӹ��ã���Ҫ�ṩ���ӣ����ݵ���Ϣ(û��,��ʱ����)
	BUFFER_S	            StBufRGB;        //���ݱ���
	BUFFER_S                StBufFont;       //����
	OSD_TIME	            osdTime;         //ͨ��ʱ�����͸�ʽ
//	REGION_CTRL_PARAM_U     *osdMarker;      //ˮӡ,��ʱû���õ�
	ubool					bUsedVi;         //!ֵΪ1-������vi, 0-������venc
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

//ע�⣬ʱ�������Ҫ�ַ�ת���󣬶�ͨ���������������SOFIA�����
int Osd_SetTime(int channel, OSD_TYPE_E type, const SYSTEM_TIME * pTime, unsigned char bDbg);
int Osd_SetTitle(int channel, OSD_TYPE_E type, CAPTURE_TITLE_PARAM * pParam);
//void Osd_PrintRegion(REGION_ATTR_S* pstRgnAttr);
void Osd_FontBgColorInit();
short ConvertRGB32TORGB1555(int rgb);

#endif	

