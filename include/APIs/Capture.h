

#ifndef __CAPTURE_API_H__
#define __CAPTURE_API_H__

#include "DVRDEF.H"
#include "Video.h" 
#include "System.h" 

#ifdef __cplusplus
extern "C" {
#endif

	/// \defgroup CaptureAPI API Capture
	/// ��·����������Ƶ����ӿڣ�������Memory�ӿڡ�
	///	\n ��������ͼ:
	/// \code
	///    ============================================
	///                   	|                            
	///             CaptureCreate             
	///       +-----------|------------------+         
	///       |     CaptureSetVstd         CaptureGetChannels
	///		  |				|                CaptureGetDspInfo,
	///       |   	CaptureSetFormat		CaptureGetCaps,
	///    	  |	CaptureSetPacketPolicy	CaptureGetExtCaps      
	///       |           	|           	CaptureSetTime   	 
	///       |     CaptureStart     		CaptureSetTitle   
	///       | +---------|          		CaptureSetCover   
	///       | |  CaptureGetBuffer  		CaptureSetVolume  
	///       | | CaptureForceIFrame 	CaptureSynchronize
	///       | |  CaptureGetVolume          |         
	///       | +---------|                  |         
	///       |      CaptureStop             |         
	///       +-----------|------------------+         
	///           CaptureDestroy                       
	///                   |                            
	///    ============================================


/// ����ͨ������
enum capture_channel_t {
	CHL_MAIN_T = 0,		///< ��ͨ��	��	������1 
	CHL_2END_T = 1,		///< ��ͨ��	��	��������2 
	CHL_3IRD_T = 2,		///< ��ͨ��	��	��������3 
	CHL_4RTH_T = 3,		///< ��ͨ��	��	��������4 
	CHL_JPEG_T = 4,		///< ��ͨ��	��	��JPEGץͼ 
	CHL_FUNCTION_NUM
};

/// ����ʼ����
enum capture_start_t {
	CAPTURE_START_VIDEO = 1,	///< ��Ƶ���뿪��
	CAPTURE_START_AUDIO = 2		///< ��Ƶ���뿪��
};

/// ����ѹ����ʽ����
enum capture_comp_t {
	CAPTURE_COMP_DIVX_MPEG4,	///< DIVX MPEG4��
	CAPTURE_COMP_MS_MPEG4,		///< MS MPEG4��
	CAPTURE_COMP_MPEG2,			///< MPEG2��
	CAPTURE_COMP_MPEG1,			///< MPEG1��
	CAPTURE_COMP_H263,			///< H.263
	CAPTURE_COMP_MJPG,			///< MJPG
	CAPTURE_COMP_FCC_MPEG4,		///< FCC MPEG4
	CAPTURE_COMP_H264,			///< H.264
	CAPTURE_COMP_NR				///< ö�ٵ�ѹ����׼��Ŀ��
};

/// ������������ģʽ����
enum capture_brc_t {
	CAPTURE_BRC_CBR,			///< �̶�������
	CAPTURE_BRC_VBR,			///< �ɱ�������
	CAPTURE_BRC_MBR,			///< ���������
	CAPTURE_BRC_NR				///< ö�ٵ���������ģʽ��Ŀ��
};

/// ����ֱ�������
/// \note������������ṹʱ��Ҫͬ������Record.cpp����֤��������
enum capture_size_t {
	CAPTURE_SIZE_D1=0, ///< 720*576(PAL) 720*480(NTSC)
	CAPTURE_SIZE_HD1, ///< 352*576(PAL) 352*480(NTSC)
	CAPTURE_SIZE_BCIF, ///< 720*288(PAL) 720*240(NTSC)
	CAPTURE_SIZE_CIF, ///< 352*288(PAL) 352*240(NTSC)
	CAPTURE_SIZE_QCIF, ///< 176*144(PAL) 176*120(NTSC)
	CAPTURE_SIZE_VGA, ///< 640*480(PAL) 640*480(NTSC)
	CAPTURE_SIZE_QVGA, ///< 320*240(PAL) 320*240(NTSC)
	CAPTURE_SIZE_SVCD, ///< 480*480(PAL) 480*480(NTSC)
	CAPTURE_SIZE_QQVGA, ///< 160*128(PAL) 160*128(NTSC)
	CAPTURE_SIZE_720P = 9, ///< 1280*720    720p
	CAPTURE_SIZE_1080P = 10, ///< 1920*1080  1080p 
	CAPTURE_SIZE_SVGA=11, // <  800*592
	CAPTURE_SIZE_XVGA=12, // <  1024*768    VGA������룬2800��Ŀ��Ҫ
	CAPTURE_SIZE_WXGA=13, // <  1280*800
	CAPTURE_SIZE_SXGA=14, // <  1280*1024  
	CAPTURE_SIZE_WSXGA=15, // <  1600*1024  
	CAPTURE_SIZE_UXGA=16, // <  1600*1200
	CAPTURE_SIZE_WUXGA=17, // <  1920*1200
	 
	CAPTURE_SIZE_NR =18///< ö�ٵ�ͼ�δ�С�������Ŀ��
};

/// �����豸���Խṹ
typedef struct CAPTURE_CAPS
{
	/// ֧�ֵ�ѹ����׼�����룬λ��Ŷ�Ӧö������capture_comp_t��ÿһ��ֵ��
	/// ��1��ʾ֧�ָ�ö��ֵ��Ӧ�����ԣ���0��ʾ��֧�֡�
	uint Compression;

	/// ֧�ֵ���������ģʽ�����룬λ��Ŷ�Ӧö������capture_brc_t��ÿһ��ֵ��
	/// ��1��ʾ֧�ָ�ö��ֵ��Ӧ�����ԣ���0��ʾ��֧�֡�
	uint BitRateControl;

	/// ֧�ֵ�ͼ���С���͵����룬λ��Ŷ�Ӧö������capture_size_t��ÿһ��ֵ��
	/// ��1��ʾ֧�ָ�ö��ֵ��Ӧ�����ԣ���0��ʾ��֧�֡�
	uint ImageSize;

	/// ֧�ֵ�ý�����͵����룬��ö������capture_start_t��ÿһ��ֵ���룬�õ���Ӧ
	/// �����ԡ���1��ʾ֧�ָ�ö��ֵ��Ӧ�����ԣ���0��ʾ��֧�֡�
	uint Media;

	/// ���򸲸ǵ�֧�ֵĿ�����
	int CoverBlocks;

	/// ������
	//!���Ӱ�λ��ʾ��
	//!���λ��ʾ:��Ҫͨ����������, ��ͨ�����������ܲ�һ����Ҫ����CaptureGetChnCaps��ȡ
	int Reserved;

	/// ֧�����ı�����ӵĸ�����
	int	TitleCount;

	/// �Ƿ�֧�����򸲸� 0-��֧�֣�1-֧��ʵʱ�����򸲸ǣ� ����������ʱ
	/// �ٶȽϿ죬����ʵʱ���ã� 2-֧�ַ�ʵʱ�����򸲸ǣ�����������ʱ�ٶȽ�����
	/// ֻ�����û�ȷ���������á�  
	int	RegionCover;
} CAPTURE_CAPS;

/// �������ݸ�ʽ�ṹ
typedef struct CAPTURE_FORMAT
{
	uchar	Compression;		///< ѹ����׼��ȡcapture_comp_t���͵�ֵ�� 
	uchar	BitRateControl; 	///< ��������ģʽ��ȡcapture_brc_t���͵�ֵ�� 
	uchar	ImageSize; 			///< ͼ���С��ȡcapture_size_t���͵�ֵ�� 
	uchar	ImageQuality;		///< ͼ��������ȡֵ1-6��ֵԽ��ͼ������Խ��,ֻ���ڿɱ������²������á�
    ushort  Gop;                ///< ����֡����һ��I֡����������I֮֡��P֡��������P֡������ΪGop-1
	uchar	FramesPerSecond;	///< ÿ���ӵ�֡����һ��PAL��1-25֡/�룬NTSC��1-30֡/�롣 
	uchar	AVOption;			///< ����Ƶѡ����λ��ʾ��Ƶ�����λ��ʾ��Ƶ 
	ushort	BitRate;			///< �ο�����ֵ��KbpsΪ��λ
    uchar   Reserved[2];        //���ֽڶ����ã�//add by zhangxuewu 20090812 according to sofia , for gop adjust
} CAPTURE_FORMAT;

/// ������������ṹ
typedef struct CAPTURE_DSPINFO 
{
	unsigned int	nMaxEncodePower;	///< DSP ֧�ֵ���߱���������
	unsigned short	nMaxSupportChannel;	///< DSP ֧�����������Ƶͨ������
	unsigned short	bChannelMaxSetSync;	///< DSP ÿͨ���������������Ƿ�ͬ�� 0-��ͬ��, 1 -ͬ����
	unsigned short	nExpandChannel;		///< DSP ֧�ֵ���չͨ��������Ҫ�Ƕ�·�ط�ʹ�ã�Ŀǰֻ��һ��
	unsigned short  rev;
}CAPTURE_DSPINFO, *PCAPTURE_DSPINFO;

/// ����������֧�����Խṹ
typedef struct CAPTURE_EXT_STREAM
{
	uint ExtraStream;						///< ��channel_t��λ����ʾ֧�ֵĹ��ܡ�
	uint CaptureSizeMask[CAPTURE_SIZE_NR];	///< ÿһ��ֵ��ʾ��Ӧ�ֱ���֧�ֵĸ�����������Ŀǰֻ�����ڸ�������
}CAPTURE_EXT_STREAM, *pCAPTURE_EXT_STREAM;	

//! ���ӵ����峤��
#define RASTER_LENGTH 1024

typedef enum 
{
    OSD_OVERLAY_TIME = 0, //ʱ�����   
    OSD_OVERLAY_TITLE,    //ͨ�����Ʊ��� 
    OSD_OVERLAY_MARKER,   //ͼƬˮӡ
    OSD_OVERLAY_MULTILINE,//���е���
    OSD_OVERLAY_PRESET,
    OSD_OVERLAY_NR,
}OSD_OVERLAY_TYPE;

/// ���������Ӳ����ṹ
typedef struct CAPTURE_TITLE_PARAM
{
	/// ������ţ����֧�ֵı�������CaptureGetCaps����ȡ�á����0��Ӧ����
	/// ʱ����⡣ʱ���������ɵײ��ڲ�������width height raster���������ԡ�
	int		index;

	/// �Ƿ���ʾ��0-����ʾ�������������ԣ�1-��ʾ��
	int		enable;

	/// �������Ͻ�x���꣬ȡ8�����������������������ϵ(CIF)��
	ushort	x;		

	/// �������Ͻ�y���꣬�������������ϵ(CIF)��
	ushort	y;
	
	/// �����ȣ�ȡ8����������					
	ushort	width;		

	/// ����߶ȡ�
	ushort	height;		

	/// ǰ����ɫ��16���Ʊ�ʾΪ0xAABBGGRR��A(Alpha)��ʾ͸���ȡ�
	/// text color, rgba 8:8:8:8	
	uint	fg_color;
	
	/// ������ɫ��16���Ʊ�ʾΪ0xAABBGGRR��A(Alpha)��ʾ͸���ȡ�
	/// background color, rgba 8:8:8:8	
	uint	bg_color;

	/// ���ⵥɫ�������ݡ�0��ʾ�õ���Ч��1��ʾ�õ���Ч��ÿ���ֽڰ���8�����ص�
	/// ���ݣ�����ߵ����������λ�����ӵ���һ���ֽڶ�Ӧ�ұ�8�����أ�ֱ��һ��
	/// ����������������һ�����صĵ������ݡ�
	uchar	*raster;
	char	*titleStr;
}CAPTURE_TITLE_PARAM;

typedef struct osd_zoom_s
{
    uchar char_width;
    uchar char_height;
    uchar img_width;
    uchar img_height;
} OSD_ZOOM_S;

typedef enum
{
	OSD_TYPE_VENC,	//!��Ƶ
	OSD_TYPE_EXT,
	OSD_TYPE_SNAP//!ץͼ

}OSD_TYPE_E;

typedef struct TAG_OSD_FONT_BG_COLOR
{
    int channle;        //channe no witch begins from 0(end to g_nCapture),reserved now;
    OSD_TYPE_E type;    //osd to stream,OSD_TYPE_VENC or OSD_TYPE_SNAP,reserved now;
    int osdIndex;       //begins from 0(end to 3),0 is title, 1 is time,reserved now;
    int rgbFontColor;    
	int rgbFontBgColor;
}OSD_FONT_BG_COLOR;

typedef struct
{
    union tag_osd_zoom
	{
        uint size;
		OSD_ZOOM_S venc_zoom;
    }u;
	uint res[3];
}CAPTURE_OSD_ZOOM;

/// ���DSP�ı�����������Ҫ��GBE��LB2.0ʹ��
///
int CaptureGetDspInfo(CAPTURE_DSPINFO *pDspInfo);


/// �õ������ͨ����Ŀ�����������������ͨ�������������ͨ������󲶻�ͨ���ż�1��
/// 
/// \return�����ͨ����Ŀ��
int CaptureGetChannels(void);


/// �õ�����֧�ֵ����ԡ�
/// 
/// \param [out] pCaps ָ�򲶻����ԽṹCAPTURE_CAPS��ָ�롣
/// \retval 0  ��ȡ�ɹ���
/// \retval 0  ��ȡʧ�ܡ�
int CaptureGetCaps(CAPTURE_CAPS * pCaps);

/**
 * @brief ȡ��ÿͨ���ĵĲ������ԣ��ڸ�ͨ��������һ��ʱ��ʹ��
 * @param chn :ָ��Ҫȡ��ͨ��
 * @param pCaps: ָ�򲶻����ԽṹCAPTURE_CAPS��ָ��
 * @return 0  ��ȡ�ɹ�
 */
int CaptureGetChnCaps(int chn, CAPTURE_CAPS * pCaps);

/// �õ�����˫����֧�ֵ����ԡ�������CaptureGetCapsһ�������õ�����˫����֧�ֵ����ԣ�
/// ��������ͨ�������ԡ� 
/// 
/// \param [out] pCaps ָ�򲶻����ԽṹCAPTURE_CAPS��ָ�롣
/// \retval 0  ��ȡ�ɹ���
/// \retval 0  ��ȡʧ�ܡ�
int CaptureGetExtCaps(CAPTURE_EXT_STREAM *pCaps);


/// ���������豸
/// 
/// \param [in] channel ͨ���š�
/// \retval 0 ����ʧ��
/// \retval 0 �����ɹ�
int CaptureCreate(int channel);


/// ���ٲ����豸
/// 
/// \param [in] channel ͨ���š�
/// \retval 0 ����ʧ��
/// \retval 0 ���ٳɹ�
int CaptureDestroy(int channel);


/// Ӧ�ó���ʱ���ã� ֪ͨ����ģ��������״̬�� ����д��� ����ģ�顣 
/// 
void CaptureSynchronize(void);


/// ������Ƶ��ʽ��
/// 
/// \param [in]  channel ͨ���š�
/// \param [in]  dwStandard ��Ƶ��ʽ��ȡvideo_standard_t���͵�ֵ��
/// \retval 0  ���óɹ�
/// \retval 0  ����ʧ��
int CaptureSetVstd(int channel,uint dwStandard);	


/// ���ò���ʱ��͸�ʽ��Ӧ�ó�����Ҫ��ʱ���ô˺����벶���ʱ�����ͬ����
/// 
/// \param [in] channel ͨ���š�
/// \param [in] pTime ָ��ϵͳʱ��ṹSYSTEM_TIME��ָ�롣
/// \param [in] datefmt ���ڸ�ʽ��ȡdate_fmtö��ֵ��
/// \param [in] datespc ���ڷָ����ȡdate_specö��ֵ��
/// \param [in] timefmt ʱ���ʽ��ȡtime_fmtö��ֵ��
/// \retval 0  ���óɹ���
/// \retval 0  ����ʧ�ܡ�
int CaptureSetTime(int channel, const SYSTEM_TIME * pTime, int datefmt,int datespc,int timefmt);

int CaptureSetTime2(int channel, const SYSTEM_TIME * pTime, int datefmt,int datespc,int timefmt,int time_hide,int date_hide);

/// ���ñ�����ӡ�
/// 
/// \param [in] channel ͨ���š�
/// \param [in] pParam ָ�������ӽṹCAPTURE_TITLE_PARAM��ָ�롣
/// \retval 0  ���óɹ���
/// \retval 0  ����ʧ�ܡ�
int CaptureSetTitle(int channel, CAPTURE_TITLE_PARAM *pParam);


/// ���ø�������
/// 
/// \param [in] channel ͨ���š�
/// \param [in] pParam ָ����Ƶ���ǽṹVIDEO_COVER_PARAM��ָ�롣
/// \retval 0  ���óɹ���
/// \retval 0  ����ʧ�ܡ�
int CaptureSetCover(int channel,int index, VIDEO_COVER_PARAM *pParam);



/// ���ò����������
/// 
/// \param [in] channel ͨ���š�
/// \param [in] dwLVolume ������������ȡֵ0-100��
/// \param [in] dwRVolume ������������ȡֵ0-100��
/// \retval 0  ���óɹ�
/// \retval 0  ����ʧ��
int CaptureSetVolume(int channel, uint dwLVolume, uint dwRVolume);


/// ǿ�Ʊ���������I֡�� 
/// 
/// \param [in] channel ͨ���š�
/// \param [in] dwType
/// \retval 0  ���óɹ���
/// \retval 0  ����ʧ�ܡ�
int CaptureForceIFrame(int  channel, uint dwType);

/// ���ò�������ѹ���ĸ�ʽ��
/// 
/// \param [in] channel ͨ���š�
/// \param [in] dwType
/// \param [in] pFormat ָ�򲶻��ʽ�ṹCAPTURE_FORMAT��ָ�롣
/// \retval 0  ���óɹ�
/// \retval 0  ����ʧ��
int CaptureSetFormat(int channel, uint dwType, CAPTURE_FORMAT * pFormat);

/// ��ȡ��������ѹ���ķֱ��ʡ�
/// 
/// \param [in] channel ͨ���š�
/// \param [in] dwType
/// \param [out] capture_size_t ָ�������ķֱ��ʡ�
/// \retval 0  ��óɹ�
/// \retval 0  ���ʧ��
int CaptureGetImageSize(int channel, uint dwType, enum capture_size_t *size);


/// ��ʼ���롣����Ƶ��ʽ������ѡ�ͼ��������֡�ʡ��������Ʒ�ʽ���ֱ��ʡ�����
/// ģʽ�Ȳ������ı�ʱ��Ӧ�ó����ֹͣ����Ȼ�����¿�ʼ����
/// 
/// \param [in] channel ͨ���š�
/// \param [in] dwType ������ʾ���������ѡ���capture_start_t���͵�ֵ����ϡ�
/// \retval 0  ��ʼ�ɹ�
/// \retval 0  ��ʼʧ��
int CaptureStart(int  channel, uint dwType);


/// ֹͣ���룬��ǰ�����������ء�
/// 
/// \param [in]  channel ͨ���š�
/// \param [in]  dwType
/// \retval 0  ֹͣ�ɹ�
/// \retval 0  ֹͣʧ��
int CaptureStop(int  channel, uint dwType);

/// ��ʼ��ץ�ģ���ǰ�����������ء�
/// 
/// \param [in]  channel ͨ���š�
/// \retval 0  ��ʼ���ɹ�
/// \retval >0  ��ʼ��ʧ��
//int CaptureCreateSnap(int channel);

//��ʼ��ʧ�ܺ���ô˺�����������
//Ret����Ϊ��ʼ��ʧ�ܵķ���ֵ
//Ҳ��������ֹͣץ�Ĵ�ʱRet Ϊ��
//
//int CaptureDestroySnap(int channel,int Ret);

/// ��ȡ�������ݣ���Ϊ������ʱģʽ���ڴ�����豸�������䣬�豸�õ��ʵ�������
/// �󷵻ء���ʱʱ�������أ����ڴ�����Ѿ�������ݣ�������ʵ�ʵ����ݳ��Ȳ����أ�
/// ���ڴ����û�����ݣ����ش��󣻽���ʱʱ�����õĽ�С������������ݵ�ʵʱ�ԡ�
/// 
/// \param [in] pchannel ͨ���ţ�-1��ʾ������ͨ�������ݣ�������ͨ������Ϊ����
///        ����ֵ���ء�
/// \param [in] pdwType
/// \param [out] phMemory �ڴ����ָ�룬��������ȡ�����ڴ�������ڴ���ʵ�����ݳ�
///        �ȱ�ʾ���񵽵����ݳ��ȡ�
/// \return>=0 ͨ���ţ�������ͨ����Ϊ-1ʱ��
/// \retval 0  ���ɹ�
/// \retval 0  ��ʧ��
int CaptureGetBuffer(int *pchannel, uint *pdwType, VD_HANDLE *phMemory);


/// ����ץͼ����
///
//int CaptureSetSnapParam(int channel, char imgq, int second,  int millisecond);


/// ��ȡͼƬ����
///
int CaptureGetSnapBuffer(int channel, VD_HANDLE *phMemory, uint dwTimeout);



/// ��ȡͼ����ɫ
///
int CaptureGetColor(int channel, VIDEO_COLOR * pColor);


/// ��ȡ����������
/// 
/// \param [in] channel ͨ����
/// \param [out] pVolume ������ָ�룬��ʵ������ֵ����Χ�ǣ���65K
/// \retval 0  ��ȡ�ɹ�
/// \retval 0  ��ȡʧ��
int CaptureGetVolume(int channel, int *pVolume);


/// ���ò��������ԣ��ڲ�������У��˽ӿ�����ʱ���ò���Ч��δ����ʱ�����豸Ӧ��
/// �������ȣ�Ȼ��֤�����ʱ<=120ms��
/// \param channel ͨ���ţ�ͬһ��DSP�Ĳ�ͬͨ������ֿܷ�����
/// \param sizeOfPacket ���ݰ���С��ȡֵ1K-16K
/// \param maxFramesInPacket �������֡����ȡֵ1-8
/// \param maxMilliSecondOfPacket ������������������ȡֵ0-4000
/// \retval 0 ���óɹ�
/// \retval <0 ����ʧ��
int CaptureSetPacketPolicy(int channel, int sizeOfPacket, int maxFramesInPacket,
						   int maxMilliSecondOfPacket);

/// ���ò������ݻ���ָʽ�����л���
/// 
/// \param [in] bigChannel ͨ���ţ�0Ϊ�±�
/// \param [in] dwChannels��С��������룬����ʾ�ָ��ɵ�����
/// \param [in] dwType ������ʾ���������ѡ���capture_start_t���͵�ֵ����ϡ�
/// \retval 0  ���óɹ�
/// \retval 0  ����ʧ��
int CaptureSetSplit(int bigChannel, uint dwChannels, uint dwType);

/// ����CIF����D1�ֱ��ʻ���,����0�ɹ�
int ViSetCIF(int channel);
int ViSetD1(int channel);

/*
EventType,�¼�����,1:�ط�,2,����
enable,״̬,1:�¼���ʼ,2:�¼�����
channel,ͨ����,��λ
*/
int UpdateVBRValue(int EventType,int enable ,int channel,CAPTURE_FORMAT format[]) ;

//����OSD�ķŴ�����
///OSDZoom��ʾ�������
void InitOsdSize(int channel);
int SetOSDSize(CAPTURE_OSD_ZOOM * OSDZoom, int channel);
int GetOSDSize(CAPTURE_OSD_ZOOM* OSDZoom, int channel);
int Osd_SetFontBgColor(OSD_FONT_BG_COLOR *rgbColor);
int Osd_GetFontBgColor(OSD_FONT_BG_COLOR *rgbColor);
int OSD_LoadBmp(const char *filename, char** data, unsigned short * width, unsigned short * height);
int OSD_ReleaseBmp(char* data);
enum capture_size_t SystemGetVideoSize(void);
video_ad_t SystemGetVideoAd(void);
int SystemSetVideoAd(video_ad_t videoad);
unsigned int SystemGetAudioVolume(void);
int SystemSetAudioVolume(unsigned int value);
int SystemParameterRecall();

#ifdef __cplusplus
}
#endif

#endif //__CAPTURE_API_H__

