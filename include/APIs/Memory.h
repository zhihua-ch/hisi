

#ifndef __MEMORY_MANAGENENT_API_H__
#define __MEMORY_MANAGENENT_API_H__

#include "DVRDEF.H"

#ifdef __cplusplus
extern "C" {
#endif

/// \defgroup MemoryAPI API Memory
/// �ڴ����API��������֧���ڴ���ҳʽ�������̹߳����˽ӿڱȽ����⣬������
///	�ϲ�ʵ�֣��ײ������Ҫ����/�ͷſ��Ժ�Ӧ�ò㽻�����ڴ�飬 һ��Ҫʹ�����׽ӿڡ�
///	\n ��������ͼ:
/// \code
///    ===========================
///                |
///           MemoryAlloc
///   +------------|            
///   |     MemorySetLength 
///   |      MemoryAddRef  
///   |      MemoryRelease  
///   +------------|             
///    ===========================
/// \endcode
/// @{

typedef struct MEMORY_BLOCK
{
	/// �ڴ������
	VD_HANDLE		handle;

	/// �ڴ��Ļ���ָ�롣
	uchar*		ptr;

	/// 32�ֽ�ͷ�������Ŷ�������ݡ�
	uchar*		header;

	/// �ڴ��Ļ�����Ч�ֽ�����
	uint		length;	

	/// ʵ�����뵽�Ļ�������ֽ�����һ����2nKbytes��nΪ��������ָ��ĵ�ַ��1Kbytes
	/// Ϊ��λ���롣
	uint		size;

	/// ��һ���ṹָ�룬�����ײ�ʹ�á�
	struct MEMORY_BLOCK *next;

	/// ��һ���ṹָ�룬�����ײ�ʹ�á�
	struct MEMORY_BLOCK *prev;

	/// ��־1�������ײ�ʹ�á�
	uint		flag1;

	/// ��־2�������ײ�ʹ�á�
	uint		flag2;
}MEMORY_BLOCK;


/// �����ڴ�顣
/// 
/// \param [out] pBlock ָ���ڴ��ṹMEMORY_BLOCK��ָ�롣
/// \param [in] dwBytes ��Ҫ������ڴ����ֽ���.
/// \retval 0  ����ɹ�
/// \retval <0  ����ʧ��
int MemoryAlloc(MEMORY_BLOCK * pBlock, uint dwBytes);


/// �����ڴ����ʵ�����ݵĳ��ȡ�
/// 
/// \param [in] hMemory �ڴ������ 
/// \param [in] dwBytes �ڴ����ʵ�����ݵĳ��ȡ�
/// \retval 0  ���ɹ�
/// \retval <0  ��ʧ��
int MemorySetLength(VD_HANDLE hMemory, uint dwBytes);


/// �����ڴ������ü�����
/// 
/// \param [in] hMemory �ڴ������
/// \retval 0  ���ӳɹ�
/// \retval <0  ����ʧ��
int MemoryAddRef(VD_HANDLE hMemory);


/// �ͷ��ڴ�顣�ڴ��õ����ʼ���ü���Ϊ1��һ�ε���MemoryAddRef�����ͷš����
/// ���ü��������ӹ�������Ҫ�ͷŶ�Ӧ�Ĵ������ڴ����������ͷš�
/// 
/// \param [in] hMemory �ڴ������
/// \retval 0  �ͷųɹ�
/// \retval <0  �ͷ�ʧ��
int MemoryRelease(VD_HANDLE hMemory);
/// @}


///////////////////////////////////////////////////////////////////////////////
/// \defgroup ExtraMemoryAPI API Extra Memory
/// �����ڴ����API, ���ǵ���CPU���ڴ�����, Ӧ�ó���ĳЩģ�飨����������ʹ�õ���
/// �ڴ�, ���ܲ�����CPU���ڴ�, ��������DSP���ڴ�, ��������һ�׶����ڴ�ķ��ʽӿ�, 
/// ����Щ�����ڴ�����������
///	\n ��������ͼ:
/// \code
///    ===========================
///                |
///         ExtraMemoryAlloc
///   +------------|            
///   |     ExtraMemoryCopy 
///   |      ExtraMemorySet  
///   +------------|             
///         ExtraMemoryFree  
///                |
///    ===========================
/// \endcode
/// \note ���û�п��õĶ����ڴ棬�������ֱ��ʹ��c���ϵ�к�����ʵ�ּ��ɡ��˽ӿ�
/// ��δʵ�ֺ�ʹ�á�
/// @{


/// ��������ڴ�飬�ο�malloc��
/// 
/// \param [in] nSize ��Ҫ������ڴ����ֽ���.
/// \retval 0  ����ʧ��
/// \retval !0 ���뵽���ڴ���ָ��
void *ExtraMemoryAlloc(int nSize);


/// �ͷŶ����ڴ�飬�ο�free��
/// 
/// \param [in] pBuf ��Ҫ�ͷŵ��ڴ���ָ��.
/// \return��
void ExtraMemoryFree(void *pBuf);


/// �ڴ濽�����ο�memcpy��
/// 
/// \param [in] pDest ���濽�����ݵĻ��塣
/// \param [in] pSrc ȡ�������ݵĻ��塣
/// \param [in] nCount ��Ҫ�������ֽ�����
/// \return ����pDest��ֵ��
void *ExtraMemoryCopy(void *pDest, const void *pSrc, int nCount);


/// �������������ó��ض���ֵ���ο�memset��
/// 
/// \param [in] pDest �����õĻ��塣
/// \param [in] c �����óɵ��ַ���
/// \param [in] nCount ��Ҫ���õ��ֽ�����
/// \return ����pDest��ֵ��
void *ExtraMemorySet(void *pDest, int c, int nCount);

/// @} end of group

#ifdef __cplusplus
}
#endif

#endif //__MEMORY_MANAGENENT_API_H__


