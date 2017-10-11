
/* Standard Linux headers */
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <sys/time.h>

#include "sys_reg_public.h"
///////////////////////////////////////////////////////
static int fd_freg_cmd = -1;
static int sys_reg_opened = 0;

static int sys_reg_init(void)
{
    fd_freg_cmd = open(SYS_REG_CMD_DEV, O_RDWR);
    if(fd_freg_cmd < 0)
    {
        printf(">>>>>>>>>Open fd_freg_cmd faild! \n");
        return -1;
    }
    
    sys_reg_opened = 1;
    return 0;
}

int sys_reg_read(Reg_infor_struct *pReg_infor)
{
    int ret = 0;
    if(NULL==pReg_infor)
    {
        printf("\nParamer error.....\n");
        return -1;
    }
    if(1 != sys_reg_opened)
    {
        ret = sys_reg_init();
        if(0 != ret)
        {
            printf("\n sys_reg device open fail......\n");
            return -1;
        }
    }
    if(ioctl(fd_freg_cmd, REG_READ_CMD, pReg_infor))
    {
        printf("freg_cmd - read reg error\n");
        return -1;
    }
    return 0;
}

int sys_reg_write(Reg_infor_struct *pReg_infor)
{
    int ret = 0;
    
    if(NULL==pReg_infor)
    {
        printf("\nParamer error.....\n");
        return -1;
    }
    
    if(1 != sys_reg_opened)
    {
        ret = sys_reg_init();
        if(0 != ret)
        {
            printf("\n sys_reg device open fail......\n");
            return -1;
        }
    }

    if(ioctl(fd_freg_cmd, REG_WRITE_CMD, pReg_infor))
    {
        printf("freg_cmd - write reg error\n");
        return -1;
    }
    return 0;
}

int sys_reg_config(unsigned long addr,unsigned long data,unsigned char rw_en,unsigned char flag)
{
	int ret = 0;
	unsigned int reg_temp_value;
	Reg_infor_struct reg;
	reg.reg_optical_addr=addr;
	reg.reg_value=0x00;
	if(rw_en)
	{
		ret = sys_reg_read(&reg);
		if(0!=ret)
		{
			printf("\n read register error....\n");
			return -1;
		}
		reg_temp_value = reg.reg_value;
	}	
	if(0==flag)
	{
		reg_temp_value &= data;
	}
	else if(1==flag)
	{
		reg_temp_value |= data;
	}
	else
	{
		reg_temp_value = data;		
	}
	reg.reg_value = reg_temp_value;
	
	ret = sys_reg_write(&reg);
	if(0!=ret)
	{
		printf("\n write register error....\n");
		return -1;
	}
	return 0;
}
