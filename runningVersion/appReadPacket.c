#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#include "common.h"
#include "discManager.h"
#include "appReadPacket.h"
#include "ProcessLayerHandler.h"
#include "backStage.h"



static int AppQid;							/*����������Ϣ,Ҳ�մ��̽�������Ϣ*/
Flags AppFlag[nCHANNEL];					/*��ͨ��״̬��־*/
SvUploadCondition	SvCondition;				/*SVָ���ϴ�����*/
static int drvFd;							/*pci�豸�ļ�������*/

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;			/*������*/

MsgCommand		*AppMsgCommand[nCHANNEL];					/*������Ϣ���ݽṹ*/
MsgChInterface	*AppChInterface[nCHANNEL];
MsgGpsInterface	*AppGpsInterface[nCHANNEL];
MsgFpgaTime		*AppFpgaTime[nCHANNEL];
MsgTriggerTime	*AppTriggerTime[nCHANNEL];
MsgAlarmOut		*AppAlarmOut[nCHANNEL];
MsgBeepAlarm	*AppBeepAlarm[nCHANNEL];
MsgWarchDog		*AppWarchDog[nCHANNEL];
MsgClearFifo		*AppClearFifo[nCHANNEL];
MsgChFilter		*AppChFilter[nCHANNEL];
MsgFilterMac		*AppFilterMac[nCHANNEL];
MsgChStatus		*AppChStatus[nCHANNEL];
MsgTriggerStatus	*AppTriggerStatus[nCHANNEL];
MsgKeyStatus		*AppKeyStatus[nCHANNEL];
MsgFrameCounters	*AppFrameCounters[nCHANNEL];
MsgReadBuffer	*AppReadBuffer[nCHANNEL];


/*�����������ֱ���*/
static unsigned char MsgBuffer[nCHANNEL][MSG_BUF_LEN];
unsigned char		Ch1Buffer[CH_BUF_LEN];
unsigned char		Ch2Buffer[CH_BUF_LEN];

/*�����ñ���*/
static unsigned long packetCnt = 0;
/*���ķ����̲߳鿴�˱�־,�����Ƿ���з�������*/
int app_getNewFileFlag(chNo)
{
	return AppFlag[chNo].newFile;
}

int app_getStopParseFlag(int chNo)
{
	return AppFlag[chNo].stopParse;
}

unsigned char app_getSvConChNo()
{
	return SvCondition.chNo;
}

void app_setSvCondition(unsigned char *info)
{
	
	if (info == NULL)
	{
		/*����NULL��ʾֹͣ��smv�ϴ�,��ƥ�������*/
		memset((unsigned char *)&SvCondition, 0, sizeof(SvUploadCondition));
		return;
	}

	memcpy((unsigned char *)&SvCondition, info, sizeof(SvUploadCondition));

	return;
}

/*��sv�ϴ����ֵ���,����ر���sv�ϴ�,��ֹͣ�ϴ�,�ر��̻߳�����Դ*/
int app_getSvEnable()
{
	return SvCondition.svEnable;
}

/*�ɳ����ӹ������,ͨ����̨���̬����sv�ϴ�����*/
void app_setSvEnable(unsigned char flag)
{
	SvCondition.svEnable = flag;
	return;
}


/*telnet���Բ�ѯapp����״̬�ӿ�*/
int app_getMsgQueueInfo(struct msqid_ds *info)
{
	msgctl(AppQid, IPC_STAT, info);
	return 0;
}

/*telnet �����ýӿ�,�ݲ�����,�Ȳ��Ľ�*/
unsigned long app_getPacketCnt()
{
	return packetCnt;
}

/*�洢��������
flag:0,��ʱ����; 1, ��������
position:�����߼�����,0,1,2
chNo:ͨ����
*/
static unsigned long app_saveBufferData(unsigned int flag, unsigned int chNo, unsigned int position)
{
	Flags *appFlag;
	int no;
	unsigned long len, offsetLen;
	if ((flag > 1) || (chNo > nCHANNEL) || (chNo == 0))
	{
		APP_DEBUG("app_saveBufferData param error ,flag:%d, chNo:%d---lineNo:%d\n",
												flag, chNo, __LINE__);
		return -1;
	}

	printf("app_saveBufferData flag:%d, chNo:%d\n", flag, chNo);
	no = chNo - 1;
	appFlag = &AppFlag[no];
	
	if (flag == APP_SAVE_TIMEOUT)
	{
		if ((len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.buf) > 0)
		{
			if (appFlag->bufferStatus.currentBuffer == 0)
			{
				if ((len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.buf) > 0)
				{
					appFlag->bufferStatus.bufBStart = appFlag->bufferStatus.buf + MBYTES_10;
					appFlag->bufferStatus.analyz = appFlag->bufferStatus.rear = appFlag->bufferStatus.bufBStart;
				}
			}
			else if (appFlag->bufferStatus.currentBuffer == 1)
			{
				if ((len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.bufBStart) > 0)
				{
					appFlag->bufferStatus.bufCStart = appFlag->bufferStatus.buf + MBYTES_20;
					appFlag->bufferStatus.analyz = appFlag->bufferStatus.rear = appFlag->bufferStatus.bufCStart;
				}
			}
			else if (appFlag->bufferStatus.currentBuffer == 2)
			{
				if ((len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.bufCStart) > 0)
				{
					appFlag->bufferStatus.analyz = appFlag->bufferStatus.rear = appFlag->bufferStatus.buf;
				}
			}

			/*�������ٷ���Ϣ*/
			if (len > 0)
			{
				dm_msgDiscMngSend(MSGQ_RQST_SAVE, NULL, chNo, len, appFlag->bufferStatus.currentBuffer);
				appFlag->totalLength += len;
			}
			}
	}
	else if (flag == APP_SAVE_NORMAL)
	{
		//if (appFlag->bufferStatus.currentBuffer == 0)
		if (position == 0)
		{
			len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.buf;			/*AStart����buf*/
			dm_msgDiscMngSend(MSGQ_RQST_SAVE, NULL, chNo, len, 0);
			appFlag->totalLength += len;		
			appFlag->bufferStatus.bufStatus[0] = 1;
			appFlag->bufferStatus.bufBStart = appFlag->bufferStatus.analyz;
			appFlag->bufferStatus.currentBuffer = 1;
			APP_DEBUG("chNo:%d, bufA save data  ---lineNo:%d\n", chNo, __LINE__);
		}
		//else if (appFlag->bufferStatus.currentBuffer == 1)
		else if (position == 1)
		{
			len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.bufBStart;
			dm_msgDiscMngSend(MSGQ_RQST_SAVE, NULL, chNo, len, 1);
			appFlag->totalLength += len;		
			appFlag->bufferStatus.bufStatus[1] = 1;
			appFlag->bufferStatus.bufCStart = appFlag->bufferStatus.analyz;
			appFlag->bufferStatus.currentBuffer = 2;
			APP_DEBUG("chNo:%d, bufB save data  ---lineNo:%d\n", chNo, __LINE__);
		}	
		//else if (appFlag->bufferStatus.currentBuffer == 2)
		else if (position == 2)
		{
			len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.bufCStart;
			dm_msgDiscMngSend(MSGQ_RQST_SAVE, NULL, chNo, len, 2);
			appFlag->totalLength += len;		
			appFlag->bufferStatus.bufStatus[2] = 1;

			/*β�͵İ������1������ȥ*/
			memcpy(appFlag->bufferStatus.buf, appFlag->bufferStatus.analyz, (appFlag->bufferStatus.rear - appFlag->bufferStatus.analyz));
			offsetLen = appFlag->bufferStatus.rear - appFlag->bufferStatus.analyz;
			
			appFlag->bufferStatus.analyz = appFlag->bufferStatus.buf;
			appFlag->bufferStatus.rear = appFlag->bufferStatus.buf + offsetLen;
			appFlag->bufferStatus.currentBuffer = 0;
			APP_DEBUG("chNo:%d, bufC save data  ---lineNo:%d\n", chNo, __LINE__);
		}
	}

	return len;
}

/*�趨sv�ϴ���Ϣ,�ܹ�10������Ϣ�����ϴ�*/
int svUploadInfoSet(unsigned char **packet, unsigned long len, int reset)
{
	static MsgSvInfo svInfo;
	
	if (reset == 1)
	{
		svInfo.count = 0;
	}
	
	svInfo.packet[svInfo.count] = *packet;
	svInfo.length[svInfo.count] = len;

	svInfo.count++;

	if (svInfo.count == MAX_UPLOAD_NUM)
	{
		svInfo.request = MSG_RQST_SVUPLOAD;
		bs_svMsgSend(svInfo);
		svInfo.count = 0;
	}

	return 0;
}

/*���ݷ���,ʶ��ÿ֡���ݵİ�����,���ͷ�����Ϣ*/
static int parseFifoFrameData(int chNo)
{

	unsigned long qLength;
	unsigned char   *pDataTime, *pPayload;
	unsigned short *pLength, *pFrameState;
	unsigned int *pHead, *pFCS, *pTail;
	unsigned char style, abnormal;
	static unsigned int gooseSvPacketCnt[2] = {0, 0};
	int ret, no;
	int jumpLen;
	
	MsgProcessLayer msg;
	MsgSvInfo svMsg;
	Flags *appFlag;
	
	unsigned char srcMac[6], dstMac[6];
	unsigned short appId;

	unsigned char svCondition[14];			/*Դmac��Ŀ��mac��12�ֽ�,appId���ֽ�*/

	unsigned long frameLen = 0;
	int errorFrame = 0;

	no = chNo - 1;
	appFlag = &AppFlag[no];
	msg.chNo = chNo;

	while(1)
	{
		qLength = (appFlag->bufferStatus.rear - appFlag->bufferStatus.analyz) * 4;
		#if 1
		APP_DEBUG("qLenth:%ld\ndatas:", qLength);
		APP_DEBUG("%08x %08x %08x %08x %08x %08x\n", *appFlag->bufferStatus.analyz,
										*(appFlag->bufferStatus.analyz + 1),
										*(appFlag->bufferStatus.analyz + 2),
										*(appFlag->bufferStatus.analyz + 3),
										*(appFlag->bufferStatus.analyz + 4),
										*(appFlag->bufferStatus.analyz + 5));
		#endif
		if (qLength < 96)			//���֡Ϊ64�ֽ�+16�ֽ�ʱ��+16�ֽڰ���
		{
			//APP_DEBUG("qLength is too short , return to read\n");
			break;
		}

		pHead = appFlag->bufferStatus.analyz;
		if (Ntohs(*pHead) == FRAME_HEADE)		/*�ҵ���֡ͷ1a2b3c4d*/
		{
			pLength = (unsigned short*)(pHead + 1);
			pFrameState = pLength + 1;

			pPayload = pFrameState + 9;
			if (errorFrame == 1)
			{
				printf(" errorFrame , datatime:%08x  %08x\n", *(unsigned int *)(pFrameState+1), *(unsigned int *)(pFrameState+3));
				errorFrame = 0;
			}
			
			APP_DEBUG("pLength:%04x, ---lineNo:%d\n", *pLength, __LINE__);
			if ((Ntohs(*pLength)+14) > qLength)		/*��ǰ���������ݳ��Ȳ�������֡����,�����ȴ���һ�ζ����ٽ�*/
			{
				break;
			}

			packetCnt++;
			
			appFlag->totalPackets++;
			if (appFlag->newFile == 0)												/*���ļ�*/
			{
				APP_DEBUG("chNo:%d,new file name request\n", chNo);
				//strncpy(appFlag->time, "1234567", 14);
				//appFlag->time[14] = '\0';
				dm_msgDiscMngSend(MSGQ_RQST_NEWFILE, NULL, chNo, 0, 0);
				appFlag->newFile = 1;
			}
						
			#if 1
			/*�����ָ��λ���Ƿ���ȷ*/
			APP_DEBUG("pHead:%08x, pLength:%08x\n", *pHead, *pLength);
			//APP_DEBUG("pPayload:%x %x %x %x\n",*pPayload, *(pPayload+1), *(pPayload+2), *(pPayload+3));
			APP_DEBUG("pFrameState:%08x\n",*pFrameState);			
			#endif

			#if 1
			/*ȷ��֡���ͺ��쳣״̬,����goose��smv���ֱ��ĺ��쳣���͸�����Ҫ�����ϱ�*/
			style = (*pFrameState & 0xff00) >> 8;
			//abnormal = (*pFrameState & 0xff);
			//APP_DEBUG("style:%x, abnormal:%x, ---lineNo:%d\n", style, abnormal, __LINE__);

			if (((style & 0x10) == 0) && ((style & 0x20) == 0))			/*�Ȳ���smv,Ҳ����goose��������һ֡,1<<4,1<<5*/
			{
				APP_DEBUG("common packet\n");
				//appFlag->bufferStatus.analyz = (unsigned int *)(pFrameState + 5);		/*��һ֡,state��10�ֽ�*/

				/*��frameStateһֱ��֡β����6 = + 8 - 2,����frame,��time��ʼ,��Ȼ������ֽ�*/
				jumpLen = Ntohs(*pLength) + 6;		
				
				if ((jumpLen % 4) != 0)
				{
					jumpLen = jumpLen + (4 - (jumpLen % 4));
				}

				//printf("jumplen is :%d\n", jumpLen);
				/*������*/
				if ((jumpLen % 4) != 0)
				{
					printf("jumpLen calculate error,val:%d ---lineNo:%d\n",(jumpLen % 4),  __LINE__);
				}
				/************/
				
				/*Ϊ����ָ���ƶ��Ĵ���,�����������ת��,�Ȱ�8�ֽ���ת,
					�ٶ�8ȡ��,��β�ͼ����ֽڲ���,jumpLen%8�Ľ��ֻ��0��4����*/
				appFlag->bufferStatus.analyz = ((unsigned int *)((long long *)(pFrameState+1) + (jumpLen / 8))) + (jumpLen % 8) / 4;

				continue;
			}

			/*�쳣�Ȳ�����,�Ժ��ټ�*/
			#if 0
			if((abnormal & (1 << 0)) != 0)			/*CRC����*/
			{}
			else if((abnormal & (1 << 1)) != 0)			/*���մ���*/
			{}
			else if ((abnormal & (1 << 2)) != 0)			/*��Ч����*/
			{}
			else if ((abnormal & (1 << 3)) != 0)			/*�������ֽ�*/
			{}
			else if ((abnormal & (1 << 4)) != 0)			/*����֡*/
			{}
			#endif

			if (appFlag->stopParse == 0)
			{
				/*�ȿ���������Ϣ��������־�Ƿ��ù�,�������Ҫ�ȶ��п����ٷ�*/
				if (appFlag->queueFull == 1)
				{
					if (plh_getMsgQueueStatus(no) > 0)
					{
						appFlag->queueFull = 0;
						gooseSvPacketCnt[no] = 0;
						/*������ָ�븳ֵ*/
						msg.packets[gooseSvPacketCnt[no]] = (unsigned char *)appFlag->bufferStatus.analyz;
						gooseSvPacketCnt[no]++;
						/*sv��goose ���ĸ���ͳ��*/
						#if 1
						if ((style & 0x10) != 0)
						{
							appFlag->svPackets++;
							msg.type[gooseSvPacketCnt[no]] = MSG_PROCESS_SVTYPE;
						#if 0
							memcpy(dstMac, pPayload, 6);
							memcpy(srcMac, pPayload+6, 6);
							memcpy(appId, pPayload+14, 2);
						#endif

							/*���ﻹ������û��VLAN ID,���ĸ��ֽ�*/
							memcpy(svCondition, pPayload, 12);
							//memcpy(svCondition+12, pPayload+14+4,2);

						#if 0
							if ((strcmp(dstMac, appFlag->svCondition.dstMac) == 0) && 
									(strcmp(srcMac, appFlag->svCondition.srcMAC) == 0) && 
									(appFlag->svCondition.appID == appId))
						#else
						//	if (memcmp(svCondition, SvCondition.dstMac, 12) == 0)			/*�����Ƚ�Ч�ʸ�*/
						#endif
							{
								if (SvCondition.svEnable > 0)
								{
									if (SvCondition.chNo == chNo)
									{
										frameLen = Ntohs(*pLength) + 14;					/*��֡��*/
										if ((frameLen % 4) != 0)
										{
											frameLen = frameLen + 4- (frameLen % 4);			/*�����ֽ���*/
										}
										svUploadInfoSet((unsigned char **)&appFlag->bufferStatus.analyz, frameLen, 1);
									}
								}
							}
						}
						else if ((style & 0x20)  != 0)
						{
							printf("goose type packet ---lineNo:%d\n", __LINE__);
							printf("datatime:%08x  %08x\n", *(unsigned int *)(pFrameState+1), *(unsigned int *)(pFrameState+3));
							appFlag->goosePackets++;
							msg.type[gooseSvPacketCnt[no]] = MSG_PROCESS_GOOSETYPE;
						}
						#endif

						#if 1
						/*���͸澯����*/
						msg.mtype = TYPE_PROCESSLAYER;
						msg.request = MSG_RQST_RESET_WARNING;
						ret = plh_msgSend(&msg);
						#endif
					}
				}
				else
				{
					/*������ָ�븳ֵ*/
					msg.packets[gooseSvPacketCnt[no]] = (unsigned char *)appFlag->bufferStatus.analyz;

					/*sv��goose ���ĸ���ͳ��*/
					#if 1
					if ((style & 0x20) != 0)				/*1<<5*/
					{
						static unsigned long count = 0;
						count++;
						
						appFlag->svPackets++;
						msg.type[gooseSvPacketCnt[no]] = MSG_PROCESS_SVTYPE;
						#if 1
						#if 0
						memcpy(dstMac, pPayload, 6);
						memcpy(srcMac, pPayload+6, 6);
						memcpy((unsigned char *)&appId, pPayload+14, 2);

						if ((strncmp(dstMac, appFlag->svCondition.dstMac, 6) == 0) && 
							(strncmp(srcMac, appFlag->svCondition.srcMAC, 6) == 0) && 
							(appFlag->svCondition.appID == appId))
						{
							svUploadInfoSet(&appFlag->bufferStatus.analyz, (Ntohs(*pLength) + 8 + 6), 1);
						}
						#endif
							memcpy(svCondition, pPayload, 12);
	//						memcpy(svCondition+12, pPayload+14+4,2);

						
						#if 0
							if ((strcmp(dstMac, appFlag->svCondition.dstMac) == 0) && 
									(strcmp(srcMac, appFlag->svCondition.srcMAC) == 0) && 
									(appFlag->svCondition.appID == appId))
						#else
					//		if (memcmp(svCondition, SvCondition.dstMac, 12) == 0)			/*�����Ƚ�Ч�ʸ�*/
						#endif

						{
								if (SvCondition.svEnable > 0)
								{
									frameLen = Ntohs(*pLength) + 14;					/*��֡��*/
									if ((frameLen % 4) != 0)
									{
										frameLen = frameLen + 4- (frameLen % 4);			/*�����ֽ���*/
									}
									svUploadInfoSet((unsigned char **)&appFlag->bufferStatus.analyz, frameLen, 0);
								}
							}
						#endif
					}
					else if ((style & 0x10)  != 0)				/*1<<4*/
					{
						APP_DEBUG("goose type packets, ---lineNo:%d\n", __LINE__);
						printf("datatime:%08x  %08x\n", *(unsigned int *)(pFrameState+1), *(unsigned int *)(pFrameState+3));
						appFlag->goosePackets++;
						msg.type[gooseSvPacketCnt[no]] = MSG_PROCESS_GOOSETYPE;
					}
					#endif
					gooseSvPacketCnt[no]++;
					
					if (gooseSvPacketCnt[no] == MAX_PROCESS_NUM)
					{
						/*10�����������̷߳�һ����Ϣ*/
						msg.mtype = TYPE_PROCESSLAYER;
						msg.request = MSG_RQST_PROCESS;
						msg.count = MAX_PROCESS_NUM;

						#if 1
						ret = plh_msgSend(&msg);
						
						/*��Ϣ����ʧ��,��������*/
						if (ret == PROCESS_MSGQUEUE_FULL)
						{
							APP_DEBUG("PROCESS_MSGQUEUE_FULL appFlag->queueFull = 1 ---lineNo:%d\n", __LINE__);
							appFlag->queueFull = 1;
						}
						#endif
						gooseSvPacketCnt[no] = 0;
					}
				}
			}
			else
			{
				/*��ռ�¼��ָ��,�ȹر�ֹͣ������־�������¼�¼*/				
				gooseSvPacketCnt[no] = 0;
			}
			#endif
		//	appFlag->bufferStatus.analyz = (unsigned int *)(pFrameState + 5); 				/*��һ֡*/
		//	pDataTime = (unsigned char *)(pLength + 1);										/*ʱ���*/

			/*��frameStateһֱ��֡β����6 = + 8 - 2,����frame,��time��ʼ,��Ȼ������ֽ�*/
			jumpLen = Ntohs(*pLength) + 6;		

			if ((jumpLen % 4) != 0)
			{
				jumpLen = jumpLen + (4 - jumpLen % 4);
			}
			
			/*Ϊ����ָ���ƶ��Ĵ���,�����������ת��,�Ȱ�8�ֽ���ת,
				�ٶ�8ȡ��,��β�ͼ����ֽڲ���,jumpLen%8�Ľ��ֻ��0��4����*/
			appFlag->bufferStatus.analyz = ((unsigned int *)((long long *)(pFrameState+1) + (jumpLen / 8))) + (jumpLen % 8) / 4;
		}
		else										/*û�ҵ�֡ͷ������Ų1���ֽ�,������*/
		{
			#if 1
//			static count = 0;
//			count++;
//			if ((count % 2000) == 0)
			{		
				errorFrame = 1;
				//printf("wrong frame head pointer +1\n");
				//printf("time is :%s\n", dm_getCurrentTimeStr());
			}
			#endif
			appFlag->bufferStatus.analyz += 1;
		}
	}

	return 0;

}

/*app��Ϣ���з���Ϣ�ӿ�*/
int app_msgSend(unsigned char quest, int param2, int chNo)
{
	MsgAppBuf msg;

	memset(&msg, 0, sizeof(MsgAppBuf));

	if ((quest == MSGQ_RQST_SAVE_FILE) && (chNo == 0))
	{
		int i;
		for (i = 1; i <= nCHANNEL; i++)
		{
			app_msgSend(MSGQ_RQST_SAVE_FILE, 0, i);
		}
		
		return 0;
	}
	
	msg.msgRequest = quest;
	msg.mtype = chNo;

	msg.msgParam2 = param2;
	
	if (msgsnd(AppQid, &msg, (sizeof(MsgAppBuf)-sizeof(long)), IPC_NOWAIT) < 0)
	{
		APP_DEBUG("chNo:%d,msgsnd(AppQid failed, error:%s ---lineNo:%d\n", chNo, strerror(errno), __LINE__);
	}
	
	return 0;
}


/*���̹������ʱ�����յ������������,��ȡ����������ݶ��׵�ַ*/
unsigned char * app_getBufferStartPtr(int no, int chNo)
{
	if (no == 0)						/*bufferA*/
	{
		return (unsigned char *)AppFlag[chNo].bufferStatus.buf;
	}
	else if (no == 1)					/*bufferB*/
	{
		return (unsigned char *)AppFlag[chNo].bufferStatus.bufBStart;
	}
	else if (no == 2)					/*bufferC*/
	{
		return (unsigned char *)AppFlag[chNo].bufferStatus.bufCStart;
	}
	
	return NULL;
}

/*������Ϣ���ýӿ�*/
static int SetMsgBuffer(int no)
{
	AppMsgCommand[no] = (MsgCommand *)MsgBuffer[no];
	memset(MsgBuffer[no], 0x0, MSG_BUF_LEN);
	AppMsgCommand[no]->Head = MSGHEAD;
	AppMsgCommand[no]->VersionSid	= MSGSESSIONID;
	AppMsgCommand[no]->Id	= MSGID;
	AppMsgCommand[no]->Tail = MSGTAIL;
	return 0;
}


/*������,��ȡLED��״̬*/
int app_getLedStatus(int *ledH, int *ledL)
{
	MsgLedStatus AppLedStatus;
	int ret;
	/*����*/
	AppLedStatus.Head = MSGHEAD;
	AppLedStatus.VersionSid = MSGSESSIONID;
	AppLedStatus.Id = CHECK_LED_STATUS;
	AppLedStatus.Tail = MSGTAIL;

 	pthread_mutex_lock(&mutex);
	ret = read(drvFd, AppLedStatus, MSG_BUF_LEN);			/*����������Ϣ��������Driver*/
	pthread_mutex_unlock(&mutex);

	if (ret == -1)
	{
		/*δ��ȡ��ȷ*/
		return -1;
	}
	else
	{
		*ledH = AppLedStatus.LedH;
		*ledL = AppLedStatus.LedL;

		return 0;
	}
	
	return 0;
}

/*������ ����������,����,���ʹ�����Ϣ,��̬�����������ܿ���*/
#if 1
static void*app_PacketRead(void *args)
#else
void	app_PacketRead(int chNo)
#endif
{
	MsgAppBuf msg;
	unsigned long qLength = 0, len =0, offsetLen = 0;			//��¼��С,countÿ10MB��һ��,totalÿ300M��һ���ļ�,qLenth > 25MB��ͣ����
	int packets = 0;						//��¼����,�����ļ��湻ʱ������̹���

	Flags *appFlag;
	MsgProcessLayer msgProcess;			/*�����߳���Ϣ������Ϣ�ṹ*/
	unsigned long singleFileLength, singleFileLengthMin;		/*�ӽ����õĳ��Ⱦʹ��һ���ļ�*/
	int chNo, no;

	unsigned int idleTimes = 0;

	
	chNo = *((int *)args);
	no = chNo -1;
	printf("chNo:%d\n",chNo);

	appFlag = &AppFlag[no];
	msgProcess.chNo = chNo;

	/*�������л�ȡ�����ļ�����*/
	singleFileLength = xml_getConfigDevFileS()*MBYTES;
	singleFileLengthMin = singleFileLength - MBYTES;	    			/*����60M�ļ�����*/
	APP_DEBUG("singleFileLengthMin = %d\n", singleFileLengthMin / MBYTES);
	
	AppReadBuffer[no] = (MsgReadBuffer *)MsgBuffer[no];
	SetMsgBuffer(no);
	AppReadBuffer[no]->Id   = READ_CH_BUFFER;			//�����յ��ı���

	printf("app_read start, chNo:%d\n", chNo);
	while(1)
	{
		//msgctl(AppQid, IPC_STAT, &info);
		//printf("msgInfo :msg No:%d, max No:%d cur No:%d\n", info.msg_qnum, info.msg_qbytes, info.msg_cbytes);
		#if 1
		if (msgrcv(AppQid, &msg, (sizeof(MsgAppBuf)-sizeof(long)), chNo, IPC_NOWAIT) > 0)		
		{
			APP_DEBUG("AppReadPacket msgrcv packet read,chNo:%d,request:%d,buf No:%ld line:%d\n", chNo, msg.msgRequest, msg.msgParam2, __LINE__);
			if (msg.msgRequest == MSGQ_RQST_WRITE_END)
			{
				/*���̽���,����״̬����*/
				if ((msg.msgParam2 >= 0) && (msg.msgParam2 < 3))
				{
					appFlag->bufferStatus.bufStatus[msg.msgParam2] = 0;
				}
				
				if (appFlag->stopParse == 1)									/*���ù��رշ����ı�־*/
				{
					msgProcess.mtype = TYPE_PROCESSLAYER;
					msgProcess.request = MSG_RQST_RESET_WARNING;
					//plh_msgSend(&msgProcess);								/*�������ø澯������Ϣ*/
					appFlag->stopParse = 0;    									/*��ֹͣ���ķ�����־*/
				}
			}
			else if (msg.msgRequest == MSGQ_RQST_SAVE_FILE)
			{
				/*���ٴ���ֻ�ǽ���ǰ�ļ�����β����,������δ��������ݲ���д�̲���,д��
				����������ʱ����*/
				if (appFlag->newFile != 0)
				{
					APP_FILE_SAVE_END_SET(appFlag, chNo);
				}
			}
			else if (msg.msgRequest == MSGQ_RQST_SAVE_DATA)
			{
				#if 1
				/*ֻ������,�ļ�βͨ����̨���������̰�ťʵ��*/
				if (appFlag->newFile != 0)
				{
					if (appFlag->bufferStatus.currentBuffer == 0)
					{
						if ((len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.buf) > 0)
						{
							appFlag->bufferStatus.bufBStart = appFlag->bufferStatus.buf + MBYTES_10;
							appFlag->bufferStatus.analyz = appFlag->bufferStatus.rear = appFlag->bufferStatus.bufBStart;
						}
					}
					else if (appFlag->bufferStatus.currentBuffer == 1)
					{
						if ((len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.bufBStart) > 0)
						{
							appFlag->bufferStatus.bufCStart = appFlag->bufferStatus.buf + MBYTES_20;
							appFlag->bufferStatus.analyz = appFlag->bufferStatus.rear = appFlag->bufferStatus.bufCStart;
						}
					}
					else if (appFlag->bufferStatus.currentBuffer == 2)
					{
						if ((len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.bufCStart) > 0)
						{
							appFlag->bufferStatus.analyz = appFlag->bufferStatus.rear = appFlag->bufferStatus.buf;
						}
					}


					if (len > 0)
					{
						dm_msgDiscMngSend(MSGQ_RQST_SAVE, NULL, chNo, len, appFlag->bufferStatus.currentBuffer);
						appFlag->bufferStatus.currentBuffer = (appFlag->bufferStatus.currentBuffer + 1) % 3;
					}
				}
				#endif				/*ֻ������,�ļ�βͨ����̨���������̰�ťʵ��*/
				#if 0
				if (appFlag->newFile != 0)
				{
					app_saveBufferData(APP_SAVE_TIMEOUT, chNo, appFlag->bufferStatus.currentBuffer);
				}
				#endif
			}
			else if (msg.msgRequest == MSGQ_RQST_DRV_IOCTRL)
			{
				/*����ԭ������loop1���ݴ���UDP����������Ϣ,Ŀǰ�޴�UDP�˿�,�������ʱֻ�轫
				main.c��if(UdpFlag == 1)������뿽����������,��Ҫ���Ǵ���Զ�ȡ���ĺͷ���Ч�ʵ�Ӱ��*/
				/*ioctl(drvFd,xxx,xxx)*/
				/*write(drvFd,xxx,xxx)*/
				/*����Ҫ����*/
			}
		}

	#if 0
		if (appFlag->queueFull == 0)
		{
			/*�����������澯*/
		}
	#endif
	
		#endif

#if 1		
		//printf("APP_MSGBUFF_READ_SET, chNo:%d, no:%d\n", chNo, no);

		#if 1
		/*��ͨ��ͬʱ����fpga��Դ������,��������*/
	 	if(pthread_mutex_lock(&mutex) != 0)
     		{                      
      			printf("pthread_mutex_lock error\n", strerror(errno));           
     		}                                                       
		else
		{
			APP_MSGBUFF_READ_SET(appFlag->bufferStatus.rear, no, chNo, MsgBuffer[no]);
			len = read(drvFd, appFlag->bufferStatus.rear, CH_BUF_LEN);			/*����������Ϣ��������Driver*/
		}

		 if(pthread_mutex_unlock(&mutex) != 0)
		 {                    
		 	printf("pthread_mutex_unlock error:%s\n", strerror(errno));                         
   		 }                
		#endif

		/*������*/
		//continue;

		/*���ص�len�����ֽڵı���*/
		if (len == 0)
		{
			#if 1
			//û��������,ԭ���봦��UDP��Ϣ���˹���Ų���ϱߴ���,���ﲻ��Ҫ��������
			idleTimes++;
			/*����500�ζ�û����,����*/
			if ((idleTimes % 5000) == 0)
			{
			//	printf("chNo:%d,idleTimes:%d\n", chNo, idleTimes);
			}
			
			if ((idleTimes > 5000) && (idleTimes <= 15000))
			{
				if (appFlag->networkStatus == 1)
				{
					appFlag->networkStatus = 0;	
			//		printf("chNo:%d,send msg to APP , 100 times no packets\n", chNo);
					app_msgSend(MSGQ_RQST_SAVE_DATA, 0, chNo);
				}
			}
			else if (idleTimes > 15000)				/*����15�뻹��û���ݹ���,������ǰ�ļ��洢,���ļ�β*/
			{
				if (appFlag->newFile != 0)
				{
			//		printf("chNo:%d,1500 times no packets, end file\n", chNo);
					app_msgSend(MSGQ_RQST_SAVE_FILE, 0, chNo);
				}
			}
			#endif
			usleep(200);						/*û�������ݾ��ӳ�10ms,�ͷ�cpu��Դ*/
			continue;
		}
		else if (len >0)
		{
			#if 0
			/*���Զ�����������*/
			int i;
			for(i = 0; i < len; i++) 
			{
				if((i % 0x04) == 0) printf("\n%04x:", i);	
					printf(" %08x", appFlag->bufferStatus.rear[i]);	
			}
			//fflush(stdout);		
			printf("\n");
			#endif
			#if 1
			/*����ָ�,�ֶ���������,��״̬*/
			if (idleTimes > 0)
			{
				idleTimes = 0;

				if (appFlag->networkStatus == 0)
				{
					appFlag->networkStatus = 1;	
				}
			}
			#endif
			
			appFlag->bufferStatus.rear += len;
			parseFifoFrameData(chNo);					//��������,����С
		}
		else
		{
			#if 1
			/*���������󣬹ر����´�*/
			APP_DEBUG("driver error ,chNo:%d, reopen ---lineNo:%d\n", chNo, __LINE__);
			/*�����쳣ͬ��������*/
		 	if(pthread_mutex_lock(&mutex) != 0)
	     		{                      
	      			APP_DEBUG("pthread_mutex_lock chNo:%d,error:%s\n", chNo, strerror(errno));                           
	     		}                                                       
	
			close(drvFd);
			
			if ((drvFd = open(READ_DRIVER, O_RDONLY)) == -1)
			{
				APP_DEBUG("pci driver reopen failed,chNo:%d, check error:%s\n",chNo, strerror(errno));
			}		

			 if(pthread_mutex_unlock(&mutex) != 0)
			 {                    
			 	APP_DEBUG("pthread_mutex_unlock chNo:%d, error:%s\n", chNo, strerror(errno));                         
	   		 } 
			 
			#endif
		}
#endif	

#if 1
		/*��ʱ�϶��ڵ�����,����Ҫ������,������β,�����ʱ1����û����,��������������*/
		if((appFlag->bufferStatus.rear - appFlag->bufferStatus.buf) > (MBYTES_30))
		{
			//APP_DEBUG("((appFlag->bufferStatus.buf + MAXQSIZE - appFlag->bufferStatus.rear) < (MSGMAXLEN*1024))\n");
			if (appFlag->bufferStatus.bufStatus[0] == 1)
			{
				/*��������,1��û����,�澯������*/
				APP_DEBUG("chNo:%d, bufA overflow , warning ---lineNo:%d\n", chNo, __LINE__);
				printf("chNo:%d, bufA overflow , warning ---lineNo:%d\n", chNo, __LINE__);
				/*β�͵İ������1������ȥ*/
				memcpy(appFlag->bufferStatus.buf, appFlag->bufferStatus.analyz, (appFlag->bufferStatus.rear - appFlag->bufferStatus.analyz)*4);
				offsetLen = appFlag->bufferStatus.rear - appFlag->bufferStatus.analyz;
				
				appFlag->bufferStatus.analyz = appFlag->bufferStatus.buf;
				appFlag->bufferStatus.rear = appFlag->bufferStatus.buf + offsetLen;
				//appFlag->bufferStatus.rear = appFlag->bufferStatus.buf;
				appFlag->bufferStatus.currentBuffer = 0;
			}
			else
			{
	//			app_saveBufferData(APP_SAVE_NORMAL, chNo, appFlag->bufferStatus.currentBuffer);
				len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.bufCStart;
				dm_msgDiscMngSend(MSGQ_RQST_SAVE, NULL, chNo, len, 2);
				appFlag->totalLength += len;		
				appFlag->bufferStatus.bufStatus[2] = 1;

				/*β�͵İ������1������ȥ*/
				memcpy(appFlag->bufferStatus.buf, appFlag->bufferStatus.analyz, (appFlag->bufferStatus.rear - appFlag->bufferStatus.analyz)*4);
				offsetLen = appFlag->bufferStatus.rear - appFlag->bufferStatus.analyz;
				
				appFlag->bufferStatus.analyz = appFlag->bufferStatus.buf;
				appFlag->bufferStatus.rear = appFlag->bufferStatus.buf + offsetLen;
				//appFlag->bufferStatus.rear = appFlag->bufferStatus.buf;
				appFlag->bufferStatus.currentBuffer = 0;
				APP_DEBUG("chNo:%d, bufC save data  ---lineNo:%d\n", chNo, __LINE__);			
			}
		}
		/*��ʱ�����ڵ�����,����ڶ������ݵĴ洢����,�ж�2�������Ƿ��Ѵ�,û��ͷ���Ϣ�洢*/
		else if((appFlag->bufferStatus.rear - appFlag->bufferStatus.buf) > (MBYTES_20))
		{
			//APP_DEBUG("(appFlag->bufferStatus.rear - appFlag->bufferStatus.buf) >= MBYTES_20\n");
			if (appFlag->bufferStatus.bufStatus[2] == 1)
			{
				/*��������,3��û����,�澯������*/
				APP_DEBUG("chNo:%d, bufC overflow , warning ---lineNo:%d\n", chNo,  __LINE__);
				printf("chNo:%d,bufC overflow , warning ---lineNo:%d\n", chNo, __LINE__);

				appFlag->bufferStatus.bufStatus[1] = 1;
				appFlag->bufferStatus.bufCStart = appFlag->bufferStatus.analyz;
				appFlag->bufferStatus.currentBuffer = 2;
			}
			else
			{
				/*���2�����ݻ�û��,�ȴ���2������*/
			//	app_saveBufferData(APP_SAVE_NORMAL, chNo, appFlag->bufferStatus.currentBuffer);
				if (appFlag->bufferStatus.currentBuffer == 1)
				{
					len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.bufBStart;
					dm_msgDiscMngSend(MSGQ_RQST_SAVE, NULL, chNo, len, 1);
					appFlag->totalLength += len;		
					appFlag->bufferStatus.bufStatus[1] = 1;
					appFlag->bufferStatus.bufCStart = appFlag->bufferStatus.analyz;
					appFlag->bufferStatus.currentBuffer = 2;
					APP_DEBUG("chNo:%d, bufB save data  ---lineNo:%d\n", chNo, __LINE__);
				}
			}
		}
		/*��ʱ�����ڵڶ���,�����һ�����ݵĴ洢����,�ж�1�������Ƿ��Ѵ�,û��ͷ���Ϣ�洢*/
		else if ((appFlag->bufferStatus.rear - appFlag->bufferStatus.buf) > (MBYTES_10))
		{
			if (appFlag->bufferStatus.bufStatus[1] == 1)
			{
				/*��������,2��û����,�澯������*/
				APP_DEBUG("chNo:%d, bufB overflow , warning ---lineNo:%d\n", chNo, __LINE__);
				printf("chNo:%d, bufB overflow , warning ---lineNo:%d\n", chNo, __LINE__);
				appFlag->bufferStatus.bufStatus[0] = 1;
				appFlag->bufferStatus.bufBStart = appFlag->bufferStatus.analyz;
				appFlag->bufferStatus.currentBuffer = 1;
			}
			else
			{
				/*���1�����ݻ�û��,�ȴ���һ������*/
			//	app_saveBufferData(APP_SAVE_NORMAL, chNo, appFlag->bufferStatus.currentBuffer);
				if (appFlag->bufferStatus.currentBuffer == 0)
				{
					len = appFlag->bufferStatus.analyz - appFlag->bufferStatus.buf;	/*AStart����buf*/
					dm_msgDiscMngSend(MSGQ_RQST_SAVE, NULL, chNo, len, 0);
					appFlag->totalLength += len;		
					appFlag->bufferStatus.bufStatus[0] = 1;
					appFlag->bufferStatus.bufBStart = appFlag->bufferStatus.analyz;
					appFlag->bufferStatus.currentBuffer = 1;
					APP_DEBUG("chNo:%d, bufA save data  ---lineNo:%d\n", chNo, __LINE__);
				}			
			}
		}
		/*�����己æ���,�ж��Ƿ��ֹͣ��������,��������������æ�͹ط���*/
		if (((appFlag->bufferStatus.bufStatus[0] == 1) &&(appFlag->bufferStatus.bufStatus[1] == 1)) ||
			((appFlag->bufferStatus.bufStatus[0] == 1) && (appFlag->bufferStatus.bufStatus[2] == 1)) ||
			((appFlag->bufferStatus.bufStatus[1] == 1) && (appFlag->bufferStatus.bufStatus[2] == 1)))
		{
			appFlag->stopParse = 1;
			printf("chNo:%d,buf busy , stop parse ---lineNo:%d\n", chNo, __LINE__);		
			#if 0
			printf("buf 0:%d, 1:%d, 2:%d , stop parse ---lineNo:%d\n", appFlag->bufferStatus.bufStatus[0], 
															appFlag->bufferStatus.bufStatus[1],
															appFlag->bufferStatus.bufStatus[2],
															__LINE__);			
			#endif
		}

		if (appFlag->totalLength >= singleFileLengthMin)
		{
			APP_FILE_SAVE_END_SET(appFlag, chNo);			
			/*APP_DEBUG("SAVE END, xml_getConfigDevFileS:%d\n",xml_getConfigDevFileS());*/
		}
	}
#endif
	return;
}

/*��ʼ��,������Ϣ����,�����ڴ�*/
int app_Init(pthread_t *id1, pthread_t *id2)
{
	key_t key;
	int chNo[2];
	int ret;
	memset(&AppFlag, 0, sizeof(AppFlag));

	#if 1
	chNo[0] = 1;
	AppFlag[0].bufferStatus.buf = (unsigned int *)malloc(sizeof(unsigned int) * MAXQSIZE);

	if (AppFlag[0].bufferStatus.buf == NULL)
	{
		APP_DEBUG("malloc failed, exit system\n");
		exit(-1);
	}

	AppFlag[0].bufferStatus.analyz = AppFlag[0].bufferStatus.buf;
	AppFlag[0].bufferStatus.rear = AppFlag[0].bufferStatus.buf;
	#endif

	#if 1
	chNo[1] = 2;
	AppFlag[1].bufferStatus.buf = (unsigned int *)malloc(sizeof(unsigned int) * MAXQSIZE);

	if (AppFlag[1].bufferStatus.buf == NULL)
	{
		APP_DEBUG("malloc failed, exit system\n");
		exit(-1);
	}

	AppFlag[1].bufferStatus.analyz = AppFlag[1].bufferStatus.buf;
	AppFlag[1].bufferStatus.rear = AppFlag[1].bufferStatus.buf;
	#endif

	printf("mutext init\n");
	if (pthread_mutex_init(&mutex,NULL) != 0)
	{
		printf("pthread_mutex_init failed errno:%s\n", strerror(errno));
		exit(-1);
	}
	
	drvFd = open(READ_DRIVER, O_RDONLY);				//����������
	if (drvFd == -1)
	{
		printf("TY-3012S App open /dev/pcifpga error!\n");	//������ʹ��App��Ϊ��ʶ
		exit(-1);					//��������,��������ʧ
	}

	{
		int len;
		MsgFpgaTime buf;

		buf.Head = MSGHEAD;
		buf.VersionSid = MSGSESSIONID;
		buf.Id = CHECK_FPGA_TIMER;
		buf.Tail = MSGTAIL;
	
		len = read(drvFd, (unsigned char *)&buf, CH_BUF_LEN);			/*����������Ϣ��������Driver*/
	}
	
	if ((AppQid = msgget(500, 0666 | IPC_CREAT)) == -1)
	{
		APP_DEBUG("msgget failed, line:%d\n", __LINE__);
		exit(-1);
	}
	printf("AppQid:%d\n", AppQid);
	
	sleep(1);
	#if 1
	if((ret = pthread_create(id1, NULL, app_PacketRead, (void *)&chNo[0])) != 0 ) 
	{
		DM_DEBUG("\nTY-3012S creats app_Init 1 thread failed!\n");
		return ret;
	}
	#endif
	
	sleep(1);
	#if 1
	if((ret = pthread_create(id2, NULL, app_PacketRead, (void *)&chNo[1])) != 0 ) 
	{
		DM_DEBUG("\nTY-3012S creats app_Init 2 thread failed!\n");
		return ret;
	}
	#endif
	return;
}



