BASEDIR = .
include ./Makefile.para

OBJS = main.o init.o fvideo.o videoenc.o
OBJS += dvr/i2c/i2c.o\
		dvr/sysreg/sys_reg_public.o


$(CPP) $(CFLAGS) $(COMPILEOPT) -o $@ $+ $(LFLAGS) -Wl,-Map,$(DIST_MAP)
