#ifndef __VD_INITIAL_H__
#define __VD_INITIAL_H__

#include "DVRINC.H"
#include "Capture.h"
#include "Chip.h"
//#include "CSplit.h"
//!������ɽ����������ƣ�������Ʒ�����
//#define OEMZSLB 
#define NEW_TV_AGJEST
/*
��Ʒid���һ���ֽ�����Ϊö��,ͬ����������
	
*/
typedef enum
{
//--------------4Ƭ��Ʒ��
	ENUM_CHIP_4_START = 0x0,
	ENUM_PRODUCT_TYPE_16 = 0x1,	///< 16·DVR <VR9816SDS-2U=80000001>
	ENUM_PRODUCT_TYPE_16_4H = 0x2,   ///<1· UXGA +  3·720p+4D1����16+8 ���HDVR <VR9816SD-2U=80000002>
	ENUM_PRODUCT_TYPE_16_8	= 0x3,	///< 16+8HDVR,��ʱû��ʹ��
	ENUM_PRODUCT_TYPE_16_I = 0x4,      ///< ����IDVR <VR9816SDI-2U=80000004>
	ENUM_PRODUCT_TYPE_16_M = 0x5,     ///< ����,���16+8HDVR <VR9816SDL-2U=80000005>
	ENUM_PRODUCT_TYPE_NVR24_3520_4_chip = 0x6, //!24·D1 NVR <VR9824TDN-2U=80000006>
	ENUM_PRODUCT_TYPE_NVR8_3520_4_chip = 0x7, //!8·720P NVR <VR9808EHN-2U(4)=80000007>
	ENUM_PRODUCT_TYPE_SDI_3520_4Chip = 0x8, //!sdi���룬4·1080p����
	ENUM_CHIP_4_END	,
//--------------end
//--------------2Ƭ��Ʒ��
	ENUM_CHIP_2_START = 0x20,
	ENUM_PRODUCT_TYPE_8	=0x21,	///< 8·DVR <VR9808EDE-2U=80000021 VR9808EDE-1H=80000021>
	ENUM_PRODUCT_TYPE_8_2H =0x22,	///< 1· UXGA + 1·720p+ 2D1����8+4 ���HDVR <VR9808ED-2U=80000022 VR9808ED-1H=80000022>
	ENUM_PRODUCT_TYPE_4_1VGA = 0x23,	///< 4+1 (��Ƭ4D1+��Ƭ1VGA)VGA��DVR <VR9804FDH-2U=80000023 >
	ENUM_PRODUCT_TYPE_4_1VGA_1	=0x24,	///< 4+1(��Ƭ���л�VGA����Ƭ4D1+1VGA�ɼ���D1����)VGA��
	ENUM_PRODUCT_TYPE_8_4=0x25, ///< 8+4HDVR ,��ʱû��ʹ��
	ENUM_PRODUCT_TYPE_9816SDA_1H=0x26,	///< 3520ƽ̨16·(16D1,1D1�ط�) DVR <VR9816SDA-1H=80000026>
	ENUM_PRODUCT_TYPE_8_M = 0x27,     ///< ����,���8+4HDVR,<VR9808EDL-2U=80000027>
	ENUM_PRODUCT_TYPE_NVR_720p_D4P4_3520_2 = 0x28,	//!4·720p����, ֧��4·�ط�, 2Ƭ3520 NVR<VR9804FHN-2U(2)=80000028>
	ENUM_PRODUCT_TYPE_NVR_720p_D8P4_3520_2 = 0x29,	//!8·720p����, ֧��4·�ط�, 2Ƭ3520 NVR<VR9808EHN-2U(2)=80000029>

	//9816SDA-1H����NVR
	ENUM_PRODUCT_TYPE_NVR_9816SDN_1H = 0x2A,//16·D1����9816SDA���죬2Ƭ3520 NVR <VR9816SDN-1H =8000002A>
	ENUM_PRODUCT_TYPE_NVR_9808EHN_1H = 0x2B,//8·720P����9816SDA���죬2Ƭ3520 NVR <VR9808EHN-1H =8000002B>
	ENUM_PRODUCT_TYPE_VD9809HDN_1H = 0x2C,//9·720P����9816SDA���죬2Ƭ3520 ������ <VD9809HDN-1H=8000002C>

	ENUM_PRODUCT_TYPE_NVR_720p_D16P4_3520_2 = 0x30, //!16·D1����, ֧��4·�طţ� 2Ƭ3520 NVR <VR9816SDN-2U(2)=80000030>
	ENUM_PRODUCT_TYPE_9808EHA_1H=0x31,	///< 3520ƽ̨4D1����+4 720P����(4 720 P�ط�) HVR <VR9808EHA-1H=80000031>
	ENUM_PRODUCT_TYPE_NVR_SVAC_8902=0x32,	///<svac 2Ƭ3520 2·����VD8902 = 80000032>,������svac ��֧ʵ��
	ENUM_CHIP_2_END,
//--------------end
//---------------1Ƭ��Ʒ��
	ENUM_CHIP_1_START = 0x40,
	
	/* 3520 1�� 1U 3���ͺ�*/
	ENUM_PRODUCT_TYPE_981U=0x41,///< 3520ƽ̨16·16CIF,16�ط�) DVR
	ENUM_PRODUCT_TYPE_3520_9716SC_1H=0x42,	///< 3520ƽ̨16·(4D1+12CIF,4D1�ط�) DVR <VR9716SC-1H=80000042>
	ENUM_PRODUCT_TYPE_3520_9808EDA_1H=0x43,		///< 3520ƽ̨8·(8D1+4D1�ط�)DVR <VR9808EDA-1H=80000043>
	
	ENUM_PRODUCT_TYPE_3515_9704FC_1U=0x44,		///< 3515ƽ̨4·(1D1+3CIF,4�ط�)DVR <VR9704FC-01F=81000044,VR9804FD-01F=81000044>
	ENUM_PRODUCT_TYPE_3515_9708FC_01F=0x45,		///< 3515ƽ̨8·(1D1+7CIF)DVR <VR9708EC-01F=81000045>
	ENUM_PRODUCT_TYPE_3515_9716SC_01F=0x46,		///< 3515ƽ̨16·(16CIF)DVR	<VR9716SC-1U=81000046>

	/* ��ȡ3520 2Ƭ�����壬ʵ��Ӳ����ֻ����1Ƭ3520, 1.5U��2���ͺ�*/
	ENUM_PRODUCT_TYPE_3520_9804_01F=0x47,		///< 3520ƽ̨4·(4D1)DVR <VR9804FDF-2U=VR9804FDF-1H =81000047>	
	ENUM_PRODUCT_TYPE_3520_9804_01F_H=0x48,		///< 3520ƽ̨4·(4D1+1·UXGA + 1·720p)���HDVR <VR9804FD-2U=81000048>	

	ENUM_PRODUCT_TYPE_3515_9704FC_1U_NVR_1L=0x49,///< 3515ƽ̨1·UXGA NVR	 <1Ƭ3515 1~2 720P������3801>
	ENUM_PRODUCT_TYPE_3515_HVR_4_1=0x4a, //3515ƽ̨��4D1+1 720p����
	ENUM_PRODUCT_TYPE_3515_VD3801D_B=0x4C,///< 3515ƽ̨1·D1 NVR	 <1Ƭ3515 1 ·>
	ENUM_PRODUCT_TYPE_3515_VD3804D_B=0x4D,///< 3515ƽ̨1· D1 3·CIF NVR	 <1Ƭ3515 1· D1 + 3·CIF >
	/* ��ȡ3520 2Ƭ�����壬ʵ��Ӳ����ֻ����1Ƭ3520, 1.5U*/	
	ENUM_PRODUCT_TYPE_NVR_720p_D4P4_3520_1 = 0x50,	//!3520ƽ̨4·720p NVR<VR9804FHN-2U(1)=81000050>
	
	ENUM_PRODUCT_TYPE_3520_E4D1_1 = 0x51,	//!һƬ3520��4��d1����
	
	ENUM_PRODUCT_TYPE_3515_9704FC_1U_NVR_4L=0x52,///< 3515ƽ̨1·UXGA + 3D1 NVR	 <1Ƭ3515 4·������ VD3804=81000052>
	
	ENUM_PRODUCT_TYPE_3515_9704FC_CUSTDOWN=0x53,  //1D1+3CIF ��Ʒid����VR9804FD_1E
	ENUM_PRODUCT_TYPE_3515_9708FC_CUSTDOWN=0x54,  //8CIF ��Ʒid����VR9708EC_1E
	ENUM_PRODUCT_TYPE_3520_9808EDA_1H_E = 0x55,  ///< 3520ƽ̨8·(8D1+2D1�ط�)DVR <VR9808EDA-1H(E)=> ��ENUM_PRODUCT_TYPE_9816SDA_1HӲ����ͬ

	ENUM_PRODUCT_TYPE_3515_9716SC_1U_E = 0x56,  ///< 3515ƽ̨16·(16CIF custdown))DVR
	ENUM_PRODUCT_TYPE_3515_9704FC_1U_E = 0x57,  ///< 3515ƽ̨4·(4CIF))DVR <VR9704FC-01F(E) ��VR9804FC-01FӲ����ͬ>
	ENUM_PRODUCT_TYPE_3515_VN3701 = 0x58,  ///< 3515ƽ̨1·D1 DVS
	ENUM_PRODUCT_TYPE_3515_VN3702 = 0x59,  ///< 3515ƽ̨2·D1 DVS
	ENUM_PRODUCT_TYPE_3515_VN3704 = 0x5a,  ///< 3515ƽ̨4·D1 DVS
	ENUM_PRODUCT_TYPE_3515_9704FD_CUSTDOWN_EX=0x5b,  //1D1+3CIF ��Ʒid����VR9804FD_1E_LB
	ENUM_PRODUCT_TYPE_3515_9708EC_CUSTDOWN_EX=0x5c,  //8CIF ��Ʒid����VR9708EC_1E_LB
	ENUM_PRODUCT_TYPE_3515_VC2761=0x5d,//720P 
       ENUM_PRODUCT_TYPE_3515_VN3704C_B =0x5e,///4·CIF DVS VN3704C-B=8100005E
	ENUM_PRODUCT_TYPE_3515_VB2761=0x5f,//1080P
	ENUM_CHIP_1_END,
//---------------end
//---------------3Ƭ��Ʒ��
	ENUM_CHIP_3_START = 0x60,
	ENUM_PRODUCT_TYPE_9816SD_1H=0x61,	///< 3520ƽ̨16·(16D1,8D1�ط�) ���HDVR <VR9816SD-1H=80000061> 1·UXGA  +3D1
	ENUM_PRODUCT_TYPE_9816SD_1N=0x62,	///< 3520ƽ̨16·(16D1,8D1�ط�) DVR  <VR9816SDE-1H=80000062>
	ENUM_PRODUCT_TYPE_9812EHN_2U=0x63,	///<8·720p +4D1 ����12·720p
	/*
	 * 20·NVR��16D1+4·720P��
	 * �����ȡ3*3520 ʵ�֣�����һ����Ʒid ��VR9820THN-1H=80000064)
        ����ָ����20���棻
        JSAר�õ�web�Լ��汾��Ϣ����visiondi��Ϣ�����԰汾��
	 */
	ENUM_PRODUCT_TYPE_VR9820THN_1H = 0x64,
	ENUM_PRODUCT_TYPE_VR9824TDN_1H_3 = 0x65, //!VR9824TDN-1H��3��=80000065
	ENUM_CHIP_3_END,

       ENUM_PRODUCT_TYPE_3520_NVR_VR9801HN_1H  = 0xA0,
       ENUM_PRODUCT_TYPE_3520_NVR_VR9801HN_1H_E  = 0xA1,
       ENUM_PRODUCT_TYPE_3520_NVR_VR9802HN_2U  = 0xA2,
       ENUM_PRODUCT_TYPE_3520_NVR_VR9802HN_1H  = 0xA3,
       ENUM_PRODUCT_TYPE_3520_NVR_VR9804HN_2U  = 0xA4,
       ENUM_PRODUCT_TYPE_3520_NVR_VR9806HN_2U  = 0xA5,/*VR9806SHN-2U=800000A5 9808-2U ����,2·1080P,+4· 720P*/

	//!�¿�3515ϵ��id
	ENUM_PRODUCT_TYPE_3515_Yuhua = 0xC0, //!Ŀǰ��֪����Ʒ���ƣ��Ƚ�yuhua
	ENUM_PRODUCT_TYPE_3515_VD3809_V=0xC1,///< 3515ƽ̨9·CIF ������<1Ƭ3515 1 ·>
	ENUM_PRODUCT_TYPE_3515_NVR_AUTOSWITCH_1_720p_4D1_=0xC2, //!nvr��������ʱ��Ԥ��1·720p,4��ʱ������4D1
	ENUM_PRODUCT_TYPE_3515_VR9808EDN_1U = 0xC3,//8·D1 NVR

//---------------end
	ENUM_PRODUCT_TYPE_ALL
}PRODUCT_TYPE;

///	�����ͺ�
typedef enum
{
	ENUM_BOARD_TYPE_982U = 0,
	ENUM_BOARD_TYPE_981U
		
}BOARD_TYPE;

/// ��Ʒ�ط�·��ö��
typedef enum
{
	ENUM_PRODUCT_PLAYBACK_CHANNEL_0	= 0,		///< �ò�Ʒ�޻طŹ��ܣ��ط�·��Ϊ0
	ENUM_PRODUCT_PLAYBACK_CHANNEL_1	= 1,	
	ENUM_PRODUCT_PLAYBACK_CHANNEL_2 = 2,
	ENUM_PRODUCT_PLAYBACK_CHANNEL_4 = 4,		///< �ط���·��Ϊ4
	ENUM_PRODUCT_PLAYBACK_CHANNEL_8 = 8,		///< �ط���·��Ϊ8
	ENUM_PRODUCT_PLAYBACK_CHANNEL_16 = 16,		///< �ط���·��Ϊ16
}PRODUCT_PLAYBACK_NUM;

typedef struct VO_ADN_GUI_LAYER_ATTR_S
{
	int	iDisplayRectWidth;
	int iDisplayRectHeight;
	int iImageRectWidth;
	int iImageRectHeight;
	int iVoOutputType;
	int iGUIRectWidth;
	int iGUIRectHeight;
}VO_ADN_GUI_LAYER_ATTR_S;

enum
{
	VO_OUTPUT_TYPE_1920_1080 = 0,	
	VO_OUTPUT_TYPE_1280_1024 = 1,
	VO_OUTPUT_TYPE_1280_720 = 2,
	VO_OUTPUT_TYPE_1024_768 = 3,
	VO_OUTPUT_TYPE_800_600 ,
	VO_OUTPUT_TYPE_MAX_NUM ,
};
typedef struct product_depend
{
	BOARD_TYPE board_type;
	int total_ipc_channel;
	int total_encode_channel;
	int total_playback_channel;
}PRODUCT_DEPEND;
void InitHicap(void);

PRODUCT_TYPE get_the_product_type(void);
PRODUCT_DEPEND* get_peoduct_depend(void);
enum video_standard_t  get_the_video_std(void);
int initial_product(void);
int verify_product( CHIP_MANAGER_T* pChipDev );
int product_GetTotalChn(void);
int product_GetChnChip(void);
int product_GetNumChn(void);

//!��������ǿ��ָ���˹�ָ��оƬ����ģʽ
int product_GetManualChipMode(void);


VI_DEV GetViDevByChn( int chn );
VI_CHN GetViChnByChn( int chn );
int GetVpssGrpByChn( int chn );
int GetVpssJpegGrpByChn( int chn );
int GetVpssJpegManualGrpByChn( int chn );
int GetVpssHdClipGrpByChn( int chn );
int GetVpssVdecGrpByChn( int chn );
int GetVdecVoChnByChn( int chn );

AUDIO_DEV GetAiDevByChn( int chn );
AI_CHN GetAiChnByChn( int chn );

VENC_CHN GetVencChnByChn( int chn,  enum capture_channel_t type );
VENC_GRP GetVencGrpByChn( int chn,  enum capture_channel_t type );
VENC_GRP GetOsdBaseHandleByChn( int chn,  enum capture_channel_t type );
//VENC_CHN GetVencGrpByChn_Jpg( int chn );
//! chn ��0��ʼ
int GetChipByChn ( int chn );

int GetChnByVencChn(HI_S32 ChipId, VENC_CHN venc_chn);
enum capture_channel_t GetTypeByVencChn(HI_S32 ChipId, VENC_CHN venc_chn);

AENC_CHN GetAencChnByAChn(int chn);
int GetChipByAchn(int chn);
int GetChnByAencChn(HI_S32 ChipId, AENC_CHN aenc_chn);

int GetVoDevByChn(/*CHIP_MANAGER_T* pThis,*/  int chn );
int GetVoChnByChn(/*CHIP_MANAGER_T* pThis,*/  int chn );
int GetAdecChnByChn( int chn );
int GetAoDevByChn( int chn );
int GetAoChnByChn( int chn );


/*********************************************************
 * �߼�dvrͨ��-����
 *********************************************************/

//!ȡ����Ӧ��ͼ��֧�ֵ���С���ʣ��������
HI_U32 GetMinBitRate(enum capture_size_t size);
HI_U32 GetMaxBitRate(enum capture_size_t size);

enum capture_size_t GetSizeByWH(int width, int height);
int GetWidthBySize( enum capture_size_t size, VIDEO_NORM_E vstd);
int GetHeightBySize( enum capture_size_t size, VIDEO_NORM_E vstd);
int GetRate(VIDEO_NORM_E vstd);

/**
 * @brief Vi ����--start
 * ������� ֻ��ͨ�������ͨ����Ӧ�豸������ͨ��
 * ����ԭ��
 * 	�����豸ʼ�մ��ڿ���״̬��ֹͣ����ͨ����ֻ������Ӧ������ͨ��
 * ������������VI�豸Ӧ��ֻ��һ��
 */
typedef struct tagVI_MANAGER
{
	unsigned char m_bSysInit;
	VIDEO_NORM_E m_vstd;
	//!�豸�ţ���ЧֵΪ-1
	//!��ʶ�����豸״̬��-1 ������: 0-����״̬��1-����״̬
	int m_vi_dev[VIU_MAX_DEV_NUM];
	//!��¼ÿ��vi�豸��ÿһ��VIͨ��״̬��0-����״̬��1-����״̬
	unsigned char m_vi_chn_state[VIU_MAX_DEV_NUM][VIU_MAX_CHN_NUM];
	//!����ÿ��ͨ��������ߴ�
	enum capture_size_t m_vi_chn_size[VIU_MAX_DEV_NUM][VIU_MAX_CHN_NUM];
}VI_MANAGER_T;

VI_MANAGER_T* VIM_GetIns(void);

//!vstd������̬�ı�(�Ƚ��鷳).
int 	VIM_Init(VI_MANAGER_T* pThis, VIDEO_NORM_E vstd);
int 	VIM_Start(VI_MANAGER_T* pThis, int chn);
int 	VIM_Stop(VI_MANAGER_T* pThis, int chn);
void 	VIM_Exit(VI_MANAGER_T* pThis);

int 			VIM_SetVsvd(VI_MANAGER_T* pThis, VIDEO_NORM_E vstd);
VIDEO_NORM_E 	VIM_GetVsvd(VI_MANAGER_T* pThis);
int 			VIM_SetMode(VI_MANAGER_T* pThis, VI_INPUT_MODE_E mode, VI_WORK_MODE_E enViWorkMode);

int 			VIM_SetChnSize(VI_MANAGER_T* pThis, int chn, enum capture_size_t size);

/***VI ���� --end****/

/****************VO ���� --start****************/
#define M_CHIP_SLAVE_MASTER		1
#define M_CHIP_SLAVE_SLAVE		0

/* RGB format is 1888. */
#define VO_BKGRD_RED      0xFF0000    /* red back groud color */
#define VO_BKGRD_GREEN    0x00FF00    /* green back groud color */
#define VO_BKGRD_BLUE     0x0000FF    /* blue back groud color */
#define VO_BKGRD_BLACK    0x000000    /* black back groud color */

//#define VO_CCD_INTF_TYPE    VO_INTF_VGA
#define VO_CCD_INTF_TYPE    VO_INTF_BT1120
#define VO_CCD_INTF_STND    VO_OUTPUT_1280x1024_60
#define VO_CCD_SLAVE_INTF   VO_INTF_BT1120
//!Ϊ�������������ü������8������
#define TOP_CUT_ROW 8
#define CHECK_RET(express,name)\
                            do{\
                                HI_S32 s32Ret;\
                                s32Ret = express;\
                                if (HI_SUCCESS != s32Ret)\
                                {\
                                    printf("%s failed at %s: LINE: %d whit %#x!\n", name, __FUNCTION__, __LINE__, s32Ret);\
                                    return HI_FAILURE;\
                                }\
                            }while(0)

typedef enum hiVO_DEV_E
{
    VO_DEV_HD  = 0,                 /* high definition device */
    VO_DEV_SD  = 2,                 /* assistant device */
    VO_DEV_AD  = 1,                 /* spot device */
} VO_DEV_E;
#define VO_DEV_BUTT 3
typedef struct tagVO_MANAGER
{
	unsigned char m_bSysInit;
}VO_MANAGER_T;

VO_MANAGER_T* VOM_GetIns(void);

int VOM_Init(VO_MANAGER_T* pThis);
void VOM_Exit(VO_MANAGER_T* pThis);

/****************VO ���� -- stop****************/

/****************AI ���� --start****************/
typedef struct tagAI_MANAGER
{
	unsigned char m_bSysInit;
	//!��Ƶ�豸����Ӧ״̬��-1 ������: 0-����״̬��1-����״̬
	int AiDevId[SIO_MAX_NUM];
	//!��¼ÿ��ai�豸��ÿһ��VIͨ��״̬��0-����״̬��1-����״̬
	unsigned char	  AiChn[SIO_MAX_NUM][AIO_MAX_CHN_NUM];
}AI_MANAGER_T;

AI_MANAGER_T* AIM_GetIns(void);

//!��ʼ����Ƶ��������
void 	AIM_Init(AI_MANAGER_T* pThis,PAYLOAD_TYPE_E s_enPayloadType);
int 	AIM_Start(AI_MANAGER_T* pThis, int chn);
int 	AIM_Stop(AI_MANAGER_T* pThis, int chn);
void 	AIM_Exit(AI_MANAGER_T* pThis);

int 	AIM_SetSampleRate(AI_MANAGER_T* pThis, AUDIO_SAMPLE_RATE_E rate);
/****************AI ���� -- stop****************/

/****************depend ���� --start****************/
typedef struct tagDepend_MANAGER
{
	int split_combine;
}Depend_MANAGER_T;

Depend_MANAGER_T* Depend_GetIns(void);

int Depend_for_SplitCreate(Depend_MANAGER_T* pThis);
int Depend_for_DecoderMapChannel(Depend_MANAGER_T* pThis);

int Depend_set_SplitCombine(Depend_MANAGER_T* pThis);

/****************depend ���� --end****************/

HI_S32 ViPubConfig(VI_WORK_MODE_E enViWorkMode, VIDEO_NORM_E  enViNorm);//VI_DEV ViDev,
HI_S32 ViChnConfig(VI_CHN channel, HI_U16 image_size, VIDEO_NORM_E normal);

#endif

