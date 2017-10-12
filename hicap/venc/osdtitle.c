
#include "DVRDEF.H"
#include "DVRINC.H"
#include "AVenc.h"
//#include "pciv_msg.h"
#include "Capture.h"
#include "OSDTitle.h"
/*#include "BlindDetect.h"
#include "MotionDetect.h"*/
#include "initial.h"
#include "AVenc.h"
#include "PointFont.h"
//#include "loadbmp.h"

#define trip printf("_______%d_________\n", __LINE__);

///16·PCIVͨ��ʱ��OSD��Ϣ�Ľṹ��
typedef struct CAPTURE_TITLE_PARAM_PCIV
{
	int channel;
	int slaveVideoMode;
	int leftDate;
	CAPTURE_TITLE_PARAM param;
	unsigned char	buf[PCIV_OSD_TITLE_PKT_LEN];
}CAPTURE_TITLE_PARAM_PCIV;

typedef struct tagOSD_TIME_PCIV
{
	int channel;
	int datefmt;
	int datespc;
	int timefmt;
	SYSTEM_TIME time;
}tagOSD_TIME_PCIV;

static unsigned char g_bInit = 0;
static int g_TimeLen = 21;//osdʱ������ַ����ȣ��벻Ҫ�����޸�
static OSD_OVERLAY_PARAM g_osd_overlay_para[N_SYS_CH][OSD_OVERLAY_NR];
extern enum capture_size_t g_videosize;
void InitOSD()
{
	if( 1 == g_bInit )
	{
		return ;
	}
    
	g_bInit = 1;

	HI_S32 s32Ret;
	int chn = 0, i;
	for( chn= 0; chn < N_SYS_CH; chn++ )
	{
		for( i = 0; i < OSD_OVERLAY_NR; i++ )
		{
			memset(&g_osd_overlay_para[chn][i], 0, sizeof(OSD_OVERLAY_PARAM));
			g_osd_overlay_para[chn][i].changed = OSD_OVERLAY_STATE_NORMAL;
			g_osd_overlay_para[chn][i].osdHandle = 0x123456;
			g_osd_overlay_para[chn][i].ExtosdHandle = 0x123456;
			g_osd_overlay_para[chn][i].SnapOsdHandle = 0x123456;
			
			g_osd_overlay_para[chn][i].Param.index = i;
			g_osd_overlay_para[chn][i].Param.enable=0;
			////0xff0000(��),0xffff00(��),0xffffff(��),0x0(��)
          		  //��osd���ӵ���ɫ��ʼ��Ϊ���ֺڵ�,��ɫ��RGB��ʽ��ʾ
			g_osd_overlay_para[chn][i].Param.fg_color = 0xffffff;
			g_osd_overlay_para[chn][i].Param.bg_color = 0x0;
			g_osd_overlay_para[chn][i].bUsedVi = 0;
			if(get_the_product_type() == ENUM_PRODUCT_TYPE_3515_Yuhua)
			{
				g_osd_overlay_para[chn][i].bUsedVi = 1;
			}
			g_osd_overlay_para[chn][i].time_hide = 0;
			g_osd_overlay_para[chn][i].date_hide = 0;
			s32Ret = pthread_mutex_init(&(g_osd_overlay_para[chn][i].lock), NULL);		
			assert( s32Ret == 0 );
		}
	}
}

HI_S32 HI_PointFont2RGB_Title(CAPTURE_TITLE_PARAM * pParam, char *pRGBBuf, int channel, OSD_TYPE_E type,int iWidth,int iLength)
{
    OSD_FONT_BG_COLOR rgb;
	short a1555FontColor;
	short a1555FontBgColor;
	HI_S32   i, j, k, rowAddr, colAddr;
	HI_CHAR  *fgRGB, *bgRGB;
	HI_CHAR     byteSrc;
	int IMG_WIDTH = 1;
	int IMG_HEIGHT = 1;
	
	CAPTURE_OSD_ZOOM OSDZoom;

	int iRet = 0;

       if (iWidth>2 ||iWidth<1 
            || iWidth>2 ||iWidth<1)
       {
	    ERR_PRINT("\n");
	    return -1;
       }

       IMG_WIDTH = iWidth;
       IMG_HEIGHT = iLength;
       
#if 0
#if defined(_USE_720P_MODULE_)	
	if(OSD_TYPE_EXT == type) 
	{
		iRet= GetOSDSizeExt(&OSDZoom, channel,CHL_2END_T);
	}
	else
	{
		iRet= GetOSDSizeExt(&OSDZoom, channel,CHL_MAIN_T);
	}
#else
	iRet = GetOSDSize(&OSDZoom, channel);
#endif
	if(0 == iRet)
	{
	    IMG_WIDTH = OSDZoom.u.venc_zoom.img_width;
	    IMG_HEIGHT = OSDZoom.u.venc_zoom.img_height;
	}
	else
	{
	    ERR_PRINT("\n");
	    return -1;
	}
#endif

	if(NULL == pParam)
	{
		return -1;
	}
	if(NULL == pParam->raster)
	{
		return -1;
	}
	if(NULL == pRGBBuf)
	{
		return -1;
	}

	rgb.channle = channel;
    rgb.osdIndex = pParam->index;
	if(0 != Osd_GetFontBgColor(&rgb))
	{
	    return -1;
	}
    a1555FontColor = ConvertRGB32TORGB1555(rgb.rgbFontColor);
    a1555FontBgColor = ConvertRGB32TORGB1555(rgb.rgbFontBgColor);
	fgRGB = (HI_CHAR* )&a1555FontColor;
	bgRGB = (HI_CHAR* )&a1555FontBgColor;

	for(i=0; i<pParam->height*IMG_HEIGHT; i++)
	{    
		for(j=0; j<(pParam->width*IMG_WIDTH); j++)
		{
		    size_t off = (i*(pParam->width*IMG_WIDTH)+j)*2;
			memcpy((pRGBBuf+off), bgRGB, 2);
		}
	}

	for(i = 0; i<pParam->height; i++)
	{
		rowAddr = i;
		for(j = 0; j < pParam->width>>3; j++)
		{
			byteSrc = *(pParam->raster + i * (pParam->width>>3) + j);
			for(k = 0; k < 8; k++)
			{
				colAddr = j*8+k;
				if(byteSrc & (0x80 >> k))
				{
  					if((IMG_WIDTH==1) && (IMG_HEIGHT==1))
  					{
  						memcpy(pRGBBuf+(rowAddr * pParam->width + colAddr)*2, fgRGB, 2);
  					}
  					
  					else if ((IMG_WIDTH==2) && (IMG_HEIGHT==1))
  					{
  						memcpy(pRGBBuf+(rowAddr * pParam->width + colAddr)*2*IMG_WIDTH, fgRGB, 2);
  						memcpy(pRGBBuf+(rowAddr * pParam->width + colAddr)*2*IMG_WIDTH+2, fgRGB, 2);
  					}
  					else if ((IMG_WIDTH==2) && (IMG_HEIGHT==2))
  					{							
  						memcpy(pRGBBuf+(rowAddr*IMG_HEIGHT * pParam->width + colAddr)*2*IMG_WIDTH, fgRGB, 2);
  						memcpy(pRGBBuf+(rowAddr*IMG_HEIGHT * pParam->width + colAddr)*2*IMG_WIDTH+2, fgRGB, 2);
  						memcpy(pRGBBuf+((rowAddr*IMG_HEIGHT +1) * pParam->width + colAddr)*2*IMG_WIDTH, fgRGB, 2);
  						memcpy(pRGBBuf+((rowAddr*IMG_HEIGHT +1) * pParam->width + colAddr)*2*IMG_WIDTH+2, fgRGB, 2);							
  					}	
				}
			}
		}
	}

	return HI_SUCCESS;
}

static HI_S32 HI_PointFont2RGB_Time(CAPTURE_TITLE_PARAM * pParam, char *pRGBBuf, int channel)
{
    OSD_FONT_BG_COLOR rgb;
	short a1555FontColor;
	short a1555FontBgColor;
	HI_S32   i, j, k, rowAddr, colAddr;
	HI_CHAR  *fgRGB, *bgRGB;
	HI_CHAR     byteSrc;

	if(NULL == pParam)
	{
		return -1;
	}
	if(NULL == pParam->raster)
	{
		return -1;
	}
	if(NULL == pRGBBuf)
	{
		return -1;
	}
	
	rgb.channle = channel;
    rgb.osdIndex = OSD_OVERLAY_TIME;
	if(0 != Osd_GetFontBgColor(&rgb))
	{
	    return -1;
	}
	a1555FontColor = ConvertRGB32TORGB1555(rgb.rgbFontColor);
	a1555FontBgColor = ConvertRGB32TORGB1555(rgb.rgbFontBgColor);
	fgRGB = (HI_CHAR* )&a1555FontColor;
	bgRGB = (HI_CHAR* )&a1555FontBgColor;

	for(i=0; i<pParam->height; i++)
	{
		for(j=0; j<(pParam->width); j++)
		{
			memcpy((pRGBBuf+(i*(pParam->width)+j)*2), bgRGB, 2);
		}
	}

	for(i = 0; i<pParam->height; i++)
	{
		rowAddr = i;
		for(j = 0; j < pParam->width>>3; j++)
		{
			byteSrc = *(pParam->raster + i * (pParam->width>>3) + j);
			for(k = 0; k < 8; k++)
			{
				colAddr = j*8+k;
				if(byteSrc & (0x80 >> k))
				{
					memcpy(pRGBBuf+(rowAddr * pParam->width + colAddr)*2, fgRGB, 2);					
				}
			}
		}
	}

	return HI_SUCCESS;
}

static inline void char_cpy( char* dest, char* src, const POINT_FONT_S *pFont, OSD_ZOOM_S* zoom)
{
	int i, j, width;
	width = (pFont->s32Width/8) ;
	int CHAR_WIDTH	= zoom->char_width;
	int CHAR_HEIGHT  = zoom->char_height;

	for (i = 0; i < pFont->s32Height; i++)
	{
		//for (j = 0; j < width; j+=2)
		for (j = 0; j < width; j++)
		{
			//	*(dest+j) = *src++;

			if((CHAR_WIDTH==1)&&(CHAR_HEIGHT==1))
			{
					*(dest+j) = *(src++);
			}		
			else
			{	
				if((CHAR_WIDTH==2)&&(CHAR_HEIGHT==1))
				{			
					*(dest+j+1) = (*(src)&0x1)|((*(src)&0x1)<<1)
											|(*(src)&0x2)<<1|((*(src)&0x2)<<2)
											|(*(src)&0x4)<<2|((*(src)&0x4)<<3)
											|(*(src)&0x8)<<3|((*(src)&0x8)<<4);
					*(dest+j) = (*(src)&0x10)>>4|((*(src)&0x10)>>3)
											|(*(src)&0x20)>>3|((*(src)&0x20)>>2)
											|(*(src)&0x40)>>2|((*(src)&0x40)>>1)
											|(*(src)&0x80)>>1|((*(src)&0x80));
				}
				if((CHAR_WIDTH==2)&&(CHAR_HEIGHT==2))
				{
					*(dest+j+1) = (*(src)&0x1)|((*(src)&0x1)<<1)
											|(*(src)&0x2)<<1|((*(src)&0x2)<<2)
											|(*(src)&0x4)<<2|((*(src)&0x4)<<3)
											|(*(src)&0x8)<<3|((*(src)&0x8)<<4);
					*(dest+j) = (*(src)&0x10)>>4|((*(src)&0x10)>>3)
											|(*(src)&0x20)>>3|((*(src)&0x20)>>2)
											|(*(src)&0x40)>>2|((*(src)&0x40)>>1)
											|(*(src)&0x80)>>1|((*(src)&0x80));
					dest +=  g_TimeLen*(CHAR_WIDTH)*width;				
					*(dest+j+1) = (*(src)&0x1)|((*(src)&0x1)<<1)
											|(*(src)&0x2)<<1|((*(src)&0x2)<<2)
											|(*(src)&0x4)<<2|((*(src)&0x4)<<3)
											|(*(src)&0x8)<<3|((*(src)&0x8)<<4);
					*(dest+j) = (*(src)&0x10)>>4|((*(src)&0x10)>>3)
											|(*(src)&0x20)>>3|((*(src)&0x20)>>2)
											|(*(src)&0x40)>>2|((*(src)&0x40)>>1)
											|(*(src)&0x80)>>1|((*(src)&0x80));
				}
				//*(dest+j) = 0xFF;
				//*(dest+j+1) = 0x00;
	
				*src++;
			}
		}
		dest +=  g_TimeLen * (CHAR_WIDTH) * width;
	}
}

static HI_S32 HI_TimeToPointFont(OSD_TIME *pTm, char *pBuf, const POINT_FONT_S *pFont, int channel, OSD_TYPE_E type,
    int iWidth,int iLength)
{	
	HI_U32 		 u32Index, u32Tmp;;
	HI_S32 		 s32FW, s32FH;	//	�ֿ��ַ���С
	HI_S32 		 s32CW, s32CH;	//	��Ļ�ַ���С
	SYSTEM_TIME	*pSysTm = NULL;

	OSD_ZOOM_S zoom;
	zoom.char_width = 1;
	zoom.char_height = 1;

	CAPTURE_OSD_ZOOM OSDZoom;
	int iRet = 0;

#if 0    
#if defined(_USE_720P_MODULE_)	
	if(OSD_TYPE_EXT == type) 
	{
		iRet= GetOSDSizeExt(&OSDZoom, channel,CHL_2END_T);
	}
	else
	{
		iRet= GetOSDSizeExt(&OSDZoom, channel,CHL_MAIN_T);
	}
#else
	iRet = GetOSDSize(&OSDZoom, channel);
#endif
	if(0 == iRet)
	{
	    zoom.char_width = OSDZoom.u.venc_zoom.char_width;
	    zoom.char_height= OSDZoom.u.venc_zoom.char_height;
	}
	else
	{
	    ERR_PRINT("\n");
	    return -1;
	}
#endif

       if (iWidth>2 ||iWidth<1 
            || iWidth>2 ||iWidth<1)
       {
	    ERR_PRINT("\n");
	    return -1;
       }

       zoom.char_width = iWidth;
       zoom.char_height = iLength;
	
	//printf("CHAR_WIDTH %d CHAR_HEIGHT %d\n", CHAR_WIDTH, CHAR_HEIGHT);
	switch(pTm->dateSpec)
	{
	case DS_DASH:
		u32Index = 11;
		break;
	case DS_SLASH:
		u32Index = 12;
		break;
	case DS_DOT:
		u32Index = 13;
		break;
	default:
		u32Index = 13;
		break;
	}

	//	2010-08-03	LiChuan Modify for test
	//	s32FW    = pFont->s32Width/8;
	s32FW   = pFont->s32Width/8;
	s32FH   = pFont->s32Height * s32FW;
	
	s32CW		= s32FW * zoom.char_width;
	s32CH		= s32FH * zoom.char_height;
	
	pSysTm  = &(pTm->sysTime);

	// 2010-08-03 LiChuan �޸ģ� �˴�ʵ����Ӧ����
	//char_cpy(pBuf+s32FW*0, pFont->pFont + s32FW*s32FH*NUMBER_DIGIT(pSysTm->year, 1000), pFont);
	//	����ÿ���ֽڱ�ʾ8���㣬16���ֽڱ�ʾ8*16�ĵ���
	//s32FW*s32FH*u32Index �����ַ�Ϊ��λ�ġ���u32Index��λ���ϵ��ַ�
	//	������Ļ������ַ���С���ֿ����ַ��Ĵ�С���ܲ�һ�������Դ˴���Ļ�ַ�Ӧ��Ϊ��s32CW, s32CH
	switch( pTm->dateFmt)
	{
	case DF_YYMMDD:
		char_cpy(pBuf+s32CW*0, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1000), pFont, &zoom);
		char_cpy(pBuf+s32CW*1, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 100), pFont, &zoom);
		char_cpy(pBuf+s32CW*2, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*3, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1), pFont, &zoom);
		char_cpy(pBuf+s32CW*4, pFont->pFont + (s32FH*u32Index), pFont, &zoom);
		char_cpy(pBuf+s32CW*5, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*6, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 1), pFont, &zoom);
		char_cpy(pBuf+s32CW*7, pFont->pFont + (s32FH*u32Index), pFont, &zoom);
		char_cpy(pBuf+s32CW*8, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*9, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 1), pFont, &zoom);
		break;
	case DF_MMDDYY:
		char_cpy(pBuf+s32CW*0, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*1, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 1), pFont, &zoom);
		char_cpy(pBuf+s32CW*2, pFont->pFont + (s32FH*u32Index), pFont, &zoom);
		char_cpy(pBuf+s32CW*3, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*4, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 1), pFont, &zoom);
		char_cpy(pBuf+s32CW*5, pFont->pFont + (s32FH*u32Index), pFont, &zoom);
		char_cpy(pBuf+s32CW*6, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1000), pFont, &zoom);
		char_cpy(pBuf+s32CW*7, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 100), pFont, &zoom);
		char_cpy(pBuf+s32CW*8, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*9, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1), pFont, &zoom);
		break;
	case DF_DDMMYY:
		char_cpy(pBuf+s32CW*0, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*1, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->day, 1), pFont, &zoom);
		char_cpy(pBuf+s32CW*2, pFont->pFont + (s32FH*u32Index), pFont, &zoom);
		char_cpy(pBuf+s32CW*3, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*4, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->month, 1), pFont, &zoom);
		char_cpy(pBuf+s32CW*5, pFont->pFont + (s32FH*u32Index), pFont, &zoom);
		char_cpy(pBuf+s32CW*6, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1000), pFont, &zoom);
		char_cpy(pBuf+s32CW*7, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 100), pFont, &zoom);
		char_cpy(pBuf+s32CW*8, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*9, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->year, 1), pFont, &zoom);
		break;
	default:
		break;
	}

	
	char_cpy( pBuf+s32CW*10, pFont->pFont + (s32FH*17), pFont, &zoom);

	
	switch (pTm->timeFmt)
	{
	case TF_12:
		u32Tmp = pSysTm->hour % 12;
		if (u32Tmp != 0)
		{
			char_cpy(pBuf+s32CW*11, pFont->pFont + s32FH*NUMBER_DIGIT(u32Tmp, 10), pFont, &zoom);
			char_cpy(pBuf+s32CW*12, pFont->pFont + s32FH*NUMBER_DIGIT(u32Tmp, 1), pFont, &zoom);
		}
		else
		{
			char_cpy(pBuf+s32CW*11, pFont->pFont + s32FH*1, pFont, &zoom);
			char_cpy(pBuf+s32CW*12, pFont->pFont + s32FH*2, pFont, &zoom);
		}
		break;
	case TF_24:
		u32Tmp = pSysTm->hour;
		char_cpy(pBuf+s32CW*11, pFont->pFont + s32FH*NUMBER_DIGIT(u32Tmp, 10), pFont, &zoom);
		char_cpy(pBuf+s32CW*12, pFont->pFont + s32FH*NUMBER_DIGIT(u32Tmp, 1), pFont, &zoom);
		break;
	default:
		break;
	}

	
	char_cpy(pBuf+s32CW*13, pFont->pFont + (s32FH*10), pFont, &zoom);
	char_cpy(pBuf+s32CW*14, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->minute, 10), pFont, &zoom);
	char_cpy(pBuf+s32CW*15, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->minute, 1), pFont, &zoom);
	char_cpy(pBuf+s32CW*16, pFont->pFont + (s32FH*10), pFont, &zoom);
	char_cpy(pBuf+s32CW*17, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->second, 10), pFont, &zoom);
	char_cpy(pBuf+s32CW*18, pFont->pFont + s32FH*NUMBER_DIGIT(pSysTm->second, 1), pFont, &zoom);
	
	switch (pTm->timeFmt)
	{
	case TF_12:
		if (pSysTm->hour < 12)
		{
			char_cpy(pBuf+s32CW*19, pFont->pFont + (s32FH*14), pFont, &zoom);
		}
		else
		{
			char_cpy(pBuf+s32CW*19, pFont->pFont + (s32FH*15), pFont, &zoom);
		}
		char_cpy(pBuf+s32CW*20, pFont->pFont + (s32FH*16), pFont, &zoom);
		break;
	case TF_24:
	default:
		char_cpy(pBuf+s32CW*(19), pFont->pFont + (s32FH*17), pFont, &zoom);
		char_cpy(pBuf+s32CW*(20), pFont->pFont + (s32FH*17), pFont, &zoom);
		break;
	}
	return HI_SUCCESS;
}

ushort GetAsciiCharCode(VD_PCSTR pch, int *pn)
{
    ushort code = 0;
	*pn = 1;
	code = (uchar)*pch;
	return code;
}

uchar GetAsciiCharRaster(ushort code, uchar* p, const POINT_FONT_S *pFont)
{
    int cw = 14;
    static char font[] = "0123456789:-/.APM FT";
    int index = 0;
    int i = 0;
    for(i = 0; i < strlen(font); i++ )
    {
        if( font[i] == (uchar)code )
        {
            index = i;
            break;
        }
    }
    memcpy(p, pFont->pFont+pFont->s32Width*pFont->s32Height/8*index, pFont->s32Width*pFont->s32Height/8);
    return pFont->pFont_cw[index];
}

void TextToRaster(uchar * pBuffer, VD_PSIZE pSize, const POINT_FONT_S *pFont, VD_PCSTR str)
{
    ushort code;//�ַ�unicode
    int n;//�ַ�����
    int cw;//�ַ����
    int cl;//�ַ��ֽ���
    int ox, ox_start, ox_end;//����ƫ��
    int oy, oy_start, oy_end;//����ƫ��

    int xoffset =0;//x����
    uchar * p;//���󻺳�
    //uchar raster[128];
	uchar raster[512];//�Ĵ�BUF�����ô�����֮��BUFװ���¡�yangsc@20151106
    int x,y; //�������

    int len = (int)strlen(str);

    x = 0;
    y = 0;

    if (x%8)
    {
        ERR_PRINT("the X offset  is not match\n");
        return;
    }

    //�������ͳһʹ�ô������õ㣬��Ҫ����
    oy_start = 0;

    oy_end = pFont->s32Height;

        
    for(n = 0; n < len; n += cl/*, x += cw*/)
    {
        code = GetAsciiCharCode(&str[n], &cl);
        if(cl == 0)
        {
            break;
        }
        if(code == '\t')
        {
            xoffset = 96;
        }
		
        cw = GetAsciiCharRaster(code, raster, pFont);
        //cw +=2; //���������ļ��
        p = raster;

        ox_start = 0;
        ox_end = cw;  
        for (oy = oy_start; oy < oy_end; oy++)
        {
            for (ox = ox_start; ox < ox_end; ox++)
            {
                if (*(p + ox / 8) & ( 128 >> (ox % 8) ) )
                {
                    pBuffer[(y+oy)*pSize->w/8+(x+xoffset+ox)/8] |= BITMSK(7-(xoffset+ox)%8);
                }
            }
            p += pFont->s32Width/8;
        }
        xoffset += cw;
    }
}

//!ʱ��������ǰ��
static HI_S32 HI_TimeToPointFont2(OSD_TIME *pTm, char *pBuf, 
										const POINT_FONT_S *pFont, int channel, OSD_TYPE_E type)
{	
    VD_SIZE size;
    size.w = g_TimeLen * pFont->s32Width;
    size.h = pFont->s32Height;

    char spec;
	switch(pTm->dateSpec)
	{
	case DS_DASH:
		spec = '-';
		break;
	case DS_SLASH:
		spec = '/';
		break;
	case DS_DOT:
		spec = '.';
		break;
	default:
		spec = '.';
		break;
	} 
    char noon = 0; //!������
    int hour = pTm->sysTime.hour;
	switch (pTm->timeFmt)
	{
	case TF_12:
	    noon = hour < 12?'A':'P';
		hour = hour > 12?hour % 12:hour;
		break;
	case TF_24:
		break;
	default:
		break;
	}	

    char time_str[120] = {0};
    char str_ymd[20] = {0};
	if(spec == '-')
	{
	    if( pTm->dateFmt == DF_YYMMDD )
	    {
	        sprintf(str_ymd, "%04d%02d%02d", pTm->sysTime.year, pTm->sysTime.month, pTm->sysTime.day);
	    }
	    else if( pTm->dateFmt == DF_MMDDYY )
	    {
	        sprintf(str_ymd, "%02d%02d%04d", pTm->sysTime.month, pTm->sysTime.day, pTm->sysTime.year);
	    }
	    else if( pTm->dateFmt == DF_DDMMYY )
	    {
	        sprintf(str_ymd, "%02d%02d%04d", pTm->sysTime.day, pTm->sysTime.month, pTm->sysTime.year);
	    }
	}
	else
	{
	    if( pTm->dateFmt == DF_YYMMDD )
	    {
	        sprintf(str_ymd, "%04d%c%02d%c%02d", pTm->sysTime.year, spec, pTm->sysTime.month, spec, pTm->sysTime.day);
	    }
	    else if( pTm->dateFmt == DF_MMDDYY )
	    {
	        sprintf(str_ymd, "%02d%c%02d%c%04d", pTm->sysTime.month, spec, pTm->sysTime.day, spec, pTm->sysTime.year);
	    }
	    else if( pTm->dateFmt == DF_DDMMYY )
	    {
	        sprintf(str_ymd, "%02d%c%02d%c%04d", pTm->sysTime.day, spec, pTm->sysTime.month, spec, pTm->sysTime.year);
	    }
	}
	
	if((g_osd_overlay_para[channel][OSD_OVERLAY_TIME].time_hide==1)&&\
	   (g_osd_overlay_para[channel][OSD_OVERLAY_TIME].date_hide==1))
	{
		sprintf(time_str, " ");
	}
	else if(g_osd_overlay_para[channel][OSD_OVERLAY_TIME].time_hide==1)
	{
		if(g_videosize == 0)
			sprintf(time_str, "               %s",str_ymd);
			
		else
		    sprintf(time_str, "           %s",str_ymd);
	}
	else if(g_osd_overlay_para[channel][OSD_OVERLAY_TIME].date_hide==1)
	{
		if(g_videosize == 0)
			sprintf(time_str, "%02d:%02d:%02d%c%c", hour, pTm->sysTime.minute,
        		pTm->sysTime.second, noon==0?' ':noon, noon==0?' ':'M');
		else
	    	sprintf(time_str, "%02d:%02d:%02d%c%c", hour, pTm->sysTime.minute,
	    		pTm->sysTime.second, noon==0?' ':noon, noon==0?' ':'M');

	}
	else
	{
		if(g_videosize == 0)
			sprintf(time_str, "%02d:%02d:%02d%c%c     %s", hour, pTm->sysTime.minute,
	        pTm->sysTime.second, noon==0?' ':noon, noon==0?' ':'M', str_ymd);
			
		else
		    sprintf(time_str, "%02d:%02d:%02d%c%c %s", hour, pTm->sysTime.minute,
		    pTm->sysTime.second, noon==0?' ':noon, noon==0?' ':'M', str_ymd);
	}
    TextToRaster(pBuf, &size, pFont, time_str);
	return HI_SUCCESS;
}

void FlushChnOSD( int channel )
{
	int i = 0;
	for(i = 0; i < OSD_OVERLAY_NR; i++)
	{
		if(g_osd_overlay_para[channel][i].Param.enable != 0)
		{
			Osd_SetTitle( channel, OSD_TYPE_VENC, &(g_osd_overlay_para[channel][i].Param) );
			Osd_SetTitle( channel, OSD_TYPE_SNAP, &(g_osd_overlay_para[channel][i].Param) );

		}
	}
}

//intput:video_size,vstd
//output:param
static HI_S32 OSDCheckRegion(int video_size, VIDEO_NORM_E vstd, CAPTURE_TITLE_PARAM* param)
{
        if(!param)
        {
            return HI_FAILURE;
        }
        
      //  printf("param->width %d, param->height %d\n",param->width, param->height);
        int width = GetWidthBySize(video_size, vstd);
        int height = GetHeightBySize(video_size, vstd);
        int widthD1 = GetWidthBySize(CAPTURE_SIZE_D1, vstd);
        int heightD1 = GetHeightBySize(CAPTURE_SIZE_D1, vstd);
	//	printf("%d:%s()width=%d,height=%d\n",__LINE__,__FUNCTION__,width,height);
	if(param->y>200)
		param->y -= 24;
	if(0 == width || 0 == height || width < param->width || height < param->height)
	{
	    return HI_FAILURE;
	}
	else
	{
            //����D1�ֱ��ʾ�����
            param->x = param->x * width / widthD1;
            param->y = param->y * height / heightD1;

            int iTemp = 0;		
            if(width - param->x < param->width)
            {
                iTemp = width - param->width;
                param->x = (iTemp > 0 ? iTemp : 0);
            }

            if(height - param->y < param->height)
            {
                iTemp = height - param->height;
                param->y = (iTemp > 0 ? iTemp : 0);
            }
	}
    
	//printf("param->width %d, param->height %d\n",param->width, param->height);
	return HI_SUCCESS;
}
int Osd_SetTime(int channel, OSD_TYPE_E type, const SYSTEM_TIME * pTime, unsigned char bDbg )
{
	HI_S32			s32Ret;
	OSD_TIME		osdTime;
	POINT_FONT_S	font;
	BUFFER_S		*pStBufFont=NULL;
	BUFFER_S		*pStBufRGB=NULL;
	RGN_ATTR_S		stRgnAttr;
	MPP_CHN_S stOverlayExChn;
	RGN_CHN_ATTR_S stOverlayExChnAttr;
	BITMAP_S stBitmap;
	RGN_HANDLE*	pOsdHandle ;
	CAPTURE_TITLE_PARAM Param;
	VIDEO_NORM_E vstd = VIM_GetVsvd( VIM_GetIns() );
	int CHAR_WIDTH = 1;
	int CHAR_HEIGHT = 1; 
	CAPTURE_OSD_ZOOM OSDZoom;
	Capture_Dev_t* pCapDev = CaptureDevGetIns();
	OSD_OVERLAY_PARAM* pOsdTimeParam = &g_osd_overlay_para[channel][OSD_OVERLAY_TIME];
	if( GetChipByChn( channel) != Chip_GetLocalID(Chip_GetIns()) )
	{
		return -1;
	}

	if( pOsdTimeParam->bUsedVi && type != OSD_TYPE_VENC )
	{//!��vi���ӵ�ʱ�ֻ��Ҫ����һ·�Ϳ���
		return 0;
	}

	int iRet = 0;
#if defined(_USE_720P_MODULE_)	
#else
	iRet = GetOSDSize(&OSDZoom, channel);
#endif
	if(0 == iRet)
	{
		CHAR_WIDTH = OSDZoom.u.venc_zoom.char_width;
		CHAR_HEIGHT = OSDZoom.u.venc_zoom.char_height;
	}
	else
	{
	    ERR_PRINT("\n");
	    return -1;
	}	
	VENC_GRP venc_grp;
	s32Ret = pthread_mutex_lock( &(pOsdTimeParam->lock) );
	
	int video_size = pCapDev->vi_chn[channel].venc_chn_t.venc_chn[CHL_MAIN_T].image_size;
	if( type == OSD_TYPE_VENC)
	{
		pOsdHandle = &(pOsdTimeParam->osdHandle);
		venc_grp = GetVencGrpByChn(channel,CHL_MAIN_T);
	}
	else if( type == OSD_TYPE_SNAP )
	{
		pOsdHandle = &(pOsdTimeParam->SnapOsdHandle);
		venc_grp = GetVencGrpByChn(channel,CHL_JPEG_T);
	}
	else if(OSD_TYPE_EXT == type)
	{
		pOsdHandle = &(pOsdTimeParam->ExtosdHandle);
		venc_grp = GetVencGrpByChn(channel,CHL_2END_T);
		video_size = pCapDev->vi_chn[channel].venc_chn_t.venc_chn[CHL_2END_T].image_size;
	}
	else
	{
		assert(0);
	}

    if (video_size == CAPTURE_SIZE_CIF ||video_size == CAPTURE_SIZE_QCIF)
    {
        CHAR_WIDTH =1;
        CHAR_HEIGHT = 1;
    }
        
    memcpy( &Param, &pOsdTimeParam->Param, sizeof(CAPTURE_TITLE_PARAM) );
	//printf("%d:%s()Param.enable =%d\n",__LINE__,__FUNCTION__,Param.enable );
	if(Param.enable == 0)
	{
        if( (*pOsdHandle) != 0x123456)
        {
			if( !pOsdTimeParam->bUsedVi )
			{
				stOverlayExChn.enModId = HI_ID_GROUP;
				stOverlayExChn.s32DevId = venc_grp;
				stOverlayExChn.s32ChnId = 0;
			}
			else
			{
				stOverlayExChn.enModId = HI_ID_VIU;
				stOverlayExChn.s32DevId = 0;//GetViDevByChn(channel);
				stOverlayExChn.s32ChnId = GetViChnByChn(channel);
			}
        	HI_MPI_RGN_DetachFrmChn(*pOsdHandle,&stOverlayExChn);
            HI_MPI_RGN_Destroy(*pOsdHandle);
          	(*pOsdHandle) =0x123456;
        }
		goto END;
	}
	pStBufFont = &(pOsdTimeParam->StBufFont);
	pStBufRGB = &(pOsdTimeParam->StBufRGB);
	osdTime.dateFmt = pOsdTimeParam->osdTime.dateFmt;
	osdTime.dateSpec = pOsdTimeParam->osdTime.dateSpec;
	osdTime.timeFmt  = pOsdTimeParam->osdTime.timeFmt;
	memcpy(&(osdTime.sysTime), pTime, sizeof(osdTime.sysTime));

	if(get_the_product_type() == ENUM_PRODUCT_TYPE_3515_Yuhua)
	{
		switch(video_size)
		{
			case 0:
				font.pFont_cw = (unsigned char *)&fontmatrix_w24_h24_cw_2[0];
				font.pFont	   = (unsigned char *)&fontmatrix_w24_h24_2[0][0];
				font.s32Width  = 24;
				font.s32Height = 24;
				break;
			case 9:
				font.pFont_cw = (unsigned char *)&fontmatrix_w24_h32_cw_2[0];
				font.pFont	   = (unsigned char *)&fontmatrix_w24_h32_2[0][0];
				font.s32Width  = 24;
				font.s32Height = 32;
				break;
			case 10:
				font.pFont_cw = (unsigned char *)&fontmatrix_w32_h48_cw_2[0];//fontmatrix_w25_h47_cw_2[0];
				font.pFont	   = (unsigned char *)&fontmatrix_w32_h48_2[0][0];
				font.s32Width  = 40;
				font.s32Height = 45;
				break;
			default:
				font.pFont_cw = (unsigned char *)&fontmatrix_w24_h24_cw_2[0];
				font.pFont	   = (unsigned char *)&fontmatrix_w24_h24_2[0][0];
				font.s32Width  = 24;
				font.s32Height = 24;
				break;
		}

	}
	Param.width    = (font.s32Width * CHAR_WIDTH * g_TimeLen+2-1)/2*2;
	Param.height   = (font.s32Height * CHAR_HEIGHT+2-1)/2*2;
	int fontSize = Param.width*Param.height*2;
	if(pStBufFont->nSize < fontSize)
	{
//		printf("%d:%s()pStBufFont->nSize=%d\n",__LINE__,__FUNCTION__,pStBufFont->nSize);
		if(pStBufFont->pAddr != NULL)
		{
			free(pStBufFont->pAddr);
			pStBufFont->pAddr = NULL;
		}
		pStBufFont->nSize = fontSize;
		pStBufFont->pAddr = malloc(pStBufFont->nSize);
	}
	if(NULL == pStBufFont->pAddr)
	{
		pStBufFont->nSize = 0;
		ERR_PRINT(" Error !!! \n");
		goto END;
	}
	//ʱ��ת��Ϊ����

	if(get_the_product_type() == ENUM_PRODUCT_TYPE_3515_Yuhua)
	{
	    memset(pStBufFont->pAddr, 0, pStBufFont->nSize);
		s32Ret = HI_TimeToPointFont2(&osdTime, pStBufFont->pAddr, &font, channel,type);
	}
	else
	{
		s32Ret = HI_TimeToPointFont(&osdTime, pStBufFont->pAddr, &font, channel,type,CHAR_WIDTH,CHAR_HEIGHT);
	}
	if(HI_SUCCESS != s32Ret)
	{
		ERR_PRINT(" Error !!! \n");
		goto END;
	}
	Param.width    = (font.s32Width * CHAR_WIDTH * g_TimeLen+2-1)/2*2;
	Param.height   = (font.s32Height * CHAR_HEIGHT+2-1)/2*2;
	Param.raster   = pStBufFont->pAddr;
	s32Ret = OSDCheckRegion(video_size, vstd, &Param);
    if(HI_SUCCESS != s32Ret)
    {  
		ERR_PRINT(" Error !!! \n");
		goto END;
    }
	Param.x = Param.x - Param.x%8;
	Param.y = Param.y - Param.y%2;
	if((*pOsdHandle) ==0x123456)
	{
		if( !pOsdTimeParam->bUsedVi )
		{		
			stRgnAttr.enType = OVERLAY_RGN;
			stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1555;
			stRgnAttr.unAttr.stOverlay.stSize.u32Width	= Param.width;
			stRgnAttr.unAttr.stOverlay.stSize.u32Height = Param.height;
			stRgnAttr.unAttr.stOverlay.u32BgColor = 0x00ffffff;

			stOverlayExChn.enModId = HI_ID_GROUP;
            stOverlayExChn.s32DevId = venc_grp;
            stOverlayExChn.s32ChnId = 0;
            
            memset(&stOverlayExChnAttr,0,sizeof(stOverlayExChnAttr));
            stOverlayExChnAttr.bShow = HI_TRUE;
            stOverlayExChnAttr.enType = OVERLAY_RGN;
            stOverlayExChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = Param.x;
            stOverlayExChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = Param.y;
            stOverlayExChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0x40;
            stOverlayExChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 0x40;
            stOverlayExChnAttr.unChnAttr.stOverlayChn.u32Layer = Param.index;
            stOverlayExChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_FALSE;
            stOverlayExChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 0;
		}
		else
		{
			//stViOverlay
			stRgnAttr.enType = OVERLAYEX_RGN;
			stRgnAttr.unAttr.stOverlayEx.enPixelFmt = PIXEL_FORMAT_RGB_1555;
			stRgnAttr.unAttr.stOverlayEx.u32BgColor =  0x00ffffff;
			stRgnAttr.unAttr.stOverlayEx.stSize.u32Height =Param.height;
			stRgnAttr.unAttr.stOverlayEx.stSize.u32Width= Param.width;
			
			stOverlayExChnAttr.enType = OVERLAYEX_RGN;
			stOverlayExChnAttr.bShow = HI_TRUE;
			stOverlayExChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = Param.x;
			stOverlayExChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = Param.y;
			stOverlayExChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha =0;
			stOverlayExChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha = 255;
			stOverlayExChnAttr.unChnAttr.stOverlayExChn.u32Layer = Param.index+1;
			if(get_the_product_type() == ENUM_PRODUCT_TYPE_3515_Yuhua)
	        {
	          stOverlayExChnAttr.unChnAttr.stOverlayExChn.u32Layer += 10;
	        }
			stOverlayExChn.enModId = HI_ID_VIU;
			stOverlayExChn.s32DevId = 0;//GetViDevByChn(channel);
			stOverlayExChn.s32ChnId = GetViChnByChn(channel);	  
        }
		
		if( type == OSD_TYPE_VENC)
		{
			*pOsdHandle = GetOsdBaseHandleByChn(channel,CHL_MAIN_T)+Param.index;
			venc_grp = GetVencGrpByChn(channel,CHL_MAIN_T);
		}
		else if( type == OSD_TYPE_SNAP )
		{
			*pOsdHandle = GetOsdBaseHandleByChn(channel,CHL_JPEG_T)+Param.index;
		}
		else if(OSD_TYPE_EXT == type)
		{
			*pOsdHandle = GetOsdBaseHandleByChn(channel,CHL_2END_T)+Param.index;
		}
		else
		{
			assert(0);
		}

		s32Ret = HI_MPI_RGN_Create(*pOsdHandle, &stRgnAttr);
		if(HI_SUCCESS != s32Ret)
		{
			printf("%d:%s()HI_MPI_RGN_Create fail s32Ret=%#x\n",__LINE__,__FUNCTION__,s32Ret);
			*pOsdHandle =0x123456;
			goto END;
		}
		s32Ret = HI_MPI_RGN_AttachToChn(*pOsdHandle,&stOverlayExChn,&stOverlayExChnAttr);
		if(HI_SUCCESS != s32Ret)
		{
			printf("%d:%s()HI_MPI_RGN_AttachToChn fail s32Ret=%#x\n",__LINE__,__FUNCTION__,s32Ret);
			HI_MPI_RGN_Destroy(*pOsdHandle);
			*pOsdHandle =0x123456;
			goto END;
		}		
	}
	if(pCapDev->vi_chn[channel].venc_chn_t.venc_chn[0].started 
		|| pCapDev->vi_chn[channel].venc_chn_t.venc_chn[1].started
		|| pOsdTimeParam->bUsedVi )
	{
		if(pStBufRGB->nSize < (Param.height * Param.width * 2))
		{
			if(pStBufRGB->pAddr != NULL)
			{
				free(pStBufRGB->pAddr);
				pStBufRGB->pAddr = NULL;
			}
			pStBufRGB->nSize = ((Param.height * Param.width * 2));
			pStBufRGB->pAddr = malloc(pStBufRGB->nSize);
		}
		if(NULL == pStBufRGB->pAddr)
		{
			pStBufRGB->nSize = 0;
			ERR_PRINT(" Error !!! \n");
			goto END;
		}
		//������ɫ	
		s32Ret = HI_PointFont2RGB_Time(&Param, pStBufRGB->pAddr, channel);
		if(HI_SUCCESS != s32Ret)
		{
			ERR_PRINT(" Error !!! \n");
			goto END;
		}
		
		stBitmap.u32Width	   = Param.width;
		stBitmap.u32Height	   = Param.height;
		stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
		stBitmap.pData = (HI_VOID *)pStBufRGB->pAddr;
		s32Ret = HI_MPI_RGN_SetBitMap(*pOsdHandle,&stBitmap);
		if(s32Ret != HI_SUCCESS)
		{
		    ERR_PRINT("HI_MPI_RGN_SetBitMap failed with %#x!\n", s32Ret);
		    goto END;
		}		
	}
END:
	pthread_mutex_unlock( &(pOsdTimeParam->lock) );
	return s32Ret;
}
int OSD_LoadBmp(const char *filename, char** data, unsigned short * width, unsigned short * height )
#if 1
	{
		return 0;
	}
#else

{
    OSD_SURFACE_S Surface;    
	OSD_BITMAPFILEHEADER bmpFileHeader;    
	OSD_BITMAPINFO bmpInfo; 
	REGION_CTRL_PARAM_U *pParam = NULL;
	int tLen = 0;
	
	if(!data || !width || !height)
	{
	    return -1;
	}
	if(GetBmpInfo(filename,&bmpFileHeader,&bmpInfo) < 0)    
	{		
		LIBDVR_PRINT("GetBmpInfo err!\n");        
		return -1;    
	}    
	Surface.enColorFmt = OSD_COLOR_FMT_RGB1555; 
	
	*width = bmpInfo.bmiHeader.biWidth;
	*height = bmpInfo.bmiHeader.biHeight;
	
	tLen = 2 * bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biHeight + sizeof(REGION_CTRL_PARAM_U);
	*data = (REGION_CTRL_PARAM_U* )malloc(tLen);    
	if(NULL == *data)    
	{        
		LIBDVR_PRINT("malloc osd memroy err!\n");        
		return -1;    
	}   
	pParam = (REGION_CTRL_PARAM_U* )(*data);
	pParam->stBitmap.pData = (void* )((*data) + sizeof(REGION_CTRL_PARAM_U));
	CreateSurfaceByBitMap(filename,&Surface,(HI_U8*)(pParam->stBitmap.pData));  

    if(Surface.u16Width > 180 || Surface.u16Height > 140)
    {
        free(pParam->stBitmap.pData);
        return HI_FAILURE;
    }
	
	pParam->stBitmap.u32Width = Surface.u16Width;    
	pParam->stBitmap.u32Height = Surface.u16Height;    
	pParam->stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;    
	return 0;
}
#endif
int OSD_ReleaseBmp(char* data)
{
    if(data)
    {
        free(data);
    }
	return 0;
}
#if 0

HI_S32 SampleLoadBmp(const char *filename, REGION_CTRL_PARAM_U *pParam)

{
    OSD_SURFACE_S Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO bmpInfo;

    if(GetBmpInfo(filename,&bmpFileHeader,&bmpInfo) < 0)
    {
		printf("GetBmpInfo err!\n");
        return HI_FAILURE;
    }

    Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;

    pParam->stBitmap.pData = malloc(2*(bmpInfo.bmiHeader.biWidth)*(bmpInfo.bmiHeader.biHeight));

    if(NULL == pParam->stBitmap.pData)
    {
        printf("malloc osd memroy err!\n");
        return HI_FAILURE;
    }

    CreateSurfaceByBitMap(filename,&Surface,(HI_U8*)(pParam->stBitmap.pData));

    pParam->stBitmap.u32Width = Surface.u16Width;
    pParam->stBitmap.u32Height = Surface.u16Height;
    pParam->stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;

    return HI_SUCCESS;
}
#endif
int Osd_SetTitle(int channel, OSD_TYPE_E type, CAPTURE_TITLE_PARAM * pParam)
{
	HI_S32		s32Ret;
	BUFFER_S	*pStBuf = NULL;
	RGN_HANDLE*	pOsdHandle ;
	CAPTURE_TITLE_PARAM Param;
	MPP_CHN_S stOverlayExChn;
	RGN_CHN_ATTR_S stOverlayExChnAttr;
	RGN_ATTR_S stRgnAttr;
	BITMAP_S stBitmap;
	Capture_Dev_t* pCapDev = CaptureDevGetIns();
	VIDEO_NORM_E vstd = VIM_GetVsvd( VIM_GetIns() );
	int IMG_WIDTH = 1;
	int IMG_HEIGHT = 1; 
	CAPTURE_OSD_ZOOM OSDZoom;
	int video_size = 0;
	int iRet = 0;
	OSD_OVERLAY_PARAM* pOsdParam = &(g_osd_overlay_para[channel][pParam->index]);
	if( pOsdParam->bUsedVi && type != OSD_TYPE_VENC )
	{//!��vi���ӵ�ʱ�ֻ��Ҫ����һ·�Ϳ���
		return 0;
	}
#if defined(_USE_720P_MODULE_)	
	if(OSD_TYPE_EXT == type) 
	{
		iRet= GetOSDSizeExt(&OSDZoom, channel,CHL_2END_T);
	}
	else
	{
		iRet= GetOSDSizeExt(&OSDZoom, channel,CHL_MAIN_T);
	}
#else
	iRet = GetOSDSize(&OSDZoom, channel);
#endif
	if(0 == iRet)
	{
		IMG_WIDTH = OSDZoom.u.venc_zoom.char_width;
		IMG_HEIGHT = OSDZoom.u.venc_zoom.char_height;
	}
	else
	{
	    ERR_PRINT("\n");
	    return -1;
	}

	video_size = pCapDev->vi_chn[channel].venc_chn_t.venc_chn[CHL_MAIN_T].image_size;
	//���֧��4����������-----������ӣ���Ԥ�������������Ǹ������ڵ�����
	if( (GetChipByChn( channel) != Chip_GetLocalID(Chip_GetIns()) )
		||( (HI_U32)(pParam->index) >= OSD_OVERLAY_NR)) 
	{
		ERR_PRINT("Error :: Osd_SetTitle, chn:%d!\n", channel);
		return -1;
	}
	VENC_GRP venc_grp;
	if( type == OSD_TYPE_VENC)
	{
		pOsdHandle = &(pOsdParam->osdHandle);
		venc_grp = GetVencGrpByChn(channel,CHL_MAIN_T);
	}
	else if( type == OSD_TYPE_SNAP )
	{
		pOsdHandle = &(pOsdParam->SnapOsdHandle);
		venc_grp = GetVencGrpByChn(channel,CHL_JPEG_T);
	}
	else if(OSD_TYPE_EXT == type)
	{
		pOsdHandle = &(pOsdParam->ExtosdHandle);
		venc_grp = GetVencGrpByChn(channel,CHL_2END_T);
		video_size = pCapDev->vi_chn[channel].venc_chn_t.venc_chn[CHL_2END_T].image_size;
	}	
	else
	{
		assert(0);
	}
    
    if (video_size == CAPTURE_SIZE_CIF ||video_size == CAPTURE_SIZE_QCIF)
    {
        IMG_WIDTH =1;
        IMG_HEIGHT = 1;
    }

    if(pParam->index == OSD_OVERLAY_TIME)
    {
		 pthread_mutex_lock( &(pOsdParam->lock) );
    }	
	if( *pOsdHandle != 0x123456)
	{
		if( !pOsdParam->bUsedVi )
		{
			stOverlayExChn.enModId = HI_ID_GROUP;
			stOverlayExChn.s32DevId = venc_grp;
			stOverlayExChn.s32ChnId = 0;
		}
		else
		{
			stOverlayExChn.enModId = HI_ID_VIU;
			stOverlayExChn.s32DevId = 0;//GetViDevByChn(channel);
			stOverlayExChn.s32ChnId = GetViChnByChn(channel);
		}
	 	HI_MPI_RGN_DetachFrmChn(*pOsdHandle,&stOverlayExChn);
        HI_MPI_RGN_Destroy(*pOsdHandle);
		*pOsdHandle = 0x123456;
		pOsdParam->changed = OSD_OVERLAY_STATE_NORMAL;
	}
    if(pParam->index == OSD_OVERLAY_TIME)
    {
		 pthread_mutex_unlock( &(pOsdParam->lock) );
    }	
	 if(&pOsdParam->Param!=pParam)
	 {
		memcpy(&pOsdParam->Param, pParam, sizeof(CAPTURE_TITLE_PARAM));	
	 }
    if(pParam->index == OSD_OVERLAY_TIME)//ʱ�����
	{
		SYSTEM_TIME 	osdtime;
		time_t			curosdtime = time(NULL);
		struct tm		time;
		localtime_r(&curosdtime, &time);
		osdtime.second	= time.tm_sec;
		osdtime.minute	= time.tm_min;
		osdtime.hour	= time.tm_hour;
		osdtime.wday	= time.tm_wday;
		osdtime.day		= time.tm_mday;
		osdtime.month	= time.tm_mon + 1;
		osdtime.year	= time.tm_year + 1900;
		pOsdParam->time_hide = 0;
		pOsdParam->date_hide = 0;
		Osd_SetTime(channel, type, &osdtime, 0);
	}	
	else if(pParam->index == OSD_OVERLAY_TITLE 
		|| pParam->index == OSD_OVERLAY_MULTILINE
		|| pParam->index == OSD_OVERLAY_PRESET)//ͨ�����Ʊ���Ͷ��е���
	{				
		//����Ѿ����ڵ���������������
		if(pParam->enable == 0)
		{
	//		LIBDVR_PRINT("\n");
			return 0;
		}
		pStBuf = &pOsdParam->StBufRGB;
       	memcpy(&Param, pParam, sizeof(CAPTURE_TITLE_PARAM));
		int fontSize;
		if(video_size == 9)
		{
			POINT_FONT_S	font;
			font.pFont_cw = (unsigned char *)&fontmatrix_w24_h32_cw_2[0];
			font.pFont	   = (unsigned char *)&fontmatrix_w24_h32_2[0][0];
			font.s32Width  = 24;
			font.s32Height = 32;
			VD_SIZE size;
			size.w = strlen(Param.titleStr) * font.s32Width;
			size.h = font.s32Height;
		//	printf("%d:%s()Param.titleStr=%s\n",__LINE__,__FUNCTION__,Param.titleStr);
			memset(Param.raster,0,1024*8*8);
			TextToRaster(Param.raster, &size, &font, Param.titleStr);
			Param.width    = (font.s32Width * IMG_WIDTH * strlen(Param.titleStr)+2-1)/2*2;
			Param.height   = (font.s32Height * IMG_HEIGHT+2-1)/2*2;
			fontSize = Param.width*Param.height*2;
		}
		else if(video_size == 10)
		{
			POINT_FONT_S	font;
			font.pFont_cw = (unsigned char *)&fontmatrix_w32_h48_cw_2[0];
			font.pFont	   = (unsigned char *)&fontmatrix_w32_h48_2[0][0];
			font.s32Width  = 40;
			font.s32Height = 45;
			VD_SIZE size;
			size.w = strlen(Param.titleStr) * font.s32Width;
			size.h = font.s32Height;
		//	printf("%d:%s()Param.titleStr=%s\n",__LINE__,__FUNCTION__,Param.titleStr);
			memset(Param.raster,0,1024*8*8);
			TextToRaster(Param.raster, &size, &font, Param.titleStr);
			Param.width    = (font.s32Width * IMG_WIDTH * strlen(Param.titleStr)+2-1)/2*2;
			Param.height   = (font.s32Height * IMG_HEIGHT+2-1)/2*2;
			fontSize = Param.width*Param.height*2;
		}
		else if(video_size == 0)
		{
			Param.width = (Param.width+2-1)/2*2;
			Param.height = (Param.height+2-1)/2*2;
			fontSize = Param.height * Param.width * 2 * IMG_WIDTH * IMG_HEIGHT;
		}
		if(pStBuf->nSize < fontSize)
		{
			if(pStBuf->pAddr != NULL)
			{
				free(pStBuf->pAddr);
				pStBuf->pAddr = NULL;
			}
			pStBuf->nSize = fontSize;
			pStBuf->pAddr = malloc(pStBuf->nSize);
			assert( pStBuf->pAddr != NULL );
		}
		s32Ret = HI_PointFont2RGB_Title(&Param, pStBuf->pAddr, channel,type,IMG_WIDTH,IMG_HEIGHT);
		if(HI_SUCCESS != s32Ret)
		{
			ERR_PRINT("HI_PointFont2RGB-err:%#x\n", s32Ret);
			return s32Ret;
		}		
		Param.width *= IMG_WIDTH;
		Param.height *= IMG_HEIGHT;
		s32Ret = OSDCheckRegion(video_size, vstd, &Param);
		if(HI_SUCCESS != s32Ret)
		{  
			ERR_PRINT("ERROR");
			return -1;
		}
		Param.x = Param.x - Param.x%8;
	    Param.y = Param.y - Param.y%2;
		
		if( (*pOsdHandle) == 0x123456)
		{
			if( !pOsdParam->bUsedVi )
			{
#if 0//yangsc�ò���
			    stRgnAttr.enType = OVERLAY_REGION;//��������
				stRgnAttr.unAttr.stOverlay.VeGroup		 	= venc_grp;//�������������ͨ����
				stRgnAttr.unAttr.stOverlay.bPublic 		 	= HI_FALSE;
				stRgnAttr.unAttr.stOverlay.stRect.s32X      	= Param.x;
				stRgnAttr.unAttr.stOverlay.stRect.s32Y      	= Param.y;
				stRgnAttr.unAttr.stOverlay.stRect.u32Height 	= Param.height;
				stRgnAttr.unAttr.stOverlay.stRect.u32Width  	= Param.width;
				stRgnAttr.unAttr.stOverlay.enPixelFmt		= PIXEL_FORMAT_RGB_1555;
				stRgnAttr.unAttr.stOverlay.u32FgAlpha = 0x40;
				stRgnAttr.unAttr.stOverlay.u32BgAlpha = 0x40;
				stRgnAttr.unAttr.stOverlay.u32BgColor = 0x0000;
#endif
			}
			else
			{
				stRgnAttr.enType = OVERLAYEX_RGN;
				stRgnAttr.unAttr.stOverlayEx.enPixelFmt = PIXEL_FORMAT_RGB_1555;
				stRgnAttr.unAttr.stOverlayEx.u32BgColor =  0x00;
				stRgnAttr.unAttr.stOverlayEx.stSize.u32Height = Param.height;
				stRgnAttr.unAttr.stOverlayEx.stSize.u32Width= Param.width;
				stOverlayExChnAttr.enType = OVERLAYEX_RGN;
				stOverlayExChnAttr.bShow = HI_TRUE;
				stOverlayExChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = Param.x;
				stOverlayExChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = Param.y;
				stOverlayExChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha = 0;
				stOverlayExChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha = 255;
				stOverlayExChnAttr.unChnAttr.stOverlayExChn.u32Layer = Param.index+1;
				stOverlayExChn.enModId = HI_ID_VIU;
				stOverlayExChn.s32DevId = 0;//GetViDevByChn(channel);
				stOverlayExChn.s32ChnId = GetViChnByChn(channel);
			}
			if( type == OSD_TYPE_VENC)
			{
				*pOsdHandle = GetOsdBaseHandleByChn(channel,CHL_MAIN_T)+Param.index;
				venc_grp = GetVencGrpByChn(channel,CHL_MAIN_T);
			}
			else if( type == OSD_TYPE_SNAP )
			{
				*pOsdHandle = GetOsdBaseHandleByChn(channel,CHL_JPEG_T)+Param.index;
			}
			else if(OSD_TYPE_EXT == type)
			{
				*pOsdHandle = GetOsdBaseHandleByChn(channel,CHL_2END_T)+Param.index;
			}
			else
			{
				assert(0);
			}
			s32Ret = HI_MPI_RGN_Create(*pOsdHandle, &stRgnAttr);
			if(HI_SUCCESS != s32Ret)
			{
				printf("%d:%s()HI_MPI_RGN_Create fail s32Ret=%#x\n",__LINE__,__FUNCTION__,s32Ret);
				*pOsdHandle = 0x123456;
				return -1;
			}
			s32Ret = HI_MPI_RGN_AttachToChn(*pOsdHandle,&stOverlayExChn,&stOverlayExChnAttr);
			if(HI_SUCCESS != s32Ret)
			{
				printf("%d:%s()HI_MPI_RGN_AttachToChn fail s32Ret=%#x\n",__LINE__,__FUNCTION__,s32Ret);
				HI_MPI_RGN_Destroy(*pOsdHandle);
				*pOsdHandle = 0x123456;
				return -1;
			}		
		}
		stBitmap.u32Width	   = Param.width;
		stBitmap.u32Height	   = Param.height;
		stBitmap.enPixelFormat = PIXEL_FORMAT_RGB_1555;
		stBitmap.pData = (HI_VOID *)pStBuf->pAddr;
		s32Ret = HI_MPI_RGN_SetBitMap(*pOsdHandle,&stBitmap);
		if(s32Ret != HI_SUCCESS)
		{
			ERR_PRINT("HI_MPI_RGN_SetBitMap failed with %#x!\n", s32Ret);
			return -1;
		}
	}
	else if(pParam->index == OSD_OVERLAY_MARKER)//ͨ��ˮӡ����
	{
#if 0//yangsc�ò���
		if(pParam->enable == 0)
		{
		//LIBDVR_PRINT("\n");
		return 0;
		}

	    REGION_ATTR_S stRgnAttr; 
	    REGION_CRTL_CODE_E enCtrl; 
		REGION_CTRL_PARAM_U* markerPara = NULL;
		if( (*pOsdHandle) ==0x123456)
		{
			if(!pParam->raster)
			{
			    return -1;
			}
			markerPara = (REGION_CTRL_PARAM_U* )pParam->raster;
			markerPara->stBitmap.pData = (HI_VOID *)(pParam->raster + sizeof(REGION_CTRL_PARAM_U));
			Param.x = pParam->x;
			Param.y = pParam->y;
			Param.width = markerPara->stBitmap.u32Width;
			Param.height = markerPara->stBitmap.u32Height;
			/*
			printf("____________Param.x[%d] Param.y[%d] Param.width[%d] Param.height[%d]\n", 
				Param.x, Param.y, Param.width, Param.height);
				*/
			s32Ret = OSDCheckRegion(video_size, vstd, &Param);
			Param.x = Param.x - Param.x%8;
	        Param.y = Param.y - Param.y%2;
			/*
			printf("++++++++++++Param.x[%d] Param.y[%d] Param.width[%d] Param.height[%d]\n", 
				Param.x, Param.y, Param.width, Param.height);
				*/
	        if(HI_SUCCESS != s32Ret)
	        {  
	            return -1;
	        }
			
		    stRgnAttr.enType = OVERLAY_REGION;// VIOVERLAY_REGION
		    stRgnAttr.unAttr.stOverlay.bPublic = HI_FALSE; 
		    stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1555; 
			stRgnAttr.unAttr.stOverlay.stRect.s32X = Param.x; 
		    stRgnAttr.unAttr.stOverlay.stRect.s32Y = Param.y; 
		    stRgnAttr.unAttr.stOverlay.stRect.u32Width = Param.width; 
		    stRgnAttr.unAttr.stOverlay.stRect.u32Height = Param.height; 
		    stRgnAttr.unAttr.stOverlay.u32BgAlpha = 128; 
		    stRgnAttr.unAttr.stOverlay.u32FgAlpha = 128; 
		    stRgnAttr.unAttr.stOverlay.u32BgColor = 0; 
		    stRgnAttr.unAttr.stOverlay.VeGroup = venc_grp; 
		    s32Ret = HI_MPI_VPP_CreateRegion(&stRgnAttr, pOsdHandle); 
		    if(s32Ret != HI_SUCCESS) 
		    { 
		       ERR_PRINT("HI_MPI_VPP_CreateRegion err 0x%x!\n",s32Ret); 
		       return HI_FAILURE; 
		    } 
			
		    enCtrl = REGION_SHOW; 
		    s32Ret = HI_MPI_VPP_ControlRegion(*pOsdHandle, enCtrl, markerPara);  
	        if(s32Ret != HI_SUCCESS) 
	        { 
	            ERR_PRINT("show faild 0x%x!\n",s32Ret); 
	            return HI_FAILURE; 
	        } 
			
	        enCtrl = REGION_SET_BITMAP; 
	        s32Ret = HI_MPI_VPP_ControlRegion(*pOsdHandle, enCtrl, markerPara); 
	        if(s32Ret != HI_SUCCESS) 
	        { 
	            ERR_PRINT("set region bitmap faild 0x%x!\n",s32Ret); 
	            return HI_FAILURE; 
	        } 
		}
#endif
	}
	return 0;
}
int Osd_SetTimeFmt(int channel, const SYSTEM_TIME * pTime, int datefmt,int datespc,int timefmt)
{	
	int s32Ret = 0;
	if( channel < 0 )
	{
		return -1;
	}

	g_osd_overlay_para[channel][OSD_OVERLAY_TIME].osdTime.dateFmt = datefmt;
	g_osd_overlay_para[channel][OSD_OVERLAY_TIME].osdTime.dateSpec = datespc;
	g_osd_overlay_para[channel][OSD_OVERLAY_TIME].osdTime.timeFmt = timefmt;
	return s32Ret;
}
int Osd_SetTimeFmt2(int channel, const SYSTEM_TIME * pTime, int datefmt,int datespc,int timefmt,int time_hide,int date_hide)
{	
	int s32Ret = 0;
	if( channel < 0 )
	{
		return -1;
	}

	g_osd_overlay_para[channel][OSD_OVERLAY_TIME].osdTime.dateFmt = datefmt;
	g_osd_overlay_para[channel][OSD_OVERLAY_TIME].osdTime.dateSpec = datespc;
	g_osd_overlay_para[channel][OSD_OVERLAY_TIME].osdTime.timeFmt = timefmt;
	g_osd_overlay_para[channel][OSD_OVERLAY_TIME].time_hide = time_hide;
	g_osd_overlay_para[channel][OSD_OVERLAY_TIME].date_hide = date_hide;
	return s32Ret;
}

int SetSnapTitleTime(int chn)
{
	int u32Ret =0; 

	SYSTEM_TIME 	systime;

	time_t			curosdtime = time(NULL);
	struct tm		time;

	localtime_r(&curosdtime, &time);
	systime.second	= time.tm_sec;
	systime.minute	= time.tm_min;
	systime.hour	= time.tm_hour;
	systime.wday	= time.tm_wday;
	systime.day		= time.tm_mday;
	systime.month	= time.tm_mon + 1;
	systime.year	= time.tm_year + 1900;


	u32Ret = Osd_SetTime(chn, OSD_TYPE_SNAP, &systime, 0);
	if(u32Ret != HI_SUCCESS)
	{
		LIBDVR_PRINT(" hi_CaptureSetTimeGrp faile!\n");
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}


#if 0
void Osd_PrintRegion(REGION_ATTR_S* pstRgnAttr)
{
	printf("\n  enType: %d \n",pstRgnAttr->enType );
	printf("\n REGION_ATTR_U-stCover:\n    ViDevId:%d\n      ViChn:%d\n       bIsPublic:%d \n",
			pstRgnAttr->unAttr.stCover.ViDevId, pstRgnAttr->unAttr.stCover.ViChn,
			pstRgnAttr->unAttr.stCover.bIsPublic);
	printf("        u32Layer:%d \n       stRect:[x[%d],y[%d],w[%d],h[%d]]\n        u32Color:%d\n",
			pstRgnAttr->unAttr.stCover.u32Layer, pstRgnAttr->unAttr.stCover.stRect.s32X,
			pstRgnAttr->unAttr.stCover.stRect.s32Y, pstRgnAttr->unAttr.stCover.stRect.u32Width,
			pstRgnAttr->unAttr.stCover.stRect.u32Height, pstRgnAttr->unAttr.stCover.u32Color);
	printf("         OVERLAY_ATTR_S:\n            VeGroup:%d\nbPublic:%d\n      \
			stRect:[x[%d],y[%d],w[%d],h[%d]]\n           enPixelFmt:%d\n        \
			u32FgAlpha:%d\n      u32BgAlpha:%d\n    \
			u32BgColor:%d\n", pstRgnAttr->unAttr.stOverlay.VeGroup, pstRgnAttr->unAttr.stOverlay.bPublic,
			pstRgnAttr->unAttr.stOverlay.stRect.s32X, pstRgnAttr->unAttr.stOverlay.stRect.s32Y,
			pstRgnAttr->unAttr.stOverlay.stRect.u32Width, pstRgnAttr->unAttr.stOverlay.stRect.u32Height,
			pstRgnAttr->unAttr.stOverlay.u32BgColor, pstRgnAttr->unAttr.stOverlay.u32FgAlpha,
			pstRgnAttr->unAttr.stOverlay.u32BgAlpha, pstRgnAttr->unAttr.stOverlay.u32BgColor);
}
#endif
void InitOsdSize(int channel)
{
	Capture_Dev_t* pThis = CaptureDevGetIns();
	VENC_CHN_T* pVencChn = &(pThis->vi_chn[channel].venc_chn_t);
#if defined(_USE_720P_MODULE_)  //720P�豸��������ʹ�ò�ͬ�ı���ͨ����OSD��Ҫ������������
    pVencChn->venc_chn[CHL_MAIN_T].osd_venc_zoom.u.size = 0x02020202;
	pVencChn->venc_chn[CHL_2END_T].osd_venc_zoom.u.size = 0x01010101;
#else
	pVencChn->venc_chn[CHL_MAIN_T].osd_venc_zoom.u.size = 0x01010101;
#endif
}
int SetOSDSize(CAPTURE_OSD_ZOOM* OSDZoom, int channel)
{
    if(!OSDZoom || channel < 0 || channel >= N_SYS_CH)
    {
        LIBDVR_PRINT("SetOSDSize error, OSDZoom[%p], channel[%d]\n",  OSDZoom, channel);
        return -1;
    }

        //Ӧ��Ŀǰ���ݵ���1����2
        if (OSDZoom->u.size == 1 || OSDZoom->u.size == 2 )
        {
            OSDZoom->u.size = OSDZoom->u.size * 0x01010101;
        }
        
	Capture_Dev_t* pThis = CaptureDevGetIns();
	VENC_CHN_T* pVencChn = &(pThis->vi_chn[channel].venc_chn_t);
	if(pVencChn->venc_chn[CHL_MAIN_T].osd_venc_zoom.u.size != OSDZoom->u.size)
	{
        pVencChn->venc_chn[CHL_MAIN_T].osd_venc_zoom.u.size = OSDZoom->u.size;	
	    FlushChnOSD(channel);	
	}
	return 0;
}

int GetOSDSize(CAPTURE_OSD_ZOOM* OSDZoom, int channel)
{
    if(!OSDZoom || channel < 0 || channel >= N_SYS_CH)
    {
        LIBDVR_PRINT("GetOSDSize error, OSDZoom[%p], channel[%d]\n",  OSDZoom, channel);
        return -1;
    }
	Capture_Dev_t* pThis = CaptureDevGetIns();
	VENC_CHN_T* pVencChn = &(pThis->vi_chn[channel].venc_chn_t);
	if(0 == pVencChn->venc_chn[CHL_MAIN_T].osd_venc_zoom.u.size)
	{
	    //for debug,never exec
	    InitOsdSize(channel);
		assert(0);
	}
    OSDZoom->u.size = pVencChn->venc_chn[CHL_MAIN_T].osd_venc_zoom.u.size;
	
	return 0;
}
////720P�豸��������ʹ�ò�ͬ�ı���ͨ����OSD��Ҫ������������
int GetOSDSizeExt(CAPTURE_OSD_ZOOM* OSDZoom, int channel,int type)
{
    if(!OSDZoom || channel < 0 || channel >= N_SYS_CH)
    {
        LIBDVR_PRINT("GetOSDSizeExt error, OSDZoom[%p], channel[%d]\n",  OSDZoom, channel);
        return -1;
    }
	Capture_Dev_t* pThis = CaptureDevGetIns();
	VENC_CHN_T* pVencChn = &(pThis->vi_chn[channel].venc_chn_t);
	if(0 == pVencChn->venc_chn[type].osd_venc_zoom.u.size)
	{
	    //for debug,never exec
	    InitOsdSize(channel);
		assert(0);
	}
    OSDZoom->u.size = pVencChn->venc_chn[type].osd_venc_zoom.u.size;
	
	return 0;
}

int Osd_GetFontBgColor(OSD_FONT_BG_COLOR *rgbColor)
{
    if(rgbColor 
		&& rgbColor->channle < N_SYS_CH 
		&& rgbColor->channle >= 0
		&& rgbColor->osdIndex >= 0
		&& rgbColor->osdIndex < OSD_OVERLAY_NR)
    {
        rgbColor->rgbFontColor = g_osd_overlay_para[rgbColor->channle][rgbColor->osdIndex].Param.fg_color ;
		rgbColor->rgbFontBgColor = g_osd_overlay_para[rgbColor->channle][rgbColor->osdIndex].Param.bg_color; 
		return 0;
       
    }
	 return -1;
}

int Osd_SetFontBgColor(OSD_FONT_BG_COLOR *rgbColor)
{
	 if(rgbColor 
		&& rgbColor->channle < N_SYS_CH 
		&& rgbColor->channle >= 0
		&& rgbColor->osdIndex >= 0
		&& rgbColor->osdIndex < OSD_OVERLAY_NR)
    {
	    g_osd_overlay_para[rgbColor->channle][rgbColor->osdIndex].Param.fg_color = rgbColor->rgbFontColor;
		g_osd_overlay_para[rgbColor->channle][rgbColor->osdIndex].Param.bg_color = rgbColor->rgbFontBgColor; 
		return 0;   
    }
	return -1;
}

short ConvertRGB32TORGB1555(int rgb)
{
    int uR,uG,uB,uA;
	
	uR = rgb & 0xff;
	uG = (rgb & 0xff00) >> 8;
	uB = (rgb & 0xff0000) >> 16;
	uA = (rgb & 0xff000000) >> 24;
    int rgb1555 = (((uR>>3) & 0x1f)<<11) | (((uG>>3) & 0x1f))<<5 | ((uB>>3) & 0x1f);
    if(uA == 1 ) rgb1555 = rgb1555 | 0x8000;
	return (short)rgb1555;
}

