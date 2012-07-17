/*
 * tinySky.h
 * 
 * Created: Jul-17-2012 by Thong T. Nguyen <xcodevn at gmail dot com>
 * 
 */
 
#ifndef __TINYSKY_H
#define __TINYSKY_H

/* Start of COPY from rtl8139.h */
#include <minix/drivers.h>
#include <minix/netdriver.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <minix/com.h>
#include <minix/ds.h>
#include <minix/keymap.h>
#include <minix/syslib.h>
#include <minix/type.h>
#include <minix/sysutil.h>
#include <minix/endpoint.h>
#include <timers.h>
#include <net/hton.h>
#include <net/gen/ether.h>
#include <net/gen/eth_io.h>
#include <machine/pci.h>

#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/ioc_memory.h>
#include "kernel/const.h"
#include "kernel/config.h"
#include "kernel/type.h"
/* End of COPY from rtl8139.h */

/* Wrapper FreeBSD header file */
#include "_bus_dma.h"
#include "i386_bus.h"

struct device{
	
};
typedef struct device * device_t;
/* End of wrapper */

/* Include all definitions from if_mskreg.h (FreeBSD) */
#include "if_mskreg.h"
#include "io_port.h"

/* TODO HERE */

#endif /* __TINYSKY_H */
