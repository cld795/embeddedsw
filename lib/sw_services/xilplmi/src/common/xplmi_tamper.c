/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xplmi_tamper.c
*
* This file contains the tamper response processing routines
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.0   ma   07/08/2022 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xplmi_tamper.h"
#include "xplmi_err_common.h"
#include "xplmi_status.h"
#include "xplmi_hw.h"
#include "xplmi.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
#define PMC_ANALOG_GD_CTRL_REG		(0xF1160000U)
					/**< PMC_ANALOG base address */
#define PMC_ANALOG_GD0_RST_STATUS_REG_MASK	(0x200U)
					/**< Glitch detector0 status mask */
#define PMC_ANALOG_GD1_RST_STATUS_REG_MASK	(0x2000000U)
					/**< Glitch detector1 status mask */
#define PMC_ANALOG_GD_STATUS 		(PMC_ANALOG_GD0_RST_STATUS_REG_MASK | \
					 PMC_ANALOG_GD1_RST_STATUS_REG_MASK)
					/**< Glitch detector status mask */

/************************** Function Prototypes ******************************/
static void XPlmi_PmcApbErrorHandler(const u32 ErrorNodeId,
		const u32 ErrorMask);
/************************** Variable Definitions *****************************/

/************************** Function Definitions *****************************/

/*****************************************************************************/
/**
 * @brief	This function processes the tamper response
 *
 * @param   TamperResp is the tamper response
 *
 * @return  Error code if secure lockdown proc execution fails
 *
 *****************************************************************************/
int XPlmi_ProcessTamperResponse(u32 TamperResp)
{
	int Status = XST_FAILURE;

	if ((TamperResp & XPLMI_RTCFG_TAMPER_RESP_SLD_0_1_MASK) != 0x0U) {
		/* Execute secure lockdown proc */
		Status = XPlmi_ExecuteProc(XPLMI_SLD_PROC_ID);
		if (Status != XST_SUCCESS) {
			XPlmi_Printf(DEBUG_GENERAL, "Secure Lockdown failed with 0x%x "
					"error\r\n", Status);
			goto END;
		}
	}

	/**
	 * Configure TAMPER_RESP_0 with the received response
	 */
	Xil_Out32(PMC_GLOBAL_TAMPER_RESP_0, TamperResp);

	/**
	 * Trigger software tamper event to ROM to execute lockdown
	 * for PMC
	 */
	Xil_Out32(PMC_GLOBAL_TAMPER_TRIG, PMC_GLOBAL_TAMPER_TRIG_VAL);

	/**
	 * Wait forever; ROM to complete secure lockdown
	 */
	while(1U) {
		;
	}

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function registers the handler for tamper interrupt
 *
 * @return
 *	-	XST_SUCCESS - On success
 *	-	XPLMI_INVALID_ERROR_ID - On invalid ID
 *	-	XPLMI_INVALID_ERROR_HANDLER - On Null handler
 *
 *****************************************************************************/
int XPlmi_RegisterTamperIntrHandler(void)
{
	int Status = XST_FAILURE;

	/**
	 * Register handler
	 */
	Status = XPlmi_EmSetAction(XIL_NODETYPE_EVENT_ERROR_PMC_ERR2,
			XIL_EVENT_ERROR_MASK_PMCAPB, XPLMI_EM_ACTION_CUSTOM,
			XPlmi_PmcApbErrorHandler);
	if(Status != XST_SUCCESS) {
		goto END;
	}

	/**
	 * Enable tamper interrupt in PMC GLOBAL
	 */
	Xil_Out32(PMC_GLOBAL_IER, PMC_GLOBAL_TAMPER_INT_MASK);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This is the handler for tamper interrupt
 *
 * @param	ErrorNodeId - Node Identifier
 * @param	ErrorMask   - Mask Identifier
 *
 * @return  None
 *
 *****************************************************************************/
static void XPlmi_PmcApbErrorHandler(const u32 ErrorNodeId,
		const u32 ErrorMask)
{
	volatile u32 TamperResp = XPLMI_RTCFG_TAMPER_RESP_SLD_0_1_MASK;
	volatile u32 TamperRespTmp = XPLMI_RTCFG_TAMPER_RESP_SLD_0_1_MASK;
	volatile u32 IsrVal = PMC_GLOBAL_TAMPER_INT_MASK;
	volatile u32 IsrValTmp = PMC_GLOBAL_TAMPER_INT_MASK;

	(void)ErrorNodeId;
	(void)ErrorMask;

	/**
	 * Check the reason for interrupt
	 */
	IsrVal = Xil_In32(PMC_GLOBAL_ISR);
	IsrValTmp = Xil_In32(PMC_GLOBAL_ISR);
	if (((IsrVal & PMC_GLOBAL_TAMPER_INT_MASK) ==
			PMC_GLOBAL_TAMPER_INT_MASK) ||
		((IsrValTmp & PMC_GLOBAL_TAMPER_INT_MASK) ==
				PMC_GLOBAL_TAMPER_INT_MASK)) {
		TamperResp = Xil_In32(XPLMI_RTCFG_TAMPER_RESP);
		TamperRespTmp = Xil_In32(XPLMI_RTCFG_TAMPER_RESP);
		if (((TamperResp & XPLMI_RTCFG_TAMPER_RESP_SLD_0_1_MASK) != 0x0U) ||
			((TamperRespTmp & XPLMI_RTCFG_TAMPER_RESP_SLD_0_1_MASK) != 0x0U)) {
			(void)XPlmi_ProcessTamperResponse(TamperResp);
		} else {
			XPlmi_Printf(DEBUG_GENERAL, "Warning: Invalid Tamper Response. "
					"Configured Tamper Response at RTCA: 0x%x\r\n"
					"For SYSTEM_INERRUPT response, user need to register for "
					"notification of PMC_APB error\r\n",
					TamperResp);
		}
	} else {
		XPlmi_Printf(DEBUG_GENERAL, "Received PMC_APB interrupt is other than "
				"tamper interrupt. PMC_GLOBAL_ISR: 0x%x\r\n", IsrVal);
	}

	/**
	 * Clear the interrupt source
	 */
	Xil_UtilRMW32(PMC_ANALOG_GD_CTRL_REG, PMC_ANALOG_GD_STATUS,
			PMC_ANALOG_GD_STATUS);
	Xil_Out32(PMC_GLOBAL_ISR, PMC_GLOBAL_TAMPER_INT_MASK);
	Xil_UtilRMW32(PMC_ANALOG_GD_CTRL_REG, PMC_ANALOG_GD_STATUS, 0U);

	/**
	 * Once the interrupt is received, PLM disables the interrupt and
	 * calls the handler. So it is necessary to re-enable the interrupt.
	 */
	(void)XPlmi_EmSetAction(XIL_NODETYPE_EVENT_ERROR_PMC_ERR2,
			XIL_EVENT_ERROR_MASK_PMCAPB, XPLMI_EM_ACTION_CUSTOM,
			XPlmi_PmcApbErrorHandler);
}