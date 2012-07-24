/*
 * tinySky.c
 *
 * This is a fast ethernet driver for Marwell Yukon E88E8040 card on Minix3 OS
 *
 * Created: Jul-17-2012 by Thong T. Nguyen <xcodevn at gmail dot com>
 *
 * Change logs:
 *
 *
 */

#define SKY_VERBOSE 5       /* many levels */

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
    int r;
    eth_stat_t stats;
    // e1000_t *e = &e1000_state;

    SKY_DEBUG(3, ("sky: getstat_s()\n"));

    stats.ets_recvErr   = 0 // TODO: e1000_reg_read(e, E1000_REG_RXERRC);
    stats.ets_sendErr   = 0;
    stats.ets_OVW       = 0;
    stats.ets_CRCerr    = 0; // TODO: e1000_reg_read(e, E1000_REG_CRCERRS);
    stats.ets_frameAll  = 0;
    stats.ets_missedP   = 0; // TODO: e1000_reg_read(e, E1000_REG_MPC);
    stats.ets_packetR   = 0; // TODO: e1000_reg_read(e, E1000_REG_TPR);
    stats.ets_packetT   = 0; // TODO: e1000_reg_read(e, E1000_REG_TPT);
    stats.ets_collision = 0; // TOOD: e1000_reg_read(e, E1000_REG_COLC);
    stats.ets_transAb   = 0;
    stats.ets_carrSense = 0;
    stats.ets_fifoUnder = 0;
    stats.ets_fifoOver  = 0;
    stats.ets_CDheartbeat = 0;
    stats.ets_OWC = 0;

    sys_safecopyto(mp->m_source, mp->DL_GRANT, 0, (vir_bytes)&stats,
                   sizeof(stats));
    mp->m_type  = DL_STAT_REPLY;
    if((r=send(mp->m_source, mp)) != OK)
	panic("sky_getstat: send() failed: %d", r);
}


static void sky_init_pci()
{
    sky_t *s;

    /* Initialize the PCI bus. */
    pci_init();

    /* Try to detect Marwell's. */
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

    SKY_DEBUG(3, ("%s: hello world probe()\n", s->name));

    /*
     * Attempt to iterate the PCI bus. Start at the beginning.
     */
    if ((r = pci_first_dev(&devind, &vid, &did)) == 0)
    {
		panic("We don't have Mavell cards");
		return FALSE;
    }
    /* Loop devices on the PCI bus. */
    while (skip--)
    {
    SKY_DEBUG(3, ("%s: probe() devind %d vid 0x%x did 0x%x\n",
                s->name, devind, vid, did));

    if (!(r = pci_next_dev(&devind, &vid, &did)))
    {
		panic("We don't have Mavell cards");
        return FALSE;
    }
    }
    /*
     * Successfully detected card on the PCI bus.
     */
    s->status |= SKY_DETECTED;

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
    cr = pci_attr_r16(devind, PCI_CR);
    if (!(cr & PCI_CR_MAST_EN))
        pci_attr_w16(devind, PCI_CR, cr | PCI_CR_MAST_EN);

   // status[0] = e1000_reg_read(e, E1000_REG_STATUS);
    SKY_DEBUG(3, ("%s: MEM at %p, IRQ %d\n",
            s->name, s->regs, s->irq));
    // E1000_DEBUG(3, ("%s: link %s, %s duplex\n",
            // e->name, status[0] & 3 ? "up"   : "down",
                 // status[0] & 1 ? "full" : "half"));
    return TRUE;
}


void sky_reset_hw(sky_t *s) {
    /* TODO */
}

static int sky_init_hw(s)
sky_t *s;
{
    int r, i;

    s->status  |= SKY_ENABLED;
    s->irq_hook = s->irq;

    /*
     * Set the interrupt handler and policy. Do not automatically
     * re-enable interrupts. Return the IRQ line number on interrupts.
     */
    if ((r = sys_irqsetpolicy(s->irq, 0, &s->irq_hook)) != OK)
    {
        panic("sys_irqsetpolicy failed: %d", r);
    }
    if ((r = sys_irqenable(&s->irq_hook)) != OK)
    {
    panic("sys_irqenable failed: %d", r);
    }
    /* Reset hardware. */
    sky_reset_hw(s);

    /*
     * Aquire MAC address and setup RX/TX buffers.
     */
    //e1000_init_addr(e);
    //e1000_init_buf(e);

    /* Enable interrupts. */
    // e1000_reg_set(e,   E1000_REG_IMS, E1000_REG_IMS_LSC  |
                      // E1000_REG_IMS_RXO  |
                      // E1000_REG_IMS_RXT  |
                      // E1000_REG_IMS_TXQE |
                      // E1000_REG_IMS_TXDW);
    return TRUE;
}



static void sky_init(message *mp)
{
    static int first_time = 1;
    message reply_mess;
    sky_t *s;

    SKY_DEBUG(3, ("sky: init()\n"));

    /* Configure PCI devices, if needed. */
    if (first_time)
    {
        first_time = 0;
        sky_init_pci();
    }
    s = &sky_state;

    /* Initialize hardware, if needed. */
    if (!(s->status & SKY_ENABLED) && !(sky_init_hw(s)))
    {
        reply_mess.m_type  = DL_CONF_REPLY;
        reply_mess.DL_STAT = ENXIO;
        mess_reply(mp, &reply_mess);
        return;
    }

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

