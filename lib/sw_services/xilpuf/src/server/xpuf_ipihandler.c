/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpuf_ipihandler.c
* @addtogroup xpuf_apis XilPuf Versal APIs
* @{
* @cond xnvm_internal
* This file contains the XilPuf IPI Handler definition.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  kpt  01/04/2022 Initial release
*       kpt  01/31/2022 Removed redundant code in XNvm_EfuseMemCopy
*
* </pre>
*
* @note
* @endcond
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xplmi_config.h"
#ifdef PLM_PUF
#include "xpuf.h"
#include "xpuf_ipihandler.h"
#include "xpuf_defs.h"
#include "xpuf_init.h"
#include "xplmi_dma.h"
#include "xplmi_hw.h"

/************************** Constant Definitions *****************************/

#define XPUF_SYNDROME_ADDRESS		(0xF2008000U) /**< Address to copy PUF syndrome data
                                                       * when the provided SyndromeAddr is 64-bit */

/************************** Function Prototypes *****************************/
static int XPuf_PufRegistration(u32 AddrLow, u32 AddrHigh);
static int XPuf_PufRegeneration(u32 AddrLow, u32 AddrHigh);
static INLINE int XPuf_MemCopy(u64 SourceAddr, u64 DestAddr, u32 Len);

/*************************** Function Definitions *****************************/

/*****************************************************************************/
/**
 * @brief       This function calls respective IPI handler based on the API_ID
 *
 * @param 	    Cmd is pointer to the command structure
 *
 * @return	- XST_SUCCESS - If the handler execution is successful
 * 		    - ErrorCode   - If there is a failure
 *
 ******************************************************************************/
int XPuf_IpiHandler(XPlmi_Cmd *Cmd) {
	volatile int Status = XST_INVALID_PARAM;
	u32 *Pload = Cmd->Payload;

	if (Pload == NULL) {
		goto END;
	}

	switch (Cmd->CmdId & XPUF_API_ID_MASK) {
		case XPUF_API(XPUF_PUF_REGISTRATION):
			Status = XPuf_PufRegistration(Pload[0], Pload[1]);
			break;
		case XPUF_API(XPUF_PUF_REGENERATION):
			Status = XPuf_PufRegeneration(Pload[0], Pload[1]);
			break;
		case XPUF_API(XPUF_PUF_CLEAR_PUF_ID):
			Status = XPuf_ClearPufID();
			break;

		default:
			XPuf_Printf(XPUF_DEBUG_GENERAL, "CMD: INVALID PARAM\r\n");
			break;
	}

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief   This function performs PUF registration
 *
 * @param 	AddrLow		Lower 32 bit address of the
 * 				XPuf_DataAddr structure
 * @param	AddrHigh	Higher 32 bit address of the
 *				XPuf_DataAddr structure
 *
 * @return	- XST_SUCCESS - If the programming is successful
 * 		    - ErrorCode - If there is a failure
 *
 ******************************************************************************/
static int XPuf_PufRegistration(u32 AddrLow, u32 AddrHigh) {
	int Status = XST_FAILURE;
	u64 Addr = ((u64)AddrHigh << 32U) | (u64)AddrLow;
	XPuf_DataAddr PufDataAddr __attribute__((aligned(32U))) = {0U};
	XPuf_Data PufData __attribute__((aligned(32U))) = {0U};

	Status = XPuf_MemCopy(Addr, (u64)(UINTPTR)&PufDataAddr,
			sizeof(PufDataAddr));
	if  (Status != XST_SUCCESS) {
		goto END;
	}

	PufData.ShutterValue = PufDataAddr.ShutterValue;
	PufData.RegMode = PufDataAddr.RegMode;
	PufData.PufOperation = PufDataAddr.PufOperation;
	PufData.GlobalVarFilter = PufDataAddr.GlobalVarFilter;

	Status = XPuf_Registration(&PufData);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	Status = XPuf_MemCopy((u64)(UINTPTR)&PufData.SyndromeData,
		PufDataAddr.SyndromeDataAddr,
		XPUF_MAX_SYNDROME_DATA_LEN_IN_BYTES);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	Status = XPuf_MemCopy((u64)(UINTPTR)&PufData.PufID,
		PufDataAddr.PufIDAddr, XPUF_ID_LEN_IN_BYTES);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	/* Read Chash and Aux */
	XPlmi_Out64(PufDataAddr.AuxAddr, PufData.Aux);
	XPlmi_Out64(PufDataAddr.ChashAddr, PufData.Chash);

	Status = XPuf_GenerateFuseFormat(&PufData);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	Status = XPuf_MemCopy((u64)(UINTPTR)&PufData.EfuseSynData,
		PufDataAddr.EfuseSynDataAddr,
		XPUF_EFUSE_TRIM_SYN_DATA_IN_BYTES);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief   This function performs PUF regeneration
 *
 * @param 	AddrLow		Lower 32 bit address of the
 * 				XPuf_DataAddr structure
 * @param	AddrHigh	Higher 32 bit address of the
 *				XPuf_DataAddr structure
 *
 * @return	- XST_SUCCESS - If the programming is successful
 * 		    - ErrorCode - If there is a failure
 *
 ******************************************************************************/
static int XPuf_PufRegeneration(u32 AddrLow, u32 AddrHigh) {
	int Status = XST_FAILURE;
	u64 Addr = ((u64)AddrHigh << 32U) | (u64)AddrLow;
	XPuf_DataAddr PufDataAddr __attribute__((aligned(32U))) = {0U};
	XPuf_Data PufData __attribute__((aligned(32U))) = {0U};

	Status = XPuf_MemCopy(Addr, (u64)(UINTPTR)&PufDataAddr, sizeof(PufDataAddr));
	if (Status != XST_SUCCESS) {
		goto END;
	}

	PufData.ShutterValue = PufDataAddr.ShutterValue;
	PufData.RegMode = PufDataAddr.RegMode;
	PufData.PufOperation = PufDataAddr.PufOperation;
	PufData.GlobalVarFilter = PufDataAddr.GlobalVarFilter;
	PufData.ReadOption = (XPuf_ReadOption)PufDataAddr.ReadOption;
	PufData.Chash = XPlmi_In64(PufDataAddr.ChashAddr);
	PufData.Aux = XPlmi_In64(PufDataAddr.AuxAddr);
	PufData.SyndromeAddr = PufDataAddr.SyndromeAddr;
	if ((PufData.ReadOption == XPUF_READ_FROM_RAM) &&
		((u32)(PufDataAddr.SyndromeAddr >> 32) != 0U)) {
		Status = XPuf_MemCopy((u64)PufDataAddr.SyndromeAddr,
			(u64)XPUF_SYNDROME_ADDRESS, XPUF_MAX_SYNDROME_DATA_LEN_IN_BYTES);
		if (Status != XST_SUCCESS) {
			goto END;
		}
		PufData.SyndromeAddr = XPUF_SYNDROME_ADDRESS;
	}

	Status = XPuf_Regeneration(&PufData);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	Status = XPuf_MemCopy((u64)(UINTPTR)&PufData.PufID,
		PufDataAddr.PufIDAddr, XPUF_ID_LEN_IN_BYTES);

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function copies word aligned or non word aligned data
 * 		from source address to destination address.
 *
 * @param 	SourceAddr 	From where the buffer data is read
 *
 * @param 	DestAddr 	To which the buffer data is copied
 *
 * @param 	Len 		Length of data to be copied in bytes
 *
 * @return	- XST_SUCCESS - If the copy is successful
 * 		    - XST_FAILURE - If there is a failure
 *
 *****************************************************************************/
static INLINE int XPuf_MemCopy(u64 SourceAddr, u64 DestAddr, u32 Len)
{
	int Status = XST_FAILURE;

	Status = XPlmi_MemCpy64(DestAddr, SourceAddr, Len);

	return Status;
}
#endif