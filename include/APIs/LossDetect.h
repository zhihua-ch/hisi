

#ifndef __LOSS_DETECT_API_H__
#define __LOSS_DETECT_API_H__

#include "DVRDEF.H"

#ifdef __cplusplus
extern "C" {
#endif

/// \defgroup LossDetectAPI API Loss Detect
/// 视频丢失接口。
///	\n 调用流程图:
/// \code
///    ===========================
///                |
///        *LossDetectGetCaps
///                |
///         LossDetectCreate
///    +-----------|
///    |   LossDetectGetState
///    +-----------|
///       LossDetectDestory
///                |
///    ===========================
/// \endcode
/// @{

/// 视频丢失检测设备特性结构
typedef struct LOSS_DETECT_CAPS
{
	uint Enabled;		///< 置1表示支持视频丢失检测，置0表示不支持视频丢失检测。
}LOSS_DETECT_CAPS;


/// 创建视频丢失检测设备
/// 
/// \param 无
/// \retval <0 创建失败
/// \retval 0 创建成功
int LossDetectCreate(void);


/// 销毁视频丢失检测设备
/// 
/// \param 无
/// \retval <0 销毁失败
/// \retval 0 销毁成功
int LossDetectDestory(void);


/// 执行视频丢失检测。立即返回。
/// 
/// \param [out] pData 指向一个uint值的指针，uint值是各个报警输入通道电平状态
///        的掩码。低通道在低位，高通道在高位。高电平置1，低电平置0，不存在的通
///        道置0。
/// \retval 0  检测成功
/// \retval <0  检测失败
int LossDetectGetState(uint* pData);


/// 得到视频丢失支持的特性。
/// 
/// \param [out] pCaps 指向视频丢失特性结构LOSS_DETECT _CAPS的指针。
/// \retval 0  获取成功。
/// \retval <0  获取失败。
int LossDetectGetCaps(LOSS_DETECT_CAPS * pCaps);

/// @} end of group

#ifdef __cplusplus
}
#endif

#endif //__LOSS_DETECT_API_H__

