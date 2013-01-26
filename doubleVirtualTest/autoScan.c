/* 1��װ�õ�Xml�����ļ����ء����ء��汾��ѯ����
   2��װ�õ�IDPROM�����ļ������س���
   3��װ�õ�״̬��ѯ����
*/

#include <stddef.h>
#include <stdint.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <malloc.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <sys/stat.h>
#include "autoScan.h"
#include "backStage.h"

unsigned char	RomFileBuf[FACTORY_USER_LEN];	//����DeviceConfig_factory.rom�ļ���������һֱ����DeviceConfig_factory.rom�����ļ�
unsigned char	RomUserBuf[FACTORY_USER_LEN];	//����DeviceConfig_user.rom�ļ���������һֱ����DeviceConfig_user.rom�����ļ�
unsigned char	RomMsgBuff[MSG_ACK_LEN];	//������Ϣ������
unsigned char	RomAckBuff[MSG_ACK_LEN];	//����Ӧ�𻺳���
unsigned char	IPSETUP[] = "IP-SETUP";		//�����ַ���IP-SETUP

//static pthread_t rom_udp_thread;

void ConfigRomFileUdp(int RomSockfd)
{
	struct tm *ConfigRomFileUdpTime;
	time_t LocalTime;
	struct sockaddr_in my_udpserver_addr;
	int addr_len = sizeof(struct sockaddr_in);
	//int RomSockfd; 
	int RomFilefd, UserFilefd, UdpSoBroadcast;
	RomFactoryInfo		*ServerRomFactoryInfo;
	RomUserInfo		*ServerRomUserInfo;
	RomMsgAckHeader		*ServerRomMsgHeader,  *ServerRomAckHeader;
	RomDeviceConfInfo	*ServerRomDeviceConf, *ServerRomDeviceInfo;
	RomParamConfInfo	*ServerRomParamConf,  *ServerRomParamInfo;
	RomConfIp		*ServerRomConfIp;
	int MsgLen, i, j, ret;

	ServerRomFactoryInfo	= (RomFactoryInfo *)RomFileBuf;
	ServerRomUserInfo	= (RomUserInfo *)RomUserBuf;
	ServerRomMsgHeader	= (RomMsgAckHeader *)RomMsgBuff;
	ServerRomAckHeader	= (RomMsgAckHeader *)RomAckBuff;
	ServerRomDeviceConf	= (RomDeviceConfInfo *)RomMsgBuff;
	ServerRomDeviceInfo	= (RomDeviceConfInfo *)RomAckBuff;
	ServerRomParamConf	= (RomParamConfInfo *)RomMsgBuff;
	ServerRomParamInfo	= (RomParamConfInfo *)RomAckBuff;
	ServerRomConfIp		= (RomConfIp *)RomMsgBuff;
	//ȷ���ó��������ʱ��
	LocalTime = time(NULL);
	ConfigRomFileUdpTime = localtime(&LocalTime);
	//RomSockfd = *(int *)arg;
	#if 0
	//����RomSockfd
	RomSockfd = socket(AF_INET, SOCK_DGRAM, 0);//����һ��UDP��������ɨ�����
	if (RomSockfd < 0) {
		printf("TY3012S ConfigRomFile creates RomSockfd error!\n");
		exit(1);//ϵͳ���ش�����Ҫ��λ����!
		ret = -1;
	}
	
	printf("TY3012S ConfigRomFile creates RomSocket success: %d!\n", RomSockfd);
	UdpSoBroadcast = 1;
	setsockopt(RomSockfd, SOL_SOCKET, SO_BROADCAST, &UdpSoBroadcast, sizeof(UdpSoBroadcast));//�����͹㲥
	bzero(&my_udpserver_addr, sizeof(my_udpserver_addr));
	my_udpserver_addr.sin_family = AF_INET;
	my_udpserver_addr.sin_port = htons(UDP_PORT);//����idprom�ļ�!
	my_udpserver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(RomSockfd, (struct sockaddr *)&my_udpserver_addr, sizeof(my_udpserver_addr)) < 0) {
		printf("TY3012S ConfigRomFile binds RomSocket error!\n");
		exit(1);//ϵͳ���ش�����Ҫ��λ����!
		ret = -2;
	}
	#endif
	
	addr_len = sizeof(struct sockaddr_in);//��ȡ��ַ�ṹ�ĳ���
	printf("TY3012S ConfigRomFile binds RomSocket successful: %d!\n", my_udpserver_addr.sin_port);
//	while(1) {
		//������Ϣ
		memset(RomMsgBuff, 0, sizeof(RomMsgBuff));
		memset(RomAckBuff, 0, sizeof(RomAckBuff));
		MsgLen = recvfrom(RomSockfd, RomMsgBuff, sizeof(RomMsgBuff), 0, (struct sockaddr *)&my_udpserver_addr, &addr_len);//�ӿͻ��˽���
		if(MsgLen < 0) {
			printf("TY3012S ConfigRomFile recvfrom error!\n");
 			exit(1);//ϵͳ���ش���ʵ����Ҫ��λ����!
		}
		printf("TY3012S ConfigRomFile recvfrom OK, MsgLen: %d!\n", MsgLen);
		#if 1
		printf("TY3012S ConfigRomFile RomMsgBuff: %d", MsgLen);
		for(i = 0; i < MsgLen; i++) {
			if((i % 0x10) == 0) printf("!\n%02x:", i);
			printf(" %02x", RomMsgBuff[i]);
		}
		printf("!\n");
		#endif
		//����Ϣ��ͷ�����н���
		i = ServerRomMsgHeader->Header[0] + ServerRomMsgHeader->Header[1] + ServerRomMsgHeader->Header[2];
		if(i != 0) {//��Ϣ�Ķ�λ֡ͷ3�ֽڳ��ִ��󣬶�ȡ��һ������!
			printf("TY3012S ConfigRomFile decodes head error1: %d!\n", i);
		//	continue;
			return;
		}
		#if 0
		if(memcmp(ServerRomMsgHeader->Passwd, ServerRomFactoryInfo->passwd, sizeof(ServerRomFactoryInfo->passwd)) != 0) {
			//��Ϣ����֤����4�ֽڳ��ִ��󣬶�ȡ��һ������!
			printf("TY3012S ConfigRomFile decodes head error2:");
			for (i = 0; i < 4; i++) 
			{
				printf(" %02x", ServerRomMsgHeader->Passwd[i]);
			}
			printf("!\n");
			for (i = 0; i < 4; i++) 
			{
				printf(" %02x", ServerRomFactoryInfo->passwd[i]);
			}
			printf("!\n");
			continue;
		}
		#endif
		j = 0;
		for (i = 0; i < 6; i++) j = j + ServerRomMsgHeader->Mac[i];
		if((memcmp(ServerRomMsgHeader->Mac, ServerRomFactoryInfo->mac1, sizeof(ServerRomFactoryInfo->mac1)) != 0) && (j != 0)) {
			//��Ϣ��MAC��ַ6�ֽڳ��ִ��󣬶�ȡ��һ������!
			printf("TY3012S ConfigRomFile decodes head error3: %d:", j);
			for (i = 0; i < 6; i++) printf(" %02x", ServerRomMsgHeader->Mac[i]);
			printf("!\n");
			//continue;
			return;
		}
		if((ServerRomMsgHeader->Reserved[0] != 0) || (ServerRomMsgHeader->Reserved[1] != 0)) {
			//��Ϣ����λ�ֽڴ��󣬶�ȡ��һ������!
			printf("TY3012S ConfigRomFile decodes head error4: %02x,%02x!\n", ServerRomMsgHeader->Reserved[0], ServerRomMsgHeader->Reserved[1]);
			//continue;
			return;
		}
		//���յ��ĺϷ���Ϣ���д���
		switch(ServerRomMsgHeader->MsgID) {
			case MSG_DN_RESET:		//ǿ��ϵͳ���и�λ������������򣬸���Ϣ����Ҫ��Ӧ
				if((ServerRomMsgHeader->Parameter[0] == 0x33) && (ServerRomMsgHeader->Parameter[1] == 0x51)) {
					printf("TY3012S ConfigRomFile decode MSG_DN_RESET ok!\n");
					execl("/tools/reset", NULL);
				}
				printf("TY3012S ConfigRomFile decode MSG_DN_RESET error: %02x,%02x!\n", ServerRomMsgHeader->Parameter[0], ServerRomMsgHeader->Parameter[1]);
				break;
			case MSG_DN_IP_RESET:		//�ָ�װ�ó������ã�����Ϣ����Ҫ��Ӧ����ɺ�ϵͳ��λ
				printf("TY3012S ConfigRomFile decode MSG_DN_IP_RESET ok!\n");
				memcpy(ServerRomUserInfo->name,      ServerRomFactoryInfo->name,      sizeof(ServerRomUserInfo->name));
				memcpy(ServerRomUserInfo->type,      ServerRomFactoryInfo->type,      sizeof(ServerRomUserInfo->type));
				memcpy(ServerRomUserInfo->passwd,    ServerRomFactoryInfo->passwd,    sizeof(ServerRomUserInfo->passwd));
				memcpy(ServerRomUserInfo->ip1,       ServerRomFactoryInfo->ip1,       sizeof(ServerRomUserInfo->ip1));
				memcpy(ServerRomUserInfo->ip2,       ServerRomFactoryInfo->ip2,       sizeof(ServerRomUserInfo->ip2));
				memcpy(ServerRomUserInfo->mask1,     ServerRomFactoryInfo->mask1,     sizeof(ServerRomUserInfo->mask1));
				memcpy(ServerRomUserInfo->mask2,     ServerRomFactoryInfo->mask2,     sizeof(ServerRomUserInfo->mask2));
				memcpy(ServerRomUserInfo->gateway1,  ServerRomFactoryInfo->gateway1,  sizeof(ServerRomUserInfo->gateway1));
				memcpy(ServerRomUserInfo->gateway2,  ServerRomFactoryInfo->gateway2,  sizeof(ServerRomUserInfo->gateway2));
				memcpy(ServerRomUserInfo->note,      ServerRomFactoryInfo->note,      sizeof(ServerRomUserInfo->note));
				memcpy(ServerRomUserInfo->reserved1, ServerRomFactoryInfo->reserved1, sizeof(ServerRomUserInfo->reserved1));
				memset(ServerRomUserInfo->reserved2, 0, sizeof(ServerRomUserInfo->reserved2));
				memset(ServerRomUserInfo->crc,       0, sizeof(ServerRomUserInfo->crc));
				//��DeviceConfig_user.rom�����ļ�д������ļ�DeviceConfig_user.rom
				if(remove("DeviceConfig_user.rom") == -1) {
					printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom error1!\n");
					//ɾ���ļ����ִ��󣬼�������!
				} else {
					printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom sucess1:\n");
				}
				UserFilefd = open("DeviceConfig_user.rom", O_RDWR | O_CREAT, 00700);
				if(UserFilefd == -1) {
					close(UserFilefd);	//���ļ����ִ��󣬼�������!
					printf("TY3012S ConfigRomFile open DeviceConfig_user.rom error1: %d!\n", UserFilefd);
					break;
				}
				i = write(UserFilefd, RomUserBuf, FACTORY_USER_LEN);
				if(i != FACTORY_USER_LEN) {
					close(UserFilefd);	//д���ļ����ִ��󣬼�������!
					printf("TY3012S ConfigRomFile write DeviceConfig_user.rom error1: %d!\n", i);
					break;
				}
				close(UserFilefd);
				printf("TY3012S ConfigRomFile write DeviceConfig_user.rom sucess1: %d!\n\n", i);
				execl("/tools/reset", NULL);
				break;
			case MSG_QUERY_DN: //��ѯװ�õ�DN��Ϣ������Ϣ����Ҫ��������ҪӦ��
				ServerRomDeviceInfo->MsgID = MSG_QUERY_DN_ACK;
				//������Ϣ��ͷ����Ϣ
				memset(ServerRomDeviceInfo->Header,	0,    				sizeof(ServerRomDeviceInfo->Header));
				memcpy(ServerRomDeviceInfo->Passwd,	ServerRomUserInfo->passwd,	sizeof(ServerRomDeviceInfo->Passwd));
				memcpy(ServerRomDeviceInfo->Mac,	ServerRomFactoryInfo->mac1, 	sizeof(ServerRomDeviceInfo->Mac));
				memset(ServerRomDeviceInfo->Reserved,	0, 				sizeof(ServerRomDeviceInfo->Reserved));
				//������Ϣ�Ĳ�����Ϣ
				memcpy(ServerRomDeviceInfo->name,	ServerRomUserInfo->name,    	sizeof(ServerRomDeviceInfo->name));
				memcpy(ServerRomDeviceInfo->type,	ServerRomUserInfo->type,	sizeof(ServerRomDeviceInfo->type));
				memcpy(ServerRomDeviceInfo->mac,	ServerRomFactoryInfo->mac1, 	sizeof(ServerRomDeviceInfo->mac));
				memset(ServerRomDeviceInfo->reserved0,	0,				sizeof(ServerRomDeviceInfo->ip1));
				memcpy(ServerRomDeviceInfo->ip1,	ServerRomUserInfo->ip1,		sizeof(ServerRomDeviceInfo->ip1));
				memcpy(ServerRomDeviceInfo->mask1,	ServerRomUserInfo->mask1,	sizeof(ServerRomDeviceInfo->mask1));
				memcpy(ServerRomDeviceInfo->gateway1,	ServerRomUserInfo->gateway1,	sizeof(ServerRomDeviceInfo->gateway1));
				memcpy(ServerRomDeviceInfo->ip2,	ServerRomUserInfo->ip2,		sizeof(ServerRomDeviceInfo->ip2));
				memcpy(ServerRomDeviceInfo->mask2,	ServerRomUserInfo->mask2,	sizeof(ServerRomDeviceInfo->mask2));
				memcpy(ServerRomDeviceInfo->gateway2,	ServerRomUserInfo->gateway2,	sizeof(ServerRomDeviceInfo->gateway2));
				memcpy(ServerRomDeviceInfo->note,	ServerRomUserInfo->note,	sizeof(ServerRomDeviceInfo->note));
				memcpy(ServerRomDeviceInfo->reserved2,	ServerRomUserInfo->reserved1,	sizeof(ServerRomDeviceInfo->reserved2));	
				//������Ϣ��ʱ����Ϣ
				ServerRomDeviceInfo->yearh	= ConfigRomFileUdpTime->tm_year >> 8;
				ServerRomDeviceInfo->yearl	= ConfigRomFileUdpTime->tm_year & 0x00ff;
				ServerRomDeviceInfo->mouth	= ConfigRomFileUdpTime->tm_mon;
				ServerRomDeviceInfo->date	= ConfigRomFileUdpTime->tm_mday;
				ServerRomDeviceInfo->hours	= ConfigRomFileUdpTime->tm_hour;
				ServerRomDeviceInfo->minutes	= ConfigRomFileUdpTime->tm_min;
				ServerRomDeviceInfo->seconds	= ConfigRomFileUdpTime->tm_sec;
				ServerRomDeviceInfo->reserved1	= 0;
				//������Ϣ�ķ��͵�ַΪ�㲥��ַ����ʼ��ַmy_udpserver_addr.sin_addr.s_addr = htonl(INADDR_ANY);

				/*��255.255.255.255���ͱ�����ʾ��ַ���ɴ�,����Դ��ַ�ظ�,haopeng ---2012.11.21*/
				//my_udpserver_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
				//����Ϣ���ͳ�ȥ
				MsgLen = sendto(RomSockfd, RomAckBuff, sizeof(RomDeviceConfInfo), 0, (struct sockaddr*)&my_udpserver_addr, addr_len);	//���͵�������
				if(MsgLen < 0) {
					
					printf("TY3012S ConfigRomFile sendto command error1!:%s\n", strerror(errno));
				} else {
					printf("TY3012S ConfigRomFile sendto command success1!\n");
				}
				break;
			case MSG_QUERY_CFG_INFO: //��ѯװ��������Ϣ������Ϣ����Ҫ��������ҪӦ��
				ServerRomDeviceInfo->MsgID = MSG_QUERY_CFG_INFO_ACK;
				//������Ϣ��ͷ����Ϣ
				memset(ServerRomParamInfo->Header,	0,    				sizeof(ServerRomParamInfo->Header));
				memcpy(ServerRomParamInfo->Passwd,	ServerRomUserInfo->passwd,	sizeof(ServerRomParamInfo->Passwd));
				memcpy(ServerRomParamInfo->Mac,		ServerRomFactoryInfo->mac1, 	sizeof(ServerRomParamInfo->Mac));
				memset(ServerRomParamInfo->Reserved,	0, 				sizeof(ServerRomParamInfo->Reserved));
				//������Ϣ�Ĳ�����Ϣ
				memcpy(ServerRomParamInfo->name,	ServerRomUserInfo->name,    	sizeof(ServerRomParamInfo->name));
				memcpy(ServerRomParamInfo->type,	ServerRomUserInfo->type,	sizeof(ServerRomParamInfo->type));
				memcpy(ServerRomParamInfo->note,	ServerRomUserInfo->note,	sizeof(ServerRomParamInfo->note));
				memcpy(ServerRomParamInfo->reserved,	ServerRomUserInfo->reserved1,	sizeof(ServerRomParamInfo->reserved));	
				//������Ϣ�ķ��͵�ַΪ�㲥��ַ����ʼ��ַmy_udpserver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
				my_udpserver_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
				//����Ϣ���ͳ�ȥ
				MsgLen = sendto(RomSockfd, RomAckBuff, sizeof(RomParamConfInfo), 0, (struct sockaddr*)&my_udpserver_addr, addr_len);	//���͵�������
				if(MsgLen < 0) {
					printf("TY3012S ConfigRomFile sendto command error2!\n");
				} else {
					printf("TY3012S ConfigRomFile sendto command success2!\n");
				}
				break;
			case MSG_PARAM_CFG: //��������
				memcpy(ServerRomUserInfo->name,		ServerRomParamConf->name,	sizeof(ServerRomParamConf->name));
				memcpy(ServerRomUserInfo->type,		ServerRomParamConf->type,	sizeof(ServerRomParamConf->type));
				memcpy(ServerRomUserInfo->note,		ServerRomParamConf->note,	sizeof(ServerRomParamConf->note));
				memcpy(ServerRomUserInfo->reserved1,	ServerRomParamConf->reserved,	sizeof(ServerRomParamConf->reserved));
				//��DeviceConfig_user.rom�����ļ�д������ļ�DeviceConfig_user.rom
				if(remove("DeviceConfig_user.rom") == -1) {
					printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom error2!\n");
					//ɾ���ļ����ִ��󣬼�������!
				} else {
					printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom sucess2!\n");
				}
				UserFilefd = open("DeviceConfig_user.rom", O_RDWR | O_CREAT, 00700);
				if(UserFilefd == -1) {
					close(UserFilefd);	//���ļ����ִ��󣬼�������!
					printf("TY3012S ConfigRomFile open DeviceConfig_user.rom error2: %d!\n", UserFilefd);
					break;
				}
				i = write(UserFilefd, RomUserBuf, FACTORY_USER_LEN);
				if(i != FACTORY_USER_LEN) {
					close(UserFilefd);	//д���ļ����ִ��󣬼�������!
					printf("TY3012S ConfigRomFile write DeviceConfig_user.rom error2: %d!\n", i);
					break;
				}
				close(UserFilefd);
				printf("TY3012S ConfigRomFile write DeviceConfig_user.rom sucess2: %d!\n\n", i);
				ServerRomAckHeader->MsgID = MSG_PARAM_CFG_ACK;
				//������Ϣ��ͷ����Ϣ
				memset(ServerRomAckHeader->Header,	0,    				sizeof(ServerRomAckHeader->Header));
				memcpy(ServerRomAckHeader->Passwd,	ServerRomUserInfo->passwd,	sizeof(ServerRomAckHeader->Passwd));
				memcpy(ServerRomAckHeader->Mac,		ServerRomFactoryInfo->mac1, 	sizeof(ServerRomAckHeader->Mac));
				memset(ServerRomAckHeader->Reserved,	0,			 	sizeof(ServerRomAckHeader->Reserved));
				//������Ϣ�ķ��͵�ַΪ�㲥��ַ����ʼ��ַmy_udpserver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
				my_udpserver_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
				//����Ϣ���ͳ�ȥ
				MsgLen = sendto(RomSockfd, RomAckBuff, sizeof(RomMsgAckHeader), 0, (struct sockaddr*)&my_udpserver_addr, addr_len);	//���͵�������
				if(MsgLen < 0) {
					printf("TY3012S ConfigRomFile sendto command error3!\n");
				} else {
					printf("TY3012S ConfigRomFile sendto command success3!\n");
				}
				break;
			case MSG_SET_IP://����IP��ַ
				if((memcmp(ServerRomMsgHeader->Mac, ServerRomFactoryInfo->mac1, sizeof(ServerRomMsgHeader->Mac)) != 0)) 
				{
					//��Ϣ��MAC��ַ6�ֽڳ��ִ��󣬶�ȡ��һ������!
					printf("TY3012S ConfigRomFile decodes head error5: %d:");
					for (i = 0; i < 6; i++) printf(" %02x", ServerRomMsgHeader->Mac[i]);
					printf("!\n");
					//continue;
					return;
				}
				if((memcmp(ServerRomConfIp->ipsetup, IPSETUP, sizeof(ServerRomConfIp->ipsetup)) != 0)) 
				{
					//��Ϣ��IPSETUP�ֶγ��ִ��󣬶�ȡ��һ������!
					printf("TY3012S ConfigRomFile decodes message error1: %d:");
					for (i = 0; i < 6; i++) printf(" %02x", ServerRomConfIp->ipsetup[i]);
					printf("!\n");
					//continue;
					return;
				}
				if((ServerRomConfIp->signal == 0x01) || (ServerRomConfIp->signal == 0x03)) {
					memcpy(ServerRomUserInfo->ip1,      ServerRomConfIp->ip1,      sizeof(ServerRomConfIp->ip1));
					memcpy(ServerRomUserInfo->mask1,    ServerRomConfIp->mask1,    sizeof(ServerRomConfIp->mask1));
					memcpy(ServerRomUserInfo->gateway1, ServerRomConfIp->gateway1, sizeof(ServerRomConfIp->gateway1));
				}
				if((ServerRomConfIp->signal == 0x02) || (ServerRomConfIp->signal == 0x03)) {
					memcpy(ServerRomUserInfo->ip2,      ServerRomConfIp->ip2,      sizeof(ServerRomConfIp->ip2));
					memcpy(ServerRomUserInfo->mask2,    ServerRomConfIp->mask2,    sizeof(ServerRomConfIp->mask2));
					memcpy(ServerRomUserInfo->gateway2, ServerRomConfIp->gateway2, sizeof(ServerRomConfIp->gateway2));
				}
				//��DeviceConfig_user.rom�����ļ�д������ļ�DeviceConfig_user.rom
				if(remove("DeviceConfig_user.rom") == -1) {
					printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom error3!\n");
					//ɾ���ļ����ִ��󣬼�������!
				} else {
					printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom sucess3!\n");
				}
				UserFilefd = open("DeviceConfig_user.rom", O_RDWR | O_CREAT, 00700);
				if(UserFilefd == -1) {
					close(UserFilefd);	//���ļ����ִ��󣬼�������!
					printf("TY3012S ConfigRomFile open DeviceConfig_user.rom error3: %d!\n", UserFilefd);
					break;
				}
				i = write(UserFilefd, RomUserBuf, FACTORY_USER_LEN);
				if(i != FACTORY_USER_LEN) {
					close(UserFilefd);	//д���ļ����ִ��󣬼�������!
					printf("TY3012S ConfigRomFile write DeviceConfig_user.rom error3: %d!\n", i);
					break;
				}
				close(UserFilefd);
				printf("TY3012S ConfigRomFile write DeviceConfig_user.rom sucess3:  %d!\n\n", i);
				execl("/tools/reset", NULL);
				break;
			case MSG_SET_IP_AND_PARAM://����IP��ַ�Ͳ���
				if((memcmp(ServerRomMsgHeader->Mac, ServerRomFactoryInfo->mac1, sizeof(ServerRomMsgHeader->Mac)) != 0)) 
				{
					//��Ϣ��MAC ��ַ6�ֽڳ��ִ��󣬶�ȡ��һ������!
					printf("TY3012S ConfigRomFile decodes head error6: %d:");
					for (i = 0; i < 6; i++) printf(" %02x", ServerRomDeviceConf->Mac[i]);
					printf("!\n");
					//continue;
					return;
				}
				memcpy(ServerRomUserInfo->name,      ServerRomDeviceConf->name,     sizeof(ServerRomDeviceConf->name));
				memcpy(ServerRomUserInfo->type,      ServerRomDeviceConf->type,     sizeof(ServerRomDeviceConf->type));
				memcpy(ServerRomUserInfo->ip1,       ServerRomDeviceConf->ip1,      sizeof(ServerRomDeviceConf->ip1));
				memcpy(ServerRomUserInfo->ip2,       ServerRomDeviceConf->ip2,      sizeof(ServerRomDeviceConf->ip2));
				memcpy(ServerRomUserInfo->mask1,     ServerRomDeviceConf->mask1,    sizeof(ServerRomDeviceConf->mask1));
				memcpy(ServerRomUserInfo->mask2,     ServerRomDeviceConf->mask2,    sizeof(ServerRomDeviceConf->mask2));
				memcpy(ServerRomUserInfo->gateway1,  ServerRomDeviceConf->gateway1, sizeof(ServerRomDeviceConf->gateway1));
				memcpy(ServerRomUserInfo->gateway2,  ServerRomDeviceConf->gateway2, sizeof(ServerRomDeviceConf->gateway2));
				memcpy(ServerRomUserInfo->note,      ServerRomDeviceConf->note,	    sizeof(ServerRomDeviceConf->note));
				memcpy(ServerRomUserInfo->reserved1, ServerRomDeviceConf->reserved2,sizeof(ServerRomDeviceConf->reserved2));
				//��DeviceConfig_user.rom�����ļ�д������ļ�DeviceConfig_user.rom
				if(remove("DeviceConfig_user.rom") == -1) {
					printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom error4:\n");
					//ɾ���ļ����ִ��󣬼�������!
				} else {
					printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom sucess4:\n");
				}
				UserFilefd = open("DeviceConfig_user.rom", O_RDWR | O_CREAT, 00700);
				if(UserFilefd == -1) {
					close(UserFilefd);	//���ļ����ִ��󣬼�������!
					printf("TY3012S ConfigRomFile open DeviceConfig_user.rom error4: %d!\n", UserFilefd);
					break;
				}
				i = write(UserFilefd, RomUserBuf, FACTORY_USER_LEN);
				if(i != FACTORY_USER_LEN) {
					close(UserFilefd);	//д���ļ����ִ��󣬼�������!
					printf("TY3012S ConfigRomFile write DeviceConfig_user.rom error4: %d!\n", i);
					break;
				}
				close(UserFilefd);
				printf("TY3012S ConfigRomFile write DeviceConfig_user.rom sucess4: %d!\n\n", i);
				execl("/tools/reset", NULL);
				break;
			default:
				printf("TY3012S ConfigRomFile decodes command error1: %d!\n", ServerRomMsgHeader->MsgID);
				break;
		}
//	}
	
	return;
}

static unsigned short crc16_left(unsigned char crcbyte, unsigned short crc)
{
	unsigned char i;

	for(i = 0x80; i != 0; i >>= 1) {
		if((crc & 0x8000) != 0) {
			crc <<= 1;
			crc ^= 0x1021;
		} else {
			crc <<= 1;
		}
		if((crcbyte & i) != 0) {
			crc ^= 0x1021;
		}
	}
	return crc;
}

static int idprom_factory_set(int cmd)
{
	//----------------0000000001111111111222222222233333333334444444444500000000066666
	//----------------1234567890123456789012345678901234567890123456789012345678901234
	char DevName[] = "Testing System1 ";
	char DevType[] = "TY-3012S-000001 ";
	char DevSn[]   = "Ctdt 0123456789 ";
	char DevPass[] = "ctdt";
	char DevMac1[] = {0x00, 0x80, 0xc8, 0xe1, 0xcf, 0x5d};
	char DevMac2[] = {0x00, 0x80, 0xc8, 0xe1, 0xcf, 0x5e};
	char DevNip1[] = {192, 168, 000, 100};
	char DevNip2[] = {192, 168, 001, 100};
	char DevMsk1[] = {255, 255, 255, 000};
	char DevMsk2[] = {255, 255, 255, 000};
	char DevGat1[] = {192, 168, 000, 001};
	char DevGat2[] = {192, 168, 001, 001};
	char DevHard[] = "010a";
	char DevSoft[] = "010a";
	char DevDate[] = "2010.10.20";
	char DevNote[] = "http://www.dxyb.com 10-62301187!";
	char DevRes1[] = "www.ctdt.com.cn www.ctdt.com.cn!";
	char DevRes2[] = "Testing System for Communication Network of Digital Substation! ";
	//----------------0000000001111111111222222222233333333334444444444500000000066666
	//----------------1234567890123456789012345678901234567890123456789012345678901234
	RomFactoryInfo *ServerRomFactoryInfo;
	int RomFilefd, FileLen, i;
	unsigned short factory_crc;

	if(cmd == 0) return -1;	//�ж��Ƿ���Ҫ��ʼ��DeviceConfig_factory.rom�ļ�����һ����Ҫ
	printf("TY3012S ConfigRomFile set DeviceConfig_factory.rom start!\n");
	ServerRomFactoryInfo = (RomFactoryInfo *)RomFileBuf;
	memset(RomFileBuf, 0, sizeof(RomFileBuf));	//���RomFileBuf
	memset(ServerRomFactoryInfo->flag,	0,	 sizeof(ServerRomFactoryInfo->flag));
	memcpy(ServerRomFactoryInfo->name,	DevName, sizeof(ServerRomFactoryInfo->name));
	memcpy(ServerRomFactoryInfo->type,	DevType, sizeof(ServerRomFactoryInfo->type));
	memcpy(ServerRomFactoryInfo->sn,	DevSn,   sizeof(ServerRomFactoryInfo->sn));
	memcpy(ServerRomFactoryInfo->passwd,	DevPass, sizeof(ServerRomFactoryInfo->passwd));
	memcpy(ServerRomFactoryInfo->mac1,	DevMac1, sizeof(ServerRomFactoryInfo->mac1));
	memcpy(ServerRomFactoryInfo->mac2,	DevMac2, sizeof(ServerRomFactoryInfo->mac2));
	memcpy(ServerRomFactoryInfo->ip1,	DevMac1, sizeof(ServerRomFactoryInfo->ip1));
	memcpy(ServerRomFactoryInfo->ip2,	DevMac2, sizeof(ServerRomFactoryInfo->ip2));
	memcpy(ServerRomFactoryInfo->mask1,	DevMsk1, sizeof(ServerRomFactoryInfo->mask1));
	memcpy(ServerRomFactoryInfo->mask2,	DevMsk2, sizeof(ServerRomFactoryInfo->mask2));
	memcpy(ServerRomFactoryInfo->gateway1,	DevGat1, sizeof(ServerRomFactoryInfo->gateway1));
	memcpy(ServerRomFactoryInfo->gateway2,	DevGat2, sizeof(ServerRomFactoryInfo->gateway2));
	ServerRomFactoryInfo->udp_port1[0] = 0x27;  ServerRomFactoryInfo->udp_port1[1] = 0x11;
	ServerRomFactoryInfo->udp_port2[0] = 0x77;  ServerRomFactoryInfo->udp_port2[1] = 0xee;
	ServerRomFactoryInfo->tcp_port1[0] = 0xd0;  ServerRomFactoryInfo->tcp_port1[1] = 0xd0;
	ServerRomFactoryInfo->tcp_port2[0] = 0xd0;  ServerRomFactoryInfo->tcp_port2[1] = 0xd1;
	ServerRomFactoryInfo->tcp_port3[0] = 0xd0;  ServerRomFactoryInfo->tcp_port3[1] = 0xd2;
	ServerRomFactoryInfo->tcp_port4[0] = 0xd0;  ServerRomFactoryInfo->tcp_port4[1] = 0xd3;
	ServerRomFactoryInfo->tcp_port5[0] = 0xd0;  ServerRomFactoryInfo->tcp_port5[1] = 0xd4;
	ServerRomFactoryInfo->tcp_port6[0] = 0xd0;  ServerRomFactoryInfo->tcp_port6[1] = 0xd5;
	memcpy(ServerRomFactoryInfo->hardware,	DevHard, sizeof(ServerRomFactoryInfo->hardware));
	memcpy(ServerRomFactoryInfo->software,	DevSoft, sizeof(ServerRomFactoryInfo->software));
	memcpy(ServerRomFactoryInfo->date,	DevDate, sizeof(ServerRomFactoryInfo->date));
	memcpy(ServerRomFactoryInfo->note,	DevNote, sizeof(ServerRomFactoryInfo->note));
	memcpy(ServerRomFactoryInfo->reserved1,	DevRes1, sizeof(ServerRomFactoryInfo->reserved1));
	memcpy(ServerRomFactoryInfo->reserved2,	DevRes2, sizeof(ServerRomFactoryInfo->reserved2));
	for (i = 0; i < (FACTORY_USER_LEN - 2); i++) {	//����RomFileBuf��CRC-16ֵ
		factory_crc = crc16_left(RomFileBuf[i], factory_crc);
	}
	printf("TY3012S ConfigRomFile RomFactoryBuf CRC:!\n", factory_crc);
	ServerRomFactoryInfo->crc[0] = factory_crc >> 8;	//CRC-16�ĸ�8λ
	ServerRomFactoryInfo->crc[1] = factory_crc & 0x00ff;	//CRC-16�ĵ�8λ
	if(remove("/root/hp/DeviceConfig_factory.rom") == -1) {
		printf("TY3012S ConfigRomFile rm DeviceConfig_factory.rom error!\n");
		//ɾ���ļ����ִ�����Ҫ���ļ������ڣ���������!
	} else {
		printf("TY3012S ConfigRomFile rm DeviceConfig_factory.rom sucess!\n");
	}
	RomFilefd = open("/root/hp/DeviceConfig_factory.rom", O_RDWR | O_CREAT| O_TRUNC, 00700);
	if(RomFilefd == -1) {
		close(RomFilefd);	//���ļ����ִ���ϵͳ���ش����޷���������!
		printf("TY3012S ConfigRomFile open DeviceConfig_factory.rom error: %d!\n", RomFilefd);
		return -2;
	}
	FileLen = write(RomFilefd, RomFileBuf, FACTORY_USER_LEN);
	if(FileLen != FACTORY_USER_LEN) {
		close(RomFilefd);	//д���ļ����ִ����޷���������!
		printf("TY3012S ConfigRomFile write DeviceConfig_factory.rom error: %d!\n", FileLen);
		return -3;
	}
	close(RomFilefd);
	printf("TY3012S ConfigRomFile write DeviceConfig_factory.rom sucess: %d!\n\n", FileLen);
	printf("TY3012S ConfigRomFile set DeviceConfig_factory.rom ok!\n");
	return 0;
}

static int idprom_user_set(int cmd)
{
	RomFactoryInfo *ServerRomFactoryInfo;
	RomUserInfo *ServerRomUserInfo;
	int RomFilefd, UserFilefd, FileLen, i;

	if(cmd == 0) return -1;//�ж��Ƿ���Ҫ��ʼ��DeviceConfig_user.rom�ļ�����һ����Ҫ
	printf("TY3012S ConfigRomFile set DeviceConfig_user.rom start!\n");
	ServerRomFactoryInfo = (RomFactoryInfo *)RomFileBuf;
	ServerRomUserInfo = (RomUserInfo *)RomUserBuf;
	memset(RomFileBuf, 0, sizeof(RomFileBuf));	//���RomFileBuf
	memset(RomUserBuf, 0, sizeof(RomUserBuf));	//���RomUserBuf
	//��ȡDeviceConfig_factory.rom�ļ�����ʼ��RomFileBuf
	RomFilefd = open("DeviceConfig_factoryu.rom", O_RDWR);
	if(RomFilefd == -1) {
		close(RomFilefd);	//���ļ����ִ�����Ҫ���ļ������ڣ��޷���������!
		printf("TY3012S ConfigRomFile open DeviceConfig_factory.rom error: %d!!\n", RomFilefd);
		return -2;
	}
	FileLen = read(RomFilefd, RomFileBuf, FACTORY_USER_LEN);
	if(FileLen != FACTORY_USER_LEN) {
		close(RomFilefd);	//�����ļ����ִ��󣬶�ȡ�ļ������޷���������!
		printf("TY3012S ConfigRomFile read DeviceConfig_factory.rom error: %d!\n", FileLen);
		return -3;
	}
	#if 1
	printf("TY3012S ConfigRomFile print RomFileBuf: %d!", FileLen);
	for(i = 0; i < FileLen; i++) {
		if((i % 0x10) == 0) printf("!\n%02x:", i);
		printf(" %02x", RomFileBuf[i]);
	}
	printf("!\n");
	#endif
	close(RomFilefd);
	printf("TY3012S ConfigRomFile read DeviceConfig_factory.rom ok!!\n");
	memcpy(ServerRomUserInfo->name,      ServerRomFactoryInfo->name,      sizeof(ServerRomUserInfo->name));
	memcpy(ServerRomUserInfo->type,      ServerRomFactoryInfo->type,      sizeof(ServerRomUserInfo->type));
	memcpy(ServerRomUserInfo->passwd,    ServerRomFactoryInfo->passwd,    sizeof(ServerRomUserInfo->passwd));
	memcpy(ServerRomUserInfo->ip1,       ServerRomFactoryInfo->ip1,       sizeof(ServerRomUserInfo->ip1));
	memcpy(ServerRomUserInfo->ip2,       ServerRomFactoryInfo->ip2,       sizeof(ServerRomUserInfo->ip2));
	memcpy(ServerRomUserInfo->mask1,     ServerRomFactoryInfo->mask1,     sizeof(ServerRomUserInfo->mask1));
	memcpy(ServerRomUserInfo->mask2,     ServerRomFactoryInfo->mask2,     sizeof(ServerRomUserInfo->mask2));
	memcpy(ServerRomUserInfo->gateway1,  ServerRomFactoryInfo->gateway1,  sizeof(ServerRomUserInfo->gateway1));
	memcpy(ServerRomUserInfo->gateway2,  ServerRomFactoryInfo->gateway2,  sizeof(ServerRomUserInfo->gateway2));
	memcpy(ServerRomUserInfo->note,      ServerRomFactoryInfo->note,      sizeof(ServerRomUserInfo->note));
	memcpy(ServerRomUserInfo->reserved1, ServerRomFactoryInfo->reserved1, sizeof(ServerRomUserInfo->reserved1));
	memset(ServerRomUserInfo->reserved2, 0, sizeof(ServerRomUserInfo->reserved2));
	memset(ServerRomUserInfo->crc,       0, sizeof(ServerRomUserInfo->crc));
	//��DeviceConfig_user.rom�����ļ�д������ļ�DeviceConfig_user.rom
	if(remove("DeviceConfig_user.rom") == -1) {
		printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom error!\n");
		//ɾ���ļ����ִ�����Ҫ���ļ������ڣ���������!
	} else {
		printf("TY3012S ConfigRomFile rm DeviceConfig_user.rom sucess!\n");
	}
	UserFilefd = open("/root/hp/DeviceConfig_user.rom", O_RDWR | O_CREAT, 00700);
	if(UserFilefd == -1) {
		close(UserFilefd);	//���ļ����ִ��󣬼�������!
		printf("TY3012S ConfigRomFile open DeviceConfig_user.rom error: %d!\n", UserFilefd);
	}
	FileLen = write(UserFilefd, RomUserBuf, FACTORY_USER_LEN);
	if(FileLen != FACTORY_USER_LEN) {
		close(UserFilefd);	//д���ļ����ִ��󣬼�������!
		printf("TY3012S ConfigRomFile write DeviceConfig_user.rom error: %d!\n", i);
	}
	close(UserFilefd);
	printf("TY3012S ConfigRomFile write DeviceConfig_user.rom sucess: %d!\n\n", i);
	printf("TY3012S ConfigRomFile set DeviceConfig_user.rom ok!\n");
	return 0;
}

static int idprom_factory_display(int cmd)
{
	RomFactoryInfo *ServerRomFactoryInfo;
	int i;

	if(cmd == 0) return -1;
	ServerRomFactoryInfo = (RomFactoryInfo *)RomFileBuf;
	ServerRomFactoryInfo->name[15] = 0;
	ServerRomFactoryInfo->type[15] = 0;
	ServerRomFactoryInfo->sn[15]   = 0;
	ServerRomFactoryInfo->note[31] = 0;
	ServerRomFactoryInfo->reserved1[31] = 0;
	ServerRomFactoryInfo->reserved1[63] = 0;
	printf("TY3012S ConfigRomFile RomFactoryBuf is follows:\n");
	printf("TY3012S ConfigRomFile DevFlag:%x,%x,%x,%x!\n", ServerRomFactoryInfo->flag[0], ServerRomFactoryInfo->flag[1], ServerRomFactoryInfo->flag[2], ServerRomFactoryInfo->flag[3]);
	printf("TY3012S ConfigRomFile DevName:%s!\n", ServerRomFactoryInfo->name);
	printf("TY3012S ConfigRomFile DevType:%s!\n", ServerRomFactoryInfo->type);
	printf("TY3012S ConfigRomFile DevSn  :%s!\n", ServerRomFactoryInfo->sn);
	printf("TY3012S ConfigRomFile DevPass:%c%c%c%c!\n", ServerRomFactoryInfo->passwd[0], ServerRomFactoryInfo->passwd[1], ServerRomFactoryInfo->passwd[2], ServerRomFactoryInfo->passwd[3]);
	printf("TY3012S ConfigRomFile DevMac1:%02x,%02x,%02x,%02x,%02x,%02x!\n", ServerRomFactoryInfo->mac1[0], ServerRomFactoryInfo->mac1[1], ServerRomFactoryInfo->mac1[2], ServerRomFactoryInfo->mac1[3], ServerRomFactoryInfo->mac1[4], ServerRomFactoryInfo->mac1[4]);
	printf("TY3012S ConfigRomFile DevMac1:%02x,%02x,%02x,%02x,%02x,%02x!\n", ServerRomFactoryInfo->mac2[0], ServerRomFactoryInfo->mac2[1], ServerRomFactoryInfo->mac2[2], ServerRomFactoryInfo->mac2[3], ServerRomFactoryInfo->mac2[4], ServerRomFactoryInfo->mac2[4]);
	printf("TY3012S ConfigRomFile DevNip1:%03d.%03d.%03d.%03d!\n", ServerRomFactoryInfo->ip1[0], ServerRomFactoryInfo->ip1[1], ServerRomFactoryInfo->ip1[2], ServerRomFactoryInfo->ip1[3]);
	printf("TY3012S ConfigRomFile DevNip2:%03d.%03d.%03d.%03d!\n", ServerRomFactoryInfo->ip2[0], ServerRomFactoryInfo->ip2[1], ServerRomFactoryInfo->ip2[2], ServerRomFactoryInfo->ip2[3]);
	printf("TY3012S ConfigRomFile DevMsk1:%03d.%03d.%03d.%03d!\n", ServerRomFactoryInfo->mask1[0], ServerRomFactoryInfo->mask1[1], ServerRomFactoryInfo->mask1[2], ServerRomFactoryInfo->mask1[3]);
	printf("TY3012S ConfigRomFile DevMsk2:%03d.%03d.%03d.%03d!\n", ServerRomFactoryInfo->mask2[0], ServerRomFactoryInfo->mask2[1], ServerRomFactoryInfo->mask2[2], ServerRomFactoryInfo->mask2[3]);
	printf("TY3012S ConfigRomFile DevGat1:%03d.%03d.%03d.%03d!\n", ServerRomFactoryInfo->gateway1[0], ServerRomFactoryInfo->gateway1[1], ServerRomFactoryInfo->gateway1[2], ServerRomFactoryInfo->gateway1[3]);
	printf("TY3012S ConfigRomFile DevGat2:%03d.%03d.%03d.%03d!\n", ServerRomFactoryInfo->gateway2[0], ServerRomFactoryInfo->gateway2[1], ServerRomFactoryInfo->gateway2[2], ServerRomFactoryInfo->gateway2[3]);
	i = ServerRomFactoryInfo->udp_port1[0] * 0x100 + ServerRomFactoryInfo->udp_port1[1];
	printf("TY3012S ConfigRomFile DevUdp1:%x, %d!\n", i, i);
	i = ServerRomFactoryInfo->udp_port2[0] * 0x100 + ServerRomFactoryInfo->udp_port2[1];
	printf("TY3012S ConfigRomFile DevUdp2:%x, %d!\n", i, i);
	i = ServerRomFactoryInfo->tcp_port1[0] * 0x100 + ServerRomFactoryInfo->tcp_port1[1];
	printf("TY3012S ConfigRomFile DevTcp1:%x, %d!\n", i, i);
	i = ServerRomFactoryInfo->tcp_port2[0] * 0x100 + ServerRomFactoryInfo->tcp_port2[1];
	printf("TY3012S ConfigRomFile DevTcp2:%x, %d!\n", i, i);
	i = ServerRomFactoryInfo->tcp_port3[0] * 0x100 + ServerRomFactoryInfo->tcp_port3[1];
	printf("TY3012S ConfigRomFile DevTcp3:%x, %d!\n", i, i);
	i = ServerRomFactoryInfo->tcp_port4[0] * 0x100 + ServerRomFactoryInfo->tcp_port4[1];
	printf("TY3012S ConfigRomFile DevTcp4:%x, %d!\n", i, i);
	i = ServerRomFactoryInfo->tcp_port5[0] * 0x100 + ServerRomFactoryInfo->tcp_port5[1];
	printf("TY3012S ConfigRomFile DevTcp5:%x, %d!\n", i, i);
	i = ServerRomFactoryInfo->tcp_port6[0] * 0x100 + ServerRomFactoryInfo->tcp_port6[1];
	printf("TY3012S ConfigRomFile DevTcp6:%x, %d!\n", i, i);
	printf("TY3012S ConfigRomFile DevHard:%c%c%c%c!\n", ServerRomFactoryInfo->hardware[0], ServerRomFactoryInfo->hardware[1], ServerRomFactoryInfo->hardware[2], ServerRomFactoryInfo->hardware[3]);
	printf("TY3012S ConfigRomFile DevSoft:%c%c%c%c!\n", ServerRomFactoryInfo->software[0], ServerRomFactoryInfo->software[1], ServerRomFactoryInfo->software[2], ServerRomFactoryInfo->software[3]);
	printf("TY3012S ConfigRomFile DevDate:");
	for(i = 0; i < 10; i++) {
		printf("%c", ServerRomFactoryInfo->date[i]);
	}
	printf("!\n");
	printf("TY3012S ConfigRomFile DevNote:%s!\n", ServerRomFactoryInfo->note);
	printf("TY3012S ConfigRomFile DevRes1:%s!\n", ServerRomFactoryInfo->reserved1);
	printf("TY3012S ConfigRomFile DevRes2:%s!\n", ServerRomFactoryInfo->reserved2);
	printf("TY3012S ConfigRomFile DevCrc :%x,%x!\n", ServerRomFactoryInfo->crc[0], ServerRomFactoryInfo->crc[1]);
	return 0;
}

static int idprom_user_display(int cmd)
{
	RomUserInfo *ServerRomUserInfo;

	if(cmd == 0) return -1;
	ServerRomUserInfo = (RomUserInfo *)RomUserBuf;
	ServerRomUserInfo->name[15] = 0;
	ServerRomUserInfo->type[15] = 0;
	ServerRomUserInfo->note[31] = 0;
	ServerRomUserInfo->reserved1[31] = 0;
	ServerRomUserInfo->reserved1[129] = 0;
	printf("TY3012S ConfigRomFile RomUserBuf is follows:\n");
	printf("TY3012S ConfigRomFile DevName:%s!\n", ServerRomUserInfo->name);
	printf("TY3012S ConfigRomFile DevType:%s!\n", ServerRomUserInfo->type);
	printf("TY3012S ConfigRomFile DevPass:%c%c%c%c!\n", ServerRomUserInfo->passwd[0], ServerRomUserInfo->passwd[1], ServerRomUserInfo->passwd[2], ServerRomUserInfo->passwd[3]);
	printf("TY3012S ConfigRomFile DevNip1:%03d.%03d.%03d.%03d!\n", ServerRomUserInfo->ip1[0], ServerRomUserInfo->ip1[1], ServerRomUserInfo->ip1[2], ServerRomUserInfo->ip1[3]);
	printf("TY3012S ConfigRomFile DevNip2:%03d.%03d.%03d.%03d!\n", ServerRomUserInfo->ip2[0], ServerRomUserInfo->ip2[1], ServerRomUserInfo->ip2[2], ServerRomUserInfo->ip2[3]);
	printf("TY3012S ConfigRomFile DevMsk1:%03d.%03d.%03d.%03d!\n", ServerRomUserInfo->mask1[0], ServerRomUserInfo->mask1[1], ServerRomUserInfo->mask1[2], ServerRomUserInfo->mask1[3]);
	printf("TY3012S ConfigRomFile DevMsk2:%03d.%03d.%03d.%03d!\n", ServerRomUserInfo->mask2[0], ServerRomUserInfo->mask2[1], ServerRomUserInfo->mask2[2], ServerRomUserInfo->mask2[3]);
	printf("TY3012S ConfigRomFile DevGat1:%03d.%03d.%03d.%03d!\n", ServerRomUserInfo->gateway1[0], ServerRomUserInfo->gateway1[1], ServerRomUserInfo->gateway1[2], ServerRomUserInfo->gateway1[3]);
	printf("TY3012S ConfigRomFile DevGat2:%03d.%03d.%03d.%03d!\n", ServerRomUserInfo->gateway2[0], ServerRomUserInfo->gateway2[1], ServerRomUserInfo->gateway2[2], ServerRomUserInfo->gateway2[3]);
	printf("TY3012S ConfigRomFile DevNote:%s!\n", ServerRomUserInfo->note);
	printf("TY3012S ConfigRomFile DevRes1:%s!\n", ServerRomUserInfo->reserved1);
	printf("TY3012S ConfigRomFile DevRes2:%s!\n", ServerRomUserInfo->reserved2);
	return 0;
}

static int initRomUserFile()
{
	int RomFilefd, UserFilefd;
	int FileLen, i;

	memset(RomFileBuf, 0, sizeof(RomFileBuf));
	memset(RomUserBuf, 0, sizeof(RomUserBuf));
	//��ʼ��RomFileBuf������DeviceConfig_factory.rom
	printf("TY3012S ConfigRomFile init RomFileBuf start!\n");
	RomFilefd = open("DeviceConfig_factoryu.rom", O_RDWR);
	if(RomFilefd == -1) {
		close(RomFilefd);	//���ļ����ִ���û���ļ����޷���������!
		printf("TY3012S ConfigRomFile open DeviceConfig_factory.rom error: %d!\n", RomFilefd);
		return -1;
	}
	FileLen = read(RomFilefd, RomFileBuf, FACTORY_USER_LEN);
	if(FileLen != FACTORY_USER_LEN) {
		close(RomFilefd);	//�����ļ����ִ��󣬲���ȷ���ļ����޷���������!
		printf("TY3012S ConfigRomFile read DeviceConfig_factory.rom error: %d!\n", FileLen);
		return -2;
	}
	#if 1
	printf("TY3012S ConfigRomFile print RomFileBuf: %d!", FileLen);
	for(i = 0; i < FileLen; i++) {
		if((i % 0x10) == 0) printf("!\n%02x:", i);
		printf(" %02x", RomFileBuf[i]);
	}
	printf("!\n");
	#endif
	close(RomFilefd);
	printf("TY3012S ConfigRomFile init RomFileBuf ok!\n");
	//��ʼ��RomUserBuf������DeviceConfig_user.rom
	printf("TY3012S ConfigRomFile read DeviceConfig_user.rom start!\n");
	UserFilefd = open("DeviceConfig_user.rom", O_RDWR);
	if(UserFilefd == -1) {
		close(UserFilefd);	//���ļ����ִ���û���ļ����޷���������!
		printf("TY3012S ConfigRomFile open DeviceConfig_user.rom error: %d!\n", UserFilefd);
		return -3;
	}
	FileLen = read(UserFilefd, RomUserBuf, FACTORY_USER_LEN);
	if(FileLen != FACTORY_USER_LEN) {
		close(UserFilefd);	//�����ļ����ִ��󣬲���ȷ���ļ����޷���������!
		printf("TY3012S ConfigRomFile read DeviceConfig_user.rom error: %d!\n", FileLen);
		return -4;
	}
	#if 1
	printf("TY3012S ConfigRomFile print RomUserBuf: %d", FileLen);
	for(i = 0; i < FileLen; i++) {
		if((i % 0x10) == 0) printf("!\n%02x:", i);
		printf(" %02x", RomUserBuf[i]);
	}
	printf("!\n");
	#endif
	close(UserFilefd);
	printf("TY3012S ConfigRomFile init RomUserBuf ok!\n");
	return 0;
}

int as_idpromInit()
{
	idprom_factory_set(1);
	idprom_user_set(1);
	//initRomUserFile();

	return 0;
}
#if 0
int main()
{
	int ret;
	pid_t parent_pid = 0;

	sleep(1);
	parent_pid = getpid();
	printf("\n");
	printf("********************************************************\n");
	printf("* Beijing Changtongda Conmunication Technology Co.,Ltd *\n");
	printf("*  40# Xueyuan Road, Haidian District, Beijing 100083  *\n");
	printf("*    TY-3012S Xml/Rom Config Program (version 0.0a)    *\n");
	printf("*                   www.ctdt.com.cn                    *\n");
	printf("*                  Tel (010)62301681                   *\n");
	printf("********************************************************\n");
	printf("TY-3012S enter Xml/Rom Config Program: %d!\n", parent_pid);
	sleep(1);
	//��ʼ��DeviceConfig_factory.rom�ļ�
	ret = idprom_factory_set(1);
	//��ʼ��DeviceConfig_user.rom�ļ�
	ret = idprom_user_set(1);
	//��ȡDeviceConfig_factory.rom�ļ�����ʼ��RomFileBuf
	//��ȡDeviceConfig_user.rom�ļ�����ʼ��RomUserBuf
	ret = initRomUserFile();
	//����ServerRomFactoryInfo=(RomFactoryInfo*)RomFileBuf����ʾRomFileBuf����
	ret = idprom_factory_display(1);
	//����ServerRomUserInfo=(RomUserInfo*)RomUserBuf����ʾRomUserBuf����
	ret = idprom_user_display(1);
	//����ɨ������߳�
	ret = pthread_create(&rom_udp_thread, NULL, ConfigRomFileUdp, NULL);
	if (ret != 0) {
		printf("TY-3012S Rom Config Program Create rom_udp_thread error!\n");
		system("/tools/reset");
		exit(1);
	}
	printf("TY-3012S Rom Config Program Create rom_udp_thread finished!\n");
	sleep(1);
	//����xmlDocument�����߳�
	ret = pthread_create(&xml_tcp_thread, NULL, ConfigXmlFileTcp, NULL);
	if (ret != 0) {
		printf("TY-3012S Xml Config Program Create xml_tcp_thread error!\n");
		system("/tools/reset");
		exit(1);
	}
	printf("TY-3012S Xml Config Program Create xml_tcp_thread finished!\n");
	#if 1
	while(1){
		sleep(1);
	}
	#endif
	ret = 0;
	return ret;
}

#endif
