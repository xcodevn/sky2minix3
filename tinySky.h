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
    /* TODO */
};
typedef struct device * device_t;

struct mtx { /*TODO */ };

struct callout { /*TODO */ };

/* End of wrapper */

/* Include all definitions from if_mskreg.h (FreeBSD) */
#include "if_mskreg.h"
#include "io_port.h"

#define SKY_DEBUG(level, args) \
    if ((level) <= SKY_VERBOSE) \
    { \
        printf args; \
    } \

typedef struct sky
{
    char name[8];         /**< String containing the device name. */
    int status;           /**< Describes the card's current state. */
    int irq;              /**< Interrupt Request Vector. */
    int irq_hook;                 /**< Interrupt Request Vector Hook. */
    u8_t *regs;           /**< Memory mapped hardware registers. */
    ether_addr_t address;     /**< Ethernet MAC address. */
    char *rx_buffer;          /**< Receive buffer returned by malloc(). */
    int rx_buffer_size;       /**< Size of the receive buffer. */

    char *tx_buffer;          /**< Transmit buffer returned by malloc(). */
    int tx_buffer_size;       /**< Size of the transmit buffer. */

    int client;                   /**< Process ID being served by e1000. */
    message rx_message;       /**< Read message received from client. */
    message tx_message;       /**< Write message received from client. */
    size_t rx_size;       /**< Size of one packet received. */
}
sky_t;


/**
 * @name Status Flags.
 * @{
 */

/** Card has been detected on the PCI bus. */
#define SKY_DETECTED (1 << 0)

/** Card is enabled. */
#define SKY_ENABLED  (1 << 1)

/** Client has requested to receive packets. */
#define SKY_READING  (1 << 2)

/** Client has requested to write packets. */
#define SKY_WRITING  (1 << 3)

/** Received some packets on the card. */
#define SKY_RECEIVED (1 << 4)

/** Transmitted some packets on the card. */
#define SKY_TRANSMIT (1 << 5)

/* TODO HERE */

#endif /* __TINYSKY_H */
