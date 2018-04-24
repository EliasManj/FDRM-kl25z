/*******************************************************************************
* File Name: L4.h  
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

#if !defined(CY_PINS_L4_H) /* Pins L4_H */
#define CY_PINS_L4_H

#include "cytypes.h"
#include "cyfitter.h"
#include "L4_aliases.h"


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
} L4_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   L4_Read(void);
void    L4_Write(uint8 value);
uint8   L4_ReadDataReg(void);
#if defined(L4__PC) || (CY_PSOC4_4200L) 
    void    L4_SetDriveMode(uint8 mode);
#endif
void    L4_SetInterruptMode(uint16 position, uint16 mode);
uint8   L4_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void L4_Sleep(void); 
void L4_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(L4__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define L4_DRIVE_MODE_BITS        (3)
    #define L4_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - L4_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the L4_SetDriveMode() function.
         *  @{
         */
        #define L4_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define L4_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define L4_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define L4_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define L4_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define L4_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define L4_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define L4_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define L4_MASK               L4__MASK
#define L4_SHIFT              L4__SHIFT
#define L4_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in L4_SetInterruptMode() function.
     *  @{
     */
        #define L4_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define L4_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define L4_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define L4_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(L4__SIO)
    #define L4_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(L4__PC) && (CY_PSOC4_4200L)
    #define L4_USBIO_ENABLE               ((uint32)0x80000000u)
    #define L4_USBIO_DISABLE              ((uint32)(~L4_USBIO_ENABLE))
    #define L4_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define L4_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define L4_USBIO_ENTER_SLEEP          ((uint32)((1u << L4_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << L4_USBIO_SUSPEND_DEL_SHIFT)))
    #define L4_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << L4_USBIO_SUSPEND_SHIFT)))
    #define L4_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << L4_USBIO_SUSPEND_DEL_SHIFT)))
    #define L4_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(L4__PC)
    /* Port Configuration */
    #define L4_PC                 (* (reg32 *) L4__PC)
#endif
/* Pin State */
#define L4_PS                     (* (reg32 *) L4__PS)
/* Data Register */
#define L4_DR                     (* (reg32 *) L4__DR)
/* Input Buffer Disable Override */
#define L4_INP_DIS                (* (reg32 *) L4__PC2)

/* Interrupt configuration Registers */
#define L4_INTCFG                 (* (reg32 *) L4__INTCFG)
#define L4_INTSTAT                (* (reg32 *) L4__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define L4_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(L4__SIO)
    #define L4_SIO_REG            (* (reg32 *) L4__SIO)
#endif /* (L4__SIO_CFG) */

/* USBIO registers */
#if !defined(L4__PC) && (CY_PSOC4_4200L)
    #define L4_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define L4_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define L4_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define L4_DRIVE_MODE_SHIFT       (0x00u)
#define L4_DRIVE_MODE_MASK        (0x07u << L4_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins L4_H */


/* [] END OF FILE */
