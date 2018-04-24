/*******************************************************************************
* File Name: Motor.h  
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

#if !defined(CY_PINS_Motor_H) /* Pins Motor_H */
#define CY_PINS_Motor_H

#include "cytypes.h"
#include "cyfitter.h"
#include "Motor_aliases.h"


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
} Motor_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   Motor_Read(void);
void    Motor_Write(uint8 value);
uint8   Motor_ReadDataReg(void);
#if defined(Motor__PC) || (CY_PSOC4_4200L) 
    void    Motor_SetDriveMode(uint8 mode);
#endif
void    Motor_SetInterruptMode(uint16 position, uint16 mode);
uint8   Motor_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void Motor_Sleep(void); 
void Motor_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(Motor__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define Motor_DRIVE_MODE_BITS        (3)
    #define Motor_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - Motor_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the Motor_SetDriveMode() function.
         *  @{
         */
        #define Motor_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define Motor_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define Motor_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define Motor_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define Motor_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define Motor_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define Motor_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define Motor_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define Motor_MASK               Motor__MASK
#define Motor_SHIFT              Motor__SHIFT
#define Motor_WIDTH              4u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Motor_SetInterruptMode() function.
     *  @{
     */
        #define Motor_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define Motor_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define Motor_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define Motor_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(Motor__SIO)
    #define Motor_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(Motor__PC) && (CY_PSOC4_4200L)
    #define Motor_USBIO_ENABLE               ((uint32)0x80000000u)
    #define Motor_USBIO_DISABLE              ((uint32)(~Motor_USBIO_ENABLE))
    #define Motor_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define Motor_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define Motor_USBIO_ENTER_SLEEP          ((uint32)((1u << Motor_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << Motor_USBIO_SUSPEND_DEL_SHIFT)))
    #define Motor_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << Motor_USBIO_SUSPEND_SHIFT)))
    #define Motor_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << Motor_USBIO_SUSPEND_DEL_SHIFT)))
    #define Motor_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(Motor__PC)
    /* Port Configuration */
    #define Motor_PC                 (* (reg32 *) Motor__PC)
#endif
/* Pin State */
#define Motor_PS                     (* (reg32 *) Motor__PS)
/* Data Register */
#define Motor_DR                     (* (reg32 *) Motor__DR)
/* Input Buffer Disable Override */
#define Motor_INP_DIS                (* (reg32 *) Motor__PC2)

/* Interrupt configuration Registers */
#define Motor_INTCFG                 (* (reg32 *) Motor__INTCFG)
#define Motor_INTSTAT                (* (reg32 *) Motor__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define Motor_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(Motor__SIO)
    #define Motor_SIO_REG            (* (reg32 *) Motor__SIO)
#endif /* (Motor__SIO_CFG) */

/* USBIO registers */
#if !defined(Motor__PC) && (CY_PSOC4_4200L)
    #define Motor_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define Motor_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define Motor_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define Motor_DRIVE_MODE_SHIFT       (0x00u)
#define Motor_DRIVE_MODE_MASK        (0x07u << Motor_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins Motor_H */


/* [] END OF FILE */
