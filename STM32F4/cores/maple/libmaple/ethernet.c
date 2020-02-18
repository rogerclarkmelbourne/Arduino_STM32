// ethernet.c


#include "ethernet.h"
#include "gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif // __cplusplus

#ifdef ETH_TX_D0

// RMII interface IO pins
const uint8_t eth_pins[] = {
ETH_REF_CLK, ETH_MDIO, ETH_CRS_DV, ETH_TX_EN, ETH_TX_D0, ETH_TX_D1, ETH_MDC, ETH_RX_D0, ETH_RX_D1
};


// Select MII or RMII Mode/
#define ETH_MEDIA_INTERFACE_MII       0x00000000U
#define ETH_MEDIA_INTERFACE_RMII      ((uint32_t)SYSCFG_PMC_MII_RMII_SEL)

//-----------------------------------------------------------------------------
void ethernet_init(void)
{
	rcc_clk_enable(RCC_ETHMAC);
	rcc_clk_enable(RCC_ETHMACTX);
	rcc_clk_enable(RCC_ETHMACRX);
//	rcc_clk_enable(RCC_ETHMACPTP);

	rcc_reset_dev(RCC_ETHMAC);
}

//-----------------------------------------------------------------------------
void ethernet_deinit(void)
{
	rcc_clk_disable(RCC_ETHMAC);
	rcc_clk_disable(RCC_ETHMACTX);
	rcc_clk_disable(RCC_ETHMACRX);
//	rcc_clk_enable(RCC_ETHMACPTP);
}

//-----------------------------------------------------------------------------
void ethernet_gpio_init(void)
{
  SYSCFG->PMC &= ~(SYSCFG_PMC_MII_RMII_SEL_Msk);
  // enable the following line for RMII
  SYSCFG->PMC |= ETH_MEDIA_INTERFACE_RMII;

	// setup pins
	for (uint8_t i = 0; i<sizeof(eth_pins); i++)
	{
		uint8_t pin = eth_pins[i];
		gpio_set_mode(pin, GPIO_AF_OUTPUT_PP);
		gpio_set_af_mode(pin, GPIO_AFMODE_ETH);
	}
}

#endif

#ifdef __cplusplus
}
#endif // __cplusplus
