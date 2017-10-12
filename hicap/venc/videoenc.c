#include "initial.h"
/*#include "pciv_msg.h"
#include "pciv_stream.h"*/
#include "VideoEnc.h"
#include "Capture.h"
//!private
#define NO_SNAP_IN_LIB 1
int GetSubTypeByType(VENC_CHN_T* vchn, enum capture_channel_t type)
{
#ifndef	_USE_720P_MODULE_
		return type;
#endif
	int SubType = 0;
	int i = 0,start = 0,stop = 0;
	if(CHL_MAIN_T == type && CHL_MAIN_T == vchn->venc_chn[CHL_MAIN_T].iSelected)
	{
		SubType = 0;
	}
	else if(CHL_2END_T == type && CHL_2END_T == vchn->venc_chn[CHL_3IRD_T].iSelected)
	{
		SubType = 2;
	}
	else if((CHL_MAIN_T == type && CHL_MAIN_T == vchn->venc_chn[CHL_2END_T].iSelected)
		||(CHL_2END_T == type && CHL_2END_T == vchn->venc_chn[CHL_2END_T].iSelected))	
	{
		SubType = 1;
	}
	else
	{
		SubType = CHL_FUNCTION_NUM;
	}

	return SubType;
}


static
int VEncChn_Create_Inner(VENC_CHN_T* vchn, enum capture_channel_t type, VENC_CHN_ATTR_S* attr, HI_BOOL bLock )
{
	HI_S32 s32Ret;
	int VeChn = vchn->Chn;
	VENC_GRP VeGroup;
	VI_DEV ViDev;
	VI_CHN ViChn;
	VENC_CHN venc_chn;

	pthread_mutex_t* pLock;


	ViDev = GetViDevByChn( VeChn );
	ViChn = GetViChnByChn( VeChn );
	venc_chn = GetVencChnByChn( VeChn, type );
	LIBDVR_PRINT( "VEncChn_Create-chn:%d, type:%d\n", venc_chn, type );

	if( type == CHL_JPEG_T )
	{
		VeGroup = GetVencGrpByChn( VeChn, type );
		pLock = &(vchn->venc_jpeg.lock);
	}
	else
	{
		VeGroup = GetVencGrpByChn( VeChn, type );
		pLock = &(vchn->venc_chn[type].lock);
	}

	if(bLock)
	{
		printf("%d:%s()lock\n",__LINE__,__FUNCTION__);
		pthread_mutex_lock(pLock);
	}
	
	s32Ret = HI_MPI_VENC_CreateChn(venc_chn, attr);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("HI_MPI_VENC_CreateChn:%#x-type:%d, venchn:%d\n", s32Ret, type,
				venc_chn);
		VEncChn_Print(vchn, attr);
		goto End;
	}
	LIBDVR_PRINT("HI_MPI_VENC_CreateChn:%#x-type:%d, venchn:%d\n", s32Ret, type,
			venc_chn);

	
	HI_S32 s32VencFd;
	s32Ret = HI_MPI_VENC_RegisterChn(VeGroup, venc_chn );
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("HI_MPI_VENC_RegisterChn:%#x, type:%d, venchn:%d, VeGroup:%d\n", s32Ret, type,
				venc_chn, VeGroup);
		goto End;
	}
	LIBDVR_PRINT("HI_MPI_VENC_RegisterChn:%#x, type:%d, venchn:%d, VeGroup:%d\n", s32Ret, type,
			venc_chn, VeGroup);

	s32VencFd = HI_MPI_VENC_GetFd( venc_chn );
	if (s32VencFd == HI_ERR_VENC_INVALID_CHNID
		|| s32VencFd == HI_ERR_VENC_SYS_NOTREADY)
	{
		s32VencFd = -1;
		ERR_PRINT("HI_MPI_VENC_GetFd:%#x\n", s32VencFd);
		s32Ret = -1;
		goto End;
	}
	LIBDVR_PRINT("HI_MPI_VENC_GetFd:%#x\n", s32VencFd);

	if( type == CHL_JPEG_T )
	{
		vchn->venc_jpeg.created = 1;
		vchn->venc_jpeg.venc_fd = s32VencFd;
	//	gettimeofday( &(vchn->venc_jpeg.SnapOldTime), NULL );
	}
	else
	{
		vchn->venc_chn[type].venc_fd = s32VencFd;
		vchn->venc_chn[type].created = 1;
		if(attr->stRcAttr.enRcMode==VENC_RC_MODE_H264CBR)
			vchn->venc_chn[type].frame_rate = attr->stRcAttr.stAttrH264Cbr.fr32TargetFrmRate;
		else
			vchn->venc_chn[type].frame_rate = attr->stRcAttr.stAttrH264Vbr.fr32TargetFrmRate;
		//���²����豸��Ա-----��  ��������ͨ��
		vchn->venc_chn[type].pic_width = attr->stVeAttr.stAttrH264e.u32PicWidth;
		vchn->venc_chn[type].pic_height = attr->stVeAttr.stAttrH264e.u32PicHeight;
		vchn->venc_chn[type].created = 1;
	}

End:
	if(bLock)
	{
		
		printf("%d:%s()unlock\n",__LINE__,__FUNCTION__);
		pthread_mutex_unlock(pLock);
	}
	return s32Ret;
}

int VEncChn_Destory_Inner(VENC_CHN_T* vchn, enum capture_channel_t type, HI_BOOL bLock )
{
	HI_S32 s32Ret;
	int chn = vchn->Chn;
	VENC_CHN venc_chn;
	venc_chn = GetVencChnByChn( chn, type );
	int started;
	int created;

	pthread_mutex_t* pLock;

	if( type == CHL_JPEG_T )
	{
		started = vchn->venc_jpeg.started;
		created = vchn->venc_jpeg.created;
		pLock = &(vchn->venc_jpeg.lock);
	}
	else
	{
		started = vchn->venc_chn[type].started;
		created = vchn->venc_chn[type].created;
		pLock = &(vchn->venc_chn[type].lock);
	}

	if( bLock )
	{
		pthread_mutex_lock(pLock);
	}
	LIBDVR_PRINT("VEncChn_Destory-chn:%d, type:%d, creat:%d, started:%d\n", chn, type,
			created, started);

	//!�ȹظ�����, �ٹ�������
	if( started > 0 )
	{
		s32Ret = HI_MPI_VENC_StopRecvPic( venc_chn );
		if( s32Ret != HI_SUCCESS )
		{
			ERR_PRINT(" HI_MPI_VENC_StopRecvPic:%#x-venchn:%d, type:%d \n",
					s32Ret, venc_chn, type);
			goto End;
		}
		LIBDVR_PRINT(" HI_MPI_VENC_StopRecvPic:%#x-venchn:%d, type:%d \n",
				s32Ret, venc_chn, type);
		
		if (type != CHL_JPEG_T)
		{			
			started = 0;
		}
	}

	if( created > 0 )
	{		
		s32Ret = HI_MPI_VENC_UnRegisterChn( venc_chn );
		if( s32Ret != HI_SUCCESS )
		{
			ERR_PRINT(" HI_MPI_VENC_UnRegisterChn:%#x-venchn:%d, type:%d \n",
					s32Ret, venc_chn, type);
			goto End;
		}
		LIBDVR_PRINT(" HI_MPI_VENC_UnRegisterChn:%#x-venchn:%d, type:%d \n",
							s32Ret, venc_chn, type);
		
		s32Ret = HI_MPI_VENC_DestroyChn( venc_chn );
		if( s32Ret != HI_SUCCESS )
		{
			ERR_PRINT(" HI_MPI_VENC_DestroyChn:%#x-venchn:%d, type:%d \n",
					s32Ret, venc_chn, type );
			goto End;
		}
		LIBDVR_PRINT(" HI_MPI_VENC_DestroyChn:%#x-venchn:%d, type:%d \n",
							s32Ret, venc_chn, type );

		created = 0;
	}

	if( type == CHL_JPEG_T )
	{
		vchn->venc_jpeg.started = started;
		vchn->venc_jpeg.created = created;
		vchn->venc_jpeg.venc_fd = -1;
	}
	else
	{
		vchn->venc_chn[type].started = started;
		vchn->venc_chn[type].created = created;
		vchn->venc_chn[type].venc_fd  = -1;
	}

End:
	if( bLock )
	{
		pthread_mutex_unlock(pLock);
	}
	return s32Ret;
}

static
int	VEncChn_SetSnapAttr(VENC_CHN_T* vchn, VENC_CHN_ATTR_S* attr)
{
	return 0;//yangsc
	if( attr->stVeAttr.enType!= PT_JPEG )
	{
		return -1;
	}
	HI_S32 s32Ret;
	VENC_CHN venchn = GetVencChnByChn(vchn->Chn, CHL_JPEG_T);
	VENC_CHN_ATTR_S OldAttr;
	VENC_ATTR_JPEG_S* OldstJpegAttr = NULL;

	memset(&OldAttr,0,sizeof(OldAttr));
	OldAttr.stVeAttr.enType = PT_JPEG;
	OldstJpegAttr = (VENC_ATTR_JPEG_S*)(&(OldAttr.stVeAttr.stAttrJpeg));

	s32Ret = HI_MPI_VENC_GetChnAttr( venchn,  &OldAttr);
	if( s32Ret != HI_SUCCESS )
	{
		ERR_PRINT("HI_MPI_VENC_GetChnAttr-code:%#x, chn:%d, venchn:%d\n",
				s32Ret, vchn->Chn, venchn);
		return -1;
	}

	VENC_ATTR_JPEG_S* pNewstJpegAttr = (VENC_ATTR_JPEG_S*)(&(attr->stVeAttr.stAttrJpeg));

	if( OldstJpegAttr->u32MaxPicWidth != pNewstJpegAttr->u32MaxPicWidth ||
			OldstJpegAttr->u32BufSize != pNewstJpegAttr->u32BufSize ||
			OldstJpegAttr->bByFrame != pNewstJpegAttr->bByFrame ||
			OldstJpegAttr->bVIField != pNewstJpegAttr->bVIField  )
	{
		//!��̬���Է����仯����Ҫ����
		s32Ret = VEncChn_Destory_Inner(vchn, CHL_JPEG_T, HI_FALSE);
		if( s32Ret != HI_SUCCESS )
		{
			ERR_PRINT("ERROR!!!");
			return s32Ret;
		}

		s32Ret = VEncChn_Create_Inner(vchn, CHL_JPEG_T, attr, HI_FALSE);
		if( s32Ret != HI_SUCCESS )
		{
			ERR_PRINT("ERROR!!!");
			return s32Ret;
		}
	}
	else if(OldstJpegAttr->u32PicWidth != pNewstJpegAttr->u32PicWidth ||
			OldstJpegAttr->u32PicHeight != pNewstJpegAttr->u32PicHeight )
	{
		//!��̬���ԣ�ֱ�����þͿ�
		s32Ret = HI_MPI_VENC_SetChnAttr( venchn,  attr );
		HI_ASSERT( s32Ret == HI_SUCCESS);
	}
	return 0;
}

void VEncChn_Print(VENC_CHN_T* vchn, VENC_CHN_ATTR_S* pttr)
{
#if 0
	if( pttr->stVeAttr.enType== PT_H264)
	{

		VENC_ATTR_H264_S* pH264 = (VENC_ATTR_H264_S*)(&(pttr->stVeAttr.stAttrH264e));
		VENC_ATTR_H264_CBR_S	stAttrH264Cbr;

		LIBDVR_PRINT("\n \
				u32Priority:%d\n \
				u32PicWidth:%d\n \
				u32PicHeight:%d\n \
				u32ViFramerate:%d\n \
				bMainStream:%d\n ",
				pH264->u32Priority,
				pH264->u32PicWidth,
				pH264->u32PicHeight,
				pH264->u32ViFramerate,
				pH264->bMainStream);
		printf(" \
				bVIField:%d\n \
				u32BufSize:%d\n \
				bByFrame:%d\n \
				bField:%d\n \
				u32TargetFramerate:%d\n \
				u32Gop:%d\n \
				u32MaxDelay:%d\n",
				pH264->bVIField,
				pH264->u32BufSize,
				pH264->bByFrame,
				pH264->bField,
				pH264->u32TargetFramerate,
				pH264->u32Gop;//u32Gop,
				pH264->u32MaxDelay);
		printf(" \
				enRcMode:%d\n \
				u32Bitrate:%d\n \
				u32PicLevel:%d\n \
				s32QpI:%d\n \
				s32QpP:%d\n \
				s32Minutes:%d\n",
				pH264->enRcMode,
				pH264->u32Bitrate,
				pH264->u32PicLevel,
				pH264->s32QpI,
				pH264->s32QpP,
				pH264->s32Minutes
				);
	}
	else
	{
		typedef struct hiVENC_ATTR_JPEG_S
		{
		    HI_U32  u32Priority;	   /*channels precedence level*/
		    HI_U32  u32BufSize;        /*stream buffer size*/
		    HI_U32  u32PicWidth;       /*the picture width*/
		    HI_U32  u32PicHeight;      /*the picture height*/
			HI_BOOL bVIField;		   /*the sign of the VI picture is field or frame?*/
		    HI_BOOL bByFrame;          /*get stream mode is field mode  or frame mode?*/
		    HI_U32  u32MCUPerECS;      /*MCU number of one ECS*/
		    HI_U32  u32ImageQuality;   /*image quality*/
		}VENC_ATTR_JPEG_S;

		VENC_ATTR_JPEG_S* pJpeg = (VENC_ATTR_JPEG_S* )pttr->pValue;
		//LIBDVR_PRINT("\n");
		fprintf(stderr, "u32Priority:%d\n \
				u32BufSize:%d\n \
				u32PicWidth:%d\n \
				u32PicHeight:%d\n \
				bVIField:%d\n \
				bByFrame:%d\n \
				u32MCUPerECS:%d\n \
				u32ImageQuality:%d\n",
				pJpeg->u32Priority, pJpeg->u32BufSize, pJpeg->u32PicWidth,
				pJpeg->u32PicHeight, pJpeg->bVIField, pJpeg->bByFrame,
				pJpeg->u32MCUPerECS, pJpeg->u32ImageQuality);
		fflush(stderr);usleep(0);
	}
#endif
}

  /*init.c defined too*/
		
void VEncChn_Init_General(VENC_CHN_T* vchn, int chn)
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
		//!TODO: �����ʼֵ��������޸ĳɾ����
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

//	InitOsdSize(chn); temporary
	
	//!ȷ����vi�Ѿ���
	s32Ret = VIM_Start( VIM_GetIns(), chn );
	HI_ASSERT( s32Ret == HI_SUCCESS);

	//!��ʼ��������
	VeGroup = GetVencGrpByChn(chn,CHL_MAIN_T);
	s32Ret = HI_MPI_VENC_CreateGroup(VeGroup);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("create group[%d] error[%#x]\n", VeGroup, s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);

	//!����������
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
    //��������ͨ����(������ʱ����)������VI�󶨵��ñ�����
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
#if 1//yangsc���θ�����������������Ҫ
	//!��ʼ�������鸨����
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
	//��������ͨ����(������ʱ����)������VI�󶨵��ñ�����
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
	//!jpeg ����ͨ��
	VENC_ATTR_JPEG_S stJpegAttr;
	VENC_GRP SnapGroup ;
END:
	
	SnapGroup= GetVencGrpByChn(chn,CHL_JPEG_T);
		/*����ץ��ͨ��*/
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


void VEncChn_Init_720P(VENC_CHN_T* vchn, int chn)
#if 1
{
	return 0;
}
#else
{
	LIBDVR_PRINT("func:%s-chn:%d\n", __func__, chn);
	memset( vchn, 0, sizeof(VENC_CHN_T) );
	HI_S32 s32Ret;
	int j;
	VI_DEV ViDev;
	VI_CHN ViChn;
	VENC_GRP VeGroup ;

	vchn->Chn = chn;

	ViDev = GetViDevByChn(chn);
	ViChn = GetViChnByChn(chn);
	VeGroup = GetVencGrpByChn(chn);

	VIDEO_NORM_E vstd = VIM_GetVsvd( VIM_GetIns() );

	VENC_CHN_INFO_t* pvenc;
	for(j = 0; j < 3; j++)
	{
		//!TODO: �����ʼֵ��������޸ĳɾ����
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
		pvenc->iSelected = CHL_FUNCTION_NUM;
		pvenc->gop = 50;
		pvenc->cur_frame_rate = 20;
	}

	vchn->venc_jpeg.created = 0;
	vchn->venc_jpeg.started = 0;
	vchn->venc_jpeg.nFps = 0;

	InitOsdSize(chn);
	
	//vchn->venc_jpeg.Period = 1000;

	//!ȷ��vi�Ѵ� TODO: �˴�Ҳ������������ʼ��˳��Ӧ���񾭴�
	//s32Ret = VIM_Init( VIM_GetIns() );
	//HI_ASSERT( s32Ret == HI_SUCCESS);
//VIӦ���ڳ�ʼ����ʱ�����úã��������޸Ĳ��ʺ�

	//!ȷ����vi�Ѿ���
	s32Ret = VIM_Start( VIM_GetIns(), chn );
	HI_ASSERT( s32Ret == HI_SUCCESS);

	//!��ʼ��������
	s32Ret = HI_MPI_VENC_CreateGroup(VeGroup);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("create group[%d] error[%#x]\n", VeGroup, s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);
	s32Ret = HI_MPI_VENC_BindInput(VeGroup, ViDev, ViChn);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("bindinput group[%d], videv:%d, vichn:%d error[%#x]\n",
				VeGroup, ViDev, ViChn, s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);

	//!����������
	VENC_CHN_ATTR_S stAttr;
	VENC_ATTR_H264_S stH264Attr;

	stAttr.enType = PT_H264;
	stAttr.pValue = (HI_VOID *)&stH264Attr;

	stH264Attr.bByFrame = HI_TRUE;  //��֡��ȡ
	stH264Attr.enRcMode = RC_MODE_CBR; //����ͨ����CBR���ʿ���
	stH264Attr.bField = HI_FALSE;//֡����
	stH264Attr.bVIField = HI_FALSE;//VI����Ϊ֡
	stH264Attr.u32Bitrate = 1000;//1000;

	VI_DEV chn_dev = GetViDevByChn(chn);
	VI_CHN chn_vi = GetViChnByChn(chn);
	VI_MANAGER_T* pVIM=VIM_GetIns();
	enum capture_size_t CAPTURE_SIZE=pVIM->m_vi_chn_size[chn_dev][chn_vi];
	stH264Attr.u32PicWidth = GetWidthBySize( CAPTURE_SIZE, vstd );
	stH264Attr.u32PicHeight = GetHeightBySize( CAPTURE_SIZE, vstd );
	//1U��D1�ɼ���ȴ��CIF���룬�Ǹ�����
	PRODUCT_TYPE product=get_the_product_type();

	stH264Attr.u32ViFramerate = GetRate( vstd );
	stH264Attr.u32TargetFramerate = GetRate( vstd );
	//����YUV420�ĸ�ʽ�������Ӧ���ǳ���1.5
	 if(product==ENUM_PRODUCT_TYPE_3515_VB2761)
	{
		stH264Attr.u32BufSize = 1643680;
	}
	else
	{
		stH264Attr.u32BufSize = stH264Attr.u32PicWidth*stH264Attr.u32PicHeight*1.5;
	}
	stH264Attr.u32Gop = stH264Attr.u32TargetFramerate * 2;
	stH264Attr.u32MaxDelay = 100;
	stH264Attr.u32PicLevel = 3;
	stH264Attr.u32Priority=0;
	stH264Attr.bMainStream=HI_TRUE;
	stH264Attr.s32Minutes=0;//�˲���ABRģʽ����Ч
	stH264Attr.s32QpI=0;//����������������FIXQPģʽ����Ч
	stH264Attr.s32QpP=0;
	s32Ret = pthread_mutex_init(&(vchn->venc_chn[0].lock), NULL);
	HI_ASSERT( s32Ret == HI_SUCCESS);
    //��������ͨ����(������ʱ����)������VI�󶨵��ñ�����
	s32Ret = VEncChn_Create_Inner(vchn, CHL_MAIN_T, &stAttr, HI_FALSE);
	HI_ASSERT( s32Ret == HI_SUCCESS);
	//!���ڴ��ڸ�������ͨ������Ӧ�ô���������ͨ��
	//!��˼������
	
	//!��ʼ������������

		stH264Attr.bMainStream = HI_TRUE;
		stH264Attr.u32Bitrate = 1024;
		stH264Attr.u32PicWidth =GetWidthBySize(CAPTURE_SIZE_D1, vstd);
		stH264Attr.u32PicHeight = GetHeightBySize(CAPTURE_SIZE_D1, vstd);		

	//stH264Attr.u32BufSize=stH264Attr.u32PicWidth*stH264Attr.u32PicHeight*4;
	//����YUV420�ĸ�ʽ�������Ӧ���ǳ���1.5
	stH264Attr.u32BufSize = stH264Attr.u32PicWidth*stH264Attr.u32PicHeight*1.5;
	s32Ret = VEncChn_Create_Inner(vchn, CHL_2END_T, &stAttr, HI_FALSE);
	HI_ASSERT( s32Ret == HI_SUCCESS);
	s32Ret = pthread_mutex_init(&(vchn->venc_chn[1].lock), NULL);
	HI_ASSERT( s32Ret == HI_SUCCESS);

	 if(product==ENUM_PRODUCT_TYPE_3515_VC2761
		||product == ENUM_PRODUCT_TYPE_3515_VB2761)
	{
		stH264Attr.u32Bitrate = 600;
		stH264Attr.bMainStream = HI_FALSE;
		stH264Attr.u32PicWidth =GetWidthBySize(CAPTURE_SIZE_QCIF, vstd);
		stH264Attr.u32PicHeight = GetHeightBySize(CAPTURE_SIZE_QCIF, vstd);		
	 
		stH264Attr.u32BufSize = stH264Attr.u32PicWidth*stH264Attr.u32PicHeight*1.5;

		s32Ret = VEncChn_Create_Inner(vchn, CHL_3IRD_T, &stAttr, HI_FALSE);
		HI_ASSERT( s32Ret == HI_SUCCESS);
		s32Ret = pthread_mutex_init(&(vchn->venc_chn[CHL_3IRD_T].lock), NULL);
		HI_ASSERT( s32Ret == HI_SUCCESS);
	}
	
	//!jpeg ����ͨ��
	VENC_ATTR_JPEG_S stJpegAttr;	
	VENC_GRP SnapGroup ;
END:
	
	SnapGroup= GetVencGrpByChn_Jpg(chn);
	stJpegAttr.u32Priority = 0;
	

	 if(product==ENUM_PRODUCT_TYPE_3515_VB2761)
	{
		stJpegAttr.u32PicWidth  = vchn->venc_chn[1].pic_width;
		stJpegAttr.u32PicHeight = vchn->venc_chn[1].pic_height;	
		stJpegAttr.u32BufSize = stJpegAttr.u32PicWidth*stJpegAttr.u32PicHeight;

	}
	else
	{
		stJpegAttr.u32PicWidth  = vchn->venc_chn[0].pic_width;
		stJpegAttr.u32PicHeight = vchn->venc_chn[0].pic_height;		
		stJpegAttr.u32BufSize = stJpegAttr.u32PicWidth*stJpegAttr.u32PicHeight*1.5;
	}	
	
	stJpegAttr.bVIField = HI_FALSE;
	stJpegAttr.bByFrame = HI_TRUE;
	stJpegAttr.u32MCUPerECS = stJpegAttr.u32PicWidth*stJpegAttr.u32PicHeight/256;
	stJpegAttr.u32ImageQuality = 1;
	memset(&stAttr, 0 ,sizeof(VENC_CHN_ATTR_S));
	stAttr.enType = PT_JPEG;
	stAttr.pValue = (HI_VOID *)&stJpegAttr;

	/*����ץ��ͨ��*/
	s32Ret = HI_MPI_VENC_CreateGroup(SnapGroup);
	if (s32Ret != HI_SUCCESS)
	{
		printf("HI_MPI_VENC_CreateGroup err 0x%x\n",s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);

	s32Ret = HI_MPI_VENC_BindInput(SnapGroup, ViDev, ViChn);
	if (s32Ret != HI_SUCCESS)
	{
		printf("HI_MPI_VENC_BindInput err 0x%x\n",s32Ret);
	}
	HI_ASSERT( s32Ret == HI_SUCCESS);

	s32Ret = VEncChn_Create(vchn, CHL_JPEG_T, &stAttr);
	HI_ASSERT( s32Ret == HI_SUCCESS);
	s32Ret = pthread_mutex_init(&(vchn->venc_jpeg.lock), NULL);
	HI_ASSERT( s32Ret == HI_SUCCESS);
}
#endif
void VEncChn_Init(VENC_CHN_T* vchn, int chn)
{
#ifdef _USE_720P_MODULE_	
	VEncChn_Init_720P(vchn,chn);
#else
	VEncChn_Init_General(vchn,chn);
#endif
}
void VEncChn_Exit(VENC_CHN_T* vchn)
{
	LIBDVR_PRINT("func:%s-chn:%d\n", __func__, vchn->Chn);
	HI_S32 s32Ret;
	VENC_GRP VeGroup;
	VPSS_GRP VpssGrp;
	
//	VEncChn_Destory_Inner(vchn, CHL_JPEG_T, HI_FALSE);
	VEncChn_Destory_Inner(vchn, CHL_2END_T, HI_FALSE);
	VEncChn_Destory_Inner(vchn, CHL_MAIN_T, HI_FALSE);

	pthread_mutex_destroy(&(vchn->venc_chn[0].lock));
	pthread_mutex_destroy(&(vchn->venc_chn[1].lock));
//	pthread_mutex_destroy(&(vchn->venc_jpeg.lock));
#if 0
	//!����jpeg������
	VeGroup = GetVencGrpByChn(vchn->Chn,CHL_JPEG_T);
	VpssGrp=GetVpssJpegGrpByChn(vchn->Chn);
	s32Ret = SAMPLE_COMM_VENC_UnBindVpss(VeGroup, VpssGrp, VPSS_BSTR_CHN);
	HI_ASSERT(s32Ret == HI_SUCCESS );
	s32Ret = HI_MPI_VENC_DestroyGroup(VeGroup);
	HI_ASSERT(s32Ret == HI_SUCCESS );
#endif
	//!���붨�������ٱ�����
	VeGroup = GetVencGrpByChn(vchn->Chn,CHL_MAIN_T);
	VpssGrp=GetVpssGrpByChn(vchn->Chn);
	s32Ret = SAMPLE_COMM_VENC_UnBindVpss(VeGroup, VpssGrp, VPSS_BSTR_CHN);
	HI_ASSERT(s32Ret == HI_SUCCESS );
	s32Ret = HI_MPI_VENC_DestroyGroup(VeGroup);
	HI_ASSERT(s32Ret == HI_SUCCESS );

	VeGroup = GetVencGrpByChn(vchn->Chn,CHL_2END_T);
	VpssGrp=GetVpssGrpByChn(vchn->Chn);
	s32Ret = SAMPLE_COMM_VENC_UnBindVpss(VeGroup, VpssGrp, VPSS_LSTR_CHN);
	HI_ASSERT(s32Ret == HI_SUCCESS );
	s32Ret = HI_MPI_VENC_DestroyGroup(VeGroup);
	HI_ASSERT(s32Ret == HI_SUCCESS );
}

int VEncChn_Create(VENC_CHN_T* vchn, enum capture_channel_t type, VENC_CHN_ATTR_S* attr )
{
	return VEncChn_Create_Inner(vchn, type, attr, HI_TRUE);
}
int VEncChn_Destory(VENC_CHN_T* vchn, enum capture_channel_t type)
{
	return VEncChn_Destory_Inner(vchn, type, HI_TRUE);
}

int VEncChn_GetAttr(VENC_CHN_T* vchn, enum capture_channel_t type, VENC_CHN_ATTR_S* attr)
{
	HI_S32 s32Ret;
	VENC_CHN venchn = GetVencChnByChn(vchn->Chn, type);
	pthread_mutex_t* pLock;

	if( type == CHL_JPEG_T )
	{
		pLock = &(vchn->venc_jpeg.lock);
	}
	else
	{
		pLock = &(vchn->venc_chn[type].lock);
	}

	pthread_mutex_lock(pLock);

	s32Ret = HI_MPI_VENC_GetChnAttr( venchn,  attr);

	pthread_mutex_unlock(pLock);

	return s32Ret;
}


int VEncChn_SetAttr_General(VENC_CHN_T* vchn, enum capture_channel_t type, VENC_CHN_ATTR_S* attr)	
{
	HI_S32 s32Ret =HI_SUCCESS;
	

	if( type == CHL_JPEG_T )
	{
		pthread_mutex_lock(&vchn->venc_jpeg.lock);
		s32Ret = VEncChn_SetSnapAttr(vchn, attr);
		pthread_mutex_unlock(&vchn->venc_jpeg.lock);
		return s32Ret;
	}

	if( type != CHL_MAIN_T && type != CHL_2END_T )
	{
		return -1;//!Ŀǰ��֧��������������
	}

	pthread_mutex_lock(&vchn->venc_chn[type].lock);

	enum capture_size_t size ;
	HI_BOOL bNeedResetMain = HI_FALSE;	//!��Ҫ������������־
	HI_BOOL bNeedStartMain = HI_FALSE;
	VENC_CHN_ATTR_S attr_main;
	attr_main.stVeAttr.enType = PT_H264;
#if 0//yangsc
	//!ȡ��������
	s32Ret = HI_MPI_VENC_GetChnAttr( vchn->venc_chn[type].vencChn,  &attr_main);
	if (s32Ret != HI_SUCCESS)
	{
		ERR_PRINT(" HI_MPI_VENC_GetChnAttr:%#x, venchn:%d, type:%d !!! \n",
				s32Ret, vchn->venc_chn[type].vencChn, type);
		s32Ret = HI_FAILURE;
		goto End;
	}
	
	if( vchn->venc_chn[type].started > 0 && bNeedResetMain )
	{
		bNeedStartMain = HI_TRUE;
	}

	if(bNeedResetMain)
	{
		//!����������
		s32Ret = VEncChn_Destory_Inner(vchn, type, HI_FALSE);
		HI_ASSERT( s32Ret == HI_SUCCESS);
		s32Ret = VEncChn_Create_Inner(vchn, type, &attr, HI_FALSE);
		HI_ASSERT( s32Ret == HI_SUCCESS);
		if( bNeedStartMain > 0 )
		{
			s32Ret = HI_MPI_VENC_StartRecvPic( vchn->venc_chn[type].vencChn );
			if (s32Ret != HI_SUCCESS)
			{
				ERR_PRINT("HI_MPI_VENC_StartRecvPic failed:0x%x!\r\n",s32Ret);
				s32Ret = HI_FAILURE;
				goto End;
			}
		}
	}
	else if( !bNeedResetMain )
	{
		//!��̬���ԣ�ֱ�����þͿ�
		s32Ret = HI_MPI_VENC_SetChnAttr( vchn->venc_chn[type].vencChn,  attr );
		if (s32Ret != HI_SUCCESS)
		{
			ERR_PRINT("HI_MPI_VENC_SetChnAttr failed:0x%x!\r\n",s32Ret);
		}
		HI_ASSERT( s32Ret == HI_SUCCESS);
	}
#endif
	if( s32Ret == HI_SUCCESS )
	{
		if(attr->stRcAttr.enRcMode==VENC_RC_MODE_H264CBR)
			vchn->venc_chn[type].frame_rate = attr->stRcAttr.stAttrH264Cbr.fr32TargetFrmRate;
		else
			vchn->venc_chn[type].frame_rate = attr->stRcAttr.stAttrH264Vbr.fr32TargetFrmRate;
		vchn->venc_chn[type].pic_width = attr->stVeAttr.stAttrH264e.u32PicWidth;
		vchn->venc_chn[type].pic_height = attr->stVeAttr.stAttrH264e.u32PicHeight;
		vchn->venc_chn[type].image_size = GetSizeByWH(attr->stVeAttr.stAttrH264e.u32PicWidth, attr->stVeAttr.stAttrH264e.u32PicHeight);
//		printf("%d:%s()frame_rate=%d,pic_width=%d,pic_height=%d,image_size=%d\n",__LINE__,__FUNCTION__,\
//			vchn->venc_chn[type].frame_rate,vchn->venc_chn[type].pic_width,\
//			vchn->venc_chn[type].pic_height,vchn->venc_chn[type].image_size);
	}
End:
	pthread_mutex_unlock(&vchn->venc_chn[type].lock);
	return s32Ret;
}	
int VEncChn_SetAttr_720P(VENC_CHN_T* vchn, enum capture_channel_t type, VENC_CHN_ATTR_S* attr)
#if 1
	{
		return 0;
	}
#else

{
	HI_S32 s32Ret;
	
	if( type == CHL_JPEG_T )
	{
		pthread_mutex_lock(&vchn->venc_jpeg.lock);
		s32Ret = VEncChn_SetSnapAttr(vchn, attr);
		pthread_mutex_unlock(&vchn->venc_jpeg.lock);
		return s32Ret;
	}

	if( type != CHL_MAIN_T && type != CHL_2END_T && type != CHL_3IRD_T)
	{
		return -1;//!Ŀǰ��֧��������������
	}

//	pthread_mutex_lock(&vchn->venc_chn[type].lock);

	enum capture_size_t size ;
	HI_BOOL bNeedResetMain = HI_FALSE;	//!��Ҫ������������־
	HI_BOOL bNeedStartMain = HI_FALSE;
	HI_BOOL bNeedResetSecond = HI_FALSE;	//!��Ҫ����������־
	HI_BOOL bNeedStartSecond = HI_FALSE;
	VENC_CHN_ATTR_S attr_main;
	VENC_ATTR_H264_S attrh264_main;
	VENC_CHN_ATTR_S attr_second;
	VENC_ATTR_H264_S attrh264_second;
	PRODUCT_TYPE product=get_the_product_type();
	attr_main.enType = PT_H264;
	attr_main.pValue = &attrh264_main;

	VENC_ATTR_H264_S* pSetAttr = (VENC_ATTR_H264_S*)attr->pValue;
	

	//!ȡ��������
	int subtype =0;
	if(( pSetAttr->u32PicWidth == GetWidthBySize(CAPTURE_SIZE_1080P, VIM_GetVsvd( VIM_GetIns()))
		||pSetAttr->u32PicWidth == GetWidthBySize(CAPTURE_SIZE_720P, VIM_GetVsvd( VIM_GetIns())))
		&& type == CHL_MAIN_T )
	{
		pSetAttr->bMainStream = HI_TRUE;
		subtype = 0;
	}
	else if( pSetAttr->u32PicWidth == GetWidthBySize(CAPTURE_SIZE_QCIF, VIM_GetVsvd( VIM_GetIns()))
		&& type == CHL_2END_T)
	{
		pSetAttr->bMainStream = HI_FALSE;
		subtype = 2;
	}
	else
	{
		pSetAttr->bMainStream = HI_TRUE;
		subtype =1;
	}

	int pretype= GetSubTypeByType(vchn,type);
	int pretypeExt= GetSubTypeByType(vchn,CHL_2END_T); 

	if(type == CHL_MAIN_T && 1 == subtype && 1 == pretypeExt)
	{
		//��������D1 ���CIF
		int started = vchn->venc_chn[pretypeExt].started;
		VEncChn_Stop(vchn, pretypeExt);
		vchn->venc_chn[pretypeExt].iSelected= CHL_FUNCTION_NUM;
		vchn->venc_chn[CHL_3IRD_T].iSelected= CHL_2END_T;
		if(TRUE == started)
		{
			VEncChn_Start(vchn, CHL_3IRD_T);		
		}
	}

	if(subtype != pretype)
	{
		int started =FALSE;
		if(CHL_FUNCTION_NUM != pretype)
		{
			started = vchn->venc_chn[pretype].started;
			VEncChn_Stop(vchn, pretype);
			vchn->venc_chn[pretype].iSelected= CHL_FUNCTION_NUM;
		}
		vchn->venc_chn[subtype].iSelected= type;
		if(TRUE == started)
		{
			VEncChn_Start(vchn, subtype);
		}
	}
	else
	{
		vchn->venc_chn[subtype].iSelected= type;
	}
	
	if( !bNeedResetMain && !bNeedResetSecond )
	{
		//!��̬���ԣ�ֱ�����þͿ�

	//	VEncChn_Print(vchn, attr);
		s32Ret = HI_MPI_VENC_SetChnAttr( vchn->venc_chn[subtype].vencChn,  attr );
		if (s32Ret != HI_SUCCESS)
		{
			ERR_PRINT("HI_MPI_VENC_SetChnAttr failed:0x%x!\r\n",s32Ret);
		}
		HI_ASSERT( s32Ret == HI_SUCCESS);
	}

	if( s32Ret == HI_SUCCESS )
	{
		vchn->venc_chn[subtype].frame_rate = pSetAttr->u32TargetFramerate;
		vchn->venc_chn[subtype].pic_width = pSetAttr->u32PicWidth;
		if(1088 == pSetAttr->u32PicHeight)
		{
			vchn->venc_chn[subtype].pic_height = 1080;
		}
		else
		{
			vchn->venc_chn[subtype].pic_height = pSetAttr->u32PicHeight;
		}
		vchn->venc_chn[subtype].image_size = GetSizeByWH(pSetAttr->u32PicWidth, pSetAttr->u32PicHeight);
	}

End:
	
	return s32Ret;
}
#endif
int VEncChn_SetAttr(VENC_CHN_T* vchn, enum capture_channel_t type, VENC_CHN_ATTR_S* attr)
{
#ifdef _USE_720P_MODULE_	
	VEncChn_SetAttr_720P(vchn,type,attr);
#else
	VEncChn_SetAttr_General(vchn,type,attr);
#endif
}

int VEncChn_Start(VENC_CHN_T* vchn, enum capture_channel_t type)
{
	int started;
	VENC_CHN vencChn = GetVencChnByChn(vchn->Chn, type);
	VENC_GRP vencGrp = GetVencGrpByChn(vchn->Chn, type);
	pthread_mutex_t* pLock;

	if( type == CHL_JPEG_T )
	{
		started = vchn->venc_jpeg.started;
		pLock = &(vchn->venc_jpeg.lock);
	}
	else
	{
		started = vchn->venc_chn[type].started;
		pLock = &(vchn->venc_chn[type].lock);
	}

	pthread_mutex_lock(pLock);

//	LIBDVR_PRINT("VEncChn_Start-chn:%d, type:%d, started:%d\n", vchn->Chn, type, started);

	HI_S32 s32Ret = HI_SUCCESS;
	
	LIBDVR_PRINT("func:%s-venc:%d, type:%d,started=%d\n", __FUNCTION__, vencChn, type,started );
	//!ֹͣ�������ȡ���
	if( started == 0 )
	{
		LIBDVR_PRINT("func:%s-venc:%d, type:%d\n", __FUNCTION__, vencChn, type );
		if(type == CHL_JPEG_T)
		{
			//!ͼƬ���治Ҫ����2֡
			s32Ret = HI_MPI_VENC_SetMaxStreamCnt(vencChn, 1);
			if( s32Ret != HI_SUCCESS )
			{
				ERR_PRINT("set-HI_MPI_VENC_SetMaxStreamCnt-error:%#x, vencChn:%d!\n",
						s32Ret, vencChn);
			}

			//!��ע��jpegͨ���飬�ﵽ����ץͼ�����Ч��
    		s32Ret = HI_MPI_VENC_UnRegisterChn( vencChn );
    		if( s32Ret != HI_SUCCESS )
    		{
    			ERR_PRINT("VEncChn_Start-- HI_MPI_VENC_UnRegisterChn:%#x-venchn:%d, type:%d \n",
    					s32Ret, vencChn, type);
    		}
			vencGrp = GetVencGrpByChn(vchn->Chn, type);
           	s32Ret = HI_MPI_VENC_RegisterChn(vencGrp, vencChn );
        	if (s32Ret != HI_SUCCESS)
        	{
        		ERR_PRINT("VEncChn_Start--HI_MPI_VENC_RegisterChn:%#x, type:%d, venchn:%d, VeGroup:%d\n", s32Ret, type,
        				vencChn, vchn->Chn);
        	} 		
		}
		
		if (type != CHL_JPEG_T)
		{
			
			LIBDVR_PRINT("HI_MPI_VENC_StartRecvPic() vencchn:%d, type:%dn", vencChn, type );
			s32Ret = HI_MPI_VENC_StartRecvPic( vencChn );
			if( s32Ret != HI_SUCCESS )
			{
				ERR_PRINT("func:%s-s32Ret:%#x \n", __func__, s32Ret);
				goto End;
			}
		}
		started = 1;
	}

	if( type == CHL_JPEG_T )
	{
		vchn->venc_jpeg.nFlage = 1;
		vchn->venc_jpeg.started = started;
		vchn->venc_jpeg.last_frame_time = 0;
	}
	else
	{
		vchn->venc_chn[type].started = started;
	}

End:
	pthread_mutex_unlock(pLock);
	return s32Ret;

}
int VEncChn_Stop(VENC_CHN_T* vchn, enum capture_channel_t type)
{


	if( type >= CHL_FUNCTION_NUM)
	{
		return -1;//!Ŀǰ��֧��������������
	}
	
	HI_S32 s32Ret = HI_SUCCESS;
	int started;

	pthread_mutex_t* pLock;
	VENC_CHN vencChn = GetVencChnByChn(vchn->Chn, type);

	if( type == CHL_JPEG_T )
	{
		started = vchn->venc_jpeg.started;
		pLock = &(vchn->venc_jpeg.lock);
	}
	else
	{
		started = vchn->venc_chn[type].started;
		pLock = &(vchn->venc_chn[type].lock);
	}

	pthread_mutex_lock(pLock);

//	LIBDVR_PRINT("VEncChn_Start-chn:%d, type:%d, started:%d\n", vchn->Chn, type, started);

	//!ֹͣ�������ȡ���
	if( started > 0 )
	{
		
		if (type != CHL_JPEG_T)
		{
			LIBDVR_PRINT("func:%s-venc:%d, type:%d\n", __FUNCTION__, vencChn, type );
			s32Ret = HI_MPI_VENC_StopRecvPic( vencChn );
			if( s32Ret != HI_SUCCESS )
			{
				ERR_PRINT("func:%s-s32Ret:%#x \n", __func__, s32Ret);
				goto End;
			}
		}
		started = 0;
	}

	if( type == CHL_JPEG_T )
	{
		vchn->venc_jpeg.started = started;
	}
	else
	{
		vchn->venc_chn[type].started = started;
	}

End:
	pthread_mutex_unlock(pLock);
	return s32Ret;
}
int	VEncChn_ForceIFrame(VENC_CHN_T* vchn, enum capture_channel_t type)
{
	if( type != CHL_MAIN_T && type != CHL_2END_T )
	{
		return -1;//!Ŀǰ��֧��������������
	}

	VENC_CHN_INFO_t* pVencChn = &vchn->venc_chn[type];

	pthread_mutex_lock(&pVencChn->lock);

	HI_S32 s32Ret = HI_SUCCESS;

	s32Ret = HI_MPI_VENC_RequestIDR( pVencChn->vencChn );
	if(s32Ret != HI_SUCCESS)
	{
		ERR_PRINT("HI_MPI_VENC_RequestIDR-err:%#x, chn:%d, vencChn:%d\n",
				s32Ret, vchn->Chn, pVencChn->vencChn);
		goto End;
	}

End:
	pthread_mutex_unlock(&pVencChn->lock);
	return s32Ret;
}
int	VEncChn_SetSnapParam(VENC_CHN_T* vchn, char imgq, int nFps)
{
	VENC_JPEG_CHN_T* pVencJpeg = &(vchn->venc_jpeg);
	//pthread_mutex_lock(&pVencJpeg->lock);
	HI_S32 s32Ret = HI_SUCCESS;
	VENC_CHN venchn = GetVencChnByChn(vchn->Chn, CHL_JPEG_T);
	VENC_CHN_ATTR_S stAttr;
	VENC_ATTR_JPEG_S* stJpegAttr =NULL;
	printf("%d:%s()\n",__LINE__,__FUNCTION__);
	if(imgq <= 0 )
	{
		imgq = 1;
	}
	else if( imgq >= 6 )
	{
		imgq = 5;
	}

	vchn->venc_jpeg.QVal = imgq;
	vchn->venc_jpeg.nFps =	nFps;
#if 0
	stAttr.stVeAttr.enType = PT_JPEG;
	stJpegAttr = (VENC_ATTR_JPEG_S*)&(stAttr.stVeAttr.stAttrJpeg);

	s32Ret = HI_MPI_VENC_GetChnAttr( venchn,  &stAttr);
	if( s32Ret != 0 )
	{
		ERR_PRINT("HI_MPI_VENC_GetChnAttr-s32Ret:%#x\n", s32Ret);
		goto End;
	}
	stJpegAttr->u32BufSize = stJpegAttr->u32PicHeight*stJpegAttr->u32PicWidth*2;
	stJpegAttr->bVIField = HI_TRUE;
	stJpegAttr->bByFrame = HI_TRUE;

	s32Ret = VEncChn_SetSnapAttr(vchn, &stAttr);
	if( s32Ret != HI_SUCCESS )
	{
		ERR_PRINT("VEncChn_SetSnapAttr-s32Ret:%#x\n", s32Ret);
		goto End;
	}

End:
	s32Ret = pthread_mutex_unlock(&pVencJpeg->lock);
#endif
	return s32Ret;
}

