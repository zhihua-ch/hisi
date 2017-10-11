

#ifndef __LOSS_DETECT_API_H__
#define __LOSS_DETECT_API_H__

#include "DVRDEF.H"

#ifdef __cplusplus
extern "C" {
#endif

/// \defgroup LossDetectAPI API Loss Detect
/// ��Ƶ��ʧ�ӿڡ�
///	\n ��������ͼ:
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

/// ��Ƶ��ʧ����豸���Խṹ
typedef struct LOSS_DETECT_CAPS
{
	uint Enabled;		///< ��1��ʾ֧����Ƶ��ʧ��⣬��0��ʾ��֧����Ƶ��ʧ��⡣
}LOSS_DETECT_CAPS;


/// ������Ƶ��ʧ����豸
/// 
/// \param ��
/// \retval <0 ����ʧ��
/// \retval 0 �����ɹ�
int LossDetectCreate(void);


/// ������Ƶ��ʧ����豸
/// 
/// \param ��
/// \retval <0 ����ʧ��
/// \retval 0 ���ٳɹ�
int LossDetectDestory(void);


/// ִ����Ƶ��ʧ��⡣�������ء�
/// 
/// \param [out] pData ָ��һ��uintֵ��ָ�룬uintֵ�Ǹ�����������ͨ����ƽ״̬
///        �����롣��ͨ���ڵ�λ����ͨ���ڸ�λ���ߵ�ƽ��1���͵�ƽ��0�������ڵ�ͨ
///        ����0��
/// \retval 0  ���ɹ�
/// \retval <0  ���ʧ��
int LossDetectGetState(uint* pData);


/// �õ���Ƶ��ʧ֧�ֵ����ԡ�
/// 
/// \param [out] pCaps ָ����Ƶ��ʧ���ԽṹLOSS_DETECT _CAPS��ָ�롣
/// \retval 0  ��ȡ�ɹ���
/// \retval <0  ��ȡʧ�ܡ�
int LossDetectGetCaps(LOSS_DETECT_CAPS * pCaps);

/// @} end of group

#ifdef __cplusplus
}
#endif

#endif //__LOSS_DETECT_API_H__

