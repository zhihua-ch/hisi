
#ifndef __VIDEO_API_H__
#define __VIDEO_API_H__

#include "DVRDEF.H"
#include "power_control_public.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \defgroup VideoAPI API Video
/// ��Ƶ����API����Ҫ����Ƶ���������������������ã�����һЩ���������ù��ܡ�
///	\n ��������ͼ:
/// \code
///    =======================================================
///                |                            
///	      *Video2GetOutChannels       
///          VideoGetCaps            
///                |                            
///           VideoCreate                      
///      +---------|--------------+----------------------+     
///      |         |(In)          |(Out)                 |(Others)
///      |   VideoSetColor   VideoSwitch         VideoWriteRegister         
///      |   VideoSetCover VideoSetBkColor        VideoReadRegister
///      |   VideoSetVstd   VideoSetTVMargin        VideoMatrix
///      |         |        VideoSetTVColor      VideoSwitchTVMonitor
///      |         |       VideoSetTVAntiDither VideoSwitchVGAMonitor
///      |         |      VideoSetOutputAdapter          |
///      |         |      VideoSetOutputAdapter          |
///      |         |              |                      |
///      +---------|--------------+----------------------+          
///            VideoDestory   
///                |                            
///    =======================================================
/// \endcode
/// @{

/// ��Ƶ��ʽ����
enum video_standard_t {
	VIDEO_STANDARD_PAL = 0,	///< PAL��ʽ��
	VIDEO_STANDARD_NTSC,	///< NTSC��ʽ��
	VIDEO_STANDARD_SECAM,	///< SECAM��ʽ��
	VIDEO_STANDARD_AUTO ///<�Զ������ʽ
};
typedef enum  {
	VIDEO_AD_AHD = 0,
	VIDEO_AD_TVI,
	VIDEO_AD_INVALID,
}video_ad_t;

enum {
    ENUM_COLOR_RUILI,
    ENUM_COLOR_YANLI,    
    ENUM_COLOR_BIAOZHUN,
    ENUM_COLOR_ROUHE,
    ENUM_COLOR_ZHENGCHANG,
    ENUM_COLOR_ZIDINGYI
};
typedef enum {
    ENUM_DISPLAY_LCD,
    ENUM_DISPLAY_HDMI,    
    ENUM_DISPLAY_VGA,
    ENUM_DISPLAY_INVALID
}DISPLAY_DEV;
/// ��Ƶ��ɫ��ʽ
typedef struct VIDEO_COLOR{
	uchar	Brightness;		///< ���ȣ�ȡֵ0-100��
	uchar	Contrast;		///< �Աȶȣ�ȡֵ0-100��
	uchar 	Saturation;		///< ���Ͷȣ�ȡֵ0-100��
	uchar 	Hue;			///< ɫ����ȡֵ0-100��
	uchar 	Gain;			///< ���棬ȡֵ0-100��bit7��λ��ʾ�Զ����棬����λ�����ԡ�
	uchar	WhiteBalance;	///< �Զ��׵�ƽ���ƣ�bit7��λ��ʾ�����Զ�����.0x0,0x1,0x2�ֱ�����,��,�ߵȼ�
	uchar   acutance;
	uchar	reserved;
}VIDEO_COLOR;

/// ��Ƶ����ṹ
/// - ����ʾ������Ƶ���������ʱ, ʹ��ͼ��������ϵ, Ҳ������ָ��ͼ��������ϵ����Ƶ�߽�
/// ������, �ṹ��Ա��ȡֵ��ͼ�ηֱ���, ͼ�κ���Ƶ����ʱ��ƫ�ƺ����Ų���, ��ǰ�ָ�ģ
/// ʽ�ȶ��й�ϵ;
/// - ����ʾ����������Ƶ�����е�һ��������, ��ʹ���������, �����һ������ķֱ���
/// (0, 0, RELATIVE_MAX_X, RELATIVE_MAX_Y), ʹ�������������ڲ�֪��ʵ����Ƶ�ֱ��ʵ�
/// ������趨��Ƶ��������, ���Ա�֤һ��ת�������1��������. ת����ʽ����(������Է�
/// ����Ϊaw*ah):
/// ��������(ax, ay)ת�����������(rx, ry):
/// \code
/// rx = ax * RELATIVE_MAX_X / aw;
/// ry = ay * RELATIVE_MAX_Y / ah;
/// \endcode
/// �������(rx, ry)ת���ɾ�������(ax, ay):
/// \code
/// ax = rx * aw / RELATIVE_MAX_X;
/// ay = ry * ah / RELATIVE_MAX_Y;
/// \endcode
/// ��Էֱ��ʶ�������:
/// \code
/// #define RELATIVE_MAX_X 8192
/// #define RELATIVE_MAX_Y 8192
/// \endcode
///
typedef struct VIDEO_RECT
{
	ushort	left;			///< ���Ͻ�x���ꡣ
	ushort	top;			///< ���Ͻ�y���ꡣ
	ushort	right;			///< ���½�x���ꡣ
	ushort	bottom;			///< ���½�y���ꡣ
}VIDEO_RECT;

/// ��Ƶ���ǲ�������
enum video_cover_t {
	VIDEO_COVER_WINDOW_CLEAR = 0,	///< ������ڸ�������
	VIDEO_COVER_WINDOW_DRAW,		///< ���ƴ��ڸ�������
	VIDEO_COVER_FULLSCREEN_DRAW,	///< ����ȫ����������
	VIDEO_COVER_FULLSCREEN_CLEAR	///< ���ȫ����������
};

/// ��Ƶ���ǲ���
typedef struct VIDEO_COVER_PARAM
{
	/// ���ǵľ������򣬲������������ϵ��
	VIDEO_RECT	rect;

	/// ������������ʾ����ɫ��16���Ʊ�ʾΪ0x00BBGGRR������ֽڱ�����
	uint		color;

	/// ������������ͣ� ȡvideo_cover_t���͵�ֵ�����ں�ȫ������Ӧ���Ƕ����ģ� 
	/// �������ڵĲ�����Ӱ��ȫ����ȫ���ڵĲ���Ҳ��Ӱ�촰�ڡ� �������ʱ������
	/// ���������ԡ�
	int			enable;
}VIDEO_COVER_PARAM;

/// ��Ƶ�л�����
enum video_switch_t{
	VIDEO_SWITCH_MONITOR = 0,	///< �л������������
	VIDEO_SWITCH_PLAY			///< �л����ط������
};

/// ��Ƶ�������������
enum video_output_t{
	VIDEO_OUTPUT_AUTO = 0,	///< �Զ�����豸��
	VIDEO_OUTPUT_VGA,		///< �л���VGA�����
	VIDEO_OUTPUT_TV,		///< �л���TV�����
	VIDEO_OUTPUT_VGA_LCD	///< �л���LCD VGA�����
};

/// �����豸���Խṹ
typedef struct VIDEO_CAPS
{
	/// �Ƿ�֧��TV�����ɫ���á���1��ʾ֧�ָ�ö��ֵ��Ӧ�����ԣ���0��ʾ��֧�֡�
	uchar TVColor;
	/// ͼ����������
	/// \note ����λ����ο�pic_attr
} VIDEO_CAPS;				

#if (16==CHANNEL)
/* maozheng-add-20090709 S */
/* 16·ʱ����PCIV��Ϣ���ݵĽṹ�� */
typedef struct VIDEO_COVER_PCIV_S
{
    int					channel;
	int					index;
    VIDEO_COVER_PARAM	coverAttr;
} VIDEO_COVER_PCIV_S;

/* maozheng-add-20090709 E */
#endif

#if defined (_USE_720P_MODULE_) || defined (_PRODUCT_2900)
typedef struct _VIDEOIN_CAPS{
 uint uiCamAbility;  //��λ������32��Ŀ�� 
                  //��0λ ֧��ˮƽ��תHorReverse��1Ϊ֧�ָ���ܣ�0Ϊ��֧�ָ����
                  //��1λ ֧����ֱ��תVerReverse��1Ϊ֧�ָ���ܣ�0Ϊ��֧�ָ����
                  //��2λ ֧���ع�ģʽ����Exposure��1Ϊ֧�ָ���ܣ�0Ϊ��֧�ָ����
                  //��3λ ֧�ָй������AESensitivity��1Ϊ֧�ָ���ܣ�0Ϊ��֧�ָ����
                  //��4λ ֧�ֱ��ⲹ��Backlight��1Ϊ֧�ָ���ܣ�0Ϊ��֧�ָ����
                  //��5λ ֧�ְ�ƽ��ģʽWhiteBalance��1Ϊ֧�ָù��ܣ�0Ϊ��֧�ָ���ܡ�
                  //��6λ ֧����Ӱ����ShadeCorr��1Ϊ֧�ָù��ܣ�0Ϊ��֧�ָ���ܡ�
                  //��7λ ֧�ֻ�������BadCorr��1Ϊ֧�ָù��ܣ�0Ϊ��֧�ָ���ܡ�
                  //��8λ ֧�ַ���˸Anti-Flicker��1Ϊ֧�ָù��ܣ�0Ϊ��֧�ָ����
                  //��9λ ֧��ɫ�����ڣ�1Ϊ֧�ָù��ܣ�0Ϊ��֧�ָù���
                  //��10λ֧��YUV�����Χ���ƣ�1Ϊ֧�ָù��ܣ�0Ϊ��֧�ָù���
                  //��11λ֧��IR-CUT���ƣ�1Ϊ֧�ָù��ܣ�0Ϊ��֧�ָù���
                  //��12λ֧��ȥ����ƣ�1Ϊ֧�ָù��ܣ�0Ϊ��֧�ָù���
                  //��13λ֧�ֺڰ�/��ɫ/�Զ�ת����1Ϊ֧�ָù��ܣ�0Ϊ��֧�ָù���
 
 uint uiRes;//����
 uint uiSenSpan; //�й�ȷ�Χ���ֵ ��ֵ255�����ʶ��ΧΪ0~255
 uint uiBLightSpan;//���ⲹ��֧������Backlight,��ֵ15����Χ0~15
 uint uiShutterL;//�ֶ��ع� ��ֵΪ6000��֧����С�ع�ʱ��Ϊ1/6000��
 uint uiShutterH; //�ֶ��ع⣬��ֵΪ50��֧����С�ع�ʱ��Ϊ1/50��
 uint uiAntiFlicker;//����˸֧��ģʽ����λ����
   //��0λ �Ƿ�֧��50Hz indoorģʽ
   //��1λ �Ƿ�֧��50Hz outdoorģʽ
   //��2λ �Ƿ�֧��50Hz autoģʽ
   //��3λ �Ƿ�֧��60Hz indoorģʽ
   //��4λ �Ƿ�֧��60Hz outdoorģʽ
   //��5λ �Ƿ�֧��60Hz autoģʽ
 uint uiWhiteBlance; //���֧�ְ�ƽ��ģʽ��������Ч����λ��ʾ��ƽ��֧�ֵ�ģʽ
   //ÿһλ��ӦWHITE_BALANCE_MODEö���е���Ӧλ������0xc0��ʾ֧��ɫ��4650k��ɫ��6500k����ģʽ
 uint uiRes2[2];//����
}VIDEOIN_CAPS;

//CONFIG_CAMERA��CAMERA_FORMAT�ṹ��һ�����޸�ʱ����ע��
typedef struct _CAMERA_FORMAT_
{
	 uchar HorReverse; //֧��ˮƽ��תʱȡֵ��ΧΪö��IMAGE_MODE_MIRROE�е�ֵ
	 uchar VerReverse; //֧����ֱ��תʱȡֵ��ΧΪö��IMAGE_MODE_MIRROE�е�ֵ
	 uchar ExposureType; //֧���ع�ģʽʱȡֵ��ΧΪö��EXPOSURE_MODE�е�ֵ1:�Զ��ع� 2:�ֶ�
	 uchar AESensitivity; //֧�ָй������ʱȡֵ��ΧΪ0��ABILITY_CAMERA_T.uiSenSpan
	 uchar Backlight; //֧�ֱ��ⲹ��ʱȡֵ��ΧΪ0��ABILITY_CAMERA_T.uiBLightSpan
	 uchar WhiteBalanceMode; //֧�ְ�ƽ��ʱȡֵ��ΧΪ������ABILITY_CAMERA_T.uiWhiteBlance�а�λ��ʾ��ֵ
	 uchar ShadeCorr; //�ݲ�֧�֣�ȡֵ��Χδ֪
	 uchar BadCorr;  //�ݲ�֧�֣�ȡֵ��Χδ֪
	 uint shutter;  //�ع�ģʽ��Ϊ�ֶ�ʱȡֵ��ΧΪuiShutterL��uiShutterH֮��
	 uchar AntiFlicker; //֧�ַ���˸ʱ��ȡֵ��ΧΪ������ABILITY_CAMERA_T.uiAntiFlicker�а�λ��ʾ��ֵ
	 uchar ColorTemp; //֧��ɫ������ʱ��ȡֵ��ΧΪö��COLOR_TEMP_MODE �е�ֵ
	 uchar YUVCut; //֧��yuv�����Χ����ʱ��ȡֵ��ΧΪö��YUV_RANGE_MODE �е�ֵ
	 uchar IRCut; //֧��IR-CUTʱ��ȡֵ��ΧΪö��IR_CUT_MODE�е�ֵ
	 uchar PAA; //֧��ȥ�빦��ʱ��ȡֵ��ΧΪö��PAA_MODE�е�ֵ
	 uchar Color2WhiteBlack; //֧�ֺڰ�/��ɫ/�Զ�ת��ʱ��ȡֵ��ΧΪö��COLOR_2_WHITEBLACK_MODE�е�ֵ
	 uchar SensorType;   //�������� �ߵ�ƽ��Ч�͵�ƽ��Ч 1 �ߵ�ƽ 0�͵�ƽ  
	 uchar res;
}CAMERA_FORMAT;

typedef struct _CAMERA_WHITEBALANCE_  //�ֶ����ð�ƽ���ǵ�RGB
{
	 uchar WhiteBalanceMode; //֧�ְ�ƽ��ʱȡֵ��ΧΪ������ABILITY_CAMERA_T.uiWhiteBlance�а�λ��ʾ��ֵ
	 uchar wbr; //�Զ���ƽ��R����
	 uchar wbg; //�Զ���ƽ��R����
	 uchar wbb; //�Զ���ƽ��R����
	 uchar res[4];
}CAMERA_WHITEBALANCE;

#endif


/// �õ���Ƶ�����ͨ����Ŀ��
/// 
/// \param ��
/// \return��Ƶ���ͨ����Ŀ��
int Video2GetOutChannels(void);


/// ������Ƶ�豸
/// 
/// \param ��
/// \retval <0 ����ʧ��
/// \retval 0 �����ɹ�
int VideoCreate(void);
int Video2Create(int index);
int AudioOut_SetVolume(int value);


/// ������Ƶ�豸
/// 
/// \param ��
/// \retval <0 ����ʧ��
/// \retval 0 ���ٳɹ�
int VideoDestory(void);
int Video2Destroy(int index);


/// ������Ƶ����ɫ������
/// 
/// \param [in] channel ͨ���š�
/// \param [in] pColor ָ����ɫ�ṹVIDEO _ COLOR��ָ�롣
/// \retval 0  ���óɹ�
/// \retval <0  ����ʧ��
int VideoSetColor(int channel, VIDEO_COLOR * pColor);

/// ͼ��֧�ֵ������б�����
/// \note ��Ϊͼ�����б�����mask 1:������ 0:��������
enum pic_attr
{
 PIC_COLOR, ///< ����,�Աȶ�,ɫ��,����,���Ͷ�
 PIC_BACKLIGHT,///< ���ⲹ��
 PIC_EXPOSAL, 	///< �ع�ѡ��
 PIC_AUTOC2BW, ///< �Զ��ʺ�ת��
 PIC_MIRROR,		//����
 PIC_FLIP,		//��ת
 PIC_MASKNUM,  ///< ����ѡ����
};
 
/// ��Ƶ���ƽṹ
typedef struct VIDEO_CONTROL{
 uchar  Exposure; ///< �ع�ģʽ 1-6:�ֶ��ع�ȼ�; 0:�Զ��ع�
 uchar  Backlight; ///< ���ⲹ�� 1:�򿪲���; 0:ֹͣ����
 uchar  AutoColor2BW; ///< �Զ��ʺ�ת�� 1:��ת��; 0:ֹͣת��
  uchar  Mirror;		//< ����  1 ���� 0��

}VIDEO_CONTROL;
 
/// ������Ƶ�Ŀ��Ʋ�����
/// 
/// \param [in] channel ͨ���š�
/// \param [in] pColor ָ����Ƶ���ƽṹVIDEO_CONTROL��ָ�롣
/// \retval 0  ���óɹ�
/// \retval <0  ����ʧ��
int VideoSetControl(int channel, VIDEO_CONTROL * pColor);


/// ���ø�������
/// 
/// \param [in] channel ͨ���š�
/// \param [in] index �ڵ������
/// \param [in] pParam ָ����Ƶ���ǽṹVIDEO_COVER_PARAM��ָ�롣
/// \retval 0  ���óɹ���
/// \retval <0  ����ʧ�ܡ�
int VideoSetCover(int channel, int index, VIDEO_COVER_PARAM *pParam);


/// ������Ƶ��ʽ��
/// 
/// \param [in] channel ͨ���š�
/// \param [in] dwStandard ��Ƶ��ʽ��ȡvideo_standard_t���͵�ֵ��
/// \retval 0  ���óɹ�
/// \retval <0  ����ʧ��
int VideoSetVstd(int channel,uint dwStandard);	

//!��ȡ��Ƶ��ʽ, ����ֵ��enum video_standard_t�� -1ʱ����ʾ��Ч��ʽ
int VideoGetVstd(int channel);

/// дһ���ֽڵ���Ƶ�豸�Ĵ�����
/// 
/// \param [in] Chip оƬ��š�
/// \param [in] Page ҳ����š�
/// \param [in] Register �Ĵ�����š�
/// \param [in] Data Ҫд���ֵ��
/// \return ��
void VideoWriteRegister(uchar Chip, uchar Page, uchar Register, uchar Data);


/// ����Ƶ�豸�Ĵ�����һ���ֽڡ�
/// 
/// \param [in] Chip оƬ��š�
/// \param [in] Page ҳ����š�
/// \param [in] Register �Ĵ�����š�
/// \return������ֵ��
uchar VideoReadRegister(uchar Chip, uchar Page, uchar Register);


/// �л���Ƶ��������͡�
/// \param [in] dwType ��������ͣ�ȡ�±��е�ֵ��
/// \retval 0  �л��ɹ���
/// \retval <0  �л�ʧ�ܡ�
int VideoSwitch(uint dwType);
int Video2Switch(int index, uint dwType);

int VideoDisplaySwitch(DISPLAY_DEV display_dev);
DISPLAY_DEV VideoDisplayDetection();
/// ����TV����ı߽硣��Ƶ��ʾ��ĳЩ��������ʱ�� 4�����ϻ�����ʧ�����������Ŀ��
/// ��������4���߽�ѹ���ı�����ʹ��Ƶ���ڼ������Ŀ��������ڡ����ڷ�Χ0-100��һ��
/// ʾ��ֵ��ʵ���϶�Ӧ��С�ĳߴ���Ҫ�ڵײ���ת�������ơ�
/// 
/// \param [in] left ��߽���ʣ�ȡֵ0-100�� Ĭ��ֵΪ0��
/// \param [in] top �ϱ߽���ʣ�ȡֵ0-100�� Ĭ��ֵΪ0��
/// \param [in] right �ұ߽���ʣ�ȡֵ0-100�� Ĭ��ֵΪ0��
/// \param [in] bottom �±߽���ʣ�ȡֵ0-100�� Ĭ��ֵΪ0��
/// \retval 0  ���óɹ���
/// \retval <0  ����ʧ�ܡ�
int VideoSetTVMargin(uchar left, uchar top, uchar right, uchar bottom);


/// ����TV�������ɫ��
/// 
/// \param [in] brightness ���ȣ�ȡֵ0-100�� Ĭ��ֵΪ50��
/// \param [in] contrast �Աȶȣ�ȡֵ0-100�� Ĭ��ֵΪ50��
/// \retval 0  ���óɹ���
/// \retval <0  ����ʧ�ܡ�
int VideoSetTVColor(uchar brightness, uchar contrast);


/// ����TV���������������������Ӳ�����ж��������⡣
/// 
/// \param [in] level �����������������̶���ȡֵ���Ǳ�����ϵ������һ������ֵ��
///        ȡֵ0-100�� Ĭ��ֵΪ50��
/// \retval 0  ���óɹ���
/// \retval <0  ����ʧ�ܡ�
int VideoSetTVAntiDither(uchar level);


/// ������Ƶ��������������͡�
/// 
/// \param [in] type ���������ͣ�ȡvideo_output_t���͵�ֵ��
/// \retval 0  ���óɹ���
/// \retval <0  ����ʧ�ܡ�
int VideoSetOutputAdapter(uchar type);


/// ��ȡ��Ƶ��������������͡�
/// 
/// \param [out] ptype ���������ͣ�ȡvideo_output_t���͵�ֵ��
/// \retval 0  ��ȡ�ɹ���
/// \retval <0  ��ȡʧ�ܡ�
int VideoGetOutputAdapter(uchar *ptype);


/// ������Ƶ��������ͨ��������ͨ����
/// 
/// \param [in]  channelIn ��Ƶ����ͨ��
/// \param [in]  channelOut ��Ƶ���ͨ��
/// \retval 0  ��ȡ�ɹ���
/// \retval <0  ��ȡʧ�ܡ�
int VideoMatrix(uchar channelIn,uchar channelOut);


/// ����TV������
/// 
/// \param [in] open TRUE-��TV��������FALSE-��TV������
/// \retval 0  �����ɹ���
/// \retval <0  ����ʧ�ܡ�
int VideoSwitchTVMonitor(VD_BOOL open);


/// ����VGA������
/// 
/// \param [in] open TRUE-��VGA��������FALSE-��VGA������
/// \retval 0  �����ɹ���
/// \retval <0  ����ʧ�ܡ�
int VideoSwitchVGAMonitor(VD_BOOL open);
/// ���òɼ�оƬ�Ĳɼ�����
/// 
/// \param [in] ��Ӧͨ����ˮƽ��ֱƫ�ƣ��Լ��������
/// \retval 0  �����ɹ���
/// \retval <0  ����ʧ�ܡ�
int VideoSetCropRegion(int channel,int Hoffset,int Voffset,int Width,int Height);
/// �õ���Ƶ֧�ֵ����ԡ�
/// 
/// \param [out] pCaps ָ����Ƶ���ԽṹVIDEO_CAPS��ָ�롣
/// \retval 0  ��ȡ�ɹ���
/// \retval <0  ��ȡʧ�ܡ�
int VideoGetCaps(VIDEO_CAPS * pCaps);
int Video2GetCaps(int index, VIDEO_CAPS * pCaps);


/// ����û����Ƶ�ļ��ӻ���ı�����ɫ.
/// 
/// \param [in] color Ҫ���õ���ɫ��16���Ʊ�ʾΪ0x00BBGGRR������ֽڱ�����
/// \retval 0  ���óɹ�
/// \retval <0  ����ʧ��
int VideoSetBkColor(uint color);
int Video2SetBkColor(int index, uint color);


/// ������Ƶ����ֱ���
int VideoRecordSetSize(int channel, int size);
	/*
˵��	��ʼ�Զ����

����	��

����ֵ	=0  ��	  �ɹ���
	����ֵ��  ʧ�ܡ�

*/
int VideoAutoDetectStart(void);
/*
˵��	ֹͣ�Զ����


����	��

����ֵ	=0  ��	  �ɹ���
	����ֵ��  ʧ�ܡ�
	*/
int VideoAutoDetectStop(void);

/// @} end of group


typedef struct tagWHITE_LEVEL_CAPS //Added by wangjun2-20080805
{
	int	 enable;		// Ϊ1��ʾʹ�ܣ�Ϊ0ʱ��ʾ����
}WHITE_LEVEL_CAPS;

/*
	˵��:��ȡ�׵�ƽ��������ֵ
	����ֵ=0:�ɹ�
	����ֵ:ʧ��
*/
int WhiteLevelGetCaps(WHITE_LEVEL_CAPS * pCaps);

/*
	set or get video mode
	SystemSetVideoMode() return 0 success
*/
enum video_standard_t SystemGetVideoMode(void);
int SystemSetVideoMode(enum video_standard_t videostand);
int SystemSetVideoSize(unsigned char videosize);
int SystemSetBackLight(LCD_BACKLIGHT_TYPE backlight);
LCD_BACKLIGHT_TYPE SystemGetBackLight(void);

#if defined (_USE_720P_MODULE_) || defined (_PRODUCT_2900)
/*
	˵��: 		����
	����in: 	iChannel ͨ��
				pFormat���ýṹ��
	����out: 	��
	����ֵ	=0  ��	  �ɹ���
		<0��  ʧ�ܡ�

*/

int VideoInSetCamera(uint iChannel, CAMERA_FORMAT *pFormat);

int VideoInSetWhiteBalance(uint iChannel, CAMERA_WHITEBALANCE *pWhiteBalance);

/*
	˵��: 		��ȡ��ͷ�����ṹ��
	����in: 	ich ͨ��
	����out: 	pCaps����ͷ�����ṹ��
	����ֵ	=0  ��	  �ɹ���
		<0��  ʧ�ܡ�

*/
int VideoInGetChCaps(int ich , VIDEOIN_CAPS* pCaps);

/*
	˵��: 		��ȡĳ�ַ���Ӧ�Ĳ���ֵ
	����in: 	ich ͨ��
	����in: 	iType ��ʾ���    //ENUM_COLOR_RUILI,    
	                                                        ENUM_COLOR_YANLI,        
	                                                        ENUM_COLOR_BIAOZHUN,    
	                                                        ENUM_COLOR_ROUHE,

	����out: 	pCaps����ͷ�����ṹ��
	����ֵ	=0  ��	  �ɹ���
		<0��  ʧ�ܡ�
*/
int VideoGetColorByType(int channel, int iType,VIDEO_COLOR * pColor);


int VideoSetTVoutColor(VIDEO_COLOR * pColor);

#endif

#ifdef __cplusplus
}
#endif

#endif //__VIDEO_API_H__

