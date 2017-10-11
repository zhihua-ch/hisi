BASEDIR = .
include ./Makefile.para

OBJS = main.o init.o fvideo.o videoenc.o
OBJS += dvr/i2c/i2c.o\
		dvr/sysreg/sys_reg_public.o

DIST_MAP := myrec.Map

myrec : $(OBJS)
	$(CC) $(CFLAGS) $(COMPILEOPT) -o $@ $+ $(LFLAGS) -Wl,-Map,$(DIST_MAP)

#$(CPP) $(CFLAGS) $(COMPILEOPT) -o $@ $+ $(LFLAGS) -Wl,-Map,$(DIST_MAP)

.c.o:
	$(CC)  $(CFLAGS) -c -o $@ $<

.cpp.o:
	$(CPP)  $(CFLAGS) -c -o $@ $<

clean:
	rm $(OBJS) -f

