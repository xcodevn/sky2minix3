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

/* Wrapper Linux header file */
#include "_bus_dma.h"
#include "i386_bus.h"

struct device{
    /* TODO */
};
typedef struct device * device_t;

struct mtx { /*TODO */ };

struct callout { /*TODO */ };

typedef uint64_t           u64;
typedef int64_t            s64;
typedef unsigned int       u32;
typedef signed int         s32;
typedef unsigned short     u16;
typedef signed short       s16;
typedef unsigned char      u8;
typedef signed char        s8;


#ifdef CONFIG_NEED_DMA_MAP_STATE
#define DEFINE_DMA_UNMAP_ADDR(ADDR_NAME)        dma_addr_t ADDR_NAME
#define DEFINE_DMA_UNMAP_LEN(LEN_NAME)          __u32 LEN_NAME
#define dma_unmap_addr(PTR, ADDR_NAME)           ((PTR)->ADDR_NAME)
#define dma_unmap_addr_set(PTR, ADDR_NAME, VAL)  (((PTR)->ADDR_NAME) = (VAL))
#define dma_unmap_len(PTR, LEN_NAME)             ((PTR)->LEN_NAME)
#define dma_unmap_len_set(PTR, LEN_NAME, VAL)    (((PTR)->LEN_NAME) = (VAL))
#else
#define DEFINE_DMA_UNMAP_ADDR(ADDR_NAME)
#define DEFINE_DMA_UNMAP_LEN(LEN_NAME)
#define dma_unmap_addr(PTR, ADDR_NAME)           (0)
#define dma_unmap_addr_set(PTR, ADDR_NAME, VAL)  do { } while (0)
#define dma_unmap_len(PTR, LEN_NAME)             (0)
#define dma_unmap_len_set(PTR, LEN_NAME, VAL)    do { } while (0)
#endif

#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
typedef u64 dma_addr_t;
#else
typedef u32 dma_addr_t;
#endif

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#ifdef __CHECK_ENDIAN__
#define __bitwise __bitwise__
#else
#define __bitwise
#endif

#ifndef __BITS_PER_LONG
#define __BITS_PER_LONG 32
#endif


struct u64_stats_sync {
#if BITS_PER_LONG==32 && defined(CONFIG_SMP)
        seqcount_t      seq;
#endif
};

typedef struct spinlock {
	/* TODO */
} spinlock_t;

typedef struct seqcount {
        unsigned sequence;
} seqcount_t;

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

typedef __signed__ long __s64;
typedef unsigned long __u64;

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;

#define ____cacheline_aligned_in_smp /* FIXME: TODO */
#define __iomem 					 /* FIME: TOO */
struct napi_struct {
	/* TODO */
};
struct timer_list {
	/* TODO */
};

struct work_struct {
	/* TODO */
};

typedef struct wait_queue_head {
	/* TODO */
} wait_queue_head_t;
/* End of wrapper */

/* Register accessor for memory mapped device */
u32 readl(void* reg)
{
	return 0;
}

u16 readw(void* reg)
{
	return 0;
}

u8 readb(void* reg)
{
	return 0;
}

void writel(u32 val, void* reg)
{
	/* TODO */
}

void writew(u16 val, void* reg)
{
	/* TODO */
}

void writeb(u8 val, void* reg)
{
	/* TODO */
}


/* Include all definitions from LINUX */
#include "drivers_net_ethernet_marvell_sky2.h"

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
   	
   	// eth_stat_t stat;         /**<Ethernet status. */

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
