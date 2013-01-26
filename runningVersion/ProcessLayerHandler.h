/*
�� �� ����ProcessLayerHandler.h
�������������̲�ҵ����ģ�飺��Ҫ���������������ܵĽӿ�
          1 SMV��GOOSE�Ľ��룬
          2 smv��goose���ĵĹ����ж�
��    �ߣ�
��    Ȩ��
�������ڣ�2012-09-19
�޸ļ�¼��2012-09-24

˵    ����
        �ó���ģ������ windows intel �ܹ��¿�������ֲ��power pc�£�Ҫ�����ֽ������⣬��Ҫ����
*/


#ifndef _CTD_DSMNET_PROCESS_LAYER_HANDLER_H
#define _CTD_DSMNET_PROCESS_LAYER_HANDLER_H


//typedef __int64 longlong;//�� linux�����£�ȥ�������
typedef unsigned char bool;

#ifndef MAX_MAC_LEN
#define	MAX_MAC_LEN	6
#endif
/********************************************
*
*      ͨ�ó���
*
********************************************/
#define MAX_SMV_CHNL_NUM  24              /* smv���ͨ���� �ݶ�Ϊ24*/
#define MAX_SMV_ASDU_NUM  8               /* asdu������*/
#define MAX_OBJECT_NAME_LENGTH 64         /* 7-2 �涨 ��ObjectName����󳤶��� 32���ڶ���Ϊ64��*/
#define MAX_OBJECT_REFERENCE_LENGTH 256   /* 7-2 �涨 ��ObjectRefe����󳤶��� 256   */

/********************************************
*
*      SMV GOOSE TAG
*
********************************************/
#define  TAG_APDU_92           0x60
#define  TAG_APDU_91           0x80
#define  TAG_ASDU_NUM          0x80
#define  TAG_SECURITY          0x81
#define  TAG_SEQUENCE_OF_ASDU  0xa2
#define  TAG_ASDU              0x30

#define  TAG_SV_ID        0x80
#define  TAG_DATASET_REF  0x81
#define  TAG_SMP_CNT      0x82
#define  TAG_CONF_REV     0x83
#define  TAG_REFR_TM      0x84
#define  TAG_SMP_SYNCH    0x85
#define  TAG_SMP_RATE     0x86
#define  TAG_DATASET      0x87


#define TAG_GOOSE_APDU              0x61
#define TAG_GOOSE_GOCBREF           0x80
#define TAG_GOOSE_TIMEALLOWEDTOLIVE 0x81
#define TAG_GOOSE_DATASETREF        0x82
#define TAG_GOOSE_GOID              0x83
#define TAG_GOOSE_TIMESTAMP         0x84
#define TAG_GOOSE_STNUM             0x85
#define TAG_GOOSE_SQNUM             0x86
#define TAG_GOOSE_TEST              0x87
#define TAG_GOOSE_CONFREV           0x88
#define TAG_GOOSE_NEEDCOM           0x89
#define TAG_GOOSE_DATASETENTRIESNUM 0x8a

/********************************************
*
*      SMV type
*
********************************************/
#define  SMV_91     0
#define  SMV_92LE   1
#define  SMV_92     2

/********************************************
*
*      λ������
*
********************************************/
#define SETBIT(a, b) ( a |=  (1 << b))
#define CLRBIT(a, b) ( a &= ~(1 << b))


/********************************************
*
*      SMV ���Ĺ���λ�ö���
*
********************************************/
#define  SMV_Priority           0
#define  SMV_VlanID             1
#define  SMV_APPID              2
#define  SMV_DataNum            6
#define  SMV_DstMAC             7

#define  SMV_SmpPointLost       8   /*�����㶪ʧ*/
#define  SMV_SmpPointRepeated   9   /*�������ظ�*/
#define  SMV_SmpReverseOrder    10  /*��������*/
#define  SMV_SmpSynLost         11  /*����ͬ����ʧ*/
#define  SMV_SmpFreq            12  /*����Ƶ���쳣*/
#define  SMV_MuClockSyn         13  /*MUʱ��ͬ���쳣*/
#define  SMV_SmpQuality         14  /*����ֵƷ���쳣*/
#define  SMV_SmpData            15  /*����ֵ�����쳣*/

/********************************************
*
*      GOOSE ���Ĺ���λ�ö���
*
********************************************/
#define		GOOSE_Priority           0
#define		GOOSE_VlanID             1
#define		GOOSE_APPID              2
#define		GOOSE_GocbRef            3
#define		GOOSE_DatSetRef          4
#define		GOOSE_GoID               5
#define		GOOSE_DataEntriesNum     6     /*���ݼ���������Ŀ����*/
#define		GOOSE_DstMAC             7

#define		GOOSE_PacketLost           8   /*���Ķ�ʧ*/
#define		GOOSE_PacketRepeated       9   /*�����ظ�*/
#define		GOOSE_PacketSqReverseOrder 10  /*����˳����ת*/
#define		GOOSE_PacketStJump         11  /*����״̬��Ծ*/
#define		GOOSE_PacketStReverseOrder 12  /*����״̬��ת*/
#define		GOOSE_NdsCom               13  /*��Ҫ��������*/
#define		GOOSE_StChange             14  /*״̬������λ*/
#define		GOOSE_IEDReset             15  /*IED����*/
#define		GOOSE_CommuBreak           16  /*GOOSE ����*/
#define		GOOSE_Test                 17  /*�����ź�*/
#define		GOOSE_Invalid              18  /*�Ƿ�GOOSE�ź�*/

#define	SELFDEF_HEADER_LEN 	24
/* ��ͷ���� */
typedef struct tagSelfDefPacketHeader
{
	unsigned int dwTimeStamp1; /*������ʱ����*/
	unsigned int dwTimeStamp2; /*���С������*/
	unsigned int dwCapLen;     /*����*/
	unsigned int dwTag;        /*TAG*/
}SelfDefPacketHeader;

typedef struct tagSmvDataSet
{
	long v;
	unsigned long q;
}SmvDataSet;

typedef struct tagSmvAsduInfo
{
	unsigned short wSmpCnt;    /*����������*/
	bool           bSmpSynch;  /*����ͬ��*/
	unsigned char  nChnlNum;   /*����ͨ����*/
	SmvDataSet dataSet[MAX_SMV_CHNL_NUM];
}SmvAsduInfo;

/*smv for error analyzer*/
typedef struct tagSmvPacketInfo
{
	int nType;       /*����ֵЭ������,�����9-1�����ж�ҵ����Ϣ����ΪûӴ��������*/
	int nPriority;
	int nVlanID;
	unsigned short wAppID;
	unsigned char szDstMac[6];
	int nAsduNum;
	SmvAsduInfo asdu[MAX_SMV_ASDU_NUM];/*�����Ʒ�ʽ�����Ҫ������Ҫ�������һ�׶��Ȳ���Ʒ�ʸ澯*/
	unsigned int 	dataTime1;
	unsigned int 	dataTime2;
	unsigned char srcMac[MAX_MAC_LEN];
	unsigned char reverse[2];
}SmvPacketInfo;

typedef struct
{
	int nPriority;
	int nVlanID;
	unsigned short appId;					/*AppId*/
	unsigned char destMac[6];				/*�����ļ��ж�Ӧĳ��IED��Ŀ��mac*/
}SmvCfgInfo;


/* goose for error analyzer */
typedef struct tagGoosePacketInfo
{
	int nPriority;
	int nVlanID;
	unsigned short wAppID;
	unsigned char szDstMac[6];
	//
	char szGocbRef[MAX_OBJECT_REFERENCE_LENGTH];
	char szDataSetRef[MAX_OBJECT_REFERENCE_LENGTH]; /*���ݼ�����*/
	char szGoID[MAX_OBJECT_NAME_LENGTH];

	unsigned long stNum;
	unsigned long sqNum;
	bool  bTest;        /*�����ź�*/
	bool  bNdsCom;      /*��Ҫ��������*/

	int nDataEntriesNum;//���ݼ���������Ŀ�ĸ���	

	unsigned int 	dataTime1;
	unsigned int 	dataTime2;
	unsigned char srcMac[MAX_MAC_LEN];
	unsigned char reverse[2];
}GoosePacketInfo;

typedef struct 
{
	int nPriority;
	int nVlanID;
	unsigned short wAppID;
	unsigned char szDstMac[MAX_MAC_LEN];
	char szGocbRef[MAX_OBJECT_REFERENCE_LENGTH];
	char szDataSetRef[MAX_OBJECT_REFERENCE_LENGTH]; /*���ݼ�����*/
	char szGoID[MAX_OBJECT_NAME_LENGTH];
}GooseCfgInfo;						/*�澯������Ҫ�õ�������Ϣ*/

#define	MAX_PROCESS_NUM				10
#define	MSG_PROCESS_SVTYPE			0
#define	MSG_PROCESS_GOOSETYPE		1

typedef struct
{
	long	mtype;					//��Ϣ���շ�������
	unsigned char  type[MAX_PROCESS_NUM];				/*ÿ����������,0Ϊsv,1Ϊgoose*/
	unsigned char request;			/*��������*/
	unsigned char chNo;			/*ͨ����*/
	unsigned int count;			/*��Ҫ�����İ�����*/
	unsigned char *packets[MAX_PROCESS_NUM];		/*10��sv��goose���İ���ָ��*/
}MsgProcessLayer;


typedef struct
{
	int no;					/*���е����0��Ӧ1ͨ��,1��Ӧ2ͨ��*/
	int key;					/*������Ϣ���еļ�ֵ*/
}PlhArgs;					/*�����߳���������*/

/*�澯����������ṹ*/
typedef struct ParseStandard 
{
	unsigned char		chNo;						/*ͨ����,���ڸ澯����*/
	unsigned char 	threadNo;					/*�Ժ��������һ������һ���߳�,�����Ż�����,�����ò���*/
	unsigned char 	reverse[2];	
	unsigned char 	srcMac[MAX_MAC_LEN];		/*Դmac,���ڱ�ʾIED*/
	unsigned char		smvEffect;					/*smv�ο�ֵ�Ƿ���Ч,0Ϊ��Ч,1Ϊ��Ч*/
	unsigned char 	gooseEffect;					/*goose�ο�ֵ�Ƿ���Ч,0Ϊ��Ч,1Ϊ��Ч*/
	SmvPacketInfo	smvInfo;						/*����ǰ����Ƚϵ�smv��Ϣ*/
	SmvCfgInfo		smvCfg;						/*�������ñȽϵ�smv��Ϣ*/
	GoosePacketInfo	gooseInfo;					/*���ڱȽϵ�goose��Ϣ*/
	GooseCfgInfo		gooseCfg;					/*�������ñȽϵ�goose��Ϣ*/
	struct ParseStandard  *next;
}ParseStandardList;						



#define	MSG_RQST_PROCESS		1 	/*����������n����*/
#define	MSG_RQST_RESET_WARNING	2	/*���ø澯������־*/

#define	TYPE_PROCESSLAYER	1	/*Э������澯�߳���Ϣ����*/

#define	PROCESS_MSGQUEUE_FULL	-2

#ifndef nCHANNEL
#define	nCHANNEL	2
#endif
#ifndef nIED
#define	nIED		16
#endif
/***************************************************************************
//
// ���ܽӿ�
//
****************************************************************************/

/******************************************************************
��   �� ������ASN������򣬵ó�ĳ����ĳ���ֵ���ͳ��ȳߴ�
���������pointerΪĳ���� L ����ָ�룬
���������nValSizeΪ��ĳ��ȣ�nLenSizeΪ������L�ĳߴ磨�ֽ�����
�� �� ֵ���ɹ�����true��ʧ�ܷ��� false
*******************************************************************/
bool GetFieldLength( unsigned char * pointer, int  *nValSize,int  *nLenSize);

/******************************************************************
��    �ܣ����ݸ�����ĳ������ֽ������������ֵ������ ( longlong )���ͷ���(С�˸�ʽ)
���������pointerΪĳ�������ָ�룬nSizeΪ
�� �� ֵ������ָ�����ֵ��С�˸�ʽ��
*******************************************************************/
long long GetFieldValue(unsigned char * pointer, int nSize);

/*********************************************************************

��    �ܣ�Goose����-���ڹ����ж�
���������pszSourceΪû�н���ǰ��ԭʼ���ݰ�ָ�룬
          nFileFormatΪ�ļ���ʽ
���������packetInfoΪ��������ݽṹ

�� �� ֵ��goose ����ɹ����� true��goose���ı������ʱ ���� false

**********************************************************************/
bool  GooseDecode(unsigned char* pszSource, int nFileFormat,GoosePacketInfo *packetInfo);

/*********************************************************************

��    �ܣ�Smv����-���ڹ����ж�,   
���������pszSourceΪû�н���ǰ��ԭʼ���ݰ�ָ�룬
          nFileFormatΪ�ļ���ʽ
���������packetInfoΪ��������ݽṹ

�� �� ֵ������ɹ����� true������ʧ�ܷ��� false

**********************************************************************/
bool SmvDecode(unsigned char* pszSource, int nFileFormat, SmvPacketInfo *packetInfo);



/*********************************************************************

��    �ܣ�smv ���ϡ��澯�������ýӿ�,   
���������firstPacketInfo Ϊ��һ��smv���ĵĽ�������Ϣ��
          secondPacketInfo Ϊ��һ������ smv���ĵĽ�������Ϣ
		  pSmvCfgInfo ��smv��������Ϣ ����������ע�͵�����ʱ����װ����ߵ�������Ϣ�ṹ��
          nDiffTime   Ϊ���������ڱ���֮���ʱ��������λΪ΢��

�� �� ֵ�����ظ澯״̬��

**********************************************************************/
unsigned long SmvErrAnalyse(SmvPacketInfo *firstPacketInfo, SmvPacketInfo *secondPacketInfo, int nDiffTime, SmvCfgInfo  *pSmvCfgInfo);

/*********************************************************************

��    �ܣ�GOOSE ���ϡ��澯�������ýӿ�,   
���������firstPacketInfo Ϊ��һ��smv���ĵĽ�������Ϣ��
          secondPacketInfo Ϊ��һ������ smv���ĵĽ�������Ϣ
		  pGooseCfgInfo �� GOOSE ��������Ϣ ����������ע�͵�����ʱ����װ����ߵ�������Ϣ�ṹ��
          nDiffTime   Ϊ���������ڱ���֮���ʱ��������λΪ΢��

�� �� ֵ�����ظ澯״̬��

**********************************************************************/
unsigned long GooseErrAnalyse( GoosePacketInfo *firstPacketInfo, GoosePacketInfo *secondPacketInfo, int nDiffTime, GooseCfgInfo *pGooseCfgInfo);

#endif
