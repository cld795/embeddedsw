/******************************************************************************
* Copyright (c) 2019 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xloader_cmd.c
* @addtogroup xloader_apis XilLoader Versal APIs
* @{
* @cond xloader_internal
* This file contains the xloader commands implementation.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  kc   03/12/2019 Initial release
* 1.01  kc   04/09/2019 Added support to load partial Pdi
* 1.02  har  08/28/2019 Fixed MISRA C violations
* 1.03  bsv  02/27/2020 Added support for delay handoff
*       bsv  03/09/2020 Added CDO features command for xilloader
*       bsv  04/09/2020 Code clean up Xilloader
* 1.04  kc   06/12/2020 Added IPI mask to PDI CDO commands to get
*                       subsystem information
*       kc   07/28/2020 PLM mode is set to configuration during PDI load
*       bsv  07/29/2020 Removed hard coding of DDR back up address
*       bm   08/03/2020 Added LoadReadBackPdi Cmd
*       bsv  08/10/2020 Added subsystem restart support from DDR
*       td   08/19/2020 Fixed MISRA C violations Rule 10.3
*       bm   08/19/2020 Added ImageInfo Cmds
*       bm   09/21/2020 Modified ImageInfo related API calls
*       bm   09/24/2020 Added FuncID parameter in LoadDdrCpyImg
*       bsv  10/13/2020 Code clean up
*       td   10/19/2020 MISRA C Fixes
*       ana  10/19/2020 Added doxygen comments
* 1.05  bm   12/15/2020 Added Update Multiboot command
*       bsv  02/09/2021 Added input param validation for APIs
*       bsv  02/12/2021 Initialize pointer variable to NULL before use
*       bm   02/18/2021 Added const to XLoader_Cmds
*       ma   03/04/2021 Added XLoader_CheckIpiAccess handler for checking
*                       secure access for IPI commands
*       bm   03/16/2021 Added Image Upgrade support
* 1.06  bm   07/16/2021 Added decrypt metaheader support
*       bm   07/30/2021 Updated decrypt metaheader logic to support full PDIs
*       kpt  08/22/2021 Added redundancy to XLoader_CheckIpiAccess
*       bm   08/24/2021 Updated decrypt metaheader command to extract metaheader
*       bm   08/26/2021 Removed XLOADER_PDI_LOAD_COMPLETE write from
*                       extract metaheader command
*       bsv  08/31/2021 Code clean up
*       kpt  09/01/2021 Added local volatile variable to avoid compiler
*                       optimization in XLoader_CheckIpiAccess
* 1.07  bm   10/21/2021 Updated Extract Metaheader command to return data size as
*                       response
*       bm   12/15/2021 Fix error case in Add ImageStore command
*
* </pre>
*
* @note
* @endcond
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xplmi_hw.h"
#include "xloader.h"
#include "xplmi_cmd.h"
#include "xplmi_generic.h"
#include "xplmi_modules.h"
#include "xplmi_wdt.h"
#include "xplmi_event_logging.h"
#include "xplmi.h"
#include "xpm_api.h"
#include "xpm_nodeid.h"
#include "xil_util.h"
#include "xloader_ddr.h"

/************************** Constant Definitions *****************************/

/**
 * @{
 * @cond xloader_internal
 */
/* READBACK Cmd Macros */
#define XLOADER_BUFFER_MAX_SIZE_MASK		(0x7FFFFFFFU)

/* Get Image Info List Macros */
#define XLOADER_NUM_ENTRIES_MASK		(0x0000FFFFU)

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
static XPlmi_Module XPlmi_Loader;

#define XLOADER_CMD_FEATURES_CMD_ID_INDEX	(0U)
#define XLOADER_CMD_DDR_CPY_IMGID_INDEX		(0U)
#define XLOADER_CMD_DDR_CPY_FUNCID_INDEX	(1U)
#define XLOADER_CMD_LOAD_PDI_PDISRC_INDEX	(0U)
#define XLOADER_CMD_LOAD_PDI_PDIADDR_HIGH_INDEX		(1U)
#define XLOADER_CMD_LOAD_PDI_PDIADDR_LOW_INDEX		(2U)
#define XLOADER_CMD_GET_IMG_INFO_IMGID_INDEX		(0U)
#define XLOADER_CMD_GET_IMG_INFO_LIST_DESTADDR_HIGH_INDEX	(0U)
#define XLOADER_CMD_GET_IMG_INFO_LIST_DESTADDR_LOW_INDEX	(1U)
#define XLOADER_CMD_GET_IMG_INFO_LIST_MAXLEN_INDEX	(2U)
#define XLOADER_CMD_READBACK_PDIADDR_HIGH_INDEX		(3U)
#define XLOADER_CMD_READBACK_PDIADDR_LOW_INDEX		(4U)
#define XLOADER_CMD_READBACK_MAXLEN_INDEX		(5U)
#define XLOADER_CMD_MULTIBOOT_BOOTMODE_INDEX		(0U)
#define XLOADER_CMD_MULTIBOOT_IMG_LOCATION_INDEX	(1U)
#define XLOADER_CMD_IMGSTORE_PDIADDR_HIGH_INDEX		(0U)
#define XLOADER_CMD_IMGSTORE_PDIADDR_LOW_INDEX		(1U)
#define XLOADER_CMD_EXTRACT_METAHDR_PDIADDR_HIGH_INDEX	(0U)
#define XLOADER_CMD_EXTRACT_METAHDR_PDIADDR_LOW_INDEX	(1U)
#define XLOADER_CMD_EXTRACT_METAHDR_DESTADDR_HIGH_INDEX	(2U)
#define XLOADER_CMD_EXTRACT_METAHDR_DESTADDR_LOW_INDEX	(3U)
#define XLOADER_CMD_EXTRACT_METAHDR_DEST_SIZE_INDEX	(4U)
#define XLOADER_RESP_CMD_EXEC_STATUS_INDEX	(0U)
#define XLOADER_RESP_CMD_FEATURES_CMD_SUPPORTED	(1U)
#define XLOADER_RESP_CMD_READBACK_PROCESSED_LEN_INDEX	(1U)
#define XLOADER_RESP_CMD_GET_IMG_INFO_UID_INDEX		(1U)
#define XLOADER_RESP_CMD_GET_IMG_INFO_PID_INDEX		(2U)
#define XLOADER_RESP_CMD_GET_IMG_INFO_FUNCID_INDEX		(3U)
#define XLOADER_RESP_CMD_GET_IMG_INFO_LIST_NUM_ENTRIES_INDEX	(1U)
#define XLOADER_RESP_CMD_EXTRACT_METAHDR_SIZE_INDEX	(1U)

#define XLOADER_CMD_MULTIBOOT_PDISRC_MASK		(0xFF00U)
#define XLOADER_CMD_MULTIBOOT_FLASHTYPE_MASK		(0xFU)
#define XLOADER_CMD_MULTIBOOT_PDISRC_SHIFT		(8U)
#define XLOADER_FLASHTYPE_RAW				(0U)
#define XLOADER_FLASHTYPE_FS				(1U)
#define XLOADER_FLASHTYPE_RAW_BP1			(2U)
#define XLOADER_FLASHTYPE_RAW_BP2			(3U)
#define XLOADER_DEFAULT_MULTIBOOT_VAL			(0U)
#define XLOADER_DEFAULT_RAWBOOT_VAL			(0U)
#define XLOADER_SD_FILE_SYSTEM_VAL			(0xF0000000U)

/* Loader command defines */
#define XLOADER_CMD_READBACK_CMD_ID				(0x7U)
#define XLOADER_CMD_UPDATE_MULTIBOOT_CMD_ID	(0x8U)

#define XLOADER_IMG_HDR_TBL_EXPORT_MASK0	(0x00021F7FU)
#define XLOADER_IMG_HDR_EXPORT_MASK0	(0x00003FFBU)
#define XLOADER_PRTN_HDR_EXPORT_MASK0	(0x00001DFFU)
#define XLOADER_DDR_LOW_END_ADDR	(0x7FFFFFFFU)

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/*****************************************************************************/
/**
 * @brief This function checks if a particular Loader Command ID is supported
 * or not. Command ID is the only payload parameter.
 *
 * @param Cmd is pointer to the command structure
 *
 * @return Returns XST_SUCCESS
 *
 *****************************************************************************/
static int XLoader_Features(XPlmi_Cmd *Cmd)
{
	if (Cmd->Payload[XLOADER_CMD_FEATURES_CMD_ID_INDEX] <
		XPlmi_Loader.CmdCnt) {
		Cmd->Response[XLOADER_RESP_CMD_FEATURES_CMD_SUPPORTED] = XLOADER_SUCCESS;
	}
	else {
		Cmd->Response[XLOADER_RESP_CMD_FEATURES_CMD_SUPPORTED] = XLOADER_FAILURE;
	}
	Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = XLOADER_SUCCESS;

	return XST_SUCCESS;
}
/**
 * @}
 * @endcond
 */

/*****************************************************************************/
/**
 * @brief	This function provides load DDR copy image execution.
 * Command payload parameters are
 *	* Img ID - of ddr copied image
 *	* Func ID - to verify with the FuncID of the image copied to DDR
 *
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
static int XLoader_LoadDdrCpyImg(XPlmi_Cmd *Cmd)
{
	int Status = XST_FAILURE;
	u32 ImgId = Cmd->Payload[XLOADER_CMD_DDR_CPY_IMGID_INDEX];
	u32 *FuncID = (Cmd->Len > XLOADER_CMD_DDR_CPY_FUNCID_INDEX) ?
		(&Cmd->Payload[XLOADER_CMD_DDR_CPY_FUNCID_INDEX]) : NULL;
	XilPdi* PdiPtr = BootPdiPtr;

	PdiPtr->IpiMask = Cmd->IpiMask;
	XPlmi_Printf(DEBUG_INFO, "%s \n\r", __func__);

	XPlmi_SetPlmMode(XPLMI_MODE_CONFIGURATION);

	Status = XLoader_RestartImage(ImgId, FuncID);
	if (Status != XST_SUCCESS) {
		/* Update the error code */
		XPlmi_ErrMgr(Status);
	}

	XPlmi_SetPlmMode(XPLMI_MODE_OPERATIONAL);
	Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
	return Status;
}


/*****************************************************************************/
/**
 * @brief	This function provides PDI execution from DDR.
 *  Command payload parameters are
 *	- PdiSrc - Boot Mode values, DDR, PCIe
 *	- PdiAddr - 64bit PDI address located in the Source
 *
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
static int XLoader_LoadSubsystemPdi(XPlmi_Cmd *Cmd)
{
	int Status = XST_FAILURE;
	PdiSrc_t PdiSrc;
	u64 PdiAddr;
	XilPdi* PdiPtr = &SubsystemPdiIns;

	XPlmi_Printf(DEBUG_DETAILED, "%s \n\r", __func__);

	/* Store the command fields in resume data */
	PdiSrc = (PdiSrc_t)(Cmd->Payload[XLOADER_CMD_LOAD_PDI_PDISRC_INDEX]);
	PdiAddr = (u64)Cmd->Payload[XLOADER_CMD_LOAD_PDI_PDIADDR_HIGH_INDEX];
	PdiAddr = ((u64)(Cmd->Payload[XLOADER_CMD_LOAD_PDI_PDIADDR_LOW_INDEX]) |
			(PdiAddr << 32U));

	/* Input parameter validation */
	if (!((PdiSrc == XLOADER_PDI_SRC_QSPI24) ||
		(PdiSrc == XLOADER_PDI_SRC_QSPI32) ||
		(PdiSrc == XLOADER_PDI_SRC_OSPI) ||
		(PdiSrc == XLOADER_PDI_SRC_DDR))) {
		Status = XPlmi_UpdateStatus(
			XLOADER_ERR_UNSUPPORTED_SUBSYSTEM_PDISRC, (int)PdiSrc);
		goto END;
	}

	XPlmi_Printf(DEBUG_INFO, "Subsystem PDI Load: Started\n\r");

	PdiPtr->PdiType = XLOADER_PDI_TYPE_PARTIAL;
	PdiPtr->IpiMask = Cmd->IpiMask;
	Status = XLoader_LoadPdi(PdiPtr, PdiSrc, PdiAddr);
	if (Status != XST_SUCCESS) {
		/* Update the error code */
		XPlmi_ErrMgr(Status);
		goto END;
	}

	XPlmi_Printf(DEBUG_GENERAL, "Subsystem PDI Load: Done\n\r");

END:
	Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function provides ImageInfo stored in ImageInfoTbl for a
 *  given NodeID.
 *  Command payload parameters are
 *	- Node ID
 *  Command Response parameters are
 * 	- Unique ID
 * 	- Parent Unique ID
 * 	- Function ID
 *
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
static int XLoader_GetImageInfo(XPlmi_Cmd *Cmd)
{
	int Status = XST_FAILURE;
	const XLoader_ImageInfo *ImageInfo = NULL;

	if (Cmd->Payload[XLOADER_CMD_GET_IMG_INFO_IMGID_INDEX] ==
		XLOADER_INVALID_IMG_ID) {
		Status = XLOADER_ERR_INVALID_IMGID;
		XPlmi_Printf(DEBUG_GENERAL, "Invalid ImgID\n\r");
		goto END;
	}

	ImageInfo = XLoader_GetImageInfoEntry(
		Cmd->Payload[XLOADER_CMD_GET_IMG_INFO_IMGID_INDEX]);
	if (ImageInfo == NULL) {
		Status = XLOADER_ERR_NO_VALID_IMG_FOUND;
		goto END;
	}
	if ((ImageInfo->ImgID != Cmd->Payload[XLOADER_CMD_GET_IMG_INFO_IMGID_INDEX])
		|| (ImageInfo->ImgID == XLOADER_INVALID_IMG_ID)) {
		Status = XLOADER_ERR_NO_VALID_IMG_FOUND;
		XPlmi_Printf(DEBUG_GENERAL, "No Valid Image Entry Found\n\r");
		goto END;
	}

	Cmd->Response[XLOADER_RESP_CMD_GET_IMG_INFO_UID_INDEX] = ImageInfo->UID;
	Cmd->Response[XLOADER_RESP_CMD_GET_IMG_INFO_PID_INDEX] = ImageInfo->PUID;
	Cmd->Response[XLOADER_RESP_CMD_GET_IMG_INFO_FUNCID_INDEX] =
		ImageInfo->FuncID;

	Status = XST_SUCCESS;

END:
	Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function does nothing but provides a command which is
 * used by bootgen to set the Image Header IDs
 *
 *  Command payload parameters are
 *	- Node ID
 *	- Unique ID
 *	- Parent Unique ID
 *	- Function ID
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS
 *
 *****************************************************************************/
static int XLoader_SetImageInfo(XPlmi_Cmd *Cmd)
{
	/* This acts as a placeholder for the implementation done by bootgen */
	Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)XST_SUCCESS;
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * @brief	This function gets ImageInfo Table and copies it into the
 * buffer address passed in the command.
 *
 *  Command payload parameters are
 *	- 64-bit Buffer Address
 *	- Max Size
 *
 *  Command Response parameters are
 * 	- Number of Entries Returned
 *
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS on success, error code on failure
 *
 *****************************************************************************/
static int XLoader_GetImageInfoList(XPlmi_Cmd *Cmd)
{
	int Status = XST_FAILURE;
	u64 DestAddr;
	u32 MaxSize;
	u32 NumEntries = 0U;

	DestAddr =
		(u64)Cmd->Payload[XLOADER_CMD_GET_IMG_INFO_LIST_DESTADDR_HIGH_INDEX];
	DestAddr =
		(((u64)Cmd->Payload[XLOADER_CMD_GET_IMG_INFO_LIST_DESTADDR_LOW_INDEX]) |
		(DestAddr << 32U));
	MaxSize = (u32)(Cmd->Payload[XLOADER_CMD_GET_IMG_INFO_LIST_MAXLEN_INDEX] &
			XLOADER_BUFFER_MAX_SIZE_MASK);
	Status = XLoader_LoadImageInfoTbl(DestAddr, MaxSize, &NumEntries);
	if (Status != XST_SUCCESS) {
		goto END;
	}
	Cmd->Response[XLOADER_RESP_CMD_GET_IMG_INFO_LIST_NUM_ENTRIES_INDEX] =
		NumEntries & XLOADER_NUM_ENTRIES_MASK;

END:
	Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function provides loading of ReadBack PDI overriding
 *  destination address of readback data.
 *  Command payload parameters are
 *	- PdiSrc - Boot Mode values, DDR
 *	- PdiAddr - 64bit PDI address located in the Source
 *	- ReadbackDdrDestAddr - 64bit DDR destination address
 *	- MaxSize - MaxSize of the buffer present at destination address
 *
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
static int XLoader_LoadReadBackPdi(XPlmi_Cmd *Cmd)
{
	int Status = XST_FAILURE;
	XPlmi_ReadBackProps ReadBack;
	XPlmi_ReadBackProps DefaultReadBack = {
		XPLMI_READBACK_DEF_DST_ADDR, 0U, 0U
	};

	ReadBack.DestAddr = (u64)Cmd->Payload[XLOADER_CMD_READBACK_PDIADDR_HIGH_INDEX];
	ReadBack.DestAddr = ((u64)(Cmd->Payload[XLOADER_CMD_READBACK_PDIADDR_LOW_INDEX]) |
		(ReadBack.DestAddr << 32U));
	ReadBack.MaxSize = Cmd->Payload[XLOADER_CMD_READBACK_MAXLEN_INDEX] &
		XLOADER_BUFFER_MAX_SIZE_MASK;
	ReadBack.ProcessedLen = 0U;

	Status = XPlmi_SetReadBackProps(&ReadBack);
	if (Status != XST_SUCCESS) {
		Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
		goto END;
	}
	Status = XLoader_LoadSubsystemPdi(Cmd);

	Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
	Status = XPlmi_GetReadBackPropsValue(&ReadBack);
	if (Status != XST_SUCCESS) {
		if (Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] == (u32)XST_SUCCESS) {
			Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
		}
		goto END;
	}
	Cmd->Response[XLOADER_RESP_CMD_READBACK_PROCESSED_LEN_INDEX] =
		ReadBack.ProcessedLen;
END:
	Status = XPlmi_SetReadBackProps(&DefaultReadBack);
	if (Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] == (u32)XST_SUCCESS) {
		Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
	}
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function updates multiboot register value during run-time
 *  Command payload parameters are
 *	- BootMode[15:8] - Boot Mode value
 *	- FlashType[3:0] - Type of Flash
 *	- Image Location - Location of Image in the boot device
 *
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
static int XLoader_UpdateMultiboot(XPlmi_Cmd *Cmd)
{
	int Status = XST_FAILURE;
	u32 ImageLocation;
	u32 MultiBootVal = XLOADER_DEFAULT_MULTIBOOT_VAL;
	u32 RawBootVal = XLOADER_DEFAULT_RAWBOOT_VAL;
	u8 FlashType;
	PdiSrc_t PdiSrc;

	FlashType = (u8)(Cmd->Payload[XLOADER_CMD_MULTIBOOT_BOOTMODE_INDEX] &
				XLOADER_CMD_MULTIBOOT_FLASHTYPE_MASK);
	PdiSrc = (PdiSrc_t)((Cmd->Payload[XLOADER_CMD_MULTIBOOT_BOOTMODE_INDEX] &
			XLOADER_CMD_MULTIBOOT_PDISRC_MASK) >>
			XLOADER_CMD_MULTIBOOT_PDISRC_SHIFT);
	ImageLocation = Cmd->Payload[XLOADER_CMD_MULTIBOOT_IMG_LOCATION_INDEX];

	if ((PdiSrc == XLOADER_PDI_SRC_SD0) || (PdiSrc == XLOADER_PDI_SRC_SD1)
		|| (PdiSrc == XLOADER_PDI_SRC_SD1_LS)) {
		if (FlashType == XLOADER_FLASHTYPE_RAW) {
			RawBootVal = XLOADER_SD_RAWBOOT_VAL;
		}
		else if (FlashType == XLOADER_FLASHTYPE_FS) {
			RawBootVal = XLOADER_SD_FILE_SYSTEM_VAL;
		}
		else {
			XPlmi_Printf(DEBUG_GENERAL, "Unsupported Flash Type\n");
			Status = (int)XLOADER_ERR_UNSUPPORTED_MULTIBOOT_FLASH_TYPE;
			goto END;
		}
	}
	else if (PdiSrc == XLOADER_PDI_SRC_EMMC1) {
		if (FlashType == XLOADER_FLASHTYPE_RAW) {
			RawBootVal = XLOADER_SD_RAWBOOT_VAL;
		}
		else if (FlashType == XLOADER_FLASHTYPE_FS) {
			RawBootVal = XLOADER_SD_FILE_SYSTEM_VAL;

		}
		else if (FlashType == XLOADER_FLASHTYPE_RAW_BP1) {
			RawBootVal = XLOADER_EMMC_BP1_RAW_VAL;
		}
		else if (FlashType == XLOADER_FLASHTYPE_RAW_BP2) {
			RawBootVal = XLOADER_EMMC_BP2_RAW_VAL;
		}
		else {
			XPlmi_Printf(DEBUG_GENERAL, "Unsupported Flash Type\n");
			Status = (int)XLOADER_ERR_UNSUPPORTED_MULTIBOOT_FLASH_TYPE;
			goto END;
		}
	}
	else {
		 if ((PdiSrc != XLOADER_PDI_SRC_QSPI24) &&
			(PdiSrc != XLOADER_PDI_SRC_QSPI32) &&
			(PdiSrc != XLOADER_PDI_SRC_OSPI)) {
			XPlmi_Printf(DEBUG_GENERAL, "Unsupported PdiSrc\n");
			Status = (int)XLOADER_ERR_UNSUPPORTED_MULTIBOOT_PDISRC;
			goto END;
		}
	}

	if (RawBootVal == XLOADER_SD_FILE_SYSTEM_VAL) {
		if (ImageLocation < XLOADER_SD_MAX_BOOT_FILES_LIMIT) {
			MultiBootVal = ImageLocation;
		}
		else {
			XPlmi_Printf(DEBUG_GENERAL, "Unsupported Boot File Num\n");
			Status = (int)XLOADER_ERR_UNSUPPORTED_FILE_NUM;
			goto END;
		}
	}
	else {
		if ((ImageLocation % XLOADER_IMAGE_SEARCH_OFFSET) == 0U) {
			MultiBootVal = ImageLocation / XLOADER_IMAGE_SEARCH_OFFSET;
		}
		else {
			XPlmi_Printf(DEBUG_GENERAL, "Unsupported Image Location\n");
			Status = (int)XLOADER_ERR_UNSUPPORTED_MULTIBOOT_OFFSET;
			goto END;
		}
	}

	MultiBootVal = (RawBootVal & XLOADER_SD_RAWBOOT_MASK) |
			(MultiBootVal & XLOADER_MULTIBOOT_OFFSET_MASK);
	XPlmi_Out32(PMC_GLOBAL_PMC_MULTI_BOOT, MultiBootVal);
	Status = XST_SUCCESS;

END:
	Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function adds PdiAddress to ImageStore PdiList
 *  Command payload parameters are
 *	- High PdiAddr - Upper 32 bit value of PdiAddr
 *	- Low PdiAddr - Lower 32 bit value of PdiAddr
 *
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
static int XLoader_AddImageStorePdi(XPlmi_Cmd *Cmd)
{
	int Status = XST_FAILURE;
	u64 PdiAddr = (u64)Cmd->Payload[XLOADER_CMD_IMGSTORE_PDIADDR_HIGH_INDEX];
	XLoader_ImageStore *PdiList = XLoader_GetPdiList();
	u8 Index;

	if (PdiList->Count >= XLOADER_MAX_PDI_LIST) {
		Status = XPlmi_UpdateStatus(XLOADER_ERR_PDI_LIST_FULL, 0);
		goto END;
	}
	PdiAddr = ((u64)Cmd->Payload[XLOADER_CMD_IMGSTORE_PDIADDR_LOW_INDEX]) |
			(PdiAddr << 32U);
	for (Index = 0U; Index < PdiList->Count; Index++) {
		if (PdiList->PdiAddr[Index] == PdiAddr) {
			break;
		}
	}
	if (Index < PdiList->Count) {
		Status = XPlmi_UpdateStatus(XLOADER_ERR_PDI_ADDR_EXISTS, 0);
		goto END;
	}

	Status = XLoader_DdrInit(XLOADER_PDI_SRC_DDR);
	if (Status != XST_SUCCESS) {
		goto END;
	}
	PdiList->PdiAddr[Index] = PdiAddr;
	PdiList->Count++;

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function removes PdiAddress from ImageStore PdiList
 *  Command payload parameters are
 *	- High PdiAddr - Upper 32 bit value of PdiAddr
 *	- Low PdiAddr - Lower 32 bit value of PdiAddr
 *
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
static int XLoader_RemoveImageStorePdi(XPlmi_Cmd *Cmd)
{
	int Status = XST_FAILURE;
	u64 PdiAddr = (u64)Cmd->Payload[XLOADER_CMD_IMGSTORE_PDIADDR_HIGH_INDEX];
	u8 Index;
	u8 ShiftIdx;
	XLoader_ImageStore *PdiList = XLoader_GetPdiList();

	if (PdiList->Count == 0U) {
		Status = XPlmi_UpdateStatus(XLOADER_ERR_PDI_LIST_EMPTY, 0);
		goto END;
	}

	PdiAddr = ((u64)Cmd->Payload[XLOADER_CMD_IMGSTORE_PDIADDR_LOW_INDEX]) |
			(PdiAddr << 32U);
	/* If PdiAddr matches with any entry in the List, remove it */
	for (Index = 0U; Index < PdiList->Count; Index++) {
		if (PdiList->PdiAddr[Index] == PdiAddr) {
			for (ShiftIdx = Index; ShiftIdx < (PdiList->Count - 1U);
				ShiftIdx++) {
				PdiList->PdiAddr[ShiftIdx] =
					PdiList->PdiAddr[ShiftIdx + 1U];
			}
			break;
		}
	}
	if (Index == PdiList->Count) {
		Status = XPlmi_UpdateStatus(
				XLOADER_ERR_PDI_ADDR_NOT_FOUND, 0);
		goto END;
	}
	PdiList->Count--;
	if (PdiList->Count == 0U) {
		Status = XLoader_DdrRelease();
		if (Status != XST_SUCCESS) {
			goto END;
		}
	}
	Status = XST_SUCCESS;

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function zeroizes non-exportable fields of a buffer
 *
 * @param	Buffer is the pointer to the metaheader buffer
 * @param	Mask is the exportable mask which is used to zeroise the
 * non-exportable fields of the buffer
 * @param	Size is the size of the buffer in bytes
 *
 * @return	None
 *
 *****************************************************************************/
static void XLoader_GetExportableBuffer(u32 *Buffer, u32 Mask, u32 Size)
{
	u32 Index;

	Size >>= XPLMI_WORD_LEN_SHIFT;
	for (Index = 0U; Index < Size; Index++) {
		if ((Mask & 0x1U) == 0U) {
			Buffer[Index] = 0U;
		}
		Mask >>= 1U;
	}
}

/*****************************************************************************/
/**
 * @brief	This function decrypts the metaheader during run-time and exports
 * it to the user specified location.
 *
 *  Command payload parameters are:
 *	- Source Buffer Low Address
 *	- Source Buffer High Address
 *	- Source Buffer Size
 *	- Destination Buffer Low Address
 *	- Destination Buffer High Address
 *	- Destination Buffer Size
 *
 * @param	Cmd is pointer to the command structure
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 *****************************************************************************/
static int XLoader_ExtractMetaheader(XPlmi_Cmd *Cmd)
{
	int Status = XST_FAILURE;
	XilPdi* PdiPtr = &SubsystemPdiIns;
	u64 SrcAddr = (u64)Cmd->Payload[XLOADER_CMD_EXTRACT_METAHDR_PDIADDR_HIGH_INDEX];
	u64 DestAddr = (u64)Cmd->Payload[XLOADER_CMD_EXTRACT_METAHDR_DESTADDR_HIGH_INDEX];
	u32 DestSize = (u32)Cmd->Payload[XLOADER_CMD_EXTRACT_METAHDR_DEST_SIZE_INDEX];
	u32 IdString;
	u32 DataSize;
	u32 TotalDataSize = 0U;
	u64 MetaHdrOfst;
	u32 Index;

	XPlmi_SetPlmMode(XPLMI_MODE_CONFIGURATION);
	SrcAddr = ((u64)Cmd->Payload[XLOADER_CMD_EXTRACT_METAHDR_PDIADDR_LOW_INDEX]) |
			(SrcAddr << 32U);
	DestAddr = ((u64)Cmd->Payload[XLOADER_CMD_EXTRACT_METAHDR_DESTADDR_LOW_INDEX]) |
			(DestAddr << 32U);

	IdString = XPlmi_In64(SrcAddr + XIH_BH_IMAGE_IDENT_OFFSET);
	if (IdString == XIH_BH_IMAGE_IDENT) {
		PdiPtr->PdiType = XLOADER_PDI_TYPE_FULL_METAHEADER;
	}
	else {
		IdString = XPlmi_In64(SrcAddr + SMAP_BUS_WIDTH_LENGTH +
				XIH_IHT_IDENT_STRING_OFFSET);
		if (IdString == XIH_IHT_PPDI_IDENT_VAL) {
			PdiPtr->PdiType = XLOADER_PDI_TYPE_PARTIAL_METAHEADER;
		}
		else {
			Status = XLOADER_ERR_INVALID_PDI_INPUT;
			goto END;
		}
	}

	Status = XPlmi_VerifyAddrRange(SrcAddr, SrcAddr + (XPLMI_WORD_LEN - 1U));
	if (Status != XST_SUCCESS) {
		Status = XLOADER_ERR_INVALID_METAHEADER_SRC_ADDR;
		goto END;
	}

	/* Check if Metaheader offset is pointing to a valid location */
	if (PdiPtr->PdiType == XLOADER_PDI_TYPE_FULL_METAHEADER) {
		MetaHdrOfst = SrcAddr + (u64)XPlmi_In64(SrcAddr +
				XIH_BH_META_HDR_OFFSET);
		if ((MetaHdrOfst > XLOADER_DDR_LOW_END_ADDR) &&
			(MetaHdrOfst <= (u64)XPAR_PSV_OCM_RAM_0_S_AXI_HIGHADDR)) {
			Status = XLOADER_ERR_INVALID_METAHEADER_OFFSET;
			goto END;
		}
	}

	Status = XPlmi_VerifyAddrRange(DestAddr, DestAddr + DestSize - 1U);
	if (Status != XST_SUCCESS) {
		Status = XLOADER_ERR_INVALID_METAHEADER_DEST_ADDR;
		goto END;
	}

	PdiPtr->IpiMask = Cmd->IpiMask;
	/* Extract Metaheader using PdiInit */
	XSECURE_TEMPORAL_CHECK(END, Status, XLoader_PdiInit, PdiPtr,
			XLOADER_PDI_SRC_DDR, SrcAddr);

	DataSize = (PdiPtr->MetaHdr.ImgHdrTbl.TotalHdrLen * XPLMI_WORD_LEN) +
			XIH_IHT_LEN;
	if (DestSize < DataSize) {
		Status = XLOADER_ERR_INVALID_METAHDR_BUFF_SIZE;
		goto END;
	}

	/* Zeroize non-exportable fields of image header table */
	XLoader_GetExportableBuffer((u32 *)&PdiPtr->MetaHdr.ImgHdrTbl,
		XLOADER_IMG_HDR_TBL_EXPORT_MASK0, XIH_IHT_LEN);
	/* Zeroize non-exportable fields of image headers */
	for (Index = 0U; Index < PdiPtr->MetaHdr.ImgHdrTbl.NoOfImgs; Index++) {
		XLoader_GetExportableBuffer((u32 *)&PdiPtr->MetaHdr.ImgHdr[Index],
			XLOADER_IMG_HDR_EXPORT_MASK0, XIH_IH_LEN);
	}
	/* Zeroize non-exportable fields of partition headers */
	for (Index = 0U; Index < PdiPtr->MetaHdr.ImgHdrTbl.NoOfPrtns; Index++) {
		XLoader_GetExportableBuffer((u32 *)&PdiPtr->MetaHdr.PrtnHdr[Index],
			XLOADER_PRTN_HDR_EXPORT_MASK0, XIH_PH_LEN);
	}

	/* Copy image header table to destination address*/
	DataSize = XIH_IHT_LEN;
	Status = XPlmi_DmaXfr((u64)(UINTPTR)&PdiPtr->MetaHdr.ImgHdrTbl, DestAddr,
			 DataSize / XPLMI_WORD_LEN, XPLMI_PMCDMA_0);
	if (Status != XST_SUCCESS) {
		goto END;
	}
	TotalDataSize += DataSize;
	DestAddr += DataSize;
	/* Copy image headers to destination address*/
	DataSize = XIH_IH_LEN * PdiPtr->MetaHdr.ImgHdrTbl.NoOfImgs;
	Status = XPlmi_DmaXfr((u64)(UINTPTR)PdiPtr->MetaHdr.ImgHdr, DestAddr,
			DataSize / XPLMI_WORD_LEN, XPLMI_PMCDMA_0);
	if (Status != XST_SUCCESS) {
		goto END;
	}
	TotalDataSize += DataSize;
	DestAddr += DataSize;
	/* Copy partition headers to destination address*/
	DataSize = XIH_PH_LEN * PdiPtr->MetaHdr.ImgHdrTbl.NoOfPrtns;
	Status = XPlmi_DmaXfr((u64)(UINTPTR)PdiPtr->MetaHdr.PrtnHdr, DestAddr,
			DataSize / XPLMI_WORD_LEN, XPLMI_PMCDMA_0);
	if (Status != XST_SUCCESS) {
		goto END;
	}
	TotalDataSize += DataSize;
	XPlmi_Printf(DEBUG_GENERAL, "Extracted Metaheader Successfully\n\r");

END:
	XPlmi_SetPlmMode(XPLMI_MODE_OPERATIONAL);
	Cmd->Response[XLOADER_RESP_CMD_EXEC_STATUS_INDEX] = (u32)Status;
	Cmd->Response[XLOADER_RESP_CMD_EXTRACT_METAHDR_SIZE_INDEX] = TotalDataSize;

	return Status;
}

/**
 * @{
 * @cond xloader_internal
 */

/*****************************************************************************/
/**
 * @brief	This function checks if the IPI command is accessible or not
 *
 * @param	CmdId is the Command ID
 * @param	IpiReqType is the IPI command request type
 *
 * @return	XST_SUCCESS on success and XST_FAILURE on failure
 *
 *****************************************************************************/
static int XLoader_CheckIpiAccess(u32 CmdId, u32 IpiReqType)
{
	int Status = XST_FAILURE;
	volatile u8 ModuleCmdId = (u8)(CmdId & XPLMI_PLM_GENERIC_CMD_ID_MASK);
	volatile u8 ModuleCmdIdTmp = (u8)(CmdId & XPLMI_PLM_GENERIC_CMD_ID_MASK);
	volatile u32 IpiRequestType = IpiReqType;
	volatile u32 IpiRequestTypeTmp = IpiReqType;

	/* Secure check for Loader IPI commands */
	if (((ModuleCmdId == XLOADER_CMD_READBACK_CMD_ID) &&
		(ModuleCmdIdTmp == XLOADER_CMD_READBACK_CMD_ID)) ||
		((ModuleCmdId == XLOADER_CMD_UPDATE_MULTIBOOT_CMD_ID) &&
		(ModuleCmdIdTmp == XLOADER_CMD_UPDATE_MULTIBOOT_CMD_ID))) {
		if ((XPLMI_CMD_SECURE == IpiRequestType) &&
			(XPLMI_CMD_SECURE == IpiRequestTypeTmp)) {
			Status = XST_SUCCESS;
		}
	}
	else if (((ModuleCmdId != XLOADER_CMD_READBACK_CMD_ID) &&
		(ModuleCmdIdTmp != XLOADER_CMD_READBACK_CMD_ID)) &&
		((ModuleCmdId != XLOADER_CMD_UPDATE_MULTIBOOT_CMD_ID) &&
		(ModuleCmdIdTmp != XLOADER_CMD_UPDATE_MULTIBOOT_CMD_ID))) {
		if (ModuleCmdId == ModuleCmdIdTmp) {
			Status = XST_SUCCESS;
		}
	}
	else {
		Status = (int)XLOADER_ERR_GLITCH_DETECTED;
	}

	return Status;
}

/*****************************************************************************/
/**
 * @brief	Contains the array of PLM loader commands
 *
 *****************************************************************************/
static const XPlmi_ModuleCmd XLoader_Cmds[] =
{
	XPLMI_MODULE_COMMAND(XLoader_Features),
	XPLMI_MODULE_COMMAND(XLoader_LoadSubsystemPdi),
	XPLMI_MODULE_COMMAND(XLoader_LoadDdrCpyImg),
	XPLMI_MODULE_COMMAND(XLoader_GetImageInfo),
	XPLMI_MODULE_COMMAND(XLoader_SetImageInfo),
	XPLMI_MODULE_COMMAND(XLoader_GetImageInfoList),
	XPLMI_MODULE_COMMAND(XLoader_ExtractMetaheader),
	XPLMI_MODULE_COMMAND(XLoader_LoadReadBackPdi),
	XPLMI_MODULE_COMMAND(XLoader_UpdateMultiboot),
	XPLMI_MODULE_COMMAND(XLoader_AddImageStorePdi),
	XPLMI_MODULE_COMMAND(XLoader_RemoveImageStorePdi)
};

/*****************************************************************************/
/**
 * @brief	Contains the module ID and loader commands array
 *
 *****************************************************************************/
static XPlmi_Module XPlmi_Loader =
{
	XPLMI_MODULE_LOADER_ID,
	XLoader_Cmds,
	XPLMI_ARRAY_SIZE(XLoader_Cmds),
	XLoader_CheckIpiAccess,
};

/*****************************************************************************/
/**
 * @brief	This function registers the PLM Loader commands to the PLMI.
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
void XLoader_CmdsInit(void)
{
	XPlmi_ModuleRegister(&XPlmi_Loader);
}

/**
 * @}
 * @endcond
 */

/** @} */
