
#ifndef __AUDIOENC_H__
#define __AUDIOENC_H__

#include "DVRINC.h"

enum audio_codec_format_t {
	AUDIO_PCM8,
	AUDIO_G729,
	AUDIO_ADPCM,
	AUDIO_PCM_MULAW,
	AUDIO_G721,
	AUDIO_PCM8_VWIS,
	AUDIO_MS_ADPCM,
	AUDIO_G711A,
	AUDIO_AVI_AUDIO,
	AUDIO_NR,

};

typedef struct hiAUDIO_RAW_INFO_S
{
    AUDIO_BIT_WIDTH_E   enBitwidth;
    AUDIO_SOUND_MODE_E  enSoundmode;
    HI_U64 u64TimeStamp;
    HI_U32 u32Seq;
    HI_U32 u32Len;
}AUDIO_RAW_INFO_S;

/**********Audio Encode Start**************************/
typedef struct tagAENC_CHN_INFO
{
	int HiAencChn;
	int created;
	int started;
	int regular_enable;//!��������Ҫ��Ƶ
	int extra_enable;	//!��������Ҫ��Ƶ
	//pthread_t threadid;
	int Channel;//!�߼�ͨ����
	int AiDev;	//!�����豸��
	int AiChn;	//!����ͨ����
	unsigned long long oldpts;
	unsigned long long scr;
	int fd;
	PAYLOAD_TYPE_E enType;
	pthread_mutex_t lock;
} AENC_CHN_INFO_t;

void AEncChn_Init(AENC_CHN_INFO_t* vchn, int chn);
void AEncChn_Exit(AENC_CHN_INFO_t* vchn);

int AEncChn_Start(AENC_CHN_INFO_t* vchn);
int AEncChn_Stop(AENC_CHN_INFO_t* vchn);
/**********Audio Encode End**************************/

#endif
