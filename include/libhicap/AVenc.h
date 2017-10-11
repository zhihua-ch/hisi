
#ifndef __AVENC_H__
#define __AVENC_H__

#include "DVRINC.h"
#include "Memory.h"

#include "Video.h"
#include "Capture.h"
#include "CaptureList.h"
#include "AudioEnc.h"
#include "VideoEnc.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define M_Dbg_Venc	0

//#define IEplayBitRate 400

typedef struct tagCapture_Dev Capture_Dev_t;

typedef int (*PF_Save_CaptureData)(Capture_Dev_t* pThis, int remote_chip, int chn,
		int stream, void *pStream) ;

typedef struct tagCOVER_INFO
{
	RGN_HANDLE handle[4];
	VIDEO_COVER_PARAM Param[4];
}COVER_INFO_t;

typedef struct tagVI_CHN_INFO
{
	uchar bDiscardMainVencFlag;	//!�Ƿ�����������־, Ϊ1����, �ӵײ��ȡ�������������϶�����0-��������
	VENC_CHN_T 		venc_chn_t;
	AENC_CHN_INFO_t aenc_chn;
	COVER_INFO_t cover;
#if defined (_USE_720P_MODULE_) 	
#define M_BLOCK_LIST_MAXSIZE (2*1024*1024)
#else
#define M_BLOCK_LIST_MAXSIZE (4*1024*1024)//1080Pģʽ��BUF�ܿ�����ˣ���ʱ�Ĵ�yangsc@1015.11.06
#endif
	BLOCK_LIST list[CHL_FUNCTION_NUM];	//!�ֱ��Ӧ����������͸���������
	uint getcount[CHL_FUNCTION_NUM];
} VI_CHN_INFO_t;

typedef struct tagUPDATE_VBR
{
	int flag[N_SYS_CH];
	int curbitrate[N_SYS_CH];
	int ConnectCount[N_SYS_CH];
	pthread_mutex_t lock;
}UPDATE_VBR_T;

typedef struct tagDEV_SNAP
{
	//8·�豸ʹ��16-24��Ϊץ��ͨ��
	int chn_start;//!ץ��ͨ����ʼͨ����
	int QVal[8];	//!Ĭ��ץ������Ϊ���
	char Flag[8];
	int Period;//��λΪ���룬Ĭ��ץ�ļ��1s
	struct timeval SnapOldTime[8];
}DEV_SNAP_T; 

struct tagCapture_Dev
{
	int sys_init;

	// ��Ƶ�������Ƿ�򿪣�0Ϊ�رգ�1Ϊ��
	int venc_opened;
	// ��Ƶ�������Ƿ�򿪣�0Ϊ�رգ�1Ϊ��
	int aenc_opened;					

	// ��Ƶ������ʽ
	int video_codec_format;
	// ��Ƶ������ʽ
	int audio_codec_format;			
	time_t basetime;					
	VI_CHN_INFO_t  vi_chn[N_SYS_CH];
	//BLOCK_LIST list[N_SYS_CH*2];//!ͨ�����ݻ�������,��Ӧ��Ӧ�� 2*�߼�ͨ����+��������
	pthread_mutex_t lock;
	fd_set read_fds;
	HI_S32 MaxVencFd;

	PF_Save_CaptureData pVenc_PF;
	PF_Save_CaptureData pAenc_PF;
	PF_Save_CaptureData pSnap_PF;

	//!����pidȫ��ʼ��Ϊ0
	pthread_t pid_vencode;
	pthread_t pid_ven_recv[PCIV_MAX_CHIPNUM];
	pthread_t pid_ven_send;
	pthread_t pid_venc_snap;

	pthread_t pid_aencode;//!����ȡ��Ƶ�����߳�
};

typedef enum 
{
	VI_INTERFACE_0,//hisi3512 only 0.2
	VI_INTERFACE_1,
	VI_INTERFACE_2,
	VI_INTERFACE_3,
	VI_INTERFACE_MAX,
}VI_INTERFACE_E;


typedef enum
{
	VI_CHANNEL_0,
	VI_CHANNEL_1,
	VI_CHANNEL_MAX,
}VI_CHANNEL_E;

enum capture_faile {
	CRTGRP_F = 1,		/*CreatGroup faile*/
	CRTCHN_F = 2,		/*Creatchn faile*/
	BINDGRP_F = 3,		/*BindInput faile*/
	RGTCHN_F = 4,		/*RegisterChn faile*/
	FAILE_FUNCTION_NUM
};



/*typedef struct tagVD_PCIVStreamBuf
{
    HI_U8   *pu8Addr[8];
    HI_U32   u32Len[8];
    int 	count;	//!����������
} VD_PCIVStreamBuf_T;
int PCIV_SendStreamDma(int remote_chip, PCIV_STREAM_HEAD_S* pstHead, VD_PCIVStreamBuf_T* pBuf );
*/
Capture_Dev_t* CaptureDevGetIns();
int VencGetThread(void *arg);

//!TODO: ��Ƶ�Խ�ҲҪ����Capture_Dev_t�豸����
//!�˽ṹ��Ӧ�ù������к�˼���豸
void CaptureDevInit(Capture_Dev_t* pThis, VIDEO_NORM_E vstd);
void CaptureDevExit(Capture_Dev_t* pThis);

/**
 * Capture_Dev_t ��Ա��������
 */
int SnapGetStream(void *arg);
int AencGetStream(Capture_Dev_t* pThis);
//!�Ӻ�˼����ȡ�������������뻺�������
int VencGetStream(Capture_Dev_t* pThis );
int FormStandHead(Capture_Dev_t* pThis, int vi_chn, enum capture_channel_t dwType,
		uchar *buf, int framelen, int headtype);
int SnapCopy2Buf(Capture_Dev_t* pThis, int SnapChn, VENC_STREAM_S *pvencStream);
int VencCopy2Buf(Capture_Dev_t* pThis, int vchn, enum capture_channel_t vstream,
		VENC_STREAM_S *pvencStream);
int SlaveVencCopy2Buf(Capture_Dev_t* pThis, int chn, int stream, HI_U8 *pvencStream,
		HI_U32 encodeFrameLen, uchar* HeadArray,uchar HeadLen, uchar HeadType);
int AencCopy2Buf(Capture_Dev_t* pThis, int aenc_chn, enum capture_channel_t stream,
		AUDIO_STREAM_S *paencStream);
int CaptureDev_SaveVenc2List(Capture_Dev_t* pThis, int remote_chip, int vchn,
		int stream, void *pStream);
int CaptureDev_SaveVenc2Remote(Capture_Dev_t* pThis, int remote_chip, int vchn,
		int stream, void *pStream);

int CaptureDev_SaveAenc2List(Capture_Dev_t* pThis, int remote_chip, int achn,
		int stream, void *pStream);
int CaptureDev_SaveAenc2Remote(Capture_Dev_t* pThis, int remote_chip, int achn,
		int stream, void *pStream);

int CaptureDev_SaveSnap2List(Capture_Dev_t* pThis, int remote_chip, int vchn,
		int stream, void *pStream);
int CaptureDev_SaveSnap2Remote(Capture_Dev_t* pThis, int remote_chip, int vchn,
		int stream, void *pStream);

//int SnapChnCreat(Capture_Dev_t* pThis, int channel);
//int SnapChnDestroy(Capture_Dev_t* pThis, int channel);

int Venc_chn_config(int channel, uint dwType, CAPTURE_FORMAT *pFormat,VENC_CHN_ATTR_S *stAttr);
/*HI_S32 PCIV_Stream_SaveStream(HI_S32 s32RmtChipId, PCIV_STREAM_TYPE_E enStreamType,
		const PCIV_NOTIFY_STREAM_S *pStreamInfo);*/

int IS_EXT_TalkChannel(int channel);
void AudioStreamProc(Capture_Dev_t* pThis, int chn, AUDIO_STREAM_S* stStream);

#ifdef __cplusplus
}
#endif

#endif	





















