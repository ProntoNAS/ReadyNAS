/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
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
* mvNfpSec.c - Marvell Network Fast Processing with IPSec(Routing only)
*
* DESCRIPTION:
*       
*       Supported Features:
*       - OS independent. 
*
*******************************************************************************/

/* includes */
#include "mvOs.h"
#include "mvDebug.h"
#include "eth/nfp/mvNfp.h"
#include "eth/mvEth.h"
#include "eth/nfp/mvNfpSec.h"
#include "cesa/mvCesa.h"

/* IPSec SA & SPD DBs */
MV_NFP_SEC_SPD_RULE* spdInDb;
MV_NFP_SEC_SPD_RULE* spdOutDb;
MV_NFP_SEC_SA_ENTRY* saInDb;
MV_NFP_SEC_SA_ENTRY* saOutDb;

static MV_CESA_COMMAND cesaCmdArray[MV_NFP_SEC_Q_SIZE];
static MV_NFP_SEC_CESA_PRIV cesaPrivArray[MV_NFP_SEC_Q_SIZE + MV_NFP_SEC_REQ_Q_SIZE];
static int cesaCmdIndx = 0;
static int cesaPrivIndx = 0;

static MV_U32 spdInRuleCount;
static MV_U32 spdOutRuleCount;
static MV_U32 saInEntryCount;
static MV_U32 saOutEntryCount;
static MV_U32 secDbSize;

extern int cesaReqResources;

MV_STATUS mvNfpSecInit(MV_U32 dbSize)
{
	if(dbSize == 0)
		return MV_BAD_PARAM;

	spdInDb  = (struct _mv_nfp_sec_spd_rule*)mvOsMalloc(dbSize * (sizeof(struct _mv_nfp_sec_spd_rule)));
	spdOutDb = (struct _mv_nfp_sec_spd_rule*)mvOsMalloc(dbSize * (sizeof(struct _mv_nfp_sec_spd_rule)));
	saInDb   = (struct _mv_nfp_sec_sa_entry*)mvOsMalloc(dbSize * (sizeof(struct _mv_nfp_sec_sa_entry)));
	saOutDb  = (struct _mv_nfp_sec_sa_entry*)mvOsMalloc(dbSize * (sizeof(struct _mv_nfp_sec_sa_entry)));

	if((spdInDb == NULL) || (spdOutDb== NULL) || (saInDb == NULL) || (saOutDb == NULL)) {
		mvOsPrintf("NFP-IPSec Rules DB: Not Enough Memory\n");
	    		return MV_NO_RESOURCE;	
	}
	
	secDbSize = dbSize;
	spdInRuleCount = spdOutRuleCount = saInEntryCount = saOutEntryCount = 0;

	memset(spdInDb, 0, (dbSize * sizeof(struct _mv_nfp_sec_spd_rule)));
	memset(spdOutDb, 0, (dbSize * sizeof(struct _mv_nfp_sec_spd_rule)));
	memset(saInDb, 0, (dbSize * sizeof(struct _mv_nfp_sec_sa_entry)));
	memset(saOutDb, 0, (dbSize * sizeof(struct _mv_nfp_sec_sa_entry)));

	return MV_OK;

}

MV_STATUS mvNfpSecDbClear(MV_VOID)
{
	MV_U32 i;
	MV_NFP_SEC_SPD_RULE *pCurrSpdInRule, *pCurrSpdOutRule;
	MV_NFP_SEC_SA_ENTRY *pCurrSAInEntery, *pCurrSAOutEntery;

	if((spdInDb  == NULL) && (spdOutDb == NULL) && (saInDb == NULL) && (saOutDb == NULL))
		return MV_NOT_INITIALIZED;

	/* assume all 4 DBs are initialized */
	for(i = 0; i < secDbSize; i++) {
		pCurrSpdInRule = (spdInDb + i);
		pCurrSpdOutRule = (spdOutDb + i);
		pCurrSAInEntery = (saInDb + i);
		pCurrSAOutEntery = (saOutDb + i);
		mvOsFree(pCurrSpdInRule);
		mvOsFree(pCurrSpdOutRule);
		mvOsFree(pCurrSAInEntery);
		mvOsFree(pCurrSAOutEntery);
	}

	spdInDb = spdOutDb = NULL;
	saInDb = saOutDb = NULL;

	return MV_OK;
}


static INLINE MV_VOID mvNfpSecClearRange(MV_U8* addr, MV_U32 size)
{
	MV_U32 i;
	MV_U8  *align;

	align = (MV_U8*)((MV_U32)addr & ~0x1f);
	
	for(i = 0; align <= (addr+size); align += CPU_D_CACHE_LINE_SIZE)
		mvOsCacheLineFlushInv(NULL, align);
}

static INLINE MV_VOID mvNfpSecInvRange(MV_U8* addr, MV_U32 size)
{
	MV_U32 i;
	MV_U8  *align;

	align = (MV_U8*)((MV_U32)addr & ~0x1f);
	
	for(i = 0; align <= (addr+size); align += CPU_D_CACHE_LINE_SIZE)
		mvOsCacheLineInv(NULL, align);
}


/****************************************************/
/* warning: need to replace DB list with hash table */
/****************************************************/
MV_NFP_SEC_SPD_RULE* mvNfpSecSPDRuleSet(MV_NFP_SEC_SPD_RULE* pSpdRule, MV_NFP_SEC_RULE_DB_DIR inOut)
{
	MV_NFP_SEC_SPD_RULE* pCurrSpdRule;
	MV_U32 currRuleIndex = 0, spdRuleCount;

	pCurrSpdRule = (inOut ? spdOutDb : spdInDb);
	spdRuleCount = (inOut ? spdOutRuleCount : spdInRuleCount);

	if(spdRuleCount >= secDbSize)
		return NULL;

	/* search if rule already exists */
	while(currRuleIndex < spdRuleCount) {
		if((pCurrSpdRule->sIp == pSpdRule->sIp) &&
		    (pCurrSpdRule->dIp == pSpdRule->dIp)
#ifdef	MV_NFP_SEC_5TUPLE_KEY_SUPPORT
 		    && (pCurrSpdRule->proto == pSpdRule->proto) &&
		    (pCurrSpdRule->srcPort == pSpdRule->srcPort) &&
		    (pCurrSpdRule->dstPort == pSpdRule->dstPort)
#endif
		  ) {
			/* rule exists - return */
			return pCurrSpdRule;
		}
		currRuleIndex++;
		pCurrSpdRule++;
	}
	
	pCurrSpdRule = (inOut ? (spdOutDb+spdRuleCount) : (spdInDb+spdRuleCount));;
	memcpy(pCurrSpdRule, pSpdRule, sizeof(struct _mv_nfp_sec_spd_rule));
	inOut ? spdOutRuleCount++ : spdInRuleCount++ ;

	return pCurrSpdRule;

}

MV_NFP_SEC_SA_ENTRY* mvNfpSecSAEntrySet(MV_NFP_SEC_SA_ENTRY* pSAEntry, MV_NFP_SEC_RULE_DB_DIR inOut)
{

	MV_NFP_SEC_SA_ENTRY* pCurrSAEntery;
	MV_U32 currEntryIndex = 0, saEntryCount;

	pCurrSAEntery = (inOut ? saOutDb : saInDb);
	saEntryCount = (inOut ? saOutEntryCount : saInEntryCount);

	if(saEntryCount >= secDbSize)
		return NULL;

	/* search if rule already exists */
	while(currEntryIndex < saEntryCount) {
		if(pCurrSAEntery->spi == pSAEntry->spi) {
			/* rule exists - return */
			return pCurrSAEntery;
		}
		currEntryIndex++;
		pCurrSAEntery++;
	}

	
	pCurrSAEntery = (inOut ? (saOutDb+saEntryCount) : (saInDb+saEntryCount));
	memcpy(pCurrSAEntery, pSAEntry, sizeof(struct _mv_nfp_sec_sa_entry));
	inOut ? saOutEntryCount++ : saInEntryCount++ ;

	return pCurrSAEntery;


}

MV_STATUS mvNfpSecOutCheck(MV_PKT_INFO *pPktInfo)
{
	if(pPktInfo->pFrags->dataSize > MV_NFP_SEC_MAX_PACKET)
		return MV_OUT_OF_RANGE;
	return MV_OK;
}

INLINE MV_STATUS mvNfpSecInCheck(MV_PKT_INFO *pPktInfo, MV_NFP_SEC_SA_ENTRY* pSAEntry)
{
		/* TBD - sequence number */
		return MV_OK;
}

MV_NFP_SEC_SPD_RULE* mvNfpSecSPDRuleFind(MV_U32 dstIp, MV_U32 srcIp,
						MV_U8 proto, MV_U16 dport, MV_U16 sport, MV_NFP_SEC_RULE_DB_DIR inOut)
{
	MV_NFP_SEC_SPD_RULE* pCurrSpdRule;
	MV_U32 currRuleIndex = 0, spdRuleCount;

	pCurrSpdRule = (inOut ? spdOutDb : spdInDb);
	spdRuleCount = (inOut ? spdOutRuleCount : spdInRuleCount);

	/* SPD DB is empty */
	if(!spdRuleCount) 
		return NULL;

	/* scan IN/OUT SPD database for matching rule */
	while(currRuleIndex < spdRuleCount) {
		if((pCurrSpdRule->sIp == srcIp) &&
		    (pCurrSpdRule->dIp == dstIp)
#ifdef	MV_NFP_SEC_5TUPLE_KEY_SUPPORT
 		&& (pCurrSpdRule->proto == proto)
		    (pCurrSpdRule->srcPort == sport) &&
		    (pCurrSpdRule->dstPort == dport)
#endif
		  ) {
			/* rule found - return */
			return pCurrSpdRule;
		}
		currRuleIndex++;
		pCurrSpdRule++;
	}

		
		return NULL;

}

INLINE MV_VOID mvNfpSecBuildMac(MV_PKT_INFO *pPktInfo, MV_NFP_SEC_SA_ENTRY* pSAEntry)
{
        MV_802_3_HEADER *pMacHdr;

        pMacHdr = (MV_802_3_HEADER*)((MV_U8 *)(pPktInfo->pFrags[0].bufVirtPtr));
        memcpy(pMacHdr, &pSAEntry->tunnelHdr.dstMac, 12);
        pMacHdr->typeOrLen = 0x08;  /* stands for IP protocol code 16bit swapped */          
	return;	
}

INLINE MV_VOID mvNfpSecBuildIPTunnel(MV_PKT_INFO *pPktInfo, MV_NFP_SEC_SA_ENTRY* pSAEntry)
{
	MV_IP_HEADER *pIpHdr, *pIntIpHdr;
	MV_U16 newIpTotalLength;

	newIpTotalLength = pPktInfo->pFrags[0].dataSize - sizeof(MV_802_3_HEADER);	

	pIpHdr = (MV_IP_HEADER*)(pPktInfo->pFrags[0].bufVirtPtr + 
				sizeof(MV_802_3_HEADER));
	pIntIpHdr = (MV_IP_HEADER*)((MV_U8*)(pIpHdr) + sizeof(MV_IP_HEADER) + sizeof(MV_ESP_HEADER) + 
			pSAEntry->ivSize);

	/* TBD - review below settings in RFC */
	pIpHdr->version = 0x45; 
	pIpHdr->tos = 0; 
	pIpHdr->totalLength = MV_16BIT_BE(newIpTotalLength);
	pIpHdr->identifier = 0; 
	pIpHdr->fragmentCtrl = 0; 
	pIpHdr->ttl = pIntIpHdr->ttl -1 ; 
	pIpHdr->protocol = MV_IP_PROTO_ESP; 
	pIpHdr->srcIP = pSAEntry->tunnelHdr.sIp;
	pIpHdr->dstIP = pSAEntry->tunnelHdr.dIp;

	pPktInfo->status = ETH_TX_IP_NO_FRAG | ETH_TX_GENERATE_IP_CHKSUM_MASK | 
					(0x5 << ETH_TX_IP_HEADER_LEN_OFFSET);

	return;
}

/* Append sequence number and spi, save some space for IV */
INLINE MV_VOID mvNfpSecBuildEspHdr(MV_PKT_INFO *pPktInfo, MV_NFP_SEC_SA_ENTRY* pSAEntry)
{
	MV_ESP_HEADER *pEspHdr;

	pEspHdr = (MV_ESP_HEADER*)(pPktInfo->pFrags[0].bufVirtPtr + 
			sizeof(MV_802_3_HEADER) + sizeof(MV_IP_HEADER));	
	pEspHdr->spi = pSAEntry->spi;
	pSAEntry->seqNum = (pSAEntry->seqNum++);
	pEspHdr->seqNum = MV_32BIT_BE(pSAEntry->seqNum);
	return;	
}

MV_STATUS mvNfpSecEspProcess(MV_PKT_INFO *pPktInfo, MV_NFP_SEC_SA_ENTRY* pSAEntry)
{
	MV_CESA_COMMAND	*pCesaCmd;
	MV_CESA_MBUF cesaMbuf;
	MV_NFP_SEC_CESA_PRIV *pCesaPriv;
	MV_STATUS status;

	pCesaCmd = &cesaCmdArray[cesaCmdIndx++];
	cesaCmdIndx = cesaCmdIndx%(MV_NFP_SEC_Q_SIZE);
	pCesaPriv = &cesaPrivArray[cesaPrivIndx++];
	cesaPrivIndx = cesaPrivIndx%(MV_NFP_SEC_Q_SIZE + MV_NFP_SEC_REQ_Q_SIZE);

	pCesaPriv->pPktInfo = pPktInfo;
	pCesaPriv->pSaEntry = pSAEntry;
	pCesaPriv->pCesaCmd = pCesaCmd;

	pPktInfo->pFrags[0].bufVirtPtr += MV_NFP_SEC_ESP_OFFSET;
	pPktInfo->pFrags[0].bufPhysAddr += MV_NFP_SEC_ESP_OFFSET;
	pPktInfo->pFrags[0].dataSize -= MV_NFP_SEC_ESP_OFFSET;

	cesaMbuf.pFrags = pPktInfo->pFrags;
	cesaMbuf.numFrags = 1;
	cesaMbuf.mbufSize = pPktInfo->pFrags[0].dataSize ;	

	pCesaCmd->pReqPrv = (MV_VOID*)pCesaPriv;
	pCesaCmd->sessionId = pSAEntry->sid;
	pCesaCmd->pSrc = &cesaMbuf;
	pCesaCmd->pDst = &cesaMbuf;
	pCesaCmd->skipFlush = MV_TRUE;

	/* Assume ESP */
	pCesaCmd->cryptoOffset = sizeof(MV_ESP_HEADER) + pSAEntry->ivSize;
    	pCesaCmd->cryptoLength =  pPktInfo->pFrags[0].dataSize - (sizeof(MV_ESP_HEADER) 
				  + pSAEntry->ivSize + pSAEntry->digestSize);
	pCesaCmd->ivFromUser = 0; /* relevant for encode only */
	pCesaCmd->ivOffset = sizeof(MV_ESP_HEADER);
	pCesaCmd->macOffset = 0;
	pCesaCmd->macLength = pPktInfo->pFrags[0].dataSize - pSAEntry->digestSize;
	pCesaCmd->digestOffset = pPktInfo->pFrags[0].dataSize - pSAEntry->digestSize ;

	/* save original digest in case of decrypt+auth */
	if(pSAEntry->secOp == MV_NFP_SEC_DECRYPT) {
		memcpy(pCesaPriv->orgDigest,(pPktInfo->pFrags[0].bufVirtPtr + pCesaCmd->digestOffset), 
			pSAEntry->digestSize);
		mvNfpSecInvRange((pPktInfo->pFrags[0].bufVirtPtr + pCesaCmd->digestOffset),
				 pSAEntry->digestSize);
	}
	disable_irq(CESA_IRQ);
	status = mvCesaAction(pCesaCmd);
	enable_irq(CESA_IRQ);
	if(status != MV_OK) {
		mvOsPrintf("mvNfpSecEspProcess: mvCesaAction failed !!!\n");
	}

	return status;
}

MV_STATUS mvNfpSecOutgoing(MV_PKT_INFO *pPktInfo, MV_NFP_SEC_SA_ENTRY* pSAEntry)
{
	MV_U8* pTmp;
	MV_U32 cryptoSize, encBlockMod, flushHeaderSize;
	MV_BUF_INFO* 	pBuf = pPktInfo->pFrags;

	/* CESA Q is full drop. */
	if( cesaReqResources <= 1 )
		return 	MV_NO_RESOURCE;	

	if(MV_OK != mvNfpSecOutCheck(pPktInfo))
		return MV_NFP_NONE;
	
	/* ignore Marvell header */	
	pBuf->dataSize -= ETH_MV_HEADER_SIZE;
	pBuf->bufVirtPtr += ETH_MV_HEADER_SIZE;
	pBuf->bufPhysAddr += ETH_MV_HEADER_SIZE;

	cryptoSize = pBuf->dataSize - sizeof(MV_802_3_HEADER);
	/* Align buffer address to beginning of new packet - TBD handle VLAN tag, LLC */
	pBuf->bufAddrShift += (pSAEntry->ivSize + sizeof(MV_ESP_HEADER) + 
			     sizeof(MV_IP_HEADER));
	pBuf->bufVirtPtr -= pBuf->bufAddrShift;
	pBuf->bufPhysAddr -= pBuf->bufAddrShift;
	pBuf->dataSize += pBuf->bufAddrShift;

	encBlockMod = (cryptoSize % MV_NFP_SEC_ENC_BLOCK_SIZE);
	/* leave space for padLen + Protocol */
	if(encBlockMod > 14 ) {
		encBlockMod =  MV_NFP_SEC_ENC_BLOCK_SIZE - encBlockMod;
		encBlockMod += MV_NFP_SEC_ENC_BLOCK_SIZE;
	}
	else 
		encBlockMod =  MV_NFP_SEC_ENC_BLOCK_SIZE - encBlockMod;

	pBuf->dataSize += encBlockMod;
	pTmp = pBuf->bufVirtPtr + pBuf->dataSize;
	memset(pTmp - encBlockMod, 0, encBlockMod - 2);
	*((MV_U8*)(pTmp-2)) = (MV_U8)(encBlockMod-2);
	*((MV_U8*)(pTmp-1)) = (MV_U8)4;
	mvNfpSecClearRange(pTmp - encBlockMod, encBlockMod);
	
	pBuf->dataSize += pSAEntry->digestSize;
	mvNfpSecBuildEspHdr(pPktInfo, pSAEntry);
	mvNfpSecBuildIPTunnel(pPktInfo, pSAEntry);
	mvNfpSecBuildMac(pPktInfo, pSAEntry);

	/* flush & invalidate new MAC, IP, & ESP headers + old ip*/
	flushHeaderSize = pBuf->bufAddrShift + sizeof(MV_IP_HEADER) + sizeof(MV_802_3_HEADER);
	mvNfpSecClearRange(pBuf->bufVirtPtr, flushHeaderSize);

	return mvNfpSecEspProcess(pPktInfo, pSAEntry);
}

MV_STATUS mvNfpSecIncoming(MV_PKT_INFO *pPktInfo, MV_NFP_SEC_SA_ENTRY* pSAEntry)
{

	MV_BUF_INFO* pBuf = pPktInfo->pFrags;
	MV_U32 invSize;

	/* CESA Q is full drop. */
	if( cesaReqResources <= 1 )
		return 	MV_NO_RESOURCE;	

	/* TBD - duplicate invalidatation */
	if(MV_OK != mvNfpSecInCheck(pPktInfo, pSAEntry))
		return	MV_ERROR;
 
	/* ignore Marvell header */	
	pBuf->dataSize -= ETH_MV_HEADER_SIZE;
	pBuf->bufVirtPtr += ETH_MV_HEADER_SIZE;
	pBuf->bufPhysAddr += ETH_MV_HEADER_SIZE;

	/* update buffer address shift value */
	pPktInfo->pFrags->bufAddrShift -= (pSAEntry->ivSize + sizeof(MV_ESP_HEADER) + 
			     sizeof(MV_IP_HEADER));

	/* invalidate MAC, IP & ESP  headers */
	invSize = sizeof(MV_802_3_HEADER) + sizeof(MV_IP_HEADER) + sizeof(MV_ESP_HEADER);
	mvNfpSecInvRange(pBuf->bufVirtPtr, invSize);

	return mvNfpSecEspProcess(pPktInfo, pSAEntry);
}

MV_NFP_SEC_SA_ENTRY* mvNfpSecSARuleFind(MV_U32 spiPkt)
{
	MV_NFP_SEC_SA_ENTRY* pCurrSAEntery = saInDb;
	MV_U32 currEntryIndex = 0;

	while(currEntryIndex < saInEntryCount) {
		if(pCurrSAEntery->spi == spiPkt)
			return pCurrSAEntery;
		currEntryIndex++;
		pCurrSAEntery++;
	}

	return NULL;
	
}

MV_VOID mvNfpSecDbPrint(MV_VOID)
{
	MV_U32 i;

	printk("NFP-IPSec DB info:\n");
	for(i = 0; i < spdInRuleCount; i++)
		printk("inbound-entry #%d: srcip=0x%x, dstip=0x%x\n",i,spdInDb[i].sIp, spdInDb[i].dIp);

	for(i = 0; i < spdOutRuleCount; i++)
		printk("outbound-entry #%d: srcip=0x%x, dstip=0x%x\n",i,spdOutDb[i].sIp, spdOutDb[i].dIp);

}
