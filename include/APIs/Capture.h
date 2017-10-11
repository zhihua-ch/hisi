

#ifndef __CAPTURE_API_H__
#define __CAPTURE_API_H__

#include "DVRDEF.H"
#include "Video.h" 
#include "System.h" 

#ifdef __cplusplus
extern "C" {
#endif

	/// \defgroup CaptureAPI API Capture
	/// 多路多码流音视频编码接口，依赖于Memory接口。
	///	\n 调用流程图:
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


/// 捕获通道类型
enum capture_channel_t {
	CHL_MAIN_T = 0,		///< 主通道	－	主码流1 
	CHL_2END_T = 1,		///< 辅通道	－	出辅码流2 
	CHL_3IRD_T = 2,		///< 辅通道	－	出辅码流3 
	CHL_4RTH_T = 3,		///< 辅通道	－	出辅码流4 
	CHL_JPEG_T = 4,		///< 辅通道	－	出JPEG抓图 
	CHL_FUNCTION_NUM
};

/// 捕获开始类型
enum capture_start_t {
	CAPTURE_START_VIDEO = 1,	///< 视频编码开。
	CAPTURE_START_AUDIO = 2		///< 音频编码开。
};

/// 捕获压缩格式类型
enum capture_comp_t {
	CAPTURE_COMP_DIVX_MPEG4,	///< DIVX MPEG4。
	CAPTURE_COMP_MS_MPEG4,		///< MS MPEG4。
	CAPTURE_COMP_MPEG2,			///< MPEG2。
	CAPTURE_COMP_MPEG1,			///< MPEG1。
	CAPTURE_COMP_H263,			///< H.263
	CAPTURE_COMP_MJPG,			///< MJPG
	CAPTURE_COMP_FCC_MPEG4,		///< FCC MPEG4
	CAPTURE_COMP_H264,			///< H.264
	CAPTURE_COMP_NR				///< 枚举的压缩标准数目。
};

/// 捕获码流控制模式类型
enum capture_brc_t {
	CAPTURE_BRC_CBR,			///< 固定码流。
	CAPTURE_BRC_VBR,			///< 可变码流。
	CAPTURE_BRC_MBR,			///< 混合码流。
	CAPTURE_BRC_NR				///< 枚举的码流控制模式数目。
};

/// 捕获分辨率类型
/// \note更新下面这个结构时，要同步更新Record.cpp里验证编码能力
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
	CAPTURE_SIZE_XVGA=12, // <  1024*768    VGA输入编码，2800项目需要
	CAPTURE_SIZE_WXGA=13, // <  1280*800
	CAPTURE_SIZE_SXGA=14, // <  1280*1024  
	CAPTURE_SIZE_WSXGA=15, // <  1600*1024  
	CAPTURE_SIZE_UXGA=16, // <  1600*1200
	CAPTURE_SIZE_WUXGA=17, // <  1920*1200
	 
	CAPTURE_SIZE_NR =18///< 枚举的图形大小种类的数目。
};

/// 捕获设备特性结构
typedef struct CAPTURE_CAPS
{
	/// 支持的压缩标准的掩码，位序号对应枚举类型capture_comp_t的每一个值。
	/// 置1表示支持该枚举值对应的特性，置0表示不支持。
	uint Compression;

	/// 支持的码流控制模式的掩码，位序号对应枚举类型capture_brc_t的每一个值。
	/// 置1表示支持该枚举值对应的特性，置0表示不支持。
	uint BitRateControl;

	/// 支持的图像大小类型的掩码，位序号对应枚举类型capture_size_t的每一个值。
	/// 置1表示支持该枚举值对应的特性，置0表示不支持。
	uint ImageSize;

	/// 支持的媒体类型的掩码，与枚举类型capture_start_t的每一个值相与，得到对应
	/// 的特性。置1表示支持该枚举值对应的特性，置0表示不支持。
	uint Media;

	/// 区域覆盖的支持的块数。
	int CoverBlocks;

	/// 保留。
	//!增加按位表示，
	//!最低位表示:需要通道能力功能, 各通道的能力可能不一样，要调用CaptureGetChnCaps获取
	int Reserved;

	/// 支持最多的标题叠加的个数。
	int	TitleCount;

	/// 是否支持区域覆盖 0-不支持，1-支持实时的区域覆盖， 即设置区域时
	/// 速度较快，可以实时调用， 2-支持非实时的区域覆盖，即设置区域时速度较慢，
	/// 只能在用户确定区域后调用。  
	int	RegionCover;
} CAPTURE_CAPS;

/// 捕获数据格式结构
typedef struct CAPTURE_FORMAT
{
	uchar	Compression;		///< 压缩标准，取capture_comp_t类型的值。 
	uchar	BitRateControl; 	///< 码流控制模式，取capture_brc_t类型的值。 
	uchar	ImageSize; 			///< 图像大小，取capture_size_t类型的值。 
	uchar	ImageQuality;		///< 图像质量，取值1-6，值越大，图像质量越好,只有在可变码流下才起作用。
    ushort  Gop;                ///< 多少帧中有一个I帧，描述两个I帧之间P帧的数量，P帧的数量为Gop-1
	uchar	FramesPerSecond;	///< 每秒钟的帧数。一般PAL制1-25帧/秒，NTSC制1-30帧/秒。 
	uchar	AVOption;			///< 音视频选项，最低位表示视频，其次位表示音频 
	ushort	BitRate;			///< 参考码流值，Kbps为单位
    uchar   Reserved[2];        //作字节对齐用，//add by zhangxuewu 20090812 according to sofia , for gop adjust
} CAPTURE_FORMAT;

/// 捕获编码能力结构
typedef struct CAPTURE_DSPINFO 
{
	unsigned int	nMaxEncodePower;	///< DSP 支持的最高编码能力。
	unsigned short	nMaxSupportChannel;	///< DSP 支持最多输入视频通道数。
	unsigned short	bChannelMaxSetSync;	///< DSP 每通道的最大编码设置是否同步 0-不同步, 1 -同步。
	unsigned short	nExpandChannel;		///< DSP 支持的扩展通道数，主要是多路回放使用，目前只是一个
	unsigned short  rev;
}CAPTURE_DSPINFO, *PCAPTURE_DSPINFO;

/// 捕获辅助码流支持特性结构
typedef struct CAPTURE_EXT_STREAM
{
	uint ExtraStream;						///< 用channel_t的位来表示支持的功能。
	uint CaptureSizeMask[CAPTURE_SIZE_NR];	///< 每一个值表示对应分辨率支持的辅助码流。（目前只能用于辅码流）
}CAPTURE_EXT_STREAM, *pCAPTURE_EXT_STREAM;	

//! 叠加的字体长度
#define RASTER_LENGTH 1024

typedef enum 
{
    OSD_OVERLAY_TIME = 0, //时间标题   
    OSD_OVERLAY_TITLE,    //通道名称标题 
    OSD_OVERLAY_MARKER,   //图片水印
    OSD_OVERLAY_MULTILINE,//多行叠加
    OSD_OVERLAY_PRESET,
    OSD_OVERLAY_NR,
}OSD_OVERLAY_TYPE;

/// 捕获标题叠加参数结构
typedef struct CAPTURE_TITLE_PARAM
{
	/// 标题序号，最多支持的标题数从CaptureGetCaps可以取得。序号0对应的是
	/// 时间标题。时间标题点阵由底层内部产生，width height raster参数被忽略。
	int		index;

	/// 是否显示。0-不显示，其他参数忽略，1-显示。
	int		enable;

	/// 标题左上角x坐标，取8的整数倍，采用相对坐标体系(CIF)。
	ushort	x;		

	/// 标题左上角y坐标，采用相对坐标体系(CIF)。
	ushort	y;
	
	/// 标题宽度，取8的整数倍。					
	ushort	width;		

	/// 标题高度。
	ushort	height;		

	/// 前景颜色，16进制表示为0xAABBGGRR，A(Alpha)表示透明度。
	/// text color, rgba 8:8:8:8	
	uint	fg_color;
	
	/// 背景颜色，16进制表示为0xAABBGGRR，A(Alpha)表示透明度。
	/// background color, rgba 8:8:8:8	
	uint	bg_color;

	/// 标题单色点阵数据。0表示该点无效，1表示该点有效。每个字节包含8个象素的
	/// 数据，最左边的象素在最高位，紧接的下一个字节对应右边8个象素，直到一行
	/// 结束，接下来是下一行象素的点阵数据。
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
	OSD_TYPE_VENC,	//!视频
	OSD_TYPE_EXT,
	OSD_TYPE_SNAP//!抓图

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

/// 获得DSP的编码能力，主要是GBE和LB2.0使用
///
int CaptureGetDspInfo(CAPTURE_DSPINFO *pDspInfo);


/// 得到捕获的通道数目。不包括语音捕获的通道，语音捕获的通道是最大捕获通道号加1。
/// 
/// \return捕获的通道数目。
int CaptureGetChannels(void);


/// 得到捕获支持的特性。
/// 
/// \param [out] pCaps 指向捕获特性结构CAPTURE_CAPS的指针。
/// \retval 0  获取成功。
/// \retval 0  获取失败。
int CaptureGetCaps(CAPTURE_CAPS * pCaps);

/**
 * @brief 取出每通道的的捕获特性，在各通道能力不一样时，使用
 * @param chn :指定要取的通道
 * @param pCaps: 指向捕获特性结构CAPTURE_CAPS的指针
 * @return 0  获取成功
 */
int CaptureGetChnCaps(int chn, CAPTURE_CAPS * pCaps);

/// 得到捕获双码流支持的特性。参数和CaptureGetCaps一样，但得到得是双码流支持得特性，
/// 而不是普通编码特性。 
/// 
/// \param [out] pCaps 指向捕获特性结构CAPTURE_CAPS的指针。
/// \retval 0  获取成功。
/// \retval 0  获取失败。
int CaptureGetExtCaps(CAPTURE_EXT_STREAM *pCaps);


/// 创建捕获设备
/// 
/// \param [in] channel 通道号。
/// \retval 0 创建失败
/// \retval 0 创建成功
int CaptureCreate(int channel);


/// 销毁捕获设备
/// 
/// \param [in] channel 通道号。
/// \retval 0 销毁失败
/// \retval 0 销毁成功
int CaptureDestroy(int channel);


/// 应用程序定时调用， 通知捕获模块检测自身状态， 如果有错误， 重启模块。 
/// 
void CaptureSynchronize(void);


/// 设置视频制式。
/// 
/// \param [in]  channel 通道号。
/// \param [in]  dwStandard 视频制式，取video_standard_t类型的值。
/// \retval 0  设置成功
/// \retval 0  设置失败
int CaptureSetVstd(int channel,uint dwStandard);	


/// 设置捕获时间和格式。应用程序需要定时调用此函数与捕获的时间进行同步。
/// 
/// \param [in] channel 通道号。
/// \param [in] pTime 指向系统时间结构SYSTEM_TIME的指针。
/// \param [in] datefmt 日期格式，取date_fmt枚举值。
/// \param [in] datespc 日期分割符，取date_spec枚举值。
/// \param [in] timefmt 时间格式，取time_fmt枚举值。
/// \retval 0  设置成功。
/// \retval 0  设置失败。
int CaptureSetTime(int channel, const SYSTEM_TIME * pTime, int datefmt,int datespc,int timefmt);

int CaptureSetTime2(int channel, const SYSTEM_TIME * pTime, int datefmt,int datespc,int timefmt,int time_hide,int date_hide);

/// 设置标题叠加。
/// 
/// \param [in] channel 通道号。
/// \param [in] pParam 指向标题叠加结构CAPTURE_TITLE_PARAM的指针。
/// \retval 0  设置成功。
/// \retval 0  设置失败。
int CaptureSetTitle(int channel, CAPTURE_TITLE_PARAM *pParam);


/// 设置覆盖区域。
/// 
/// \param [in] channel 通道号。
/// \param [in] pParam 指向视频覆盖结构VIDEO_COVER_PARAM的指针。
/// \retval 0  设置成功。
/// \retval 0  设置失败。
int CaptureSetCover(int channel,int index, VIDEO_COVER_PARAM *pParam);



/// 设置捕获的音量。
/// 
/// \param [in] channel 通道号。
/// \param [in] dwLVolume 左声道音量，取值0-100。
/// \param [in] dwRVolume 右声道音量，取值0-100。
/// \retval 0  设置成功
/// \retval 0  设置失败
int CaptureSetVolume(int channel, uint dwLVolume, uint dwRVolume);


/// 强制编码器产生I帧。 
/// 
/// \param [in] channel 通道号。
/// \param [in] dwType
/// \retval 0  设置成功。
/// \retval 0  设置失败。
int CaptureForceIFrame(int  channel, uint dwType);

/// 设置捕获数据压缩的格式。
/// 
/// \param [in] channel 通道号。
/// \param [in] dwType
/// \param [in] pFormat 指向捕获格式结构CAPTURE_FORMAT的指针。
/// \retval 0  设置成功
/// \retval 0  设置失败
int CaptureSetFormat(int channel, uint dwType, CAPTURE_FORMAT * pFormat);

/// 获取捕获数据压缩的分辨率。
/// 
/// \param [in] channel 通道号。
/// \param [in] dwType
/// \param [out] capture_size_t 指定码流的分辨率。
/// \retval 0  获得成功
/// \retval 0  获得失败
int CaptureGetImageSize(int channel, uint dwType, enum capture_size_t *size);


/// 开始编码。在视频制式、捕获选项、图像质量、帧率、码流控制方式、分辨率、编码
/// 模式等参数被改变时，应用程序会停止捕获，然后重新开始捕获。
/// 
/// \param [in] channel 通道号。
/// \param [in] dwType 参数表示启动捕获的选项，是capture_start_t类型的值的组合。
/// \retval 0  开始成功
/// \retval 0  开始失败
int CaptureStart(int  channel, uint dwType);


/// 停止编码，当前操作立即返回。
/// 
/// \param [in]  channel 通道号。
/// \param [in]  dwType
/// \retval 0  停止成功
/// \retval 0  停止失败
int CaptureStop(int  channel, uint dwType);

/// 初始化抓拍，当前操作立即返回。
/// 
/// \param [in]  channel 通道号。
/// \retval 0  初始化成功
/// \retval >0  初始化失败
//int CaptureCreateSnap(int channel);

//初始化失败后调用此函数进行清理
//Ret参数为初始化失败的返回值
//也可以用于停止抓拍此时Ret 为零
//
//int CaptureDestroySnap(int channel,int Ret);

/// 读取采样数据，读为阻塞超时模式。内存块由设备申请和填充，设备得到适当的数据
/// 后返回。超时时立即返回；若内存块中已经填充数据，则设置实际的数据长度并返回；
/// 若内存块中没有数据，返回错误；将超时时间设置的较小，可以提高数据的实时性。
/// 
/// \param [in] pchannel 通道号，-1表示读所有通道的数据，读到的通道号作为函数
///        返回值返回。
/// \param [in] pdwType
/// \param [out] phMemory 内存块句柄指针，用来保存取到的内存块句柄。内存块的实际数据长
///        度表示捕获到的数据长度。
/// \return>=0 通道号（在输入通道号为-1时）
/// \retval 0  读成功
/// \retval 0  读失败
int CaptureGetBuffer(int *pchannel, uint *pdwType, VD_HANDLE *phMemory);


/// 发送抓图命令
///
//int CaptureSetSnapParam(int channel, char imgq, int second,  int millisecond);


/// 获取图片数据
///
int CaptureGetSnapBuffer(int channel, VD_HANDLE *phMemory, uint dwTimeout);



/// 获取图像颜色
///
int CaptureGetColor(int channel, VIDEO_COLOR * pColor);


/// 获取编码后的音量
/// 
/// \param [in] channel 通道号
/// \param [out] pVolume 音量的指针，真实的码流值，范围是０－65K
/// \retval 0  获取成功
/// \retval 0  获取失败
int CaptureGetVolume(int channel, int *pVolume);


/// 设置捕获打包策略，在捕获过程中，此接口能随时调用并生效。未设置时各个设备应该
/// 性能优先，然后保证打包延时<=120ms。
/// \param channel 通道号，同一个DSP的不同通道最好能分开设置
/// \param sizeOfPacket 数据包大小，取值1K-16K
/// \param maxFramesInPacket 包中最大帧数，取值1-8
/// \param maxMilliSecondOfPacket 包中数据最大毫秒数，取值0-4000
/// \retval 0 设置成功
/// \retval <0 设置失败
int CaptureSetPacketPolicy(int channel, int sizeOfPacket, int maxFramesInPacket,
						   int maxMilliSecondOfPacket);

/// 设置捕获数据画面分割方式（画中画）
/// 
/// \param [in] bigChannel 通道号，0为下标
/// \param [in] dwChannels　小画面的掩码，０表示恢复成单画面
/// \param [in] dwType 参数表示启动捕获的选项，是capture_start_t类型的值的组合。
/// \retval 0  设置成功
/// \retval 0  设置失败
int CaptureSetSplit(int bigChannel, uint dwChannels, uint dwType);

/// 设置CIF或者D1分辨率环回,返回0成功
int ViSetCIF(int channel);
int ViSetD1(int channel);

/*
EventType,事件类型,1:回放,2,网络
enable,状态,1:事件开始,2:事件结束
channel,通道号,按位
*/
int UpdateVBRValue(int EventType,int enable ,int channel,CAPTURE_FORMAT format[]) ;

//设置OSD的放大倍数，
///OSDZoom表示具体参数
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

