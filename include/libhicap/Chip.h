
#ifndef __VD_CHIP_H__
#define __VD_CHIP_H__

#include "DVRINC.H"
#include "hi_comm_pciv.h"
#include "Capture.h"
#include "hi_comm_pciv.h"
//#include "Split.h"


/***************chip manager start*******************/
typedef enum
{
	CHIP_SINGLE,	//!��Ƭ
	CHIP_MASTER,	//!��Ƭ
	CHIP_SLAVE,		//!��Ƭ
	CHIP_AUTO,	//!�Զ����
}CHIP_MODE;

typedef struct tagCHIP_CHN_LIST
{
	int chip;//!оƬ��
	int chn[N_SYS_CH];//!оƬ��ӵ�е�ͨ���б�ֵ�����Ӧ��ͨ����
	int chn_num;//!оƬӵ�е�ͨ����Ŀ
}CHIP_CHN_LIST_T;

typedef struct tagCHIP_MANAGER
{
	unsigned char m_bSysInit;

	int LocalChipID;

	int m_RemoteChipID[PCIV_MAX_CHIPNUM];
	PCIV_BASEWINDOW_S m_u32PfWinBase[PCIV_MAX_CHIPNUM];
	CHIP_CHN_LIST_T m_ChipChn[PCIV_MAX_CHIPNUM];//!��n��оƬ��ͨ����
	int m_ChipNum;
	CHIP_MODE m_mode;
}CHIP_MANAGER_T;

typedef struct tagPCIV_MSG_CHIP
{
	int chip_num;
	int bDvr;//�Ƿ��ǻ��DVR
	int iVoVideoOutType;
	int iProductType;
	int iVideoStd;
	int ret;
} PCIV_MSG_CHIP_T;

//!��ʼ��оƬ�����Ϣ
//!��ʼmcc��أ��Թ�������ϢͨѶ
void Chip_Init(CHIP_MANAGER_T* pThis);
void Chip_Exit(CHIP_MANAGER_T* pThis);
CHIP_MANAGER_T* Chip_GetIns();

//!ȡ��оƬ����ģʽ
CHIP_MODE Chip_GetMode(CHIP_MANAGER_T* pThis);
int Chip_GetLocalID(CHIP_MANAGER_T* pThis);
CHIP_CHN_LIST_T* Chip_GetChnList(CHIP_MANAGER_T* pThis, int chip);

HI_U32 GetChipPfWinBase(CHIP_MANAGER_T* pThis, int chip_id);

//!chip debug function
void Chip_AssertID( CHIP_MANAGER_T* pThis, char* file, int line, char* func );
#define Chip_AssertID_M() Chip_AssertID( Chip_GetIns(), __FILE__, __LINE__, __FUNCTION__)

/***************chip manager end*******************/


/***********************************************************************
 * �߼�dvrͨ��
 *********************************************************************/

typedef struct tagDVR_CHN
{
	int 		dvr_chn; 		//!dvr�߼�ͨ����
	int 		chip;			//!����оƬ��

	VI_DEV 		vi_dev;			//!������Ƶ�豸��
	VI_CHN  	vi_chn;			//!������Ƶ����ͨ����
	VPSS_GRP 	vpss_grp;
	VPSS_GRP 	vpss_jpeg_grp;
	VPSS_GRP 	vpss_jpeg_manual_grp;
	VPSS_GRP 	vpss_vdec_grp;
	VPSS_GRP	vpss_hd_clip_grp;
	VENC_GRP	venc_grp[CHL_FUNCTION_NUM];	//����ͨ����
	VENC_CHN	venc_chn[CHL_FUNCTION_NUM];		//!����ͨ����, ÿ������һ��
	RGN_HANDLE	osd_handle[CHL_FUNCTION_NUM];

	VO_DEV		vo_dev;			//!��Ƶ����豸��
	VO_GRP		vo_grp;			//!��Ƶ������
	VO_CHN		vo_chn;			//!��Ƶ���ͨ����
	VO_CHN		Vdec_vo_chn;
	AUDIO_DEV	ai_dev;			//!������Ƶ�豸��
	AI_CHN		ai_chn;			//!������Ƶ����ͨ����
	AENC_CHN	aenc_chn;		//!��Ƶ����ͨ����
	AUDIO_DEV	ao_dev;			//!��Ƶ���ͨ����
	AO_CHN		ao_chn;			//!��Ƶ���ͨ����
	ADEC_CHN    adec_chn;
}DVR_CHN_T;

typedef int LOGIC_VDEC_CHN;
typedef struct tagLOGIC_CHN_VDEC
{
	LOGIC_VDEC_CHN logic_chn_vdec;		//!�߼�����ͨ����
	int chip;				//!����оƬ��
	VDEC_CHN vdec_chn;		//!����ͨ����
	int pic_width;			//!����֧�ֵ����ͼ���
	int pic_height;			//!����֧�ֵ����ͼ���
}LOGIC_CHN_VDEC_T;

typedef struct tagDVR_CHN_M
{
	unsigned char m_bSysInit;
	DVR_CHN_T chn[N_SYS_CH]; //!��Ӧ��ͨ���ź���������Ӧ
	LOGIC_CHN_VDEC_T vdec_chn[N_SYS_CH];
	//SPLIT_CAPS	split_caps; 2017.10.10
	//!�˲�Ʒ�Ƿ�ʹ���˿������ù��ܣ��������ã�ͨ�ò�Ʒ��һЩ��������ѡ���
	//!һ���ط��������ã� ������Һ�ά��, �Ӵ˱�־��Ϊ��֤����ǰ�ķ�ɢʽ����
	//!���ݹ���������Ӱ��
	int 	bQuickConfig;
	int     alarm_in_num;//!�����������
	int     alarm_out_num;//�����������
}DVR_CHN_M_T;

DVR_CHN_M_T* DCM_GetIns();
void DCM_Init(DVR_CHN_M_T* pThis,CHIP_MANAGER_T* pchipIns);
void DCM_Exit(DVR_CHN_M_T* pThis);


#endif
