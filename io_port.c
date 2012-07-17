#include "io_port.h"

/* Copied from rtl8139.c */
unsigned my_inb(u16_t port) {
	u32_t value;
	int s;
	if ((s=sys_inb(port, &value)) !=OK)
		printf("SKY: warning, sys_inb failed: %d\n", s);
	return value;
}
unsigned my_inw(u16_t port) {
	u32_t value;
	int s;
	if ((s=sys_inw(port, &value)) !=OK)
		printf("SKY: warning, sys_inw failed: %d\n", s);
	return value;
}
unsigned my_inl(u16_t port) {
	u32_t value;
	int s;
	if ((s=sys_inl(port, &value)) !=OK)
		printf("SKY: warning, sys_inl failed: %d\n", s);
	return value;
}
void my_outb(u16_t port, u8_t value) {
	int s;
	if ((s=sys_outb(port, value)) !=OK)
		printf("SKY: warning, sys_outb failed: %d\n", s);
}
void my_outw(u16_t port, u16_t value) {
	int s;
	if ((s=sys_outw(port, value)) !=OK)
		printf("SKY: warning, sys_outw failed: %d\n", s);
}
void my_outl(u16_t port, u32_t value) {
	int s;
	if ((s=sys_outl(port, value)) !=OK)
		printf("SKY: warning, sys_outl failed: %d\n", s);
}
/* end of copy from rtl8139.c */
