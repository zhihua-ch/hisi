BASEDIR = .
include ./Makefile.para

OBJS = main.o 
OBJS += dvr/i2c/i2c.o\
		dvr/sysreg/sys_reg_public.o\
		hicap/common/sample_comm_venc.o \
		hicap/common/sample_comm_vpss.o\
		hicap/common/sample_comm_sys.o\
		hicap/pub/init.o\
		hicap/venc/videoenc.o\
		fvideo/fvideo.o
		
#		hicap/venc/osdtitle.o

DIST_MAP := myrec.Map

INC_FLAGS :=  -I$(BASEDIR)/include/3521mpi
INC_FLAGS += -I$(BASEDIR)/include/APIs
INC_FLAGS += -I$(BASEDIR)/include/libhicap

HIARCH := hi3521

CFLAGS := -Wall -g $(INC_FLAGS) -D$(HIARCH) -lpthread -lm
CFLAGS += -Wl,-gc-sections 
myrec :%: $(OBJS)
#	$(CC) $(CFLAGS) $(COMPILEOPT) -o $@ $+ $(LFLAGS) -Wl,-Map,$(DIST_MAP)
	$(CC) $(CFLAGS) -lpthread -lm -o $@ $^ $(COMLIBS_HISI)
#.c.o:
#	$(CC) -c $(CFLAGS) $< -o $@ 

#.cpp.o:
#	$(CPP)  $(CFLAGS) -c -o $@ $<

clean:
	rm $(OBJS) -f

#$(CPP) $(CFLAGS) $(COMPILEOPT) -o $@ $+ $(LFLAGS) -Wl,-Map,$(DIST_MAP)