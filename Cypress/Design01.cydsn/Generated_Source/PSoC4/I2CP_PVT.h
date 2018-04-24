/***************************************************************************//**
* \file .h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_PVT_I2CP_H)
#define CY_SCB_PVT_I2CP_H

#include "I2CP.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define I2CP_SetI2CExtClkInterruptMode(interruptMask) I2CP_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define I2CP_ClearI2CExtClkInterruptSource(interruptMask) I2CP_CLEAR_INTR_I2C_EC(interruptMask)
#define I2CP_GetI2CExtClkInterruptSource()                (I2CP_INTR_I2C_EC_REG)
#define I2CP_GetI2CExtClkInterruptMode()                  (I2CP_INTR_I2C_EC_MASK_REG)
#define I2CP_GetI2CExtClkInterruptSourceMasked()          (I2CP_INTR_I2C_EC_MASKED_REG)

#if (!I2CP_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define I2CP_SetSpiExtClkInterruptMode(interruptMask) \
                                                                I2CP_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define I2CP_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                I2CP_CLEAR_INTR_SPI_EC(interruptMask)
    #define I2CP_GetExtSpiClkInterruptSource()                 (I2CP_INTR_SPI_EC_REG)
    #define I2CP_GetExtSpiClkInterruptMode()                   (I2CP_INTR_SPI_EC_MASK_REG)
    #define I2CP_GetExtSpiClkInterruptSourceMasked()           (I2CP_INTR_SPI_EC_MASKED_REG)
#endif /* (!I2CP_CY_SCBIP_V1) */

#if(I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void I2CP_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (I2CP_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_I2CP_CUSTOM_INTR_HANDLER)
    extern cyisraddress I2CP_customIntrHandler;
#endif /* !defined (CY_REMOVE_I2CP_CUSTOM_INTR_HANDLER) */
#endif /* (I2CP_SCB_IRQ_INTERNAL) */

extern I2CP_BACKUP_STRUCT I2CP_backup;

#if(I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 I2CP_scbMode;
    extern uint8 I2CP_scbEnableWake;
    extern uint8 I2CP_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 I2CP_mode;
    extern uint8 I2CP_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * I2CP_rxBuffer;
    extern uint8   I2CP_rxDataBits;
    extern uint32  I2CP_rxBufferSize;

    extern volatile uint8 * I2CP_txBuffer;
    extern uint8   I2CP_txDataBits;
    extern uint32  I2CP_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 I2CP_numberOfAddr;
    extern uint8 I2CP_subAddrSize;
#endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (I2CP_SCB_MODE_I2C_CONST_CFG || \
        I2CP_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 I2CP_IntrTxMask;
#endif /* (! (I2CP_SCB_MODE_I2C_CONST_CFG || \
              I2CP_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define I2CP_SCB_MODE_I2C_RUNTM_CFG     (I2CP_SCB_MODE_I2C      == I2CP_scbMode)
    #define I2CP_SCB_MODE_SPI_RUNTM_CFG     (I2CP_SCB_MODE_SPI      == I2CP_scbMode)
    #define I2CP_SCB_MODE_UART_RUNTM_CFG    (I2CP_SCB_MODE_UART     == I2CP_scbMode)
    #define I2CP_SCB_MODE_EZI2C_RUNTM_CFG   (I2CP_SCB_MODE_EZI2C    == I2CP_scbMode)
    #define I2CP_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (I2CP_SCB_MODE_UNCONFIG == I2CP_scbMode)

    /* Defines wakeup enable */
    #define I2CP_SCB_WAKE_ENABLE_CHECK       (0u != I2CP_scbEnableWake)
#endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!I2CP_CY_SCBIP_V1)
    #define I2CP_SCB_PINS_NUMBER    (7u)
#else
    #define I2CP_SCB_PINS_NUMBER    (2u)
#endif /* (!I2CP_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_I2CP_H) */


/* [] END OF FILE */
