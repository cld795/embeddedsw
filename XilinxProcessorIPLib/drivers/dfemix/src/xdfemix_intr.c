/******************************************************************************
* Copyright (C) 2021-2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xdfexmix_intr.c
* This file contains functions related to Mixer interrupt handling.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ---    -------- -----------------------------------------------
* 1.0   dc     10/21/20 Initial version
*       dc     02/15/21 align driver to curent specification
*       dc     03/18/21 New model parameter list
*       dc     04/18/21 Update trigger and event handlers
*       dc     04/20/21 Doxygen documentation update
* 1.1   dc     07/13/21 Update to common latency requirements
* 1.2   dc     10/29/21 Update doxygen comments
*       dc     11/05/21 Align event handlers
*       dc     11/19/21 Update doxygen documentation
*
* </pre>
* @addtogroup Overview
* @{
******************************************************************************/
/**
* @cond nocomments
*/

#include "xdfemix.h"
#include "xdfemix_hw.h"

/**************************** Macros Definitions ****************************/

/************************** Function Prototypes *****************************/
extern u32 XDfeMix_RdBitField(u32 FieldWidth, u32 FieldOffset, u32 Data);
extern u32 XDfeMix_WrBitField(u32 FieldWidth, u32 FieldOffset, u32 Data,
			      u32 Val);
/**
* @endcond
*/

/****************************************************************************/
/**
*
* Gets interrupt mask status.
*
* @param    InstancePtr Pointer to the Mixer instance.
* @param    Mask Interrupt masks container.
*
****************************************************************************/
void XDfeMix_GetInterruptMask(const XDfeMix *InstancePtr,
			      XDfeMix_InterruptMask *Mask)
{
	u32 Val;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(Mask != NULL);

	Val = XDfeMix_ReadReg(InstancePtr, XDFEMIX_IMR);
	Mask->DUCDDCOverflow =
		XDfeMix_RdBitField(XDFEMIX_DUC_DDC_OVERFLOW_WIDTH,
				   XDFEMIX_DUC_DDC_OVERFLOW_OFFSET, Val);
	Mask->MixerOverflow =
		XDfeMix_RdBitField(XDFEMIX_MIXER_OVERFLOW_WIDTH,
				   XDFEMIX_MIXER_OVERFLOW_OFFSET, Val);
	Mask->CCUpdate =
		XDfeMix_RdBitField(XDFEMIX_CC_UPDATE_TRIGGERED_WIDTH,
				   XDFEMIX_CC_UPDATE_TRIGGERED_OFFSET, Val);
	Mask->CCSequenceError =
		XDfeMix_RdBitField(XDFEMIX_CC_SEQUENCE_ERROR_WIDTH,
				   XDFEMIX_CC_SEQUENCE_ERROR_OFFSET, Val);
}
/****************************************************************************/
/**
*
* Sets interrupt mask.
*
* @param    InstancePtr Pointer to the Mixer instance.
* @param    Mask Interrupt mask flags container.
*           - 0 - does not mask coresponding interrupt
*           - 1 - masks coresponding interrupt
*
****************************************************************************/
void XDfeMix_SetInterruptMask(const XDfeMix *InstancePtr,
			      const XDfeMix_InterruptMask *Mask)
{
	u32 ValIER = 0U;
	u32 ValIDR = 0U;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(Mask != NULL);
	Xil_AssertVoid(Mask->DUCDDCOverflow <= 1U);
	Xil_AssertVoid(Mask->MixerOverflow <= 1U);
	Xil_AssertVoid(Mask->CCUpdate <= 1U);
	Xil_AssertVoid(Mask->CCSequenceError <= 1U);

	if (Mask->DUCDDCOverflow == XDFEMIX_IMR_INTERRUPT) {
		ValIER |= (1U << XDFEMIX_DUC_DDC_OVERFLOW_OFFSET);
	} else {
		ValIDR |= (1U << XDFEMIX_DUC_DDC_OVERFLOW_OFFSET);
	}

	if (Mask->MixerOverflow == XDFEMIX_IMR_INTERRUPT) {
		ValIER |= (1U << XDFEMIX_MIXER_OVERFLOW_OFFSET);
	} else {
		ValIDR |= (1U << XDFEMIX_MIXER_OVERFLOW_OFFSET);
	}

	if (Mask->CCUpdate == XDFEMIX_IMR_INTERRUPT) {
		ValIER |= (1U << XDFEMIX_CC_UPDATE_TRIGGERED_OFFSET);
	} else {
		ValIDR |= (1U << XDFEMIX_CC_UPDATE_TRIGGERED_OFFSET);
	}

	if (Mask->CCSequenceError == XDFEMIX_IMR_INTERRUPT) {
		ValIER |= (1U << XDFEMIX_CC_SEQUENCE_ERROR_OFFSET);
	} else {
		ValIDR |= (1U << XDFEMIX_CC_SEQUENCE_ERROR_OFFSET);
	}

	XDfeMix_WriteReg(InstancePtr, XDFEMIX_IER, ValIER);
	XDfeMix_WriteReg(InstancePtr, XDFEMIX_IDR, ValIDR);
}

/****************************************************************************/
/**
*
* Gets event status.
*
* @param    InstancePtr Pointer to the Mixer instance.
* @param    Status Event status container.
*
****************************************************************************/
void XDfeMix_GetEventStatus(const XDfeMix *InstancePtr, XDfeMix_Status *Status)
{
	u32 Val;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(Status != NULL);

	Val = XDfeMix_ReadReg(InstancePtr, XDFEMIX_ISR);
	Status->DUCDDCOverflow =
		XDfeMix_RdBitField(XDFEMIX_DUC_DDC_OVERFLOW_WIDTH,
				   XDFEMIX_DUC_DDC_OVERFLOW_OFFSET, Val);
	Status->MixerOverflow =
		XDfeMix_RdBitField(XDFEMIX_MIXER_OVERFLOW_WIDTH,
				   XDFEMIX_MIXER_OVERFLOW_OFFSET, Val);
	Status->CCUpdate =
		XDfeMix_RdBitField(XDFEMIX_CC_UPDATE_TRIGGERED_WIDTH,
				   XDFEMIX_CC_UPDATE_TRIGGERED_OFFSET, Val);
	Status->CCSequenceError =
		XDfeMix_RdBitField(XDFEMIX_CC_SEQUENCE_ERROR_WIDTH,
				   XDFEMIX_CC_SEQUENCE_ERROR_OFFSET, Val);
}

/****************************************************************************/
/**
*
* Clears event status.
*
* @param    InstancePtr Pointer to the Mixer instance.
* @param    Status Clear event status container.
*           - 0 - does not clear coresponding event status
*           - 1 - clear coresponding event status
*
****************************************************************************/
void XDfeMix_ClearEventStatus(const XDfeMix *InstancePtr,
			      const XDfeMix_Status *Status)
{
	u32 Data = 0;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(Status != NULL);
	Xil_AssertVoid(Status->DUCDDCOverflow <= 1U);
	Xil_AssertVoid(Status->MixerOverflow <= 1U);
	Xil_AssertVoid(Status->CCUpdate <= 1U);
	Xil_AssertVoid(Status->CCSequenceError <= 1U);

	Data = XDfeMix_WrBitField(XDFEMIX_DUC_DDC_OVERFLOW_WIDTH,
				  XDFEMIX_DUC_DDC_OVERFLOW_OFFSET, Data,
				  Status->DUCDDCOverflow);
	Data = XDfeMix_WrBitField(XDFEMIX_MIXER_OVERFLOW_WIDTH,
				  XDFEMIX_MIXER_OVERFLOW_OFFSET, Data,
				  Status->MixerOverflow);
	Data = XDfeMix_WrBitField(XDFEMIX_CC_UPDATE_TRIGGERED_WIDTH,
				  XDFEMIX_CC_UPDATE_TRIGGERED_OFFSET, Data,
				  Status->CCUpdate);
	Data = XDfeMix_WrBitField(XDFEMIX_CC_SEQUENCE_ERROR_WIDTH,
				  XDFEMIX_CC_SEQUENCE_ERROR_OFFSET, Data,
				  Status->CCSequenceError);
	XDfeMix_WriteReg(InstancePtr, XDFEMIX_ISR, Data);
}

/** @} */
