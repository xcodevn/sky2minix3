#
# Makefile for the Marvell Yukon E88E8040 ethernet driver (E88E8040)
#
# Using template from drivers/rtl8139/Makefile
#
PROG	=	tinySky
SRCS	=	tinySky.c io_port.c

FILES		=	$(PROG).conf
FILESNAME	= 	$(PROG)
FILESDIR		=	/etc/system.conf.d

DPADD	+= $(LIBNETDRIVER) $(LIBSYS) $(LIBTIMERS)
LDADD	+= -lnetdriver -lsys -ltimers

MAN	=
BINDIR	?= /usr/bin
CPPFLAGS	+= -I$(NETBSDSRCDIR)

.include <minix.service.mk>


