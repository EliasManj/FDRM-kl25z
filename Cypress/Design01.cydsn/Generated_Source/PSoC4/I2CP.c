/***************************************************************************//**
* \file I2CP.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "I2CP_PVT.h"

#if (I2CP_SCB_MODE_I2C_INC)
    #include "I2CP_I2C_PVT.h"
#endif /* (I2CP_SCB_MODE_I2C_INC) */

#if (I2CP_SCB_MODE_EZI2C_INC)
    #include "I2CP_EZI2C_PVT.h"
#endif /* (I2CP_SCB_MODE_EZI2C_INC) */

#if (I2CP_SCB_MODE_SPI_INC || I2CP_SCB_MODE_UART_INC)
    #include "I2CP_SPI_UART_PVT.h"
#endif /* (I2CP_SCB_MODE_SPI_INC || I2CP_SCB_MODE_UART_INC) */


/***************************************
*    Run Time Configuration Vars
***************************************/

/* Stores internal component configuration for Unconfigured mode */
#if (I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    uint8 I2CP_scbMode = I2CP_SCB_MODE_UNCONFIG;
    uint8 I2CP_scbEnableWake;
    uint8 I2CP_scbEnableIntr;

    /* I2C configuration variables */
    uint8 I2CP_mode;
    uint8 I2CP_acceptAddr;

    /* SPI/UART configuration variables */
    volatile uint8 * I2CP_rxBuffer;
    uint8  I2CP_rxDataBits;
    uint32 I2CP_rxBufferSize;

    volatile uint8 * I2CP_txBuffer;
    uint8  I2CP_txDataBits;
    uint32 I2CP_txBufferSize;

    /* EZI2C configuration variables */
    uint8 I2CP_numberOfAddr;
    uint8 I2CP_subAddrSize;
#endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Common SCB Vars
***************************************/
/**
* \addtogroup group_general
* \{
*/

/** I2CP_initVar indicates whether the I2CP 
*  component has been initialized. The variable is initialized to 0 
*  and set to 1 the first time SCB_Start() is called. This allows 
*  the component to restart without reinitialization after the first 
*  call to the I2CP_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  I2CP_Init() function can be called before the 
*  I2CP_Start() or I2CP_Enable() function.
*/
uint8 I2CP_initVar = 0u;


#if (! (I2CP_SCB_MODE_I2C_CONST_CFG || \
        I2CP_SCB_MODE_EZI2C_CONST_CFG))
    /** This global variable stores TX interrupt sources after 
    * I2CP_Stop() is called. Only these TX interrupt sources 
    * will be restored on a subsequent I2CP_Enable() call.
    */
    uint16 I2CP_IntrTxMask = 0u;
#endif /* (! (I2CP_SCB_MODE_I2C_CONST_CFG || \
              I2CP_SCB_MODE_EZI2C_CONST_CFG)) */
/** \} globals */

#if (I2CP_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_I2CP_CUSTOM_INTR_HANDLER)
    void (*I2CP_customIntrHandler)(void) = NULL;
#endif /* !defined (CY_REMOVE_I2CP_CUSTOM_INTR_HANDLER) */
#endif /* (I2CP_SCB_IRQ_INTERNAL) */


/***************************************
*    Private Function Prototypes
***************************************/

static void I2CP_ScbEnableIntr(void);
static void I2CP_ScbModeStop(void);
static void I2CP_ScbModePostEnable(void);


/*******************************************************************************
* Function Name: I2CP_Init
****************************************************************************//**
*
*  Initializes the I2CP component to operate in one of the selected
*  configurations: I2C, SPI, UART or EZI2C.
*  When the configuration is set to "Unconfigured SCB", this function does
*  not do any initialization. Use mode-specific initialization APIs instead:
*  I2CP_I2CInit, I2CP_SpiInit, 
*  I2CP_UartInit or I2CP_EzI2CInit.
*
*******************************************************************************/
void I2CP_Init(void)
{
#if (I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
    if (I2CP_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        I2CP_initVar = 0u;
    }
    else
    {
        /* Initialization was done before this function call */
    }

#elif (I2CP_SCB_MODE_I2C_CONST_CFG)
    I2CP_I2CInit();

#elif (I2CP_SCB_MODE_SPI_CONST_CFG)
    I2CP_SpiInit();

#elif (I2CP_SCB_MODE_UART_CONST_CFG)
    I2CP_UartInit();

#elif (I2CP_SCB_MODE_EZI2C_CONST_CFG)
    I2CP_EzI2CInit();

#endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: I2CP_Enable
****************************************************************************//**
*
*  Enables I2CP component operation: activates the hardware and 
*  internal interrupt. It also restores TX interrupt sources disabled after the 
*  I2CP_Stop() function was called (note that level-triggered TX 
*  interrupt sources remain disabled to not cause code lock-up).
*  For I2C and EZI2C modes the interrupt is internal and mandatory for 
*  operation. For SPI and UART modes the interrupt can be configured as none, 
*  internal or external.
*  The I2CP configuration should be not changed when the component
*  is enabled. Any configuration changes should be made after disabling the 
*  component.
*  When configuration is set to “Unconfigured I2CP”, the component 
*  must first be initialized to operate in one of the following configurations: 
*  I2C, SPI, UART or EZ I2C, using the mode-specific initialization API. 
*  Otherwise this function does not enable the component.
*
*******************************************************************************/
void I2CP_Enable(void)
{
#if (I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Enable SCB block, only if it is already configured */
    if (!I2CP_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        I2CP_CTRL_REG |= I2CP_CTRL_ENABLED;

        I2CP_ScbEnableIntr();

        /* Call PostEnable function specific to current operation mode */
        I2CP_ScbModePostEnable();
    }
#else
    I2CP_CTRL_REG |= I2CP_CTRL_ENABLED;

    I2CP_ScbEnableIntr();

    /* Call PostEnable function specific to current operation mode */
    I2CP_ScbModePostEnable();
#endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: I2CP_Start
****************************************************************************//**
*
*  Invokes I2CP_Init() and I2CP_Enable().
*  After this function call, the component is enabled and ready for operation.
*  When configuration is set to "Unconfigured SCB", the component must first be
*  initialized to operate in one of the following configurations: I2C, SPI, UART
*  or EZI2C. Otherwise this function does not enable the component.
*
* \globalvars
*  I2CP_initVar - used to check initial configuration, modified
*  on first function call.
*
*******************************************************************************/
void I2CP_Start(void)
{
    if (0u == I2CP_initVar)
    {
        I2CP_Init();
        I2CP_initVar = 1u; /* Component was initialized */
    }

    I2CP_Enable();
}


/*******************************************************************************
* Function Name: I2CP_Stop
****************************************************************************//**
*
*  Disables the I2CP component: disable the hardware and internal 
*  interrupt. It also disables all TX interrupt sources so as not to cause an 
*  unexpected interrupt trigger because after the component is enabled, the 
*  TX FIFO is empty.
*  Refer to the function I2CP_Enable() for the interrupt 
*  configuration details.
*  This function disables the SCB component without checking to see if 
*  communication is in progress. Before calling this function it may be 
*  necessary to check the status of communication to make sure communication 
*  is complete. If this is not done then communication could be stopped mid 
*  byte and corrupted data could result.
*
*******************************************************************************/
void I2CP_Stop(void)
{
#if (I2CP_SCB_IRQ_INTERNAL)
    I2CP_DisableInt();
#endif /* (I2CP_SCB_IRQ_INTERNAL) */

    /* Call Stop function specific to current operation mode */
    I2CP_ScbModeStop();

    /* Disable SCB IP */
    I2CP_CTRL_REG &= (uint32) ~I2CP_CTRL_ENABLED;

    /* Disable all TX interrupt sources so as not to cause an unexpected
    * interrupt trigger after the component will be enabled because the 
    * TX FIFO is empty.
    * For SCB IP v0, it is critical as it does not mask-out interrupt
    * sources when it is disabled. This can cause a code lock-up in the
    * interrupt handler because TX FIFO cannot be loaded after the block
    * is disabled.
    */
    I2CP_SetTxInterruptMode(I2CP_NO_INTR_SOURCES);

#if (I2CP_SCB_IRQ_INTERNAL)
    I2CP_ClearPendingInt();
#endif /* (I2CP_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: I2CP_SetRxFifoLevel
****************************************************************************//**
*
*  Sets level in the RX FIFO to generate a RX level interrupt.
*  When the RX FIFO has more entries than the RX FIFO level an RX level
*  interrupt request is generated.
*
*  \param level: Level in the RX FIFO to generate RX level interrupt.
*   The range of valid level values is between 0 and RX FIFO depth - 1.
*
*******************************************************************************/
void I2CP_SetRxFifoLevel(uint32 level)
{
    uint32 rxFifoCtrl;

    rxFifoCtrl = I2CP_RX_FIFO_CTRL_REG;

    rxFifoCtrl &= ((uint32) ~I2CP_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    rxFifoCtrl |= ((uint32) (I2CP_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    I2CP_RX_FIFO_CTRL_REG = rxFifoCtrl;
}


/*******************************************************************************
* Function Name: I2CP_SetTxFifoLevel
****************************************************************************//**
*
*  Sets level in the TX FIFO to generate a TX level interrupt.
*  When the TX FIFO has less entries than the TX FIFO level an TX level
*  interrupt request is generated.
*
*  \param level: Level in the TX FIFO to generate TX level interrupt.
*   The range of valid level values is between 0 and TX FIFO depth - 1.
*
*******************************************************************************/
void I2CP_SetTxFifoLevel(uint32 level)
{
    uint32 txFifoCtrl;

    txFifoCtrl = I2CP_TX_FIFO_CTRL_REG;

    txFifoCtrl &= ((uint32) ~I2CP_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    txFifoCtrl |= ((uint32) (I2CP_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    I2CP_TX_FIFO_CTRL_REG = txFifoCtrl;
}


#if (I2CP_SCB_IRQ_INTERNAL)
    /*******************************************************************************
    * Function Name: I2CP_SetCustomInterruptHandler
    ****************************************************************************//**
    *
    *  Registers a function to be called by the internal interrupt handler.
    *  First the function that is registered is called, then the internal interrupt
    *  handler performs any operation such as software buffer management functions
    *  before the interrupt returns.  It is the user's responsibility not to break
    *  the software buffer operations. Only one custom handler is supported, which
    *  is the function provided by the most recent call.
    *  At the initialization time no custom handler is registered.
    *
    *  \param func: Pointer to the function to register.
    *        The value NULL indicates to remove the current custom interrupt
    *        handler.
    *
    *******************************************************************************/
    void I2CP_SetCustomInterruptHandler(void (*func)(void))
    {
    #if !defined (CY_REMOVE_I2CP_CUSTOM_INTR_HANDLER)
        I2CP_customIntrHandler = func; /* Register interrupt handler */
    #else
        if (NULL != func)
        {
            /* Suppress compiler warning */
        }
    #endif /* !defined (CY_REMOVE_I2CP_CUSTOM_INTR_HANDLER) */
    }
#endif /* (I2CP_SCB_IRQ_INTERNAL) */


/*******************************************************************************
* Function Name: I2CP_ScbModeEnableIntr
****************************************************************************//**
*
*  Enables an interrupt for a specific mode.
*
*******************************************************************************/
static void I2CP_ScbEnableIntr(void)
{
#if (I2CP_SCB_IRQ_INTERNAL)
    /* Enable interrupt in NVIC */
    #if (I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
        if (0u != I2CP_scbEnableIntr)
        {
            I2CP_EnableInt();
        }

    #else
        I2CP_EnableInt();

    #endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */
#endif /* (I2CP_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: I2CP_ScbModePostEnable
****************************************************************************//**
*
*  Calls the PostEnable function for a specific operation mode.
*
*******************************************************************************/
static void I2CP_ScbModePostEnable(void)
{
#if (I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
#if (!I2CP_CY_SCBIP_V1)
    if (I2CP_SCB_MODE_SPI_RUNTM_CFG)
    {
        I2CP_SpiPostEnable();
    }
    else if (I2CP_SCB_MODE_UART_RUNTM_CFG)
    {
        I2CP_UartPostEnable();
    }
    else
    {
        /* Unknown mode: do nothing */
    }
#endif /* (!I2CP_CY_SCBIP_V1) */

#elif (I2CP_SCB_MODE_SPI_CONST_CFG)
    I2CP_SpiPostEnable();

#elif (I2CP_SCB_MODE_UART_CONST_CFG)
    I2CP_UartPostEnable();

#else
    /* Unknown mode: do nothing */
#endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: I2CP_ScbModeStop
****************************************************************************//**
*
*  Calls the Stop function for a specific operation mode.
*
*******************************************************************************/
static void I2CP_ScbModeStop(void)
{
#if (I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
    if (I2CP_SCB_MODE_I2C_RUNTM_CFG)
    {
        I2CP_I2CStop();
    }
    else if (I2CP_SCB_MODE_EZI2C_RUNTM_CFG)
    {
        I2CP_EzI2CStop();
    }
#if (!I2CP_CY_SCBIP_V1)
    else if (I2CP_SCB_MODE_SPI_RUNTM_CFG)
    {
        I2CP_SpiStop();
    }
    else if (I2CP_SCB_MODE_UART_RUNTM_CFG)
    {
        I2CP_UartStop();
    }
#endif /* (!I2CP_CY_SCBIP_V1) */
    else
    {
        /* Unknown mode: do nothing */
    }
#elif (I2CP_SCB_MODE_I2C_CONST_CFG)
    I2CP_I2CStop();

#elif (I2CP_SCB_MODE_EZI2C_CONST_CFG)
    I2CP_EzI2CStop();

#elif (I2CP_SCB_MODE_SPI_CONST_CFG)
    I2CP_SpiStop();

#elif (I2CP_SCB_MODE_UART_CONST_CFG)
    I2CP_UartStop();

#else
    /* Unknown mode: do nothing */
#endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */
}


#if (I2CP_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: I2CP_SetPins
    ****************************************************************************//**
    *
    *  Sets the pins settings accordingly to the selected operation mode.
    *  Only available in the Unconfigured operation mode. The mode specific
    *  initialization function calls it.
    *  Pins configuration is set by PSoC Creator when a specific mode of operation
    *  is selected in design time.
    *
    *  \param mode:      Mode of SCB operation.
    *  \param subMode:   Sub-mode of SCB operation. It is only required for SPI and UART
    *             modes.
    *  \param uartEnableMask: enables TX or RX direction and RTS and CTS signals.
    *
    *******************************************************************************/
    void I2CP_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask)
    {
        uint32 pinsDm[I2CP_SCB_PINS_NUMBER];
        uint32 i;
        
    #if (!I2CP_CY_SCBIP_V1)
        uint32 pinsInBuf = 0u;
    #endif /* (!I2CP_CY_SCBIP_V1) */
        
        uint32 hsiomSel[I2CP_SCB_PINS_NUMBER] = 
        {
            I2CP_RX_SDA_MOSI_HSIOM_SEL_GPIO,
            I2CP_TX_SCL_MISO_HSIOM_SEL_GPIO,
            0u,
            0u,
            0u,
            0u,
            0u,
        };

    #if (I2CP_CY_SCBIP_V1)
        /* Supress compiler warning. */
        if ((0u == subMode) || (0u == uartEnableMask))
        {
        }
    #endif /* (I2CP_CY_SCBIP_V1) */

        /* Set default HSIOM to GPIO and Drive Mode to Analog Hi-Z */
        for (i = 0u; i < I2CP_SCB_PINS_NUMBER; i++)
        {
            pinsDm[i] = I2CP_PIN_DM_ALG_HIZ;
        }

        if ((I2CP_SCB_MODE_I2C   == mode) ||
            (I2CP_SCB_MODE_EZI2C == mode))
        {
        #if (I2CP_RX_SDA_MOSI_PIN)
            hsiomSel[I2CP_RX_SDA_MOSI_PIN_INDEX] = I2CP_RX_SDA_MOSI_HSIOM_SEL_I2C;
            pinsDm  [I2CP_RX_SDA_MOSI_PIN_INDEX] = I2CP_PIN_DM_OD_LO;
        #elif (I2CP_RX_WAKE_SDA_MOSI_PIN)
            hsiomSel[I2CP_RX_WAKE_SDA_MOSI_PIN_INDEX] = I2CP_RX_WAKE_SDA_MOSI_HSIOM_SEL_I2C;
            pinsDm  [I2CP_RX_WAKE_SDA_MOSI_PIN_INDEX] = I2CP_PIN_DM_OD_LO;
        #else
        #endif /* (I2CP_RX_SDA_MOSI_PIN) */
        
        #if (I2CP_TX_SCL_MISO_PIN)
            hsiomSel[I2CP_TX_SCL_MISO_PIN_INDEX] = I2CP_TX_SCL_MISO_HSIOM_SEL_I2C;
            pinsDm  [I2CP_TX_SCL_MISO_PIN_INDEX] = I2CP_PIN_DM_OD_LO;
        #endif /* (I2CP_TX_SCL_MISO_PIN) */
        }
    #if (!I2CP_CY_SCBIP_V1)
        else if (I2CP_SCB_MODE_SPI == mode)
        {
        #if (I2CP_RX_SDA_MOSI_PIN)
            hsiomSel[I2CP_RX_SDA_MOSI_PIN_INDEX] = I2CP_RX_SDA_MOSI_HSIOM_SEL_SPI;
        #elif (I2CP_RX_WAKE_SDA_MOSI_PIN)
            hsiomSel[I2CP_RX_WAKE_SDA_MOSI_PIN_INDEX] = I2CP_RX_WAKE_SDA_MOSI_HSIOM_SEL_SPI;
        #else
        #endif /* (I2CP_RX_SDA_MOSI_PIN) */
        
        #if (I2CP_TX_SCL_MISO_PIN)
            hsiomSel[I2CP_TX_SCL_MISO_PIN_INDEX] = I2CP_TX_SCL_MISO_HSIOM_SEL_SPI;
        #endif /* (I2CP_TX_SCL_MISO_PIN) */
        
        #if (I2CP_CTS_SCLK_PIN)
            hsiomSel[I2CP_CTS_SCLK_PIN_INDEX] = I2CP_CTS_SCLK_HSIOM_SEL_SPI;
        #endif /* (I2CP_CTS_SCLK_PIN) */

            if (I2CP_SPI_SLAVE == subMode)
            {
                /* Slave */
                pinsDm[I2CP_RX_SDA_MOSI_PIN_INDEX] = I2CP_PIN_DM_DIG_HIZ;
                pinsDm[I2CP_TX_SCL_MISO_PIN_INDEX] = I2CP_PIN_DM_STRONG;
                pinsDm[I2CP_CTS_SCLK_PIN_INDEX] = I2CP_PIN_DM_DIG_HIZ;

            #if (I2CP_RTS_SS0_PIN)
                /* Only SS0 is valid choice for Slave */
                hsiomSel[I2CP_RTS_SS0_PIN_INDEX] = I2CP_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm  [I2CP_RTS_SS0_PIN_INDEX] = I2CP_PIN_DM_DIG_HIZ;
            #endif /* (I2CP_RTS_SS0_PIN) */

            #if (I2CP_TX_SCL_MISO_PIN)
                /* Disable input buffer */
                 pinsInBuf |= I2CP_TX_SCL_MISO_PIN_MASK;
            #endif /* (I2CP_TX_SCL_MISO_PIN) */
            }
            else 
            {
                /* (Master) */
                pinsDm[I2CP_RX_SDA_MOSI_PIN_INDEX] = I2CP_PIN_DM_STRONG;
                pinsDm[I2CP_TX_SCL_MISO_PIN_INDEX] = I2CP_PIN_DM_DIG_HIZ;
                pinsDm[I2CP_CTS_SCLK_PIN_INDEX] = I2CP_PIN_DM_STRONG;

            #if (I2CP_RTS_SS0_PIN)
                hsiomSel [I2CP_RTS_SS0_PIN_INDEX] = I2CP_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm   [I2CP_RTS_SS0_PIN_INDEX] = I2CP_PIN_DM_STRONG;
                pinsInBuf |= I2CP_RTS_SS0_PIN_MASK;
            #endif /* (I2CP_RTS_SS0_PIN) */

            #if (I2CP_SS1_PIN)
                hsiomSel [I2CP_SS1_PIN_INDEX] = I2CP_SS1_HSIOM_SEL_SPI;
                pinsDm   [I2CP_SS1_PIN_INDEX] = I2CP_PIN_DM_STRONG;
                pinsInBuf |= I2CP_SS1_PIN_MASK;
            #endif /* (I2CP_SS1_PIN) */

            #if (I2CP_SS2_PIN)
                hsiomSel [I2CP_SS2_PIN_INDEX] = I2CP_SS2_HSIOM_SEL_SPI;
                pinsDm   [I2CP_SS2_PIN_INDEX] = I2CP_PIN_DM_STRONG;
                pinsInBuf |= I2CP_SS2_PIN_MASK;
            #endif /* (I2CP_SS2_PIN) */

            #if (I2CP_SS3_PIN)
                hsiomSel [I2CP_SS3_PIN_INDEX] = I2CP_SS3_HSIOM_SEL_SPI;
                pinsDm   [I2CP_SS3_PIN_INDEX] = I2CP_PIN_DM_STRONG;
                pinsInBuf |= I2CP_SS3_PIN_MASK;
            #endif /* (I2CP_SS3_PIN) */

                /* Disable input buffers */
            #if (I2CP_RX_SDA_MOSI_PIN)
                pinsInBuf |= I2CP_RX_SDA_MOSI_PIN_MASK;
            #elif (I2CP_RX_WAKE_SDA_MOSI_PIN)
                pinsInBuf |= I2CP_RX_WAKE_SDA_MOSI_PIN_MASK;
            #else
            #endif /* (I2CP_RX_SDA_MOSI_PIN) */

            #if (I2CP_CTS_SCLK_PIN)
                pinsInBuf |= I2CP_CTS_SCLK_PIN_MASK;
            #endif /* (I2CP_CTS_SCLK_PIN) */
            }
        }
        else /* UART */
        {
            if (I2CP_UART_MODE_SMARTCARD == subMode)
            {
                /* SmartCard */
            #if (I2CP_TX_SCL_MISO_PIN)
                hsiomSel[I2CP_TX_SCL_MISO_PIN_INDEX] = I2CP_TX_SCL_MISO_HSIOM_SEL_UART;
                pinsDm  [I2CP_TX_SCL_MISO_PIN_INDEX] = I2CP_PIN_DM_OD_LO;
            #endif /* (I2CP_TX_SCL_MISO_PIN) */
            }
            else /* Standard or IrDA */
            {
                if (0u != (I2CP_UART_RX_PIN_ENABLE & uartEnableMask))
                {
                #if (I2CP_RX_SDA_MOSI_PIN)
                    hsiomSel[I2CP_RX_SDA_MOSI_PIN_INDEX] = I2CP_RX_SDA_MOSI_HSIOM_SEL_UART;
                    pinsDm  [I2CP_RX_SDA_MOSI_PIN_INDEX] = I2CP_PIN_DM_DIG_HIZ;
                #elif (I2CP_RX_WAKE_SDA_MOSI_PIN)
                    hsiomSel[I2CP_RX_WAKE_SDA_MOSI_PIN_INDEX] = I2CP_RX_WAKE_SDA_MOSI_HSIOM_SEL_UART;
                    pinsDm  [I2CP_RX_WAKE_SDA_MOSI_PIN_INDEX] = I2CP_PIN_DM_DIG_HIZ;
                #else
                #endif /* (I2CP_RX_SDA_MOSI_PIN) */
                }

                if (0u != (I2CP_UART_TX_PIN_ENABLE & uartEnableMask))
                {
                #if (I2CP_TX_SCL_MISO_PIN)
                    hsiomSel[I2CP_TX_SCL_MISO_PIN_INDEX] = I2CP_TX_SCL_MISO_HSIOM_SEL_UART;
                    pinsDm  [I2CP_TX_SCL_MISO_PIN_INDEX] = I2CP_PIN_DM_STRONG;
                    
                    /* Disable input buffer */
                    pinsInBuf |= I2CP_TX_SCL_MISO_PIN_MASK;
                #endif /* (I2CP_TX_SCL_MISO_PIN) */
                }

            #if !(I2CP_CY_SCBIP_V0 || I2CP_CY_SCBIP_V1)
                if (I2CP_UART_MODE_STD == subMode)
                {
                    if (0u != (I2CP_UART_CTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* CTS input is multiplexed with SCLK */
                    #if (I2CP_CTS_SCLK_PIN)
                        hsiomSel[I2CP_CTS_SCLK_PIN_INDEX] = I2CP_CTS_SCLK_HSIOM_SEL_UART;
                        pinsDm  [I2CP_CTS_SCLK_PIN_INDEX] = I2CP_PIN_DM_DIG_HIZ;
                    #endif /* (I2CP_CTS_SCLK_PIN) */
                    }

                    if (0u != (I2CP_UART_RTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* RTS output is multiplexed with SS0 */
                    #if (I2CP_RTS_SS0_PIN)
                        hsiomSel[I2CP_RTS_SS0_PIN_INDEX] = I2CP_RTS_SS0_HSIOM_SEL_UART;
                        pinsDm  [I2CP_RTS_SS0_PIN_INDEX] = I2CP_PIN_DM_STRONG;
                        
                        /* Disable input buffer */
                        pinsInBuf |= I2CP_RTS_SS0_PIN_MASK;
                    #endif /* (I2CP_RTS_SS0_PIN) */
                    }
                }
            #endif /* !(I2CP_CY_SCBIP_V0 || I2CP_CY_SCBIP_V1) */
            }
        }
    #endif /* (!I2CP_CY_SCBIP_V1) */

    /* Configure pins: set HSIOM, DM and InputBufEnable */
    /* Note: the DR register settings do not effect the pin output if HSIOM is other than GPIO */

    #if (I2CP_RX_SDA_MOSI_PIN)
        I2CP_SET_HSIOM_SEL(I2CP_RX_SDA_MOSI_HSIOM_REG,
                                       I2CP_RX_SDA_MOSI_HSIOM_MASK,
                                       I2CP_RX_SDA_MOSI_HSIOM_POS,
                                        hsiomSel[I2CP_RX_SDA_MOSI_PIN_INDEX]);

        I2CP_uart_rx_i2c_sda_spi_mosi_SetDriveMode((uint8) pinsDm[I2CP_RX_SDA_MOSI_PIN_INDEX]);

        #if (!I2CP_CY_SCBIP_V1)
            I2CP_SET_INP_DIS(I2CP_uart_rx_i2c_sda_spi_mosi_INP_DIS,
                                         I2CP_uart_rx_i2c_sda_spi_mosi_MASK,
                                         (0u != (pinsInBuf & I2CP_RX_SDA_MOSI_PIN_MASK)));
        #endif /* (!I2CP_CY_SCBIP_V1) */
    
    #elif (I2CP_RX_WAKE_SDA_MOSI_PIN)
        I2CP_SET_HSIOM_SEL(I2CP_RX_WAKE_SDA_MOSI_HSIOM_REG,
                                       I2CP_RX_WAKE_SDA_MOSI_HSIOM_MASK,
                                       I2CP_RX_WAKE_SDA_MOSI_HSIOM_POS,
                                       hsiomSel[I2CP_RX_WAKE_SDA_MOSI_PIN_INDEX]);

        I2CP_uart_rx_wake_i2c_sda_spi_mosi_SetDriveMode((uint8)
                                                               pinsDm[I2CP_RX_WAKE_SDA_MOSI_PIN_INDEX]);

        I2CP_SET_INP_DIS(I2CP_uart_rx_wake_i2c_sda_spi_mosi_INP_DIS,
                                     I2CP_uart_rx_wake_i2c_sda_spi_mosi_MASK,
                                     (0u != (pinsInBuf & I2CP_RX_WAKE_SDA_MOSI_PIN_MASK)));

         /* Set interrupt on falling edge */
        I2CP_SET_INCFG_TYPE(I2CP_RX_WAKE_SDA_MOSI_INTCFG_REG,
                                        I2CP_RX_WAKE_SDA_MOSI_INTCFG_TYPE_MASK,
                                        I2CP_RX_WAKE_SDA_MOSI_INTCFG_TYPE_POS,
                                        I2CP_INTCFG_TYPE_FALLING_EDGE);
    #else
    #endif /* (I2CP_RX_WAKE_SDA_MOSI_PIN) */

    #if (I2CP_TX_SCL_MISO_PIN)
        I2CP_SET_HSIOM_SEL(I2CP_TX_SCL_MISO_HSIOM_REG,
                                       I2CP_TX_SCL_MISO_HSIOM_MASK,
                                       I2CP_TX_SCL_MISO_HSIOM_POS,
                                        hsiomSel[I2CP_TX_SCL_MISO_PIN_INDEX]);

        I2CP_uart_tx_i2c_scl_spi_miso_SetDriveMode((uint8) pinsDm[I2CP_TX_SCL_MISO_PIN_INDEX]);

    #if (!I2CP_CY_SCBIP_V1)
        I2CP_SET_INP_DIS(I2CP_uart_tx_i2c_scl_spi_miso_INP_DIS,
                                     I2CP_uart_tx_i2c_scl_spi_miso_MASK,
                                    (0u != (pinsInBuf & I2CP_TX_SCL_MISO_PIN_MASK)));
    #endif /* (!I2CP_CY_SCBIP_V1) */
    #endif /* (I2CP_RX_SDA_MOSI_PIN) */

    #if (I2CP_CTS_SCLK_PIN)
        I2CP_SET_HSIOM_SEL(I2CP_CTS_SCLK_HSIOM_REG,
                                       I2CP_CTS_SCLK_HSIOM_MASK,
                                       I2CP_CTS_SCLK_HSIOM_POS,
                                       hsiomSel[I2CP_CTS_SCLK_PIN_INDEX]);

        I2CP_uart_cts_spi_sclk_SetDriveMode((uint8) pinsDm[I2CP_CTS_SCLK_PIN_INDEX]);

        I2CP_SET_INP_DIS(I2CP_uart_cts_spi_sclk_INP_DIS,
                                     I2CP_uart_cts_spi_sclk_MASK,
                                     (0u != (pinsInBuf & I2CP_CTS_SCLK_PIN_MASK)));
    #endif /* (I2CP_CTS_SCLK_PIN) */

    #if (I2CP_RTS_SS0_PIN)
        I2CP_SET_HSIOM_SEL(I2CP_RTS_SS0_HSIOM_REG,
                                       I2CP_RTS_SS0_HSIOM_MASK,
                                       I2CP_RTS_SS0_HSIOM_POS,
                                       hsiomSel[I2CP_RTS_SS0_PIN_INDEX]);

        I2CP_uart_rts_spi_ss0_SetDriveMode((uint8) pinsDm[I2CP_RTS_SS0_PIN_INDEX]);

        I2CP_SET_INP_DIS(I2CP_uart_rts_spi_ss0_INP_DIS,
                                     I2CP_uart_rts_spi_ss0_MASK,
                                     (0u != (pinsInBuf & I2CP_RTS_SS0_PIN_MASK)));
    #endif /* (I2CP_RTS_SS0_PIN) */

    #if (I2CP_SS1_PIN)
        I2CP_SET_HSIOM_SEL(I2CP_SS1_HSIOM_REG,
                                       I2CP_SS1_HSIOM_MASK,
                                       I2CP_SS1_HSIOM_POS,
                                       hsiomSel[I2CP_SS1_PIN_INDEX]);

        I2CP_spi_ss1_SetDriveMode((uint8) pinsDm[I2CP_SS1_PIN_INDEX]);

        I2CP_SET_INP_DIS(I2CP_spi_ss1_INP_DIS,
                                     I2CP_spi_ss1_MASK,
                                     (0u != (pinsInBuf & I2CP_SS1_PIN_MASK)));
    #endif /* (I2CP_SS1_PIN) */

    #if (I2CP_SS2_PIN)
        I2CP_SET_HSIOM_SEL(I2CP_SS2_HSIOM_REG,
                                       I2CP_SS2_HSIOM_MASK,
                                       I2CP_SS2_HSIOM_POS,
                                       hsiomSel[I2CP_SS2_PIN_INDEX]);

        I2CP_spi_ss2_SetDriveMode((uint8) pinsDm[I2CP_SS2_PIN_INDEX]);

        I2CP_SET_INP_DIS(I2CP_spi_ss2_INP_DIS,
                                     I2CP_spi_ss2_MASK,
                                     (0u != (pinsInBuf & I2CP_SS2_PIN_MASK)));
    #endif /* (I2CP_SS2_PIN) */

    #if (I2CP_SS3_PIN)
        I2CP_SET_HSIOM_SEL(I2CP_SS3_HSIOM_REG,
                                       I2CP_SS3_HSIOM_MASK,
                                       I2CP_SS3_HSIOM_POS,
                                       hsiomSel[I2CP_SS3_PIN_INDEX]);

        I2CP_spi_ss3_SetDriveMode((uint8) pinsDm[I2CP_SS3_PIN_INDEX]);

        I2CP_SET_INP_DIS(I2CP_spi_ss3_INP_DIS,
                                     I2CP_spi_ss3_MASK,
                                     (0u != (pinsInBuf & I2CP_SS3_PIN_MASK)));
    #endif /* (I2CP_SS3_PIN) */
    }

#endif /* (I2CP_SCB_MODE_UNCONFIG_CONST_CFG) */


#if (I2CP_CY_SCBIP_V0 || I2CP_CY_SCBIP_V1)
    /*******************************************************************************
    * Function Name: I2CP_I2CSlaveNackGeneration
    ****************************************************************************//**
    *
    *  Sets command to generate NACK to the address or data.
    *
    *******************************************************************************/
    void I2CP_I2CSlaveNackGeneration(void)
    {
        /* Check for EC_AM toggle condition: EC_AM and clock stretching for address are enabled */
        if ((0u != (I2CP_CTRL_REG & I2CP_CTRL_EC_AM_MODE)) &&
            (0u == (I2CP_I2C_CTRL_REG & I2CP_I2C_CTRL_S_NOT_READY_ADDR_NACK)))
        {
            /* Toggle EC_AM before NACK generation */
            I2CP_CTRL_REG &= ~I2CP_CTRL_EC_AM_MODE;
            I2CP_CTRL_REG |=  I2CP_CTRL_EC_AM_MODE;
        }

        I2CP_I2C_SLAVE_CMD_REG = I2CP_I2C_SLAVE_CMD_S_NACK;
    }
#endif /* (I2CP_CY_SCBIP_V0 || I2CP_CY_SCBIP_V1) */


/* [] END OF FILE */
