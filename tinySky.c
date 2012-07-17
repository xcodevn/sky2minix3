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

#define VERBOSE 0       /* 1 for more debug information */


#include "tinySky.h"

static message m;
static int int_event_check;     /* set to TRUE if events arrived */

static u32_t system_hz;


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
    system_hz = sys_hz();

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

static void sky_init(message *mp)
{
    /* TODO */
}

static void sky_readv_s(const message *mp, int from_int) {
    /* TODO */
}
static void sky_writev_s(const message *mp, int from_int) {
    /* TODO */
}

static void sky_dump(message *m);           /* pointer to request message */
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

void sef_local_startup(void) {
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
                    rtl8139_dump(&m);
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
        case DL_WRITEV_S: rl_writev_s(&m, FALSE);   break;
        case DL_READV_S: rl_readv_s(&m, FALSE);     break;
        case DL_CONF:   rl_init(&m);            break;
        case DL_GETSTAT_S: rl_getstat_s(&m);        break;
        default:
            panic("illegal message: %d", m.m_type);
        }
    }
}

