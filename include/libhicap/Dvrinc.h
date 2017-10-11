

#ifndef __DVRINC_H__
#define __DVRINC_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <sys/mman.h>
#include <linux/fb.h>
//#include <asm/page.h>
#include <assert.h>

#include "hi_comm_adec.h"
#include "hi_comm_aenc.h"
#include "hi_comm_ai.h"
#include "hi_comm_aio.h"
#include "hi_comm_ao.h"
#include "hi_comm_hdmi.h"
#include "hi_comm_ive.h"
#include "hi_comm_pciv.h"
#include "hi_comm_rc.h"
#include "hi_comm_region.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vda.h"
#include "hi_comm_vdec.h"
#include "hi_comm_venc.h"
#include "hi_comm_vi.h"
#include "hi_comm_video.h"
#include "hi_comm_vo.h"
#include "hi_comm_vpss.h"
#include "hi_common.h"
#include "hi_debug.h"
#include "hi_defines.h"
#include "hi_defines.h"
#include "hi_io.h"
#include "hi_jpeg_api.h"
#include "hi_math.h"
#include "hi_mem.h"
#include "hi_tde_api.h"
#include "hi_tde_errcode.h"
#include "hi_tde_type.h"
#include "hi_type.h"
#include "hifb.h"
#include "jconfig.h"
#include "jerror.h"
//#include "jmorecfg.h"

#include "jpeglib.h"
#include "list.h"
#include "mpi_adec.h"

#include "mpi_aenc.h"

#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_hdmi.h"

#include "mpi_ive.h"

#include "mpi_pciv.h"
#include "mpi_region.h"
#include "mpi_sys.h"

#include "mpi_vb.h"

#include "mpi_vda.h"
#include "mpi_vdec.h"
#include "mpi_venc.h"

#include "mpi_vi.h"

#include "mpi_vo.h"
#include "mpi_vpss.h"

#endif

