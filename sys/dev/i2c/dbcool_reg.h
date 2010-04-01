/*	$NetBSD: dbcool_reg.h,v 1.5 2010/04/01 04:29:35 macallan Exp $ */

/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Goyette
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* 
 * a driver for the dbCool(tm) family of environmental controllers
 */

#ifndef DBCOOLREG_H
#define DBCOOLREG_H

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: dbcool_reg.h,v 1.5 2010/04/01 04:29:35 macallan Exp $");

#define DBCOOL_ADDRMASK		0x7c
#define	DBCOOL_ADDR		0x2c	/* Some chips have multiple addrs */

/* The dBCool chip family register set */

/* Not all registers are available on all chips! */
#define	DBCOOL_CONFIG5A_REG	0x04
#define	DBCOOL_CONFIG6_REG	0x10
#define	DBCOOL_CONFIG7_REG	0x11
#define	DBCOOL_INTERNAL_TRIP	0x13
#define	DBCOOL_EXTERNAL_TRIP	0x14
#define	DBCOOL_TEST		0x15
#define	DBCOOL_CHANNEL_MODE	0x16
#define	DBCOOL_INT_TRIP_FIXED	0x17
#define	DBCOOL_EXT_TRIP_FIXED	0x18
#define	DBCOOL_ANALOG_OUT	0x19
#define	DBCOOL_PECI1_TEMP	0x1A
#define	DBCOOL_PECI2_TEMP	0x1B
#define	DBCOOL_PECI3_TEMP	0x1C
#define	DBCOOL_IMON		0x1D
#define	DBCOOL_VTT		0x1E
#define	DBCOOL_EXTRES_VTT_IMON	0x1F
#define	DBCOOL_OFFSET		0x1F
#define	DBCOOL_25VIN		0x20
#define	DBCOOL_VCCP		0x21
#define	DBCOOL_VCC		0x22
#define	DBCOOL_5VIN		0x23
#define	DBCOOL_12VIN		0x24
#define	DBCOOL_CPU_VOLTAGE2	0x25
#define	DBCOOL_REMOTE1_TEMP	0x25
#define	DBCOOL_LOCAL_TEMP	0x26
#define	DBCOOL_REMOTE2_TEMP	0x27
#define	DBCOOL_FAN1_TACH_LSB	0x28
#define	DBCOOL_FAN1_TACH_MSB	0x29
#define	DBCOOL_FAN2_TACH_LSB	0x2A
#define	DBCOOL_FAN2_TACH_MSB	0x2B
#define	DBCOOL_FAN3_TACH_LSB	0x2C
#define	DBCOOL_FAN3_TACH_MSB	0x2D
#define	DBCOOL_FAN4_TACH_LSB	0x2E
#define	DBCOOL_FAN4_TACH_MSB	0x2F
#define	DBCOOL_PWM1_CURDUTY	0x30
#define	DBCOOL_DAC0_START	0x30
#define	DBCOOL_PWM2_CURDUTY	0x31
#define	DBCOOL_DAC1_START	0x31
#define	DBCOOL_PWM3_CURDUTY	0x32
#define	DBCOOL_DAC0_MIN		0x32
#define	DBCOOL_PECI0		0x33
#define	DBCOOL_DAC1_MIN		0x33
#define	DBCOOL_PECI_LOWLIM	0x34
#define	DBCOOL_DAC0_MAX		0x34
#define	DBCOOL_PECI_HIGHLIM	0x35
#define	DBCOOL_DAC1_MAX		0x35
#define	DBCOOL_PECI_CFG1	0x36
#define	DBCOOL_DYNTMIN_CNTRL1	0x36
#define	DBCOOL_DYNTMIN_CNTRL2	0x37
#define	DBCOOL_PWM1_MAXDUTY	0x38
#define	DBCOOL_PWM2_MAXDUTY	0x39
#define	DBCOOL_PWM3_MAXDUTY	0x3A
/*
 * Note: ADT7490 reused the Device_ID register for PECI Tcontrol value
 * (equivalent to Ttherm for the regular temp sensors)
 */
#define	DBCOOL_DEVICEID_REG	0x3D
#define	DBCOOL_PECI_TCONTROL	0x3D
#define	DBCOOL_COMPANYID_REG	0x3E
#define	DBCOOL_REVISION_REG	0x3F
#define	DBCOOL_CONFIG1_REG	0x40
#define	DBCOOL_DAC0_OUT		0x40
#define	DBCOOL_ISR1_REG		0x41
#define	DBCOOL_DAC1_OUT		0x41
#define	DBCOOL_ISR2_REG		0x42
#define	DBCOOL_ISR3_REG		0x43
#define	DBCOOL_VID_REG		0x43
#define	DBCOOL_25VIN_LOWLIM	0x44
#define	DBCOOL_25VIN_HIGHLIM	0x45
#define	DBCOOL_VCCP_LOWLIM	0x46
#define	DBCOOL_VCCP_HIGHLIM	0x47
#define	DBCOOL_VIDB		0x47
#define	DBCOOL_VCC_LOWLIM	0x48
#define	DBCOOL_VCC_HIGHLIM	0x49
#define	DBCOOL_VID4		0x49
#define	DBCOOL_5VIN_LOWLIM	0x4A
#define	DBCOOL_5VIN_HIGHLIM	0x4B
#define	DBCOOL_12VIN_LOWLIM	0x4C
#define	DBCOOL_12VIN_HIGHLIM	0x4D
#define	DBCOOL_REMOTE1_LOWLIM	0x4E
#define	DBCOOL_REMOTE1_HIGHLIM	0x4F
#define	DBCOOL_LOCAL_LOWLIM	0x50
#define	DBCOOL_LOCAL_HIGHLIM	0x51
#define	DBCOOL_REMOTE2_LOWLIM	0x52
#define	DBCOOL_REMOTE2_HIGHLIM	0x53
#define	DBCOOL_TACH1_MIN_LSB	0x54
#define	DBCOOL_TACH1_MIN_MSB	0x55
#define	DBCOOL_TACH2_MIN_LSB	0x56
#define	DBCOOL_TACH2_MIN_MSB	0x57
#define	DBCOOL_TACH3_MIN_LSB	0x58
#define	DBCOOL_TACH3_MIN_MSB	0x59
#define	DBCOOL_TACH4_MIN_LSB	0x5A
#define	DBCOOL_TACH4_MIN_MSB	0x5B
#define	DBCOOL_PWM1_CTL		0x5C
#define	DBCOOL_PWM2_CTL		0x5D
#define	DBCOOL_PWM3_CTL		0x5E
#define	DBCOOL_REMOTE1_TRANGE	0x5F	/* Bits [7:4] */
#define	DBCOOL_LOCAL_TRANGE	0x60	/* Bits [7:4] */
#define	DBCOOL_REMOTE2_TRANGE	0x61	/* Bits [7:4] */
#define	DBCOOL_ENH_ACOUST_1	0x62
#define	DBCOOL_ENH_ACOUST_2	0x63
#define	DBCOOL_PWM1_MINDUTY	0x64
#define	DBCOOL_PWM2_MINDUTY	0x65
#define	DBCOOL_PWM3_MINDUTY	0x66
#define	DBCOOL_REMOTE1_TMIN	0x67
#define	DBCOOL_LOCAL_TMIN	0x68
#define	DBCOOL_REMOTE2_TMIN	0x69
#define	DBCOOL_REMOTE1_TTHRESH	0x6A
#define	DBCOOL_LOCAL_TTHRESH	0x6B
#define	DBCOOL_REMOTE2_TTHRESH	0x6C
#define	DBCOOL_R1_LCL_TMIN_HYST	0x6D
#define	DBCOOL_R2_TMIN_HYST	0x6E
#define	DBCOOL_XNOR_ENABLE	0x6F
#define	DBCOOL_REMOTE1_TEMPOFF	0x70
#define	DBCOOL_LOCAL_TEMPOFF	0x71
#define	DBCOOL_REMOTE2_TEMPOFF	0x72
#define	DBCOOL_CONFIG2_REG	0x73
#define	DBCOOL_IMASK1_REG	0x74
#define	DBCOOL_IMASK2_REG	0x75
#define	DBCOOL_EXTRES1_REG	0x76
#define	DBCOOL_EXTRES2_REG	0x77
#define	DBCOOL_CONFIG3_REG	0x78
#define	DBCOOL_THERM_TIMERSTATUS_REG	0x79
#define	DBCOOL_THERM_TIMERLIMIT_REG	0x7A
#define	DBCOOL_TACHPULSE_REG	0x7B
#define	DBCOOL_CONFIG5_REG	0x7C
#define	DBCOOL_CONFIG4_REG	0x7D
#define	DBCOOL_TEST1_REG	0x7E
#define	DBCOOL_TEST2_REG	0x7F
#define	DBCOOL_GPIO_CONFIG	0x80
#define	DBCOOL_ISR4_REG		0x81
#define	DBCOOL_IMASK3_REG	0x82
#define	DBCOOL_IMASK4_REG	0x83
#define	DBCOOL_VTT_LOWLIM	0x84
#define	DBCOOL_IMON_LOWLIM	0x85
#define	DBCOOL_VTT_HIGHLIM	0x86
#define	DBCOOL_IMON_HIGHLIM	0x87
#define	DBCOOL_PECI_CFG2	0x88
#define	DBCOOL_TEST3_REG	0x89
#define	DBCOOL_PECI_OP_PT	0x8A
#define	DBCOOL_REMOTE1_OP_PT	0x8B
#define	DBCOOL_LOCAL_OP_PT	0x8C
#define	DBCOOL_REMOTE2_OP_PT	0x8D
#define	DBCOOL_DYNTMIN_CTL1	0x8E
#define	DBCOOL_DYNTMIN_CTL2	0x8F
#define	DBCOOL_DYNTMIN_CTL3	0x90
#define	DBCOOL_PECI0_TEMPOFF	0x94
#define	DBCOOL_PECI1_TEMPOFF	0x95
#define	DBCOOL_PECI2_TEMPOFF	0x96
#define	DBCOOL_PECI3_TEMPOFF	0x97
#define	DBCOOL_NO_REG		0xff

/* Config register bit definitions */
#define	DBCOOL_CFG1_START	0x01
#define	DBCOOL_CFG1_LOCK	0x02
#define	DBCOOL_CFG1_RDY		0x04
#define	DBCOOL_CFG1_FSPD	0x08
#define	DBCOOL_CFG1_VxI		0x10
#define	DBCOOL_CFG1_RESET	0x10
#define	DBCOOL_CFG1_FSPDIS	0x20
#define	DBCOOL_CFG1_12VVID4_SEL	0x20
#define	DBCOOL_CFG1_TODIS	0x40
#define	DBCOOL_CFG1_Vcc		0x80
#define	DBCOOL_CFG1_RESET_LATCH	0x80
#define	DBCOOL_CFG2_AIN1	0x01
#define	DBCOOL_CFG2_AIN2	0x02
#define	DBCOOL_CFG2_AIN3	0x04
#define	DBCOOL_CFG2_AIN4	0x08
#define	DBCOOL_CFG2_AVG		0x10
#define	DBCOOL_CFG2_ATTN	0x20
#define	DBCOOL_CFG2_CONV	0x40
#define	DBCOOL_CFG2_SHDN	0x80
#define	DBCOOL_CFG3_ALERT	0x01
#define	DBCOOL_CFG3_THERM	0x02
#define	DBCOOL_CFG3_BOOST	0x04
#define	DBCOOL_CFG3_FAST	0x08
#define	DBCOOL_CFG3_DC1		0x10
#define	DBCOOL_CFG3_DC2		0x20
#define	DBCOOL_CFG3_DC3		0x40
#define	DBCOOL_CFG3_DC4		0x80

#define	DBCOOL_CFG4_PIN9FUNC	0x03
#define	DBCOOL_CFG4_AINL	0x0C
#define	DBCOOL_CFG4_BYPASS_ATTN	0x20

#define	DBCOOL_CFG5_TWOSCOMP	0x01
#define	DBCOOL_CFG5_FREQ	0x02
#define	DBCOOL_CFG5_GPIOD	0x04
#define	DBCOOL_CFG5_GPIOP	0x08

#define	DBCOOL_CFG6_SLOW_REM1	0x01
#define	DBCOOL_CFG6_SLOW_LOCAL	0x02
#define	DBCOOL_CFG6_SLOW_REM2	0x04
#define	DBCOOL_CFG6_THERM_MAN	0x08
#define DBCOOL_CFG6_VCCP_LOW	0x40
#define	DBCOOL_CFG6_EXTRASLOW	0x80

#define	DBCOOL_CFG7_DIS_THERM_HYST	0x10

/*
 * The ADT7466 is an orphan stepchild in the dbCool family
 */
#define	DBCOOL_ADT7466_CONFIG1		0x00
#define	DBCOOL_ADT7466_CONFIG2		0x01
#define	DBCOOL_ADT7466_CONFIG3		0x02
#define	DBCOOL_ADT7466_CONFIG4		0x03
#define	DBCOOL_ADT7466_CONFIG5		0x04
#define	DBCOOL_ADT7466_AFC1		0x05
#define	DBCOOL_ADT7466_AFC2		0x06
#define	DBCOOL_ADT7466_REM_TEMP_LSB	0x08
#define	DBCOOL_ADT7466_LCL_TEMP_LSB	0x09
#define	DBCOOL_ADT7466_AIN1		0x0A
#define	DBCOOL_ADT7466_AIN2		0x0B
#define	DBCOOL_ADT7466_VCC		0x0C
#define	DBCOOL_ADT7466_REM_TEMP_MSB	0x0D
#define	DBCOOL_ADT7466_LCL_TEMP_MSB	0x0E
#define	DBCOOL_ADT7466_PROCHOT		0x0F
#define	DBCOOL_ADT7466_INTRPT1		0x10
#define	DBCOOL_ADT7466_INTRPT2		0x11
#define	DBCOOL_ADT7466_INTMSK1		0x12
#define	DBCOOL_ADT7466_INTMSK2		0x13
#define	DBCOOL_ADT7466_AIN1_LOLIM	0x14
#define	DBCOOL_ADT7466_AIN1_HILIM	0x15
#define	DBCOOL_ADT7466_AIN2_LOLIM	0x16
#define	DBCOOL_ADT7466_AIN2_HILIM	0x17
#define	DBCOOL_ADT7466_VCC_LOLIM	0x18
#define	DBCOOL_ADT7466_VCC_HILIM	0x19
#define	DBCOOL_ADT7466_REM_TEMP_LOLIM	0x1A
#define	DBCOOL_ADT7466_REM_TEMP_HILIM	0x1B
#define	DBCOOL_ADT7466_LCL_TEMP_LOLIM	0x1C
#define	DBCOOL_ADT7466_LCL_TEMP_HILIM	0x1D
#define	DBCOOL_ADT7466_PROCHOT_LIM	0x1E
#define	DBCOOL_ADT7466_AIN1_THERM	0x1F
#define	DBCOOL_ADT7466_AIN2_THREM	0x20
#define	DBCOOL_ADT7466_REM_THERM	0x21
#define	DBCOOL_ADT7466_LCL_THERM	0x22
#define	DBCOOL_ADT7466_AIN1_OFFSET	0x24
#define	DBCOOL_ADT7466_AIN2_OFFSET	0x25
#define	DBCOOL_ADT7466_REM_OFFSET	0x26
#define	DBCOOL_ADT7466_LCL_OFFSET	0x27
#define	DBCOOL_ADT7466_AIN1_TMIN	0x28
#define	DBCOOL_ADT7466_AIN2_TMIN	0x29
#define	DBCOOL_ADT7466_REM_TMIN		0x2A
#define	DBCOOL_ADT7466_LCL_TMIN		0x2B
#define	DBCOOL_ADT7466_AIN_RANGES	0x2C
#define	DBCOOL_ADT7466_LCL_REM_RANGES	0x2D
#define	DBCOOL_ADT7466_AIN_HYSTS	0x2E
#define	DBCOOL_ADT7466_LCL_REM_HYSTS	0x2F
#define	DBCOOL_ADT7466_FANA_STARTV	0x30
#define	DBCOOL_ADT7466_FANB_STARTV	0x31
#define	DBCOOL_ADT7466_FANA_MINV	0x32
#define	DBCOOL_ADT7466_FANB_MINV	0x33
#define	DBCOOL_ADT7466_FANA_MAXRPM_MSB	0x34
#define	DBCOOL_ADT7466_FANB_MAXRPM_MSB	0x35
#define	DBCOOL_ADT7466_ENH_ACOUSTICS	0x36
#define	DBCOOL_ADT7466_FAULT_INCR	0x37
#define	DBCOOL_ADT7466_TIMEOUT		0x38
#define	DBCOOL_ADT7466_PULSES		0x39
#define	DBCOOL_ADT7466_DRIVE1		0x40
#define	DBCOOL_ADT7466_DRIVE2		0x41
#define	DBCOOL_ADT7466_XOR_TEST		0x42
#define	DBCOOL_ADT7466_FANA_LSB		0x48
#define	DBCOOL_ADT7466_FANA_MSB		0x49
#define	DBCOOL_ADT7466_FANB_LSB		0x4A
#define	DBCOOL_ADT7466_FANB_MSB		0x4B
#define	DBCOOL_ADT7466_FANA_LOLIM_LSB	0x4C
#define	DBCOOL_ADT7466_FANA_LOLIM_MSB	0x4D
#define	DBCOOL_ADT7466_FANB_LOLIM_LSB	0x4E
#define	DBCOOL_ADT7466_FANB_LOLIM_MSB	0x4F

#define	DBCOOL_ADT7466_CFG1_Vcc		0x40
#define	DBCOOL_ADT7466_CFG2_SHDN	0x40

/*
 * Even though it's not really a member of the dbCool family, we also
 * support the ADM1030 chip.  It has a different register set.
 * the ADM1030 is in fact a cut down ADM1031 - the register set is identical
 * except the registers used for the extra temperature and fan control sensors
 * DBCOOL_ADM1030_* are present in both chips with identical functionality
 * DBCOOL_ADM1031_* are ADM1031 only
 */
#define	DBCOOL_ADM1030_CFG1		0x00
#define	DBCOOL_ADM1030_CFG2		0x01
#define	DBCOOL_ADM1030_STATUS1		0x02
#define	DBCOOL_ADM1030_STATUS2		0x03
#define	DBCOOL_ADM1030_TEMP_EXTRES	0x06
#define	DBCOOL_ADM1030_TEST_REG		0x07
#define	DBCOOL_ADM1030_FAN_TACH		0x08
#define	DBCOOL_ADM1031_FAN2_TACH	0x09
#define	DBCOOL_ADM1030_L_TEMP		0x0A
#define	DBCOOL_ADM1030_R_TEMP		0x0B
#define	DBCOOL_ADM1031_R2_TEMP		0x0C
#define	DBCOOL_ADM1030_L_OFFSET		0x0D
#define	DBCOOL_ADM1030_R_OFFSET		0x0E
#define	DBCOOL_ADM1031_R2_OFFSET	0x0F
#define	DBCOOL_ADM1030_FAN_LO_LIM	0x10
#define	DBCOOL_ADM1031_FAN2_LO_LIM	0x11
#define	DBCOOL_ADM1030_L_HI_LIM		0x14
#define	DBCOOL_ADM1030_L_LO_LIM		0x15
#define	DBCOOL_ADM1030_L_TTHRESH	0x16
#define	DBCOOL_ADM1030_R_HI_LIM		0x18
#define	DBCOOL_ADM1030_R_LO_LIM		0x19
#define	DBCOOL_ADM1030_R_TTHRESH	0x1A
#define	DBCOOL_ADM1031_R2_HI_LIM	0x1C
#define	DBCOOL_ADM1031_R2_LO_LIM	0x1D
#define	DBCOOL_ADM1031_R2_TTHRESH	0x1E
#define	DBCOOL_ADM1030_FAN_CHAR		0x20
#define	DBCOOL_ADM1031_FAN2_CHAR	0x21
#define	DBCOOL_ADM1030_FAN_SPEED_CFG	0x22
#define	DBCOOL_ADM1030_FAN_FILTER	0x23
#define	DBCOOL_ADM1030_L_TMIN		0x24
#define	DBCOOL_ADM1030_R_TMIN		0x25
#define	DBCOOL_ADM1031_R2_TMIN		0x26
#define	DBCOOL_ADM1030_DEVICEID		DBCOOL_DEVICEID_REG
#define	DBCOOL_ADM1030_COMPANYID	DBCOOL_COMPANYID_REG
#define	DBCOOL_ADM1030_REVISION		DBCOOL_REVISION_REG

/*      
 * Macros to locate limit registers for the various sensor types
 */     
#define DBCOOL_VOLT_LOLIM(reg) ((reg - DBCOOL_25VIN) * 2 + DBCOOL_25VIN_LOWLIM)
#define DBCOOL_VOLT_HILIM(reg) (DBCOOL_VOLT_LOLIM(reg) + 1)
#define DBCOOL_TEMP_LOLIM(reg)		\
		((reg - DBCOOL_LOCAL_TEMP) * 2 + DBCOOL_LOCAL_LOWLIM)
#define DBCOOL_TEMP_HILIM(reg) (DBCOOL_TEMP_LOLIM(reg) + 1)
#define DBCOOL_TACH_LOLIM(reg)		\
		(reg - DBCOOL_FAN1_TACH_LSB + DBCOOL_TACH1_MIN_LSB)
#define	ADM1030_TEMP_HILIM(reg)		\
		((reg - DBCOOL_ADM1030_L_TEMP) * 3 + DBCOOL_ADM1030_L_HI_LIM)
#define	ADM1030_TEMP_LOLIM(reg)		\
		((reg - DBCOOL_ADM1030_L_TEMP) * 3 + DBCOOL_ADM1030_L_LO_LIM)
#define ADT7466_LIM_OFFSET(reg)         \
		((reg - DBCOOL_AIN1) * 2 + DBCOOL_AIN1_LOWLIM)
#define ADT7466_FAN_LIM_OFFSET(reg)     \
		(reg - DBCOOL_FANA_LSB + DBCOOL_FANA_LOWLIM_LSB)


/* Company and Device ID values */
#define	DBCOOL_COMPANYID	0x41

#define	ADM1027_DEVICEID	0x27
#define	ADM1030_DEVICEID	0x30
#define	ADM1031_DEVICEID	0x31
#define	ADT7463_DEVICEID	0x27
#define	ADT7466_DEVICEID	0x66
#define	ADT7467_DEVICEID	0x68	/* The ADT7467/7468 cannot be */
#define	ADT7468_DEVICEID	0x68	/* distinguished by DEVICEID  */
#define	ADT7473_DEVICEID	0x73
#define	ADT7475_DEVICEID	0x75
#define	ADT7476_DEVICEID	0x76
#define	ADT7490_DEVICEID	0xFF	/* Device ID not used on 7490 */

#define	ADM1027_REV_ID		0x60
#define	ADT7463_REV_ID1		0x62
#define	ADT7463_REV_ID2		0x6A
#define	ADT7467_REV_ID1		0x71
#define	ADT7467_REV_ID2		0x72
#define	ADT7473_REV_ID1		0x68
#define	ADT7473_REV_ID2		0x69
#define	ADT7490_REV_ID		0x6E

#endif /* def DBCOOLREG_H */
