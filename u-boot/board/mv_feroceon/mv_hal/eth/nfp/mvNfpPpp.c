/*******************************************************************************
Copyright (C) Marvell Interpppional Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
Interpppional Ltd. and/or its affiliates ("Marvell") under the following
alterpppive licensing terms.  Once you have made an election to distribute the
File under one of the following license alterpppives, please (i) delete this
introductory statement regarding license alterpppives, (ii) delete the two
license alterpppives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

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
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

/*******************************************************************************
* mvPPP.c - Marvell Fast Route PPP Processing
*
* DESCRIPTION:
*       
*       Supported Features:
*       - OS independent. 
*
*******************************************************************************/


/* includes */
#include "mvTypes.h"
#include "mvOs.h"
#include "mvStack.h"
#include "mvDebug.h"
#include "eth/nfp/mvNfp.h"
#include "eth/mvEth.h"

#define MACFMT    "%02X:%02X:%02X:%02X:%02X:%02X"
#define MACSTR(m) m[0],m[1],m[2],m[3],m[4],m[5]

MV_FP_PPP_RULE pppOpen[ETH_FP_IFINDEX_MAX];	/* connected pppoe sessions */
MV_FP_PPP_RULE pppHalf[ETH_FP_IFINDEX_MAX]; /* half-open pppoe connections */


MV_STATUS mvFpPppInit(void)
{
	memset(pppOpen, 0, sizeof(MV_FP_PPP_RULE) * ETH_FP_IFINDEX_MAX);
	memset(pppHalf, 0, sizeof(MV_FP_PPP_RULE) * ETH_FP_IFINDEX_MAX);

    mvOsPrintf("NFP (pppoe) init %d entries, %d bytes\n", ETH_FP_IFINDEX_MAX,
					2 * sizeof(MV_FP_PPP_RULE) * ETH_FP_IFINDEX_MAX);
	return MV_OK;
}

/* Clear Fast Route Bridge Rule Database (SNAT + DNAT table) */
MV_STATUS mvFpPppClear(void)
{
	memset(pppOpen, 0, sizeof(MV_FP_PPP_RULE) * ETH_FP_IFINDEX_MAX);
	return MV_OK;
}

void mvFpPppDestroy(void)
{
	mvFpPppClear();
}

void mvFpPppRulePrint(char* text, MV_FP_PPP_RULE *rule)
{
		mvOsPrintf("NFP (pppoe): ppp %s (%s) if_ppp=%u if_eth=%u session=%u DA=" MACFMT " SA=" MACFMT " %x\n",
			   text,	
               rule->pppInfo.if_ppp ? "open":"half",
			   rule->pppInfo.if_ppp,
			   rule->pppInfo.if_eth,
			   rule->pppInfo.u.ppp.session,
			   MACSTR(rule->pppInfo.u.ppp.da),
			   MACSTR(rule->pppInfo.u.ppp.sa),
			   rule->pppInfo.channel);
}

MV_STATUS mvFpPppRuleSet(MV_FP_PPP_RULE *rule)
{
	int if_ppp = rule->pppInfo.if_ppp;
	int i = ETH_FP_IFINDEX_MAX;

#ifdef MV_FP_DEBUG
	mvFpPppRulePrint("new", rule);
#endif

	/* half open */
	if (!if_ppp) {
		memcpy(&pppHalf[rule->pppInfo.if_eth], rule, sizeof(MV_FP_PPP_RULE));
	}
	else while(i--) {
		/* look for channel id match */
		if (pppHalf[i].pppInfo.channel == rule->pppInfo.channel) {
			memcpy(&pppOpen[if_ppp], &pppHalf[i], sizeof(MV_FP_PPP_RULE));
			memset(&pppHalf[i], 0, sizeof(MV_FP_PPP_RULE));
			pppOpen[if_ppp].pppInfo.if_ppp = if_ppp;
			break;
		}
	}

    return MV_OK;
}

MV_STATUS mvFpPppRuleDel(MV_FP_PPP_RULE *rule)
{
	int i = ETH_FP_IFINDEX_MAX;

#ifdef MV_FP_DEBUG
	mvOsPrintf("NFP (pppoe): del ppp channel %x\n", rule->pppInfo.channel);
#endif

	while(i--) {
		if (pppOpen[i].pppInfo.channel == rule->pppInfo.channel) {
			mvFpPppRulePrint("del", &pppOpen[i]);
			pppOpen[i].pppInfo.if_ppp = 0;
			memset(&pppOpen[i], 0, sizeof(MV_FP_PPP_RULE));
			break;
		}
	}

    return MV_OK;
}

MV_U32 mvFpPppRuleAge(MV_FP_PPP_RULE *rule)
{
	return MV_OK;
}

MV_STATUS mvFpPppPrint(void)
{
	int i = ETH_FP_IFINDEX_MAX;

	while(i--) {
		if (pppOpen[i].pppInfo.if_eth) 
				mvFpPppRulePrint("", &pppOpen[i]);
		
		if (pppHalf[i].pppInfo.if_eth) 
				mvFpPppRulePrint("", &pppHalf[i]);
		
	}

    return MV_OK;
}


