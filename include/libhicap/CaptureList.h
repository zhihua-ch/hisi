#ifndef __CAPTURELIST_H__
#define __CAPTURELIST_H__

#include <time.h>
#include <sys/time.h>
#include "Memory.h"

/* ֡���Ͷ���*/
enum PackTypeFlag
{
	PACK_TYPE_FRAME_P=0,
	PACK_TYPE_FRAME_I,
	PACK_TYPE_FRAME_B,
	PACK_TYPE_FRAME_AUDIO,
	PACK_TYPE_FRAME_JEPG,
	PACK_TYPE_FRAME_MOTION,
	PACK_TYPE_FRAME_NUM,
	PACK_TYPE_FRAME_NULL=0xff,/*������������*/
};

enum FrameBeginEndFlag
{
	PACK_CONTAIN_FRAME_NONHT = 0, /*��ͷ��β,����֡ͷ��֡β */
	PACK_CONTAIN_FRAME_HEAD, /*ֻ��ͷ*/
	PACK_CONTAIN_FRAME_TRAIL, /*ֻ��β*/
	PACK_CONTAIN_FRAME_HEADTRAIL, /* ����֡ͷ��֡β */
	PACK_CONTAIN_FRAME_MOTION_HEAD = 5,
	PACK_CONTAIN_FRAME_NUM,
};

//! ֡��λ��Ϣ
typedef struct tagFRAMEINFO
{
	uchar FrameType;					/*!< ֡����, PackTypeFlag */
	uchar FrameFlag;					/*!< ֡ͷβ�ı�ʶ, enum FrameBeginEndFlag*/
	ushort FramePos;					/*!< ֡����ʼλ�� */
	uint FrameLength;				/*!< ֡�ĳ��ȣ����ܿ�� */
	ushort DataLength;				/*!< ֡�ڱ����еĳ��� */
	uchar Reserve[2];				/*!< ���� */
}FRAMEINFO;

typedef struct tagPKT_HEAD_INFO
{
	FRAMEINFO FrameInfo[8];
	uchar			PacketInfo;
	uchar			Reserve[7];
}PKT_HEAD_INFO;

typedef struct tagSTREAM_INFO
{
	uchar FrameType;
	uchar HeadTail;
	uint FrameLength;
	uint RemainLen;
	uint PacketLen;
}STREAM_INFO;

struct CAPTURE_LIST
{
	int length;
	int count;
	int frametype;
	int framelength;
	int ready;
	int start_length;
	struct timeval times;
	PKT_HEAD_INFO pack_head;
	struct CAPTURE_LIST *pre;
	struct CAPTURE_LIST *next;
	MEMORY_BLOCK avenc_buf;
};

typedef struct tagBLOCK_LIST
{
	struct CAPTURE_LIST *head;
	struct CAPTURE_LIST *tail;
	struct timeval sendtime;
	pthread_mutex_t lock;
}BLOCK_LIST;

typedef struct tagBLOCK_INFO
{
	int block_len;
	int packet_count;
}BLOCK_INFO;

int CapListCreate(BLOCK_LIST *list);
int CapListDestory(BLOCK_LIST *list);
struct CAPTURE_LIST *CapListAlloc(BLOCK_LIST *list, int len);
struct CAPTURE_LIST *CapListDetect(BLOCK_LIST *list, int len);
int CapListFree(BLOCK_LIST *list, struct CAPTURE_LIST *node);
struct CAPTURE_LIST *CapListHead(BLOCK_LIST *list);
struct CAPTURE_LIST *CapListGet(BLOCK_LIST *list);

int CapListInfo(BLOCK_LIST *list, BLOCK_INFO *info);

struct CAPTURE_LIST * StreamCopy(BLOCK_LIST *pList, struct CAPTURE_LIST *pbuf,
		uchar * SrcAddr, int Length,STREAM_INFO *pStreamInfo);
void CapList2Next(struct CAPTURE_LIST *pbuf);

#endif
