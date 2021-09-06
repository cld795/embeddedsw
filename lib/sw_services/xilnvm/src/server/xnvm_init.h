/******************************************************************************
* Copyright (c) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file xnvm_init.h
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date        Changes
 * ----- ---- ---------- -------------------------------------------------------
 * 1.0   kal  07/05/2021 Initial release
 *
 * </pre>
 *
 * @note
 *
 ******************************************************************************/
#ifndef XNVM_INIT_H_
#define XNVM_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xplmi_config.h"

#ifndef PLM_NVM_EXCLUDE
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/************************** Function Prototypes ******************************/
void XNvm_Init(void);

#endif /* PLM_NVM_EXCLUDE */

#ifdef __cplusplus
}
#endif

#endif /* XNVM_INIT_H_ */