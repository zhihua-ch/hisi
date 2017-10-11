
#ifndef _SYS_REG_PUBLIC_API_H
#define _SYS_REG_PUBLIC_API_H
typedef enum
{
	REG_READ_CMD,
	REG_WRITE_CMD,
	REG_CMD_MAX
}Reg_cmd_operations_enum;

#define REG_READ_WRITE		1
#define REG_WRITE_ONLY		0

#define BIT_AND_OPER		0
#define BIT_OR_OPER 		1
#define NO_OPER			-1

  
#define SYS_REG_CMD_DEV "/dev/reg_cmd"

typedef struct 
{
	unsigned int reg_optical_addr;
	unsigned int reg_value;
}Reg_infor_struct;


int sys_reg_read(Reg_infor_struct *pReg_infor);
int sys_reg_write(Reg_infor_struct *pReg_infor);
int sys_reg_config(unsigned long addr,unsigned long data,unsigned char rw_en,unsigned char flag);

#endif

