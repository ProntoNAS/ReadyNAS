/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.

*******************************************************************************/

#include <common.h>
#if defined(CONFIG_CMD_EFUSE)
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"

#undef DEBUG

#ifdef DEBUG
#define DBPR(x...)		printf(x);
#else
#define DBPR(x...)
#endif

#define MASK_ALL_32BIT	0xFFFFFFFFUL

typedef enum _efuseDefId {
	EFUSE_RSAKEY_ID = 0,
	EFUSE_AESKEY_ID,
	EFUSE_BOXID_ID,
	EFUSE_FLASHID_ID,
	EFUSE_JTAGDIS_ID,
	EFUSE_BOOTSRC_ID,
	EFUSE_SECURE_ID,
	EFUSE_INVALID_ID
} efuseDefId;

typedef struct _efuseDef {
	char		*name;
	MV_U32		efuseNum;	/* Efuse number - for multi filed values, the number of the first field */
	MV_U32		regAddr;	/* Address of register used for eFuse value set before the burn process starts */
	efuseDefId	efid;
	MV_U32		length;		/* Number of 32-bit efuse words the value stored in */
	MV_U32		mask;		/* Value to apply to a single 32-bit word - valid only if length == 1*/
	MV_U32		offset;		/* Position ov bits inside the 32-bit word - valid only if length == 1*/
} efuseDef;

static efuseDef eFuses[] = {
	/* Name		eFuse Number			Reg.Address			eFuse ID
								Length	Mask			Offset*/
	{ "RSAKEY",	EFUSE_BURN_ADDR_RSA_KEY_0_63,	SEC_RSA_KEY_REG,		EFUSE_RSAKEY_ID,
								8,	MASK_ALL_32BIT,		0 },
	{ "AESKEY",	EFUSE_BURN_ADDR_AES_KEY_0_63,	SEC_AES_KEY_REG,		EFUSE_AESKEY_ID,
								4,	MASK_ALL_32BIT,		0 },
	{ "BOXID",	EFUSE_BURN_ADDR_BOX_ID,		SEC_BOX_ID_REG,			EFUSE_BOXID_ID,
								1,	MASK_ALL_32BIT,		0 },
	{ "FLASHID",	EFUSE_BURN_ADDR_SEC_CTRL,	SEC_BOOT_CTRL_REG,		EFUSE_FLASHID_ID,
								1,	SEC_FLASH_ID_MASK,	SEC_FLASH_ID_OFFS },
	{ "JTAGDIS",	EFUSE_BURN_ADDR_SEC_CTRL,	SEC_BOOT_CTRL_REG,		EFUSE_JTAGDIS_ID,
								1,	SEC_JTAG_DSBL_MASK,	SEC_JTAG_DSBL_OFFS },
	{ "BOOTSRC",	EFUSE_BURN_ADDR_SEC_CTRL,	SEC_BOOT_CTRL_REG,		EFUSE_BOOTSRC_ID,
								1,	SEC_BOOT_DEV_MASK,	SEC_BOOT_DEV_OFFS },
	{ "SECURE",	EFUSE_BURN_ADDR_SEC_CTRL,	SEC_BOOT_CTRL_REG,		EFUSE_SECURE_ID,
								1,	SEC_BOOT_MODE_MASK,	SEC_BOOT_MODE_OFFS }
};

/*******************************************************************************
*    write_efuse_hw
*          Run the procedure required for burning eFuse in the HW
*    INPUT:
*          efDef 	efuse definition structure
*          values	array of efuse entry values
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
static int write_efuse_hw(efuseDef *efDef, MV_U32 values[])
{
	MV_U32		efuseRegAddr, efuseNum;
	int		wIdx;
	int		ret = 0;
	MV_U32		regVal;

	/* Activate eFuse Access mode */
	regVal = MV_REG_READ(EFUSE_SW_CTRL_REG);
	DBPR("Read from %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
	regVal |= EFUSE_CTRL_ACCESS_MODE_MASK;
	MV_REG_WRITE(EFUSE_SW_CTRL_REG, regVal);
	DBPR("Write to  %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);

	/* Each eFuse is represented by 2 DWORDs + FSB in the HW. Walk trough every DWORD */
	for (wIdx = 0, efuseRegAddr = efDef->regAddr, efuseNum = efDef->efuseNum;
		wIdx < efDef->length;
		wIdx++, efuseRegAddr += 4, efuseNum += (wIdx & 0x1) == 0 ? 1 : 0) {

		printf("Burning the eFuse %#0x[%d] ... ", efuseRegAddr, efuseNum);
		DBPR("\n");

		/* Prepare register(s) values for the burn procedure */
		/* For fields that have to modify a part of DWORD, trigger eFuse value re-load */
		if (efDef->mask != MASK_ALL_32BIT) {
			DBPR("Upload values from efuse %08X\n", efuseRegAddr);
			regVal = MV_REG_READ(EFUSE_SW_CTRL_REG);
			DBPR("Read from %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
			regVal &= ~EFUSE_CTRL_BURN_ADDR_MASK;
			regVal |= ((efuseRegAddr - EFUSE_BASE_ADDR) <<
						EFUSE_CTRL_BURN_ADDR_OFFS) & EFUSE_CTRL_BURN_ADDR_MASK;
			regVal |= EFUSE_CTRL_UPLOAD_TRIG_MASK;
			MV_REG_WRITE(EFUSE_SW_CTRL_REG, regVal);
			DBPR("Write to  %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);

			/* Wait until the upload process is done in HW */
			for (regVal = 0; regVal == 0;) {
				mdelay(20);
				regVal = MV_REG_READ(EFUSE_SW_CTRL_REG);
				DBPR("Read from %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
				regVal &= EFUSE_CTRL_ACCESS_DONE_OFFS;
			}
			DBPR("Done uploading from efuse %08X\n", efuseRegAddr);

			/* Now the register content reflects the real eFuse value and it can be modified */
			regVal = MV_REG_READ(efuseRegAddr);
			DBPR("Read from %08X: [%08X]\n", efuseRegAddr, regVal);
			regVal |= (values[wIdx] << efDef->offset) & efDef->mask;
			MV_REG_WRITE(efuseRegAddr, regVal);
			DBPR("Write to  %08X: [%08X]\n", efuseRegAddr, regVal);
		} else {
			MV_REG_WRITE(efuseRegAddr, values[wIdx]);
			DBPR("Write to  %08X: [%08X]\n", efuseRegAddr, values[wIdx]);
		}

		/* Multi-DWORD eFuses are modified only once. The eFuse content is locked when second part of
		   64-bit eFuse is written. Activation of secure boot mode locks the secure boot control register */
		if (((wIdx & 0x1) != 0) || (efDef->efid == EFUSE_SECURE_ID)) {
			/* Protect this Efuse from further modifications (set the security bit) */
			regVal  = MV_REG_READ(EFUSE_PROTECTION_FSB_REG);
			DBPR("Read from %08X: [%08X]\n", EFUSE_PROTECTION_FSB_REG, regVal);
			regVal |= 0x1UL << efuseNum;
			MV_REG_WRITE(EFUSE_PROTECTION_FSB_REG, regVal);
			DBPR("Write to  %08X: [%08X]\n", EFUSE_PROTECTION_FSB_REG, regVal);
		}

		/* Set eFuse address and trigger the burn process */
		regVal = MV_REG_READ(EFUSE_SW_CTRL_REG);
		DBPR("Read from %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
		regVal &= ~EFUSE_CTRL_BURN_ADDR_MASK;
		regVal |= ((efuseRegAddr - EFUSE_BASE_ADDR) << EFUSE_CTRL_BURN_ADDR_OFFS) & EFUSE_CTRL_BURN_ADDR_MASK;
		regVal |= EFUSE_CTRL_BURN_TRIG_MASK;
		MV_REG_WRITE(EFUSE_SW_CTRL_REG, regVal);
		DBPR("Write to  %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);

		/* Wait until the burn process is done in HW */
		for (regVal = 0; (regVal & EFUSE_CTRL_ACCESS_DONE_OFFS) == 0;) {
			mdelay(20);
			regVal = MV_REG_READ(EFUSE_SW_CTRL_REG);
			DBPR("Read from %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
		}

		/* Check that everything is OK */
		if ((regVal & EFUSE_CTRL_BURN_OK_MASK) != 0)
			printf("DONE\n");
		else {
			printf("FAIL\n");
			ret = -1;
		}

	} /* For each DWORD in eFuse */

	/* Clear eFuse Access mode */
	regVal = MV_REG_READ(EFUSE_SW_CTRL_REG);
	DBPR("Read from %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
	regVal &= ~EFUSE_CTRL_ACCESS_MODE_MASK;
	MV_REG_WRITE(EFUSE_SW_CTRL_REG, regVal);
	DBPR("Write to  %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);

	return ret;
} /* end of write_efuse_hw */

/*******************************************************************************
*    do_efuse_set
*          Equire efuse value and burn the appropriate efuse field
*    INPUT:
*          name 	efuse entry name
*          value	efuse entry value
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
static int do_efuse_set(char *const name, char *const value)
{
	int 	fIdx, wIdx, bIdx;
	int 	ret = -1;
	MV_U32	efuseVal[8];
	char	asciiWord[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	char	*devList = NULL;
	char	*device;
	char	*efuseBytes = (char *)efuseVal;

	/* Walk trough the fuses definitions and find the efuse requested for writing */
	for (fIdx = 0; fIdx < EFUSE_INVALID_ID; fIdx++) {

		if (strcmp(eFuses[fIdx].name, name) != 0)
			continue;

		/* Special handling is needed for device names list */
		if (eFuses[fIdx].efid == EFUSE_BOOTSRC_ID) {

			efuseVal[0] = 0;
			for (devList = value; ; devList = NULL) {
				device = strtok(devList, ",");
				if (device == NULL)
					break;

				if (strcmp(device, "NAND") == 0)
					efuseVal[0] |= EFUSE_BOOT_SRC_NAND;
				else if (strcmp(device, "NOR") == 0)
					efuseVal[0] |= EFUSE_BOOT_SRC_NOR;
				else if (strcmp(device, "SPI") == 0)
					efuseVal[0] |= EFUSE_BOOT_SRC_SPI;
				else if (strcmp(device, "SATA") == 0)
					efuseVal[0] |= EFUSE_BOOT_SRC_SATA;
				else {
					printf("\nWrong device name in the list - \"%s\"\n", device);
					goto efuse_set_stop;
				}
			} /* For each device name in the list */

		} else {

			/* Check that value string does not exeed the boundary */
			if ((eFuses[fIdx].length > 1) ||
			    (eFuses[fIdx].mask == MASK_ALL_32BIT)) {
				if (strlen(value) != (eFuses[fIdx].length * 8)) {
					printf("\nWrong value length %d, expected %d HEX numbers\n",
						strlen(value), eFuses[fIdx].length * 8);
					goto efuse_set_stop;
				}
			}

			for (wIdx = 0; wIdx < eFuses[fIdx].length; wIdx++) {
				strncpy(asciiWord, (value + wIdx * 8), 8);
				/* The multi-DWORD values like RSA and AES key are kept in efuse
				   in a network bytes order (BE). SOC registers are always in LE */
				if (eFuses[fIdx].length > 1)
					efuseVal[wIdx] = (MV_U32)htonl(simple_strtoul(asciiWord, NULL, 16));
				else
					efuseVal[wIdx] = (MV_U32)simple_strtoul(asciiWord, NULL, 16);
			}
			if (eFuses[fIdx].length == 1) {
				/* For less than single 32-bit word feeds we should make sure that
				   the acquired value fits the specific efuse field */
				if (efuseVal[0] > eFuses[fIdx].mask >> eFuses[fIdx].offset) {
					printf("\nThe value %#08X cannot fit the efuse %s\n",
						efuseVal[0], eFuses[fIdx].name);
					goto efuse_set_stop;
				}
			}
		}
		/* Print the acquired values and warn the user for a last time... */
		printf("\nYou are about to set \"%s\" efuse [%08X] with value(s)\n\n",
			eFuses[fIdx].name, eFuses[fIdx].regAddr);

		if (eFuses[fIdx].length > 1) {
			for (bIdx = 0; bIdx < (eFuses[fIdx].length * 4); bIdx++)
				printf("%02X", efuseBytes[bIdx]);
		} else
			printf("%08X", efuseVal[0]);

		ret = 0;
		printf("\n\nThis is irreversible change! Type \"YES\" for continue, or any other key for cancel");
		readline(" ");
		if (strcmp(console_buffer,"YES") != 0) {
			printf("\nCancelling changes on user request!\n");
			goto efuse_set_stop;
		}

		break;

	} /* For every index in efuses array */

	if (ret == 0) {
		printf("\n");
		ret = write_efuse_hw(&eFuses[fIdx], efuseVal);
		if (ret != 0)
			printf("\nThe HW burn procedure has failed!\n\n");
	} else
		printf("\nBad efuse name, operation cancelled\n\n");

efuse_set_stop:

	return ret;
} /* end of do_efuse_set */

/*******************************************************************************
*    do_efuse_get
*          read efuse value
*    INPUT:
*          name 	efuse entry name
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
static int do_efuse_get(char *const name)
{
	int 	fIdx, wIdx, bIdx;
	int 	ret = -1;
	MV_U32	efuseVal;
	char	*efuseBytes = (char *)&efuseVal;

	for (fIdx = 0; fIdx < EFUSE_INVALID_ID; fIdx++) {

		if (strcmp(eFuses[fIdx].name, name) == 0) {
			ret = 0;

			printf("Value(s) obtained from eFuse \"%s\" [%#08X]:\n",
				eFuses[fIdx].name, eFuses[fIdx].regAddr);

			for (wIdx = 0; wIdx < eFuses[fIdx].length; wIdx++) {

				efuseVal = MV_REG_READ(eFuses[fIdx].regAddr + 4 * wIdx);
				if (eFuses[fIdx].length > 1) {
					for (bIdx = 0; bIdx < 4; bIdx++)
						printf("%02X", efuseBytes[bIdx]);
				} else {
					efuseVal &= eFuses[fIdx].mask;
					efuseVal >>= eFuses[fIdx].offset;
					printf("  %08X\t", efuseVal);
					if (eFuses[fIdx].efid == EFUSE_BOOTSRC_ID) {
						printf("(");
						if ((efuseVal & EFUSE_BOOT_SRC_SPI) != 0)
							printf("SPI ");
						if ((efuseVal & EFUSE_BOOT_SRC_NAND) != 0)
							printf("NAND ");
						if ((efuseVal & EFUSE_BOOT_SRC_SATA) != 0)
							printf("SATA ");
						if ((efuseVal & EFUSE_BOOT_SRC_NOR) != 0)
							printf("NOR ");
						printf(")");
					}
				} /* multi-DWORD efuse */
			} /* For each efuse DWORD */

			printf("\n");
		} /* If the right efuse name is found*/
	} /* For every index in efuses array */

	if (ret == -1)
		printf("\nBad efuse name, operation cancelled\n\n");

	return ret;
} /* end of do_efuse_get */

/*******************************************************************************
*    burn_efuse_cmd
*          handle efuse set/get command
*    RETURN:
*          0 on success
*******************************************************************************/
int burn_efuse_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	char	*cmd, *name;

	/* need at least 3 arguments */
	if (argc < 3)
		goto usage;

	cmd = argv[1];
	name = argv[2];

	if (strncmp(cmd, "set", 3) == 0) {
		if (argc != 4)
			goto usage;
		do_efuse_set(name, argv[3]);
	} else if (strncmp(cmd, "get", 3) == 0)
		do_efuse_get(name);

	return 0;
usage:
	return cmd_usage(cmdtp);
} /* end of burn_efuse_cmd */

U_BOOT_CMD(
	efuse,      4,     1,      burn_efuse_cmd,
	"eFuse manupilation subsystem for secure boot mode",
	"set name value - Write value to eFuse \"name\"\n"
	"efuse get name       - Read value from eFuse \"name\"\n"
	"===========================================================================================================\n"
	"Supported | RSAKEY   32 bytes- SHA-256 digest of RSA-2048 public key in DER format. For example:\n"
	"  eFuse   |                    64C50CA3A4AF47D7FC5F61A7D5C3FA58C483BA2E86F61E99460963A8DF3DCFE0\n"
	"  names   | AESKEY   16 bytes- AES-128 symmetrical key for image decryption in secure boot mode\n"
	"          |                    For example: 94E83C8A892E648462EE2178C93D2779\n"
	"          | BOXID    4 bytes - configures HEX Box ID for secure boot mode. For example DEADBEEF\n"
	"          | FLASHID  2 bytes - configures HEX Flash ID for secure boot mode. For example ABED\n"
	"          | JTAGDIS  1 bit   - disables JTAG interface in secure boot mode.\n"
	"          | BOOTSRC  8 bits  - configures boot devices enabled in secure boot mode.\n"
	"          |                    Accepts device names separated by commas. For example NAND,NOR,SPI,SATA\n"
	"          |                    Supported boot devices: NAND,NOR,SPI,SATA\n"
	"          | SECURE   1 bit   - Enables secure boot mode and protects all eFuses from further modifications\n"
	"          |                    Setting this value MUST be the LAST command in eFuse configuration sequence!\n"
	"===========================================================================================================\n"
	"WARNING:   EACH EFUSE CAN BE SET ONLY ONCE! PLEASE DOUBLE-CHECK YOUR DATA!\n"
	"===========================================================================================================\n"
	);

#endif /*defined(CONFIG_CMD_EFUSE)*/
