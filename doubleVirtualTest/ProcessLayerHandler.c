/*
�� �� ����ProcessLayerHandler.cpp
�������������̲�ҵ����ģ�飬����SMV��GOOSE�Ľ��룬�Լ������ж� ��ʵ��
��    �ߣ�
��    Ȩ��
�������ڣ�2012-09-19
�޸ļ�¼��
*/
//#include "stdafx.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "ProcessLayerHandler.h"
#include "common.h"
//#include "xmlHandler.h"

#define TRUE 			1
#define FALSE			0
#define true 				TRUE
#define false				FALSE

typedef unsigned short WORD;

static int ProcessQid[nCHANNEL];			/*Э�������Ϣ����ID*/
extern int errno;

GooseCfgInfo GooseCfg[nCHANNEL][nIED];		/*GOOSE������Ϣ*/
SmvCfgInfo	SmvCfg[nCHANNEL][nIED];			/*SMV������Ϣ*/

/*��xmlģ�����,��ͨ���Ż�ȡGoose���ýṹ���׵�ַ*/
GooseCfgInfo *plh_getGooseCfgInfoPtr(int chNo, int iedNo)
{
	return &GooseCfg[chNo][iedNo];
}

/*��xmlģ�����,��ͨ���Ż�ȡSMV���ýṹ���׵�ַ*/
SmvCfgInfo *plh_getSmvCfgInfoPtr(int chNo, int iedNo)
{
	return &SmvCfg[chNo-1][iedNo];
}



bool GetFieldLength(unsigned char * pointer, int *nValSize,int *nLenSize) 
{
	unsigned char * pPointer = pointer;

	*nValSize = *pPointer;
	*nLenSize = 1;

	unsigned char nLenBytes = 0; 

	if(*nValSize >= 128)
	{
		nLenBytes = *nValSize & 0x7f;
		pPointer += 1;
		switch(nLenBytes)
		{
		case 1:
			*nValSize   = *(unsigned char *)pPointer;
			*nLenSize  = 1 + 1;
			break;
		case 2:
			*nValSize   = ((unsigned short)((*(unsigned char*)(pPointer+1)))<<8)
				| (*(unsigned char*)(pPointer+1));
			*nLenSize  = 2 + 1;
			break;
		case 3://
			*nValSize   = ((unsigned long)((*(unsigned char*)(pPointer)))<<16) 
				| ((unsigned short)((*(unsigned char*)(pPointer+1)))<<8)
				| (*(unsigned char*)(pPointer+1));
			*nLenSize  = 3 + 1;
			break;
		case 4:
			*nValSize   = ((unsigned long)((*(unsigned char*)(pPointer)))<<24) 
				| ((unsigned long)((*(unsigned char*)(pPointer)))<<16) 
				| ((unsigned short)((*(unsigned char*)(pPointer+1)))<<8)
				| (*(unsigned char*)(pPointer+1));
			*nLenSize  = 4 + 1;
			break;
		default:
			PLH_DEBUG("ProcessLayerHandler.c >>  GetFieldLength >>  error happened......\n");
			return FALSE;


		}

	}
	//
	return TRUE;

}

long long GetFieldValue(unsigned char * pointer, int nSize)
{
	long long nRet = 0;
	if(nSize > 8 || nSize <0 )
	{
		//
		return 0;
	}
	//
	if(nSize == 1)
	{
		nRet = *pointer;
	}
	else if(nSize == 2)
	{
		nRet = (((unsigned short)(*(unsigned char*)(pointer+1)))<<8)
			|(*(unsigned char*)(pointer + 2));
	}
	else if(nSize == 3)
	{
		nRet = (((unsigned long)(*(unsigned char*)pointer))<<16)
			|(((unsigned short)(*(unsigned char*)(pointer+1)))<<8)
			|(*(unsigned char*)(pointer+2));
	}
	else if(nSize == 4)
	{
		nRet = (((unsigned long)(*(unsigned char*)(pointer+1)))<<24)
			| (((unsigned long)(*(unsigned char*)(pointer+2)))<<16)
			| (((unsigned short)(*(unsigned char*)(pointer+3)))<<8)
			| (*(unsigned char*)(pointer+4));
	}
	else if(nSize == 5)
	{
		nRet = (((long long)(*(unsigned char*)(pointer)))<<32)
			| (((unsigned long)(*(unsigned char*)(pointer+1)))<<24)
			| (((unsigned long)(*(unsigned char*)(pointer+2)))<<16)
			| (((unsigned short)(*(unsigned char*)(pointer+3)))<<8)
			| (*(unsigned char*)(pointer+4));
	}
	else if(nSize == 6)
	{
		nRet = (((long long)(*(unsigned char*)(pointer)))<<40)
			| (((long long)(*(unsigned char*)(pointer+1)))<<32)
			| (((unsigned long)(*(unsigned char*)(pointer+2)))<<24) 
			| (((unsigned long)(*(unsigned char*)(pointer+3)))<<16)
			| (((unsigned short)(*(unsigned char*)(pointer+4)))<<8)
			| (*(unsigned char*)(pointer+5));
	}
	else if(nSize == 7)
	{
		nRet =(((long long)(*(unsigned char*)(pointer)))<<48) 
			| (((long long)(*(unsigned char*)(pointer+1)))<<40)
			| (((long long)(*(unsigned char*)(pointer+2)))<<32)
			| (((unsigned long)(*(unsigned char*)(pointer+3)))<<24) 
			| (((unsigned long)(*(unsigned char*)(pointer+4)))<<16)
			| (((unsigned short)(*(unsigned char*)(pointer+5)))<<8)
			| (*(unsigned char*)(pointer+6));
	}
	else if(nSize == 8)
	{
		nRet =(((long long)(*(unsigned char*)(pointer)))<<56)
			| (((long long)(*(unsigned char*)(pointer+1)))<<48) 
			| (((long long)(*(unsigned char*)(pointer+2)))<<40)
			| (((long long)(*(unsigned char*)(pointer+3)))<<32)
			| (((unsigned long)(*(unsigned char*)(pointer+4)))<<24) 
			| (((unsigned long)(*(unsigned char*)(pointer+5)))<<16)
			| (((unsigned short)(*(unsigned char*)(pointer+6)))<<8)
			| (*(unsigned char*)(pointer+7));
	}
	else
		PLH_DEBUG("ProcessLayerHandler.c   GetFieldValue  ......\n");

	return nRet;

}


/*********************************************************************

��    �ܣ�Goose����-���ڹ����ж�   
���������pszSourceΪû�н���ǰ��ԭʼ���ݰ�ָ�룬
          nFileFormatΪ�ļ���ʽ
���������packetInfoΪ��������ݽṹ

�� �� ֵ������ɹ����� true������ʧ�ܷ��� false

**********************************************************************/
bool  GooseDecode(unsigned char * pszSource, int nFileFormat,GoosePacketInfo *packetInfo)
{
	int nLenSize = 0;
	int nValSize = 0;

	int nVlanSize = 0;

	unsigned char *pEtherTypeHeader = NULL;
	unsigned char *pPointer = NULL;
	unsigned short wTCI = 0;

	#if 0
	if( *(unsigned short *)(pszSource + sizeof(SelfDefPacketHeader) + 12) == 0x8100)//��vlan
		nVlanSize = 4;
	#endif
	/*fpga֡ͷ��ʽpayloadǰ��22�ֽ�,����������Խṹ���ڴ��Զ�������24�ֽ�,��Ϊ��*/
	if (*(unsigned short *)(pszSource + SELFDEF_HEADER_LEN + 12) == 0x8100)
	{
		nValSize = 4;
	}
#if 0
	int i;
	pPointer = pszSource;
	for (i = 0; i < 64; i++)
	{
		PLH_DEBUG("%x ", *pPointer);
		pPointer++;
		if ((i+1) % 16 == 0)
			PLH_DEBUG("\n");
	}
	pPointer = NULL;
#endif

	/*pEtherTypeHeader == 0x88b8 goose*/
	pEtherTypeHeader = pszSource + SELFDEF_HEADER_LEN + 12 + nVlanSize;
	//
	pPointer = pEtherTypeHeader + 10;//apduTag
	
	// 1.vlan info
	packetInfo->wAppID = *(unsigned short *)(pEtherTypeHeader + 2);
	
	if(nValSize > 0)//����VLAN
	{
		//priority
		//wTCI = *(unsigned short*)(pszSource + sizeof(SelfDefPacketHeader) + 14);
		wTCI = *(unsigned short*)(pszSource + SELFDEF_HEADER_LEN + 14);
		packetInfo->nPriority = (wTCI>>5) & 0x7;
		packetInfo->nVlanID   = wTCI & 0xf;
	}
	
	// 2.ҵ����Ϣ
	if( *(unsigned char*)pPointer == TAG_GOOSE_APDU)
	{
		// adpu length
		pPointer += 1;
		GetFieldLength(pPointer,&nValSize,&nLenSize);
		
		//ָ����һ����
		pPointer += nLenSize;

	}
	else
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >>The tag of  goose Apdu is error ......\n");
		return false;
	}
	//////////////////////////////////////////////////////////////
	//
	// gocbRef
	//
	//////////////////////////////////////////////////////////////
	// gocbRef tag
	if(*(unsigned char *) pPointer != TAG_GOOSE_GOCBREF)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> The tag of gocbRef is error ......\n");
		return false;
	}
	//gocbRef length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	//gocbRef val
	pPointer += nLenSize;
	if(nValSize < sizeof(packetInfo->szGocbRef))
		memcpy(&packetInfo->szGocbRef, pPointer, nValSize);
	else
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >>  the length of gocbRef is error ......\n");
	pPointer += nValSize;
	//////////////////////////////////////////////////////////////
	//
	// TimeAllowedToLive
	//
	//////////////////////////////////////////////////////////////
	// TimeAllowedToLive tag
	if(*(unsigned char *) pPointer != TAG_GOOSE_TIMEALLOWEDTOLIVE)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> The tag of TimeAllowedToLive is error ......\n");
		return false;
	}
	//TimeAllowedToLive length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	//TimeAllowedToLive val
	pPointer += nLenSize;
	pPointer += nValSize;
	
	//////////////////////////////////////////////////////////////
	//
	// dataSetRef
	//
	//////////////////////////////////////////////////////////////
	// dataSetRef tag
	if(*(unsigned char *) pPointer != TAG_GOOSE_DATASETREF)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> The tag of  dataSetRef is error ......\n");
		return false;
	}
	//dataSetRef length
	pPointer += 1;
	GetFieldLength(pPointer,&nValSize,&nLenSize);
	//dataSetRef val
	pPointer += nLenSize;
	if(nValSize < sizeof(packetInfo->szDataSetRef))
		memcpy(packetInfo->szDataSetRef,pPointer,nValSize);
	else
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >>  the length of  dataSetRef is error ......\n");
	pPointer += nValSize;
	//////////////////////////////////////////////////////////////
	//
	// GoID
	//
	//////////////////////////////////////////////////////////////
	// GoID tag 
	if(*(unsigned char *) pPointer != TAG_GOOSE_GOID)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> The tag of GoID is error ......\n");
		return false;
	}
	//GoID length
	pPointer += 1;
	GetFieldLength(pPointer,&nValSize,&nLenSize);
	//GoID val
	pPointer += nLenSize;
	if(nValSize < sizeof(packetInfo->szGoID))
		memcpy(packetInfo->szGoID,pPointer,nValSize);
	else
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >>  the length of  GoID is error ......\n");
	pPointer += nValSize;
	
	//////////////////////////////////////////////////////////////
	//
	// TimeStamp
	//
	//////////////////////////////////////////////////////////////
	// TimeStamp tag
	if(*(unsigned char *) pPointer != 0x84)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> The tag of TimeStamp is error ......\n");
		return false;
	}
	//TimeStamp length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	pPointer += nLenSize;
	pPointer += nValSize;
	//////////////////////////////////////////////////////////////
	//
	// stNum
	//
	//////////////////////////////////////////////////////////////
	// stNum tag
	if(*(unsigned char *) pPointer != TAG_GOOSE_STNUM)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> The tag of stNum is error ......\n");
		return false;
	}
	//stNum length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	//stNum val
	pPointer += nLenSize;
	packetInfo->stNum = (unsigned long)GetFieldValue(pPointer,nValSize); 
	
	pPointer += nValSize;
	//////////////////////////////////////////////////////////////
	//
	// sqNum
	//
	//////////////////////////////////////////////////////////////
	// sqNum tag
	if(*(unsigned char *) pPointer != TAG_GOOSE_SQNUM)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> The tag of  sqNum is error ......\n");
		return false;
	}
	//sqNum length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	//sqNum val
	pPointer += nLenSize;
	//
	packetInfo->sqNum = (unsigned long)GetFieldValue(pPointer,nValSize); 
	
    pPointer += nValSize;
	//////////////////////////////////////////////////////////////
	//
	// testMode
	//
	//////////////////////////////////////////////////////////////
	// testMode tag
	if(*(unsigned char *) pPointer != TAG_GOOSE_TEST)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> The tag of testMode is error ......\n");
		return false;
	}
	//testMode length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	//testMode val
	pPointer += nLenSize;
	//
	packetInfo->bTest = (unsigned char)GetFieldValue(pPointer,nValSize) == 0 ? false : true;
	pPointer += nValSize;
	//////////////////////////////////////////////////////////////
	//
	// confRev
	//
	//////////////////////////////////////////////////////////////
	// confRev tag
	if(*(unsigned char *) pPointer != TAG_GOOSE_CONFREV)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> the tag of confRev is error ......\n");
		return false;
	}
	//confRev length
	pPointer += nValSize;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	//confRev val
	pPointer += nLenSize;
	pPointer += nValSize;
	//////////////////////////////////////////////////////////////
	//
	//Need commision
	//
	//////////////////////////////////////////////////////////////
	//  tag
	if(*(unsigned char *) pPointer != TAG_GOOSE_NEEDCOM)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> the tag of Needcommision is error ......\n");
		return false;
	}
	// length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	// val
	pPointer += nLenSize;
	//
	packetInfo->bNdsCom = ((unsigned char)GetFieldValue(pPointer,nValSize) == 0 ? false : true);
	pPointer += nValSize;
	//////////////////////////////////////////////////////////////
	//
	// Dataset Entries Num
	// 
	//////////////////////////////////////////////////////////////
	if(*(unsigned char *) pPointer != TAG_GOOSE_DATASETENTRIESNUM)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> GooseDecode >> The tag of DatasetEntriesNum  is error ......\n");
		return false;
	}
	// length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	// val
	pPointer += nLenSize;
	packetInfo->nDataEntriesNum = (unsigned long)GetFieldValue(pPointer,nValSize);
	pPointer += nValSize;
	//......
  return TRUE;
}



/*********************************************************************

��    �ܣ�Smv����-���ڹ����ж�
���������pszSourceΪû�н���ǰ��ԭʼ���ݰ�ָ�룬
          nFileFormatΪ�ļ���ʽ
���������packetInfoΪ��������ݽṹ

�� �� ֵ������ɹ����� true������ʧ�ܷ��� false

**********************************************************************/
bool SmvDecode(unsigned char * pszSource, int nFileFormat,SmvPacketInfo *packetInfo)
{
	int nLenSize = 0;
	int nValSize = 0;
	unsigned char * pEtherTypeHeader;
	int nVlanSize = 0;
	int i;
	unsigned short wTCI;
	
	if( *(unsigned short *)(pszSource + SELFDEF_HEADER_LEN + 12) == 0x8100)//��vlan
		nVlanSize = 4;
	
	pEtherTypeHeader = pszSource+ SELFDEF_HEADER_LEN + 12 + nVlanSize;
	
	memcpy(packetInfo->szDstMac,pszSource + SELFDEF_HEADER_LEN,6);
	
	unsigned char * pPointer = pEtherTypeHeader + 10;//apduTag
	packetInfo->wAppID = *(unsigned short *)(pEtherTypeHeader + 2);
	
	// 1.vlan info
	if(nValSize > 0)//����VLAN
	{
		//priority
		wTCI = *(unsigned short*)(pszSource  + SELFDEF_HEADER_LEN + 14);
		packetInfo->nPriority = (wTCI>>5) & 0x7;
		packetInfo->nVlanID   = wTCI & 0xf;
	}
	// 2.ҵ����Ϣ
	if( *(unsigned char*)pPointer == TAG_APDU_91)//91�����9-1��ҵ������Ҳ������
	{
		packetInfo->nType = SMV_91;
		return true;
	}

	if(*(unsigned char*)pPointer != TAG_APDU_92)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> SmvDecode >> The tag of Apduprot is error ......\n");
		return false;
	}
	//
	packetInfo->nType = SMV_92;

	//apdu length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	pPointer += nLenSize;
	/////////////////////////////////////////////////////////////
	//
	//  AsduNum
	//
	/////////////////////////////////////////////////////////////
	//AsduNum tag = 0x80)
	if(*(unsigned char*)pPointer != TAG_ASDU_NUM)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> SmvDecode >> The tag of AsduNum is error ......\n");
		return false;
	}
	//AsduNum Length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
	//AsduNum
	pPointer += nLenSize;
	int nAsduNum = (int)GetFieldValue(pPointer,nValSize);
	packetInfo->nAsduNum = nAsduNum;
       pPointer += nValSize;
	////////////////////////////////////////////////////////////
	//
    //  security tag
	//
	/////////////////////////////////////////////////////////////
	if(*(unsigned char *)pPointer == TAG_SECURITY)
	{
		//security length
		pPointer += 1;
		GetFieldLength(pPointer, &nValSize, &nLenSize);
		//
		pPointer += nLenSize;
		pPointer += nValSize;
	}
	
	///////////////////////////////////////////////////////////////
	//
	//  sequence of Asdu
	//
	///////////////////////////////////////////////////////////////
	//sequence of Asdu tag
	if(*(unsigned char *)pPointer != TAG_SEQUENCE_OF_ASDU)
	{
		PLH_DEBUG("ProcessLayerHandler.c >> SmvDecode >> The tag of sequenceOfAsdu is error ......\n");
		return false;
	}
	// sequence of Asdu length
	pPointer += 1;
	GetFieldLength(pPointer, &nValSize, &nLenSize);
    	int nAsduLen = nValSize;
	//first Asdu tag
	pPointer += nLenSize;
	
	//
	for(i=0; i < nAsduNum; i++)
	{

		//asdu tag 0x30
		if(*(unsigned char*)pPointer != TAG_ASDU)
		{
			PLH_DEBUG("ProcessLayerHandler.c >> SmvDecode >> The tag of asdu is error ......\n");
			return false;
		}
		// Asdu length
		pPointer += 1;
		GetFieldLength(pPointer, &nValSize, &nLenSize);
		//int nAsduLen = nValSize;
		pPointer += nLenSize;

		////////////////////////////////////////////////////////
		//
		// svID
		//
		////////////////////////////////////////////////////////
		//svID tag 0x80
		if( *(unsigned char*)pPointer != TAG_SV_ID)
		{
			PLH_DEBUG("ProcessLayerHandler.c >> SmvDecode >> The tag of svID is error ......\n");
			return false;
		}

		//svID len
		pPointer += 1;
		GetFieldLength(pPointer, &nValSize, &nLenSize);
		pPointer += nLenSize;
		pPointer += nValSize;

		////////////////////////////////////////////////
		//
		// dataSetRef ��9-2��
		//
		////////////////////////////////////////////////
		//datasetRef tag 0x81
        if(*(unsigned char*)pPointer == TAG_DATASET_REF)
		{
			pPointer += 1;
			GetFieldLength(pPointer, &nValSize, &nLenSize);
			pPointer += nLenSize;
			pPointer += nValSize;
		}

		///////////////////////////////////////////////////////
		//
		// smpcnt
		//
		///////////////////////////////////////////////////////
		//smpCnt tag = 0x82
		if(*(unsigned char *)pPointer != TAG_SMP_CNT)
		{
			PLH_DEBUG("ProcessLayerHandler.c >> SmvDecode >> The tag of smpCnt is error ......\n");
			return false;
		}
		//smpCnt length
		pPointer += 1;
		GetFieldLength(pPointer, &nValSize, &nLenSize);
		//smdCnt Value
		pPointer += nLenSize;
		packetInfo->asdu[i].wSmpCnt =  *(WORD*)pPointer;
		pPointer += nValSize;

		///////////////////////////////////////////////////////////
		//
		// confRev
		//
		///////////////////////////////////////////////////////////
		//tag 0x82
		if(*(unsigned char*)pPointer != TAG_CONF_REV)
		{
			PLH_DEBUG("ProcessLayerHandler.c >> SmvDecode >> The tag of confRev is error ......\n");
			return false;
		}
		// confRev length == 04
		pPointer+=1;
		GetFieldLength(pPointer, &nValSize, &nLenSize);
		//confRev value
		pPointer += nLenSize;
		pPointer += nValSize;

		////////////////////////////////////////////////////////
		//
		// refrTm  0x84 , 9-2 ��
		//
		////////////////////////////////////////////////////////
		//refrTm tag 0x84
		if(*(unsigned char*)pPointer == TAG_REFR_TM)
		{
			pPointer += 1;
			GetFieldLength(pPointer, &nValSize, &nLenSize);
			//refrTm value
			pPointer += nLenSize;
			pPointer += nValSize;
		}

		///////////////////////////////////////////////////////////
		//
		// smpSynch
		//
		///////////////////////////////////////////////////////////
		//smpSynch tag 0x85
		if( *(unsigned char*)pPointer != TAG_SMP_SYNCH)
		{
			PLH_DEBUG("ProcessLayerHandler.c >> SmvDecode >> The tag of smpSynch is error ......\n");
			return false;
		}
		//smpSynch Length
		pPointer += 1;
		GetFieldLength(pPointer, &nValSize, &nLenSize);
		//smpSynch value
		pPointer += nLenSize;
		packetInfo->asdu[i].bSmpSynch = *(unsigned char*)pPointer == 0 ? false : true;
       	pPointer += nValSize;

		/////////////////////////////////////////////////////////////
		//
		// smpRate (9-2��)
		//
		/////////////////////////////////////////////////////////////
		//smpRate tag 0x86
		if(*(unsigned char*)pPointer == TAG_SMP_RATE)
		{
			pPointer += 1;
			GetFieldLength(pPointer, &nValSize, &nLenSize);
			pPointer += nLenSize;
			pPointer += nValSize;

		}

		//////////////////////////////////////////////////////////
		//
		// dataset
		//
		//////////////////////////////////////////////////////////
		//dataset tag 0x87
        if(*(unsigned char*)pPointer != TAG_DATASET)
		{
			PLH_DEBUG("ProcessLayerHandler.c >> SmvDecode >> The tag of dataset is error ......\n");
			return false;
		}
		//dataset length
		pPointer += 1;
		int nDatasetLength = 0;
		int nDatasetLenSize = 0;
		GetFieldLength(pPointer, &nDatasetLength, &nDatasetLenSize);
		//dataset
		int nChnlNum;
		nChnlNum = nDatasetLength/8;//һ�����ݼ��ڵ�ͨ����
		packetInfo->asdu[i].nChnlNum = nChnlNum;
		pPointer += nDatasetLenSize;
		
		int j;
		for(j = 0; j < nChnlNum; j++)
		{
			packetInfo->asdu[i].dataSet[j].v = *(long*)pPointer;
			pPointer += 4;

			packetInfo->asdu[i].dataSet[j].q = *(unsigned long*)pPointer;
			pPointer += 4;

			if( j > MAX_SMV_CHNL_NUM )
				break;
		}
		
		if(i > MAX_SMV_ASDU_NUM)
			break;

	}
	
	return true;

}


unsigned long SmvErrAnalyse(SmvPacketInfo *firstPacketInfo, SmvPacketInfo *secondPacketInfo, int nDiffTime, int chNo)
{
	unsigned long dwErrWord = 0;
	int i = 0;
	SmvCfgInfo *smvCfg;
	static unsigned count = 0;

	count++;
	
	if ((count % 4000) == 0)
	{
		PLH_DEBUG("secondPacketInfo, appId:%d, nPriority:%d, nVlanID:%d\n",
				secondPacketInfo->wAppID, secondPacketInfo->nPriority, secondPacketInfo->nVlanID);
	}

	/*����Ч�ʽϵ�,��ʼ�����ý��������������¼�������õ�����,��������ıȽϴ���
	�����ٸ�*/
	for (i = 0; i < nIED; i++)
	{
		smvCfg = plh_getSmvCfgInfoPtr(chNo, i);
		if ((count % 4000) == 0)
		{
			PLH_DEBUG("plh_getSmvCfgInfoPtr, appId:%d, nPriority:%d, nVlanID:%d\n",
					smvCfg->appId, smvCfg->nPriority, smvCfg->nVlanID);
		}
		
		if (memcmp((unsigned char *)&secondPacketInfo->nPriority, (unsigned char *)smvCfg, sizeof(SmvCfgInfo)) == 0)
		{
			/*�ҵ�ƥ����,���������澯,��Ϊ����,���﷢�ͼٸ澯,��̨�յ��澯˵����ƥ����
			ͬʱ��֤�˸澯��Ϣ���շ�����,����ǰ����ȽϹ��ܺ��ⲿ��ɾ��*/
			
			if ((count % 4000) == 0)
			{
				bs_smvWarningMsgSend();
			}
			
			break;
		}
	}

	/*���û�и���������ƥ���,���˳�*/
	if (i == nIED)
	{
		if ((count % 8000) == 0)
		{
			PLH_DEBUG("SmvErrAnalyse no match \n");
		}
		return -1;
	}
	
	/*��һ����������Ϣ���жϱȽ�*/
	/*
	if(secondpacketInfo->nPriority != atoi(pSmvCfgInfo->strPriority.c_str()))
		SETBIT(dwErrWord,SMV_Priority);

	if(secondpacketInfo->nVlanID != atoi(pSmvCfgInfo->strVlanId.c_str()))
		SETBIT(dwErrWord,SMV_VlanID);

	if(secondpacketInfo->wAppID != atoi(pSmvCfgInfo->strAppId.c_str()))
		SETBIT(dwErrWord,SMV_APPID);

	byte * pszDstMac = 	m_MacAddrConvert.Str2ByteArray(pSmvCfgInfo->strDestMac.c_str());
	if(!memcmp(secondpacketInfo->szDstMac,pszDstMac,6))
		SETBIT(dwErrWord,SMV_DstMAC);

	if( secondpacketInfo->asdu[0].nChnlNum != pSmvCfgInfo->smv92.lstSMV92Channel.size())
		SETBIT(dwErrWord,SMV_DataNum);
	*/

	//�ڶ�����ҵ����Ϣ���ж�
	////////////////////////////////////////////////////////
	// ������ʧ SMV_SmpPointLost
	////////////////////////////////////////////////////////

	//���1 ��second > first��
	#if 0
	if( (secondPacketInfo->asdu[0].wSmpCnt > firstPacketInfo->asdu[0].wSmpCnt ) 
		&& (secondPacketInfo->asdu[0].wSmpCnt > ( firstPacketInfo->asdu[0].wSmpCnt + (secondPacketInfo->nAsduNum - 1) + 1 )) 
		)
	{
		SETBIT(dwErrWord,SMV_SmpPointLost);
	}
	

	////////////////////////////////////////////////////////
	// �����ظ�
	////////////////////////////////////////////////////////

	//��second == first��
	if( secondPacketInfo->asdu[0].wSmpCnt == firstPacketInfo->asdu[0].wSmpCnt )
	{
		SETBIT(dwErrWord,SMV_SmpPointRepeated);
	}

	////////////////////////////////////////////////////////
	// ��������
	////////////////////////////////////////////////////////
	if( (secondPacketInfo->asdu[0].wSmpCnt < firstPacketInfo->asdu[0].wSmpCnt ) 
		&& (secondPacketInfo->asdu[0].wSmpCnt != 0) 
		)
	{
		SETBIT(dwErrWord,SMV_SmpReverseOrder);
	}
		
	////////////////////////////////////////////////////////
	// ����ͬ����ʧ SMV_SmpSynLost 
	////////////////////////////////////////////////////////
	if( secondPacketInfo->asdu[0].bSmpSynch == false)
	{
		SETBIT(dwErrWord,SMV_SmpSynLost);
	}

	////////////////////////////////////////////////////////
	// ����Ƶ���쳣 SMV_SmpFreq (�ٶ�ÿ�� 4000��,Ƕ��ʽ����ʹ��ʱ��Ҫ�������á�������)
	////////////////////////////////////////////////////////
	/*
	if(nDiffTime > (250*secondpacketInfo->nAsduNum + atoi(pSmvCfgInfo->strSmpTimeDeviation.c_str())*secondpacketInfo->nAsduNum) || (nDiffTime < (250 - atoi(m_pSmvCfgInfo->strSmpTimeDeviation.c_str())*secondpacketInfo->nAsduNum) ))
	{
		SETBIT(dwErrWord,SMV_SmpFreq);
	}
	*/
	int i;
	////////////////////////////////////////////////////////
	// ����ֵƷ����(Ʒ���쳣���ж�)
	////////////////////////////////////////////////////////
	for(i = 0; i < secondPacketInfo->asdu[0].nChnlNum;i++)
	{
		if( (secondPacketInfo->asdu[0].dataSet[i].q & 0x1) != 0)
		{
			SETBIT(dwErrWord,SMV_SmpQuality);
		    break;
		}
	}
	#endif
	
	return dwErrWord;
	
}

unsigned long GooseErrAnalyse( GoosePacketInfo *firstPacketInfo, GoosePacketInfo *secondPacketInfo, int nDiffTime, int chNo)
{
	unsigned long dwErrWord = 0;
	int i;
	GooseCfgInfo *gooseCfg;
	
	PLH_DEBUG("plh_getGooseCfgInfoPtr, appId:%d, nPriority:%d, nVlanID:%d, mac:%06x, szGocbRef:%s, dsRef:%s, goId:%s\n",
			secondPacketInfo->wAppID, secondPacketInfo->nPriority, secondPacketInfo->nVlanID, secondPacketInfo->szDstMac, 
			secondPacketInfo->szGocbRef, secondPacketInfo->szDataSetRef, secondPacketInfo->szGoID);

	/*�ȱȽ�SV�ϴ�����*/

	/*����Ч�ʽϵ�,��ʼ�����ý��������������¼�������õ�����,��������ıȽϴ���
	�����ٸ�*/
	for (i = 0; i < nIED; i++)
	{
		gooseCfg = plh_getGooseCfgInfoPtr(chNo, i);
		PLH_DEBUG("plh_getGooseCfgInfoPtr, appId:%d, nPriority:%d, nVlanID:%d, mac:%06x, szGocbRef:%s, dsRef:%s, goId:%s\n",
				gooseCfg->wAppID, gooseCfg->nPriority, gooseCfg->nVlanID, gooseCfg->szDstMac, 
				gooseCfg->szGocbRef, gooseCfg->szDataSetRef, gooseCfg->szGoID);
		if (memcmp((unsigned char *)&secondPacketInfo->nPriority, (unsigned char *)gooseCfg, sizeof(GooseCfgInfo)) == 0)
		{
			/*�ҵ�ƥ����,���������澯,��Ϊ����,���﷢�ͼٸ澯,��̨�յ��澯˵����ƥ����
			ͬʱ��֤�˸澯��Ϣ���շ�����,����ǰ����ȽϹ��ܺ��ⲿ��ɾ��*/
			bs_gooseWarningMsgsend();
			break;
		}
	}

	/*���û�и���������ƥ���,���˳�*/
	if (i == nIED)
	{
		PLH_DEBUG("GooseErrAnalyse no match \n");
		return -1;
	}	

	/*��һ����������Ϣ���жϱȽ�*/
	/*
	if(secondpacketInfo->nPriority != atoi(pGooseCfgInfo->strPriority.c_str()))
		SETBIT(dwErrWord,GOOSE_Priority);

	if(secondpacketInfo->nVlanID != atoi(pGooseCfgInfo->strVLanId.c_str()))
		SETBIT(dwErrWord,GOOSE_VlanID);

	if(secondpacketInfo->wAppID != atoi(pGooseCfgInfo->strAPPId.c_str()))
		SETBIT(dwErrWord,GOOSE_APPID);

	if(!memcmp(secondpacketInfo->szGocbRef,pGooseCfgInfo->strCbRef.c_str(),pGooseCfgInfo->strCbRef.length()))
		SETBIT(dwErrWord,GOOSE_GocbRef);

	if(!memcmp(secondpacketInfo->szDataSetRef,pGooseCfgInfo->strdatasetRef.c_str(),pGooseCfgInfo->strdatasetRef.length()))
		SETBIT(dwErrWord,GOOSE_DatSetRef);

	if(!memcmp(secondpacketInfo->szGoID,pGooseCfgInfo->strgoID.c_str(),pGooseCfgInfo->strgoID.length()))
		SETBIT(dwErrWord,GOOSE_GoID);


	byte * pszDstMac = 	m_MacAddrConvert.Str2ByteArray(pGooseCfgInfo->strDestMac.c_str());
	if(!memcmp(secondpacketInfo->szDstMac,pszDstMac,6))
		SETBIT(dwErrWord,GOOSE_DstMAC);

        //Ŀǰ�����ýӿ���û���ҵ�������Ҫ���Ӹñ����ӿڣ�2012-05-26
	//if( secondpacketInfo->nDataEntriesNum != gooseCfgInfo.)
	//	SETBIT(wErrWord,GOOSE_DataEntriesNum);
	*/

	//�ڶ�����ҵ����Ϣ���ж�
	////////////////////////////////////////////////////////
	// ���Ķ�ʧ GOOSE_PacketLost
	////////////////////////////////////////////////////////

	//����1��StNum���䣬sqNum����
	if(  (secondPacketInfo->stNum ==  firstPacketInfo->stNum) &&  (secondPacketInfo->sqNum > (firstPacketInfo->sqNum +1))) 
	{
		SETBIT(dwErrWord,GOOSE_PacketLost);
	}
	//����2��stNum����
	if(  secondPacketInfo->stNum >  (firstPacketInfo->stNum + 1)) 

	{
		SETBIT(dwErrWord,GOOSE_PacketLost);
	}


	////////////////////////////////////////////////////////
	// �����ظ�
	////////////////////////////////////////////////////////

	//��second == first��
	if( (secondPacketInfo->sqNum == firstPacketInfo->sqNum ) &&
		 (firstPacketInfo->sqNum != 0)

		)
	{
		SETBIT(dwErrWord,GOOSE_PacketRepeated);
	}

	////////////////////////////////////////////////////////
	// ����˳����ת
	////////////////////////////////////////////////////////

	//(second < first && second !=0)
	if( (secondPacketInfo->sqNum < firstPacketInfo->sqNum ) 
		&& (secondPacketInfo->sqNum != 0) 
		)
	{
		SETBIT(dwErrWord,SMV_SmpReverseOrder);
	}


	////////////////////////////////////////////////////////
	// ����״̬��Ծ GOOSE_PacketStJump 
	////////////////////////////////////////////////////////
	if( secondPacketInfo->stNum > ( firstPacketInfo->stNum + 1 ) )
	{
		SETBIT(dwErrWord,GOOSE_PacketStJump);
	}

	////////////////////////////////////////////////////////
	// ����״̬��ת GOOSE_PacketStReverseOrder
	////////////////////////////////////////////////////////
	if( (secondPacketInfo->stNum < firstPacketInfo->stNum ) 
		&& (secondPacketInfo->stNum != 0)  )
	{
		SETBIT(dwErrWord,GOOSE_PacketStReverseOrder);
	}
    
	////////////////////////////////////////////////////////
	// ��Ҫ�������� GOOSE_NdsCom
	////////////////////////////////////////////////////////
	if( secondPacketInfo->bNdsCom ) 
	{
		SETBIT(dwErrWord,GOOSE_NdsCom);
	}
	////////////////////////////////////////////////////////
	// ����״̬��λ GOOSE_StChange
	////////////////////////////////////////////////////////
	if( secondPacketInfo->stNum == (firstPacketInfo->stNum + 1) ) 
	{
		SETBIT(dwErrWord,GOOSE_StChange);
	}

	////////////////////////////////////////////////////////
	// IED���� GOOSE_IEDReset
	////////////////////////////////////////////////////////
	if( (secondPacketInfo->stNum == 1) && (secondPacketInfo->sqNum ==1) ) 
	{
		SETBIT(dwErrWord,GOOSE_IEDReset);
	}

	////////////////////////////////////////////////////////
	// GOOSE ���� GOOSE_CommuBreak
	////////////////////////////////////////////////////////
	/*
	if( nDiffTime > 2 * (atoi(m_pGooseCfgInfo->strMaxTime.c_str()) + atoi(m_pGooseCfgInfo->strMinTime.c_str()))  ) 
	{
		SETBIT(dwErrWord,GOOSE_CommuBreak);
	}
	*/

	////////////////////////////////////////////////////////
	// �����ź� GOOSE_Test
	////////////////////////////////////////////////////////
	if( secondPacketInfo->bTest ) 
	{
		SETBIT(dwErrWord,GOOSE_Test);
	}

	return dwErrWord;

}


int plh_getMsgQueueStatus(int chNo)
{
	struct msqid_ds info;

	if (msgctl(ProcessQid[chNo], IPC_STAT, &info) < 0)
	{
		PLH_DEBUG("plh_getMsgQueueStatus msgctl failed, errno:%d, %s, ---lineNo:%d\n", errno, strerror(errno), __LINE__);
	}

	PLH_DEBUG("msgInfo :msg No:%d, max No:%d cur No:%d\n", info.msg_qnum, info.msg_qbytes, info.msg_cbytes);
	if (info.msg_cbytes == 0)
	{
		return 1;
	}

	return 0;
}

/*��������ʽ������Ϣ,�������������̵߳���,������ʱ����ʧ�ܷ���-2*/
int plh_msgSend(MsgProcessLayer *msg)
{
	struct msqid_ds info;
	int i;
	unsigned char no;
	static unsigned long cnt = 0;

	cnt++;
	
	if (msg == NULL)
	{
		return -1;	
	}

	no = msg->chNo - 1;
	
	if ((cnt % 400) == 0)
	{
		PLH_DEBUG("plh send msg,id:%d, type:%d, rqst:%d\n", ProcessQid[no], msg->mtype, msg->request);
	}
	
	
	#if 0
	/*����ͨ����ɹر�,����Ҫÿ�ζ���״̬*/
	if (msgctl(ProcessQid[no], IPC_STAT, &info) < 0)
	{
		PLH_DEBUG("plh_getMsgQueueStatus msgctl failed, errno:%d, %s, ---lineNo:%d\n", errno, strerror(errno), __LINE__);
	}
	#endif
	
	/*δ֪ԭ����ʧ���˾��ط�,��������,��ʧ���˾ͰѸ澯�����ص�,����ͬ������
	�������Ա���������ķ����澯,�ȶ��п����ٳ��Է���*/
	for (i = 0; i < 3; i++)
	{
		if (msgsnd(ProcessQid[no], msg, (sizeof(MsgProcessLayer)-sizeof(long)), IPC_NOWAIT) < 0)
		{
			/*����ʧ��,���п�������*/
			if (errno == EAGAIN)
			{
				PLH_DEBUG("msgsnd failed, errno:%d, str:%s, ---lineNo:%d\n", errno, strerror(errno), __LINE__);
				PLH_DEBUG("msgInfo :msg No:%d, max No:%d cur No:%d\n", info.msg_qnum, info.msg_qbytes, info.msg_cbytes);
				return PROCESS_MSGQUEUE_FULL;
			}
			else			/*���Ƕ�����,��������δ֪ԭ���·���ʧ��,�ط�����*/
			{
				PLH_DEBUG("ProcessQid msgsnd failed,time:%d, errno:%d, str:%s, ---lineNo:%d\n", i, errno, strerror(errno), __LINE__);
				continue;
			}
		}
		else
		{
			return 0;
		}
	}

	PLH_DEBUG("Try 3 times, ProcessQid also msgsnd failed, stop parse, errno:%d, str:%s, ---lineNo:%d\n", errno, strerror(errno), __LINE__);
	return PROCESS_MSGQUEUE_FULL;
	
}

/*�ṩ��telnet�����ýӿ�,��ѯ��Ϣ���е�״̬*/
int plh_getMsgQueueInfo(struct msqid_ds *info, int chNo)
{
	int no;
	no = chNo - 1;
	msgctl(ProcessQid[no], IPC_STAT, info);
	return 0;
}


static void *processLayerHandlerThread(void *arg)
{
	key_t key;
	MsgProcessLayer msg;
	
	GooseCfgInfo*goose;
	GoosePacketInfo goosePacket;
	
	SmvCfgInfo *smv;
	SmvPacketInfo smvPacket;
	
	int i, fileFormate;
	int no;

	key = ((PlhArgs *)arg)->key;
	no = ((PlhArgs *)arg)->no;

	if ((ProcessQid[no] = msgget(key, 0666 | IPC_CREAT)) == -1)
	{
		PLH_DEBUG("msgget failed, line:%d\n", __LINE__);
		exit(-1);
	}
	PLH_DEBUG("process handler no:%d,key:%d,ProcessQid:%d\n",no, key, ProcessQid[no]);

	fileFormate = xml_getConfigDevFileF();
	
	while(1)
	{
		static int cnt = 0;
		if (msgrcv(ProcessQid[no], &msg, (sizeof(MsgProcessLayer)-sizeof(long)), TYPE_PROCESSLAYER, 0) == -1)
		{
			PLH_DEBUG("msgrcv failed , line:%d\n", __LINE__);
			continue;
		}
		cnt++;

		if ((cnt % 200) == 0)
		{
			PLH_DEBUG("rcv msgs process,request:%d\n",msg.request);
		}
		
		switch(msg.request)
		{
			case MSG_RQST_PROCESS:
			{
				#if 0
				//PLH_DEBUG("PLH_HANDLER MSG_RQST_PROCESS receive\n");
				for (i = 0; i < msg.count; i++)
				{
					if (msg.type[i] == 0)		/*sv*/
					{
						/*>0 �����ɹ�*/
						if(SmvDecode(msg.packets[i], fileFormate, &smvPacket) > 0)
						{
							/*Ŀǰ֮��������Ƚ�,��ǰ��֡�Ƚ�,difftime���ò���,����0*/
							/*ǰ����ȽϵĻ�����Ӧ���Ƚ�����,�����������ȡ��һ�͵ڶ�Ԫ�ؼ�
							ǰ������и澯����*/
							unsigned long errWord;
							errWord = SmvErrAnalyse(NULL, &smvPacket, 0, no);
							if (errWord > 0)
							{
								/*˵���и澯,�������Ϣ*/
							}
							else if (errWord == -1)
							{
								/*������ȫ����ƥ��,����в����ͼ�����*/
							}
						}
					}
					else if (msg.type[i] == 1)		/*goose*/
					{
						/*>0 �����ɹ�*/
						if(GooseDecode(msg.packets[i], fileFormate, &goosePacket) > 0)
						{
							/*Ŀǰ֮��������Ƚ�,��ǰ��֡�Ƚ�,difftime���ò���,����0*/
							unsigned long errWord;
							errWord = GooseErrAnalyse(NULL, &goosePacket, 0, msg.chNo);
							if (errWord != 0)
							{
								/*˵���и澯,�������Ϣ*/
							}
						}
					}
					else
					{
						PLH_DEBUG("Process handler unknown packet style ---lineNo:%d\n", __LINE__);					
					}
				
				}
				#endif
			}
				break;

			case MSG_RQST_RESET_WARNING:
			{

			}
				break;
				
			default:
				break;
		}
	}		

}

/*��ʼ������,������Ϣ����,��ʼ������*/
int plh_init(pthread_t *id1, pthread_t *id2)
{
	int ret = 0;
	PlhArgs arg[2];

	arg[0].no = 0;
	arg[0].key = 400;
	/*˫ͨ���Ժ����������,һ��ͨ��һ������*/
	if((ret = pthread_create(id1, NULL, processLayerHandlerThread, (void *)&arg[0])) != 0 ) 
	{
		PLH_DEBUG("\nTY-3012S creats process thread1 failed!\n");
		return ret;
	}

	#if 1
	arg[1].no = 1;
	arg[1].key = 401;
	if((ret = pthread_create(id2, NULL, processLayerHandlerThread, (void *)&arg[1])) != 0 ) 
	{
		PLH_DEBUG("\nTY-3012S creats process thread2 failed!\n");
		return ret;
	}
	#endif 
	return ret;


}

#define FILENAME	"./Goose.pcap"
#define PK_LENGTH	0x400

#if 0
int main()
{
	int pkFd;
	char packetBuffer[PK_LENGTH];
	int ret, len = 0;
	GoosePacketInfo gooseInfo;
		
	if ((pkFd = open(FILENAME, O_RDONLY)) == -1)
	{
		printf("open failed\n");
		return -1;
	}

	lseek(pkFd, 0x9cc, SEEK_SET);
	
	if ((len = read(pkFd, packetBuffer, PK_LENGTH)) < 0)
	{
		printf("read error\n");
		return -1;
	}

	memset(&gooseInfo, 0, sizeof(gooseInfo));
	
	ret = GooseDecode(packetBuffer, 0, &gooseInfo);

	printf("GooseDecode end:\n");
	printf("gooseInfo->nPriority:%d\n", gooseInfo.nPriority);
	printf("gooseInfo->nVlanID:%d\n", gooseInfo.nVlanID);
	printf("gooseInfo->wAppID:%d\n", gooseInfo.wAppID);
	printf("gooseInfo->szDstMac:%s\n", gooseInfo.szDstMac);
	printf("gooseInfo->szGocbRef:%s\n", gooseInfo.szGocbRef);
	printf("gooseInfo->szDataSetRef:%s\n", gooseInfo.szDataSetRef);
	printf("gooseInfo->szGoID:%s\n", gooseInfo.szGoID);
	printf("gooseInfo->stNum:%d\n", gooseInfo.stNum);
	printf("gooseInfo->sqNum:%d\n", gooseInfo.sqNum);
	printf("gooseInfo->bTest:%d\n", gooseInfo.bTest);
	printf("gooseInfo->bNdsCom:%d\n", gooseInfo.bNdsCom);
	printf("gooseInfo->nDataEntriesNum:%d\n", gooseInfo.nDataEntriesNum);
	
	
	
	return 0;
};
#endif
