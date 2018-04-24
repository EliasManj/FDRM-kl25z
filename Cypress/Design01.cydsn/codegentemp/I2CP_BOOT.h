/***************************************************************************//**
* \file I2CP_BOOT.h
* \version 4.0
*
* \brief
*  This file provides constants and parameter values of the bootloader
*  communication APIs for the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2014-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_BOOT_I2CP_H)
#define CY_SCB_BOOT_I2CP_H

#include "I2CP_PVT.h"

#if (I2CP_SCB_MODE_I2C_INC)
    #include "I2CP_I2C.h"
#endif /* (I2CP_SCB_MODE_I2C_INC) */

#if (I2CP_SCB_MODE_EZI2C_INC)
    #include "I2CP_EZI2C.h"
#endif /* (I2CP_SCB_MODE_EZI2C_INC) */

#if (I2CP_SCB_MODE_SPI_INC || I2CP_SCB_MODE_UART_INC)
    #include "I2CP_SPI_UART.h"
#endif /* (I2CP_SCB_MODE_SPI_INC || I2CP_SCB_MODE_UART_INC) */


/***************************************
*  Conditional Compilation Parameters
****************************************/

/* Bootloader communication interface enable */
#define I2CP_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_I2CP) || \
                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

/* Enable I2C bootloader communication */
#if (I2CP_SCB_MODE_I2C_INC)
    #define I2CP_I2C_BTLDR_COMM_ENABLED     (I2CP_BTLDR_COMM_ENABLED && \
                                                            (I2CP_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             I2CP_I2C_SLAVE_CONST))
#else
     #define I2CP_I2C_BTLDR_COMM_ENABLED    (0u)
#endif /* (I2CP_SCB_MODE_I2C_INC) */

/* EZI2C does not support bootloader communication. Provide empty APIs */
#if (I2CP_SCB_MODE_EZI2C_INC)
    #define I2CP_EZI2C_BTLDR_COMM_ENABLED   (I2CP_BTLDR_COMM_ENABLED && \
                                                         I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
#else
    #define I2CP_EZI2C_BTLDR_COMM_ENABLED   (0u)
#endif /* (I2CP_EZI2C_BTLDR_COMM_ENABLED) */

/* Enable SPI bootloader communication */
#if (I2CP_SCB_MODE_SPI_INC)
    #define I2CP_SPI_BTLDR_COMM_ENABLED     (I2CP_BTLDR_COMM_ENABLED && \
                                                            (I2CP_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             I2CP_SPI_SLAVE_CONST))
#else
        #define I2CP_SPI_BTLDR_COMM_ENABLED (0u)
#endif /* (I2CP_SPI_BTLDR_COMM_ENABLED) */

/* Enable UART bootloader communication */
#if (I2CP_SCB_MODE_UART_INC)
       #define I2CP_UART_BTLDR_COMM_ENABLED    (I2CP_BTLDR_COMM_ENABLED && \
                                                            (I2CP_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             (I2CP_UART_RX_DIRECTION && \
                                                              I2CP_UART_TX_DIRECTION)))
#else
     #define I2CP_UART_BTLDR_COMM_ENABLED   (0u)
#endif /* (I2CP_UART_BTLDR_COMM_ENABLED) */

/* Enable bootloader communication */
#define I2CP_BTLDR_COMM_MODE_ENABLED    (I2CP_I2C_BTLDR_COMM_ENABLED   || \
                                                     I2CP_SPI_BTLDR_COMM_ENABLED   || \
                                                     I2CP_EZI2C_BTLDR_COMM_ENABLED || \
                                                     I2CP_UART_BTLDR_COMM_ENABLED)


/***************************************
*        Function Prototypes
***************************************/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_I2C_BTLDR_COMM_ENABLED)
    /* I2C Bootloader physical layer functions */
    void I2CP_I2CCyBtldrCommStart(void);
    void I2CP_I2CCyBtldrCommStop (void);
    void I2CP_I2CCyBtldrCommReset(void);
    cystatus I2CP_I2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus I2CP_I2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map I2C specific bootloader communication APIs to SCB specific APIs */
    #if (I2CP_SCB_MODE_I2C_CONST_CFG)
        #define I2CP_CyBtldrCommStart   I2CP_I2CCyBtldrCommStart
        #define I2CP_CyBtldrCommStop    I2CP_I2CCyBtldrCommStop
        #define I2CP_CyBtldrCommReset   I2CP_I2CCyBtldrCommReset
        #define I2CP_CyBtldrCommRead    I2CP_I2CCyBtldrCommRead
        #define I2CP_CyBtldrCommWrite   I2CP_I2CCyBtldrCommWrite
    #endif /* (I2CP_SCB_MODE_I2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_I2C_BTLDR_COMM_ENABLED) */


#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void I2CP_EzI2CCyBtldrCommStart(void);
    void I2CP_EzI2CCyBtldrCommStop (void);
    void I2CP_EzI2CCyBtldrCommReset(void);
    cystatus I2CP_EzI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus I2CP_EzI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map EZI2C specific bootloader communication APIs to SCB specific APIs */
    #if (I2CP_SCB_MODE_EZI2C_CONST_CFG)
        #define I2CP_CyBtldrCommStart   I2CP_EzI2CCyBtldrCommStart
        #define I2CP_CyBtldrCommStop    I2CP_EzI2CCyBtldrCommStop
        #define I2CP_CyBtldrCommReset   I2CP_EzI2CCyBtldrCommReset
        #define I2CP_CyBtldrCommRead    I2CP_EzI2CCyBtldrCommRead
        #define I2CP_CyBtldrCommWrite   I2CP_EzI2CCyBtldrCommWrite
    #endif /* (I2CP_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_EZI2C_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_SPI_BTLDR_COMM_ENABLED)
    /* SPI Bootloader physical layer functions */
    void I2CP_SpiCyBtldrCommStart(void);
    void I2CP_SpiCyBtldrCommStop (void);
    void I2CP_SpiCyBtldrCommReset(void);
    cystatus I2CP_SpiCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus I2CP_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map SPI specific bootloader communication APIs to SCB specific APIs */
    #if (I2CP_SCB_MODE_SPI_CONST_CFG)
        #define I2CP_CyBtldrCommStart   I2CP_SpiCyBtldrCommStart
        #define I2CP_CyBtldrCommStop    I2CP_SpiCyBtldrCommStop
        #define I2CP_CyBtldrCommReset   I2CP_SpiCyBtldrCommReset
        #define I2CP_CyBtldrCommRead    I2CP_SpiCyBtldrCommRead
        #define I2CP_CyBtldrCommWrite   I2CP_SpiCyBtldrCommWrite
    #endif /* (I2CP_SCB_MODE_SPI_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_SPI_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_UART_BTLDR_COMM_ENABLED)
    /* UART Bootloader physical layer functions */
    void I2CP_UartCyBtldrCommStart(void);
    void I2CP_UartCyBtldrCommStop (void);
    void I2CP_UartCyBtldrCommReset(void);
    cystatus I2CP_UartCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus I2CP_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map UART specific bootloader communication APIs to SCB specific APIs */
    #if (I2CP_SCB_MODE_UART_CONST_CFG)
        #define I2CP_CyBtldrCommStart   I2CP_UartCyBtldrCommStart
        #define I2CP_CyBtldrCommStop    I2CP_UartCyBtldrCommStop
        #define I2CP_CyBtldrCommReset   I2CP_UartCyBtldrCommReset
        #define I2CP_CyBtldrCommRead    I2CP_UartCyBtldrCommRead
        #define I2CP_CyBtldrCommWrite   I2CP_UartCyBtldrCommWrite
    #endif /* (I2CP_SCB_MODE_UART_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_UART_BTLDR_COMM_ENABLED) */

/**
* \addtogroup group_bootloader
* @{
*/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_BTLDR_COMM_ENABLED)
    #if (I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
        /* Bootloader physical layer functions */
        void I2CP_CyBtldrCommStart(void);
        void I2CP_CyBtldrCommStop (void);
        void I2CP_CyBtldrCommReset(void);
        cystatus I2CP_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
        cystatus I2CP_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    #endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Map SCB specific bootloader communication APIs to common APIs */
    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_I2CP)
        #define CyBtldrCommStart    I2CP_CyBtldrCommStart
        #define CyBtldrCommStop     I2CP_CyBtldrCommStop
        #define CyBtldrCommReset    I2CP_CyBtldrCommReset
        #define CyBtldrCommWrite    I2CP_CyBtldrCommWrite
        #define CyBtldrCommRead     I2CP_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_I2CP) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (I2CP_BTLDR_COMM_ENABLED) */

/** @} group_bootloader */

/***************************************
*           API Constants
***************************************/

/* Timeout unit in milliseconds */
#define I2CP_WAIT_1_MS  (1u)

/* Return number of bytes to copy into bootloader buffer */
#define I2CP_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )

/* Size of Read/Write buffers for I2C bootloader  */
#define I2CP_I2C_BTLDR_SIZEOF_READ_BUFFER   (64u)
#define I2CP_I2C_BTLDR_SIZEOF_WRITE_BUFFER  (64u)

/* Byte to byte time interval: calculated basing on current component
* data rate configuration, can be defined in project if required.
*/
#ifndef I2CP_SPI_BYTE_TO_BYTE
    #define I2CP_SPI_BYTE_TO_BYTE   (160u)
#endif

/* Byte to byte time interval: calculated basing on current component
* baud rate configuration, can be defined in the project if required.
*/
#ifndef I2CP_UART_BYTE_TO_BYTE
    #define I2CP_UART_BYTE_TO_BYTE  (2500u)
#endif /* I2CP_UART_BYTE_TO_BYTE */

#endif /* (CY_SCB_BOOT_I2CP_H) */


/* [] END OF FILE */
