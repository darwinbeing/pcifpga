#ifndef	__APPREADPACKET_H__
#define	__APPREADPACKET_H__
#if 0
const static unsigned short TPID = 0x8100;
const static unsigned short ETHERNET_TYPE_GOOSE  = 0x88B8;
const static unsigned short ETHERNET_TYPE_SMV    = 0x88BA;
const static unsigned short ETHERNET_TYPE_VLAN   = 0x8100;
const static unsigned short ETHERNET_TYPE_IP     = 0x0800;
const static unsigned short ETHERNET_TYPE_ARP    = 0x0806;
const static unsigned short ETHERNET_TYPE_RARP   = 0x8035;
const static unsigned short ETHERNET_TYPE_GMRP   = 0x0001;	//��ȷ��
#endif
//#define MBYTES		(1024 * 1024)
#define MBYTES		0x100000			/*1MB*/
#define MBYTES_30	0x1E00000			/*30MB*/
#define MBYTES_20	0x1400000			/*20MB*/
#define MBYTES_10	0xA00000			/*10MB*/
#define MAXQSIZE 	0x2000000			/*32MB*/
#ifndef MAX_MAC_LEN
#define	MAX_MAC_LEN		6
#endif

#define	TPID	0x8100
#define	ETHERNET_TYPE_GOOSE		0x88B8
#define	ETHERNET_TYPE_SMV		0x88BA
#define	ETHERNET_TYPE_VLAN		0x8100
#define	ETHERNET_TYPE_IP			0x0800
#define	ETHERNET_TYPE_ARP		0x0806
#define	ETHERNET_TYPE_RARP		0x8035
#define	ETHERNET_TYPE_GMRP		0x0001

#define	TYPE_APPREAD_A					1		/*1ͨ�����շ���Ϣ���ͺ�*/
#define	TYPE_APPREAD_B					2		/*2ͨ��*/

#define 	MSGQ_RQST_WRITE_END				1		/*д�̽��������»����־*/
#define	MSGQ_RQST_DRV_IOCTRL			2		/*��������,��UDP��������󷢳�*/
#define	MSGQ_RQST_SAVE_FILE				3		/*���ٴ���,�����ļ�*/
#define	MSGQ_RQST_SAVE_DATA				4		/*��������,������*/

#define	APP_SAVE_TIMEOUT					0		/*��ʱ����*/
#define	APP_SAVE_NORMAL					1		/*��������*/
typedef struct 
{
	long 	mtype;				//��Ϣ���շ�������
	u8 		msgRequest;			//��Ϣ���������
	u8 		msgReverse[3];		//�ṹ����
	//int		msgParam;			/*ͨ����*/
	unsigned int 		msgParam2;			/*���������,1,2,3*/
	unsigned long 	msgParam3;			/*д�̴�С*/
}MsgAppBuf;


#define 	READ_DRIVER		"/dev/pcifpga"



typedef struct
{
	unsigned char *buf;				/*������ָ��*/
	unsigned char *rear;				/*βָ��,ָ�򻺳�����ǰ����β��*/
	unsigned char *analyz;				/*����ָ�룬ָ��������ͷ*/
	unsigned char *bufBStart;			/*����2������ʼλ��*/
	unsigned char *bufCStart;			/*����3������ʼλ��*/
	unsigned char currentBuffer;		/*30MB�����Ϊ������,0:1��,1:2��,2:3��*/
	unsigned char bufStatus[3];			/*0Ϊ��,1Ϊæ(������)*/
	//unsigned long bufALength;			/*����1���Ĵ��̳���*/
	//unsigned long bufBLength;			/*����2���Ĵ��̳���*/
	//unsigned long bufCLength;			/*����3���Ĵ��̳���*/
}BufferStatus;

typedef struct
{
	unsigned char 	chNo;						/*ͨ����*/
	unsigned char 	svEnable;			/*0Ϊ�ر�sv�ϴ�,1Ϊʹ��sv�ϴ�*/
	unsigned char 	dstMac[MAX_MAC_LEN];		/*Ŀ��MAC��ַ*/
	unsigned char 	srcMAC[MAX_MAC_LEN];		/*ԴMAC��ַ*/
	unsigned short 	appID;						/*SMV AppID*/
}SvUploadCondition;							/*ָ��SV�ϴ��ľ�������*/

typedef struct
{
	unsigned char stopParse;			/*ֹͣ���ķ�����־,0Ϊ�ر�,1Ϊ����(����������)*/
	unsigned char queueFull;			/*��������־,0Ϊ����(��պ���0),1Ϊ��*/
	unsigned char newFile;				/*0��ʾû���Ѵ򿪵��ļ�,1��ʾ����,0ʱ�������ļ���*/
	unsigned char networkStatus;		/*0Ϊæ,1Ϊ��(�����ݰ�)*/
	unsigned char time[16];				/*��β����ʱ��*/
	unsigned long totalPackets;			/*�ܰ���*/
	unsigned long	totalLength;			/*�Ѵ����ݴ�С*/
	unsigned long svPackets;			/*����sv���ĸ���ͳ��*/
	unsigned long goosePackets;		/*����goose���ĸ���ͳ��*/
	BufferStatus	bufferStatus;			/*��������һЩ״̬*/
}Flags;

#define APP_MSGBUFF_READ_SET(ptr, no, chNo, buf) \
do \
{ \
	AppReadBuffer[no]->ChNo = chNo; \
	AppReadBuffer[no]->Byte = CH_BUF_LEN; \
	memcpy(ptr, buf, MSG_BUF_LEN); \
}while(0)

#define	APP_FILE_SAVE_END_SET(appFlag, chNo)	\
do \
{\
	dm_msgDiscMngSend(MSGQ_RQST_SAVE_END, NULL, chNo, appFlag->totalPackets, 0); \
	appFlag->totalLength = 0; \
	appFlag->totalPackets = 0; \
	appFlag->newFile = 0; \
}while(0)

#endif
