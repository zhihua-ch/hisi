
#ifndef __VIDEOENC__H__
#define __VIDEOENC__H__

#include "DVRINC.h"

/**********Video Encode Start**************************/
typedef struct tagVENC_CHN_INFO
{
	VENC_CHN vencChn;	//!ָʾ�ı���ͨ����0--64
	int created;
	int started;
	int enable;	//!ָʾ��Ƶʹ��
	int pic_width;
	int pic_height;
	int frame_rate;
	int image_size;
	int venc_fd;
	int gop;
	int bitrate;
	int cbrvbr;
	int preframe;
	int cur_frame_rate;
	int frame_count;
	int error_count;
	unsigned long long oldpts;
	unsigned long long scr;
	pthread_t threadid;
	pthread_mutex_t lock;
	struct timeval oldtime;
	CAPTURE_OSD_ZOOM osd_venc_zoom;
	int iSelected; 
	//��ʾ��ǰʹ�ô�ͨ���������� ��ѡֵ 0: CHL_MAIN_T   1:CHL_2END_T  5 :CHL_FUNCTION_NUM ��
} VENC_CHN_INFO_t;

typedef struct tagVENC_JPEG_CHN
{
	int created;
	int started;
	int enable;
	int QVal;	//!Ĭ��ץ������Ϊ���
	//int Period;//��λΪ���룬Ĭ��ץ�ļ��1s
	//!TODO:����ץͼ��С����С�ڱ���ͼƬ
	//struct timeval SnapOldTime;
	int nFps;//֡��ֵ��NTSC/PAL������,������ʾ����һ֡,0-��ʾȫ��ץͼ
	int nFlage;
	HI_U64 last_frame_time; //!��������һ֡��ʱ��,us
	int venc_fd;
	pthread_t threadid;
	pthread_mutex_t lock;	
} VENC_JPEG_CHN_T;

typedef struct tagVENC_CHN
{
	int Chn;//!��Ӧdvr���߼�ͨ����,0-31
	VENC_CHN_INFO_t venc_chn[CHL_FUNCTION_NUM];
	VENC_JPEG_CHN_T venc_jpeg; 
} VENC_CHN_T;

void	VEncChn_Init_General(VENC_CHN_T* vchn, int chn);
void 	VEncChn_Init(VENC_CHN_T* vchn, int chn);
void 	VEncChn_Exit(VENC_CHN_T* vchn);

//!bLock-Ϊ��ʱ���̰߳�ȫ����
int 	VEncChn_Create(VENC_CHN_T* vchn, enum capture_channel_t type, VENC_CHN_ATTR_S* attr );
int 	VEncChn_Destory(VENC_CHN_T* vchn, enum capture_channel_t type);

int 	VEncChn_GetAttr(VENC_CHN_T* vchn, enum capture_channel_t type, VENC_CHN_ATTR_S* attr);
int 	VEncChn_SetAttr(VENC_CHN_T* vchn, enum capture_channel_t type, VENC_CHN_ATTR_S* attr);

int 	VEncChn_Start(VENC_CHN_T* vchn, enum capture_channel_t type);
int 	VEncChn_Stop(VENC_CHN_T* vchn, enum capture_channel_t type);

int 	VEncChn_ForceIFrame(VENC_CHN_T* vchn, enum capture_channel_t type);
void 	VEncChn_Print(VENC_CHN_T* vchn, VENC_CHN_ATTR_S* pttr);

int		VEncChn_SetSnapParam(VENC_CHN_T* vchn, char imgq, int nFps);

/****************Video Encode end**********************************/

#endif
