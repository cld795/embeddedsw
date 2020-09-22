/******************************************************************************
* Copyright (c) 2019 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/******************************************************************************/
/**
*
* @file xnvm_efuse_hw.h
*
* This file contains NVM library eFUSE controller register definitions
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- ---------- --------------------------------------------------------
* 1.0   kal  08/16/2019 Initial release
* 2.0	kal  02/27/2019	Added eFuse rows, Cache offsets
* 2.1   kal  07/09/2020 Replaced hardcoded CLK_REF_FREQ to the
*                       XPAR_PSU_PSS_REF_CLK_FREQ_HZ
*	am   08/19/2020 Resolved MISRA C violations.
*	kal  09/03/2020 Fixed Security CoE review comments
*
* </pre>
*
* @note
*
*******************************************************************************/
#ifndef XNVM_EFUSE_HW_H
#define XNVM_EFUSE_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Include Files *********************************/
#include "xil_io.h"
#include "xparameters.h"

/*************************** Constant Definitions *****************************/

/* CRP base address definition */
#define XNVM_CRP_BASE_ADDR				(0xF1260000U)

/* CRP eFUSE Clock Control Register Map */
#define XNVM_CRP_EFUSE_REF_CLK_REG_OFFSET		(0x00000134U)

#define XNVM_CRP_EFUSE_REF_CLK_SELSRC_SHIFT		(2U)
#define XNVM_CRP_EFUSE_REF_CLK_IN			(0x01 <<\
					XNVM_CRP_EFUSE_REF_CLK_SELSRC_SHIFT)

#define XNVM_CRP_EFUSE_REF_CLK_SELSRC		(XNVM_CRP_EFUSE_REF_CLK_IN)


/* eFUSE Controller base address definition */
#define XNVM_EFUSE_CTRL_BASEADDR			(0xF1240000U)

/* eFUSE Controller Register Map definition */
#define XNVM_EFUSE_WR_LOCK_REG_OFFSET			(0x00000000U)
#define XNVM_EFUSE_CFG_REG_OFFSET			(0x00000004U)
#define XNVM_EFUSE_STATUS_REG_OFFSET			(0x00000008U)
#define XNVM_EFUSE_PGM_ADDR_REG_OFFSET			(0x0000000CU)
#define XNVM_EFUSE_RD_ADDR_REG_OFFSET			(0x00000010U)
#define XNVM_EFUSE_RD_DATA_REG_OFFSET			(0x00000014U)
#define XNVM_EFUSE_TPGM_REG_OFFSET			(0x00000018U)
#define XNVM_EFUSE_TRD_REG_OFFSET			(0x0000001CU)
#define XNVM_EFUSE_TSU_H_PS_REG_OFFSET			(0x00000020U)
#define XNVM_EFUSE_TSU_H_PS_CS_REG_OFFSET		(0x00000024U)
#define XNVM_EFUSE_TRDM_REG_OFFSET			(0x00000028U)
#define XNVM_EFUSE_TSU_H_CS_REG_OFFSET			(0x0000002CU)
#define XNVM_EFUSE_ISR_REG_OFFSET			(0x00000030U)
#define XNVM_EFUSE_CACHE_LOAD_REG_OFFSET		(0x00000040U)
#define XNVM_EFUSE_PGM_LOCK_REG_OFFSET			(0x00000044U)
#define XNVM_EFUSE_AES_CRC_REG_OFFSET			(0x00000048U)
#define XNVM_EFUSE_AES_USR_KEY0_CRC_REG_OFFSET		(0x0000004CU)
#define XNVM_EFUSE_AES_USR_KEY1_CRC_REG_OFFSET		(0x00000050U)
#define XNVM_EFUSE_PD_REG_OFFSET			(0x00000054U)
#define XNVM_EFUSE_TEST_CTRL_REG_OFFSET			(0x00000100U)

/* Register: EFUSE_CTRL_CFG */
#define XNVM_EFUSE_CTRL_CFG_MARGIN_RD_MASK    		(0x00000004U)

/* access_type: ro  */
#define XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_1_CRC_PASS_MASK	(0x00000800U)
#define XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_1_CRC_DONE_MASK	(0x00000400U)
#define XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_0_CRC_PASS_MASK	(0x00000200U)
#define XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_0_CRC_DONE_MASK	(0x00000100U)
#define XNVM_EFUSE_CTRL_STATUS_AES_CRC_PASS_MASK		(0x00000080U)
#define XNVM_EFUSE_CTRL_STATUS_AES_CRC_DONE_MASK		(0x00000040U)

/*  EFUSE_CACHE Base Address */
#define XNVM_EFUSE_CACHE_BASEADDR				(0xF1250000U)

 /* eFUSE Cache Register Map definition */
#define XNVM_EFUSE_CACHE_TBITS0_SVD_OFFSET    			(0x00000000U)
#define XNVM_EFUSE_CACHE_BOOT_ENV_CTRL_OFFSET			(0x00000094U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_OFFSET			(0x000000A0U)
#define XNVM_EFUSE_CACHE_PUF_CHASH_OFFSET			(0x000000A8U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_OFFSET		(0x000000ACU)
#define XNVM_EFUSE_CACHE_SECURITY_MISC_0_OFFSET			(0x000000E4U)
#define XNVM_EFUSE_CACHE_SECURITY_MISC_1_OFFSET			(0x000000E8U)
#define XNVM_EFUSE_CACHE_PPK0_HASH_0_OFFSET			(0x00000100U)
#define XNVM_EFUSE_CACHE_METAHEADER_IV_RANGE_0_OFFSET		(0x00000180U)

/* access_type: ro  */
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_REG_INIT_DIS_1_0_MASK	(0xc0000000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_BOOT_ENV_WR_LK_MASK	(0x10000000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_SEC_LOCK_DBG_DIS_MASK	(0x00600000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_SEC_DEBUG_DIS_MASK	(0x00180000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_DIS_MASK		(0x00040000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_TEST2_DIS_MASK	(0x00020000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_SYN_LK_MASK	(0x00010000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_WR_LK_MASK	(0x00008000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_CRC_LK_MASK	(0x00004000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_WR_LK_MASK	(0x00002000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_CRC_LK_MASK	(0x00001000U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_WR_LK_MASK	(0x00000800U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_CRC_LK_1_0_MASK	(0x00000600U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK2_WR_LK_MASK	(0x00000100U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK1_WR_LK_MASK	(0x00000080U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK0_WR_LK_MASK	(0x00000040U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_DIS_MASK		(0x00000004U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_ERR_OUT_DIS_MASK	(0x00000002U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_DIS_MASK		(0x00000001U)

/* access_type: ro  */
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_REG_INIT_DIS_1_0_SHIFT	(30U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_BOOT_ENV_WR_LK_SHIFT		(28U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_SEC_LOCK_DBG_DIS_1_0_SHIFT	(21U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_SEC_DEBUG_DIS_1_0_SHIFT	(19U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_DIS_SHIFT			(18U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_TEST2_DIS_SHIFT		(17U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PUF_SYN_LK_SHIFT		(16U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_WR_LK_SHIFT		(15U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_CRC_LK_0_SHIFT	(14U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_WR_LK_SHIFT		(13U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_CRC_LK_0_SHIFT	(12U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_WR_LK_SHIFT		(11U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_CRC_LK_1_0_SHIFT		(9U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK2_WR_LK_SHIFT		(8U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK1_WR_LK_SHIFT		(7U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK0_WR_LK_SHIFT		(6U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_DIS_SHIFT		(2U)
#define XNVM_EFUSE_CACHE_SECURITY_CONTROL_JTAG_ERROR_OUT_DIS_SHIFT	(1U)

/* access_type: ro */
#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_REGEN_DIS_MASK	(0x80000000U)
#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_HD_INVLD_MASK		(0x40000000U)
#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_ECC_23_0_MASK		(0x00ffffffU)

#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_PUF_REGEN_DIS_SHIFT		(31U)
#define XNVM_EFUSE_CACHE_PUF_ECC_PUF_CTRL_PUF_HD_INVLD_SHIFT		(30U)

/* access_type: ro */
#define XNVM_EFUSE_CACHE_TBITS0_SVD_EFUSE_0_TBIT_3_0_MASK    	(0xf0000000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ANCHOR_3_MASK    		(0x08000000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ANCHOR_2_MASK    		(0x04000000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_37_PROT_MASK    	(0x00200000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_40_PROT_MASK    	(0x00100000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_42_PROT_MASK    	(0x00080000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_58_PROT_MASK    	(0x00040000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_VID_7_0_MASK		(0x0000ff00U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_SVD_2_0_MASK		(0x00000070U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ANCHOR_1_MASK		(0x00000002U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ANCHOR_0_MASK		(0x00000001U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_43_PROT_MASK 		(0x02000004U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_57_PROT_MASK 		(0x01000008U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_64_87_PROT_MASK		(0x00810000U)
#define XNVM_EFUSE_CACHE_TBITS0_SVD_ROW_96_99_PROT_MASK		(0x00420000U)

/* access_type: ro  */
#define XNVM_EFUSE_CACHE_SECURITY_MISC_0_DEC_EFUSE_ONLY_MASK	(0x0000ffffU)

/* access_type: ro */
#define XNVM_EFUSE_CACHE_MISC_CTRL_GD_HALT_BOOT_EN_1_0_MASK	(0xc0000000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_GD_ROM_MONITOR_EN_MASK	(0x20000000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ERROR_1_0_MASK	(0x00600000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ENV_1_0_MASK	(0x00180000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_CRYPTO_KAT_EN_MASK		(0x00008000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_LBIST_EN_MASK		(0x00004000U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_SAFETY_MISSION_EN_MASK	(0x00000100U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK2_INVLD_1_0_MASK		(0x000000c0U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK1_INVLD_1_0_MASK		(0x00000030U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK0_INVLD_1_0_MASK		(0x0000000cU)

/* access_type: ro  */
#define XNVM_EFUSE_CACHE_MISC_CTRL_GD_HALT_BOOT_EN_1_0_SHIFT	   	(30U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_GD_ROM_MONITOR_EN_SHIFT   		(29U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ERROR_1_0_SHIFT   		(21U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_HALT_BOOT_ENV_1_0_SHIFT   		(19U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_CRYPTO_KAT_EN_SHIFT   		(15U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_LBIST_EN_SHIFT   			(14U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_SAFETY_MISSION_EN_SHIFT   		(8U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK2_INVLD_1_0_SHIFT			(6U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK1_INVLD_1_0_SHIFT			(4U)
#define XNVM_EFUSE_CACHE_MISC_CTRL_PPK0_INVLD_1_0_SHIFT			(2U)

/* EFUSE ROWS */
#define XNVM_EFUSE_TBITS_XILINX_CTRL_ROW		(0U)
#define XNM_EFUSE_GLITCH_ANLG_TRIM_3			(4U)
#define XNVM_EFUSE_DNA_START_ROW			(8U)
#define XNVM_EFUSE_AES_KEY_START_ROW			(12U)
#define XNVM_EFUSE_USER_KEY_0_START_ROW			(20U)
#define XNVM_EFUSE_USER_KEY_1_START_ROW			(28U)
#define XNVM_EFUSE_USER_KEY_1_END_ROW			(35U)
#define XNVM_EFUSE_MISC_CTRL_ROW				(40U)
#define XNVM_EFUSE_PUF_AUX_ROW					(41U)
#define XNVM_EFUSE_PUF_CHASH_ROW				(42U)
#define XNVM_EFUSE_SECURITY_CONTROL_ROW			(43U)
#define XNVM_EFUSE_REVOCATION_ID_0_ROW			(44U)
#define XNVM_EFUSE_DEC_EFUSE_ONLY_ROW			(57U)
#define XNVM_EFUSE_PPK_0_HASH_START_ROW			(64U)
#define XNVM_EFUSE_PPK_1_HASH_START_ROW			(72U)
#define XNVM_EFUSE_PPK_2_HASH_START_ROW			(80U)
#define XNVM_EFUSE_META_HEADER_IV_START_ROW		(96U)
#define XNVM_EFUSE_BLACK_OBFUS_IV_START_ROW		(116U)
#define XNVM_EFUSE_PLM_IV_START_ROW				(119U)
#define XNVM_EFUSE_DATA_PARTITION_IV_START_ROW	(122U)
#define XNVM_EFUSE_PUF_SYN_START_ROW			(129U)
#define XNVM_EFUSE_USER_FUSE_START_ROW			(129U)


/* EFUSE ROW COUNT*/
#define XNVM_EFUSE_AES_KEY_NUM_OF_ROWS			(8U)
#define XNVM_EFUSE_USER_KEY_NUM_OF_ROWS			(8U)
#define XNVM_EFUSE_PPK_HASH_NUM_OF_ROWS			(8U)
#define XNVM_EFUSE_DEC_EFUSE_ONLY_NUM_OF_ROWS		(1U)
#define XNVM_EFUSE_GLITCH_NUM_OF_ROWS			(1U)
#define XNVM_EFUSE_PUF_SYN_DATA_NUM_OF_ROWS		(127U)
#define XNVM_EFUSE_PUF_CHASH_NUM_OF_ROWS		(1U)
#define XNVM_EFUSE_PUF_AUX_NUM_OF_ROWS			(1U)
#define XNVM_EFUSE_IV_NUM_OF_ROWS			(3U)
#define XNVM_EFUSE_DNA_NUM_OF_ROWS			(4U)

/* EFUSE COLUMNS */
#define XNVM_EFUSE_ROW_43_1_PROT_COLUMN			(2U)
#define XNVM_EFUSE_ROW_57_1_PROT_COLUMN			(3U)
#define XNVM_EFUSE_ROW64_87_1_PROT_COLUMN		(16U)
#define XNVM_EFUSE_ROW96_99_1_PROT_COLUMN		(17U)
#define XNVM_EFUSE_ROW_58_PROT_COLUMN			(18U)
#define XNVM_EFUSE_ROW_42_PROT_COLUMN			(19U)
#define XNVM_EFUSE_ROW_40_PROT_COLUMN			(20U)
#define XNVM_EFUSE_ROW_37_PROT_COLUMN			(21U)
#define XNVM_EFUSE_ROW96_99_0_PROT_COLUMN		(22U)
#define XNVM_EFUSE_ROW64_87_0_PROT_COLUMN		(23U)
#define XNVM_EFUSE_ROW_57_0_PROT_COLUMN			(24U)
#define XNVM_EFUSE_ROW_43_0_PROT_COLUMN			(25U)
#define XNVM_EFUSE_TBITS_0_COLUMN				(28U)
#define XNVM_EFUSE_TBITS_2_COLUMN				(30U)
#define XNVM_EFUSE_TBITS_3_COLUMN				(31U)
#define XNVM_EFUSE_GLITCH_WRLK_COLUMN			(31U)
#define XNVM_EFUSE_GLITCH_ROM_EN_COLUMN			(29U)
#define XNVM_EFUSE_GLITCH_HALT_EN_0_COLUMN		(30U)
#define XNVM_EFUSE_GLITCH_HALT_EN_1_COLUMN		(31U)
#define XNVM_EFUSE_HALT_BOOT_ERROR_1			(22U)
#define XNVM_EFUSE_HALT_BOOT_ERROR_0			(21U)
#define XNVM_EFUSE_HALT_ENV_ERROR_1				(20U)
#define XNVM_EFUSE_HALT_ENV_ERROR_0				(19U)

#define XNVM_EFUSE_PUF_ECC_PUF_CTRL_REGEN_DIS_COLUMN	(31U)
#define XNVM_EFUSE_PUF_ECC_PUF_CTRL_HD_INVLD_COLUMN	(30U)

/* eFUSE Controller WR_LOCK register definition */
#define XNVM_EFUSE_WR_UNLOCK_PASSCODE			(0xDF0DU)

/* eFUSE Controller CFG register definition */
#define XNVM_EFUSE_CFG_ENABLE_PGM			(0x01U << 1U)
#define XNVM_EFUSE_CFG_MARGIN_RD			(0x01U << 2U)
#define XNVM_EFUSE_CFG_NORMAL_RD			(0x00U << 2U)

/* eFUSE Controller STATUS register definition */
#define XNVM_EFUSE_STATUS_TBIT_0			(0x01U << 0U)
#define XNVM_EFUSE_STATUS_TBIT_1			(0x01U << 1U)
#define XNVM_EFUSE_STATUS_TBIT_2			(0x01U << 2U)
#define XNVM_EFUSE_STATUS_CACHE_DONE			(0x01U << 5U)

/* eFUSE Controller PGM_ADDR register definition */
#define XNVM_EFUSE_ADDR_COLUMN_SHIFT			(0U)
#define XNVM_EFUSE_ADDR_ROW_SHIFT			(5U)
#define XNVM_EFUSE_ADDR_PAGE_SHIFT			(13U)

/* eFUSE Controller TBITS PGM ENABLE MASK and Shift */
#define XNVM_EFUSE_TBITS_PRGRMG_EN_MASK			(0x01U << 5U)
#define XNVM_EFUSE_TBITS_MASK				(0xFU)
#define XNVM_EFUSE_TBITS_SHIFT				(27U)

/* eFUSE Controller Anchor bits mask */
#define XNVM_EFUSE_ANCHOR_BITS_MASK			(0x08000002)

/* eFUse Cache load mask */
#define XNVM_EFUSE_CACHE_LOAD_MASK			(0x01U)

/* eFuse Security Misc 1 Protection Row Mask */
#define XNVM_EFUSE_SECURITY_MISC_1_PROT_MASK		(0x1FFFU)

/* eFuse ISR registers masks */
#define XNVM_EFUSE_ISR_PGM_DONE				(0x01U << 0U)
#define XNVM_EFUSE_ISR_PGM_ERROR			(0x01U << 1U)
#define XNVM_EFUSE_ISR_RD_DONE				(0x01U << 2U)
#define XNVM_EFUSE_ISR_RD_ERROR				(0x01U << 3U)
#define XNVM_EFUSE_ISR_CACHE_ERROR			(0x01U << 4U)


/* eFUSE Controller PD register definition */
#define XNVM_EFUSE_PD_ENABLE				(0x01U << 0U)

/* PS Reference clock definition in Hz. */
#define XNVM_PS_REF_CLK_FREQ			(XPAR_PSU_PSS_REF_CLK_FREQ_HZ)

/* Number of Rows per Page */
#define XNVM_NUM_OF_ROWS_PER_PAGE			(256U)

/* Timeout in term of number of times status register polled to check eFUSE
 * read operation complete
 */
#define XNVM_EFUSE_RD_TIMEOUT_VAL			(100U)

/* Timeout in term of number of times status register polled to check eFUSE
 * programming operation complete
 */
#define XNVM_EFUSE_PGM_TIMEOUT_VAL			(100U)

/* Timeout in term of number of times status register polled to check eFuse
 * Cache load is done
 */
#define XNVM_EFUSE_CACHE_LOAD_TIMEOUT_VAL		(0x800U)

/* Timeout in term of number of times status register polled to check eFuse
 * Crc check id done.
 */
#define XNVM_POLL_TIMEOUT				(0xFFFFFFFFU)

#ifdef __cplusplus
}
#endif

#endif	/* XNVM_EFUSE_HW_H */
