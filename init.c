#include "DVRDEF.h"
#include "initial.h"
#include "video.h"
//#include "Mixer.h"
#include "sample_comm.h"

int g_voOutputType = VO_OUTPUT_TYPE_1920_1080;    ///< 默认是1920高清
enum video_standard_t  g_videostd=VIDEO_STANDARD_PAL;
enum capture_size_t g_videosize = CAPTURE_SIZE_1080P;

int vim_init(VIDEO_NORM_E vstd){
	HI_S32 s32Ret;
    enum capture_size_t video_size;
	VI_WORK_MODE_E enViWorkMode;

    video_size = CAPTURE_SIZE_720P;
    enViWorkMode = VI_WORK_MODE_2Multiplex;

    s32Ret = ViPubConfig(enViWorkMode, VIDEO_STANDARD_PAL);
    if(s32Ret != 0)
    {
        return s32Ret;
    }

    s32Ret = ViChnConfig(0, g_videosize, g_videostd);//chn pThis->m_vstd
    if(s32Ret != 0)
    {
        return s32Ret;
    }
}

/*****************************************************************************
功能:设置VI公共属性，   使能VI设备
*****************************************************************************/
HI_S32 ViPubConfig(VI_WORK_MODE_E enViWorkMode, VIDEO_NORM_E  enViNorm)//VI_DEV ViDev,
{
    HI_S32 s32Ret;
	VI_DEV_ATTR_S    stViDevAttr;
	int i = 0;
    memset(&stViDevAttr, 0, sizeof(VI_DEV_ATTR_S));
	stViDevAttr.enIntfMode = VI_MODE_BT656;
	stViDevAttr.enWorkMode = enViWorkMode;
	if(g_videosize==CAPTURE_SIZE_D1)//SD是隔行
		stViDevAttr.enScanMode = VI_SCAN_INTERLACED;
	else
		stViDevAttr.enScanMode = VI_SCAN_PROGRESSIVE;

		stViDevAttr.enDataSeq = VI_INPUT_DATA_UYVY;
	for(i=0; i<4; i++)
		stViDevAttr.s32AdChnId[i] = -1;

	VI_DEV ViDev = 0;//not know viDev, 

	switch(ViDev)
	{
		case 0:
			stViDevAttr.au32CompMask[0] = 0xFF000000;
			stViDevAttr.au32CompMask[1] = 0x00;
			break;
		case 1:
			stViDevAttr.au32CompMask[0] = 0xFF0000;
			stViDevAttr.au32CompMask[1] = 0x00;
			break;
		case 2:
			stViDevAttr.au32CompMask[0] = 0xFF00;
			stViDevAttr.au32CompMask[1] = 0x00;
			break;
		case 3:
			stViDevAttr.au32CompMask[0] = 0xFF;
			stViDevAttr.au32CompMask[1] = 0x00;
			break;
	}
	 s32Ret = HI_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);
	 if (s32Ret != HI_SUCCESS)
	 {
		 SAMPLE_PRT("HI_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
		 return HI_FAILURE;
	 }
	 
	 s32Ret = HI_MPI_VI_EnableDev(ViDev);
	 if (s32Ret != HI_SUCCESS)
	 {
		 SAMPLE_PRT("HI_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
		 return HI_FAILURE;
	 }

    LIBDVR_PRINT("enable the vi device %d ,it is in %s\n",ViDev,__FUNCTION__);
    return HI_SUCCESS;

}

/*****************************************************************************
功能:设置VI通道属性，使能VI通道
*****************************************************************************/
HI_S32 ViChnConfig(int channel, HI_U16 image_size, VIDEO_NORM_E normal)
{
    HI_S32 s32Ret;
    VI_CHN_ATTR_S ViChnAttr;
    VI_DEV ViDev;
    VI_CHN ViChn;
	VPSS_GRP vpss_grp;
	VPSS_GRP vpss_jpeg_grp;
	VPSS_GRP vpss_vdec_grp;
	VPSS_GRP vpss_hdclip_grp;
	VPSS_GRP vpss_vdec_snap_grp;
    HI_U32 u32SrcFrmRate;
	SIZE_S stSize;
    ViDev = GetViDevByChn(channel);
    ViChn = GetViChnByChn(channel);
	vpss_grp = GetVpssGrpByChn(channel);
	vpss_jpeg_grp = GetVpssJpegGrpByChn(channel);
	vpss_vdec_grp = GetVpssVdecGrpByChn(channel);
	vpss_hdclip_grp = GetVpssHdClipGrpByChn(channel);
	vpss_vdec_snap_grp = GetVpssJpegManualGrpByChn(channel);
    memset(&ViChnAttr, 0, sizeof(VI_CHN_ATTR_S));

    ViChnAttr.stCapRect.s32X = 0;
    ViChnAttr.stCapRect.s32Y = 0;
    ViChnAttr.stCapRect.u32Width  = GetWidthBySize(image_size, VIM_GetVsvd( VIM_GetIns()));
    ViChnAttr.stCapRect.u32Height = GetHeightBySize(image_size, VIM_GetVsvd( VIM_GetIns()));
	ViChnAttr.enCapSel = VI_CAPSEL_BOTH;
    ViChnAttr.stDestSize.u32Width = ViChnAttr.stCapRect.u32Width;
    ViChnAttr.stDestSize.u32Height = ViChnAttr.stCapRect.u32Height;
    ViChnAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;  
    ViChnAttr.bMirror = HI_FALSE;
    ViChnAttr.bFlip = HI_FALSE;
    ViChnAttr.bChromaResample = HI_FALSE;
    ViChnAttr.s32SrcFrameRate = -1;
    ViChnAttr.s32FrameRate = -1;

    s32Ret = HI_MPI_VI_SetChnAttr(ViChn, &ViChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    
    s32Ret = HI_MPI_VI_EnableChn(ViChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
	s32Ret = HI_MPI_VI_EnableChn422to420(ViChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
	stSize.u32Width= 1920;
	stSize.u32Height = 1080;
	printf("vpss_grp=%d\n",vpss_grp);
	s32Ret = SAMPLE_COMM_VPSS_Start(vpss_grp, &stSize, VPSS_MAX_CHN_NUM,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!\n");
        return HI_FAILURE;
    }

    s32Ret = vibind2vpss(ViChn,vpss_grp);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Vi bind Vpss failed!\n");
        return HI_FAILURE;
    }
	//绑定JPEG抓图VPSS
	stSize.u32Width= 1920;
	stSize.u32Height = 1080;
	s32Ret = SAMPLE_COMM_VPSS_Start(vpss_jpeg_grp, &stSize, VPSS_MAX_CHN_NUM,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!vpss_jpeg_grp=%d\n",vpss_jpeg_grp);
        return HI_FAILURE;
    }

    s32Ret = vibind2vpss(ViChn,vpss_jpeg_grp);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Vi bind Vpss failed!\n");
        return HI_FAILURE;
    }
#if 0//和抓拍共用一个VPSS
	//绑定HD ZOOM VPSS
	stSize.u32Width= 1920;
	stSize.u32Height = 1080;
	s32Ret = SAMPLE_COMM_VPSS_Start(vpss_hdclip_grp, &stSize, VPSS_MAX_CHN_NUM,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!vpss_jpeg_grp=%d\n",vpss_jpeg_grp);
        return HI_FAILURE;
    }

	s32Ret = vibind2vpss(ViChn,vpss_hdclip_grp);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Vi bind Vpss failed!\n");
        return HI_FAILURE;
    }
#endif
	//绑定解码VPSS
	stSize.u32Width= 1920;
	stSize.u32Height = 1080;
	s32Ret = SAMPLE_COMM_VPSS_Start(vpss_vdec_grp, &stSize, VPSS_MAX_CHN_NUM,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!vpss_jpeg_grp=%d\n",vpss_jpeg_grp);
        return HI_FAILURE;
    }
#if 0
	stSize.u32Width= 1920;
	stSize.u32Height = 1080;
	s32Ret = SAMPLE_COMM_VPSS_Start(vpss_vdec_snap_grp, &stSize, 1,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Vpss failed!vpss_jpeg_grp=%d\n",vpss_jpeg_grp);
        return HI_FAILURE;
    }
#endif
    LIBDVR_PRINT("enable the vi channel %d , ViDev[%d], ViChn[%d], it is in %s\n",channel, ViDev, ViChn, __FUNCTION__);
    return HI_SUCCESS;

}
#if 0
void VEncChn_Init_General(VENC_CHN_T *vchn, int chn)
{
	LIBDVR_PRINT("func:%s-chn:%d\n", __func__, chn);
	memset( vchn, 0, sizeof(VENC_CHN_T) );
	HI_S32 s32Ret;
	int j;
	//VI_DEV ViDev;
	//VI_CHN ViChn;
	VENC_GRP VeGroup ;
	VPSS_GRP VpssGrp;
	VPSS_GRP VpssJpegGrp;
	
	vchn->Chn = chn;

	//ViDev = GetViDevByChn(chn);
	//ViChn = GetViChnByChn(chn);
	VIDEO_NORM_E vstd = VIM_GetVsvd( VIM_GetIns() );


	VENC_CHN_INFO_t* pvenc;
	for(j = 0; j < 2; j++)
	{
		//!TODO: 这里初始值，后面会修改成具体的
		pvenc = &vchn->venc_chn[j];
		pvenc->vencChn = GetVencChnByChn(chn, j);
		pvenc->created = 0;
		pvenc->started = 0;
		pvenc->enable = 0;
		pvenc->pic_width = GetWidthBySize(CAPTURE_SIZE_CIF,vstd);
		pvenc->pic_height = GetHeightBySize(CAPTURE_SIZE_CIF,vstd);
		pvenc->frame_rate = 20;
		pvenc->image_size = CAPTURE_SIZE_CIF;
		pvenc->venc_fd = -1;
		pvenc->gop = 50;
		pvenc->cur_frame_rate = 20;
	}

	vchn->venc_jpeg.created = 0;
	vchn->venc_jpeg.started = 0;
	vchn->venc_jpeg.nFps = 0;

	InitOsdSize(chn);
	
	//!确保此vi已经打开
	s32Ret = VIM_Start( VIM_GetIns(), chn );
	HI_ASSERT( s32Ret == HI_SUCCESS);

	//!初始化编码组
	VeGroup = GetVencGrpByChn(chn,CHL_MAIN_T);
	s32Ret = HI_MPI_VENC_CreateGroup(VeGroup);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("create group[%d] error[%#x]\n", VeGroup, s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);

	//!创建编码器
	VENC_CHN_ATTR_S stAttr;
	VENC_ATTR_H264_S stH264Attr;
	VENC_ATTR_H264_CBR_S    stH264Cbr;
	VENC_ATTR_H264_VBR_S	stH264Vbr;

	VI_DEV chn_dev = GetViDevByChn(chn);
	VI_CHN chn_vi = GetViChnByChn(chn);
	VI_MANAGER_T* pVIM=VIM_GetIns();
	enum capture_size_t CAPTURE_SIZE=pVIM->m_vi_chn_size[chn_dev][chn_vi];
	stAttr.stVeAttr.enType = PT_H264;	
	stH264Attr.u32MaxPicWidth = GetWidthBySize( CAPTURE_SIZE, vstd );
	stH264Attr.u32MaxPicHeight =GetHeightBySize( CAPTURE_SIZE, vstd );	
	stH264Attr.u32PicWidth = stH264Attr.u32MaxPicWidth;/*the picture width*/
	stH264Attr.u32PicHeight = stH264Attr.u32MaxPicHeight;/*the picture height*/
	stH264Attr.u32BufSize  = (stH264Attr.u32PicWidth * stH264Attr.u32PicHeight+64-1)/64*64;/*stream buffer size*/
	stH264Attr.u32Profile  = 0;/*0: baseline; 1:MP; 2:HP   ? */
	stH264Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
	stH264Attr.bField = HI_FALSE;  /* surpport frame code only for hi3516, bfield = HI_FALSE */
	stH264Attr.bMainStream = HI_TRUE; /* surpport main stream only for hi3516, bMainStream = HI_TRUE */
	stH264Attr.u32Priority = 0; /*channels precedence level. invalidate for hi3516*/
	stH264Attr.bVIField = HI_FALSE;/*the sign of the VI picture is field or frame. Invalidate for hi3516*/
	memcpy(&stAttr.stVeAttr.stAttrH264e, &stH264Attr, sizeof(VENC_ATTR_H264_S));
	stAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
	stH264Cbr.u32StatTime		= 1; /* stream rate statics time(s) */
	stH264Cbr.u32ViFrmRate		= GetRate( vstd );/* input (vi) frame rate */
	stH264Cbr.fr32TargetFrmRate = GetRate( vstd );/* target frame rate */
	stH264Cbr.u32Gop			= stH264Cbr.fr32TargetFrmRate * 2;
	switch(CAPTURE_SIZE)
	{
		case CAPTURE_SIZE_1080P:
			stH264Cbr.u32BitRate = 4*1024; /* average bit rate */
			break;
		case CAPTURE_SIZE_720P:
			stH264Cbr.u32BitRate = 2*1024; /* average bit rate */
			break;
		case CAPTURE_SIZE_D1:
			stH264Cbr.u32BitRate = 1*1024; /* average bit rate */
			break;
		default:
			stH264Cbr.u32BitRate = 2*1024; /* average bit rate */
			break;
	}
	stH264Cbr.u32FluctuateLevel = 0; /* average bit rate */	
	memcpy(&stAttr.stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));

//	printf("%d:%s()VENC u32ViFrmRate=%d,u32MaxPicWidth=%d,u32PicHeight=%d,bitrate=%d\n",\
//		__LINE__,__FUNCTION__,stH264Cbr.u32ViFrmRate,stH264Attr.u32MaxPicWidth,stH264Attr.u32MaxPicHeight,stH264Cbr.u32BitRate);
	LIBDVR_PRINT("func:%s-chn:%d,main width=%d,heigth=%d\n", \
		__func__, chn,stH264Attr.u32PicWidth,stH264Attr.u32PicHeight);
	s32Ret = pthread_mutex_init(&(vchn->venc_chn[0].lock), NULL);
	HI_ASSERT( s32Ret == HI_SUCCESS);
    //创建编码通道组(主码流时创建)，并将VI绑定到该编码组
	s32Ret = VEncChn_Create_Inner(vchn, CHL_MAIN_T, &stAttr, HI_FALSE);
	HI_ASSERT( s32Ret == HI_SUCCESS);

	VpssGrp=GetVpssGrpByChn(chn);
	s32Ret = SAMPLE_COMM_VENC_BindVpss(VeGroup, VpssGrp, VPSS_BSTR_CHN);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("bindvpss venc group[%d], VpssGrp:%d, VpssChn:%d error[%#x]\n",
				VeGroup, VpssGrp, VPSS_BSTR_CHN, s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);
#if 1//yangsc屏蔽辅码流编码器。不需要
	//!初始化编码组辅码流
	VeGroup = GetVencGrpByChn(chn,CHL_2END_T);
	s32Ret = HI_MPI_VENC_CreateGroup(VeGroup);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("create group[%d] error[%#x]\n", VeGroup, s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);

	#if 0
	stAttr.stVeAttr.enType = PT_H264;	
	stH264Attr.u32MaxPicWidth = 704;//GetWidthBySize( CAPTURE_SIZE_D1, vstd );
	stH264Attr.u32MaxPicHeight =576;// GetHeightBySize( CAPTURE_SIZE_D1, vstd ); 
	stH264Attr.u32PicWidth = 704;//GetWidthBySize( CAPTURE_SIZE_D1, vstd );/*the picture width*/
	stH264Attr.u32PicHeight = 576;//GetWidthBySize( CAPTURE_SIZE_D1, vstd );/*the picture height*/
	stH264Attr.u32BufSize  = 704*576*2;//stH264Attr.u32PicWidth * stH264Attr.u32PicHeight * 2;/*stream buffer size*/
	stH264Attr.u32Profile  = 0;/*0: baseline; 1:MP; 2:HP   ? */
	stH264Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
	stH264Attr.bField = HI_FALSE;  /* surpport frame code only for hi3516, bfield = HI_FALSE */
	stH264Attr.bMainStream = HI_TRUE; /* surpport main stream only for hi3516, bMainStream = HI_TRUE */
	stH264Attr.u32Priority = 0; /*channels precedence level. invalidate for hi3516*/
	stH264Attr.bVIField = HI_FALSE;/*the sign of the VI picture is field or frame. Invalidate for hi3516*/
	memcpy(&stAttr.stVeAttr.stAttrH264e, &stH264Attr, sizeof(VENC_ATTR_H264_S));
	stAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
	stH264Cbr.u32StatTime		= 1; /* stream rate statics time(s) */
	stH264Cbr.u32ViFrmRate		= 25;//GetRate( vstd );/* input (vi) frame rate */
	stH264Cbr.fr32TargetFrmRate = 25;//GetRate( vstd );/* target frame rate */
	stH264Cbr.u32Gop			= stH264Cbr.fr32TargetFrmRate * 2;
	stH264Cbr.u32BitRate = 3*1024; /* average bit rate */
	stH264Cbr.u32FluctuateLevel = 0; /* average bit rate */
	memcpy(&stAttr.stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
	#else
	stH264Attr.u32MaxPicWidth = GetWidthBySize( CAPTURE_SIZE_CIF, vstd );
	stH264Attr.u32MaxPicHeight = GetHeightBySize( CAPTURE_SIZE_CIF, vstd ); 
	stH264Attr.u32PicWidth =GetWidthBySize(CAPTURE_SIZE_CIF, vstd);
	stH264Attr.u32PicHeight = GetHeightBySize(CAPTURE_SIZE_CIF, vstd);
	printf("u32PicWidth=%d,u32PicHeight=%d\n",stH264Attr.u32PicWidth,stH264Attr.u32PicHeight);
	stH264Attr.u32BufSize = stH264Attr.u32PicWidth*stH264Attr.u32PicHeight*1.5;
	memcpy(&stAttr.stVeAttr.stAttrH264e, &stH264Attr, sizeof(VENC_ATTR_H264_S));
	stH264Cbr.u32BitRate = 512; /* average bit rate */
	memcpy(&stAttr.stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
	#endif
	s32Ret = pthread_mutex_init(&(vchn->venc_chn[1].lock), NULL);
	HI_ASSERT( s32Ret == HI_SUCCESS);
	//创建编码通道组(主码流时创建)，并将VI绑定到该编码组
	s32Ret = VEncChn_Create_Inner(vchn, CHL_2END_T, &stAttr, HI_FALSE);
	HI_ASSERT( s32Ret == HI_SUCCESS);

	VpssGrp=GetVpssGrpByChn(chn);
	s32Ret = SAMPLE_COMM_VENC_BindVpss(VeGroup, VpssGrp, VPSS_LSTR_CHN);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("bindvpss venc group[%d], VpssGrp:%d, VpssChn:%d error[%#x]\n",
				VeGroup, VpssGrp, VPSS_BSTR_CHN, s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);
#endif
#ifdef NO_SNAP_IN_LIB
		return;
#endif
	//!jpeg 编码通道
	VENC_ATTR_JPEG_S stJpegAttr;
	VENC_GRP SnapGroup ;
END:
	
	SnapGroup= GetVencGrpByChn(chn,CHL_JPEG_T);
		/*创建抓拍通道*/
	s32Ret = HI_MPI_VENC_CreateGroup(SnapGroup);
	if (s32Ret != HI_SUCCESS)
	{
		printf("HI_MPI_VENC_CreateGroup err 0x%x,SnapGroup=%d\n",s32Ret,SnapGroup);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);
	
	memset(&stAttr, 0 ,sizeof(VENC_CHN_ATTR_S));
	stAttr.stVeAttr.enType = PT_JPEG;
	stJpegAttr.u32MaxPicWidth  = GetWidthBySize( CAPTURE_SIZE, vstd );
	stJpegAttr.u32MaxPicHeight = GetHeightBySize( CAPTURE_SIZE, vstd );
	stJpegAttr.u32PicWidth  = stJpegAttr.u32MaxPicWidth;
	stJpegAttr.u32PicHeight = stJpegAttr.u32MaxPicHeight;	
	//stJpegAttr.u32BufSize = stJpegAttr.u32PicWidth*stJpegAttr.u32PicHeight*1.5;
	stJpegAttr.u32Priority = 0;
	stJpegAttr.u32BufSize = (stJpegAttr.u32PicWidth*stJpegAttr.u32PicHeight+64-1)/64*64;
	stJpegAttr.bVIField = HI_FALSE;
	stJpegAttr.bByFrame = HI_TRUE;
	memcpy(&stAttr.stVeAttr.stAttrJpeg, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
	
//	printf("%d:%s()JPEGu32MaxPicWidth=%d,u32PicHeight=%d\n",\
//		__LINE__,__FUNCTION__,stJpegAttr.u32MaxPicWidth,stJpegAttr.u32PicHeight);
	s32Ret = pthread_mutex_init(&(vchn->venc_jpeg.lock), NULL);
	HI_ASSERT( s32Ret == HI_SUCCESS);
	
	s32Ret = VEncChn_Create(vchn, CHL_JPEG_T, &stAttr);
	HI_ASSERT( s32Ret == HI_SUCCESS);
	
	VpssJpegGrp = GetVpssJpegGrpByChn(chn);
	s32Ret = SAMPLE_COMM_VENC_BindVpss(SnapGroup, VpssJpegGrp , VPSS_BSTR_CHN);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("bindvpss venc group[%d], VpssGrp:%d, VpssChn:%d error[%#x]\n",
				VeGroup, VpssGrp, VPSS_BSTR_CHN, s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);

}
#endif
