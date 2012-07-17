#ifndef __IO_PORT_H
#define __IO_PORT_H

#include <minix/drivers.h>
#include <minix/netdriver.h>
#include <minix/type.h>
#include <minix/sysutil.h>

#include "kernel/config.h"
#include <sys/types.h>
#include <kernel/type.h>
#include <sys/ioc_memory.h>
#include "kernel/const.h"
#include "kernel/type.h"

extern unsigned my_inb(u16_t port);
extern unsigned my_inw(u16_t port);
extern unsigned my_inl(u16_t port);
extern void my_outb(u16_t port, u8_t value);
extern void my_outw(u16_t port, u16_t value);
extern void my_outl(u16_t port, u32_t value);

#define sky_inb(port, offset)   (my_inb((port) + (offset)))
#define sky_inw(port, offset)   (my_inw((port) + (offset)))
#define sky_inl(port, offset)   (my_inl((port) + (offset)))
#define sky_outb(port, offset, value)   (my_outb((port) + (offset), (value)))
#define sky_outw(port, offset, value)   (my_outw((port) + (offset), (value)))
#define sky_outl(port, offset, value)   (my_outl((port) + (offset), (value)))

#endif /* __IO_PORT_H */
