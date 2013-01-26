#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "xmlHandler.h"
#include "common.h"
#include "ProcessLayerHandler.h"

DeviceStr*		pDev;	//���������ļ�ָ�룬����ָ��
DevicePar		DevPar;	//���������ļ�����


#define DEBUG_PRINT		

curFileInfo	curFile[nCHANNEL];
diskPtr 	xmlDiskPtr[nCHANNEL];
pRecord *pRec[nCHANNEL];
//pthread_mutex_t xml_mutex = PTHREAD_MUTEX_INITIALIZER;			/*������*/


static int mac_display(int *ptr)
{
	int i;

	for (i = 0; i < 6; i++) {
		printf("%02x", ptr[i]);
		if(i < 5) printf(":");
	}
	printf("!\n");
	return 0;
}


int xml_printDevicePar(int command)
{
	int nChannel, nVlan, nMac, nIed;

	if(command == 0) return -1;
	//1��װ�õ��б��֣����ơ�Ip��ַ����ţ�3����
	printf("TY-3012S xmlDoc DevName: %s!\n", DevPar.DevName);	//��ʾ/SCL/DeviceList/Device/name(attribute)
	printf("TY-3012S xmlDoc DevIp:   %s!\n", DevPar.DevIp);		//��ʾ/SCL/DeviceList/Device/ip(attribute)
	printf("TY-3012S xmlDoc DevNo:   %x!\n", DevPar.DevNo);		//��ʾ/SCL/DeviceList/Device/no(attribute)
	//2��װ�ð汾����¼����װ�ð汾�ţ�xml�ļ��İ汾��!
	printf("TY-3012S xmlDoc DVersion:%s!\n", DevPar.DevVer);	//��ʾ/SCL/DeviceList/Device/Common/version
	//3��װ�õĵ�ַ���֣�Ip��ַ���������롢���ء����ܷ�����Ip��ַ�����ܷ������˿ڣ�5����
	printf("TY-3012S xmlDoc DLocalIp:%s!\n", DevPar.DevLoIp);	//��ʾ/SCL/DeviceList/Device/Address/LocIp
	printf("TY-3012S xmlDoc DSubnetM:%s!\n", DevPar.DevSnMask);	//��ʾ/SCL/DeviceList/Device/Address/SubnetMask
	printf("TY-3012S xmlDoc DGateway:%s!\n", DevPar.DevGway);	//��ʾ/SCL/DeviceList/Device/Address/Gateway
	printf("TY-3012S xmlDoc DRemIp:  %s!\n", DevPar.DevReIp);	//��ʾ/SCL/DeviceList/Device/Address/RemIp
	printf("TY-3012S xmlDoc DRemPort:%x!\n", DevPar.DevRePort);	//��ʾ/SCL/DeviceList/Device/Address/RemPort
	//4��װ�õĲ������֣���š����ơ�ͨ�����������Ĵ洢��ʽ���洢���ڡ������������μ�顢ʱ����GPS�ӿڣ�10����
	printf("TY-3012S xmlDoc DSubNo:  %x!\n", DevPar.DevSubN);	//��ʾ/SCL/DeviceList/Device/Parameter/DeviceNo
	printf("TY-3012S xmlDoc DSubName:%x!\n", DevPar.DevSname);	//��ʾ/SCL/DeviceList/Device/Parameter/Name
	printf("TY-3012S xmlDoc DChNum:  %x!\n", DevPar.DevChNum);	//��ʾ/SCL/DeviceList/Device/Parameter/ChnlNum
	printf("TY-3012S xmlDoc DFFormat:%x!\n", DevPar.DevFileF);	//��ʾ/SCL/DeviceList/Device/Parameter/FileFormat
	printf("TY-3012S xmlDoc DFPeriod:%x!\n", DevPar.DevFileP);	//��ʾ/SCL/DeviceList/Device/Parameter/FileSavedPeriod
	printf("TY-3012S xmlDoc DFMaxSiz:%x!\n", DevPar.DevFileS);	//��ʾ/SCL/DeviceList/Device/Parameter/FileMaxSize
	printf("TY-3012S xmlDoc DHdCheck:%x!\n", DevPar.DevHdisk);	//��ʾ/SCL/DeviceList/Device/Parameter/HarddiskCheckedPercentage
	printf("TY-3012S xmlDoc DTimeZon:%x!\n", DevPar.DevTimeZ);	//��ʾ/SCL/DeviceList/Device/Parameter/TimeZone
	printf("TY-3012S xmlDoc DHeartBP:%x!\n", DevPar.DevBeatP);	//��ʾ/SCL/DeviceList/Device/Parameter/HeartBeatPeriod
	printf("TY-3012S xmlDoc DGpsPort:%x!\n", DevPar.DevGpsIf);	//��ʾ/SCL/DeviceList/Device/Parameter/GpsInterface
	//5��װ�õ�ͨ�����֣�ͨ���š����ӿ����͡�����ʱ�䡢ץ�����ͣ�2��ͨ��8����
	for (nChannel = 0; nChannel < nCHANNEL; nChannel++) {
		//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/no(attribute)
		printf("TY-3012S xmlDoc ChNo  [%x, %x]:%x!\n", nChannel, 0000,  DevPar.ChNo[nChannel]);
		//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/CapturePacketType
		printf("TY-3012S xmlDoc ChCapPacket[%x, %x]:%x!\n", nChannel, 0000,  DevPar.ChPacket[nChannel]);
		//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/InterfaceType
		printf("TY-3012S xmlDoc ChInterface[%x, %x]:%x!\n", nChannel, 0000,  DevPar.ChIfType[nChannel]);
		//6��װ��ͨ��n���˲���
		//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/Filter/MaxFrameLen
		printf("TY-3012S xmlDoc ChMaxFraLen[%x, %x]:%x!\n", nChannel, 0000,  DevPar.ChMaxLen[nChannel]);
		//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/Filter/MinFrameLen
		printf("TY-3012S xmlDoc ChMinFraLen[%x, %x]:%x!\n", nChannel, 0000,  DevPar.ChMinLen[nChannel]);
		for (nVlan = 0; nVlan < nVLAN; nVlan++) {
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/Filter/VlanId/V/no(attribute)
			printf("TY-3012S xmlDoc ChannelVlNo[%x, %x]:%x!\n", nChannel, nVlan, DevPar.ChVlanNo[nChannel][nVlan]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/Filter/VlanId/V/value(attribute)
			printf("TY-3012S xmlDoc ChannelVlId[%x, %x]:%x!\n", nChannel, nVlan, DevPar.ChVlanId[nChannel][nVlan]);
		}
		for (nMac = 0; nMac < nMAC; nMac++) {
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/Filter/MacList/Mac/no(attribute)
			printf("TY-3012S xmlDoc ChannelMacN[%x, %x]:%x!\n", nChannel, nMac,  DevPar.ChMacNo[nChannel][nMac]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/Filter/MacList/Mac/value(attribute)
			printf("TY-3012S xmlDoc ChannelMacA[%x, %x]:", nChannel, nMac);
			mac_display(&DevPar.ChMacAdd[nChannel][nMac][0]);
		}
		//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/Filter/PacketType
		printf("TY-3012S xmlDoc ChPacketTyp[%x, %x]:%x!\n", nChannel, 0000,  DevPar.ChMacPkt[nChannel]);
		//7��װ��ͨ��n�ı���IED���֣�1��ͨ����¼����16��IED
		for (nIed = 0; nIed < nIED; nIed++) {
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/name(attribute)
			printf("TY-3012S xmlDoc ChIedName[%x, %x]:%s!\n", nChannel, nIed, DevPar.ChIedName[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/mac(attribute)
			printf("TY-3012S xmlDoc ChIedSMac[%x, %x]:%s!\n", nChannel, nIed, DevPar.ChIedMac[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/Type
			printf("TY-3012S xmlDoc ChIedType[%x, %x]:%s!\n", nChannel, nIed, DevPar.ChIedType[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/Bay
			printf("TY-3012S xmlDoc ChIedBayD[%x, %x]:%s!\n", nChannel, nIed, DevPar.ChIedBay[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/Manufacturer
			printf("TY-3012S xmlDoc ChIedManu[%x, %x]:%s!\n", nChannel, nIed, DevPar.ChIedMan[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/Desc
			printf("TY-3012S xmlDoc ChIedDesc[%x, %x]:%s!\n", nChannel, nIed, DevPar.ChIedDesc[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/BayIp
			printf("TY-3012S xmlDoc ChIedBIp1[%x, %x]:%s!\n", nChannel, nIed, DevPar.ChIedBip1[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/BayIp2
			printf("TY-3012S xmlDoc ChIedBIp2[%x, %x]:%s!\n", nChannel, nIed, DevPar.ChIedBip2[nChannel][nIed]);
			//8��װ��ͨ��n�ı���IED��SMV����
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/Type
			printf("TY-3012S xmlDoc ChMuType[%x, %x]:%x!\n", nChannel, nIed, DevPar.ChMuType[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/DestMac
			printf("TY-3012S xmlDoc ChIedMuDMac[%x, %x]:%s!\n", nChannel, nIed, &DevPar.ChMuDmac[nChannel][nIed][0]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/SourceMac
			printf("TY-3012S xmlDoc ChIedMuSMac[%x, %x]:%s!\n", nChannel, nIed, &DevPar.ChMuSmac[nChannel][nIed][0]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/SmpRatePerFreq
			printf("TY-3012S xmlDoc ChIedMuFreq[%x, %x]:%x!\n", nChannel, nIed, DevPar.ChMuSmpR[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/SmpTimeDeviation
			printf("TY-3012S xmlDoc ChIedMuSmpT[%x, %x]:%x!\n", nChannel, nIed, DevPar.ChMuSmpD[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/VlanId
			printf("TY-3012S xmlDoc ChIedMuVlId[%x, %x]:%x!\n", nChannel, nIed, DevPar.ChMuVlId[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/Priority
			printf("TY-3012S xmlDoc ChIedMuPrio[%x, %x]:%x!\n", nChannel, nIed, DevPar.ChMuPrio[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/AppId
			printf("TY-3012S xmlDoc ChIedMuApId[%x, %x]:%x!\n", nChannel, nIed, DevPar.ChMuApId[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/Smv92/SmvId
			printf("TY-3012S xmlDoc ChIedMuSvId[%x, %x]:%x!\n", nChannel, nIed, DevPar.ChMuSvId[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/Smv92/DatasetRef
			printf("TY-3012S xmlDoc ChIedMuDRef[%x, %x]:%x!\n", nChannel, nIed, DevPar.ChMuDref[nChannel][nIed]);
			//��ʾ/SCL/DeviceList/Device/ChannelList/Channel/IEDList/IED/SMV/Smv92/Channel desc
			printf("TY-3012S xmlDoc ChIedMuDesc[%x, %x]:%s!\n", nChannel, nIed, &DevPar.ChMuDesc[nChannel][nIed][0]);
		}
	}
	return 0;
}

/*�����ļ��е�mac��ַΪ00-01-02-03��ʽ���ַ���,�����ڴ���ת����6�ֽ���
�Է���Э��������Ƚ�*/
static int str2Char(unsigned char *str, unsigned char *cfg)
{
	int value, result = 0;
	int i, j = 0;
	unsigned char ar[17] = "0123456789abcdef";
	unsigned char *p;

	/*�ȼ��һ���Сд,ͳһת����Сд*/
	for (i = 0; i < strlen(str); i++)
	{
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
		{
			str[i] = str[i] - 'A' + 'a';
		}
	}
	
	for(i = 0; i < strlen(str); i++)
	{
		if ((p = strchr(ar, str[i])) != NULL)
		{
	  		value = p - ar;
			result = result * 16 + value;
			if (i == (strlen(str) - 1))
			{
				cfg[j] = result;
				result = 0;
			}
		}
		else
		{
			cfg[j] = result;
			j++;
			result = 0;
		}
		
	}
	printf("convert mac addr:%x,%x,%x,%x,%x,%x\n", cfg[0],cfg[1],cfg[2],cfg[3],cfg[4],cfg[5]);
	return 0;
}



//add by haopeng for Goose param
static int GooseStrToPar(void)
{
	int nLen, nChannel, nIed;
	GooseCfgInfo *gooseCfg;
	//9��װ��ͨ��n�ı���IED��GOOSE����
	for(nChannel = 0; nChannel < nCHANNEL; nChannel++) 
	{
		for(nIed = 0; nIed < nIED; nIed++) 
		{
			gooseCfg = (GooseCfgInfo *)plh_getGooseCfgInfoPtr(nChannel, nIed);
			
			if(pDev->ChGSAref[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChGSAref[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChGSAref[nChannel][nIed][0], pDev->ChGSAref[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChGSAref[nChannel][nIed][0], pDev->ChGSAref[nChannel][nIed], (NAMELEN - 1));
				}
				DEBUG_PRINT("memcpy to DevPar.ChGSAref[%d,%d]:%s\n", nChannel, nIed, DevPar.ChGSAref[nChannel][nIed]);
			}

			if(pDev->ChGSADref[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChGSADref[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChGSADref[nChannel][nIed][0], pDev->ChGSADref[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChGSADref[nChannel][nIed][0], pDev->ChGSADref[nChannel][nIed], (NAMELEN - 1));
				}
				DEBUG_PRINT("memcpy to DevPar.ChGSADref[%d,%d]:%s\n", nChannel, nIed, DevPar.ChGSADref[nChannel][nIed]);
			}

			if(pDev->ChGSAGoId[nChannel][nIed] != NULL)
			{
				DevPar.ChGSAGoId[nChannel][nIed] = strtoul(pDev->ChGSAGoId[nChannel][nIed], NULL, 0); 			
				DEBUG_PRINT("memcpy to DevPar.ChGSAGoId[%d,%d]:%d\n", nChannel, nIed, DevPar.ChGSAGoId[nChannel][nIed]);
			}

			if(pDev->ChGSGoId[nChannel][nIed] != NULL)
			{
				DevPar.ChGSGoId[nChannel][nIed] = strtoul(pDev->ChGSGoId[nChannel][nIed], NULL, 0); 	
				memcpy(gooseCfg->szGoID, pDev->ChGSGoId[nChannel][nIed], strlen(pDev->ChGSGoId[nChannel][nIed]));
				gooseCfg->szGoID[strlen(pDev->ChGSGoId[nChannel][nIed])] = '\0'; 
				DEBUG_PRINT("memcpy to DevPar.ChGSGoId[%d,%d]:%d\n", nChannel, nIed, DevPar.ChGSGoId[nChannel][nIed]);
			}

			if(pDev->ChGSDmac[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChGSDmac[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChGSDmac[nChannel][nIed][0], pDev->ChGSDmac[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChGSDmac[nChannel][nIed][0], pDev->ChGSDmac[nChannel][nIed], (NAMELEN - 1));
				}
				str2Char(pDev->ChGSDmac[nChannel][nIed], gooseCfg->szDstMac);
				DEBUG_PRINT("memcpy to DevPar.ChGSDmac[%d,%d]:%s\n", nChannel, nIed, DevPar.ChGSDmac[nChannel][nIed]);
			}

			if(pDev->ChGSSmac[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChGSSmac[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChGSSmac[nChannel][nIed][0], pDev->ChGSSmac[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChGSSmac[nChannel][nIed][0], pDev->ChGSSmac[nChannel][nIed], (NAMELEN - 1));
				}
				DEBUG_PRINT("memcpy to DevPar.ChGSSmac[%d,%d]:%s\n", nChannel, nIed, DevPar.ChGSSmac[nChannel][nIed]);
			}

			if(pDev->ChGSSmac2[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChGSSmac2[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChGSSmac2[nChannel][nIed][0], pDev->ChGSSmac2[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChGSSmac2[nChannel][nIed][0], pDev->ChGSSmac2[nChannel][nIed], (NAMELEN - 1));
				}
				DEBUG_PRINT("memcpy to DevPar.ChGSSmac2[%d,%d]:%s\n", nChannel, nIed, DevPar.ChGSSmac2[nChannel][nIed]);
			}
			
			if(pDev->ChGSVlId[nChannel][nIed] != NULL)
			{
				DevPar.ChGSVlId[nChannel][nIed] = strtoul(pDev->ChGSVlId[nChannel][nIed], NULL, 0); 	
				gooseCfg->nVlanID = DevPar.ChGSVlId[nChannel][nIed];
				DEBUG_PRINT("memcpy to DevPar.ChGSVlId[%d,%d]:%d\n", nChannel, nIed, DevPar.ChGSVlId[nChannel][nIed]);
			}

			if(pDev->ChGSPrio[nChannel][nIed] != NULL)
			{
				DevPar.ChGSPrio[nChannel][nIed] = strtoul(pDev->ChGSPrio[nChannel][nIed], NULL, 0); 	
				gooseCfg->nPriority = DevPar.ChGSPrio[nChannel][nIed];
				DEBUG_PRINT("memcpy to DevPar.ChGSPrio[%d,%d]:%d\n", nChannel, nIed, DevPar.ChGSPrio[nChannel][nIed]);
			}

			if(pDev->ChGSApId[nChannel][nIed] != NULL)
			{
				DevPar.ChGSApId[nChannel][nIed] = strtoul(pDev->ChGSApId[nChannel][nIed], NULL, 0); 
				/*�˶�һ���ǲ���ȡ����*/
				gooseCfg->wAppID = (unsigned short)DevPar.ChGSApId[nChannel][nIed];
				printf("gooseCfg->wAppID:%d\n", gooseCfg->wAppID);
				DEBUG_PRINT("memcpy to DevPar.ChGSApId[%d,%d]:%d\n", nChannel, nIed, DevPar.ChGSApId[nChannel][nIed]);
			}

			if(pDev->ChGSref[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChGSref[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChGSref[nChannel][nIed][0], pDev->ChGSref[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChGSref[nChannel][nIed][0], pDev->ChGSref[nChannel][nIed], (NAMELEN - 1));
					DevPar.ChGSref[nChannel][nIed][NAMELEN - 1] = '\0';
				}
				
				memcpy(gooseCfg->szGocbRef, &DevPar.ChGSref[nChannel][nIed][0], strlen(&DevPar.ChGSref[nChannel][nIed][0]));
				gooseCfg->szGocbRef[strlen(&DevPar.ChGSref[nChannel][nIed][0])] = '\0';
				
				DEBUG_PRINT("memcpy to DevPar.ChGSref[%d,%d]:%s\n", nChannel, nIed, DevPar.ChGSref[nChannel][nIed]);
			}

			if(pDev->ChGSDref[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChGSDref[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChGSDref[nChannel][nIed][0], pDev->ChGSDref[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChGSDref[nChannel][nIed][0], pDev->ChGSDref[nChannel][nIed], (NAMELEN - 1));
				}
				memcpy(gooseCfg->szDataSetRef, &DevPar.ChGSDref[nChannel][nIed][0], strlen(&DevPar.ChGSDref[nChannel][nIed][0]));
				gooseCfg->szDataSetRef[strlen(&DevPar.ChGSDref[nChannel][nIed][0])] = '\0';
				DEBUG_PRINT("memcpy to DevPar.ChGSDref[%d,%d]:%s\n", nChannel, nIed, DevPar.ChGSDref[nChannel][nIed]);
			}

			if(pDev->ChGSLdInst[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChGSLdInst[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChGSLdInst[nChannel][nIed][0], pDev->ChGSLdInst[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChGSLdInst[nChannel][nIed][0], pDev->ChGSLdInst[nChannel][nIed], (NAMELEN - 1));
				}
				DEBUG_PRINT("memcpy to DevPar.ChGSLdInst[%d,%d]:%s\n", nChannel, nIed, DevPar.ChGSLdInst[nChannel][nIed]);
			}

			if(pDev->ChGSminTm[nChannel][nIed] != NULL)
			{
				DevPar.ChGSminTm[nChannel][nIed] = strtoul(pDev->ChGSminTm[nChannel][nIed], NULL, 0); 			
				DEBUG_PRINT("memcpy to DevPar.ChGSminTm[%d,%d]:%d\n", nChannel, nIed, DevPar.ChGSminTm[nChannel][nIed]);
			}

			if(pDev->ChGSmaxTm[nChannel][nIed] != NULL)
			{
				DevPar.ChGSmaxTm[nChannel][nIed] = strtoul(pDev->ChGSmaxTm[nChannel][nIed], NULL, 0);			
				DEBUG_PRINT("memcpy to DevPar.ChGSmaxTm[%d,%d]:%d\n", nChannel, nIed, DevPar.ChGSmaxTm[nChannel][nIed]);
			}

		}
	}

	return 0;
}



/*�������Ľ�������ڴ���*/
int xml_deviceStrToPar(void)
{
	int nLen, nChannel, nVlan, nIed, nMac, i;
	char strDigit[32];
	SmvCfgInfo	*smvCfg;

	memset(&DevPar, 0, sizeof(DevPar));
	//1��װ�õ��б��֣����ơ�Ip��ַ����ţ�3����
	if(pDev->DevName != NULL) {
		nLen = strlen(pDev->DevName);
		if(nLen < NAMELEN) strcpy(DevPar.DevName, pDev->DevName);
		else memcpy(DevPar.DevName, pDev->DevName, (NAMELEN - 1));
		DEBUG_PRINT("memcpy to DevPar.DevName:%s\n",DevPar.DevName);
	}

	if(pDev->DevIp != NULL) {
		nLen = strlen(pDev->DevIp);
		if(nLen < NAMELEN) strcpy(DevPar.DevIp, pDev->DevIp);
		else memcpy(DevPar.DevIp, pDev->DevIp, (NAMELEN - 1));
		DEBUG_PRINT("memcpy to DevPar.DevIp:%s\n",DevPar.DevIp);
	}
	
	
	DevPar.DevNo = 0;
	if(pDev->DevNo != NULL)
	{
		DevPar.DevNo = strtoul(pDev->DevNo, NULL, 0);	
		DEBUG_PRINT("memcpy to DevPar.DevNo:%d\n",DevPar.DevNo);
	}

	//2��װ�õĹ������֣��汾�ţ�1����
	if(pDev->DevVer != NULL) {
		nLen = strlen(pDev->DevVer);
		if(nLen < NAMELEN) strcpy(DevPar.DevVer, pDev->DevVer);
		else memcpy(DevPar.DevVer, pDev->DevVer, NAMELEN);
		DEBUG_PRINT("memcpy to DevPar.DevVer:%s\n",DevPar.DevVer);
	}	

	//3��װ�õĵ�ַ���֣�Ip��ַ���������롢���ء����ܷ�����Ip��ַ�����ܷ������˿ڣ�5����
	if(pDev->DevLoIp != NULL) {
		nLen = strlen(pDev->DevLoIp);
		if(nLen < NAMELEN) strcpy(DevPar.DevLoIp, pDev->DevLoIp);
		else memcpy(DevPar.DevLoIp, pDev->DevLoIp, (NAMELEN - 1));
		DEBUG_PRINT("memcpy to DevPar.DevLoIp:%s\n",DevPar.DevLoIp);
	}	

	if(pDev->DevSnMask != NULL) {
		nLen = strlen(pDev->DevSnMask);
		if(nLen < NAMELEN) strcpy(DevPar.DevSnMask, pDev->DevSnMask);
		else memcpy(DevPar.DevSnMask, pDev->DevSnMask, (NAMELEN - 1));
		DEBUG_PRINT("memcpy to DevPar.DevSnMask:%s\n",DevPar.DevSnMask);
	}
	
	if(pDev->DevGway != NULL) {
		nLen = strlen(pDev->DevGway);
		if(nLen < NAMELEN) strcpy(DevPar.DevGway, pDev->DevGway);
		else memcpy(DevPar.DevGway, pDev->DevGway, (NAMELEN - 1));
		DEBUG_PRINT("memcpy to DevPar.DevGway:%s\n",DevPar.DevGway);
	}
	
	if(pDev->DevReIp != NULL) {
		nLen = strlen(pDev->DevReIp);
		if(nLen < NAMELEN) strcpy(DevPar.DevReIp, pDev->DevReIp);
		else memcpy(DevPar.DevReIp, pDev->DevReIp, (NAMELEN - 1));
		DEBUG_PRINT("memcpy to DevPar.DevReIp:%s\n",DevPar.DevReIp);
	}

	if(pDev->DevRePort != NULL) 
	{
		DevPar.DevRePort = strtoul(pDev->DevRePort, NULL, 0);
		DEBUG_PRINT("memcpy to DevPar.DevRePort:%d\n",DevPar.DevRePort);
	}
	
	//4��װ�õĲ������֣���š����ơ�ͨ�����������Ĵ洢��ʽ���洢���ڡ������������μ�顢ʱ����GPS�ӿڣ�10����
	if(pDev->DevSubN != NULL)
	{
		DevPar.DevSubN = strtoul(pDev->DevSubN, NULL, 0);
		DEBUG_PRINT("memcpy to DevPar.DevSubN:%d\n",DevPar.DevSubN);
	}
	
	if(pDev->DevSname != NULL) {
		nLen = strlen(pDev->DevSname);
		if(nLen < NAMELEN) 
			strcpy(DevPar.DevSname, pDev->DevSname);
		else 
			memcpy(DevPar.DevSname, pDev->DevSname, (NAMELEN - 1));
		DEBUG_PRINT("memcpy to DevPar.DevSname:%s\n",DevPar.DevSname);
	}
	
	if(pDev->DevChNum != NULL)
	{
		DevPar.DevChNum = strtoul(pDev->DevChNum, NULL, 0);
		DEBUG_PRINT("memcpy to DevPar.DevChNum:%d\n",DevPar.DevChNum);
	}
	
	if(pDev->DevFileF != NULL)
	{
		DevPar.DevFileF = strtoul(pDev->DevFileF, NULL, 0);
		DEBUG_PRINT("memcpy to DevPar.DevFileF:%d\n",DevPar.DevFileF);
	}
	
	if(pDev->DevFileP != NULL)
	{
		DevPar.DevFileP = strtoul(pDev->DevFileP, NULL, 0);
		DEBUG_PRINT("memcpy to DevPar.DevFileP:%d\n",DevPar.DevFileP);
	}
	
	if(pDev->DevFileS != NULL)
	{
		DevPar.DevFileS = strtoul(pDev->DevFileS, NULL, 0);
		DEBUG_PRINT("memcpy to DevPar.DevFileS:%d\n",DevPar.DevFileS);
	}
	
	if(pDev->DevHdisk != NULL)
	{
		DevPar.DevHdisk = strtoul(pDev->DevHdisk, NULL, 0);
		DEBUG_PRINT("memcpy to DevPar.DevHdisk:%d\n",DevPar.DevHdisk);
	}
	
	if(pDev->DevTimeZ != NULL)
		DevPar.DevTimeZ = strtoul(pDev->DevTimeZ, NULL, 0);
	DEBUG_PRINT("memcpy to DevPar.DevTimeZ:%d\n",DevPar.DevTimeZ);
	
	if(pDev->DevBeatP != NULL)
	{
		DevPar.DevBeatP = strtoul(pDev->DevBeatP, NULL, 0);
		DEBUG_PRINT("memcpy to DevPar.DevBeatP:%d\n",DevPar.DevBeatP);
	}
	
	if(pDev->DevGpsIf != NULL)
	{
		DevPar.DevGpsIf = strtoul(pDev->DevGpsIf, NULL, 0);
		DEBUG_PRINT("memcpy to DevPar.DevGpsIf:%d\n",DevPar.DevGpsIf);
	}
	
	//5��װ�õ�ͨ�����֣�ͨ���š����ӿ����͡�����ʱ�䡢ץ�����ͣ�2��ͨ����8����
	for (nChannel = 0; nChannel < nCHANNEL; nChannel++) {
		if(pDev->ChNo[nChannel] != NULL)
		{
			DevPar.ChNo[nChannel] = strtoul(pDev->ChNo[nChannel], NULL, 0);
			DEBUG_PRINT("memcpy to DevPar.ChNo[%d]:%d\n",nChannel, DevPar.ChNo[nChannel]);
		}
		
		if(pDev->ChIfType[nChannel] != NULL)
		{
			DevPar.ChIfType[nChannel] = strtoul(pDev->ChIfType[nChannel], NULL, 0);
			DEBUG_PRINT("memcpy to DevPar.ChIfType[%d]:%d\n",nChannel, DevPar.ChIfType[nChannel]);
		}

		#if 0
//		DevPar.ChTrTime[nChannel] = 0;
		if(pDev->ChTrTime[nChannel] != NULL)
		{
			DevPar.ChTrTime[nChannel] = strtoul(pDev->ChTrTime[nChannel], NULL, 0);
			DEBUG_PRINT("memcpy to DevPar.ChTrTime[%d]:%d\n",nChannel, DevPar.ChTrTime[nChannel]);
		}
		#endif
		
		if(pDev->ChPacket[nChannel] != NULL)
		{
			DevPar.ChPacket[nChannel] = strtoul(pDev->ChPacket[nChannel], NULL, 0);			
			DEBUG_PRINT("memcpy to DevPar.ChPacket[%d]:%d\n",nChannel, DevPar.ChPacket[nChannel]);
		}

		if(pDev->ChAcsLocation[nChannel] != NULL)
		{
			nLen = strlen(pDev->ChAcsLocation[nChannel]);
			if(nLen < NAMELEN) 
				strcpy(DevPar.ChAcsLocation[nChannel], pDev->ChAcsLocation[nChannel]);
			else 
				memcpy(DevPar.ChAcsLocation[nChannel], pDev->ChAcsLocation[nChannel], (NAMELEN - 1));	
			DEBUG_PRINT("memcpy to DevPar.ChAcsLocation[%d]:%s\n",nChannel, DevPar.ChAcsLocation[nChannel]);
		}
		
	}
	//6��װ��ͨ��n���˲��֣�7����
	for(nChannel = 0; nChannel < nCHANNEL; nChannel++) {
		
		if(pDev->ChMinLen[nChannel] != NULL)
		{
			DevPar.ChMinLen[nChannel] = strtoul(pDev->ChMinLen[nChannel], NULL, 0);
			DEBUG_PRINT("memcpy to DevPar.ChMinLen[%d]:%d\n",nChannel, DevPar.ChMinLen[nChannel]);
		}
		
		if(pDev->ChMaxLen[nChannel] != NULL)
		{
			DevPar.ChMaxLen[nChannel] = strtoul(pDev->ChMaxLen[nChannel], NULL, 0);
			DEBUG_PRINT("memcpy to DevPar.ChMaxLen[%d]:%d\n",nChannel, DevPar.ChMaxLen[nChannel]);
		}
		
		for(nVlan = 0; nVlan < nVLAN; nVlan++) {
			if(pDev->ChVlanNo[nChannel][nVlan] != NULL)
			{
				DevPar.ChVlanNo[nChannel][nVlan] = strtoul(pDev->ChVlanNo[nChannel][nVlan], NULL, 0);
				DEBUG_PRINT("memcpy to DevPar.ChVlanNo[%d][%d]:%d\n",nChannel, nVlan, DevPar.ChVlanNo[nChannel][nVlan]);
			}
			
			if(pDev->ChVlanId[nChannel][nVlan] != NULL)
			{
				DevPar.ChVlanId[nChannel][nVlan] = strtoul(pDev->ChVlanId[nChannel][nVlan], NULL, 0);
				DEBUG_PRINT("memcpy to DevPar.ChMaxLen[%d]:%d\n",nChannel, DevPar.ChMaxLen[nChannel]);
			}
			
		}

		#if 0
		for(nMac = 0; nMac < nMAC; nMac++) {
			DevPar.ChMacNo[nChannel][nMac] = 0;
			if(pDev->ChMacNo[nChannel][nMac] != NULL)
				DevPar.ChMacNo[nChannel][nMac] = strtoul(pDev->ChMacNo[nChannel][nMac], NULL, 0);
			
		}

		DevPar.ChMacPkt[nChannel] = 0;
		if(pDev->ChMacPkt[nChannel] != NULL)
			DevPar.ChMacPkt[nChannel] = strtoul(pDev->ChMacPkt[nChannel], NULL, 0);
		#endif
	}
	#if 0
	memset(DevPar.ChMacAdd, 0, sizeof(DevPar.ChMacAdd));
	for(nChannel = 0; nChannel < nCHANNEL; nChannel++) {
		for(nMac = 0; nMac < nMAC; nMac++) {
			if(strlen(pDev->ChMacAdd[nChannel][nMac]) >= 16) {
				for(i = 0; i < 6; i++) {
					memset(strDigit, 0, sizeof(strDigit));
					memcpy(strDigit, pDev->ChMacAdd[nChannel][nMac] + 3*i, 2);
					DevPar.ChMacAdd[nChannel][nMac][i] = strtoul(strDigit, NULL, 0);
				}
			}
		}
	}
	#endif
	//7��װ��ͨ��n�ı���IED���֣�1��ͨ����¼����16��IED
	for(nChannel = 0; nChannel < nCHANNEL; nChannel++) {
		for(nIed = 0; nIed < nIED; nIed++) {
			if(pDev->ChIedName[nChannel][nIed] != NULL) {
				nLen = strlen(pDev->ChIedName[nChannel][nIed]);
				if(nLen < NAMELEN) strcpy(&DevPar.ChIedName[nChannel][nIed][0], pDev->ChIedName[nChannel][nIed]);
				else memcpy(&DevPar.ChIedName[nChannel][nIed][0], pDev->ChIedName[nChannel][nIed], (NAMELEN - 1));
				DEBUG_PRINT("memcpy to DevPar.ChIedName[%d,%d]:%s\n", nChannel, nIed, DevPar.ChIedName[nChannel][nIed]);
			}
			
			if(pDev->ChIedMac[nChannel][nIed] != NULL) {
				nLen = strlen(pDev->ChIedMac[nChannel][nIed]);
				if(nLen < NAMELEN) strcpy(&DevPar.ChIedMac[nChannel][nIed][0], pDev->ChIedMac[nChannel][nIed]);
				else memcpy(&DevPar.ChIedMac[nChannel][nIed][0], pDev->ChIedMac[nChannel][nIed], (NAMELEN - 1));
				DEBUG_PRINT("memcpy to DevPar.ChIedMac[%d,%d]:%s\n", nChannel, nIed, DevPar.ChIedMac[nChannel][nIed]);
			}
			
			if(pDev->ChIedType[nChannel][nIed] != NULL) {
				nLen = strlen(pDev->ChIedType[nChannel][nIed]);
				if(nLen < NAMELEN) strcpy(&DevPar.ChIedType[nChannel][nIed][0], pDev->ChIedType[nChannel][nIed]);
				else memcpy(&DevPar.ChIedType[nChannel][nIed][0], pDev->ChIedType[nChannel][nIed], (NAMELEN - 1));				
				DEBUG_PRINT("memcpy to DevPar.ChIedType[%d,%d]:%s\n", nChannel, nIed, DevPar.ChIedType[nChannel][nIed]);
			}
			
			if(pDev->ChIedBay[nChannel][nIed] != NULL) {
				nLen = strlen(pDev->ChIedBay[nChannel][nIed]);
				if(nLen < NAMELEN) strcpy(&DevPar.ChIedBay[nChannel][nIed][0], pDev->ChIedBay[nChannel][nIed]);
				else memcpy(&DevPar.ChIedBay[nChannel][nIed][0], pDev->ChIedBay[nChannel][nIed], (NAMELEN - 1));
				DEBUG_PRINT("memcpy to DevPar.ChIedBay[%d,%d]:%s\n", nChannel, nIed, DevPar.ChIedBay[nChannel][nIed]);
			}
			
			if(pDev->ChIedMan[nChannel][nIed] != NULL) {
				nLen = strlen(pDev->ChIedMan[nChannel][nIed]);
				if(nLen < NAMELEN) strcpy(&DevPar.ChIedMan[nChannel][nIed][0], pDev->ChIedMan[nChannel][nIed]);
				else memcpy(&DevPar.ChIedMan[nChannel][nIed][0], pDev->ChIedMan[nChannel][nIed], (NAMELEN - 1));
				DEBUG_PRINT("memcpy to DevPar.ChIedMan[%d,%d]:%s\n", nChannel, nIed, DevPar.ChIedMan[nChannel][nIed]);
			}
			
			if(pDev->ChIedDesc[nChannel][nIed] != NULL) {
				nLen = strlen(pDev->ChIedDesc[nChannel][nIed]);
				if(nLen < NAMELEN) strcpy(&DevPar.ChIedDesc[nChannel][nIed][0], pDev->ChIedDesc[nChannel][nIed]);
				else memcpy(&DevPar.ChIedDesc[nChannel][nIed][0], pDev->ChIedDesc[nChannel][nIed], (NAMELEN - 1));
				DEBUG_PRINT("memcpy to DevPar.ChIedDesc[%d,%d]:%s\n", nChannel, nIed, DevPar.ChIedDesc[nChannel][nIed]);
			}
			
			if(pDev->ChIedBip1[nChannel][nIed] != NULL) {
				nLen = strlen(pDev->ChIedBip1[nChannel][nIed]);
				if(nLen < NAMELEN) strcpy(&DevPar.ChIedBip1[nChannel][nIed][0], pDev->ChIedBip1[nChannel][nIed]);
				else memcpy(&DevPar.ChIedBip1[nChannel][nIed][0], pDev->ChIedBip1[nChannel][nIed], (NAMELEN - 1));
				DEBUG_PRINT("memcpy to DevPar.ChIedBip1[%d,%d]:%s\n", nChannel, nIed, DevPar.ChIedBip1[nChannel][nIed]);
			}
			
			if(pDev->ChIedBip2[nChannel][nIed] != NULL) {
				nLen = strlen(pDev->ChIedBip2[nChannel][nIed]);
				if(nLen < NAMELEN) strcpy(&DevPar.ChIedBip2[nChannel][nIed][0], pDev->ChIedBip2[nChannel][nIed]);
				else memcpy(&DevPar.ChIedBip2[nChannel][nIed][0], pDev->ChIedBip2[nChannel][nIed], (NAMELEN - 1));
				DEBUG_PRINT("memcpy to DevPar.ChIedBip2[%d,%d]:%s\n", nChannel, nIed, DevPar.ChIedBip2[nChannel][nIed]);
			}
		}
	}
	
	//8��װ��ͨ��n�ı���IED��SMV����
	for(nChannel = 0; nChannel < nCHANNEL; nChannel++) 
	{
		
		for(nIed = 0; nIed < nIED; nIed++) 
		{
			smvCfg = (SmvCfgInfo *)plh_getSmvCfgInfoPtr(nChannel, nIed);

			if(pDev->ChMuType[nChannel][nIed] != NULL)
			{
				DevPar.ChMuType[nChannel][nIed] = strtoul(pDev->ChMuType[nChannel][nIed], NULL, 0);	
				DEBUG_PRINT("memcpy to DevPar.ChMuType[%d,%d]:%d\n", nChannel, nIed, DevPar.ChMuType[nChannel][nIed]);
			}

			//add by haopeng 2012.10.7
			if(pDev->ChMuDmac[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChMuDmac[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChMuDmac[nChannel][nIed][0], pDev->ChMuDmac[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChMuDmac[nChannel][nIed][0], pDev->ChMuDmac[nChannel][nIed], (NAMELEN - 1));
				}

				str2Char(pDev->ChMuDmac[nChannel][nIed], smvCfg->destMac);
				printf("smvCfg->destMac[%d,%d]:%x-%x-%x-%x-%x-%x\n", nChannel, nIed,
											*smvCfg->destMac, *(smvCfg->destMac+1),
											*(smvCfg->destMac+2), *(smvCfg->destMac+3),
											*(smvCfg->destMac+4), *(smvCfg->destMac+5));
			}

			if(pDev->ChMuSmac[nChannel][nIed] != NULL)
			{
				nLen = strlen(pDev->ChMuSmac[nChannel][nIed]);
				if(nLen < NAMELEN) 
				{
					strcpy(&DevPar.ChMuSmac[nChannel][nIed][0], pDev->ChMuSmac[nChannel][nIed]);
				}
				else 
				{
					memcpy(&DevPar.ChMuSmac[nChannel][nIed][0], pDev->ChMuSmac[nChannel][nIed], (NAMELEN - 1));
				}
				
				DEBUG_PRINT("memcpy to DevPar.ChMuDmac[%d,%d]:%s\n", nChannel, nIed, DevPar.ChMuSmac[nChannel][nIed]);
			}
			
			if(pDev->ChMuSmpR[nChannel][nIed] != NULL)
			{
				DevPar.ChMuSmpR[nChannel][nIed] = strtoul(pDev->ChMuSmpR[nChannel][nIed], NULL, 0);				
				DEBUG_PRINT("memcpy to DevPar.ChMuSmpR[%d,%d]:%d\n", nChannel, nIed, DevPar.ChMuSmpR[nChannel][nIed]);
			}
			
			if(pDev->ChMuSmpD[nChannel][nIed] != NULL)
			{
				DevPar.ChMuSmpD[nChannel][nIed] = strtoul(pDev->ChMuSmpD[nChannel][nIed], NULL, 0);
				DEBUG_PRINT("memcpy to DevPar.ChMuSmpD[%d,%d]:%d\n", nChannel, nIed, DevPar.ChMuSmpD[nChannel][nIed]);
			}
			
			if(pDev->ChMuVlId[nChannel][nIed] != NULL)
			{
				DevPar.ChMuVlId[nChannel][nIed] = strtoul(pDev->ChMuVlId[nChannel][nIed], NULL, 0);
				smvCfg->nVlanID = DevPar.ChMuVlId[nChannel][nIed];
				DEBUG_PRINT("memcpy to DevPar.ChMuVlId[%d,%d]:%d\n", nChannel, nIed, DevPar.ChMuVlId[nChannel][nIed]);
			}
			
			if(pDev->ChMuPrio[nChannel][nIed] != NULL)
			{
				DevPar.ChMuPrio[nChannel][nIed] = strtoul(pDev->ChMuPrio[nChannel][nIed], NULL, 0);
				smvCfg->nPriority = DevPar.ChMuPrio[nChannel][nIed];
				DEBUG_PRINT("memcpy to DevPar.ChMuPrio[%d,%d]:%d\n", nChannel, nIed, DevPar.ChMuPrio[nChannel][nIed]);
			}
			
			if(pDev->ChMuApId[nChannel][nIed] != NULL)
			{
				DevPar.ChMuApId[nChannel][nIed] = strtoul(pDev->ChMuApId[nChannel][nIed], NULL, 0);
				smvCfg->appId = (unsigned short)DevPar.ChMuApId[nChannel][nIed];
				DEBUG_PRINT("memcpy to DevPar.ChMuApId[%d,%d]:%d\n", nChannel, nIed, DevPar.ChMuApId[nChannel][nIed]);
			}
			
			if(pDev->ChMuSvId[nChannel][nIed] != NULL)
			{
				DevPar.ChMuSvId[nChannel][nIed] = strtoul(pDev->ChMuSvId[nChannel][nIed], NULL, 0);
				DEBUG_PRINT("memcpy to DevPar.ChMuSvId[%d,%d]:%d\n", nChannel, nIed, DevPar.ChMuSvId[nChannel][nIed]);
			}
				
			if(pDev->ChMuDref[nChannel][nIed] != NULL)
			{
				DevPar.ChMuDref[nChannel][nIed] = strtoul(pDev->ChMuDref[nChannel][nIed], NULL, 0);
				DEBUG_PRINT("memcpy to DevPar.ChMuDref[%d,%d]:%d\n", nChannel, nIed, DevPar.ChMuDref[nChannel][nIed]);
			}

			if(pDev->ChMuDesc[nChannel][nIed] != NULL) {
				nLen = strlen(pDev->ChMuDesc[nChannel][nIed]);
				if(nLen < NAMELEN) strcpy(&DevPar.ChMuDesc[nChannel][nIed][0], pDev->ChMuDesc[nChannel][nIed]);
				else memcpy(&DevPar.ChMuDesc[nChannel][nIed][0], pDev->ChMuDesc[nChannel][nIed], (NAMELEN - 1));
				DEBUG_PRINT("memcpy to DevPar.ChMuDesc[%d,%d]:%s\n", nChannel, nIed, DevPar.ChMuDesc[nChannel][nIed]);
			}
		}
	}
	#if 0
	for(nChannel = 0; nChannel < nCHANNEL; nChannel++) {
		for(nMac = 0; nMac < nMAC; nMac++) {
			if(strlen(pDev->ChMuDmac[nChannel][nMac]) >= 16) {
				for(i = 0; i < 6; i++) {
					memset(strDigit, 0, sizeof(strDigit));
					memcpy(strDigit, pDev->ChMuDmac[nChannel][nMac] + 3*i, 2);
					DevPar.ChMuDmac[nChannel][nMac][i] = strtoul(strDigit, NULL, 0);
				}
			}
			if(strlen(pDev->ChMuSmac[nChannel][nMac]) >= 16) {
				for(i = 0; i < 6; i++) {
					memset(strDigit, 0, sizeof(strDigit));
					memcpy(strDigit, pDev->ChMuSmac[nChannel][nMac] + 3*i, 2);
					DevPar.ChMuSmac[nChannel][nMac][i] = strtoul(strDigit, NULL, 0);
				}
			}
		}
	}
	#endif
	GooseStrToPar();
	return 0;
}

int xml_printDeviceStr(int cmd)
{
	int nChannel, nVlan, nMac, nIed;

	if(cmd == 0) return -1;
	//1��װ�õ��б��֣����ơ�Ip��ַ����ţ�3����
	printf("TY-3012S xmlDoc DevName:%s!\n", pDev->DevName);
	printf("TY-3012S xmlDoc DevIp:%s!\n", pDev->DevIp);
	printf("TY-3012S xmlDoc DevNo:%s!\n", pDev->DevNo);
	//2��װ�ð汾����¼����װ�ð汾�ţ�xml�ļ��İ汾��!
	printf("TY-3012S xmlDoc DVersion:%s!\n", pDev->DevVer);
	//3��װ�õĵ�ַ���֣�Ip��ַ���������롢���ء����ܷ�����Ip��ַ�����ܷ������˿ڣ�5����
	printf("TY-3012S xmlDoc DLocalIp:%s!\n", pDev->DevLoIp);
	printf("TY-3012S xmlDoc DSubnetM:%s!\n", pDev->DevSnMask);
	printf("TY-3012S xmlDoc DGateway:%s!\n", pDev->DevGway);
	printf("TY-3012S xmlDoc DRemIp:  %s!\n", pDev->DevReIp);
	printf("TY-3012S xmlDoc DRemPort:%s!\n", pDev->DevRePort);
	//4��װ�õĲ������֣���š����ơ�ͨ�����������Ĵ洢��ʽ���洢���ڡ������������μ�顢ʱ����GPS�ӿڣ�10����
	printf("TY-3012S xmlDoc DSubNo:  %s!\n", pDev->DevSubN);
	printf("TY-3012S xmlDoc DSubName:%s!\n", pDev->DevSname);
	printf("TY-3012S xmlDoc DChNum:  %s!\n", pDev->DevChNum);
	printf("TY-3012S xmlDoc DFFormat:%s!\n", pDev->DevFileF);
	printf("TY-3012S xmlDoc DFPeriod:%s!\n", pDev->DevFileP);
	printf("TY-3012S xmlDoc DFMaxSiz:%s!\n", pDev->DevFileS);
	printf("TY-3012S xmlDoc DHdCheck:%s!\n", pDev->DevHdisk);
	printf("TY-3012S xmlDoc DTimeZon:%s!\n", pDev->DevTimeZ);
	printf("TY-3012S xmlDoc DHeartBP:%s!\n", pDev->DevBeatP);
	printf("TY-3012S xmlDoc DGpsPort:%s!\n", pDev->DevGpsIf);
	//5��װ�õ�ͨ�����֣�ͨ���š����ӿ����͡�����ʱ�䡢ץ�����ͣ�2��ͨ��8����
	for (nChannel = 0; nChannel < nCHANNEL; nChannel++) {
		printf("TY-3012S xmlDoc ChNo       [%x, %x]:%s!\n", nChannel, 0000,  pDev->ChNo[nChannel]);
		printf("TY-3012S xmlDoc ChCapPacket[%x, %x]:%s!\n", nChannel, 0000,  pDev->ChPacket[nChannel]);
		printf("TY-3012S xmlDoc ChInterface[%x, %x]:%s!\n", nChannel, 0000,  pDev->ChIfType[nChannel]);
		//6��װ��ͨ��n���˲���
		printf("TY-3012S xmlDoc ChMaxFraLen[%x, %x]:%s!\n", nChannel, 0000,  pDev->ChMaxLen[nChannel]);
		printf("TY-3012S xmlDoc ChMinFraLen[%x, %x]:%s!\n", nChannel, 0000,  pDev->ChMinLen[nChannel]);
		for (nVlan = 0; nVlan < nVLAN; nVlan++) {
			printf("TY-3012S xmlDoc ChannelVlNo[%x, %x]:%s!\n", nChannel, nVlan, pDev->ChVlanNo[nChannel][nVlan]);
			printf("TY-3012S xmlDoc ChannelVlId[%x, %x]:%s!\n", nChannel, nVlan, pDev->ChVlanId[nChannel][nVlan]);
		}
		for (nMac = 0; nMac < nMAC; nMac++) {
			printf("TY-3012S xmlDoc ChannelMacN[%x, %x]:%s!\n", nChannel, nMac,  pDev->ChMacNo [nChannel][nMac]);
			printf("TY-3012S xmlDoc ChannelMacA[%x, %x]:%s!\n", nChannel, nMac,  pDev->ChMacAdd[nChannel][nMac]);
		}
		printf("TY-3012S xmlDoc ChPacketTyp[%x, %x]:%s!\n", nChannel, 0000,  pDev->ChMacPkt[nChannel]);
		//7��װ��ͨ��n�ı���IED���֣�1��ͨ����¼����16��IED
		for (nIed = 0; nIed < nIED; nIed++) {
			printf("TY-3012S xmlDoc ChIedName[%x, %x]:%s!\n", nChannel, nIed, pDev->ChIedName[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedSMac[%x, %x]:%s!\n", nChannel, nIed, pDev->ChIedMac [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedType[%x, %x]:%s!\n", nChannel, nIed, pDev->ChIedType[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedBayD[%x, %x]:%s!\n", nChannel, nIed, pDev->ChIedBay [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedManu[%x, %x]:%s!\n", nChannel, nIed, pDev->ChIedMan [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedDesc[%x, %x]:%s!\n", nChannel, nIed, pDev->ChIedDesc[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedBIp1[%x, %x]:%s!\n", nChannel, nIed, pDev->ChIedBip1[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedBIp2[%x, %x]:%s!\n", nChannel, nIed, pDev->ChIedBip2[nChannel][nIed]);
			//8��װ��ͨ��n�ı���IED��SMV����
			printf("TY-3012S xmlDoc ChMuType [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuType [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChMuDMac [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuDmac [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChMuSMac [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuSmac [nChannel][nIed]);			
			printf("TY-3012S xmlDoc ChMuFreq [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuSmpR [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChMuSmpT [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuSmpD [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChMuVlId [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuVlId [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChMuPrio [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuPrio [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChMuApId [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuApId [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChMuSvId [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuSvId [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChMuDRef [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuDref [nChannel][nIed]);
			printf("TY-3012S xmlDoc ChMuDesc [%x, %x]:%s!\n", nChannel, nIed, pDev->ChMuDesc [nChannel][nIed]);
		}
	}
	return 0;
}

int xml_freeDevice(int cmd)
{
	int nChannel, nVlan, nMac, nIed;

	if(cmd == 0) return -1;
	//װ�õ��б��֣����ơ�Ip��ַ����ţ�3����
	if(pDev->DevName != NULL) free(pDev->DevName);
	if(pDev->DevIp   != NULL) free(pDev->DevIp);
	if(pDev->DevNo   != NULL) free(pDev->DevNo);
	//װ�ð汾����¼����װ�ð汾�ţ�xml�ļ��İ汾��!
	if(pDev->DevVer  != NULL) free(pDev->DevVer);
	//װ�õĵ�ַ���֣�Ip��ַ���������롢���ء����ܷ�����Ip��ַ�����ܷ������˿ڣ�5����
	if(pDev->DevLoIp != NULL) free(pDev->DevLoIp);
	if(pDev->DevSnMask != NULL) free(pDev->DevSnMask);
	if(pDev->DevGway != NULL) free(pDev->DevGway);
	if(pDev->DevReIp != NULL) free(pDev->DevReIp);
	if(pDev->DevRePort != NULL) free(pDev->DevRePort);
	//װ�õĲ������֣���š����ơ�ͨ�����������Ĵ洢��ʽ���洢���ڡ������������μ�顢ʱ����GPS�ӿڣ�10����
	if(pDev->DevSubN  != NULL) free(pDev->DevSubN);
	if(pDev->DevSname != NULL) free(pDev->DevSname);
	if(pDev->DevChNum != NULL) free(pDev->DevChNum);
	if(pDev->DevFileF != NULL) free(pDev->DevFileF);
	if(pDev->DevFileP != NULL) free(pDev->DevFileP);
	if(pDev->DevFileS != NULL) free(pDev->DevFileS);
	if(pDev->DevHdisk != NULL) free(pDev->DevHdisk);
	if(pDev->DevTimeZ != NULL) free(pDev->DevTimeZ);
	if(pDev->DevBeatP != NULL) free(pDev->DevBeatP);
	if(pDev->DevGpsIf != NULL) free(pDev->DevGpsIf);
	//װ�õ�ͨ�����֣�ͨ���š����ӿ����͡�����ʱ�䡢ץ�����ͣ�2��ͨ��8����
	for (nChannel = 0; nChannel < nCHANNEL; nChannel++) {
		if(pDev->ChNo[nChannel]     != NULL) free(pDev->ChNo[nChannel]);
		if(pDev->ChIfType[nChannel] != NULL) free(pDev->ChIfType[nChannel]);
		if(pDev->ChTrTime[nChannel] != NULL) free(pDev->ChTrTime[nChannel]);
		if(pDev->ChPacket[nChannel] != NULL) free(pDev->ChPacket[nChannel]);
		if(pDev->ChAcsLocation[nChannel] != NULL) free(pDev->ChAcsLocation[nChannel]);
		//װ��ͨ��n���˲���
		if(pDev->ChMinLen[nChannel] != NULL) free(pDev->ChMinLen[nChannel]);
		if(pDev->ChMaxLen[nChannel] != NULL) free(pDev->ChMaxLen[nChannel]);
////////////////
		for (nVlan = 0; nVlan < nVLAN; nVlan++) {
			if(pDev->ChVlanNo[nChannel][nVlan] != NULL) free(pDev->ChVlanNo[nChannel][nVlan]);
			if(pDev->ChVlanId[nChannel][nVlan] != NULL) free(pDev->ChVlanId[nChannel][nVlan]);
		}
		for (nMac = 0; nMac < nMAC; nMac++) {
			if(pDev->ChMacNo[nChannel][nMac]	!= NULL) free(pDev->ChMacNo[nChannel][nMac]);
			if(pDev->ChMacAdd[nChannel][nMac]	!= NULL) free(pDev->ChMacAdd[nChannel][nMac]);
		}
		if(pDev->ChMacPkt[nChannel]	!= NULL) free(pDev->ChMacPkt[nChannel]);
		//װ��ͨ��n�ı���IED���֣�1��ͨ����¼����16��IED
		for (nIed = 0; nIed < nIED; nIed++) {
			if(pDev->ChIedName[nChannel][nIed]	!= NULL) free(pDev->ChIedName[nChannel][nIed]);
			if(pDev->ChIedMac[nChannel][nIed]	!= NULL) free(pDev->ChIedMac[nChannel][nIed]);
			if(pDev->ChIedType[nChannel][nIed]	!= NULL) free(pDev->ChIedType[nChannel][nIed]);
			if(pDev->ChIedBay[nChannel][nIed]	!= NULL) free(pDev->ChIedBay[nChannel][nIed]);
			if(pDev->ChIedMan[nChannel][nIed]	!= NULL) free(pDev->ChIedMan[nChannel][nIed]);
			if(pDev->ChIedDesc[nChannel][nIed]	!= NULL) free(pDev->ChIedDesc[nChannel][nIed]);
			if(pDev->ChIedBip1[nChannel][nIed]	!= NULL) free(pDev->ChIedBip1[nChannel][nIed]);
			if(pDev->ChIedBip2[nChannel][nIed]	!= NULL) free(pDev->ChIedBip2[nChannel][nIed]);
			//װ��ͨ��n�ı���IED��SMV����
			if(pDev->ChMuType[nChannel][nIed]	!= NULL) free(pDev->ChMuType[nChannel][nIed]);
			if(pDev->ChMuDmac[nChannel][nIed]	!= NULL) free(pDev->ChMuDmac[nChannel][nIed]);
			if(pDev->ChMuSmac[nChannel][nIed]	!= NULL) free(pDev->ChMuSmac[nChannel][nIed]);
			if(pDev->ChMuSmpR[nChannel][nIed]	!= NULL) free(pDev->ChMuSmpR[nChannel][nIed]);
			if(pDev->ChMuSmpD[nChannel][nIed]	!= NULL) free(pDev->ChMuSmpD[nChannel][nIed]);
			if(pDev->ChMuVlId[nChannel][nIed]	!= NULL) free(pDev->ChMuVlId[nChannel][nIed]);
			if(pDev->ChMuPrio[nChannel][nIed]	!= NULL) free(pDev->ChMuPrio[nChannel][nIed]);
			if(pDev->ChMuApId[nChannel][nIed]	!= NULL) free(pDev->ChMuApId[nChannel][nIed]);
			if(pDev->ChMuSvId[nChannel][nIed]	!= NULL) free(pDev->ChMuSvId[nChannel][nIed]);
			if(pDev->ChMuDref[nChannel][nIed]	!= NULL) free(pDev->ChMuDref[nChannel][nIed]);
			if(pDev->ChMuDesc[nChannel][nIed]	!= NULL) free(pDev->ChMuDesc[nChannel][nIed]);
			//װ��GOOSE����
			if(pDev->ChGSGoId[nChannel][nIed]	!= NULL) free(pDev->ChGSGoId[nChannel][nIed]);
			if(pDev->ChGSDmac[nChannel][nIed]	!= NULL) free(pDev->ChGSDmac[nChannel][nIed]);
			if(pDev->ChGSSmac[nChannel][nIed]	!= NULL) free(pDev->ChGSSmac[nChannel][nIed]);
			if(pDev->ChGSSmac2[nChannel][nIed] != NULL) free(pDev->ChGSSmac2[nChannel][nIed]);
			if(pDev->ChGSVlId[nChannel][nIed]	!= NULL) free(pDev->ChGSVlId[nChannel][nIed]);
			if(pDev->ChGSPrio[nChannel][nIed]	!= NULL) free(pDev->ChGSPrio[nChannel][nIed]);
			if(pDev->ChGSApId[nChannel][nIed]	!= NULL) free(pDev->ChGSApId[nChannel][nIed]);
			if(pDev->ChGSref[nChannel][nIed]	!= NULL) free(pDev->ChGSref[nChannel][nIed]);
			if(pDev->ChGSDref[nChannel][nIed]	!= NULL) free(pDev->ChGSDref[nChannel][nIed]);
			if(pDev->ChGSLdInst[nChannel][nIed] != NULL) free(pDev->ChGSLdInst[nChannel][nIed]);
			if(pDev->ChGSminTm[nChannel][nIed] != NULL) free(pDev->ChGSminTm[nChannel][nIed]);
			if(pDev->ChGSmaxTm[nChannel][nIed] != NULL) free(pDev->ChGSmaxTm[nChannel][nIed]);
		}
	}
	//װ�ò���
	if(pDev != NULL) free(pDev);
	return 0;
}

static int CopyStr(xmlChar* src, char** dst)
{
	int nLen;

	nLen = strlen(src);
	if(NULL != *dst) free(*dst);
	*dst = malloc(nLen + 1);
	memset(*dst, 0, nLen + 1);
	memcpy(*dst, src, nLen);
	return 0;
}

//add by haopeng for GOOSE node parse
static int HandleNodeGOOSE(xmlDocPtr xmlDOC, xmlNodePtr nodeGoose, int nChannel, int nIed)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	xmlChar*	strGs;	//�ڵ�����
	xmlAttrPtr	attrPtr;//�ڵ�����ָ��
	xmlChar*	vAttr;	//�ڵ���������

	nodeCur = nodeGoose;
	DEBUG_PRINT("HandleNodeGOOSE,nodeCur-name:%s\n",nodeCur->name);
	/*�Ƚ���GOOSE����*/
	attrPtr = nodeCur->properties;			//ָ������cbRef
	
	while(attrPtr != NULL) {						//attrPtr����ָ�벻��
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "cbRef")) {		//attrPtr����������"name"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"cbRef");	//vAttr  ��������ָ��ָ��"name"���Ե�����"IED1"
			CopyStr(vAttr, &(pDev->ChGSAref[nChannel][nIed]));
			xmlFree(vAttr);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSAref[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSAref[nChannel][nIed]);
		}
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "datasetRef")) { 		//attrPtr����������"mac"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"datasetRef");	//vAttr  ��������ָ��ָ��"mac"���Ե�����"11:22:33:44:55:66"
			CopyStr(vAttr, &(pDev->ChGSADref[nChannel][nIed]));
			xmlFree(vAttr);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSADref[%x]:%s\n", nChannel, pDev->ChGSADref[nChannel][nIed]);
		}		
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "goID")) { 		//attrPtr����������"mac"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"goID");	//vAttr  ��������ָ��ָ��"mac"���Ե�����"11:22:33:44:55:66"
			CopyStr(vAttr, &(pDev->ChGSAGoId[nChannel][nIed]));
			xmlFree(vAttr);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSAGoId[%x]:%s\n", nChannel, pDev->ChGSAGoId[nChannel][nIed]);
		}
		attrPtr = attrPtr->next;					//attrPtr����ָ��ָ����һ�����ԣ�name->mac
	}

	/*�ٽ���GOOSE�ӽڵ�*/
	nodeCur = nodeCur->xmlChildrenNode;
	while(nodeCur != NULL) {		//nodeCur�ڵ�ָ�벻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"GoId")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSGoId[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSGoId[%x, %x]:%s\n",nChannel, nIed, pDev->ChGSGoId[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"DestMac")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSDmac[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSDmac[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSDmac[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"SourceMac")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSSmac[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSSmac[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSSmac[nChannel][nIed]);			
		}else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"SourceMac2")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSSmac2[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSSmac2[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSSmac2[nChannel][nIed]);			
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"VlanId")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSVlId[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSVlId[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSVlId[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Priority")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSPrio[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSPrio[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSPrio[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"AppId")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSApId[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSApId[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSApId[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"CbRef")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSref[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSref[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSref[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name,  (const xmlChar*)"DatasetRef")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSDref[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSDref[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSDref[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name,  (const xmlChar*)"LdInst")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSLdInst[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSLdInst[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSLdInst[nChannel][nIed]);
		}  else if(!xmlStrcmp(nodeCur->name,  (const xmlChar*)"MinTime")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSminTm[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSminTm[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSminTm[nChannel][nIed]);
		}  else if(!xmlStrcmp(nodeCur->name,  (const xmlChar*)"MaxTime")) {
			strGs = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strGs, &(pDev->ChGSmaxTm[nChannel][nIed]));
			xmlFree(strGs);
			DEBUG_PRINT("TY-3012S xmlDoc ChGSmaxTm[%x, %x]:%s\n", nChannel, nIed, pDev->ChGSmaxTm[nChannel][nIed]);
		}  
		nodeCur = nodeCur->next;	//(IED->Type)��Type->Bay->Manufacturer->Desc->BayIp->BayIp2->SMV
	}
	return 0;

}


static int HandleNodeGOOSEList(xmlDocPtr xmlDOC, xmlNodePtr nodeGoose, int nChannel, int nIed)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	int retv = 0;
	
	nodeCur = nodeGoose->xmlChildrenNode;	//nodeCur�ڵ�ָ��ָ���¸��ӽڵ�Type��GOOSEList->GOOSE

	while(nodeCur != NULL) {					//IED�ڵ㲻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"GOOSE")) {	//�ڵ�������IED
			DEBUG_PRINT("find a node GOOSE to handle\n");
			retv = HandleNodeGOOSE(xmlDOC, nodeCur, nChannel, nIed);			
		}
		nodeCur = nodeCur->next;
	}
	return 0;
}

static int HandleNodeSmv92(xmlDocPtr xmlDOC, xmlNodePtr nodeSmv92, int nChannel, int nIed)
{
	xmlNodePtr	nodeCur;	//�ڵ�ָ��
	xmlChar*	strSmv92;	//�ڵ�����
	xmlAttrPtr	attrPtr;	//�ڵ�����ָ��
	xmlChar*	nAttr;		//�ڵ���������
	xmlChar*	vAttr;		//�ڵ���������

	nodeCur = nodeSmv92->xmlChildrenNode;	//nodeCur�ڵ�ָ��ָ���¸��ӽڵ�SmvId��Smv92->SmvId
	while(nodeCur != NULL) {		//nodeCur�ڵ�ָ�벻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"SmvId")) {
			strSmv92 = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv92, &(pDev->ChMuSvId[nChannel][nIed]));
			xmlFree(strSmv92);
			printf("TY-3012S xmlDoc ChMuSvId[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuSvId[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"DatasetRef")) {
			strSmv92 = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv92, &(pDev->ChMuDref[nChannel][nIed]));
			xmlFree(strSmv92);
			printf("TY-3012S xmlDoc Channel1IedMuDatasetRef[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuDref[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Channel")) {
			attrPtr = nodeCur->properties;						//attrPtr����ָ��ָ��Channel�ڵ�����"desc"
			while(attrPtr != NULL) {						//attrPtr����ָ�벻��
				if(!xmlStrcmp(attrPtr->name, BAD_CAST "desc")) {		//attrPtr����������"desc"
					nAttr = xmlGetProp(nodeCur, BAD_CAST "desc");		//nAttr  ��������ָ��ָ������"desc"
					printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);	//nAttr  �����ַ���ʾ��Ӧ����"desc"
					vAttr = xmlGetProp(nodeCur, (const xmlChar *)"desc");	//vAttr  ��������ָ��ָ��"desc"���Ե�����"11:22:33:44:55:66"
					CopyStr(vAttr, &(pDev->ChMuDesc[nChannel][nIed]));
					xmlFree(vAttr);
					printf("TY-3012S xmlDoc Channel1IedMuChannelDesc[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuDesc[nChannel][nIed]);
				}
				attrPtr = attrPtr->next;	//attrPtr����ָ��ָ����һ�����ԣ�desc->��
			}
		}
		nodeCur = nodeCur->next;	//(nodeSMV92->SmvId)��SmvId->DatasetRef-Channel:desc
	}
	return 0;
}

static int HandleNodeSMV(xmlDocPtr xmlDOC, xmlNodePtr nodeSMV, int nChannel, int nIed)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	xmlChar*	strSmv;	//�ڵ�����
	int retv;

	retv = 0;
	nodeCur = nodeSMV->xmlChildrenNode;	//nodeCur�ڵ�ָ��ָ���¸��ӽڵ�Type��SMV->Type
	while(nodeCur != NULL) {		//nodeCur�ڵ�ָ�벻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Type")) {
			strSmv = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv, &(pDev->ChMuType[nChannel][nIed]));
			xmlFree(strSmv);
			printf("TY-3012S xmlDoc ChMuType[%x, %x]:%s\n",nChannel, nIed, pDev->ChMuType[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"DestMac")) {
			strSmv = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv, &(pDev->ChMuDmac[nChannel][nIed]));
			xmlFree(strSmv);
			printf("TY-3012S xmlDoc ChMuDmac[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuDmac[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"SourceMac")) {
			strSmv = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv, &(pDev->ChMuSmac[nChannel][nIed]));
			xmlFree(strSmv);
			printf("TY-3012S xmlDoc ChMuSmac[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuSmac[nChannel][nIed]);			
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"SmpRatePerFreq")) {
			strSmv = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv, &(pDev->ChMuSmpR[nChannel][nIed]));
			xmlFree(strSmv);
			printf("TY-3012S xmlDoc ChMuSmpR[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuSmpR[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"SmpTimeDeviation")) {
			strSmv = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv, &(pDev->ChMuSmpD[nChannel][nIed]));
			xmlFree(strSmv);
			printf("TY-3012S xmlDoc ChMuSmpD[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuSmpD[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"VlanId")) {
			strSmv = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv, &(pDev->ChMuVlId[nChannel][nIed]));
			xmlFree(strSmv);
			printf("TY-3012S xmlDoc ChMuVlId[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuVlId[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Priority")) {
			strSmv = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv, &(pDev->ChMuPrio[nChannel][nIed]));
			xmlFree(strSmv);
			printf("TY-3012S xmlDoc ChMuPrio[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuPrio[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name,  (const xmlChar*)"AppId")) {
			strSmv = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strSmv, &(pDev->ChMuApId[nChannel][nIed]));
			xmlFree(strSmv);
			printf("TY-3012S xmlDoc ChMuApId[%x, %x]:%s\n", nChannel, nIed, pDev->ChMuApId[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name,  (const xmlChar*)"Smv92")) {
			retv = HandleNodeSmv92(xmlDOC, nodeCur, nChannel, nIed);
		} 
		nodeCur = nodeCur->next;	//(IED->Type)��Type->Bay->Manufacturer->Desc->BayIp->BayIp2->SMV
	}
	return retv;
}	

static int HandleNodeIED(xmlDocPtr xmlDOC, xmlNodePtr nodeIED, int nChannel, int nIed)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	xmlChar*	strIed;	//�ڵ�����
	xmlAttrPtr	attrPtr;//�ڵ�����ָ��
//	xmlChar*	nAttr;	//�ڵ���������
	xmlChar*	vAttr;	//�ڵ���������
	int retv;

	retv = 0;
	nodeCur = nodeIED;							//nodeCur�ڵ�ָ��ָ��IED�ڵ�
	attrPtr = nodeCur->properties;						//attrPtr����ָ��ָ��IED�ڵ�����"name"
	while(attrPtr != NULL) {						//attrPtr����ָ�벻��
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "name")) {		//attrPtr����������"name"
//			nAttr = xmlGetProp(nodeCur, BAD_CAST "name");		//nAttr  ��������ָ��ָ������"name"
//			printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);	//nAttr  ����������ʾ��Ӧ����"name"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"name");	//vAttr  ��������ָ��ָ��"name"���Ե�����"IED1"
			CopyStr(vAttr, &(pDev->ChIedName[nChannel][nIed]));
			xmlFree(vAttr);
			printf("TY-3012S xmlDoc ChIedName[%x, %x]:%s\n", nChannel, nIed, pDev->ChIedName[nChannel][nIed]);
		}
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "mac")) {			//attrPtr����������"mac"
//			nAttr = xmlGetProp(nodeCur, BAD_CAST "mac");		//nAttr  ��������ָ��ָ������"mac"
//			printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);	//nAttr  ����������ʾ��Ӧ����"mac"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"mac");	//vAttr  ��������ָ��ָ��"mac"���Ե�����"11:22:33:44:55:66"
			CopyStr(vAttr, &(pDev->ChIedMac[nChannel][nIed]));

			/*�����������*/
			/*���������ڵ����,���,��������������Ҫ�����ý���֮ǰ���*/

			/*********************/

			xmlFree(vAttr);
//			printf("TY-3012S xmlDoc ChIedMac[%x]:%s\n", nChannel, nIed, pDev->ChIedMac[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedMac[%x]:%s\n", nChannel, pDev->ChIedMac[nChannel][nIed]);
		}
		attrPtr = attrPtr->next;					//attrPtr����ָ��ָ����һ�����ԣ�name->mac
	}
	nodeCur = nodeCur->xmlChildrenNode;					//nodeCur�ڵ�ָ��ָ���¸��ӽڵ㣬IED->Type
	while(nodeCur != NULL) {						//nodeCur�ڵ�ָ�벻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Type")) {
			strIed = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strIed, &(pDev->ChIedType[nChannel][nIed]));
			xmlFree(strIed);
//			printf("TY-3012S xmlDoc ChIedType[%x]:%s\n", nChannel, nIed, pDev->ChIedType[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedType[%x]:%s\n", nChannel, pDev->ChIedType[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Bay")) {
			strIed = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strIed, &(pDev->ChIedBay[nChannel][nIed]));
			xmlFree(strIed);
//			printf("TY-3012S xmlDoc ChIedBay[%x]:%s\n", nChannel, nIed, pDev->ChIedBay[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedBay[%x]:%s\n", nChannel, pDev->ChIedBay[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Manufacturer")) {
			strIed = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strIed, &(pDev->ChIedMan[nChannel][nIed]));
			xmlFree(strIed);
//			printf("TY-3012S xmlDoc ChIedMan[%x]:%s\n", nChannel, nIed, pDev->ChIedMan[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedMan[%x]:%s\n", nChannel, pDev->ChIedMan[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Desc")) {
			strIed = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strIed, &(pDev->ChIedDesc[nChannel][nIed]));
			xmlFree(strIed);
//			printf("TY-3012S xmlDoc ChIedDesc[%x]:%s\n", nChannel, nIed, pDev->ChIedDesc[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedDesc[%x]:%s\n", nChannel, pDev->ChIedDesc[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"BayIp")) {
			strIed = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strIed, &(pDev->ChIedBip1[nChannel][nIed]));
			xmlFree(strIed);
//			printf("TY-3012S xmlDoc ChIedBip1[%x]:%s\n", nChannel, nIed, pDev->ChIedBip1[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedBip1[%x]:%s\n", nChannel, pDev->ChIedBip1[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"BayIp2")) {
			strIed = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strIed, &(pDev->ChIedBip2[nChannel][nIed]));
			xmlFree(strIed);
//			printf("TY-3012S xmlDoc ChIedBip2[%x]:%s\n", nChannel, nIed, pDev->ChIedBip2[nChannel][nIed]);
			printf("TY-3012S xmlDoc ChIedBip2[%x]:%s\n", nChannel, pDev->ChIedBip2[nChannel][nIed]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"SMV")) {
			retv = HandleNodeSMV(xmlDOC, nodeCur, nChannel, nIed);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"GOOSEList")) {	//add by haopeng for GOOSE config parse
			retv = HandleNodeGOOSEList(xmlDOC, nodeCur, nChannel, nIed);
		}
		nodeCur = nodeCur->next;	//(IED->Type)��Type->Bay->Manufacturer->Desc->BayIp->BayIp2->SMV
	}
	return retv;
}

//SCL-(Substation)DeviceList-Device(1)(2)(...)(16)
//-(Common)(Address)(Parameter)ChannelList-Channel(1)(2)
//-(CapturePacketType)(InterfaceType)(Filter)IEDList-IED(1)(2)(...)(16)
//-<Type><Bay><Manufacturer><Desc><BayIp><BayIp2><SMV><GOOSEList>
static int HandleNodeIEDList(xmlDocPtr xmlDOC, xmlNodePtr nodeIEDList, int nChannel)
{
	xmlNodePtr nodeCur;
	int nIed;
	int retv;

	retv = 0;
	nIed = 0;
	nodeCur = nodeIEDList->xmlChildrenNode;				//ָ��IED(1)(2)(...)(16)
	while(nodeCur != NULL) {					//IED�ڵ㲻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"IED")) {	//�ڵ�������IED
			retv = HandleNodeIED(xmlDOC, nodeCur, nChannel, nIed);
			nIed++;
			if(nIed >= nIED) break;	//nIED=16
		}
		nodeCur = nodeCur->next;
	}
	return retv;
}

static int HandleNodeMac(xmlDocPtr xmlDOC, xmlNodePtr nodeMac, int nChannel, int nMac)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	xmlAttrPtr	attrPtr;//�ڵ�����ָ��
	xmlChar*	nAttr;	//�ڵ���������
	xmlChar*	vAttr;	//�ڵ���������

	nodeCur = nodeMac;							//nodeCur�ڵ�ָ��ָ��Mac�ڵ�
	attrPtr = nodeCur->properties;						//attrPtr����ָ��ָ��Mac�ڵ�����"no"
	while(attrPtr != NULL) {						//attrPtr����ָ�벻��
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "no")) {			//attrPtr����������"no"��Mac���
			nAttr = xmlGetProp(nodeCur, BAD_CAST "no");		//nAttr  ��������ָ��ָ������"no"
			printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);	//nAttr  ����������ʾ��Ӧ����"no"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"no");	//vAttr  ��������ָ��ָ��"no"���Ե�����"1,2,...,16"
			CopyStr(vAttr, &(pDev->ChMacNo[nChannel][nMac]));
			xmlFree(vAttr);
			printf("TY-3012S xmlDoc ChMacNo[%x, %x]:%s\n", nChannel, nMac, pDev->ChMacNo[nChannel][nMac]);
		}
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "value")) {		//attrPtr����������"value"
			nAttr = xmlGetProp(nodeCur, BAD_CAST "value");		//nAttr  �����ַ�ָ��ָ������"value"
			printf("TY-3012S xmlDoc vAttr:%s\n", vAttr);	//nAttr  �����ַ���ʾ��Ӧ����"value"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"value");  //vAttr  ��������ָ��ָ��"value"���Ե�����"16��Mac��ַ"
			CopyStr(vAttr, &(pDev->ChMacAdd[nChannel][nMac]));
			xmlFree(vAttr);
			printf("TY-3012S xmlDoc ChMacAdd[%x, %x]:%s\n", nChannel, nMac, pDev->ChMacAdd[nChannel][nMac]);
		}
		attrPtr = attrPtr->next;					//attrPtr����ָ��ָ����һ�����ԣ�no->value
	}
	return 0;
}

static int HandleNodeMacList(xmlDocPtr xmlDOC, xmlNodePtr nodeMacList, int nChannel)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	int nMac;		//Mac������װ����16��Mac��ַ
	int retv;

	retv = 0;
	nMac = 0;
	nodeCur = nodeMacList->xmlChildrenNode;	//nodeCur�ڵ�ָ��ָ��Mac�ڵ�
	while(nodeCur != NULL) {		//nodeCur�ڵ�ָ�벻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Mac")) {
			retv = HandleNodeMac(xmlDOC, nodeCur, nChannel, nMac);
			nMac++;
			if(nMac >= nMAC) break;	//nMAC=16
		}
		nodeCur = nodeCur->next;
	}
	return retv;
}

static int HandleNodeV(xmlDocPtr xmlDOC, xmlNodePtr nodeV, int nChannel, int nVlan)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	xmlAttrPtr	attrPtr;//�ڵ�����ָ��
	xmlChar*	nAttr;	//�ڵ���������
	xmlChar*	vAttr;	//�ڵ���������

	nodeCur = nodeV;							//nodeCur�ڵ�ָ��ָ��V�ڵ�
	attrPtr = nodeCur->properties;						//attrPtr����ָ��ָ��V�ڵ�����"no"
	while(attrPtr != NULL) {						//attrPtr����ָ�벻��
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "no")) {			//attrPtr����������"no"
			nAttr = xmlGetProp(nodeCur, BAD_CAST "no");		//nAttr  ��������ָ��ָ������"no"
			printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);	//nAttr  ����������ʾ��Ӧ����"no"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"no");	//vAttr  ��������ָ��ָ��"no"���Ե�����"1/2"
			CopyStr(vAttr, &(pDev->ChVlanNo[nChannel][nVlan]));
			xmlFree(vAttr);
			printf("TY-3012S xmlDoc ChVlanNo[%x, %x]:%s\n", nChannel, nVlan, pDev->ChVlanNo[nChannel][nVlan]);
		}
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "value")) {		//attrPtr����������"value"
			nAttr = xmlGetProp(nodeCur, BAD_CAST "value");		//nAttr  �����ַ�ָ��ָ������"value"
			printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);	//nAttr  ����������ʾ��Ӧ����"value"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"value");	//vAttr  ��������ָ��ָ��"value"���Ե�����"1234"
			CopyStr(vAttr, &(pDev->ChVlanId[nChannel][nVlan]));
			xmlFree(vAttr);
			printf("TY-3012S xmlDoc ChVlanId[%x, %x]:%s\n", nChannel, nVlan, pDev->ChVlanId[nChannel][nVlan]);
		}
		attrPtr = attrPtr->next;					//attrPtr����ָ��ָ����һ�����ԣ�no->value
	}
	return 0;
}

static int HandleNodeVlanId(xmlDocPtr xmlDOC, xmlNodePtr nodeVlanId, int nChannel)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	int		nVlan;	//Vlan������װ����2��Vlan
	int		retv;

	retv = 0;
	nVlan = 0;
	nodeCur = nodeVlanId->xmlChildrenNode;	//nodeCur�ڵ�ָ��ָ��V�ڵ�
	while(nodeCur != NULL) {		//nodeCur�ڵ�ָ�벻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"V")) {
			retv = HandleNodeV(xmlDOC, nodeCur, nChannel, nVlan);
			nVlan++;
			if(nVlan >= nVLAN) break;	//nVLAN=2
		}
		nodeCur = nodeCur->next;	//V->V����2���ڵ�
	}
	return retv;
}

//SCL-(Substation)DeviceList-Device(1)(2)(...)(16)
//-(Common)(Address)(Parameter)ChannelList-Channel(1)(2)
//-(CapturePacketType)(InterfaceType)Filter(IEDList)
//-<MaxFrameLen><MinFrameLen><VlanId><MacList><PacketType>
static int HandleNodeFilter(xmlDocPtr xmlDOC, xmlNodePtr nodeFilter, int nChannel)
{
	xmlNodePtr	nodeCur;		//�ڵ�ָ��
	xmlChar*	strFilter;		//�ڵ�����
	int		retv;

	retv = 0;
	nodeCur = nodeFilter->xmlChildrenNode;	//nodeCur�ڵ�ָ��ָ���¸��ӽڵ�MaxFrameLen��nodeFilter->MaxFrameLen
	while(nodeCur != NULL) {		//nodeCur�ڵ�ָ�벻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"MaxFrameLen")) {
			strFilter = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strFilter, &(pDev->ChMaxLen[nChannel]));
			xmlFree(strFilter);
			printf("TY-3012S xmlDoc ChMaxLen[%x]:%s\n", nChannel, pDev->ChMaxLen[nChannel]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"MinFrameLen")) {
			strFilter = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strFilter, &(pDev->ChMinLen[nChannel]));
			xmlFree(strFilter);
			printf("TY-3012S xmlDoc ChMinLen[%x]:%s\n", nChannel, pDev->ChMinLen[nChannel]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"VlanId")) {
			retv = HandleNodeVlanId(xmlDOC, nodeCur, nChannel);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"MacList")) {
			retv = HandleNodeMacList(xmlDOC, nodeCur, nChannel);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"PacketType")) {
			strFilter = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strFilter, &(pDev->ChMacPkt[nChannel]));
			xmlFree(strFilter);
			printf("TY-3012S xmlDoc ChMacPkt[%x]:%s\n", nChannel, pDev->ChMacPkt[nChannel]);
		}
		nodeCur = nodeCur->next;	//(nodeFilter->MaxFrameLen)��MaxFrameLen->MinFrameLen->VlanId->MacList->PacketType
	}
	return retv;
}

static int HandleNodeChannel(xmlDocPtr xmlDOC, xmlNodePtr nodeChannel,  int nChannel)
{
	xmlNodePtr	nodeCur;	//�ڵ�ָ��
	xmlChar*	strChannel;	//�ڵ�����
	xmlAttrPtr	attrPtr;	//�ڵ�����ָ��
	xmlChar*	nAttr;		//�ڵ���������
	xmlChar*	vAttr;		//�ڵ���������
	int		retv;

	retv = 0;
	nodeCur = nodeChannel;			//add by haopeng for debug 2012.10.5 �����ָ��Ҫ��ֵ������
	attrPtr = nodeCur->properties;						//attrPtrָ��Channel������"no"

	while(attrPtr != NULL) {						//Channel������"no"����
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "no")) {			//Channel�������Ƿ���"no"
			nAttr = xmlGetProp(nodeCur, BAD_CAST "no");		//nAttr  ��������ָ��ָ������"no"	
			printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);	//nAttr  ����������ʾ��Ӧ����"no"
			vAttr = xmlGetProp(nodeCur, (const xmlChar *)"no");	//vAttr  ��������ָ��ָ��"no"���Ե�����"1,2,...,16"
			CopyStr(vAttr, &(pDev->ChNo[nChannel]));
			xmlFree(vAttr);
			printf("TY-3012S xmlDoc ChNo[%x]:%s\n", nChannel, pDev->ChNo[nChannel]);
			break;
		}
		attrPtr = attrPtr->next;
	}
	nodeCur = nodeCur->xmlChildrenNode;	//nodeCur�ڵ�ָ��ָ��CapturePacketType�ڵ�
	while(nodeCur != NULL) {		//nodeCur�ڵ㲻�գ�CapturePacketType�ڵ㲻��
		DEBUG_PRINT("nodeCur name is :%s\n", nodeCur->name);
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"CapturePacketType")) {
			strChannel = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strChannel, &(pDev->ChPacket[nChannel]));
			xmlFree(strChannel);
			printf("TY-3012S xmlDoc ChPacket[%x]:%s\n", nChannel, pDev->ChPacket[nChannel]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"InterfaceType")) {
			strChannel = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strChannel, &(pDev->ChIfType[nChannel]));
			xmlFree(strChannel);
			printf("TY-3012S xmlDoc ChIfType[%x]:%s\n", nChannel, pDev->ChIfType[nChannel]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"AccessLocation")) {
			strChannel = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strChannel, &(pDev->ChAcsLocation[nChannel]));
			xmlFree(strChannel);
			printf("TY-3012S xmlDoc ChAcsLocation[%x]:%s\n", nChannel, pDev->ChAcsLocation[nChannel]);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Filter")) {
			retv = HandleNodeFilter(xmlDOC, nodeCur, nChannel);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"IEDList")) {
			retv = HandleNodeIEDList(xmlDOC, nodeCur, nChannel);
		}
		nodeCur = nodeCur->next;
	}
	return retv;
}

static int HandleNodeChannelList(xmlDocPtr xmlDOC, xmlNodePtr nodeChannelList)
{
	xmlNodePtr	nodeCur;
	int		nChannel;
	int		retv;

	retv = 0;
	nChannel = 0;
//	nodeCur = nodeCur->xmlChildrenNode;					//nodeCur�ڵ�ָ��ָ��Channel�ڵ�
	nodeCur = nodeChannelList->xmlChildrenNode;					//nodeCur�ڵ�ָ��ָ��Channel�ڵ�
	while(nodeCur != NULL) {						//Channel�ڵ㲻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Channel")) {	//�ж��Ƿ�Channel�ڵ�
			retv = HandleNodeChannel(xmlDOC, nodeCur, nChannel);	//��
			nChannel++;
			if(nChannel >= nCHANNEL) break;				//nCHANNEL=2
		}
		nodeCur = nodeCur->next;					//Channel->Channel->Channel...
	}
	return retv;
}

static int HandleNodePar(xmlDocPtr xmlDOC, xmlNodePtr nodePar)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	xmlChar*	strPar;	//�ڵ�����

	nodeCur = nodePar->xmlChildrenNode;					//nodeCur�ڵ�ָ��ָ��DeviceNo�ڵ�
	while(nodeCur != NULL) {						//nodeCur�ڵ㲻�գ�DeviceNo�ڵ㲻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"DeviceNo")) {	//�ж��Ƿ�DeviceNo�ڵ�
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevSubN));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DevSubN:%s\n", pDev->DevSubN);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Name")) {
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevSname));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DevSname:%s\n", pDev->DevSname);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"ChnlNum")) {
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevChNum));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DeviceChnlNum:%s\n", pDev->DevChNum);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"FileFormat")) {
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevFileF));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DevFileF:%s\n", pDev->DevFileF);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"FileSavedPeriod")) {
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevFileP));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DevFileP:%s\n", pDev->DevFileP);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"FileMaxSize")) {
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevFileS));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DevFileS:%s\n", pDev->DevFileS);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"HarddiskCheckedPercentage")) {
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevHdisk));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DevHdisk:%s\n", pDev->DevHdisk);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"TimeZone")) {
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevTimeZ));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DevTimeZ:%s\n", pDev->DevTimeZ);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"HeartBeatPeriod")) {
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevBeatP));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DevBeatP:%s\n", pDev->DevBeatP);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"GpsInterface")) {
			strPar = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strPar, &(pDev->DevGpsIf));
			xmlFree(strPar);
			printf("TY-3012S xmlDoc DevGpsIf:%s\n", pDev->DevGpsIf);
		}
		nodeCur = nodeCur->next;
	}
	return 0;
}

static int HandleNodeDevAdd(xmlDocPtr xmlDOC, xmlNodePtr nodeAddr)
{
	xmlNodePtr	nodeCur;//�ڵ�ָ��
	xmlChar*	strAddr;//�ڵ�����

	nodeCur = nodeAddr->xmlChildrenNode;					//nodeCur�ڵ�ָ��ָ��LocIp�ڵ�
	while(nodeCur != NULL) {						//nodeCur�ڵ㲻�գ�LocIp�ڵ㲻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"LocIp")) {	//�ж��Ƿ�LocIp�ڵ�
			strAddr = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strAddr, &(pDev->DevLoIp));
			xmlFree(strAddr);
			printf("TY-3012S xmlDoc DevLoIp:%s\n", pDev->DevLoIp);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"SubnetMask")) {
			strAddr = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strAddr, &(pDev->DevSnMask));
			xmlFree(strAddr);
			printf("TY-3012S xmlDoc DevSnMask:%s\n", pDev->DevSnMask);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Gateway")) {
			strAddr = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strAddr, &(pDev->DevGway));
			xmlFree(strAddr);
			printf("TY-3012S xmlDoc DevGway:%s\n", pDev->DevGway);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"RemIp")) {
			strAddr = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strAddr, &(pDev->DevReIp));
			xmlFree(strAddr);
			printf("TY-3012S xmlDoc DeviceRemIp:%s\n", pDev->DevReIp);
		} else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"RemPort")) {
			strAddr = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strAddr, &(pDev->DevRePort));
			xmlFree(strAddr);
			printf("TY-3012S xmlDoc DeviceRemPort:%s\n", pDev->DevRePort);
		}
		nodeCur = nodeCur->next;
	}
	return 0;
}

static int HandleNodeDevice(xmlDocPtr xmlDOC, xmlNodePtr nodeDev)
{
	xmlNodePtr	Cur;	//�ڵ�ָ�룬�������ڵ�Device
	xmlNodePtr	nodeCur;//�ڵ�ָ�룬�����ӽڵ㣬Common/Address/Parameter/ChannelList
	xmlChar*	strCom;	//�ڵ�����
	xmlAttrPtr	attrPtr;//�ڵ�����ָ��
	xmlChar*	nAttr;	//�ڵ���������
	xmlChar*	vAttr;	//�ڵ���������
	int		retv;

	retv = 0;
	Cur = nodeDev;								//nodeDevָ��"Device"
	attrPtr = Cur->properties;						//attrPtrָ��Device������"name"
	while(attrPtr != NULL) {						//Device ������"name"����
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "name")) {		//Device �������Ƿ���"name"
			nAttr = xmlGetProp(Cur, BAD_CAST "name");		//nAttr  ָ��ָ��name����
			printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);		//nAttr  ����������ʾ��Ӧ����"name"
			vAttr = xmlGetProp(Cur, (const xmlChar *)"name");	//vAttr  ��������ָ��ָ��"name"���Ե�����"Dev1,Dev2,...,Dev16"
			CopyStr(vAttr, &(pDev->DevName));
			printf("TY-3012S xmlDoc DevName:%s!\n", pDev->DevName);
			xmlFree(vAttr);						//�ͷ�vAttr
		}
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "ip")) {			//Device �������Ƿ���"ip"
			nAttr = xmlGetProp(Cur, BAD_CAST "ip");			//nAttr  ָ��ָ��name����
			printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);		//nAttr  ����������ʾ��Ӧ����"ip"
			vAttr = xmlGetProp(Cur, (const xmlChar *)"ip");		//vAttr  ��������ָ��ָ��"ip"���Ե�����"11:22:33:44:55:66"
			CopyStr(vAttr, &(pDev->DevIp));
			printf("TY-3012S xmlDoc DevIp:%s!\n", pDev->DevIp);
			xmlFree(vAttr);						//�ͷ�vAttr
		}
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "no")) {			//Device �������Ƿ���"no"
			nAttr = xmlGetProp(Cur, BAD_CAST "no");			//nAttr  ָ��ָ��no����
			printf("TY-3012S xmlDoc nAttr:%s\n", nAttr);		//nAttr  ����������ʾ��Ӧ����"no"
			vAttr = xmlGetProp(Cur, (const xmlChar *)"no");		//vAttr  ��������ָ��ָ��"no"���Ե�����"1,2,...,16"
			CopyStr(vAttr, &(pDev->DevNo));
			printf("TY-3012S xmlDoc DevNo:%s!\n", pDev->DevNo);
			xmlFree(vAttr);						//�ͷ�vAttr			
		}
		attrPtr = attrPtr->next;					///attrPtrָ��ָ����һ������
	}
	/*add by haopeng*/
	Cur = Cur->xmlChildrenNode;
	/***********************/
	while(Cur != NULL) {							//nodeCur�ӽڵ㲻��
		DEBUG_PRINT("current node is :%s\n", Cur->name);
	
		if(!xmlStrcmp(Cur->name, (const xmlChar*)"Common")) {		//�ж��Ƿ�"Common"
			nodeCur = Cur->xmlChildrenNode;				//ָ����һ���ӽڵ�"version"
			while(nodeCur != NULL) {				//version�ӽڵ㲻��
				if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"version")) {
					strCom = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
					CopyStr(strCom, &(pDev->DevVer));
					xmlFree(strCom);
					printf("TY-3012S xmlDoc DevVer:%s!\n", pDev->DevVer);
				}
				nodeCur = nodeCur->next;
			}
		} 
		else if(!xmlStrcmp(Cur->name, (const xmlChar*)"Address")) {	//�ж��Ƿ�"Address"�ڵ�
			DEBUG_PRINT("find node Address\n");
			retv = HandleNodeDevAdd(xmlDOC, Cur);
		}
		else if(!xmlStrcmp(Cur->name, (const xmlChar*)"Parameter")) {	//�ж��Ƿ�"Parameter"�ڵ�
			retv = HandleNodePar(xmlDOC, Cur);
		}
//		else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"ChannelList")) {
		else if(!xmlStrcmp(Cur->name, (const xmlChar*)"ChannelList")) {
			retv = HandleNodeChannelList(xmlDOC, Cur);		//�ж��Ƿ�"ChannelList"�ڵ�
		}
		Cur = Cur->next;
	}
	return retv;
}

int xml_parseConfigXml(char* fileName)
{
	xmlNodePtr	Cur;				//��������ڵ�ָ�룬��Ҫָ��
	xmlDocPtr	xmlDOC;				//��������ļ�ָ�룬��Ҫָ��
	int		retv;
	xmlNodePtr testCur;
	
	retv = 0;
	xmlDOC = xmlParseFile(fileName);		//��xml�ļ����н�����ʹ��xmlParseFile����
	if(xmlDOC == NULL) {				//��xml�ļ����н������ɹ�
		printf("TY-3012S xmlDoc parse no successfully!\n");
		retv = -1;				//��xml�ļ����н������ɹ�
		return retv;
	}						//��xml�ļ����н����ɹ�
	printf("TY-3012S xmlDoc parse successfully!\n");
	Cur = xmlDocGetRootElement(xmlDOC);		//ȡ���ĵ���Ԫ�أ�ʹ��xmlDocGetRootElement����
	testCur = Cur;
	
	while(NULL != testCur)
	{
		DEBUG_PRINT("parse xmlFile node name is :%s\n", testCur->name);
		testCur = testCur->next;
	}
	
	if(Cur == NULL) {				//��һ����xml�ļ�
		printf("TY-3012S xmlDoc empty document!\n");
		xmlFreeDoc(xmlDOC);			//�ͷŽ�����ʹ���ڴ�ռ䣬ʹ��xmlFreeDoc����
		retv = -2;				//��һ����xml�ļ�
		return retv;
	}						//��һ��xml�ļ�
	printf("TY-3012S xmlDoc Cur->name: %s!\n", Cur->name);
	if(xmlStrcmp(Cur->name, BAD_CAST "SCL")) {	//�жϽڵ����֣�Ѱ��"SCL"��ʹ��xmlStrcmp����
		printf("TY-3012S xmlDoc root node no SCL!\n");
		xmlFreeDoc(xmlDOC);			//�ͷŽ�����ʹ���ڴ�ռ�
		retv = -3;				//��һ����SCL��xml�ļ�
		return retv;
	}						//��ʼ�������ò���
	pDev = (DeviceStr*)malloc(sizeof(DeviceStr));	//�����ļ������ݽṹ������洢�ռ�
	if(pDev == NULL) {				//�����ļ��ṹ����
		printf("TY-3012S xmlDoc out of memory!\n");
		xmlFreeDoc(xmlDOC);			//�ͷŽ�����ʹ���ڴ�ռ䣬ʹ��xmlFreeDoc����
		retv = -4;				//�����ڴ����
		return retv;
	}
	memset(pDev, 0, sizeof(DeviceStr));		//�����ļ������ݽṹ������Ĵ洢�ռ����
	Cur = Cur->xmlChildrenNode;			//ȡ��Cur�ĵ�һ���ӽ�㣬��������������ǰCurָ���ĵ��ĸ�
	while(Cur != NULL) {				//�ļ��ĵ�һ���ӽ�㲻�գ�ִ��
		printf("TY-3012S xmlDoc child node: %s!\n", Cur->name);
		if(!xmlStrcmp(Cur->name, (const xmlChar*)"Device")) {	//Ѱ��"Device"�����!
			//����Device�ڵ����ݣ�������������
			HandleNodeDevice(xmlDOC, Cur);	//"Device"�ڵ�
			break;				//�ж��ӽڵ�������"Device"�����д���
		}					//�ж��ӽڵ����ֲ���"Device"
		Cur = Cur->next;			//�ļ��ĵ�һ���ӽ���
	}
	xmlFreeDoc(xmlDOC);				//�ͷŽ�����ʹ���ڴ�ռ䣬ʹ��ʹ��xmlFreeDoc����
	xmlCleanupParser();				//�˳���xml�ļ����н����������Դ�ӿڣ�ʹ��xmlCleanupParser����
	return 0;					//ָ��ָ������������ݽṹ
}




void ___xmlNewFunctionCode_start___(){};


curFileInfo *xml_getCurFileInfoPtr(int chNo)
{
	return &curFile[chNo];
}

/*��ѯװ�������ļ��汾��*/
char *xml_getConfigVersion()
{
	return DevPar.DevVer;
}

#if 1
int xml_saveRecordFile(int chNo)
{
	#if 0
	if(pthread_mutex_lock(&xml_mutex) != 0)
	{                      
		printf("pthread_mutex_lock error\n", strerror(errno));    
		return -1;
	}                                                       
	#endif
	printf("chNo:%d, xml_saveRecordFile return\n", chNo);
	return 0;
	if (chNo == 0)
	{
		if (xmlSaveFormatFileEnc(XML_RECORD_FILE1A, xmlDiskPtr[0].doc, "UTF-8", 1) == -1)
		{
			printf("chNo:1,xmlSaveFormatFileEnc A failed\n");
		}
		if (xmlSaveFormatFileEnc(XML_RECORD_FILE1C, xmlDiskPtr[0].doc, "UTF-8", 1) == -1)
		{
			printf("chNo:1,xmlSaveFormatFileEnc B failed\n");
		}

	}
	else if(chNo == 1)
	{
		if (xmlSaveFormatFileEnc(XML_RECORD_FILE2A, xmlDiskPtr[1].doc, "UTF-8", 1) == -1)
		{
			printf("chNo:2,xmlSaveFormatFileEnc A failed\n");
		}
		
		if (xmlSaveFormatFileEnc(XML_RECORD_FILE2C, xmlDiskPtr[1].doc, "UTF-8", 1) == -1)
		{
			printf("chNo:2,xmlSaveFormatFileEnc B failed\n");
		}
	}
	else
	{
		DEBUG_PRINT("xml_saveRecordFile chNo error, chNo:%d, lineNo:%d\n", chNo, __LINE__);
		return  -1;
	}

	#if 0
	 if(pthread_mutex_unlock(&xml_mutex) != 0)
	 {                    
	 	printf("pthread_mutex_unlock error:%s\n", strerror(errno));
		return -1;
	 }                
	#endif
	return 0;
}
#endif

//û�ҵ������ļ������ڴ��е�Ĭ������
void xml_getDefaultConfig()
{

	return ;
}

//��ȡ�����д����ļ�����
int xml_getConfigDevFileF()
{
	return DevPar.DevFileF;
}

//��ȡ��¼�ļ��е�ǰ���̷�������Ϣ
char  xml_getCurrentDiskInfo(int chNo)
{
	if (pRec[chNo]->currentPos != NULL)
	{
		if (*(pRec[chNo]->currentPos+4) >= 'A' && *(pRec[chNo]->currentPos+4) <= 'F')
		{
			return *(pRec[chNo]->currentPos+4);
		}
	}
	
	return 0;
}

/*��ȡ�����ļ����ļ���С����*/
int xml_getConfigDevFileS()
{
	return DevPar.DevFileS;
}

//���ü�¼�ļ��е�ǰ���̷�������Ϣ
int xml_setCurrentDiskInfo(int chNo, char position)
{
	if (position < 'A' || position > 'F')
		return -1;
	
	*(pRec[chNo]->currentPos + 4) = position;

	//����Ҫ��xml��Դ�е��ֶθ���
	
	xmlNodeSetContent(xmlDiskPtr[chNo].curDisk, (const xmlChar *)(pRec[chNo]->currentPos));

	xml_saveRecordFile(chNo);
	return 0;
}

/*�������ļ������½���Ӧ��¼�����ú����*/
int xml_addNewNodeFile(int chNo, curFileInfo *fileInfo)
{
	xmlNodePtr file_node = NULL;
	int diskno;
	char no[4];

	memset(no, 0, sizeof(no));

	diskno = fileInfo->position[4] - 'A';
	sprintf(no, "%d", fileInfo->attrNo);

	//printf("chNo:%d, diskno:%d\n",chNo, diskno);
	file_node = xmlNewChild(xmlDiskPtr[chNo].diskx[diskno], NULL, BAD_CAST "File",NULL); 

	if (file_node == NULL)
	{
		printf("error ----%d\n", __LINE__);
		return -1;
	}
	
	xmlNewProp(file_node, BAD_CAST "no", BAD_CAST no); 
	xmlNewProp(file_node, BAD_CAST "startTime", BAD_CAST fileInfo->attrStartTm); 
	curFile[chNo].xmlnodePtr.endTime = xmlNewProp(file_node, BAD_CAST "endTime", BAD_CAST "0"); 
	
//	xmlNewChild(file_node, NULL, BAD_CAST "chNo",BAD_CAST "0"); 
	curFile[chNo].xmlnodePtr.status= xmlNewChild(file_node, NULL, BAD_CAST "status",BAD_CAST "0"); 
	xmlNewChild(file_node, NULL, BAD_CAST "position",BAD_CAST fileInfo->position); 
	xmlNewChild(file_node, NULL, BAD_CAST "path",BAD_CAST fileInfo->path); 
	xmlNewChild(file_node, NULL, BAD_CAST "realname",BAD_CAST "X1111234"); 
	xmlNewChild(file_node, NULL, BAD_CAST "filename",BAD_CAST fileInfo->filename); 
	curFile[chNo].xmlnodePtr.packCnt= xmlNewChild(file_node, NULL, BAD_CAST "packCnt",BAD_CAST "0"); 

	xmlNodeSetContent(xmlDiskPtr[chNo].amtx[diskno], (const xmlChar *)no);		//�����ļ���������
	
	return 0;
}

int xml_delDiskRecord(int chNo)
{
	int diskno;
	xmlNodePtr	Cur;	//�ڵ�ָ�룬�������ڵ�Device
	int 	retv;
	int 	i;

//	info = &curFile;
#if 1
	diskno = xml_getCurrentDiskInfo(chNo) - 'A';
	//diskno = (diskno + 1) % PATITION_NUMS;
	if (diskno < 0)
	{
		DEBUG_PRINT("diskno = 0, ---lineNo:%d\n", __LINE__);
		return -1;
	}
	
	retv = 0;
	Cur = xmlDiskPtr[chNo].diskx[diskno];								//nodeDevָ��"File"
	printf("xml_delDiskRecord, chNo:%d,position:%d\n", chNo, diskno);
	
	//DEBUG_PRINT("xml_delDiskRecord cur name is :%s, child name is :%s\n", Cur->name, Cur->xmlChildrenNode->name);
	if (Cur == NULL)
	{
		return -1;
	}
	
	Cur = Cur->xmlChildrenNode;
	
	while(Cur != NULL)
	{				
		//�ļ��ĵ�һ���ӽ�㲻�գ�ִ��
		//printf("xml_delDiskRecord xmlDoc child node: %s!\n", Cur->name);
		if (!xmlStrcmp(Cur->name, BAD_CAST "File"))
		{
			DEBUG_PRINT("find a File node to del, cur name :%s, ---lineNo:%d\n", Cur->name, __LINE__);
			xmlNodePtr tempNode;
			tempNode = Cur->next;
			/*���������Ҫ����,���߳�ͬʱ����,��ʱ���������ļ����̵ļ�¼����*/
			#if 0
			if(pthread_mutex_lock(&xml_mutex) != 0)
			{                      
				printf("xml_mutex lock error, ---lineNo:%d\n", __LINE__);    
			}                                                       
			#endif
			
			xmlUnlinkNode(Cur);
			xmlFreeNode(Cur);

			#if 0
			if(pthread_mutex_unlock(&xml_mutex) != 0)
			{                      
				printf("xml_mutex unlock error, ---lineNo:%d\n", __LINE__);    
			}     
			#endif
			
			Cur = tempNode;
			continue;
		}	
		Cur = Cur->next;			//�ļ��ĵ�һ���ӽ���
	}

	DEBUG_PRINT("find a File node to del, end, ---lineNo:%d\n", __LINE__);
	xmlNodeSetContent(xmlDiskPtr[chNo].amtx[diskno], (const xmlChar *)"0");
#endif
//	xmlUnlinkNode(xmlDiskPtr.diskx[diskno]);
//	xmlFreeNode(xmlDiskPtr.diskx[diskno]);
	return 0;
}

static int addNodeDiskToList(xmlNodePtr parent, int diskno, char* diskname, int chNo)
{
	int no;

	no = chNo - 1;
	if (parent == NULL || diskno < 0 || diskno > 6)
	{
		return -1;
	}
	
	xmlDiskPtr[no].diskx[diskno] = xmlNewChild(parent, NULL, BAD_CAST diskname,NULL); 
	if (xmlDiskPtr[no].diskx[diskno] == NULL)
	{
		printf("add node disk error --- %d \n", __LINE__);
		return -1;
	}
	xmlDiskPtr[no].amtx[diskno] = xmlNewChild(xmlDiskPtr[no].diskx[diskno], NULL, BAD_CAST "Amount",BAD_CAST "0"); 
	return 0;
}

#if 0
int xml_recreateDiskRecord(int chNo, curFileInfo *info)
{
	char c;
	char diskName[8];
	int diskno;

	diskno = info->position[4] - 'A';
	diskno = (diskno + 1) % PATITION_NUMS;

	c = 'A' + diskno;
	
	sprintf(diskName, "disk%c", c);

	DEBUG_PRINT("xml_recreateDiskRecord, chNo:%d, diskno:%d, diskName:%s, ---lineNo:%d\n", chNo, diskno, diskName, __LINE__);
	xmlDiskPtr.diskx[diskno] = addNodeDiskToList(xmlDiskPtr.diskList, diskno, diskName);
	
	return 0;
}
#endif

/*���̽�������״̬��Ϊ1����������size && packCnt*/
int xml_refreshNodeContent(int chNo)
{
	char cnt[32];
	memset(cnt, 0, sizeof(cnt));

	sprintf(cnt,"%ld", curFile[chNo].packCnt);

//	DEBUG_PRINT("xml_refreshNodeContent packCnt:%ld,cnt:%s, endTm:%s\n",curFile.packCnt, cnt, curFile.attrEndTm);
	xmlNodeSetContent(curFile[chNo].xmlnodePtr.status, (const xmlChar *)"1");
	xmlNodeSetContent(curFile[chNo].xmlnodePtr.packCnt, (const xmlChar *)cnt);
	xmlNodeSetContent(curFile[chNo].xmlnodePtr.endTime, (const xmlChar *)curFile[chNo].attrEndTm);
	/*xmlSetProp(FILE_node, "endTime",  (const xmlChar *)curFile.attrEndTm);*/
	return 0;
}

int xml_createNewRecord(int chNo)
{
//	xmlDocPtr doc = NULL;		/* document pointer */ 
	int no;
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;/* node pointers */ 
//	xmlNodePtr diskList = NULL;
	no = chNo - 1;
	// Creates a new document, a node and set it as a root node 
	xmlDiskPtr[no].doc = xmlNewDoc(BAD_CAST "1.0"); 
	xmlDiskPtr[no].doc->standalone = 0;
	root_node = xmlNewNode(NULL, BAD_CAST "RECORD");	//�������ڵ�RECORD
	xmlDocSetRootElement(xmlDiskPtr[no].doc, root_node);					//���øýڵ�Ϊ���ڵ�
	//creates a new node, which is "attached" as child node of root_node node. 

//	xmlDiskPtr.total = xmlNewChild(root_node, NULL, BAD_CAST "Amount",BAD_CAST "0"); 
//	xmlDiskPtr.curDisk = xmlNewChild(root_node, NULL, BAD_CAST "CurrentDisk",BAD_CAST "diskA"); 
	xmlNewChild(root_node, NULL, BAD_CAST "Amount",BAD_CAST "0"); 
	xmlNewChild(root_node, NULL, BAD_CAST "CurrentDisk",BAD_CAST "diskA"); 

	xmlDiskPtr[no].diskList= xmlNewNode(NULL, BAD_CAST "DiskList"); 
	xmlAddChild(root_node, xmlDiskPtr[no].diskList);

	addNodeDiskToList(xmlDiskPtr[no].diskList, 0, DISKA_STRING, chNo);
	addNodeDiskToList(xmlDiskPtr[no].diskList, 1, DISKB_STRING, chNo);
	addNodeDiskToList(xmlDiskPtr[no].diskList, 2, DISKC_STRING, chNo);
	addNodeDiskToList(xmlDiskPtr[no].diskList, 3, DISKD_STRING, chNo);
	addNodeDiskToList(xmlDiskPtr[no].diskList, 4, DISKE_STRING, chNo);		  
	addNodeDiskToList(xmlDiskPtr[no].diskList, 5, DISKF_STRING, chNo);

	xml_saveRecordFile(no);
	
	xmlFreeDoc(xmlDiskPtr[no].doc);				//�ͷŽ�����ʹ���ڴ�ռ䣬ʹ��ʹ��xmlFreeDoc����	
	xmlCleanupParser();				//�˳���xml�ļ����н����������Դ�ӿڣ�ʹ��xmlCleanupParser����
	xmlMemoryDump();

	return 0; 

}

static int HandleNodeCurrentDis(xmlDocPtr xmlDOC, xmlNodePtr nodeCuDis, int chNo)
{
	xmlNodePtr	Cur;	//�ڵ�ָ�룬�������ڵ�CurrentPos
	xmlChar*	strCom; //�ڵ�����
	int 	retv;

	Cur = nodeCuDis;
	
	strCom = xmlNodeListGetString(xmlDOC, Cur->xmlChildrenNode, 1);
	CopyStr(strCom, &(pRec[chNo]->currentPos));
	xmlFree(strCom);
	printf("TY-3012S xmlDoc pRec->currentPos:%s!\n", pRec[chNo]->currentPos);

	return retv;
}


static int HandleNodeAmount(xmlDocPtr xmlDOC, xmlNodePtr nodeAmount, int chNo)
{
	xmlNodePtr	Cur;	//�ڵ�ָ�룬�������ڵ�Device
	xmlChar*	strCom; //�ڵ�����
	int 	retv;
	
	retv = 0;
	Cur = nodeAmount;								//nodeDevָ��"Amount"

	pRec[chNo] = (pRecord *)malloc(sizeof(pRecord));	//�����ļ������ݽṹ������洢�ռ�
	if(pRec[chNo] == NULL) {				//�����ļ��ṹ����
		printf("TY-3012S xmlDoc out of memory!\n");
		xmlFreeDoc(xmlDOC); 		//�ͷŽ�����ʹ���ڴ�ռ䣬ʹ��xmlFreeDoc����
		retv = -4;				//�����ڴ����
		return retv;
	}
	memset(pRec[chNo], 0, sizeof(pRecord)); 	//�����ļ������ݽṹ������Ĵ洢�ռ����

	strCom = xmlNodeListGetString(xmlDOC, Cur->xmlChildrenNode, 1);
	CopyStr(strCom, &(pRec[chNo]->amount));
	xmlFree(strCom);
	printf("TY-3012S xmlDoc pRec->amount:%s!\n", pRec[chNo]->amount);
		
	return retv;
}

static int HandleNodeFile(xmlDocPtr xmlDOC, xmlNodePtr nodeFile, int disk, int chNo)
{
	xmlNodePtr	Cur;	//�ڵ�ָ�룬�������ڵ�Device
	xmlNodePtr	nodeCur;//�ڵ�ָ�룬�����ӽڵ㣬Common/Address/Parameter/ChannelList
	xmlChar*		strFile; //�ڵ�����
	xmlAttrPtr	attrPtr;//�ڵ�����ָ��
	xmlChar*		nAttr;	//�ڵ���������
	xmlChar*		vAttr;	//�ڵ���������
	int 			retv;
	int 			i, len;
	curFileInfo 	*info;

	info = xml_getCurFileInfoPtr(chNo);
	
	Cur = nodeFile;
	/*�Ƚ���File����*/
	attrPtr = Cur->properties;			//ָ������no

	while(attrPtr != NULL) {						//attrPtr����ָ�벻��
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "no")) {		//attrPtr����������"no"
			vAttr = xmlGetProp(Cur, (const xmlChar *)"no");	

			if (strcmp(pRec[chNo]->diskAmount[disk], vAttr) != 0)
			{
				//DEBUG_PRINT("not the last file node ,continue ---lineNo:%d\n", __LINE__);
				return -1;								//�������һ���ڵ�
			}

			info->attrNo = atol((char *)vAttr);
			//DEBUG_PRINT("TY-3012S xmlDocinfo->attrNo:%d, lineNo:%d\n", info->attrNo, __LINE__);
		}
		
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "startTime")) { 		
			vAttr = xmlGetProp(Cur, (const xmlChar *)"startTime");	
			strcpy(info->attrStartTm, (char *)vAttr);
			//DEBUG_PRINT("TY-3012S xmlDoc info->attrStartTm:%s,\n", info->attrStartTm);
		}
		
		if(!xmlStrcmp(attrPtr->name, BAD_CAST "endTime")) { 		//attrPtr����������"mac"
			vAttr = xmlGetProp(Cur, (const xmlChar *)"endTime");	//vAttr  ��������ָ��ָ��"mac"���Ե�����"11:22:33:44:55:66"
			strcpy(info->attrEndTm, (char *)vAttr);
			//DEBUG_PRINT("TY-3012S xmlDoc info->attrStartTm:%s,\n", info->attrEndTm);
		}
		attrPtr = attrPtr->next;					//attrPtr����ָ��ָ����һ�����ԣ�name->mac
	}


	nodeCur = Cur->xmlChildrenNode;					//nodeCur�ڵ�ָ��ָ��LocIp�ڵ�
	while(nodeCur != NULL) {						//nodeCur�ڵ㲻�գ�LocIp�ڵ㲻��
		#if 0
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"chNo")) 
		{	
			strFile = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strFile, &(pRec->record[disk]->chNo[count]));
			xmlFree(strFile);
			DEBUG_PRINT("TY-3012S xmlDoc pRec->record[%d]->chNo[%d]:%s\n", disk, count, pRec->record[disk]->chNo[count]);
		} 
		else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"size")) 
		{
			strFile = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			CopyStr(strFile, &(pRec->record[disk]->size[count]));
			xmlFree(strFile);
			DEBUG_PRINT("TY-3012S xmlDoc pRec->record[%d]->size[%d]:%s\n", disk, count, pRec->record[disk]->size[count]);
		} 
		#endif
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"status")) 
		{
			strFile = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			info->status = atol((char *)strFile);
			//DEBUG_PRINT("TY-3012S xmlDoc info->status:%d\n", info->status);
		}
		else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"position"))
		{
			strFile = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			strcpy(info->position, (char *)strFile);
			//DEBUG_PRINT("TY-3012S xmlDoc info->position:%s\n", info->position);
		} 
		else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"path")) 
		{
			strFile = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			strcpy(info->path, (char *)strFile);
			//DEBUG_PRINT("TY-3012S xmlDoc info->path:%s\n", info->path);
		}
		else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"filename")) 
		{
			strFile = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			strcpy(info->filename, (char *)strFile);
			//DEBUG_PRINT("TY-3012S xmlDoc info->filename:%s\n", info->filename);
		}
		else if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"packCnt")) 
		{
			strFile = xmlNodeListGetString(xmlDOC, nodeCur->xmlChildrenNode, 1);
			info->packCnt = atol((char *)strFile);
			//DEBUG_PRINT("TY-3012S xmlDoc info->packCnt:%ld\n", info->packCnt);
		}

		nodeCur = nodeCur->next;
	}
	return retv;
}

static int HandleNodeDiskAmount(xmlDocPtr xmlDOC, xmlNodePtr nodeAmount, int disk, int chNo)
{
	xmlNodePtr	Cur;	//�ڵ�ָ�룬�������ڵ�Device
	xmlChar*	strCom; //�ڵ�����
	int 	retv;
	
	retv = 0;
	Cur = nodeAmount;								//nodeDevָ��"Amount"


	strCom = xmlNodeListGetString(xmlDOC, Cur->xmlChildrenNode, 1);
	CopyStr(strCom, &(pRec[chNo]->diskAmount[disk]));
	xmlFree(strCom);
	DEBUG_PRINT("TY-3012S xmlDoc pRec->amount:%s! disk:%d  lineNo:%d\n", pRec[chNo]->diskAmount[disk], disk, __LINE__);

	return 0;
}


static int HandleNodeDisk(xmlDocPtr xmlDOC, xmlNodePtr nodeDisk, int disk, int chNo)
{
	xmlNodePtr	nodeCur;
	int 	retv, flag;
	char temp;

	retv = 0;

	if (disk < NODE_DISKA || disk > NODE_DISKF)
	{
		DEBUG_PRINT("disk value is error! handleNodeDisk failed!\n");
		return -1;
	}

	temp = xml_getCurrentDiskInfo(chNo);

	flag = ((temp == ('A'+disk))? 1 : 0);						//���Ƿ�Ϊ��ǰ���õķ���������ǣ�����file��
	
	nodeCur = nodeDisk->xmlChildrenNode;
	
	while(nodeCur != NULL)
	{		//nodeCur�ڵ�ָ�벻��
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"Amount"))
		{
			DEBUG_PRINT("find disk Amount node, lineNo:%d\n", __LINE__);
			xmlDiskPtr[chNo].amtx[disk] = nodeCur;
			HandleNodeDiskAmount(xmlDOC, nodeCur, disk, chNo);
		}

		if (flag == 0)
		{
			DEBUG_PRINT("currentDisk is :%c, node disk is :%d, no need to parse  ---lineNo:%d\n", temp, disk, __LINE__);
			break;
		}
		
		if(!xmlStrcmp(nodeCur->name, (const xmlChar*)"File"))
		{
			retv = HandleNodeFile(xmlDOC, nodeCur, disk, chNo);
		} 

		nodeCur = nodeCur->next;
	}
	
	return retv;
}


static int HandleNodeDiskList(xmlDocPtr xmlDOC, xmlNodePtr nodeDiscList, int chNo)
{
	xmlNodePtr	Cur;	//�ڵ�ָ�룬�������ڵ�Device
	xmlNodePtr	nodeCur;//�ڵ�ָ�룬�����ӽڵ㣬Common/Address/Parameter/ChannelList
	xmlChar*	strCom; //�ڵ�����
	xmlAttrPtr	attrPtr;//�ڵ�����ָ��
	xmlChar*	nAttr;	//�ڵ���������
	xmlChar*	vAttr;	//�ڵ���������
	int 	retv;
	int 	i;

	retv = 0;
	Cur = nodeDiscList;								//nodeDevָ��"DiskList"
	DEBUG_PRINT("disklist cur name is :%s, child name is :%s\n", Cur->name, Cur->xmlChildrenNode->name);

	
	Cur = Cur->xmlChildrenNode;
	while(Cur != NULL)
	{				
		//�ļ��ĵ�һ���ӽ�㲻�գ�ִ��
		printf("TY-3012S xmlDoc child node: %s!\n", Cur->name);
		if(!xmlStrcmp(Cur->name, (const xmlChar*)"DiskA"))
		{	//Ѱ��"Amount"�����!
			//����Amount�ڵ����ݣ�������������
			DEBUG_PRINT("find node DiskA!\n");
			xmlDiskPtr[chNo].diskx[0] = Cur;
			HandleNodeDisk(xmlDOC, Cur, NODE_DISKA, chNo);	//�ж��ӽڵ�������"DiskA"�����д���
		}

		if(!xmlStrcmp(Cur->name, (const xmlChar*)"DiskB")) 
		{	//Ѱ��"CurrentDisk"�����!
			//����Amount�ڵ����ݣ�������������
			DEBUG_PRINT("find node DiskB!\n");
			xmlDiskPtr[chNo].diskx[1] = Cur;
			HandleNodeDisk(xmlDOC, Cur, NODE_DISKB, chNo);	//�ж��ӽڵ�������"DiskB"�����д���
		}	

		if(!xmlStrcmp(Cur->name, (const xmlChar*)"DiskC")) 
		{	//Ѱ��"CurrentDisk"�����!
			//����Amount�ڵ����ݣ�������������
			DEBUG_PRINT("find node DiskC!\n");
			xmlDiskPtr[chNo].diskx[2] = Cur;
			HandleNodeDisk(xmlDOC, Cur, NODE_DISKC, chNo);	//�ж��ӽڵ�������"DiskC"�����д���
		}
		
		if(!xmlStrcmp(Cur->name, (const xmlChar*)"DiskD")) 
		{	//Ѱ��"CurrentDisk"�����!
			//����Amount�ڵ����ݣ�������������
			DEBUG_PRINT("find node DiskD!\n");
			xmlDiskPtr[chNo].diskx[3] = Cur;
			HandleNodeDisk(xmlDOC, Cur, NODE_DISKD, chNo);	//�ж��ӽڵ�������"DiskD"�����д���
		}	
		
		if(!xmlStrcmp(Cur->name, (const xmlChar*)"DiskE")) 
		{	//Ѱ��"CurrentDisk"�����!
			//����Amount�ڵ����ݣ�������������
			DEBUG_PRINT("find node DiskE!\n");
			xmlDiskPtr[chNo].diskx[4] = Cur;
			HandleNodeDisk(xmlDOC, Cur, NODE_DISKE, chNo);	//�ж��ӽڵ�������"DiskE"�����д���
		}
		
		if(!xmlStrcmp(Cur->name, (const xmlChar*)"DiskF")) 
		{	//Ѱ��"CurrentDisk"�����!
			//����Amount�ڵ����ݣ�������������
			DEBUG_PRINT("find node DiskF!\n");
			xmlDiskPtr[chNo].diskx[5] = Cur;
			HandleNodeDisk(xmlDOC, Cur, NODE_DISKF, chNo);	//�ж��ӽڵ�������"DiskF"�����д���
		}	
		
		Cur = Cur->next;			//�ļ��ĵ�һ���ӽ���
	
	}

	return retv;
}


int xml_parseRecordFile(int chNo, char *filename)
{
	xmlNodePtr	Cur;				//��������ڵ�ָ�룬��Ҫָ��
	xmlDocPtr	xmlDOC; 			//��������ļ�ָ�룬��Ҫָ��
	int 	retv;
	xmlNodePtr testCur;

	retv = 0;
	
	//xmlDiskPtr.doc = xmlParseFile(filename);		//��xml�ļ����н�����ʹ��xmlParseFile����
	xmlDiskPtr[chNo].doc = xmlReadFile(filename, NULL, XML_PARSE_NOBLANKS); 
	if(xmlDOC == NULL) {				//��xml�ļ����н������ɹ�
		DEBUG_PRINT("TY-3012S xmlDoc parse no successfully!\n");
		retv = -1;				//��xml�ļ����н������ɹ�
		return retv;
	}						//��xml�ļ����н����ɹ�
	DEBUG_PRINT("TY-3012S xmlDoc parse successfully!\n");
	Cur = xmlDocGetRootElement(xmlDiskPtr[chNo].doc); 	//ȡ���ĵ���Ԫ�أ�ʹ��xmlDocGetRootElement����
	testCur = Cur;
	
	while(NULL != testCur)
	{
		DEBUG_PRINT("parse xmlFile node name is :%s\n", testCur->name);
		testCur = testCur->next;
	}
	
	if(Cur == NULL) {				//��һ����xml�ļ�
		DEBUG_PRINT("TY-3012S xmlDoc empty document!\n");
		xmlFreeDoc(xmlDiskPtr[chNo].doc); 		//�ͷŽ�����ʹ���ڴ�ռ䣬ʹ��xmlFreeDoc����
		retv = -2;				//��һ����xml�ļ�
		return retv;
	}						//��һ��xml�ļ�
	printf("TY-3012S xmlDoc Cur->name: %s!\n", Cur->name);
	if(xmlStrcmp(Cur->name, BAD_CAST "RECORD")) {	//�жϽڵ����֣�Ѱ��"RECORD"��ʹ��xmlStrcmp����
		DEBUG_PRINT("TY-3012S xmlDoc root node no RECORD!\n");
		xmlFreeDoc(xmlDiskPtr[chNo].doc); 		//�ͷŽ�����ʹ���ڴ�ռ�
		retv = -3;				//��һ����SCL��xml�ļ�
		return retv;
	}						//��ʼ�������ò���

	Cur = Cur->xmlChildrenNode; 		//ȡ��Cur�ĵ�һ���ӽ�㣬��������������ǰCurָ���ĵ��ĸ�
	while(Cur != NULL) {				//�ļ��ĵ�һ���ӽ�㲻�գ�ִ��
		printf("TY-3012S xmlDoc child node: %s!\n", Cur->name);
		if(!xmlStrcmp(Cur->name, (const xmlChar*)"Amount")) {	//Ѱ��"Amount"�����!
			//����Amount�ڵ����ݣ�������������
			DEBUG_PRINT("find node Amount!\n");
			xmlDiskPtr[chNo].total = Cur;
			HandleNodeAmount(xmlDiskPtr[chNo].doc, Cur, chNo);	//�ж��ӽڵ�������"Amount"�����д���
		}
		
		if(!xmlStrcmp(Cur->name, (const xmlChar*)"CurrentDisk")) {	//Ѱ��"CurrentDisk"�����!
			//����Amount�ڵ����ݣ�������������
			DEBUG_PRINT("find node CurrentDisk!\n");
			xmlDiskPtr[chNo].curDisk = Cur;
			HandleNodeCurrentDis(xmlDiskPtr[chNo].doc, Cur, chNo);	//�ж��ӽڵ�������"CurrentDisk"�����д���				
		}	

		if(!xmlStrcmp(Cur->name, (const xmlChar*)"DiskList")) {	//Ѱ��"CurrentDisk"�����!
			//����Amount�ڵ����ݣ�������������
			DEBUG_PRINT("find node DiskList!\n");
			HandleNodeDiskList(xmlDiskPtr[chNo].doc, Cur, chNo);	//�ж��ӽڵ�������"CurrentDisk"�����д���				
		}	
		
		Cur = Cur->next;			//�ļ��ĵ�һ���ӽ���
	}

//	��¼�ļ���Դ���ͷţ����滹Ҫ��ζ��ļ����и��²���
//	xmlFreeDoc(xmlDiskPtr[chNo].doc); 			//�ͷŽ�����ʹ���ڴ�ռ䣬ʹ��ʹ��xmlFreeDoc����
//	xmlCleanupParser(); 			//�˳���xml�ļ����н����������Դ�ӿڣ�ʹ��xmlCleanupParser����

	return 0;					//ָ��ָ������������ݽṹ
}

extern int errno;


void xml_deviceConfigInit()
{
	int configFd;
	int err;

	err = errno;
	//����Ƿ���������ļ�,û�о���Ĭ�ϵģ��оͽ���
	if ((configFd = open(XML_CONFIGFILE, O_RDONLY)) == -1)
	{
		printf("can't open errno:%d, %s, ---lineNo:%d\n", errno, strerror(errno), __LINE__);
		if (errno == ENOENT)
		{
			errno = err;
			xml_getDefaultConfig();
		}
		else
		{
			exit(-1);			/*���ܽ����ļ�����,�˳�ϵͳ*/
		}
	}
	else
	{
		close(configFd);
		xml_parseConfigXml(XML_CONFIGFILE);
		//xml_printDeviceStr(1);
		xml_deviceStrToPar();
		//xml_printDevicePar(1);
		xml_freeDevice(1);
//		xml_setDevicePar(1);				//����Ӳ����ʹ������Ч,�ݲ�ʹ��
	}

	//as_idpromInit();							/*��̨ɨ��������س�ʼ��,�ݲ���*/
	return;

}
#if 0
/*���ݽ������������һ���ļ���������,Ŀǰͨ���Զ����ļ�βȷ��,�����������û��*/
int xml_checkRecordFile(int chNo)
{
	curFileInfo *info;
	char filePath[64];
	
	info = xml_getCurFileInfoPtr(chNo -1);
	if (info->attrNo == 0)
	{
		DEBUG_PRINT("info->attrNo : %d, ---lineNo:%d\n", info->attrNo, __LINE__);
		return 0;
	}
	memset(filePath, 0, sizeof(filePath));
	
	sprintf(filePath,"%s%s", info->path, info->filename);

	
	if (info->status == 0)
	{
		DEBUG_PRINT("xml_checkRecordFile, file not finish saving\n");
		//�ļ�����������������,�ҵ��Ǹ��ļ�д�ļ�β?
		//open
		//write
		//close
		return -1;
	}
	else
	{
		DEBUG_PRINT("xml_checkRecordFile, file finished \n");
		return 0;
	}
}
#endif

//������ͨ���ļ�¼�ļ��ֱ���г�ʼ��
void	xml_fileRecordInit()
{
	int recFd;
	
	/*xml��Ҫ����,��Ȼ��������������*/
	#if 0
	if (pthread_mutex_init(&xml_mutex,NULL) != 0)
	{
		printf("xml_mutex failed errno:%s\n", strerror(errno));
		exit(-1);
	}
	#endif
	/************************************************/

	/*����Ƿ���ڼ�¼,û�оʹ����µģ��оͽ���*/
	/*��һ��Ӳ��*/
	if ((recFd = open(XML_RECORD_FILE1A, O_RDONLY)) == -1)
	{
		#if 0
		printf("can't open errno:%d, %s\n", errno, strerror(errno));
		if (errno == -ENOENT)
		{
			errno = err;
		}
		#endif
		if ((recFd = open(XML_RECORD_FILE1C, O_RDONLY)) == -1)
		{
			DEBUG_PRINT("xml_createNewRecord, lineNo:%d\n", __LINE__);
			/*A,B�¶�û��,�ж�Ϊ�»���,�����¼�¼*/
			xml_createNewRecord(1);
			sleep(1);
			xml_parseRecordFile(0, XML_RECORD_FILE1A);
		}
		else
		{
			DEBUG_PRINT("xml_parseRecordFile, lineNo:%d\n", __LINE__);
			close(recFd);
			xml_parseRecordFile(0, XML_RECORD_FILE1C);
		}		
			//xml_checkRecordFile();	
	}
	else
	{
		DEBUG_PRINT("xml_parseRecordFile, lineNo:%d\n", __LINE__);
		close(recFd);
		if (xml_parseRecordFile(0, XML_RECORD_FILE1A) < 0)
		{
			/*������������*/
			if ((recFd = open(XML_RECORD_FILE1C, O_RDONLY)) == -1)
			{
				/*A�д����ļ�����������,C���ļ���ʧ,����������ᷢ��*/
				DEBUG_PRINT("fileRecord.xml lose, check disk to recovery, it will take a long time, please wait...");
				#if 0
				sleep(1);
				xml_parseRecordFile(1, XML_RECORD_FILEA);
				xml_checkRecordFile();
				#endif
			}
			else
			{
				DEBUG_PRINT("xml_parseRecordFile, lineNo:%d\n", __LINE__);
				/*Ӧ��������*/
				close(recFd);
				xml_parseRecordFile(0, XML_RECORD_FILE1C);
				//xml_checkRecordFile();
			}
			
		}
		//xml_checkRecordFile();
	}	

	#if 1	
	//�ڶ���Ӳ��
	if ((recFd = open(XML_RECORD_FILE2A, O_RDONLY)) == -1)
	{
		#if 0
		printf("can't open errno:%d, %s\n", errno, strerror(errno));
		if (errno == -ENOENT)
		{
			errno = err;
		}
		#endif
		if ((recFd = open(XML_RECORD_FILE2C, O_RDONLY)) == -1)
		{
			DEBUG_PRINT("xml_createNewRecord chNo:2, lineNo:%d\n", __LINE__);
			/*A,B�¶�û��,�ж�Ϊ�»���,�����¼�¼*/
			xml_createNewRecord(2);
			sleep(1);
			xml_parseRecordFile(1, XML_RECORD_FILE2A);
		}
		else
		{
			DEBUG_PRINT("xml_parseRecordFile, lineNo:%d\n", __LINE__);
			close(recFd);
			xml_parseRecordFile(1, XML_RECORD_FILE2C);
		}
		
			//xml_checkRecordFile();
	
	}
	else
	{
		DEBUG_PRINT("xml_parseRecordFile, lineNo:%d\n", __LINE__);
		close(recFd);
		if (xml_parseRecordFile(1, XML_RECORD_FILE2A) < 0)
		{
			/*������������*/
			if ((recFd = open(XML_RECORD_FILE2C, O_RDONLY)) == -1)
			{
				/*A�д����ļ�����������,C���ļ���ʧ,����������ᷢ��*/
				DEBUG_PRINT("fileRecord.xml lose, check disk to recovery, it will take a long time, please wait...");
				#if 0
				sleep(1);
				xml_parseRecordFile(1, XML_RECORD_FILEA);
				xml_checkRecordFile();
				#endif
			}
			else
			{
				DEBUG_PRINT("xml_parseRecordFile, lineNo:%d\n", __LINE__);
				/*Ӧ��������*/
				close(recFd);
				xml_parseRecordFile(1, XML_RECORD_FILE2C);
			}
			
		}
		//xml_checkRecordFile();					
		/*ԭ���ӵ��ļ������Լ��Ĺ���,����ͨ���ļ�β�ж�,�ⲿ�־Ͳ���Ҫ��*/
	}	
	
	errno =0;					/*�屨���־*/
	#endif	

	return;
}

void ___xmlNewFunctionCode_end___(){};


/*
�����򣬿�ʼxmlDoc��������ַ������Գ��򣬲������Գ���
*/
#if 0
int main(int argc)
{
	int retv;

	sleep(1);
	printf("\n");
	printf("********************************************************\n");
	printf("* Beijing Changtongda Conmunication Technology Co.,Ltd *\n");
	printf("*  40# Xueyuan Road, Haidian District, Beijing 100083  *\n");
	printf("*      TY-3012S xmlDoc Decode Program(version 0.0a)    *\n");
	printf("*                   www.ctdt.com.cn                    *\n");
	printf("*                  Tel (010)62301681                   *\n");
	printf("********************************************************\n");
	sleep(1);

	ParseRecordFile(recordFile);
	
	#if 0
	//��xmlDoc���н���
	printf("TY-3012S xmlDoc Decode start: %s\n", xmlFile);
	retv = ParseConfigXml(xmlFile);
	printf("TY-3012S xmlDoc Decode finish: %x\n", retv);
	//��ʾxmlDoc������
	printf("TY-3012S xmlDoc printDeviceStr start: %s\n", xmlFile);
	retv = printDeviceStr(1);
	printf("TY-3012S xmlDoc printDeviceStr finish: %x\n", retv);

	//���xmlDoc�������
	printf("TY-3012S xmlDoc StrToPar start: %s\n", xmlFile);
	retv = DeviceStrToPar();
	printf("TY-3012S xmlDoc StrToPar finish: %x\n", retv);
	//��ʾxmlDoc�������
//	printf("TY-3012S xmlDoc printDevicePar start: %s\n", xmlFile);
//	retv = printDevicePar(1);
//	printf("TY-3012S xmlDoc printDevicePar finish: %x\n", retv);
	#endif

	#if 0
	//����xmlDoc�������
	printf("TY-3012S xmlDoc printDevicePar start: %s\n", xmlFile);
	retv = setDevicePar(1);
	printf("TY-3012S xmlDoc printDevicePar finish: %x\n", retv);
	//�ͷ�xmlDocռ�õ��ڴ�
	printf("TY-3012S xmlDoc printDevicePar start: %s\n", xmlFile);
	retv = FreeDevice(1);
	printf("TY-3012S xmlDoc printDevicePar finish: %x\n", retv);
	#endif
	return 0;
}

#endif
