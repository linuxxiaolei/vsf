/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "app_type.h"
#include "vsfhal.h"

// TODO: remove MACROs below to stm32_reg.h
#define STM32_RCC_APB2ENR_IOPAEN		((uint32_t)1 << 2)
#define STM32_RCC_APB2ENR_AFIOEN		((uint32_t)1 << 0)

#define STM32_RCC_CFGR_MCO_SFT			24
#define STM32_RCC_CFGR_MCO_MSK			((uint32_t)0x07 << \
											STM32_RCC_CFGR_MCO_SFT)
#define STM32_RCC_CFGR_MCO_NOCLK		((uint32_t)0x00 << \
											STM32_RCC_CFGR_MCO_SFT)
#define STM32_RCC_CFGR_MCO_SYSCLK		((uint32_t)0x04 << \
											STM32_RCC_CFGR_MCO_SFT)
#define STM32_RCC_CFGR_MCO_HSI			((uint32_t)0x05 << \
											STM32_RCC_CFGR_MCO_SFT)
#define STM32_RCC_CFGR_MCO_HSE			((uint32_t)0x06 << \
											STM32_RCC_CFGR_MCO_SFT)
#define STM32_RCC_CFGR_MCO_PLLHALF		((uint32_t)0x07 << \
											STM32_RCC_CFGR_MCO_SFT)

#if VSFHAL_CLKO_EN

#include "STM32_CLKO.h"

#define STM32_CLKO_NUM					1

vsf_err_t stm32_clko_init(uint8_t index)
{
#if __VSF_DEBUG__
	if (index >= STM32_CLKO_NUM)
	{
		return VSFERR_NOT_SUPPORT;
	}
#endif
	
	switch (index)
	{
	case 0:
		// PA8
		RCC->CFGR &= ~STM32_RCC_CFGR_MCO_MSK;
		RCC->APB2ENR |= STM32_RCC_APB2ENR_IOPAEN | STM32_RCC_APB2ENR_AFIOEN;
		GPIOA->CRH = (GPIOA->CRH & ~(0x0F << ((8 - 8) * 4))) | 
						(uint32_t)stm32_GPIO_AFPP << ((8 - 8) * 4);
		break;
	default:
		return VSFERR_NOT_SUPPORT;
	}
	
	return VSFERR_NONE;
}

vsf_err_t stm32_clko_fini(uint8_t index)
{
	switch (index)
	{
	case 0:
		RCC->CFGR &= ~STM32_RCC_CFGR_MCO_MSK;
		GPIOA->CRH = (GPIOA->CRH & ~(0x0F << ((8 - 8) * 4))) | 
						(uint32_t)stm32_GPIO_INFLOAT << ((8 - 8) * 4);
		break;
	default:
		return VSFERR_NOT_SUPPORT;
	}
	return VSFERR_NONE;
}

vsf_err_t stm32_clko_config(uint8_t index, uint32_t kHz)
{
	uint32_t mco;
	struct stm32_info_t *info;
	
	if (stm32_get_info(&info))
	{
		return VSFERR_FAIL;
	}
	
#if __VSF_DEBUG__
	if (index >= STM32_CLKO_NUM)
	{
		return VSFERR_INVALID_PARAMETER;
	}
#endif
	
	if (0 == kHz)
	{
		mco = STM32_RCC_CFGR_MCO_NOCLK;
	}
	else if (info->pll_freq_hz / 2000 == kHz)
	{
		mco = STM32_RCC_CFGR_MCO_PLLHALF;
	}
	else if (info->osc_freq_hz / 1000 == kHz)
	{
		mco = STM32_RCC_CFGR_MCO_HSE;
	}
	else if (info->sys_freq_hz / 1000 == kHz)
	{
		mco = STM32_RCC_CFGR_MCO_SYSCLK;
	}
	else if (8000 == kHz)
	{
		mco = STM32_RCC_CFGR_MCO_HSI;
	}
	else
	{
		return VSFERR_INVALID_PARAMETER;
	}
	RCC->CFGR = (RCC->CFGR & ~STM32_RCC_CFGR_MCO_MSK) | mco;
	return VSFERR_NONE;
}

vsf_err_t stm32_clko_enable(uint8_t index)
{
#if __VSF_DEBUG__
	if (index >= STM32_CLKO_NUM)
	{
		return VSFERR_NOT_SUPPORT;
	}
#endif
	
	GPIOA->CRH = (GPIOA->CRH & ~(0x0F << ((8 - 8) * 4))) | 
					(uint32_t)stm32_GPIO_AFPP << ((8 - 8) * 4);
	return VSFERR_NONE;
}

vsf_err_t stm32_clko_disable(uint8_t index)
{
#if __VSF_DEBUG__
	if (index >= STM32_CLKO_NUM)
	{
		return VSFERR_NOT_SUPPORT;
	}
#endif
	
	GPIOA->CRH = (GPIOA->CRH & ~(0x0F << ((8 - 8) * 4))) | 
					(uint32_t)stm32_GPIO_INFLOAT << ((8 - 8) * 4);
	return VSFERR_NONE;
}

#endif
