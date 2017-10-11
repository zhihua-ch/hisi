
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "i2c.h"

static int fd_hii2c_dev = -1;
static int fd_GPIOI2c_dev0 = -1;
static int fd_GPIOI2c_rtc_dev = -1;

static int	I2C_Open()
{

	if(fd_hii2c_dev < 0)
	{
		fd_hii2c_dev = open(HII2C_DEVICE, O_RDWR);
		if(fd_hii2c_dev < 0)
		{
			printf("%s(), i2c device open faild! \n", __FUNCTION__);
			return -1;
		}		
	}
	return 0;
}

static int I2C_Close()
{
	if (fd_hii2c_dev >= 0)
	{
		printf("@ERROR@@@@HII2C_Close@@@@@@@@@@@@@@@@@@@\r\n");
		close(fd_hii2c_dev);
	}
	return 0;
}
static int HII2C_Read(int type, int device_addr, int reg_addr, int *pReg_value)
{
	if (I2C_Open() != 0)
	{
		return -1;
	}
	I2C_DATA_S  ioctl_para;
	ioctl_para.dev_addr = device_addr;
	ioctl_para.reg_addr = reg_addr;//8 bits addr 	

	ioctl_para.addr_byte = 1;
	ioctl_para.data = 0;
	ioctl_para.data_byte = 1;
	if(ioctl(fd_hii2c_dev, HII2C_CMD_READ, &ioctl_para))
	{
		printf("I2C_Read- read reg error\n");
		return -1;
	}
	*pReg_value = ioctl_para.data;
	return 0;

}
static int HII2C_Write(int type, int device_addr, int reg_addr, int reg_value)
{
	if (I2C_Open() != 0)
	{
		return -1;
	}	
	I2C_DATA_S	ioctl_para;
	ioctl_para.dev_addr = device_addr;
	ioctl_para.reg_addr = reg_addr;//8 bits addr	
	
	ioctl_para.addr_byte = 1;
	ioctl_para.data = reg_value;
	ioctl_para.data_byte = 1;

	if(ioctl(fd_hii2c_dev, HII2C_CMD_WRITE, &ioctl_para))
	{
		printf("I2C_Write- write reg error\n");
		return -1;
	}
	return 0;
}

int I2C_Read(int type, int device_addr, int reg_addr, int *pReg_value)
{
	int ret;

	ret=HII2C_Read(type, device_addr, reg_addr,pReg_value);
	return ret;
}
int I2C_Write(int type, int device_addr, int reg_addr, int reg_value)
{
	int ret;
	ret=HII2C_Write(type,device_addr,reg_addr,reg_value);
	return ret;
}

static int	GPIOI2C_Open0()
{

	if(fd_GPIOI2c_dev0 < 0)
	{
		fd_GPIOI2c_dev0 = open(GPIOI2C_DEVICE0, O_RDWR);
		if(fd_GPIOI2c_dev0 < 0)
		{
			printf("%s(), i2c device %s open faild! \n", __FUNCTION__,GPIOI2C_DEVICE0);
			return -1;
		}		
	}
	return 0;
}
static int GPIOI2C_Close0()
{
	if (fd_GPIOI2c_dev0 >= 0)
	{
		printf("@ERROR@@@@HII2C_Close@@@@@@@@@@@@@@@@@@@\r\n");
		close(fd_GPIOI2c_dev0);
	}
	return 0;
}

int GPIO_I2C_Read0(int device_addr, int reg_addr)
{
	unsigned int value = 0;
	int ret = 0 ;

	if(fd_GPIOI2c_dev0 < 0)
		GPIOI2C_Open0();
	value = ((device_addr&0xff)<<24) | ((reg_addr&0xff)<<16);	
	ret = ioctl(fd_GPIOI2c_dev0, GPIO_I2C_READ, &value);			  
	value = value&0xff;
	return value;
}
int GPIO_I2C_Write0(int device_addr, int reg_addr, int reg_value)
{
	unsigned int value = 0;
	int ret;
	if(fd_GPIOI2c_dev0 < 0)
		GPIOI2C_Open0();
	value = ((device_addr&0xff)<<24) | ((reg_addr&0xff)<<16) | (reg_value&0xffff);
	ret = ioctl(fd_GPIOI2c_dev0, GPIO_I2C_WRITE, &value);
	return ret;
}

static int	GPIOI2C_RTC_Open()
{
	if(fd_GPIOI2c_rtc_dev < 0)
	{
		fd_GPIOI2c_rtc_dev = open(GPIOI2C_RTC_DEVICE, O_RDWR);
		if(fd_GPIOI2c_rtc_dev < 0)
		{
			printf("%s(), i2c device %s open faild! \n", __FUNCTION__,GPIOI2C_RTC_DEVICE);
			return -1;
		}		
	}
	return 0;
}
static int GPIOI2C_RTC_Close()
{
	if (fd_GPIOI2c_rtc_dev >= 0)
	{
		printf("@ERROR@@@@HII2C_Close@@@@@@@@@@@@@@@@@@@\r\n");
		close(fd_GPIOI2c_rtc_dev);
	}
	return 0;
}
int  i2c_write_raw(const unsigned char chip_addr, const unsigned int reg_addr, int reg_count, const unsigned char *data, unsigned char data_count)
{
	int ret = 0;
	int i=0;
	if(NULL == data)
	{
		printf("\n Invalid Paramer(NULL Pointer)......\n");
		return -1;
	}
	if(fd_GPIOI2c_rtc_dev < 0)
		GPIOI2C_RTC_Open();
	struct i2c_ioctl_data  ioctl_para;
	ioctl_para.devaddr = chip_addr;
	ioctl_para.regaddr = reg_addr;//8 bits addr 	
	ioctl_para.reg_count = reg_count;
	ioctl_para.data_count = data_count;
	ioctl_para.data = data;
	if(ioctl(fd_GPIOI2c_rtc_dev, GPIO_I2C_WRITE, &ioctl_para))
	{
		printf("hisi_i2c - write reg error\n");
		return -1;
	}
	return 0;
}
int  i2c_read_raw(const unsigned char chip_addr, const unsigned int reg_addr, int reg_count,  unsigned char *data, unsigned char data_count)
{
	int ret = 0;
	int i=0;
	int value=0;
	if(NULL == data)
	{
		printf("\n Invalid Paramer(NULL Pointer)......\n");
		return -1;
	}
	if(fd_GPIOI2c_rtc_dev < 0)
		GPIOI2C_RTC_Open();
	struct i2c_ioctl_data  ioctl_para;
	ioctl_para.devaddr = chip_addr;
	ioctl_para.regaddr = reg_addr;//8 bits addr 	
	ioctl_para.reg_count = reg_count;
	ioctl_para.data_count = data_count;
	ioctl_para.data = data;
	if(ioctl(fd_GPIOI2c_rtc_dev, GPIO_I2C_READ, &ioctl_para))
	{
		printf("hisi_i2c - read reg error\n");
		return -1;
	}
	return 0;
}

