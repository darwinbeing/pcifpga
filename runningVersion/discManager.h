#ifndef	__DISCMANAGER__H__
#define 	__DISCMANAGER__H__

typedef unsigned char 		u8;
typedef char				s8;
typedef unsigned short 	u16;
typedef short				s16;
typedef unsigned int		u32;
typedef int				s32;



#define 	FALSE 								-1
#define 	TRUE								0

#define 	REFRESH_CMD_NEWFILE				0x01
#define	REFRESH_CMD_ENDFILE				0x02
#define 	REFRESH_CMD_FORMATE				0x03
#define 	REFRESH_CMD_POSITION				0x04
#define 	REFRESH_CMD_CREATE				0x05


#define 	FILENAME_LEN						64

#define   TYPE_DSCMANAGER_A				1
#define	TYPE_DSCMANAGER_B				2

#define 	MSGQ_RQST_SAVE					1		//����
#define	MSGQ_RQST_SAVE_END				2		//���̽���
#define	MSGQ_RQST_NEWFILE				3		//���ļ���
#define	MSGQ_RQST_FORMAT				4		//��ʽ������
#define 	MSGQ_RQST_QUERY					5		//¼���ļ���ѯ
//#define	MSGQ_RQST_QSAVE					6		//��������
#define	MSGQ_RQST_CLEARREC				7		//����ļ���¼��Ϣ�����������
#define 	MSGQ_RQST_DISKCHECK				8		//���̹��ϼ��
#define	MSGQ_RQST_UPLOAD					9		//¼���ļ��ϴ�����
//#define	MSGQ_RQST_UPLOADING				10		//�ֶ��ϴ�
#define 	MSGQ_RQST_STOP_UPLOAD			11		//��ֹ����

/*�����ļ��ļ�ͷ����*/
#if 0
#define	FILE_HEADER_MAGIC_SELF		0xA5B6C7D8		/*��ʶλ,a1b2c3d4*/
#define	FILE_HEADER_MAGIC		0xA1B2C3D4		/*��ʶλ,a1b2c3d4*/
#define	FILE_HEADER_MAJOR		0x02			/*���汾��,Ĭ��0x02*/
#define	FILE_HEADER_MINOR		0x04			/*���汾��,Ĭ��0x04*/
#define	FILE_HEADER_THISZONE	0x0				/*����ʱ��,û��,ȫ0*/
#define	FILE_HEADER_SIGFIGS	0x0				/*��ȷʱ���,û�ã�ȫ0*/
#define	FILE_HEADER_SNAPLEN	0xffffffff		/*���ݰ���󳤶�,0--65535*/
#define	FILE_HEADER_LINKTYPE_ETH	0x01		/*��·������,0x01��ʾ��̫������*/
#else
#define	FILE_HEADER_MAGIC_SELF		0xD8C7B6A5		/*��ʶλ,a1b2c3d4*/
#define	FILE_HEADER_MAGIC		0xD4C3B2A1		/*��ʶλ,a1b2c3d4*/
#define	FILE_HEADER_MAJOR		0x0200			/*���汾��,Ĭ��0x02*/
#define	FILE_HEADER_MINOR		0x0400			/*���汾��,Ĭ��0x04*/
#define	FILE_HEADER_THISZONE	0x0				/*����ʱ��,û��,ȫ0*/
#define	FILE_HEADER_SIGFIGS	0x0				/*��ȷʱ���,û�ã�ȫ0*/
#define	FILE_HEADER_SNAPLEN	0xffffffff		/*���ݰ���󳤶�,0--65535*/
#define	FILE_HEADER_LINKTYPE_ETH	0x01000000		/*��·������,0x01��ʾ��̫������*/
#endif
#define	FILE_TAIL_TAG		0x7e7e7e7e			/*�ļ�β��ʶλ*/
typedef struct
{
	unsigned long tailTag;			/*��ʶλ,0x7e7e7e7e*/
	unsigned long packetCount;		/*�ܰ���*/
}FileTail;

//һЩ����
typedef struct tagFileHeader{			// �ļ���ͷ������24�ֽ�
	u32		dwMagic;	// �̶�0x1A2B3C4D
	u16		wMajor;		// ���汾�� 0x0200
	u16		wMinor;		// �ΰ汾�� 0x0400
	u32		dwThisZone;	// ���صı�׼ʱ�䣬Ĭ��ȫ��
	u32		dwSigFigs;	// ʱ����ľ��ȣ�Ĭ��ȫ��
	u32		dwSnapLen;	// �������洢���ȣ����0x0000FFFF��4�ֽ�
	u32		dwLinkType;	// ��·���ͣ�������0x01000000����ʾ��Ethernet��·
}FileHeader;

typedef struct
{
	FileHeader header;				/*��׼�ļ�ͷ*/
	unsigned int cfginfolen;			/*�����ļ�����*/
	unsigned char *pCfgInfo;					/*�����ļ�����*/
}CustomFHeader;

typedef struct MsgQueueBuf{
	long 	mtype;				//��Ϣ���շ�������
	long 	msgSrc;				//��Ϣ���ͷ�������
	u8 		msgRequest;			//��Ϣ���������
	u8		msgResponse;		//��Ϣ�ظ��Ľ��
	u8 		msgReverse[2];		//�ṹ����
	u8		msgParam[FILENAME_LEN];			//��Ż�õ��ļ�·��
	s32 		msgParam2;			//ͨ����
	long 	msgParam3;			//������,д�̴�С
	s32		msgParam4;			/*Socket������/��������*/
}MsgQueueBuf;


typedef struct
{
	int chNo;
	int sockfd;
	unsigned long session;
}QueryParam;

typedef struct
{
	char filename[FILENAME_LEN];
	int sockfd;
	unsigned long session;
}FileInfoParam;


typedef struct
{
	char year[4];					
	char mon[2];
	char day[2];
	char hour[2];
	char min[2];
	char sec[2];
}TimeStr;

void dm_discManagerThread( );													//���̹����߳����
void dm_msgDiscMngSend(int quest, char *param1, int param2, long param3, int param4);			//���̹�����Ϣ�ӿ�
#endif
