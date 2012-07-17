/*
 * tinySky.c
 *
 * This is a fast ethernet driver for Marwell yukon E88E8040 card on Minix3 OS
 *
 * Created: Jul-17-2012 by Thong T. Nguyen <xcodevn at gmail dot com>
 *
 * Change logs:
 *
 *
 */

#define SKY_VERBOSE 0       /* many levels */


#include "tinySky.h"

static message m;
static int int_event_check;     /* set to TRUE if events arrived */
static int re_instance;

static u32_t system_hz;

static int sky_instance;
static sky_t sky_state;

static void sky_watchdog_f(timer_t *tp);
static void do_hard_int(void);
static void check_int_events(void);
static void sky_dump(message *m);           /* pointer to request message */
static void sky_writev_s(const message *mp, int from_int);
static void sky_readv_s(const message *mp, int from_int);
static void sky_init(message *mp);
static void sky_getstat_s(message *mp);
static void sef_local_startup();
static int sef_cb_init_fresh(int type, sef_init_info_t *UNUSED(info));
static void sef_cb_signal_handler(int signo);
static void mess_reply(message *req, message *reply_mess);
static int sky_probe(sky_t *s, int skip);


void sef_cb_lu_state_dump(int state) {
    /* TODO */
}

int sef_cb_lu_prepare(int state) {
    /* TODO */
    return OK;
}

int sef_cb_lu_state_isvalid(int state) {
    /* TODO */
    return OK;
}

static void sef_cb_signal_handler(int signo) {
    /* TODO */
    /* Only check for termination signal, ignore anything else. */
    if (signo != SIGTERM) return;


    exit(0);
}

static int sef_cb_init_fresh(int type, sef_init_info_t *UNUSED(info)) {
    /* Initialize sky driver */
    long v;

    system_hz = sys_hz();
    v = 0;
    (void) env_parse("instance", "d", 0, &v, 0, 255);
    re_instance = (int) v;

    /* Clear state. */
    memset(&sky_state, 0, sizeof(sky_state));

    /* TODO */
    /* Initialize buffer memory */

    /* Announce we are up! */
    netdriver_announce();

    return OK;
}
static void sef_local_startup()
{
  /* Register init callbacks. */
  sef_setcb_init_fresh(sef_cb_init_fresh);
  sef_setcb_init_lu(sef_cb_init_fresh);
  sef_setcb_init_restart(sef_cb_init_fresh);

  /* Register live update callbacks. */
  sef_setcb_lu_prepare(sef_cb_lu_prepare);
  sef_setcb_lu_state_isvalid(sef_cb_lu_state_isvalid);
  sef_setcb_lu_state_dump(sef_cb_lu_state_dump);

  /* Register signal callbacks. */
  sef_setcb_signal_handler(sef_cb_signal_handler);

  /* Let SEF perform startup. */
  sef_startup();
}


static void sky_getstat_s(message *mp) {
    /* TODO */
}


static void sky_init_pci()
{
    sky_t *s;

    /* Initialize the PCI bus. */
    pci_init();

    /* Try to detect e1000's. */
    s = &sky_state;
    strcpy(s->name, "SKY02#0");
    s->name[6] += sky_instance;
    sky_probe(s, sky_instance);
}

static int sky_probe(sky_t *s, int skip)
{
    int r, devind, ioflag;
    u16_t vid, did, cr;
    u32_t status[2];
    u32_t base, size;
    u32_t gfpreg, sector_base_addr;
    char *dname;

    SKY_DEBUG(3, ("%s: probe()\n", s->name));

    /*
     * Attempt to iterate the PCI bus. Start at the beginning.
     */
    if ((r = pci_first_dev(&devind, &vid, &did)) == 0)
    {
    return FALSE;
    }
    /* Loop devices on the PCI bus. */
    while (skip--)
    {
    SKY_DEBUG(3, ("%s: probe() devind %d vid 0x%x did 0x%x\n",
                s->name, devind, vid, did));

    if (!(r = pci_next_dev(&devind, &vid, &did)))
    {
        return FALSE;
    }
    }
    /*
     * Successfully detected card on the PCI bus.
     */
    // s->status |= E1000_DETECTED;
    // s->eeprom_read = eeprom_eerd;

    /*
     * Set card specific properties.
     */
    // switch (did)
    // {
        // case E1000_DEV_ID_ICH10_D_BM_LM:
        // case E1000_DEV_ID_ICH10_R_BM_LF:
            // e->eeprom_read = eeprom_ich;
            // break;
//
        // case E1000_DEV_ID_82540EM:
    // case E1000_DEV_ID_82545EM:
        // e->eeprom_done_bit = (1 << 4);
        // e->eeprom_addr_off =  8;
        // break;
//
    // default:
        // e->eeprom_done_bit = (1 << 1);
        // e->eeprom_addr_off =  2;
        // break;
    // }

    /* Inform the user about the new card. */
    if (!(dname = pci_dev_name(vid, did)))
    {
        dname = "Marwell Yukon Fast Ethernet Card";
    }
    SKY_DEBUG(1, ("%s: %s (%04x/%04x/%02x) at %s\n",
             s->name, dname, vid, did, /*s->revision*/ 0,
             pci_slot_name(devind)));

    /* Reserve PCI resources found. */
    if ((r = pci_reserve_ok(devind)) != OK)
    {
        panic("failed to reserve PCI device: %d", r);
    }
    /* Read PCI configuration. */
    s->irq   = pci_attr_r8(devind, PCI_ILR);

    if ((r = pci_get_bar(devind, PCI_BAR, &base, &size, &ioflag)) != OK)
        panic("failed to get PCI BAR (%d)", r);
    if (ioflag) panic("PCI BAR is not for memory");

    s->regs  = vm_map_phys(SELF, (void *) base, size);
    if (s->regs == (u8_t *) -1) {
        panic("failed to map hardware registers from PCI");
    }

    /* FIXME: enable DMA bus mastering if necessary. This is disabled by
     * default on VMware. Eventually, the PCI driver should deal with this.
     */
    // cr = pci_attr_r16(devind, PCI_CR);
    // if (!(cr & PCI_CR_MAST_EN))
        // pci_attr_w16(devind, PCI_CR, cr | PCI_CR_MAST_EN);

    /* Optionally map flash memory. */
    // if (did != E1000_DEV_ID_82540EM &&
    // did != E1000_DEV_ID_82545EM &&
    // did != E1000_DEV_ID_82540EP &&
    // pci_attr_r32(devind, PCI_BAR_2))
    // {
        // size_t flash_size;

        /* 82566/82567/82562V series support mapping 4kB of flash memory */
        // switch(did)
        // {
            // case E1000_DEV_ID_ICH10_D_BM_LM:
            // case E1000_DEV_ID_ICH10_R_BM_LF:
                // flash_size = 0x1000;
                // break;
            // default:
                // flash_size = 0x10000;
        // }

        // if ((e->flash = vm_map_phys(SELF,
                                    // (void *) pci_attr_r32(devind, PCI_BAR_2),
                                    // flash_size)) == MAP_FAILED) {
            // panic("e1000: couldn't map in flash.");
        // }

    // gfpreg = E1000_READ_FLASH_REG(e, ICH_FLASH_GFPREG);
        /*
         * sector_base_addr is a "sector"-aligned address (4096 bytes)
         */
        // sector_base_addr = gfpreg & FLASH_GFPREG_BASE_MASK;

        /* flash_base_addr is byte-aligned */
        // e->flash_base_addr = sector_base_addr << FLASH_SECTOR_ADDR_SHIFT;
    // }
    /*
     * Output debug information.
     */
    // status[0] = e1000_reg_read(e, E1000_REG_STATUS);
    // E1000_DEBUG(3, ("%s: MEM at %p, IRQ %d\n",
            // e->name, e->regs, e->irq));
    // E1000_DEBUG(3, ("%s: link %s, %s duplex\n",
            // e->name, status[0] & 3 ? "up"   : "down",
                 // status[0] & 1 ? "full" : "half"));
    return TRUE;
}



static void sky_init(message *mp)
{
    static int first_time = 1;
    message reply_mess;
    sky_t *s;

    printf("Sky init\n");

    /* Configure PCI devices, if needed. */
    if (first_time)
    {
        first_time = 0;
        sky_init_pci();
    }
    s = &sky_state;

    /* Initialize hardware, if needed. */
    // if (!(s->status & E1000_ENABLED) && !(e1000_init_hw(e)))
    // {
        // reply_mess.m_type  = DL_CONF_REPLY;
        // reply_mess.DL_STAT = ENXIO;
        // mess_reply(mp, &reply_mess);
        // return;
    // }

    /* Reply back to INET. */
    reply_mess.m_type  = DL_CONF_REPLY;
    reply_mess.DL_STAT = OK;
    *(ether_addr_t *) reply_mess.DL_HWADDR = s->address;
    mess_reply(mp, &reply_mess);
}


static void mess_reply(req, reply_mess)
message *req;
message *reply_mess;
{
    if (send(req->m_source, reply_mess) != OK)
        panic("unable to mess_reply");
}

static void sky_readv_s(const message *mp, int from_int) {
    /* TODO */
}
static void sky_writev_s(const message *mp, int from_int) {
    /* TODO */
}

static void sky_dump(message *m)           /* pointer to request message */
{
    /* TODO */
}

static void check_int_events(void) {
    /* TODO */
}

static void do_hard_int(void) {
    /* TODO */
}

static void sky_watchdog_f(timer_t *tp) {
    /* TODO */
}


/*
 * Main file for driver
 *
 */
int main(int argc, char ** argv) {
    int r;
    int ipc_status;

    /* SEF local startup. */
    env_setargs(argc, argv);
    sef_local_startup();

    while (TRUE)
    {
        if ((r= netdriver_receive(ANY, &m, &ipc_status)) != OK)
            panic("netdriver_receive failed: %d", r);

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(m.m_source)) {
                case CLOCK:
                    /*
                     * Under MINIX, synchronous alarms are
                     * used instead of watchdog functions.
                     * The approach is very different: MINIX
                     * VMD timeouts are handled within the
                     * kernel (the watchdog is executed by
                     * CLOCK), and notify() the driver in
                     * some cases.  MINIX timeouts result in
                     * a SYN_ALARM message to the driver and
                     * thus are handled where they should be
                     * handled. Locally, watchdog functions
                     * are used again.
                     */
                    sky_watchdog_f(NULL);
                    break;
                case HARDWARE:
                    do_hard_int();
                    if (int_event_check)
                        check_int_events();
                    break ;
                case TTY_PROC_NR:
                    sky_dump(&m);
                    break;
                default:
                    panic("illegal notify from: %d",
                    m.m_source);
            }

            /* done, get nwe message */
            continue;
        }

        switch (m.m_type)
        {
        case DL_WRITEV_S: sky_writev_s(&m, FALSE);   break;
        case DL_READV_S: sky_readv_s(&m, FALSE);     break;
        case DL_CONF:   sky_init(&m);            break;
        case DL_GETSTAT_S: sky_getstat_s(&m);        break;
        default:
            panic("illegal message: %d", m.m_type);
        }
    }
}

