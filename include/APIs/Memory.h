

#ifndef __MEMORY_MANAGENENT_API_H__
#define __MEMORY_MANAGENENT_API_H__

#include "DVRDEF.H"

#ifdef __cplusplus
extern "C" {
#endif

/// \defgroup MemoryAPI API Memory
/// 内存管理API，用来来支持内存块的页式管理及多线程共享。此接口比较特殊，它是由
///	上层实现，底层如果需要申请/释放可以和应用层交互的内存块， 一定要使用这套接口。
///	\n 调用流程图:
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
	/// 内存块句柄。
	VD_HANDLE		handle;

	/// 内存块的缓冲指针。
	uchar*		ptr;

	/// 32字节头，用来放额外的数据。
	uchar*		header;

	/// 内存块的缓冲有效字节数。
	uint		length;	

	/// 实际申请到的缓冲最大字节数，一定是2nKbytes，n为整数。且指针的地址以1Kbytes
	/// 为单位对齐。
	uint		size;

	/// 下一个结构指针，仅供底层使用。
	struct MEMORY_BLOCK *next;

	/// 上一个结构指针，仅供底层使用。
	struct MEMORY_BLOCK *prev;

	/// 标志1，仅供底层使用。
	uint		flag1;

	/// 标志2，仅供底层使用。
	uint		flag2;
}MEMORY_BLOCK;


/// 申请内存块。
/// 
/// \param [out] pBlock 指向内存块结构MEMORY_BLOCK的指针。
/// \param [in] dwBytes 需要申请的内存块的字节数.
/// \retval 0  申请成功
/// \retval <0  申请失败
int MemoryAlloc(MEMORY_BLOCK * pBlock, uint dwBytes);


/// 设置内存块内实际数据的长度。
/// 
/// \param [in] hMemory 内存块句柄。 
/// \param [in] dwBytes 内存块内实际数据的长度。
/// \retval 0  读成功
/// \retval <0  读失败
int MemorySetLength(VD_HANDLE hMemory, uint dwBytes);


/// 增加内存块的引用计数。
/// 
/// \param [in] hMemory 内存块句柄。
/// \retval 0  增加成功
/// \retval <0  增加失败
int MemoryAddRef(VD_HANDLE hMemory);


/// 释放内存块。内存块得到后初始引用计数为1，一次调用MemoryAddRef即可释放。如果
/// 引用计数被增加过，则还需要释放对应的次数，内存块才真正被释放。
/// 
/// \param [in] hMemory 内存块句柄。
/// \retval 0  释放成功
/// \retval <0  释放失败
int MemoryRelease(VD_HANDLE hMemory);
/// @}


///////////////////////////////////////////////////////////////////////////////
/// \defgroup ExtraMemoryAPI API Extra Memory
/// 额外内存管理API, 考虑到主CPU的内存有限, 应用程序某些模块（比如升级）使用到的
/// 内存, 可能不是主CPU的内存, 而是其他DSP的内存, 所以增加一套额外内存的访问接口, 
/// 把这些额外内存利用起来。
///	\n 调用流程图:
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
/// \note 如果没有可用的额外内存，则库里面直接使用c库的系列函数来实现即可。此接口
/// 暂未实现和使用。
/// @{


/// 申请额外内存块，参考malloc。
/// 
/// \param [in] nSize 需要申请的内存块的字节数.
/// \retval 0  申请失败
/// \retval !0 申请到的内存块的指针
void *ExtraMemoryAlloc(int nSize);


/// 释放额外内存块，参考free。
/// 
/// \param [in] pBuf 需要释放的内存块的指针.
/// \return无
void ExtraMemoryFree(void *pBuf);


/// 内存拷贝，参考memcpy。
/// 
/// \param [in] pDest 保存拷贝数据的缓冲。
/// \param [in] pSrc 取拷贝数据的缓冲。
/// \param [in] nCount 需要拷贝的字节数。
/// \return 返回pDest的值。
void *ExtraMemoryCopy(void *pDest, const void *pSrc, int nCount);


/// 将缓冲内容设置成特定的值，参考memset。
/// 
/// \param [in] pDest 待设置的缓冲。
/// \param [in] c 被设置成的字符。
/// \param [in] nCount 需要设置的字节数。
/// \return 返回pDest的值。
void *ExtraMemorySet(void *pDest, int c, int nCount);

/// @} end of group

#ifdef __cplusplus
}
#endif

#endif //__MEMORY_MANAGENENT_API_H__


