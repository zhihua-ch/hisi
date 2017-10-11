
#ifndef __I2C_FUNCTION_H__
#define __I2C_FUNCTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	HII2C_DEVICE	"/dev/hi_i2c"
#define HII2C_CMD_READ  0x2
#define HII2C_CMD_WRITE 0x1
#define	GPIOI2C_DEVICE0	"/dev/gpioi2c_0"
#define	GPIOI2C_RTC_DEVICE	"/dev/gpioi2c_rtc"
#define GPIO_I2C_READ   0x01
#define GPIO_I2C_WRITE  0x03


typedef struct hiI2C_DATA_S
{
	unsigned char dev_addr; 
	unsigned int reg_addr; 
	unsigned int addr_byte; 
	unsigned int data; 
	unsigned int data_byte;
    
} I2C_DATA_S;

struct i2c_ioctl_data
{
	unsigned char devaddr;	/*dev addr*/
	unsigned int  regaddr;	/*reg addr*/
	int reg_count;	/* re*/	
	unsigned char* data;	/*return data*/
	unsigned long data_count; /*read or write data count*/
};

/// ��ȡ����
/// \param[in] type �豸����,�μ�ö��enum_dev_type
/// \param[in] device_addr	�豸��ַ
/// \param[in] reg_addr		�Ĵ�����ַ
/// \param[out] pReg_value ��ȡ����ֵ
/// \return �ɹ�����0,ʧ�ܷ���-1
int I2C_Read(int type, int device_addr, int reg_addr, int *pReg_value);

/// д����
/// \param[in] type �豸����,�μ�ö��enum_dev_type
/// \param[in] device_addr	�豸��ַ
/// \param[in] reg_addr		�Ĵ�����ַ
/// \param[in] reg_value	��д���ֵ
/// \return �ɹ�����0,ʧ�ܷ���-1
int I2C_Write(int type, int device_addr, int reg_addr, int reg_value);

int GPIO_I2C_Read0(int device_addr, int reg_addr);
int GPIO_I2C_Write0(int device_addr, int reg_addr, int reg_value);
int  i2c_read_raw(const unsigned char chip_addr, const unsigned int reg_addr, int reg_count,  unsigned char *data, unsigned char data_count);
int  i2c_write_raw(const unsigned char chip_addr, const unsigned int reg_addr, int reg_count, const unsigned char *data, unsigned char data_count);
#ifdef __cplusplus
}
#endif

#endif //__I2C_FUNCTION_H__



