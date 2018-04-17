/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @file libmaple/usb/stm32f1/usb.c
 * @brief USB support.
 *
 * This is a mess.
 */

#include <libmaple/usb.h>

#include <libmaple/libmaple.h>
#include <libmaple/rcc.h>

/* Private headers */
#include "usb_reg_map.h"
#include "usb_lib_globals.h"

/* usb_lib headers */
#include "usb_type.h"
#include "usb_core.h"

static void dispatch_ctr_lp(void);

/*
 * usb_lib/ globals
 */

uint16 SaveTState;              /* caches TX status for later use */
uint16 SaveRState;              /* caches RX status for later use */

/*
 * Other state
 */

typedef enum {
    RESUME_EXTERNAL,
    RESUME_INTERNAL,
    RESUME_LATER,
    RESUME_WAIT,
    RESUME_START,
    RESUME_ON,
    RESUME_OFF,
    RESUME_ESOF
} RESUME_STATE;

struct {
    volatile RESUME_STATE eState;
    volatile uint8 bESOFcnt;
} ResumeS;

static usblib_dev usblib = {
    .irq_mask = USB_ISR_MSK,
    .state = USB_UNCONNECTED,
    .prevState = USB_UNCONNECTED,
    .clk_id = RCC_USB,
};
usblib_dev *USBLIB = &usblib;

/*
 * Routines
 */

void usb_init_usblib(usblib_dev *dev,
                     void (**ep_int_in)(void),
                     void (**ep_int_out)(void)) {
    rcc_clk_enable(dev->clk_id);

    dev->ep_int_in = ep_int_in;
    dev->ep_int_out = ep_int_out;

    /* usb_lib/ declares both and then assumes that pFoo points to Foo
     * (even though the names don't always match), which is stupid for
     * all of the obvious reasons, but whatever.  Here we are. */
    pInformation = &Device_Info;
    pProperty = &Device_Property;
    pUser_Standard_Requests = &User_Standard_Requests;

    pInformation->ControlState = 2; /* FIXME [0.0.12] use
                                       CONTROL_STATE enumerator */
    pProperty->Init();
}

static void usb_suspend(void) {
    uint16 cntr;

    /* TODO decide if read/modify/write is really what we want
     * (e.g. usb_resume_init() reconfigures CNTR). */
    cntr = USB_BASE->CNTR;
    cntr |= USB_CNTR_FSUSP;
    USB_BASE->CNTR = cntr;
    cntr |= USB_CNTR_LP_MODE;
    USB_BASE->CNTR = cntr;

    USBLIB->prevState = USBLIB->state;
    USBLIB->state = USB_SUSPENDED;
}

void usb_power_off(void) {
    USB_BASE->CNTR = USB_CNTR_FRES;
    USB_BASE->ISTR = 0;
    USB_BASE->CNTR = USB_CNTR_FRES + USB_CNTR_PDWN;
}

static void usb_resume_init(void) {
    uint16 cntr;

    cntr = USB_BASE->CNTR;
    cntr &= ~USB_CNTR_LP_MODE;
    USB_BASE->CNTR = cntr;

    /* Enable interrupt lines */
    USB_BASE->CNTR = USB_ISR_MSK;
}

static void usb_resume(RESUME_STATE eResumeSetVal) {
    uint16 cntr;

    if (eResumeSetVal != RESUME_ESOF) {
        ResumeS.eState = eResumeSetVal;
    }

    switch (ResumeS.eState) {
    case RESUME_EXTERNAL:
        usb_resume_init();
        ResumeS.eState = RESUME_OFF;
        USBLIB->state = USBLIB->prevState;
        break;
    case RESUME_INTERNAL:
        usb_resume_init();
        ResumeS.eState = RESUME_START;
        break;
    case RESUME_LATER:
        ResumeS.bESOFcnt = 2;
        ResumeS.eState = RESUME_WAIT;
        break;
    case RESUME_WAIT:
        ResumeS.bESOFcnt--;
        if (ResumeS.bESOFcnt == 0) {
            ResumeS.eState = RESUME_START;
        }
        break;
    case RESUME_START:
        cntr = USB_BASE->CNTR;
        cntr |= USB_CNTR_RESUME;
        USB_BASE->CNTR = cntr;
        ResumeS.eState = RESUME_ON;
        ResumeS.bESOFcnt = 10;
        break;
    case RESUME_ON:
        ResumeS.bESOFcnt--;
        if (ResumeS.bESOFcnt == 0) {
            cntr = USB_BASE->CNTR;
            cntr &= ~USB_CNTR_RESUME;
            USB_BASE->CNTR = cntr;
            USBLIB->state = USBLIB->prevState;
            ResumeS.eState = RESUME_OFF;
        }
        break;
    case RESUME_OFF:
    case RESUME_ESOF:
    default:
        ResumeS.eState = RESUME_OFF;
        break;
    }
}

#define SUSPEND_ENABLED 1
__weak void __irq_usb_lp_can_rx0(void) {
    uint16 istr = USB_BASE->ISTR;

    /* Use USB_ISR_MSK to only include code for bits we care about. */

#if (USB_ISR_MSK & USB_ISTR_RESET)
    if (istr & USB_ISTR_RESET & USBLIB->irq_mask) {
        USB_BASE->ISTR = ~USB_ISTR_RESET;
        pProperty->Reset();
    }
#endif

#if (USB_ISR_MSK & USB_ISTR_PMAOVR)
    if (istr & ISTR_PMAOVR & USBLIB->irq_mask) {
        USB_BASE->ISTR = ~USB_ISTR_PMAOVR;
    }
#endif

#if (USB_ISR_MSK & USB_ISTR_ERR)
    if (istr & USB_ISTR_ERR & USBLIB->irq_mask) {
        USB_BASE->ISTR = ~USB_ISTR_ERR;
    }
#endif

#if (USB_ISR_MSK & USB_ISTR_WKUP)
    if (istr & USB_ISTR_WKUP & USBLIB->irq_mask) {
        USB_BASE->ISTR = ~(USB_ISTR_WKUP | USB_ISTR_SUSP);
        usb_resume(RESUME_EXTERNAL);
    }
#endif

#if (USB_ISR_MSK & USB_ISTR_SUSP)
    if (istr & USB_ISTR_SUSP & USBLIB->irq_mask) {
        /* check if SUSPEND is possible */
        if (SUSPEND_ENABLED) {
            usb_suspend();
        } else {
            /* if not possible then resume after xx ms */
            usb_resume(RESUME_LATER);
        }
        /* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
        USB_BASE->ISTR = ~(USB_ISTR_WKUP | USB_ISTR_SUSP);
    }
#endif

#if (USB_ISR_MSK & USB_ISTR_SOF)
    if (istr & USB_ISTR_SOF & USBLIB->irq_mask) {
        USB_BASE->ISTR = ~USB_ISTR_SOF;
    }
#endif

#if (USB_ISR_MSK & USB_ISTR_ESOF)
    if (istr & USB_ISTR_ESOF & USBLIB->irq_mask) {
        USB_BASE->ISTR = ~USB_ISTR_ESOF;
        /* resume handling timing is made with ESOFs */
        usb_resume(RESUME_ESOF); /* request without change of the machine state */
    }
#endif

    /*
     * Service the correct transfer interrupt.
     */

#if (USB_ISR_MSK & USB_ISTR_CTR)
    if (istr & USB_ISTR_CTR & USBLIB->irq_mask) {
        dispatch_ctr_lp();
    }
#endif
}

/*
 * Auxiliary routines
 */

static inline uint8 dispatch_endpt_zero(uint16 istr_dir);
static inline void dispatch_endpt(uint8 ep);
static inline void set_rx_tx_status0(uint16 rx, uint16 tx);

static void handle_setup0(void);
static void handle_in0(void);
static void handle_out0(void);

static void dispatch_ctr_lp() {
    uint16 istr;

    while (((istr = USB_BASE->ISTR) & USB_ISTR_CTR) != 0) {
        /* TODO WTF, figure this out: RM0008 says CTR is read-only,
         * but ST's firmware claims it's clear-only, and emphasizes
         * the importance of clearing it in more than one place. */
        USB_BASE->ISTR = ~USB_ISTR_CTR;
        uint8 ep_id = istr & USB_ISTR_EP_ID;
        if (ep_id == 0) {
            /* TODO figure out why it's OK to break out of the loop
             * once we're done serving endpoint zero, but not okay if
             * there are multiple nonzero endpoint transfers to
             * handle. */
            if (dispatch_endpt_zero(istr & USB_ISTR_DIR)) {
                return;
            }
        } else {
            dispatch_endpt(ep_id);
        }
    }
}


/* FIXME Dataflow on endpoint 0 RX/TX status is based off of ST's
 * code, and is ugly/confusing in its use of SaveRState/SaveTState.
 * Fixing this requires filling in handle_in0(), handle_setup0(),
 * handle_out0(). */
static inline uint8 dispatch_endpt_zero(uint16 istr_dir) {
    uint32 epr = (uint16)USB_BASE->EP[0];

    if (!(epr & (USB_EP_CTR_TX | USB_EP_SETUP | USB_EP_CTR_RX))) {
        return 0;
    }

    /* Cache RX/TX statuses in SaveRState/SaveTState, respectively.
     * The various handle_foo0() may clobber these values
     * before we reset them at the end of this routine. */
    SaveRState = epr & USB_EP_STAT_RX;
    SaveTState = epr & USB_EP_STAT_TX;

    /* Set actual RX/TX statuses to NAK while we're thinking */
    set_rx_tx_status0(USB_EP_STAT_RX_NAK, USB_EP_STAT_TX_NAK);

    if (istr_dir == 0) {
        /* ST RM0008: "If DIR bit=0, CTR_TX bit is set in the USB_EPnR
         * register related to the interrupting endpoint.  The
         * interrupting transaction is of IN type (data transmitted by
         * the USB peripheral to the host PC)." */
        ASSERT_FAULT(epr & USB_EP_CTR_TX);
        usb_clear_ctr_tx(USB_EP0);
        handle_in0();
    } else {
        /* RM0008: "If DIR bit=1, CTR_RX bit or both CTR_TX/CTR_RX
         * are set in the USB_EPnR register related to the
         * interrupting endpoint. The interrupting transaction is of
         * OUT type (data received by the USB peripheral from the host
         * PC) or two pending transactions are waiting to be
         * processed."
         *
         * [mbolivar] Note how the following control flow (which
         * replicates ST's) doesn't seem to actually handle both
         * interrupts that are ostensibly pending when both CTR_RX and
         * CTR_TX are set.
         *
         * TODO sort this mess out.
         */
        if (epr & USB_EP_CTR_TX) {
            usb_clear_ctr_tx(USB_EP0);
            handle_in0();
        } else {                /* SETUP or CTR_RX */
            /* SETUP is held constant while CTR_RX is set, so clear it
             * either way */
            usb_clear_ctr_rx(USB_EP0);
            if (epr & USB_EP_SETUP) {
                handle_setup0();
            } else {            /* CTR_RX */
                handle_out0();
            }
        }
    }

    set_rx_tx_status0(SaveRState, SaveTState);
    return 1;
}

static inline void dispatch_endpt(uint8 ep) {
    uint32 epr = USB_BASE->EP[ep];
    /* If ISTR_CTR is set and the ISTR gave us this EP_ID to handle,
     * then presumably at least one of CTR_RX and CTR_TX is set, but
     * again, ST's control flow allows for the possibility of neither.
     *
     * TODO try to find out if neither being set is possible. */
    if (epr & USB_EP_CTR_RX) {
        usb_clear_ctr_rx(ep);
        (USBLIB->ep_int_out[ep - 1])();
    }
    if (epr & USB_EP_CTR_TX) {
        usb_clear_ctr_tx(ep);
        (USBLIB->ep_int_in[ep - 1])();
    }
}

static inline void set_rx_tx_status0(uint16 rx, uint16 tx) {
    usb_set_ep_rx_stat(USB_EP0, rx);
    usb_set_ep_tx_stat(USB_EP0, tx);
}

/* TODO Rip out usb_lib/ dependency from the following functions: */

static void handle_setup0(void) {
    Setup0_Process();
}

static void handle_in0(void) {
    In0_Process();
}

static void handle_out0(void) {
    Out0_Process();
}
