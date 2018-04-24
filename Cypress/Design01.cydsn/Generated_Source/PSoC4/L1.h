/*******************************************************************************
* File Name: L1.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_L1_H) /* Pins L1_H */
#define CY_PINS_L1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "L1_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} L1_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   L1_Read(void);
void    L1_Write(uint8 value);
uint8   L1_ReadDataReg(void);
#if defined(L1__PC) || (CY_PSOC4_4200L) 
    void    L1_SetDriveMode(uint8 mode);
#endif
void    L1_SetInterruptMode(uint16 position, uint16 mode);
uint8   L1_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void L1_Sleep(void); 
void L1_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(L1__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define L1_DRIVE_MODE_BITS        (3)
    #define L1_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - L1_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the L1_SetDriveMode() function.
         *  @{
         */
        #define L1_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define L1_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define L1_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define L1_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define L1_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define L1_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define L1_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define L1_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define L1_MASK               L1__MASK
#define L1_SHIFT              L1__SHIFT
#define L1_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in L1_SetInterruptMode() function.
     *  @{
     */
        #define L1_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define L1_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define L1_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define L1_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(L1__SIO)
    #define L1_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(L1__PC) && (CY_PSOC4_4200L)
    #define L1_USBIO_ENABLE               ((uint32)0x80000000u)
    #define L1_USBIO_DISABLE              ((uint32)(~L1_USBIO_ENABLE))
    #define L1_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define L1_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define L1_USBIO_ENTER_SLEEP          ((uint32)((1u << L1_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << L1_USBIO_SUSPEND_DEL_SHIFT)))
    #define L1_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << L1_USBIO_SUSPEND_SHIFT)))
    #define L1_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << L1_USBIO_SUSPEND_DEL_SHIFT)))
    #define L1_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(L1__PC)
    /* Port Configuration */
    #define L1_PC                 (* (reg32 *) L1__PC)
#endif
/* Pin State */
#define L1_PS                     (* (reg32 *) L1__PS)
/* Data Register */
#define L1_DR                     (* (reg32 *) L1__DR)
/* Input Buffer Disable Override */
#define L1_INP_DIS                (* (reg32 *) L1__PC2)

/* Interrupt configuration Registers */
#define L1_INTCFG                 (* (reg32 *) L1__INTCFG)
#define L1_INTSTAT                (* (reg32 *) L1__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define L1_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(L1__SIO)
    #define L1_SIO_REG            (* (reg32 *) L1__SIO)
#endif /* (L1__SIO_CFG) */

/* USBIO registers */
#if !defined(L1__PC) && (CY_PSOC4_4200L)
    #define L1_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define L1_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define L1_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define L1_DRIVE_MODE_SHIFT       (0x00u)
#define L1_DRIVE_MODE_MASK        (0x07u << L1_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins L1_H */


/* [] END OF FILE */
