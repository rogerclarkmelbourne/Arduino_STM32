//-----------------------------------------------------------------------------
//  @file dcmi.c
//  @brief Direct camera memory interface support.
//-----------------------------------------------------------------------------

#include "dcmi.h"
#include "gpio.h"


#ifdef DCMI_D0 // pins not yet defined for disco F407


voidFuncPtr dcmi_irq_handler = NULL;

// DCMI device
const dcmi_dev dcmi =
{
    .regs      = DCMI_BASE,
    .clk_id    = RCC_DCMI,
    .handler_p = &dcmi_irq_handler,
};

//
// DCMI routines
//
void dcmi_init(void)
{
    rcc_clk_enable(RCC_DCMI);
    rcc_reset_dev(RCC_DCMI);
}

//-----------------------------------------------------------------------------
// Configure DCMI control and data lines
// !!! Avoid overlapping with FSMC lines !!!
//-----------------------------------------------------------------------------
const uint8_t dcmi_pins[] =
{
	DCMI_PCLK, DCMI_HSYNC, DCMI_VSYNC,
	DCMI_D0, DCMI_D1, DCMI_D2, DCMI_D3, DCMI_D4, DCMI_D5, DCMI_D6, DCMI_D7,
//DCMI_D8,  DCMI_D9,  DCMI_D10, DCMI_D11, DCMI_D12, DCMI_D13
};

void dcmi_init_gpios(void)
{
	for (uint8_t i=0; i<sizeof(dcmi_pins); i++)
	{
		uint8_t pin = dcmi_pins[i];
		gpio_set_mode(pin, GPIO_AF_INPUT);
		gpio_set_af_mode(pin, GPIO_AFMODE_DCMI);
	}
}

void dcmi_begin(void)
{
	dcmi_init_gpios();
	dcmi_init();
}

void dcmi_config(uint32_t flags)
{
	uint32_t cr = DCMI->regs->CR & 0xFFFFB000; // reserved bits mask
	DCMI->regs->CR = cr | flags;
}

void dcmi_set_data_width(uint16 data_width)
{
	uint32 cr = DCMI->regs->CR & (~DCMI_CR_EDM);
    DCMI->regs->CR = cr | data_width;
}

void dcmi_set_vspol(uint8 vspol)
{
	uint32 cr = DCMI->regs->CR & (~DCMI_CR_VSPOL);
    DCMI->regs->CR = cr | vspol;
}

void dcmi_set_hspol(uint8 hspol)
{
	uint32 cr = DCMI->regs->CR & (~DCMI_CR_HSPOL);
    DCMI->regs->CR = cr | hspol;
}

void dcmi_set_pckpol(uint8 pckpol)
{
	uint32 cr = DCMI->regs->CR & (~DCMI_CR_PCKPOL);
    DCMI->regs->CR = cr | pckpol;
}

void dcmi_set_mode(uint8 capture_mode)
{
	uint32 cr = DCMI->regs->CR & (~DCMI_CR_CM);
    DCMI->regs->CR = cr | capture_mode;
}

void dcmi_set_sync_mode(uint8 ess)
{
	uint32 cr = DCMI->regs->CR & (~DCMI_CR_ESS);
    DCMI->regs->CR = cr | ess;
}

void dcmi_attach_interrupt(voidFuncPtr handler, uint8_t irq_mask)
{
	DCMI->regs->IER |= irq_mask;
    dcmi_irq_handler = handler;
    nvic_irq_enable(NVIC_DCMI);
}

void dcmi_detach_interrupt(uint8_t irq_mask)
{
	DCMI->regs->IER &= (~irq_mask);
	if (DCMI->regs->IER==0)
	{
		nvic_irq_disable(NVIC_DCMI);
		dcmi_irq_handler = NULL;
	}
}

//-----------------------------------------------------------------------------
// Configure DMA2, channel 1, stream 1 (alternative: stream 7)
//-----------------------------------------------------------------------------
void dcmi_dma_config(uint32_t * buf, uint16 bufLen, voidFuncPtr func)
{
	dma_init(DMA2);
	dma_setup_transfer(DMA2, DMA_STREAM1, DMA_CH1, DMA_SIZE_32BITS,
                        &(DCMI->regs->DR), buf, NULL,
	(DMA_PRIO_VERY_HIGH | DMA_MINC_MODE | DMA_CIRC_MODE | DMA_TRNS_HALF | DMA_TRNS_CMPLT));

    dma_set_num_transfers(DMA2, DMA_STREAM1, bufLen);
	dma_set_fifo_flags(DMA2, DMA_STREAM1, (DMA_FCR_DMDIS|DMA_FCR_FTH_2_4));
	//dma_set_fifo_flags(DMA2, DMA_STREAM1, 0);
	dma_clear_isr_bits(DMA2, DMA_STREAM1);
	//if there is an int handler to be attached
	if (func != NULL)
		dma_attach_interrupt(DMA2, DMA_STREAM1, func);

	dma_enable(DMA2, DMA_STREAM1);
}
//-----------------------------------------------------------------------------
void dcmi_dma_reset(uint16 bufLen)
{
	dcmi_disable_capture(); // disable capture with DMA
	dma_disable(DMA2, DMA_STREAM1);
    dma_set_num_transfers(DMA2, DMA_STREAM1, bufLen);
	//dma_set_fifo_flags(DMA2, DMA_STREAM1, (DMA_FCR_DMDIS|DMA_FCR_FTH_1_4));
	dma_clear_isr_bits(DMA2, DMA_STREAM1);
	dma_enable(DMA2, DMA_STREAM1);
	dcmi_enable_capture(); // enable capture with DMA
}
//-----------------------------------------------------------------------------
void __irq_dcmi(void)
{
	if (dcmi_irq_handler)
		dcmi_irq_handler();
	dcmi_clear_irq_flags();
}

#endif  // VARIANT_generic_f407v
