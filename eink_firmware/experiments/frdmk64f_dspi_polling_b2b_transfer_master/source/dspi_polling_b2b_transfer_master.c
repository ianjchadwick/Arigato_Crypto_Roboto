/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_DSPI_MASTER_BASEADDR         SPI0
#define DSPI_MASTER_CLK_SRC                  DSPI0_CLK_SRC
#define DSPI_MASTER_CLK_FREQ                 CLOCK_GetFreq(DSPI0_CLK_SRC)
#define EXAMPLE_DSPI_MASTER_PCS_FOR_INIT     kDSPI_Pcs0
#define EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER kDSPI_MasterPcs0
#define EXAMPLE_DSPI_DEALY_COUNT             0xfffffU

#define TRANSFER_SIZE     256 //64U     /*! Transfer dataSize */
#define TRANSFER_BAUDRATE 500000U /*! Transfer baudrate - 500k */

/*! @brief High/Low definition */
typedef enum _gpio_pin_hi_low
{
    kGPIO_Low  = 0U,
    kGPIO_Hi = 1U,
} gpio_pin_hi_low_t;

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define EINK_2IN9_V2_WIDTH  128
#define EINK_2IN9_V2_HEIGHT 296

#define IMAGE_SIZE 2048

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t masterRxData[TRANSFER_SIZE] = {0U};
uint8_t masterTxData[TRANSFER_SIZE] = {0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X1F,0XE0,0X00,0X00,0X00,0X0F,0XF0,0X00,
		0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X7F,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X1F,0XE0,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,
		0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XC0,0X1F,0XE0,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X03,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XC0,0X1F,0XE0,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,
		0X1F,0XE0,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFE,0X00,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X1F,0XE0,
		0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,
		0X00,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X1F,0XE0,0X00,0X00,
		0X00,0X0F,0XF0,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,
		0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X1F,0XE0,0X00,0X00,0X00,0X0F,
		0XF0,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,
		0X7F,0X80,0X00,0X00,0X00,0X00,0X3F,0XC0,0X1F,0XFF,0XFF,0XF0,0X00,0X0F,0XF0,0X00};

UBYTE gImage_test_qrcode[IMAGE_SIZE] = { /* 0X00,0X01,0X80,0X00,0X80,0X00, */
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X00,0X3E,0X00,0X00,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X00,0X3E,0X00,0X00,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X00,0X3E,0X00,0X00,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X00,0X3E,0X00,0X00,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X7C,0X00,0X00,0X1F,0X0F,0XFF,0X80,0X3E,0X01,0XF0,0XF8,0X00,0X00,0X3E,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X0F,0XFF,0X80,0X3E,0X01,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X0F,0XFF,0X80,0X3E,0X01,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X0F,0XFF,0X80,0X3E,0X01,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X0F,0XFF,0X80,0X3E,0X01,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0XFF,0X83,0XFE,0X00,0X00,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0XFF,0X83,0XFE,0X00,0X00,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0XFF,0X83,0XFE,0X00,0X00,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0XFF,0X83,0XFE,0X00,0X00,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X00,0X07,0X83,0XFE,0X1F,0XF0,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X00,0X07,0X83,0XFE,0X1F,0XF0,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X00,0X07,0X83,0XFE,0X1F,0XF0,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X00,0X07,0X83,0XFE,0X1F,0XF0,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0X07,0X80,0X01,0XE1,0XF0,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0X87,0X80,0X01,0XE1,0XF0,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0X87,0X80,0X01,0XE1,0XF0,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0X87,0X80,0X01,0XE1,0XF0,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0X87,0X80,0X01,0XE1,0XF0,0XF8,0X7F,0XFC,0X1E,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X00,0X78,0X03,0XC1,0XE1,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X00,0X78,0X03,0XC1,0XE1,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X00,0X78,0X03,0XC1,0XE1,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X00,0X78,0X03,0XC1,0XE1,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X87,0X83,0XC1,0XE1,0XF0,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X87,0X83,0XC1,0XE1,0XF0,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X87,0X83,0XC1,0XE1,0XF0,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X87,0X83,0XC1,0XE1,0XF0,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X87,0X83,0XC1,0XE1,0XF0,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X78,0X7C,0X3E,0X01,0XF0,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X78,0X7C,0X3E,0X01,0XF0,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X78,0X7C,0X3E,0X01,0XF0,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X78,0X7C,0X3E,0X01,0XF0,0X00,0X00,0X00,0X00,0X00,
		0X00,0X78,0X03,0XFF,0XFF,0XFF,0X80,0X03,0XC1,0XE1,0XFF,0X00,0X7C,0X3F,0XFE,0X00,
		0X00,0X78,0X03,0XFF,0XFF,0XFF,0X80,0X03,0XC1,0XE1,0XFF,0X00,0X7C,0X3F,0XFE,0X00,
		0X00,0X78,0X03,0XFF,0XFF,0XFF,0X80,0X03,0XC1,0XE1,0XFF,0X00,0X7C,0X3F,0XFE,0X00,
		0X00,0X78,0X03,0XFF,0XFF,0XFF,0X80,0X03,0XC1,0XE1,0XFF,0X00,0X7C,0X3F,0XFE,0X00,
		0X00,0X78,0X03,0XFF,0XFF,0XFF,0X80,0X03,0XC1,0XE1,0XFF,0X00,0X7C,0X3F,0XFE,0X00,
		0X00,0X78,0X03,0XC1,0XE0,0XFF,0X80,0X7F,0XC1,0XFE,0X0F,0X07,0XFF,0XFF,0XE0,0X00,
		0X00,0X78,0X03,0XC1,0XE0,0XFF,0X80,0X7F,0XC1,0XFE,0X0F,0X07,0XFF,0XFF,0XE0,0X00,
		0X00,0X78,0X03,0XC1,0XE0,0XFF,0X80,0X7F,0XC1,0XFE,0X0F,0X07,0XFF,0XFF,0XE0,0X00,
		0X00,0X78,0X03,0XC1,0XE0,0XFF,0X80,0X7F,0XC1,0XFE,0X0F,0X07,0XFF,0XFF,0XE0,0X00,
		0X00,0X78,0X00,0X3E,0X1F,0X0F,0XF8,0X03,0XFF,0XFE,0X00,0XFF,0X83,0XC0,0X1E,0X00,
		0X00,0X78,0X00,0X3E,0X1F,0X0F,0XF8,0X03,0XFF,0XFE,0X00,0XFF,0X83,0XC0,0X1E,0X00,
		0X00,0X78,0X00,0X3E,0X1F,0X0F,0XF8,0X03,0XFF,0XFE,0X00,0XFF,0X83,0XC0,0X1E,0X00,
		0X00,0X78,0X00,0X3E,0X1F,0X0F,0XF8,0X03,0XFF,0XFE,0X00,0XFF,0X83,0XC0,0X1E,0X00,
		0X00,0X78,0X03,0XC0,0X00,0XF0,0X78,0X7C,0X01,0XE0,0X00,0X07,0X83,0XFF,0XFE,0X00,
		0X00,0X78,0X03,0XC0,0X00,0XF0,0X78,0X7C,0X01,0XE0,0X00,0X07,0X83,0XFF,0XFE,0X00,
		0X00,0X78,0X03,0XC0,0X00,0XF0,0X78,0X7C,0X01,0XE0,0X00,0X07,0X83,0XFF,0XFE,0X00,
		0X00,0X78,0X03,0XC0,0X00,0XF0,0X78,0X7C,0X01,0XE0,0X00,0X07,0X83,0XFF,0XFE,0X00,
		0X00,0X78,0X03,0XC0,0X00,0XF0,0X78,0X7C,0X01,0XE0,0X00,0X07,0X83,0XFF,0XFE,0X00,
		0X00,0X07,0XC0,0X01,0XFF,0X00,0X07,0XFF,0XC0,0X01,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X07,0XC0,0X01,0XFF,0X00,0X07,0XFF,0XC0,0X01,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X07,0XC0,0X01,0XFF,0X00,0X07,0XFF,0XC0,0X01,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X07,0XC0,0X01,0XFF,0X00,0X07,0XFF,0XC0,0X01,0XF0,0XF8,0X00,0X00,0X1E,0X00,
		0X00,0X78,0X3C,0X3E,0X00,0XFF,0XF8,0X03,0XC1,0XFF,0XFF,0X00,0X7C,0X03,0XE0,0X00,
		0X00,0X78,0X3C,0X3E,0X00,0XFF,0XF8,0X03,0XC1,0XFF,0XFF,0X00,0X7C,0X03,0XE0,0X00,
		0X00,0X78,0X3C,0X3E,0X00,0XFF,0XF8,0X03,0XC1,0XFF,0XFF,0X00,0X7C,0X03,0XE0,0X00,
		0X00,0X78,0X3C,0X3E,0X00,0XFF,0XF8,0X03,0XC1,0XFF,0XFF,0X00,0X7C,0X03,0XE0,0X00,
		0X00,0X78,0X3C,0X3E,0X00,0XFF,0XF8,0X03,0XC1,0XFF,0XFF,0X00,0X7C,0X03,0XE0,0X00,
		0X00,0X7F,0XC0,0X3F,0XFF,0X0F,0XF8,0X00,0X3E,0X1E,0X0F,0XF8,0X7F,0XFF,0XFE,0X00,
		0X00,0X7F,0XC0,0X3F,0XFF,0X0F,0XF8,0X00,0X3E,0X1E,0X0F,0XF8,0X7F,0XFF,0XFE,0X00,
		0X00,0X7F,0XC0,0X3F,0XFF,0X0F,0XF8,0X00,0X3E,0X1E,0X0F,0XF8,0X7F,0XFF,0XFE,0X00,
		0X00,0X7F,0XC0,0X3F,0XFF,0X0F,0XF8,0X00,0X3E,0X1E,0X0F,0XF8,0X7F,0XFF,0XFE,0X00,
		0X00,0X78,0X3C,0X01,0XE0,0X00,0X7F,0XFC,0X01,0XE0,0X0F,0X07,0X83,0XFC,0X1E,0X00,
		0X00,0X78,0X3C,0X01,0XE0,0X00,0X7F,0XFC,0X01,0XE0,0X0F,0X07,0X83,0XFC,0X1E,0X00,
		0X00,0X78,0X3C,0X01,0XE0,0X00,0X7F,0XFC,0X01,0XE0,0X0F,0X07,0X83,0XFC,0X1E,0X00,
		0X00,0X78,0X3C,0X01,0XE0,0X00,0X7F,0XFC,0X01,0XE0,0X0F,0X07,0X83,0XFC,0X1E,0X00,
		0X00,0X78,0X03,0XC1,0XFF,0XF0,0X7F,0X83,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XE0,0X00,
		0X00,0X78,0X03,0XC1,0XFF,0XF0,0X7F,0X83,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XE0,0X00,
		0X00,0X78,0X03,0XC1,0XFF,0XF0,0X7F,0X83,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XE0,0X00,
		0X00,0X78,0X03,0XC1,0XFF,0XF0,0X7F,0X83,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XE0,0X00,
		0X00,0X78,0X03,0XC1,0XFF,0XF0,0X7F,0X83,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XE0,0X00,
		0X00,0X00,0X00,0X00,0X00,0X0F,0X87,0X83,0XFE,0X01,0XF0,0X00,0X7C,0X3F,0XE0,0X00,
		0X00,0X00,0X00,0X00,0X00,0X0F,0X87,0X83,0XFE,0X01,0XF0,0X00,0X7C,0X3F,0XE0,0X00,
		0X00,0X00,0X00,0X00,0X00,0X0F,0X87,0X83,0XFE,0X01,0XF0,0X00,0X7C,0X3F,0XE0,0X00,
		0X00,0X00,0X00,0X00,0X00,0X0F,0X87,0X83,0XFE,0X01,0XF0,0X00,0X7C,0X3F,0XE0,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X7C,0X00,0X01,0XF0,0XF8,0X7C,0X00,0X1E,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X7C,0X00,0X01,0XF0,0XF8,0X7C,0X00,0X1E,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X7C,0X00,0X01,0XF0,0XF8,0X7C,0X00,0X1E,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X7C,0X00,0X01,0XF0,0XF8,0X7C,0X00,0X1E,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X7C,0X00,0X01,0XF0,0XF8,0X7C,0X00,0X1E,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X0F,0XF8,0X00,0X3F,0XFF,0XF0,0X00,0X7C,0X00,0X00,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X0F,0XF8,0X00,0X3F,0XFF,0XF0,0X00,0X7C,0X00,0X00,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X0F,0XF8,0X00,0X3F,0XFF,0XF0,0X00,0X7C,0X00,0X00,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X0F,0XF8,0X00,0X3F,0XFF,0XF0,0X00,0X7C,0X00,0X00,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0XFF,0XFF,0XC1,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0XFF,0XFF,0XC1,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0XFF,0XFF,0XC1,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0XFF,0XFF,0XC1,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0XFF,0XFF,0XC1,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0X87,0XFC,0X01,0XE1,0XF0,0XF8,0X00,0X03,0XFE,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0X87,0XFC,0X01,0XE1,0XF0,0XF8,0X00,0X03,0XFE,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0X87,0XFC,0X01,0XE1,0XF0,0XF8,0X00,0X03,0XFE,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X0F,0X87,0XFC,0X01,0XE1,0XF0,0XF8,0X00,0X03,0XFE,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X00,0X07,0XFC,0X3F,0XFF,0XFF,0X00,0X7F,0XFF,0XFE,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X00,0X07,0XFC,0X3F,0XFF,0XFF,0X00,0X7F,0XFF,0XFE,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X00,0X07,0XFC,0X3F,0XFF,0XFF,0X00,0X7F,0XFF,0XFE,0X00,
		0X00,0X78,0X3F,0XFE,0X1F,0X00,0X07,0XFC,0X3F,0XFF,0XFF,0X00,0X7F,0XFF,0XFE,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X00,0X00,0X00,0X01,0XE0,0X00,0X07,0XFC,0X3F,0XFE,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X00,0X00,0X00,0X01,0XE0,0X00,0X07,0XFC,0X3F,0XFE,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X00,0X00,0X00,0X01,0XE0,0X00,0X07,0XFC,0X3F,0XFE,0X00,
		0X00,0X78,0X00,0X00,0X1F,0X00,0X00,0X00,0X01,0XE0,0X00,0X07,0XFC,0X3F,0XFE,0X00,
		0X00,0X7C,0X00,0X00,0X1F,0X00,0X00,0X00,0X01,0XE0,0X00,0X07,0XFC,0X3F,0XFE,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X03,0XC0,0X01,0XFF,0X00,0X03,0XC0,0X1E,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X03,0XC0,0X01,0XFF,0X00,0X03,0XC0,0X1E,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X03,0XC0,0X01,0XFF,0X00,0X03,0XC0,0X1E,0X00,
		0X00,0X7F,0XFF,0XFF,0XFF,0X0F,0X80,0X03,0XC0,0X01,0XFF,0X00,0X03,0XC0,0X1E,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		};

UBYTE gImage_pika_bytemap[1742] = { /* 0X00,0X01,0X64,0X00,0X86,0X00, */
		0X00,0X00,0X00,0X00,0X00,0X00,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X34,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X66,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,0XCC,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X03,0XF0,0X07,0XF9,0XF0,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,0X00,
		0X1C,0X0C,0X1C,0XED,0XFD,0X8C,0X87,0XC0,0X00,0X00,0X00,0X00,0X00,0X70,0X06,0X38,
		0XC7,0X83,0X80,0X86,0X7C,0X00,0X00,0X00,0X00,0X00,0XC0,0X03,0X3C,0X82,0X01,0X80,
		0X87,0X1F,0XC0,0X00,0X00,0X00,0X01,0XC0,0X01,0X1C,0X06,0X53,0X80,0XCF,0X18,0X40,
		0X00,0X00,0X00,0X00,0XE0,0X71,0X3C,0X0C,0XA7,0X80,0X63,0X08,0XC0,0X00,0X00,0X00,
		0X00,0X78,0X23,0X8C,0X14,0X4B,0X00,0XD1,0X08,0X80,0X00,0X00,0X00,0X00,0X7C,0X33,
		0X42,0X1C,0X01,0X80,0XB0,0X88,0X80,0X00,0X00,0X00,0X00,0X3C,0X36,0X42,0X02,0X01,
		0X90,0XBC,0X89,0X80,0X00,0X00,0X00,0X00,0X0C,0X0E,0X7A,0X03,0X07,0X11,0X18,0X81,
		0X00,0X00,0X00,0X00,0X00,0X0E,0X1C,0X32,0X60,0XFF,0X15,0X81,0X01,0X00,0X00,0X00,
		0X00,0X00,0X06,0X1C,0X02,0X78,0X33,0X1E,0X81,0X02,0X00,0X00,0X00,0X00,0X00,0X07,
		0X1E,0X06,0X5E,0X27,0XFE,0XC6,0X02,0X00,0X00,0X00,0X00,0X00,0X03,0X0F,0X0E,0X47,
		0XA0,0X7E,0X3E,0X42,0X00,0X00,0X00,0X00,0X00,0X03,0X8F,0XFF,0XC1,0XE0,0X0F,0XE7,
		0X46,0X00,0X00,0X00,0X00,0X00,0X01,0X89,0XE7,0X80,0X60,0X03,0XE7,0XC4,0X00,0X00,
		0X00,0X00,0X00,0X01,0XCC,0X00,0X00,0X00,0X00,0X00,0XE4,0X00,0X00,0X00,0X00,0X00,
		0X01,0XFC,0X00,0X00,0X00,0X00,0X00,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,0X00,
		0X00,0X00,0X00,0X00,0X38,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0XFF,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X87,
		0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XBF,0XFF,0XFF,0X80,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XBF,0XFF,0XFF,0XC0,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X03,0XBF,0XFF,0XFF,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X07,0XBF,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X17,
		0XBF,0XFF,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X2F,0XBF,0XFF,0XFF,
		0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE8,0X3F,0XFF,0XFF,0XFF,0X00,0X20,
		0X00,0X00,0X00,0X00,0X00,0X01,0XE0,0X3F,0XFF,0XFF,0XFF,0X80,0X10,0X00,0X00,0X00,
		0X00,0X00,0X03,0XC0,0X3F,0XFF,0XFF,0XFF,0XC0,0X08,0X00,0X00,0X00,0X00,0X00,0X07,
		0XC0,0X3F,0XFF,0XFF,0XFF,0XE0,0X0C,0X00,0X00,0X00,0X00,0X00,0X07,0XC0,0X3F,0XFF,
		0XFF,0XFF,0X00,0X06,0X00,0X00,0X00,0X00,0X00,0X0F,0XC0,0X3F,0XFF,0XFF,0XF0,0X40,
		0X03,0X00,0X00,0X00,0X00,0X00,0X1F,0XC0,0X3F,0XFF,0XFF,0X87,0XC0,0X01,0X80,0X00,
		0X00,0X00,0X00,0X3F,0X80,0X3F,0XFF,0XFE,0X07,0XC0,0X00,0X80,0X00,0X00,0X00,0X00,
		0X7F,0X80,0X3F,0XFF,0XF8,0X07,0X80,0X01,0XC0,0X00,0X00,0X00,0X00,0X7F,0X80,0X3F,
		0XFF,0XF0,0X07,0X00,0X0F,0XE0,0X00,0X00,0X00,0X00,0XFF,0X80,0X3F,0XFF,0XC0,0X06,
		0X00,0XFF,0XE0,0X00,0X00,0X00,0X00,0XFF,0X80,0X3F,0XFF,0X80,0X04,0X00,0X7F,0XF0,
		0X00,0X00,0X00,0X01,0XFF,0X80,0X7F,0XFF,0X00,0X03,0X80,0X3F,0XF8,0X00,0X00,0X00,
		0X01,0XFF,0X80,0X7F,0XFE,0X00,0X07,0XC0,0X1F,0XF8,0X00,0X00,0X00,0X03,0XFF,0X80,
		0X7F,0XFC,0X00,0X0F,0XF0,0X0F,0XFC,0X00,0X00,0X00,0X03,0XFF,0X80,0X7F,0XF8,0X00,
		0X1F,0XF8,0X07,0XFC,0X00,0X00,0X00,0X07,0XFF,0X80,0X7F,0XF0,0X00,0X7F,0XE0,0X03,
		0XFC,0X00,0X00,0X00,0X07,0XFF,0X80,0XE0,0X00,0X00,0XFF,0X80,0X03,0XFE,0X00,0X00,
		0X00,0X07,0XFF,0X80,0X00,0X00,0X03,0XFE,0X00,0X3F,0XFE,0X00,0X00,0X00,0X0F,0XFF,
		0X80,0X00,0X00,0X0F,0XFF,0X00,0XFF,0XFE,0X00,0X00,0X00,0X0F,0XFF,0X80,0X00,0X00,
		0X3F,0XFF,0XC0,0X7F,0XFF,0X00,0X00,0X00,0X0F,0XFF,0X80,0X00,0X00,0X1F,0XFF,0XE0,
		0X3F,0XFF,0X00,0X00,0X00,0X0F,0XFF,0X80,0X00,0X00,0X0F,0XFF,0XF8,0X1F,0XFF,0X00,
		0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X07,0XFF,0XF8,0X1F,0XFF,0X00,0X00,0X00,0X1F,
		0XFF,0X00,0X00,0X00,0X03,0XFF,0XF0,0X3F,0XFF,0X00,0X00,0X00,0X1F,0XFE,0X00,0X00,
		0X00,0X03,0XC0,0X70,0X7F,0XFF,0X00,0X00,0X00,0X1F,0XFE,0X00,0X00,0X00,0X00,0X00,
		0X00,0XFF,0XFF,0X80,0X00,0X00,0X1F,0XFC,0X00,0X00,0X00,0X10,0X00,0X01,0XFF,0XFF,
		0X80,0X00,0X00,0X1F,0XFC,0X00,0X00,0X00,0X50,0X00,0X01,0XFF,0XFF,0X80,0X00,0X00,
		0X1F,0XF8,0X01,0X08,0X00,0X78,0X00,0X01,0XFF,0XFF,0X80,0X00,0X00,0X1F,0XF8,0X01,
		0X08,0X00,0X78,0X00,0X00,0XFF,0XFF,0X80,0X00,0X00,0X1F,0XF8,0X01,0XF8,0X00,0X70,
		0X00,0X00,0X7F,0XFF,0X80,0X00,0X00,0X1F,0XF8,0X01,0XF8,0X00,0X06,0X00,0X00,0X0F,
		0XFF,0X80,0X00,0X00,0X1F,0XF8,0X00,0XF0,0X00,0X06,0X00,0X00,0X03,0XFF,0X80,0X00,
		0X00,0X1F,0XF8,0X00,0X00,0X00,0X0E,0X00,0X00,0X01,0XFF,0X00,0X00,0X00,0X1F,0XF8,
		0X1E,0X00,0X0E,0X0E,0X00,0X00,0X00,0XFF,0X00,0X00,0X00,0X1F,0XF8,0X3F,0X00,0X7E,
		0X0E,0X00,0X00,0X00,0X7F,0X00,0X00,0X00,0X0F,0XF8,0X3F,0X80,0X7E,0X0E,0X00,0X00,
		0X00,0X7F,0X00,0X00,0X00,0X0F,0XFC,0X3F,0X80,0X2E,0X0C,0X00,0X00,0X00,0X3F,0X00,
		0X00,0X00,0X0F,0XFC,0X3F,0X80,0X38,0X00,0X00,0X00,0X00,0X7F,0X00,0X00,0X00,0X0F,
		0XFE,0X3F,0X80,0X1C,0X00,0X00,0X00,0X00,0XFE,0X00,0X00,0X00,0X07,0XFF,0X3F,0X80,
		0X18,0X00,0X00,0X00,0X3F,0XFE,0X00,0X00,0X00,0X07,0XFF,0X1F,0X00,0X00,0X00,0X00,
		0X00,0X3F,0XFE,0X00,0X00,0X00,0X07,0XFF,0X80,0X00,0X00,0X00,0X00,0X00,0X3F,0XFC,
		0X00,0X00,0X00,0X03,0XFF,0XE0,0X00,0X00,0X00,0X00,0X00,0X7F,0XFC,0X00,0X00,0X00,
		0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X00,0X7F,0XFC,0X00,0X00,0X00,0X01,0XFF,0XFE,
		0X00,0X00,0X00,0X00,0X00,0XFF,0XF8,0X00,0X00,0X00,0X01,0XFF,0XFF,0X00,0X00,0X00,
		0X00,0X01,0XFF,0XF0,0X00,0X00,0X00,0X00,0XFF,0XFF,0X00,0X00,0X00,0X00,0X01,0XFF,
		0XF0,0X00,0X00,0X00,0X00,0XFF,0XFE,0X00,0X00,0X00,0X00,0X01,0XFF,0XE0,0X00,0X00,
		0X00,0X00,0X7F,0XFE,0X00,0X00,0X00,0X00,0X01,0XFF,0XE0,0X00,0X00,0X00,0X00,0X7F,
		0XFE,0X00,0X00,0X00,0X00,0X03,0XFF,0XC0,0X00,0X00,0X00,0X00,0X3F,0XFC,0X00,0X00,
		0X00,0X01,0XFF,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFC,0X00,0X1E,0X00,0X1F,0XFF,
		0XFF,0X80,0X00,0X00,0X00,0X00,0X0F,0XFC,0X00,0X3C,0X00,0XFF,0XFF,0XFF,0X00,0X00,
		0X00,0X00,0X00,0X07,0XF8,0X00,0X3C,0X00,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X00,
		0X07,0XF8,0X00,0X7E,0X01,0XFF,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0X03,0XF8,0X00,
		0X7E,0X03,0XFF,0XFF,0XF8,0X00,0X00,0X00,0X00,0X00,0X01,0XF8,0X00,0XFF,0XFF,0XFF,
		0XFF,0XF0,0X03,0X00,0X00,0X00,0X00,0X00,0XF8,0X00,0XFF,0XFF,0XFF,0XFF,0XE0,0X12,
		0X48,0XB0,0X00,0X00,0X00,0X38,0X01,0XFF,0XFF,0XFF,0XFF,0XC0,0X0A,0X85,0X00,0X00,
		0X00,0X00,0X10,0X03,0XFF,0XFF,0XFF,0XFF,0X80,0X02,0X01,0X00,0X00,0X00,0X00,0X00,
		0X03,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XFF,
		0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XFF,0XFF,0XFF,0XF0,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X07,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,
		0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X02,0X20,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X20,0X01,0X44,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X1F,0X00,0X3F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X28,0X57,0X20,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

uint8_t _WF_PARTIAL_2IN9[159] =
{
	0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0A,0x0,0x0,0x0,0x0,0x0,0x2,
	0x1,0x0,0x0,0x0,0x0,0x0,0x0,
	0x1,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,
	0x22,0x17,0x41,0xB0,0x32,0x36,
};

uint8_t WS_20_30[159] =
{
	0x80,	0x66,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x40,	0x0,	0x0,	0x0,
	0x10,	0x66,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x20,	0x0,	0x0,	0x0,
	0x80,	0x66,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x40,	0x0,	0x0,	0x0,
	0x10,	0x66,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x20,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x14,	0x8,	0x0,	0x0,	0x0,	0x0,	0x1,
	0xA,	0xA,	0x0,	0xA,	0xA,	0x0,	0x1,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x14,	0x8,	0x0,	0x1,	0x0,	0x0,	0x1,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x1,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x44,	0x44,	0x44,	0x44,	0x44,	0x44,	0x0,	0x0,	0x0,
	0x22,	0x17,	0x41,	0x0,	0x32,	0x36
};


volatile uint32_t g_systickCounter  = 20U;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void Delay_Config(void);
void delay_ms(uint32_t delay);
void EINK_test(void);
void EINK_Init(void);
void EINK_Clear(void);
void EINK_Display(UBYTE *Image, UWORD Image_Size);
void EINK_GPIO_PINS(void);
void EINK_ReadBusy(void);
static void EINK_TurnOnDisplay(void);
static void EINK_TurnOnDisplay_Partial(void);
static void EINK_SendCommand(UBYTE byte);
static void EINK_SendData(UBYTE byte);
static void EINK_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);
static void EINK_SetCursor(UWORD Xstart, UWORD Ystart);
static void EINK_LUT(UBYTE *lut);
static void EINK_LUT_by_host(UBYTE *lut);
static void EINK_Reset(void);
/*******************************************************************************
 * Code
 ******************************************************************************/
void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}

int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    EINK_GPIO_PINS();

    PRINTF("DSPI board to board polling example.\r\n");
    PRINTF("This example use one board as master and another as slave.\r\n");
    PRINTF("Master uses polling way and slave uses interrupt way. \r\n");
    PRINTF("Please make sure you make the correct line connection. Basically, the connection is: \r\n");
    PRINTF("DSPI_master -- DSPI_slave   \r\n");
    PRINTF("   CLK      --    CLK  \r\n");
    PRINTF("   PCS      --    PCS \r\n");
    PRINTF("   SOUT     --    SIN  \r\n");
    PRINTF("   SIN      --    SOUT \r\n");
    PRINTF("   GND      --    GND \r\n");

    uint32_t srcClock_Hz;
    uint32_t errorCount;
    uint32_t loopCount = 1U;
    uint32_t i;
    dspi_master_config_t masterConfig;
    dspi_transfer_t masterXfer;

    /* Master config */
    masterConfig.whichCtar                                = kDSPI_Ctar0;
    masterConfig.ctarConfig.baudRate                      = TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.bitsPerFrame                  = 8U;
    masterConfig.ctarConfig.cpol                          = kDSPI_ClockPolarityActiveHigh;
    masterConfig.ctarConfig.cpha                          = kDSPI_ClockPhaseFirstEdge;
    masterConfig.ctarConfig.direction                     = kDSPI_MsbFirst;
    masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = 1000000000U / TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = 1000000000U / TRANSFER_BAUDRATE;
    masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / TRANSFER_BAUDRATE;

    masterConfig.whichPcs           = EXAMPLE_DSPI_MASTER_PCS_FOR_INIT;
    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

    masterConfig.enableContinuousSCK        = false;
    masterConfig.enableRxFifoOverWrite      = false;
    masterConfig.enableModifiedTimingFormat = false;
    masterConfig.samplePoint                = kDSPI_SckToSin0Clock;

    srcClock_Hz = DSPI_MASTER_CLK_FREQ;
    DSPI_MasterInit(EXAMPLE_DSPI_MASTER_BASEADDR, &masterConfig, srcClock_Hz);

    EINK_test();

    while (1)
    {
        /* Set up the transfer data */
        for (i = 0U; i < TRANSFER_SIZE; i++)
        {
            //masterTxData[i] = (i + loopCount) % 256U;
            masterRxData[i] = 0U;
        }

        /* Print out transmit buffer */
        PRINTF("\r\n Master transmit:\r\n");
        for (i = 0U; i < TRANSFER_SIZE; i++)
        {
            /* Print 16 numbers in a line */
            if ((i & 0x0FU) == 0U)
            {
                PRINTF("\r\n");
            }
            PRINTF(" %02X", masterTxData[i]);
        }
        PRINTF("\r\n");

        /* Start master transfer, send data to slave */
        masterXfer.txData      = masterTxData;
        masterXfer.rxData      = NULL;
        masterXfer.dataSize    = TRANSFER_SIZE;
        masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
        DSPI_MasterTransferBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &masterXfer);

        /* Delay to wait slave is ready */
        if (SysTick_Config(SystemCoreClock / 1000U))
        {
            while (1)
            {
            }
        }
        /* Delay 20 ms */
        g_systickCounter = 20U;
        while (g_systickCounter != 0U)
        {
        }

// Comment out Receiving Functionality, only testing the
//        /* Start master transfer, receive data from slave */
//        masterXfer.txData      = NULL;
//        masterXfer.rxData      = masterRxData;
//        masterXfer.dataSize    = TRANSFER_SIZE;
//        masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
//        DSPI_MasterTransferBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &masterXfer);
//
//        errorCount = 0U;
//        for (i = 0U; i < TRANSFER_SIZE; i++)
//        {
//            if (masterTxData[i] != masterRxData[i])
//            {
//                errorCount++;
//            }
//        }
//
//        if (errorCount == 0U)
//        {
//            PRINTF(" \r\nDSPI transfer all data matched! \r\n");
//
//            /* Print out receive buffer */
//            PRINTF("\r\n Master received:\r\n");
//            for (i = 0U; i < TRANSFER_SIZE; i++)
//            {
//                /* Print 16 numbers in a line */
//                if ((i & 0x0FU) == 0U)
//                {
//                    PRINTF("\r\n");
//                }
//                PRINTF(" %02X", masterRxData[i]);
//            }
//            PRINTF("\r\n");
//        }
//        else
//        {
//            PRINTF("\r\nError occurred in DSPI transfer ! \r\n");
//        }

        /* Wait for press any key */
        PRINTF("\r\n Press any key to run again\r\n");
        GETCHAR();

        /* Increase loop count to change transmit buffer */
        loopCount++;
    }
}

void EINK_GPIO_PINS(void)
{
	PRINTF("\r\nTesting EINK_GPIO_PINS\r\n");

	// write Low
	GPIO_PinWrite(EINK_LED_BLUE_GPIO, EINK_LED_BLUE_PIN, kGPIO_Hi);
	GPIO_PinWrite(EINK_LED_BLUE_GPIO, EINK_LED_BLUE_PIN, kGPIO_Low);
	GPIO_PinWrite(EINK_DC_GPIO, EINK_DC_PIN, kGPIO_Low);
	GPIO_PinWrite(EINK_CS_GPIO, EINK_CS_PIN, kGPIO_Low);
	GPIO_PinWrite(EINK_RST_GPIO, EINK_RST_PIN, kGPIO_Low);
//	GPIO_PinWrite(EINK_BUSY_GPIO, EINK_BUSY_PIN, kGPIO_Low);

	// write Hi
	GPIO_PinWrite(EINK_LED_BLUE_GPIO, EINK_LED_BLUE_PIN, kGPIO_Hi);
	GPIO_PinWrite(EINK_DC_GPIO, EINK_DC_PIN, kGPIO_Hi);
	GPIO_PinWrite(EINK_CS_GPIO, EINK_CS_PIN, kGPIO_Hi);
	GPIO_PinWrite(EINK_RST_GPIO, EINK_RST_PIN, kGPIO_Hi);
//	GPIO_PinWrite(EINK_BUSY_GPIO, EINK_BUSY_PIN, kGPIO_Hi);


}

static void EINK_SendCommand(UBYTE byte)
{
	GPIO_PinWrite(EINK_DC_GPIO, EINK_DC_PIN, kGPIO_Low);
	GPIO_PinWrite(EINK_CS_GPIO, EINK_CS_PIN, kGPIO_Low);

	// spi transfer goes
	/* Start master transfer, send data to screen */
	dspi_transfer_t masterXfer;
	masterXfer.txData      = &byte; // transferring the byte passed in
	masterXfer.rxData      = NULL;
	masterXfer.dataSize    = 1U; // only transferring one byte
	masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
	DSPI_MasterTransferBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &masterXfer);

	GPIO_PinWrite(EINK_CS_GPIO, EINK_CS_PIN, kGPIO_Hi);
}

static void EINK_SendData(UBYTE byte)
{
	GPIO_PinWrite(EINK_DC_GPIO, EINK_DC_PIN, kGPIO_Hi);
	GPIO_PinWrite(EINK_CS_GPIO, EINK_CS_PIN, kGPIO_Low);

	// spi transfer goes
	/* Start master transfer, send data to screen */
	dspi_transfer_t masterXfer;
	masterXfer.txData      = &byte; // transferring the byte passed in
	masterXfer.rxData      = NULL;
	masterXfer.dataSize    = 1U; // only transferring one byte
	masterXfer.configFlags = kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous;
	DSPI_MasterTransferBlocking(EXAMPLE_DSPI_MASTER_BASEADDR, &masterXfer);

	GPIO_PinWrite(EINK_CS_GPIO, EINK_CS_PIN, kGPIO_Hi);
}


/******************************************************************************
function :	Setting the display window
parameter:
******************************************************************************/
static void EINK_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    EINK_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    EINK_SendData((Xstart>>3) & 0xFF);
    EINK_SendData((Xend>>3) & 0xFF);
    EINK_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    EINK_SendData(Ystart & 0xFF);
    EINK_SendData((Ystart >> 8) & 0xFF);
    EINK_SendData(Yend & 0xFF);
    EINK_SendData((Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
******************************************************************************/
static void EINK_SetCursor(UWORD Xstart, UWORD Ystart)
{
    EINK_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    EINK_SendData(Xstart & 0xFF);

    EINK_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    EINK_SendData(Ystart & 0xFF);
    EINK_SendData((Ystart >> 8) & 0xFF);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EINK_ReadBusy(void)
{
//    Debug("e-Paper busy\r\n");
	while(1)
	{	 //=1 BUSY
		if(GPIO_PinRead(EINK_BUSY_GPIO, EINK_BUSY_PIN)==0)
			break;
//		DEV_Delay_ms(50);
		delay_ms(50U);
	}
//	DEV_Delay_ms(50);
	delay_ms(50U);
//    Debug("e-Paper busy release\r\n");
}

static void EINK_LUT(UBYTE *lut)
{
	UBYTE count;
	EINK_SendCommand(0x32);
	for(count=0; count<153; count++)
		EINK_SendData(lut[count]);
	EINK_ReadBusy();
}

static void EINK_LUT_by_host(UBYTE *lut)
{
	EINK_LUT((UBYTE *)lut);			//lut
	EINK_SendCommand(0x3f);
	EINK_SendData(*(lut+153));
	EINK_SendCommand(0x03);	// gate voltage
	EINK_SendData(*(lut+154));
	EINK_SendCommand(0x04);	// source voltage
	EINK_SendData(*(lut+155));	// VSH
	EINK_SendData(*(lut+156));	// VSH2
	EINK_SendData(*(lut+157));	// VSL
	EINK_SendCommand(0x2c);		// VCOM
	EINK_SendData(*(lut+158));

}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EINK_Reset(void)
{
	GPIO_PinWrite(EINK_RST_GPIO, EINK_RST_PIN, kGPIO_Hi);

    // DEV_Delay_ms(100);
	delay_ms(100U);

    GPIO_PinWrite(EINK_RST_GPIO, EINK_RST_PIN, kGPIO_Low);

    // DEV_Delay_ms(2);
    delay_ms(2U);

    GPIO_PinWrite(EINK_RST_GPIO, EINK_RST_PIN, kGPIO_Hi);

    // DEV_Delay_ms(100);
    delay_ms(100U);
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EINK_Init(void)
{
	EINK_Reset();

	// here goes a delay
	//DEV_Delay_ms(100);
	delay_ms(100U);

	EINK_ReadBusy();
	EINK_SendCommand(0x12);  //SWRESET
	EINK_ReadBusy();

	EINK_SendCommand(0x01); //Driver output control
	EINK_SendData(0x27);
	EINK_SendData(0x01);
	EINK_SendData(0x00);

	EINK_SendCommand(0x11); //data entry mode
	EINK_SendData(0x03);

	EINK_SetWindows(0, 0, EINK_2IN9_V2_WIDTH-1, EINK_2IN9_V2_HEIGHT-1);

	// EPD_2IN9_V2_SendCommand(0x3C); //BorderWavefrom
	// EPD_2IN9_V2_SendData(0x05);

	EINK_SendCommand(0x21); //  Display update control
	EINK_SendData(0x00);
	EINK_SendData(0x80);

	// EPD_2IN9_V2_SendCommand(0x18); //Read built-in temperature sensor
	// EPD_2IN9_V2_SendData(0x80);

	EINK_SetCursor(0, 0);
	EINK_ReadBusy();

	EINK_LUT_by_host(WS_20_30);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EINK_Clear(void)
{
	UWORD i;
	EINK_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0;i<4736;i++)
	{
		EINK_SendData(0xff);
	}
	EINK_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EINK_Display(UBYTE *Image, UWORD Image_Size)
{
	UWORD i;
	EINK_SendCommand(0x24);   //write RAM for black(0)/white (1)
	for(i=0;i<Image_Size/*4736*/;i++)
	{
		EINK_SendData(Image[i]);
	}
	EINK_TurnOnDisplay();
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EINK_TurnOnDisplay(void)
{
	EINK_SendCommand(0x22); //Display Update Control
	EINK_SendData(0xc7);
	EINK_SendCommand(0x20); //Activate Display Update Sequence
	EINK_ReadBusy();
}

static void EINK_TurnOnDisplay_Partial(void)
{
	EINK_SendCommand(0x22); //Display Update Control
	EINK_SendData(0x0F);
	EINK_SendCommand(0x20); //Activate Display Update Sequence
	EINK_ReadBusy();
}

void Delay_Config(void)
{
	if (SysTick_Config(SystemCoreClock / 1000U))
	{
		while (1)
		{
		}
	}
}

void delay_ms(uint32_t delay)
{
	g_systickCounter = delay;
	while (g_systickCounter != 0U)
	{
	}
}

void EINK_test(void)
{
	PRINTF("e-Paper Init and Clear...\r\n");
	Delay_Config();
	EINK_Init();
	EINK_Clear();

	EINK_Display(gImage_test_qrcode, IMAGE_SIZE);

//	PRINTF("Clear...\r\n");
//	EINK_Init();
//	EINK_Clear();

	PRINTF("Drawing Pikachu\r\n");
	EINK_Init();
	EINK_Clear();

	// this image bytemap is not correct...explore later
	EINK_Display(gImage_pika_bytemap, 1742);

	PRINTF("Clear...\r\n");
	EINK_Clear();
}

