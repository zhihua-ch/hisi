#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "dvrinc.h"
#include "avenc.h"
#include "hi_comm_aenc.h"
#include "initial.h"

#define VENC_TEST   1

int VencGetThread(void *arg);
static void VencMaxFd(Capture_Dev_t* pThis, HI_S32 fd);

int main(int argc, char *argv[]){
    //packetmanager::config(); 40M
    //ip address set
    //config reader json ; configDigiCh.init
    // timer, threadmanager
    //device manager start
/*device  cpature size*/
    static Capture_Dev_t g_hidev = {0};
    g_hidev.sys_init = 1;
    g_hidev.venc_opened = 0;
    g_hidev.aenc_opened = 0;
    g_hidev.video_codec_format = CAPTURE_COMP_H264;
    //g_hidev.audio_codec_format = AUDIO_G771A/PCM8;
    FD_ZERO(&(g_hidev.read_fds));
        
    HI_S32 s32Ret;
    s32Ret = vim_init(VIDEO_ENCODING_MODE_PAL);
    
    HI_ASSERT( s32Ret == HI_SUCCESS);    

    //InitOSD();
    g_hidev.vi_chn[0].bDiscardMainVencFlag = 0;
    /*init video encode channel*/
    VEncChn_Init_General(&g_hidev.vi_chn[0].venc_chn_t, 0);
    /*init audio encode channel*/
    //AEncChn_Init(&pThis->vi_chn[chn].aenc_chn, chn);    
    //if(product_dep->total_encode_channel>0)
	//{
		s32Ret = pthread_create( g_hidev.pid_vencode, NULL, (void*)VencGetThread, (void*)&g_hidev );
		assert( 0 == s32Ret);
	/*#ifndef NO_SNAP_IN_LIB
		s32Ret = pthread_create(&pThis->pid_venc_snap, NULL, (void*)SnapGetStream, (void*)&g_hidev );
		assert( 0 == s32Ret);
	#endif*/
	//}

    //VideoCreate();
    nvp6124_videoCreate();
    /* AudioCreate()     */
    
    //GUI  Graphics2Create();
    //AudioInGetChannels();
    //CExCaptureManager::Init();

    nvp6124_videoGetVstd();
}
static void VencMaxFd(Capture_Dev_t* pThis, HI_S32 fd)
{
	FD_SET(fd,&(pThis->read_fds));
	if(fd > pThis->MaxVencFd)
	{
		pThis->MaxVencFd = fd;
	}
	return ;
}

int VencGetThread(void *arg){
    int i, iIndex;
	HI_S32 s32Ret;
	
	Capture_Dev_t* pThis = (Capture_Dev_t*)arg;
    //osd time
    pThis->vi_chn[0].venc_chn_t.venc_chn[0].started = 1;

    VENC_CHN_T* pVChnInfo = &pThis->vi_chn[0].venc_chn_t;

    if(pVChnInfo->venc_chn[0].started == 1)
        VencMaxFd(pThis, pVChnInfo->venc_chn[0].venc_fd);
    //audio encode

    //VencGetStream() 
    VENC_CHN_STAT_S stStat;
    static VENC_STREAM_S vencStream;
    
	//!够大了，海斯码流 一帧应该没这么多包
	#define PACKNUM 10
	static VENC_PACK_S venc_Pack[PACKNUM];
    VENC_STREAM_S *pVencStream = &vencStream;
    

    if (pVChnInfo->venc_chn[0].started == 1
        && FD_ISSET(pVChnInfo->venc_chn[0].venc_fd, &(pThis->read_fds) )
        )
    {
        assert( pVChnInfo->venc_chn[0].venc_fd != -1);
        
        iIndex = 0;

        s32Ret = HI_MPI_VENC_Query(pVChnInfo->venc_chn[0].vencChn, &stStat);
    //	LIBDVR_PRINT("HI_MPI_VENC_Query\n");
        if ( s32Ret != HI_SUCCESS )
        {
            ERR_PRINT("query error:%#x!, channel:%d\n", s32Ret, 0);//chn
           // pthread_mutex_unlock(&pVChnInfo->venc_chn[j].lock);
            //continue;
        }
        if(PACKNUM>=stStat.u32CurPacks)
        {
            pVencStream->pstPack =venc_Pack;
        }
        else
        {
            LIBDVR_PRINT("HERE\r\n");
            pVencStream->pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S)*(stStat.u32CurPacks));
            if (NULL == pVencStream->pstPack)
            {
                ERR_PRINT("malloc error:%#x!, channel:%d\n", s32Ret, 0);//chn
               // pthread_mutex_unlock(&pVChnInfo->venc_chn[j].lock);
               // continue;
            }
        }
        pVencStream->u32PackCount = stStat.u32CurPacks;
        s32Ret = HI_MPI_VENC_GetStream(pVChnInfo->venc_chn[0].vencChn, pVencStream, HI_TRUE);
        if ( s32Ret != HI_SUCCESS )
        {
            ERR_PRINT("ret : 0x%x\n", s32Ret);
            if(PACKNUM<stStat.u32CurPacks)
            {
                free(pVencStream->pstPack);
            }
            /*pthread_mutex_unlock(&pVChnInfo->venc_chn[j].lock);
            continue;*/
        }
        else
        {
    #ifdef VENC_TEST
            static FILE *pFile;
            static int count=0;
            static char file_create = 0;
            static HI_CHAR aszFileName[64];
            if(file_create == 0)
            {
                sprintf(aszFileName, "/mnt/sdb1/stream_chn0%s", ".H264");
                pFile = fopen(aszFileName, "wb");
                if (!pFile)
                {
                    printf("open file[%s] failed!\n", aszFileName);
                    return NULL;
                }
                file_create = 1;
                printf("venc file open success pFile=%#x %s\n",aszFileName,pFile);
            }
            if(count++<50)
            {
                printf("save venc to file count=%d,pFile=%#x\n",count,pFile);
                SAMPLE_COMM_VENC_SaveStream(PT_H264, pFile,pVencStream);
            }
            else if(count== 50)
            {
                count = 100;
                printf("save venc file over,pFile=%#x\n",pFile);
                fclose(pFile);
            }
            else
            {
                printf("nothing do\n");
            }
    #endif
    //		printf("%d:%s()\n",__LINE__,__FUNCTION__);
            s32Ret = pThis->pVenc_PF(pThis, 0, 0, iIndex, pVencStream);//chn
            HI_ASSERT(s32Ret == HI_SUCCESS);
            s32Ret = HI_MPI_VENC_ReleaseStream(pVChnInfo->venc_chn[0].vencChn, pVencStream);
            if ( s32Ret != HI_SUCCESS )
            {
                ERR_PRINT("HI_MPI_VENC_ReleaseStream-error:%#x\n", s32Ret);
                if(PACKNUM<stStat.u32CurPacks)
                {
                    free(pVencStream->pstPack);
                }
                /*pthread_mutex_unlock(&pVChnInfo->venc_chn[j].lock);
                break;*/
            }
        }
        if(PACKNUM<stStat.u32CurPacks)
        {
            free(pVencStream->pstPack);
            pVencStream->pstPack = NULL;
        }
    }//END if pVChnInfo
 //   pthread_mutex_unlock(&pVChnInfo->venc_chn[j].lock);




/*s32Ret = pthread_mutex_unlock(&(pThis->lock));
HI_ASSERT(s32Ret == HI_SUCCESS);*/

return 0;    
}