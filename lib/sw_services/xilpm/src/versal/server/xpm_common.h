/******************************************************************************
* Copyright (c) 2018 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#ifndef XPM_COMMON_H_
#define XPM_COMMON_H_

#include "xstatus.h"
#include "xil_io.h"
#include "xil_util.h"
#include "xpm_err.h"
#include "xplmi_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Hack: These will increase code size.  Define them as needed. */
#define xSELF_TEST
#define xSELF_TEST_DEVICE_REQUEST
#define xSELF_TEST_PIN_API
#define xSELF_TEST_RESET_API
#define xSELF_TEST_CLOCK_API
#define xDEBUG_REG_IO

#define TRUE	1U
#define FALSE	0U

/**
 * This macro defines "always false" value which is of boolean type.
 * The purpose of macro is to have boolean value which can be used
 * at loop condition instead of "0U" which is non-boolean value.
 */
#define XPM_FALSE_COND		(0U != 0U)

/**
 * GCC Specific attribute to suppress unused variable/function warning
 */
#ifndef maybe_unused
#define maybe_unused __attribute__((unused))
#endif

#define XPM_ALERT_VAL	0x10U
#define XPM_ERR_VAL	0x20U
#define XPM_WARN_VAL	0x30U
#define XPM_INFO_VAL	0x40U
#define XPM_DBG_VAL	0x50U

#define XPM_ALERT   (DEBUG_GENERAL  | XPM_ALERT_VAL)
#define XPM_ERR     (DEBUG_GENERAL  | XPM_ERR_VAL)
#define XPM_WARN    (DEBUG_GENERAL  | XPM_WARN_VAL)
#define XPM_INFO    (DEBUG_INFO     | XPM_INFO_VAL)
#define XPM_DBG     (DEBUG_DETAILED | XPM_DBG_VAL)

#define XPM_DEBUG_MASK	0x70U
#define XPM_DEBUG_SHIFT	4U

/**
 * Common baseline macro to print debug logs
 */

void XPm_Printf(u32 DebugType, const char *Fnstr, const char8 *Ctrl1, ...);

#define PmPrintCommon(DbgLevel, ...)					\
	do {								\
		if (((DbgLevel) & (XPlmiDbgCurrentTypes)) != (u8)FALSE) {\
			XPm_Printf(DbgLevel, __func__,  __VA_ARGS__);\
		}\
	} while (XPM_FALSE_COND)

/* Debug logs */
#define PmAlert(...)	PmPrintCommon(XPM_ALERT, __VA_ARGS__)
#define PmErr(...)	PmPrintCommon(XPM_ERR, __VA_ARGS__)
#define PmWarn(...)	PmPrintCommon(XPM_WARN, __VA_ARGS__)
#define PmInfo(...)	PmPrintCommon(XPM_INFO, __VA_ARGS__)
#define PmDbg(...)	PmPrintCommon(XPM_DBG, __VA_ARGS__)

#ifndef MIO_FLUSH_DEBUG
#define MIO_FLUSH_DEBUG DEBUG_INFO
#endif

#ifdef DEBUG_REG_IO

#define PmIn32(ADDR, VAL)					\
	do {							\
		(VAL) = XPm_In32(ADDR);				\
		PmInfo("RD: 0x%08X -> 0x%08X\r\n", ADDR, VAL);	\
	} while (XPM_FALSE_COND)

#define PmOut32(ADDR, VAL)					\
	do {							\
		PmInfo("WR: 0x%08X <- 0x%08X\r\n", ADDR, VAL);	\
		XPm_Out32(ADDR, VAL);				\
	} while (XPM_FALSE_COND)

#define PmRmw32(ADDR, MASK, VAL)				\
	do {							\
		XPm_RMW32(ADDR, MASK, VAL);			\
		PmInfo("RMW: Addr=0x%08X, Mask=0x%08X, Val=0x%08X, Reg=0x%08X\r\n", \
			ADDR, MASK, VAL, XPm_In32(ADDR));	\
	} while (XPM_FALSE_COND)								\

#define PmIn64(ADDR, VAL)					\
	do {							\
		(VAL) = XPm_In64(ADDR);				\
		PmInfo("RD: 0x%016X -> 0x%08X\r\n", ADDR, VAL);	\
	} while (XPM_FALSE_COND)								\

#define PmOut64(ADDR, VAL)					\
	do {							\
		PmInfo("WR: 0x%016X <- 0x%08X\r\n", ADDR, VAL);	\
		XPm_Out64(ADDR, VAL);				\
	} while (XPM_FALSE_COND)								\

#define PmRmw64(ADDR, MASK, VAL)				\
	do {							\
		XPm_RMW64(ADDR, MASK, VAL);			\
		PmInfo("RMW: Addr=0x%016X, Mask=0x%08X, Val=0x%08X, Reg=0x%08X\r\n", \
			ADDR, MASK, VAL, XPm_In64(ADDR));	\
	} while (XPM_FALSE_COND)								\

#else

#define PmIn32(ADDR, VAL)		((VAL) = XPm_In32(ADDR))

#define PmOut32(ADDR, VAL)		XPm_Out32(ADDR, VAL)

#define PmRmw32(ADDR, MASK, VAL)	XPm_RMW32(ADDR, MASK, VAL)

#define PmIn64(ADDR, VAL)		((VAL) = XPm_In64(ADDR))

#define PmOut64(ADDR, VAL)		XPm_Out64(ADDR, VAL)

#define PmRmw64(ADDR, MASK, VAL)	XPm_RMW64(ADDR, MASK, VAL)

#endif

#define PmChkRegMask32(ADDR, MASK, VAL, STATUS)				\
	do {									\
		if (((u32)(VAL) & (u32)(MASK)) != ((u32)(XPm_In32((ADDR))) & (u32)(MASK))) {	\
			(STATUS) = XPM_REG_WRITE_FAILED;			\
		}								\
	} while (XPM_FALSE_COND)								\

#define PmChkRegOut32(ADDR, VAL, STATUS)					\
	do {									\
		if ((u32)(VAL) != XPm_In32((ADDR))) {				\
			(STATUS) = XPM_REG_WRITE_FAILED;			\
		}								\
	} while (XPM_FALSE_COND)								\


/* Payload Packets */
#define XPM_PACK_PAYLOAD(Payload, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5)	\
	do {								\
		Payload[0] = (u32)Arg0;					\
		Payload[1] = (u32)Arg1;					\
		Payload[2] = (u32)Arg2;					\
		Payload[3] = (u32)Arg3;					\
		Payload[4] = (u32)Arg4;					\
		Payload[5] = (u32)Arg5;					\
		PmDbg("%s(%x, %x, %x, %x, %x)\r\n", __func__, Arg1, Arg2, Arg3, Arg4, Arg5);	\
	} while (XPM_FALSE_COND)

#define XPM_MODULE_ID			(0x02UL)

#define XPM_HEADER(len, ApiId)		((len << 16U) | (XPM_MODULE_ID << 8U) | ((u32)ApiId))

#define XPM_HEADER_SET_CMDTYPE(Payload, SecurityFlag)		\
	(Payload[0] = ((Payload[0] & ~0xFF000000UL) | ((SecurityFlag & 0x1UL) << 24U)))

#define XPM_PACK_PAYLOAD0(Payload, ApiId) \
	XPM_PACK_PAYLOAD(Payload, XPM_HEADER(0UL, ApiId), 0, 0, 0, 0, 0)
#define XPM_PACK_PAYLOAD1(Payload, ApiId, Arg1) \
	XPM_PACK_PAYLOAD(Payload, XPM_HEADER(1UL, ApiId), Arg1, 0, 0, 0, 0)
#define XPM_PACK_PAYLOAD2(Payload, ApiId, Arg1, Arg2) \
	XPM_PACK_PAYLOAD(Payload, XPM_HEADER(2UL, ApiId), Arg1, Arg2, 0, 0, 0)
#define XPM_PACK_PAYLOAD3(Payload, ApiId, Arg1, Arg2, Arg3) \
	XPM_PACK_PAYLOAD(Payload, XPM_HEADER(3UL, ApiId), Arg1, Arg2, Arg3, 0, 0)
#define XPM_PACK_PAYLOAD4(Payload, ApiId, Arg1, Arg2, Arg3, Arg4) \
	XPM_PACK_PAYLOAD(Payload, XPM_HEADER(4UL, ApiId), Arg1, Arg2, Arg3, Arg4, 0)
#define XPM_PACK_PAYLOAD5(Payload, ApiId, Arg1, Arg2, Arg3, Arg4, Arg5) \
	XPM_PACK_PAYLOAD(Payload, XPM_HEADER(5UL, ApiId), Arg1, Arg2, Arg3, Arg4, Arg5)

/**
 * Adds redundancy while comparing the return value of called function.
 * based on NULL == RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to strictly check for && as per the
 * implementation.
 *
 * @RET1	Variable to store the return value of function to be execute
 *		at at RHS during comparison.
 * @TYPE	Type of pointer which returns by the function to be execute at
 *		RHS during comparison.
 * @FUNC	The function to be execute at RHS during comparison.
 * @param	Other params are arguments to the called function
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_STRICT_CHECK_IF_NULL(STSTMP, RET1, TYPE, FUNC, ...)	\
	({							\
		volatile const TYPE *RET2 = (NULL);		\
		RET1 = (NULL);					\
		STSTMP = XST_FAILURE;				\
		RET1 = (TYPE *)(FUNC(__VA_ARGS__));		\
		RET2 = (TYPE *)(FUNC(__VA_ARGS__));		\
		if (((NULL) == RET1) && ((NULL) == RET2)) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on NULL != RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to strictly check for && as per the
 * implementation.
 *
 * @RET1	Variable to store the return value of function to be execute
 *		at at RHS during comparison.
 * @TYPE	Type of pointer which returns by the function to be execute at
 *		RHS during comparison.
 * @FUNC	The function to be execute at RHS during comparison.
 * @param	Other params are arguments to the called function
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_STRICT_CHECK_IF_NOT_NULL(STSTMP, RET1, TYPE, FUNC, ...)	\
	({							\
		volatile const TYPE *RET2 = NULL;		\
		RET1 = NULL;					\
		STSTMP = XST_FAILURE;				\
		RET1 = (TYPE *)(FUNC(__VA_ARGS__));		\
		RET2 = (TYPE *)(FUNC(__VA_ARGS__));		\
		if (((NULL) != RET1) && ((NULL) != RET2)) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on LHS == RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to strictly check for && as per the
 * implementation.
 *
 * @LHS		Left hand side value wants to compare.
 * @TYPE	Return type of function to be execute at RHS during comparison.
 * @FUNC	The function to be execute at RHS during comparison.
 * @param	Other params are arguments to the called function
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_STRICT_CHECK_IF_EQUAL_FOR_FUNC(STSTMP, LHS, TYPE, FUNC, ...)\
	({							\
		volatile TYPE RET1 = ~(LHS);			\
		volatile TYPE RET2 = ~(LHS);			\
		STSTMP = XST_FAILURE;				\
		RET1 = (FUNC(__VA_ARGS__));			\
		RET2 = (FUNC(__VA_ARGS__));			\
		if (((LHS) == RET1) && ((LHS) == RET2)) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on LHS != RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to strictly check for && as per the
 * implementation.
 *
 * @LHS		Left hand side value wants to compare.
 * @TYPE	Return type of function to be execute at RHS during comparison.
 * @FUNC	The function to be execute at RHS during comparison.
 * @param	Other params are arguments to the called function
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_STRICT_CHECK_IF_NOTEQUAL_FOR_FUNC(STSTMP, LHS, TYPE, FUNC, ...)\
	({							\
		volatile TYPE RET1 = LHS;			\
		volatile TYPE RET2 = LHS;			\
		STSTMP = XST_FAILURE;				\
		RET1 = (FUNC(__VA_ARGS__));			\
		RET2 = (FUNC(__VA_ARGS__));			\
		if (((LHS) != RET1) && ((LHS) != RET2)) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on LHS == RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to strictly check for && as per the
 * implementation.
 *
 * @LHS		Left hand side value wants to compare.
 * @RHS		Right hand side value wants to compare.
 * @TYPE	Data type of LHS and RHS.
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_STRICT_CHECK_IF_EQUAL(STSTMP, LHS, RHS, TYPE)		\
	({							\
		volatile TYPE RET1 = LHS;			\
		volatile TYPE RET2 = ~(LHS);			\
		STSTMP = XST_FAILURE;				\
		if ((RET1 == (RHS)) && (RET2 == (~(RHS)))) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on LHS != RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to strictly check for && as per the
 * implementation.
 *
 * @LHS		Left hand side value wants to compare.
 * @RHS		Right hand side value wants to compare.
 * @TYPE	Data type of LHS and RHS.
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_STRICT_CHECK_IF_NOTEQUAL(STSTMP, LHS, RHS, TYPE)		\
	({							\
		volatile TYPE RET1 = LHS;			\
		volatile TYPE RET2 = ~(LHS);			\
		STSTMP = XST_FAILURE;				\
		if ((RET1 != (RHS)) && (RET2 != (~(RHS)))) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on NULL == RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to check for || as per implementation.
 *
 * @RET1	Variable to store the return value of function to be execute
 *		at at RHS during comparison.
 * @TYPE	Type of pointer which returns by the function to be execute at
 *		RHS during comparison.
 * @FUNC	The function to be execute at RHS during comparison.
 * @param	Other params are arguments to the called function
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_CHECK_IF_NULL(STSTMP, RET1, TYPE, FUNC, ...)	\
	({							\
		volatile const TYPE *RET2 = (NULL);		\
		RET1 = (NULL);					\
		STSTMP = XST_FAILURE;				\
		RET1 = (TYPE *)(FUNC(__VA_ARGS__));		\
		RET2 = (TYPE *)(FUNC(__VA_ARGS__));		\
		if (((NULL) == RET1) || ((NULL) == RET2)) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on NULL != RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to check for || as per implementation.
 *
 * @RET1	Variable to store the return value of function to be execute
 *		at at RHS during comparison.
 * @TYPE	Type of pointer which returns by the function to be execute at
 *		RHS during comparison.
 * @FUNC	The function to be execute at RHS during comparison.
 * @param	Other params are arguments to the called function
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_CHECK_IF_NOT_NULL(STSTMP, RET1, TYPE, FUNC, ...)	\
	({							\
		volatile const TYPE *RET2 = NULL;		\
		RET1 = NULL;					\
		STSTMP = XST_FAILURE;				\
		RET1 = (TYPE *)(FUNC(__VA_ARGS__));		\
		RET2 = (TYPE *)(FUNC(__VA_ARGS__));		\
		if (((NULL) != RET1) || ((NULL) != RET2)) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on LHS == RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to check for || as per implementation.
 *
 * @LHS		Left hand side value wants to compare.
 * @TYPE	Return type of function to be execute at RHS during comparison.
 * @FUNC	The function to be execute at RHS during comparison.
 * @param	Other params are arguments to the called function
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_CHECK_IF_EQUAL_FOR_FUNC(STSTMP, LHS, TYPE, FUNC, ...)\
	({							\
		volatile TYPE RET1 = ~(LHS);			\
		volatile TYPE RET2 = ~(LHS);			\
		STSTMP = XST_FAILURE;				\
		RET1 = (FUNC(__VA_ARGS__));			\
		RET2 = (FUNC(__VA_ARGS__));			\
		if (((LHS) == RET1) || ((LHS) == RET2)) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on LHS != RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to check for || as per implementation.
 *
 * @LHS		Left hand side value wants to compare.
 * @TYPE	Return type of function to be execute at RHS during comparison.
 * @FUNC	The function to be execute at RHS during comparison.
 * @param	Other params are arguments to the called function
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_CHECK_IF_NOTEQUAL_FOR_FUNC(STSTMP, LHS, TYPE, FUNC, ...)\
	({							\
		volatile TYPE RET1 = LHS;			\
		volatile TYPE RET2 = LHS;			\
		STSTMP = XST_FAILURE;				\
		RET1 = (FUNC(__VA_ARGS__));			\
		RET2 = (FUNC(__VA_ARGS__));			\
		if (((LHS) != RET1) || ((LHS) != RET2)) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on LHS == RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to check for || as per implementation.
 *
 * @LHS		Left hand side value wants to compare.
 * @RHS		Right hand side value wants to compare.
 * @TYPE	Data type of LHS and RHS.
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_CHECK_IF_EQUAL(STSTMP, LHS, RHS, TYPE)		\
	({							\
		volatile TYPE RET1 = LHS;			\
		volatile TYPE RET2 = ~(LHS);			\
		STSTMP = XST_FAILURE;				\
		if ((RET1 == (RHS)) || (RET2 == (~(RHS)))) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

/**
 * Adds redundancy while comparing the return value of called function.
 * based on LHS != RHS comparison wants to skip some part from execution.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to".
 * This MACRO will require when need to check for || as per implementation.
 *
 * @LHS		Left hand side value wants to compare.
 * @RHS		Right hand side value wants to compare.
 * @TYPE	Data type of LHS and RHS.
 *
 * @return	XST_SUCCESS if condition become true else XST_FAILURE
 **/
#define XPM_CHECK_IF_NOTEQUAL(STSTMP, LHS, RHS, TYPE)		\
	({							\
		volatile TYPE RET1 = LHS;			\
		volatile TYPE RET2 = ~(LHS);			\
		STSTMP = XST_FAILURE;				\
		if ((RET1 != (RHS)) || (RET2 != (~(RHS)))) {	\
			STSTMP = XST_SUCCESS;			\
		}						\
		STSTMP;						\
	})

#define BIT(n)					(1U << (n))
#define BIT8(n)					((u8)1U << (n))
#define BIT16(n)				((u16)1U << (n))
#define BIT32(n)				((u32)1U << (n))
// set the first n bits to 1, rest to 0
#define BITMASK(n)				(u32)((1ULL << (n)) - 1ULL)
// set width specified bits at offset to 1, rest to 0
#define BITNMASK(offset, width) 		(BITMASK(width) << (offset))

#define ARRAY_SIZE(x)				(sizeof(x) / sizeof((x)[0]))

#define MIN(x, y)				(((x) < (y)) ? (x) : (y))

#define XPm_Read32				XPm_In32
#define XPm_Write32				XPm_Out32

#define PLATFORM_VERSION_SILICON		(0x0U)
#define PLATFORM_VERSION_SPP			(0x1U)
#define PLATFORM_VERSION_EMU			(0x2U)
#define PLATFORM_VERSION_QEMU			(0x3U)
#define PLATFORM_VERSION_FCV			(0x4U)
#define PLATFORM_VERSION_COSIM			(0x7U)

#define PLATFORM_VERSION_SILICON_ES1		(0x0U)
#define PLATFORM_VERSION_SILICON_ES2		(0x1U)

#define PMC_VERSION_SILICON_ES1			(0x10U)
#define PMC_VERSION_SILICON_PROD		(0x20U)

#define XPM_PMC_TAP_IDCODE_SBFMLY_SHIFT		(18U)
#define XPM_PMC_TAP_IDCODE_DEV_SHIFT		(12U)
#define XPM_PMC_TAP_IDCODE_SBFMLY_S		((u32)2U << XPM_PMC_TAP_IDCODE_SBFMLY_SHIFT)
/* VC1902 */
#define PMC_TAP_IDCODE_DEV_VC1902		((u32)0x28U << XPM_PMC_TAP_IDCODE_DEV_SHIFT)
#define PMC_TAP_IDCODE_DEV_SBFMLY_VC1902	(XPM_PMC_TAP_IDCODE_SBFMLY_S | PMC_TAP_IDCODE_DEV_VC1902)

#define SLR_TYPE_MONOLITHIC_DEV			(0x7U)
#define SLR_TYPE_SSIT_DEV_MASTER_SLR		(0x6U)
#define SLR_TYPE_SSIT_DEV_SLAVE_1_SLR_TOP	(0x5U)
#define SLR_TYPE_SSIT_DEV_SLAVE_1_SLR_NTOP	(0x4U)
#define SLR_TYPE_SSIT_DEV_SLAVE_2_SLR_TOP	(0x3U)
#define SLR_TYPE_SSIT_DEV_SLAVE_2_SLR_NTOP	(0x2U)
#define SLR_TYPE_SSIT_DEV_SLAVE_3_SLR_TOP	(0x1U)
#define SLR_TYPE_INVALID			(0x0U)

#ifdef STDOUT_BASEADDRESS
#if (STDOUT_BASEADDRESS == 0xFF000000U)
#define NODE_UART PM_DEV_UART_0 /* Assign node ID with UART0 device ID */
#elif (STDOUT_BASEADDRESS == 0xFF010000U)
#define NODE_UART PM_DEV_UART_1 /* Assign node ID with UART1 device ID */
#endif
#endif

void *XPm_AllocBytes(u32 SizeInBytes);

void XPm_Out32(u32 RegAddress, u32 l_Val);

u32 XPm_In32(u32 RegAddress);

u32 XPm_GetPlatform(void);
u32 XPm_GetPlatformVersion(void);
u32 XPm_GetPmcVersion(void);
u32 XPm_GetSlrType(void);
u32 XPm_GetIdCode(void);

/**
 * Read Modify Write a register
 */
void XPm_RMW32(u32 RegAddress, u32 Mask, u32 Value);

void XPm_Out64(u64 RegAddress, u32 Value);
u32 XPm_In64(u64 RegAddress);
void XPm_RMW64(u64 RegAddress, u32 Mask, u32 Value);

/**
 * Wait for a period represented by TimeOut
 *
 */
void XPm_Wait(u32 TimeOutCount);

/**
 * Poll for mask for a period represented by TimeOut
 */
static inline XStatus XPm_PollForMask(u32 RegAddress, u32 Mask,
				      u32 TimeOut)
{
	return XPlmi_UtilPoll(RegAddress, Mask, Mask, TimeOut);
}

/**
 * Poll for mask for a period represented by TimeOut for 64 bit registers
 */
static inline XStatus XPm_PollForMask64(u64 RegAddress, u32 Mask,
					u32 TimeOut)
{
	return XPlmi_UtilPoll64(RegAddress, Mask, Mask, TimeOut);
}

/**
 * Poll for zero for a period represented by TimeOut
 */
static inline XStatus XPm_PollForZero(u32 RegAddress, u32 Mask,
				      u32 TimeOut)
{
	return XPlmi_UtilPoll(RegAddress, Mask, 0, TimeOut);
}

/**
 * Compute parity of a 32-bit word
 */
u32 XPm_ComputeParity(u32 CalParity);

/* Dump Memory Related Data like Total Mem, Usaged Mem, Free Mem */
void XPm_DumpMemUsage(void);

/* NPI PCSR related general functions */
XStatus XPm_UnlockPcsr(u32 BaseAddr);
XStatus XPm_LockPcsr(u32 BaseAddr);
XStatus XPm_PcsrWrite(u32 BaseAddress, u32 Mask, u32 Value);

#ifdef __cplusplus
}
#endif

#endif /* XPM_COMMON_H_ */
