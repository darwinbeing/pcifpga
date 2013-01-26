#ifndef 	__BACKSTAGE_H__
#define __BACKSTAGE_H__

//3��TCP�˿�1��53456��ʵʱSV/SMV�����ϱ�ͨ������SMV Data Port��
//4��TCP�˿�2��53457��ʵʱ�澯�����ϱ�ͨ����RealTime Alarm Data Port��
//5��TCP�˿�3��53458����������ͨ����XML Config Data Port��
//6��TCP�˿�4��53459����ʷ�ļ��ϴ�ͨ����History Data Port��
//7��TCP�˿�5��53460��TCP���ö˿�
//8��TCP�˿�6��53461��ϵͳ����˿ڣ�System Command Port��
//9��UDP�˿�1��10001��װ��ɨ������ͨ���ö˿ڴ���
//a��UDP�˿�2��10001��������1��������2ͨ���ö˿ڴ����������Ϣ
//b��UDP�˿�3��30718��UDP���ö˿ڣ����ڹ�����չ

//����������
#define 	MS_MANAGER_SMV_REQ					0x0067	// ����SMV�����ϱ�
#define	MS_MANAGER_SMV_RESP					0x0167	// ����SMV����ȷ��
#define	MS_MANAGER_SMV_UPLOAD 				0x0168	// SMV�����ϴ�
#define 	MS_MANAGER_SMV_STOP					0x0069   // ��ֹSMV�����ϴ�
#define	MS_MANAGER_SMV_STOP_RSP			0x0169	//��ֹSMV�����ϴ�ȷ��

//װ��Xml�����ļ����ء����ء���ѯ�������Ϣ����
#define	MS_CFG_CONN_REQ		0x08	//����������������
#define	MS_CFG_CONN_RESP		0x09	//��������������Ӧ
#define	MS_CFG_VER_QRY		0x10	//�������ݰ汾��ѯ����
#define	MS_CFG_VER_RESP		0x11	//�������ݰ汾��ѯ��Ӧ
#define	MS_CFG_DOWN_REQ		0x12	//����������������
#define	MS_CFG_DOWN_RESP		0x13	//��������������Ӧ
#define	MS_CFG_DOWNLOAD		0x14	//�����������ؿ�ʼ
#define	MS_CFG_DOWNLOAD_ACK	0x15	//���������������
#define	MS_CFG_UPLOAD_REQ	0x16	//����������������
#define	MS_CFG_UPLOAD			0x17	//�����������ؿ�ʼ

//¼���ļ���ѯ���ϴ�����Ϣ����
#define 	MS_FILETRANS_CONNECT_REQ				0x1000		//¼���ļ���������
#define	MS_FILETRANS_CONNECT_RSP				0x1001		//¼���ļ�������Ӧ
#define	MS_FILETRANS_FILE_LIST_QRY				0x1002		//¼���ļ��б��ѯ
#define	MS_FILETRANS_FILE_LIST_RSP				0x1003		//¼���ļ��б���Ӧ
#define 	MS_FILETRANS_FILE_UPLOAD_REQ			0x1004		//¼���ļ��ϴ�����
#define	MS_FILETRANS_FILE_INFORMATION			0x1005		//�ļ���Ϣ
#define	MS_FILETRANS_FILE_UPLOAD					0x1006		//¼���ļ��ϴ�
#define	MS_FILETRANS_FILE_UPLOAD_CANCEL			0x1007		//¼���ļ���ֹ�ϴ�
#define	MS_FILETRANS_FILE_UPLOAD_CANCEL_ACK	0x1008		//¼���ļ���ֹ�ϴ���Ӧ

#define	TCP_PORT				53456
#define	UDP_PORT				10001
#define 	recTcpPort				4567			//���ܻ���xmlһ��
#define 	XmlTcpPort				5678			//Ӧ������IDPROM��ȷ����53458
#define 	MSG_MAX_LEN			(64*1024)		//������Ϣ�Ļ�����64K�ֽڣ�Xml�ļ����Լ64K�ֽ�-20�ֽ�
#define 	MSG_RES_LEN			32			//������Ϣ���壬ȷ��������Ӧ��Ϣ�ĳ��Ȳ�����32�ֽ�
#define	XML_MIN_LEN			0			//ȷ��Xml�ļ�����С����
#define	MAX_NAME_LEN			64
#define	MAX_MAC_LEN			6

//#define 	HEADER_LEN				14			//������Ϣ���壬ȷ����Ϣ��ͷ��������14�ֽ�
#define 	HEADER_LEN				sizeof(MsgTransHeader)			//������Ϣ���壬ȷ����Ϣ��ͷ��������12�ֽ�

#define 	XML_CFG_FILE			"/root/hp/DeviceConfig.xml"




//װ��Xml�����ļ����ء����ء���ѯ�������Ϣ�ṹ
#if 0
typedef struct MsgCommand			//���ó����һ����Ϣ��ʽ
{						//������Ϣͷ������18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ���ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢMS_CFG_****_REQ=0x08,0x10,0x12,0x14,0x16
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}XmlMsgCommand;
#endif 

typedef struct
{
	unsigned char svStatus;			/*sv�ϴ�״̬,0Ϊ��,1Ϊsv�ϴ���*/
	unsigned char recStatus;			/*¼���ļ��ϴ�״̬,0Ϊ��,1Ϊ�ϴ���*/
	unsigned char reverse[2];			/*����,�Ժ���ܻ����������ϴ�*/
}UploadStatus;

typedef struct 
{
	unsigned int		head;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	length;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ���ֽڣ�����8�ֽ�
	unsigned short	msgId;			//��Ϣ����2�ֽڣ���ϢMS_CFG_****_REQ=0x08,0x10,0x12,0x14,0x16
	unsigned short	session1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	session2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
}MsgTransHeader;

typedef struct 
{
	MsgTransHeader header;			//������ͷ,12�ֽ�
	unsigned char conn_type;			//��������
	unsigned char reverse[3];			//����
	unsigned short tail1;				//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short tail2;				//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}RecMsgCommand;

typedef struct 			//���ó���������Ӧ��Ϣ��ʽ
{						//����������Ӧ��Ϣ����18�ֽڣ���������Ϣβ������14�ֽ�
	MsgTransHeader header;			//������ͷ,12�ֽ�
	unsigned char		result;			//���	
	unsigned char		reason;			//ԭ��
	unsigned char 	reserve[2];		//����
	unsigned short	tail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	tail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}RecConnResponse;

typedef struct
{
	MsgTransHeader header;			//������ͷ,12�ֽ�
	unsigned short channelNo;			//ͨ����
	unsigned short reserve;			//����
	unsigned char dataTime1[8];		//��ʼʱ��
	unsigned char dataTime2[8];		//����ʱ��
	unsigned short tail1;
	unsigned short tail2;
}RecListCommand;

typedef struct
{
	MsgTransHeader header;			//������ͷ,12�ֽ�
	unsigned long	file_size;				//�ļ���С
	unsigned char fault_reason;			//����ԭ��
	unsigned char reserve[3];
	unsigned short tail1;
	unsigned short tail2;
}RecFileInfoResponse;

typedef struct 
{
	MsgTransHeader header;
	unsigned int packets_count;
	unsigned int current_packet;
	unsigned short data_len;
	unsigned short tail1;
	unsigned short tail2;
}RecFileTransRsp;


typedef struct
{
	MsgTransHeader header;
	unsigned char filename[MAX_NAME_LEN];
	unsigned short tail1;
	unsigned short tail2;
}RecFileCommand;

typedef struct
{
	MsgTransHeader header;			//������ͷ,12�ֽ�
	unsigned short tail1;
	unsigned short tail2;
}CommonMsg;

typedef struct
{
	MsgTransHeader header;			//������ͷ,12�ֽ�
	unsigned char dataTime[8];			/*ͬ��ʱ��,7�ֽ�������ʱ����*/
	unsigned short tail1;
	unsigned short tail2;
}TimerSyncCmd;						/*ʱ��ͬ������*/

typedef TimerSyncCmd	DeviceLedRsp;		/*��������ѯ��Ӧ,�ṹͬʱ��ͬ��,8�ֽ�Ϊled*/

typedef struct
{
	MsgTransHeader header;			//������ͷ,12�ֽ�
	unsigned char chNo;				/*ͨ����*/
	unsigned char reverse[3];			/*����*/
	unsigned short tail1;
	unsigned short tail2;
}SaveFileCmd;

typedef struct
{
	MsgTransHeader header;				/*������ͷ,12�ֽ�*/
	unsigned char chNo;					/*ͨ����*/
	unsigned char style;					/*��������*/
	unsigned char iedMac[MAX_MAC_LEN]; 	/*ied��MAC��ַ*/
	unsigned int 	appId;					/*app ID*/
	unsigned short tail1;					
	unsigned short tail2;
}NetCountCmd;						/*����ͳ������*/

typedef struct 
{
	MsgTransHeader header;				/*������ͷ,12�ֽ�*/
	unsigned char chNo;					/*ͨ����*/
	unsigned char style;					/*��������*/
	unsigned char iedMac[MAX_MAC_LEN]; 	/*ied��MAC��ַ*/
	unsigned char startTime[8];				/*��ʼʱ��*/
	long long		param[30];				/*���30��,240�ֽ�*/
	unsigned short tail1;					
	unsigned short tail2;
}NetCountRsp;

typedef struct 	//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						
	MsgTransHeader 	header;					/*������ͷ,12�ֽ�*/
	unsigned int		msgAckTag;				/*��Ϣ����4�ֽڣ���ҪӦ���־!*/
	unsigned char		almCatg;					/*��Ϣ����1�ֽڣ��澯����*/
	unsigned char 	chNo;					/*��Ϣ����1�ֽڣ�ͨ����*/
	unsigned char 	srcMac[MAX_MAC_LEN];	/*��Ϣ����6�ֽڣ�ԴMAC��ַ*/
	unsigned char 	dstMac[MAX_MAC_LEN];	/*��Ϣ����6�ֽڣ�Ŀ��MAC��ַ*/
	unsigned short	appId;					/*��Ϣ����2�ֽڣ�AppId*/
	unsigned int		MsgAlarmCode;			/*��Ϣ����4�ֽڣ����ϱ���!*/
	unsigned int 		dataTime1;				/*��Ϣ����4�ֽڣ�����ʱ��!UTCʱ��������*/
	unsigned int 		dataTime2;				/*ʱ����*/
	unsigned short	tail1;				/*��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909*/
	unsigned short	tail2;				/*��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7*/
}AlarmReportReq;							/*�澯�ϱ�����*/


typedef struct
{
	MsgTransHeader	header;			//����֡ͷ,12�ֽ�
	unsigned char 	CHNLNO;		//ͨ����
	unsigned char 	Reserved;		//����
	unsigned char 	SrcMAC[MAX_MAC_LEN];		//ԴMAC��ַ
	unsigned char 	DstMac[MAX_MAC_LEN];		//Ŀ��MAC��ַ
	unsigned short	AppID;			//SMV AppID
	unsigned short 	tail1;			//֡β
	unsigned short 	tail2;
}SmvSendCommand;

typedef struct
{
	MsgTransHeader	header;			//����֡ͷ,12�ֽ�
	unsigned int		ACKParam;		//�������룬0:�����������θ�ԭ,1:������������,2:�ܾ�����
	unsigned short 	tail1;			//֡β
	unsigned short 	tail2;
}SmvSendResponse;

typedef struct
{
	MsgTransHeader	header;
	unsigned int 		packLen;		//���ĳ���
	unsigned short 	tail1;			//֡β
	unsigned short 	tail2;
}SmvUploadResponse;


typedef struct MsgCommand			//���ó����һ����Ϣ��ʽ
{						//������Ϣͷ������18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int		MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ���ֽڣ�����8�ֽ�
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢMS_CFG_****_REQ=0x08,0x10,0x12,0x14,0x16
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}XmlMsgCommand;


typedef struct ConnResponse			//���ó���������Ӧ��Ϣ��ʽ
{						//����������Ӧ��Ϣ����18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢMS_CFG_CONN_RESP=0x09
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}XmlConnResponse;

typedef struct VerResponse			//���ó���汾��Ӧ��Ϣ��ʽ
{						//���ð汾��Ӧ��Ϣ����24�ֽ�
	unsigned int		MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����14�ֽ�
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢMS_CFG_VER_RESP=0x11
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgVersion;		//���汾��1�ֽڣ��Ӱ汾��1�ֽ�
	unsigned short	MsgTime1;		//�ļ�ʱ��2�ֽڣ���һ�ֽ�
	unsigned short	MsgTime2;		//�ļ�ʱ��2�ֽڣ��ڶ��ֽ�
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}XmlVerResponse;

typedef struct DownloadResponse			//���ó���������Ӧ��Ϣ��ʽ
{						//����������Ӧ��Ϣ����20�ֽ�
	unsigned int		MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����10�ֽ�
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢMS_CFG_DOWN_RESP=0x13
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	//unsigned short	MsgEnable;		//����ʹ��2�ֽڣ�
	unsigned char 	MsgEnable;		//����ʹ��2�ֽڣ�
	unsigned char		MsgReason;		//��������ʱ��ԭ��
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}XmlDownloadResponse;

typedef struct DownloadFinish			//���ó������������Ϣ��ʽ
{						//����������Ӧ��Ϣ����18�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢMS_CFG_DOWNLOAD_ACK=0x17
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}XmlDownloadFinish;

typedef struct UploadResponse			//���ó������������Ϣ��ʽ
{						//����������Ӧ��Ϣ����18�ֽ�
	unsigned int		MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢMS_CFG_DOWNLOAD_ACK=0x17
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgLen;			//��Ϣ����2�ֽڣ�Xml�����ļ��ĳ���
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}XmlUploadResponse;


/*�ڶ�����*/

/*
   1��װ�õĸ澯�ϱ�����
   2��װ�õ�״̬��ѯ����
*/

//HeartBeat

//װ��״̬��ѯ�������Ϣ����
#define	MS_ROUTE_TEST_REQ	0x0020	//01��01a:·�ɲ������󣬼����������Ϣ
#define	MS_ROUTE_TEST_ACK	0x0120	//02��01b:·�ɲ���ȷ�ϣ�װ�÷�������Ϣ
#define	MS_HEART_BEAT_REQ	0x0121	//03��02b:������������װ�÷�������Ϣ
#define	MS_HEART_BEAT_ACK	0x0021	//04��02a:��������ȷ�ϣ������������Ϣ
#define	MS_RESET_DEVI_REQ		0x0022	//05��03a:װ�ø�λ���󣬼����������Ϣ
#define	MS_RESET_DEVI_ACK		0x0122	//06��03b:װ�ø�λȷ�ϣ�װ�÷�������Ϣ
#define	MS_TIMER_SYNC_REQ	0x0023	//07��04a:ʱ��ͬ�����󣬼����������Ϣ
#define	MS_TIMER_SYNC_ACK	0x0123	//08��04b:ʱ��ͬ��ȷ�ϣ�װ�÷�������Ϣ
#define	MS_SYNC_TIMER_REQ	0x0124	//09��05b:����ͬ������װ�û�������Ϣ
#define	MS_SYNC_TIMER_ACK	0x0024	//10��05a:ʱ��ͬ��Ӧ�𣬼����������Ϣ
#define	MS_GPSTIME_SY_REQ	0x	//11��05b:����GPST����װ�û�������Ϣ
#define	MS_GPSTIME_SY_ACK	0x	//12��05a:GPSTͬ��ȷ�ϣ������������Ϣ
#define	MS_DEVICE_LED_REQ		0x0062	//13��06a:��������ѯ�������������Ϣ
#define	MS_DEVICE_LED_ACK		0x0162	//14��06b:�������Ӧ��װ�÷�������Ϣ
#define	MS_SAVE_HDISK_REQ	0x0063	//15��07a:�����������󣬼����������Ϣ
#define	MS_SAVE_HDISK_ACK		0x0163	//16��07b:��������ȷ�ϣ�װ�÷�������Ϣ
#define	MS_DEVI_FAULT_REP		0x0164	//17��08b:װ�ù����ϱ���װ�÷�������Ϣ
#define	MS_DEVI_FAULT_ACK		0x0064	//18��08a:�����ϱ�ȷ�ϣ������������Ϣ
#define	MS_DEVI_ALARM_REQ	0x0165	//19��09a:װ�ø澯�ϱ��������������Ϣ
#define	MS_DEVI_ALARM_ACK	0x0065	//20��09b:�澯�ϱ�ȷ�ϣ������������Ϣ
#define	MS_NET_STATUS_REQ	0x	//21��10a:����״̬���󣬼����������Ϣ!
#define	MS_NET_STATUS_ACK	0x	//22��10b:����״̬Ӧ��װ�÷�������Ϣ!
#define	MS_NET_COUNTE_REQ	0x0066	//23��11a:����ͳ�Ʋ�ѯ�������������Ϣ
#define	MS_CFG_COUNTE_ACK	0x0166	//24��11b:����ͳ��Ӧ��װ�÷�������Ϣ
#define	MS_SEND_SMVMS_REQ	0x0067	//25��12a:����SMV�ϱ��� �����������Ϣ
#define	MS_SEND_SMVMS_ACK	0x0167	//26��12b:ȷ��SMV�ϱ��� װ�÷�������Ϣ
#define	MS_STARTS_SMV_ACK	0x0168	//27��12c:��ʼSMV�ϱ��� װ�÷�������Ϣ
#define	MS_STOP_SMVMS_REQ	0x0069	//28��12d:��ֹSMV�ϱ��� �����������Ϣ
#define	MS_STOP_SMVMS_ACK	0x0169	//29��12e:ȷ��SMV��ֹ�� װ�÷�������Ϣ

#if 0
//װ�����̨�����ͨ�ų������Ϣ�ṹ
typedef struct Ty2012sCommand			//װ�����̨�����ͨ�ų����һ����Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣ֡ͷ4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ���ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ����
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevCommand;

typedef struct Ty3012sResponse			//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ��Ӧ
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevResponse;

//MS_ROUTE_TEST_REQ:·�ɲ������󣬼����������Ϣ��������յ�����
//MS_ROUTE_TEST_ACK:·�ɲ���ȷ�ϣ�װ�÷�������Ϣ���������ظ�ȷ��
typedef struct Ty3012sRouteAck			//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ��Ӧ=MS_ROUTE_TEST_ACK!
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevRouteAck;

//MS_HEART_BEAT_REQ:������������װ�÷�������Ϣ����ʱ���͸��������ö�ʱ���߳�
//MS_HEART_BEAT_ACK:��������ȷ�ϣ������������Ϣ��װ�ò�������Ϣ
typedef struct Ty3012sHeartBeatReq		//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ����=MS_HEART_BEAT_REQ!
	unsigned short	MsgHeart;		//��Ϣ����2�ֽڣ�Ŀǰ��0!
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevHeartBeatReq;

//MS_RESET_DEVI_REQ:װ�ø�λ���󣬼����������Ϣ��ֹͣ���еĽ��̣����н����˳�����Ҫ��������!
//MS_RESET_DEVI_ACK:װ�ø�λȷ�ϣ�װ�÷�������Ϣ��Ҫ�ظ���λȷ�ϣ�ϵͳ���и�λ
typedef struct Ty3012sResetAck			//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣȷ��=MS_RESET_DEVI_ACK!
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevResetAck;

//MS_TIMER_SYNC_REQ:ʱ��ͬ�����󣬼����������Ϣ
//MS_TIMER_SYNC_ACK:ʱ��ͬ��ȷ�ϣ�װ�÷�������Ϣ
//MS_SYNC_TIMER_REQ:����ͬ������װ�û�������Ϣ
//MS_SYNC_TIMER_ACK:ʱ��ͬ��Ӧ�𣬼����������Ϣ
//MS_GPSTIME_SY_REQ:����GPST����װ�û�������Ϣ
//MS_GPSTIME_SY_ACK:ʱ��ͬ��ȷ�ϣ������������Ϣ
typedef struct Ty3012sTimeSyncReq		//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ����=MS_TIMER_SYNC_REQ!
	unsigned short	MsgYear;		//��Ϣ����2�ֽڣ���!
	unsigned short	MsgMonthDay;		//��Ϣ����2�ֽڣ��º���!
	unsigned short	MsgHourMinite;		//��Ϣ����2�ֽڣ�ʱ�ͷ�!
	unsigned short	MsgSecondMilli;		//��Ϣ����2�ֽڣ���ͺ���!
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevTimeSyncReq;

//MS_DEVICE_LED_REQ:��������ѯ�������������Ϣ
//MS_DEVICE_LED_RES:�������Ӧ��װ�÷�������Ϣ
typedef struct Ty3012sLedsRes			//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢӦ��=MS_DEVICE_LED_RES!
	unsigned short	MsgLed1;		//��Ϣ����2�ֽڣ���һ��LED!
	unsigned short	MsgLed2;		//��Ϣ����2�ֽڣ��ڶ���LED!
	unsigned short	MsgLed3;		//��Ϣ����2�ֽڣ�������LED!
	unsigned short	MsgLed4;		//��Ϣ����2�ֽڣ�������LED!
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevLedsRes;

//MS_SAVE_HDISK_REQ:�����������󣬼����������Ϣ
//MS_SAVE_HDISK_ACK:��������ȷ�ϣ�װ�÷�������Ϣ
typedef struct Ty3012sSaveHdiskReq		//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ����=MS_SAVE_HDISK_REQ!
	unsigned short	MsgChannel;		//��Ϣ����2�ֽڣ�ͨ����!
	unsigned short	MsgParamete;		//��Ϣ����2�ֽڣ����ò�����Ŀǰ��0!
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevSaveHdiskReq;

//MS_DEVI_FAULT_REQ:װ�ù����ϱ���װ�÷�������Ϣ
//MS_DEVI_FAULT_ACK:�����ϱ�ȷ�ϣ������������Ϣ
typedef struct Ty3012sFaultReport		//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ����=MS_DEVI_FAULT_REQ!
	unsigned short	MsgAckTagh;		//��Ϣ����2�ֽڣ���ҪӦ���־!
	unsigned short	MsgAckTagl;		//��Ϣ����2�ֽڣ���ҪӦ���־!
	unsigned short	MsgErrorCodeh;		//��Ϣ����2�ֽڣ����ϱ���!ĿǰGPS��?��̫���ӿ���?�����?
	unsigned short	MsgErrorCodel;		//��Ϣ����2�ֽڣ����ϱ���!ĿǰGPS��?��̫���ӿ���?�����?
	unsigned short	MsgDateh;		//��Ϣ����2�ֽڣ�����ʱ��!ʹ��Linuxϵͳʱ��?GPSʱ��?
	unsigned short	MsgDatel;		//��Ϣ����2�ֽڣ�����ʱ��!ʹ��Linuxϵͳʱ��?GPSʱ��?
	unsigned short	MsgTimeh;		//��Ϣ����2�ֽڣ�����ʱ��!ʹ��Linuxϵͳʱ��?GPSʱ��?
	unsigned short	MsgTimel;		//��Ϣ����2�ֽڣ�����ʱ��!ʹ��Linuxϵͳʱ��?GPSʱ��?
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevFaultReport;

//MS_DEVI_ALARM_REQ:�澯�ϱ����󣬼����������Ϣ
//MS_DEVI_ALARM_ACK:�澯�ϱ�ȷ�ϣ������������Ϣ
typedef struct Ty3012sAlarmReport		//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ����=MS_DEVI_FAULT_REQ!
	unsigned short	MsgAckTagh;		//��Ϣ����2�ֽڣ���ҪӦ���־!
	unsigned short	MsgAckTagl;		//��Ϣ����2�ֽڣ���ҪӦ���־!
	unsigned short	MsgIedMac0;		//��Ϣ����2�ֽڣ���ҪӦ��IED��MAC��ַ��0000!
	unsigned short	MsgIedMac1;		//��Ϣ����2�ֽڣ���ҪӦ��IED��MAC��ַ����1�͵�2��Mac��ַ!
	unsigned short	MsgIedMac2;		//��Ϣ����2�ֽڣ���ҪӦ��IED��MAC��ַ����3�͵�4��Mac��ַ!
	unsigned short	MsgIedMac3;		//��Ϣ����2�ֽڣ���ҪӦ��IED��MAC��ַ����5�͵�6��Mac��ַ!
	unsigned short	MsgAlarmType;		//��Ϣ����2�ֽڣ��澯����!ĿǰSMV,GOOSE,MMS,PTP,ETHERNET,NETWORK?
	unsigned short	MsgAlarmCode;		//��Ϣ����2�ֽڣ����ϱ���!Ŀǰ?
	unsigned short	MsgDateh;		//��Ϣ����2�ֽڣ�����ʱ��!ʹ��Linuxϵͳʱ��?GPSʱ��?
	unsigned short	MsgDatel;		//��Ϣ����2�ֽڣ�����ʱ��!ʹ��Linuxϵͳʱ��?GPSʱ��?
	unsigned short	MsgTimeh;		//��Ϣ����2�ֽڣ�����ʱ��!ʹ��Linuxϵͳʱ��?GPSʱ��?
	unsigned short	MsgTimel;		//��Ϣ����2�ֽڣ�����ʱ��!ʹ��Linuxϵͳʱ��?GPSʱ��?
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevAlarmReport;

//MS_NET_STATUS_REQ:����״̬���󣬼����������Ϣ!
//MS_NET_STATUS_ACK:����״̬Ӧ��װ�÷�������Ϣ!
typedef struct Ty3012sStatusReq			//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢӦ��=MS_DEVICE_LED_RES!
	unsigned short	MsgStatus1;		//��Ϣ����2�ֽڣ���һ��״̬����̫���ӿ�!
	unsigned short	MsgStatus2;		//��Ϣ����2�ֽڣ��ڶ���״̬��GPS�ӿ�!
	unsigned short	MsgStatus3;		//��Ϣ����2�ֽڣ�������״̬�����뿪����!
	unsigned short	MsgStatus4;		//��Ϣ����2�ֽڣ�������״̬���������!
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevStatusReq;

//MS_NET_COUNTE_REQ:����ͳ�Ʋ�ѯ�������������Ϣ
//MS_CFG_COUNTE_ACK:����ͳ��Ӧ��װ�÷�������Ϣ
typedef struct Ty3012sCounterRes		//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���ϢӦ��=MS_DEVICE_LED_RES!
	unsigned short	MsgChNo;		//��Ϣ����2�ֽڣ�ͨ��1��ͨ��2
	unsigned long	MsgAllFrame;		//��Ϣ����4�ֽڣ�0:��֡��L
	unsigned long	MsgAllByte;		//��Ϣ����4�ֽڣ�1:���ֽ���L
	unsigned long	MsgBroadcastFrame;	//��Ϣ����4�ֽڣ�2:�㲥֡��L
	unsigned long	MsgBroadcastByte;	//��Ϣ����4�ֽڣ�3:�㲥�ֽ���L
	unsigned long	MsgMulticastFrame;	//��Ϣ����4�ֽڣ�4:�ಥ֡��L
	unsigned long	MsgMulticastByte;	//��Ϣ����4�ֽڣ�5:�ಥ�ֽ���L
	unsigned long	MsgCrcErrFrame;		//��Ϣ����4�ֽڣ�6:CRC����֡��L
	unsigned long	MsgErrFrame;		//��Ϣ����4�ֽڣ�7:����֡��L
	unsigned long	MsgAllignErrFrame;	//��Ϣ����4�ֽڣ�8:��λ����֡��L
	unsigned long	MsgShortFrame;		//��Ϣ����4�ֽڣ�9:����֡��L
	unsigned long	MsgLongFrame;		//��Ϣ����4�ֽڣ�10:����֡��L
	unsigned long	MsgAllFrameH;		//��Ϣ����4�ֽڣ�0:��֡��H
	unsigned long	MsgAllByteH;		//��Ϣ����4�ֽڣ�1:���ֽ���H
	unsigned long	MsgBroadcastFrameH;	//��Ϣ����4�ֽڣ�2:�㲥֡��H
	unsigned long	MsgBroadcastByteH;	//��Ϣ����4�ֽڣ�3:�㲥�ֽ���H
	unsigned long	MsgMulticastFrameH;	//��Ϣ����4�ֽڣ�4:�ಥ֡��H
	unsigned long	MsgMulticastByteH;	//��Ϣ����4�ֽڣ�5:�ಥ�ֽ���H
	unsigned long	MsgCrcErrFrameH;	//��Ϣ����4�ֽڣ�6:CRC����֡��H
	unsigned long	MsgErrFrameH;		//��Ϣ����4�ֽڣ�7:����֡��H
	unsigned long	MsgAllignErrFrameH;	//��Ϣ����4�ֽڣ�8:��λ����֡��H
	unsigned long	MsgShortFrameH;		//��Ϣ����4�ֽڣ�9:����֡��H
	unsigned long	MsgLongFrameH;		//��Ϣ����4�ֽڣ�10:����֡��H	
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevCounterRes;

//MS_SEND_SMVMS_REQ:����SMV�ϱ��� �����������Ϣ
//MS_SEND_SMVMS_ACK:ȷ��SMV�ϱ��� װ�÷�������Ϣ
//MS_STARTS_SMV_ACK:��ʼSMV�ϱ��� װ�÷�������Ϣ
//MS_STOP_SMVMS_REQ:��ֹSMV�ϱ��� �����������Ϣ
//MS_STOP_SMVMS_ACK:ȷ��SMV��ֹ�� װ�÷�������Ϣ
typedef struct Ty3012sSmvReq			//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ����=MS_DEVI_FAULT_REQ!
	unsigned short	MsgIedMac0;		//��Ϣ����2�ֽڣ���ҪӦ��IED��MAC��ַ����1�͵�2��Mac��ַ!
	unsigned short	MsgIedMac1;		//��Ϣ����2�ֽڣ���ҪӦ��IED��MAC��ַ����3�͵�4��Mac��ַ!
	unsigned short	MsgIedMac2;		//��Ϣ����2�ֽڣ���ҪӦ��IED��MAC��ַ����5�͵�6��Mac��ַ!
	unsigned short	MsgChannel;		//��Ϣ����2�ֽڣ�ͨ���ţ�
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevSmvReq;

typedef struct Ty3012sSmvAck			//װ�����̨�����ͨ�ų������Ӧ��Ϣ��ʽ
{						//��Ϣͷ��18�ֽڣ���������Ϣβ������14�ֽ�
	unsigned int	MsgHead;		//��Ϣͷ��4�ֽڣ�0xF6F62828
	unsigned short	MsgLength;		//��Ϣ����2�ֽڣ�������֮��β����֮ǰ�ֽڣ�����8�ֽ�
	unsigned short	MsgSession1;		//��Ϣ�Ự2�ֽڣ���һ�ֽ�
	unsigned short	MsgSession2;		//��Ϣ�Ự2�ֽڣ��ڶ��ֽڣ��ͻ��˷���0���������˼�1���ͻ�ȥ
	unsigned short	MsgReserve;		//�����ֽ�2�ֽڣ������ֽ���Ϊ�˱�֤��Ϣ��չ��Ŀǰ����Ϊ0
	unsigned short	MsgId;			//��Ϣ����2�ֽڣ���Ϣ����=MS_DEVI_FAULT_REQ!
	unsigned short	MsgSmvAckh;		//��Ϣ����2�ֽڣ���ҪӦ��״̬!1=������������(Ʃ��û��Ҫ���Mac),2=�ܾ�����
//	unsigned short	MsgSmvAckh;		//��Ϣ����2�ֽڣ���ҪӦ��״̬!
	unsigned short	MsgTail1;		//��Ϣβ��2�ֽڣ���һ�ֽڣ�0x0909
	unsigned short	MsgTail2;		//��Ϣβ��2�ֽڣ��ڶ��ֽڣ�0xD7D7
}DevSmvAck;

#endif

#if 0
//����װ��������Ϣ
#define SET_CH_INTERFACE	0x01		//����ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define SET_TRIGGER_TIME	0x02		//����ͨ���ⴥ������ʱ�䣬��λus
#define SET_GPS_INTERFACE	0x03		//����GPS�ӿ����ͣ�TTL��485�ӿ�
#define SET_GPS_TIME		0x04		//����GPSʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define SET_CH_FILTER		0x05		//����ͨ���Ĺ�����������������
//����������ϢӦ��
#define ACK_CH_INTERFACE	0x81		//Ӧ������ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define ACK_TRIGGER_TIME	0x82		//Ӧ������ͨ���ⴥ������ʱ�䣬��λus
#define ACK_GPS_INTERFACE	0x83		//Ӧ������GPS�ӿ����ͣ�TTL��485�ӿ�
#define ACK_GPS_TIME		0x84		//Ӧ������GPSʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define ACK_CH_FILT		0x85		//Ӧ������ͨ���Ĺ�������
//����״̬��ѯ��Ϣ
#define CHECK_CH_BUFFER		0x11		//��黺��FIFO�е����ݣ�ÿ�μ��128�ֽ�
#define CHECK_CH_INTERFACE	0x12		//���ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define CHECK_OPTICAL_STATUS	0x13		//���ͨ����ӿڵ�״̬���й���޹��źţ��Ա�ȷ���Ƿ���źŶ�ʧ
#define CHECK_LINK_SPEED	0x14		//���ͨ�����ӿڵ�����״̬�����ӡ��ٶȡ�˫��
#define CHECK_GPS_TIMER		0x15		//���GPSʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define CHECK_GPS_STATUS	0x16		//���GPS�ӿڵĸ澯״̬����������ʧ������
#define CHECK_TRIGGER_STATUS	0x17		//���ͨ���ⴥ��״̬�����޴������Ƿ����ڴ���
#define CHECK_CH_FILTER		0x18		//���ͨ���Ĺ�������
#define CHECK_FRAME_COUNTERS	0x19		//���֡���ֽڼ�������ͳ������
//�����ѯ��ϢӦ��
#define ACK_CH_BUFFER		0x91		//Ӧ���黺��FIFO�е����ݣ�ÿ��Ӧ��128�ֽ�
#define ACK_CH_STATUS		0x92		//Ӧ����ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define ACK_OPTICAL_STATUS	0x93		//Ӧ����ͨ����ӿڵ�״̬���й���޹��ź�
#define ACK_LINK_SPEED		0x94		//Ӧ����ͨ�����ӿڵ�����״̬�����ӡ��ٶȡ�˫��
#define ACK_GPS_TIMER		0x95		//Ӧ����GPSʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define ACK_GPS_STATUS		0x96		//Ӧ����GPS�澯״̬����������ʧ������
#define ACK_TRIGGER_STATUS	0x97		//Ӧ����ͨ���ⴥ��״̬�����޴������Ƿ����ڴ���
#define ACK_CH_FILTER		0x98		//Ӧ����ͨ���Ĺ�������
#define ACK_FRAME_COUNTERS	0x99		//Ӧ����֡���ֽڼ�������ͳ������
//��������ѯ��Ϣ
#define CHECK_LED_STATUS	0x21		//���ǰ���LED״̬���ܹ�36��LED
//��������ѯӦ��
#define ACK_LED_STATUS		0xa1		//Ӧ��ǰ���LED״̬���ܹ�36��LED
#endif

#define	MAX_LINK_TYPES	4			/*TCP&&UDP����������*/

#define	RESPONSE_CMD_RSP(Res, Rsp, Sockfd, Buff)		\
do \
{ \
	Res->header.head = Htons(0xF6F62828); \
	Res->header.length = Htons(0x0006); \
	Res->header.msgId = Htons(Rsp); \
	MsgSession++; \
	Res->header.session1 = Htons(MsgSession & 0x0ffff); \
	Res->header.session2 = Htons(MsgSession / 0x10000); \
	Res->tail1 = Htons(0x0909); \
	Res->tail2 = Htons(0xD7D7); \
	if (write(Sockfd, Buff, sizeof(CommonMsg)) <= 0) \
	{ \
		close(Sockfd); \
		printf("TY-3012S CMD_RSP write ClientXmlSockfd error: %d!rsp:%d\n", Sockfd, Rsp); \
		break; \
	} \
}while(0)


#define 	RESPONSE_SMV_STOP(Res, Sockfd, Buff)		RESPONSE_CMD_RSP(Res, MS_STOP_SMVMS_ACK, Sockfd, Buff)
#define 	RESPONSE_UPLOAD_STOP(Res, Sockfd, Buff)	RESPONSE_CMD_RSP(Res, MS_FILETRANS_FILE_UPLOAD_CANCEL_ACK, Sockfd, Buff)
#define	RESPONSE_ROUTE_ACK(Res, Sockfd, Buff)		RESPONSE_CMD_RSP(Res, MS_ROUTE_TEST_ACK, Sockfd, Buff)
#define	RESPONES_RESET_ACK(Res, Sockfd, Buff)		RESPONSE_CMD_RSP(Res, MS_RESET_DEVI_REQ, Sockfd, Buff)
#define	RESPONES_TIMESYNC_ACK(Res, Sockfd, Buff)	RESPONSE_CMD_RSP(Res, MS_TIMER_SYNC_ACK, Sockfd, Buff)
#define	RESPONES_SAVEFILE_ACK(Res, Sockfd, Buff)	RESPONSE_CMD_RSP(Res, MS_SAVE_HDISK_ACK, Sockfd, Buff)

#define	RESPONSE_SMV_ACK(Res, Sockfd, Buff, Ack)		\
do \
{ \
	Res->header.head = Htons(0xF6F62828); \
	Res->header.length = Htons(0x000a); \
	Res->header.msgId = Htons(MS_SEND_SMVMS_ACK); \
	MsgSession++; \
	Res->header.session1 = Htons(MsgSession & 0x0ffff); \
	Res->header.session2 = Htons(MsgSession / 0x10000); \
	Res->ACKParam = Ack; \
	Res->tail1 = Htons(0x0909); \
	Res->tail2 = Htons(0xD7D7); \
	if (write(Sockfd, Buff, sizeof(SmvSendResponse)) <= 0) \
	{ \
		close(Sockfd); \
		printf("TY-3012S SMV_ACK write ClientXmlSockfd error: %d!\n", Sockfd); \
		break; \
	} \
}while(0)

#define	RESPONSE_NET_COUNT(Res, len, Sockfd, Buff)		\
do \
{ \
	Res->header.head = Htons(0xF6F62828); \
	Res->header.length = Htons(len); \
	Res->header.msgId = Htons(MS_CFG_COUNTE_ACK); \
	MsgSession++; \
	Res->header.session1 = Htons(MsgSession & 0x0ffff); \
	Res->header.session2 = Htons(MsgSession / 0x10000); \
	Res->tail1 = Htons(0x0909); \
	Res->tail2 = Htons(0xD7D7); \
	if (write(Sockfd, Buff, sizeof(NetCountRsp)) <= 0) \
	{ \
		close(Sockfd); \
		printf("TY-3012S DEVICE_LED_ACK write ClientSockfd error: %d!\n", Sockfd); \
		break; \
	} \
}while(0)


#define	RESPONSE_DEVICE_LED_ACK(Res, Sockfd, Buff, LedParam)		\
do \
{ \
	Res->header.head = Htons(0xF6F62828); \
	Res->header.length = Htons(0x000e); \
	Res->header.msgId = Htons(MS_DEVICE_LED_ACK); \
	MsgSession++; \
	Res->header.session1 = Htons(MsgSession & 0x0ffff); \
	Res->header.session2 = Htons(MsgSession / 0x10000); \
	memcpy(Res->dataTime, LedParam, 8); \
	Res->tail1 = Htons(0x0909); \
	Res->tail2 = Htons(0xD7D7); \
	if (write(Sockfd, Buff, sizeof(DeviceLedRsp)) <= 0) \
	{ \
		close(Sockfd); \
		printf("TY-3012S DEVICE_LED_ACK write ClientSockfd error: %d!\n", Sockfd); \
		break; \
	} \
}while(0)

#define	RESPONSE_CONNECT_FAILED(Res, Sockfd, Buff, Reason)	\
do \
{ \
	Res->header.head = Htons(0xF6F62828); \
	Res->header.length = Htons(0x000a); \
	Res->header.msgId = Htons(MS_FILETRANS_CONNECT_RSP); \
	MsgSession++; \
	Res->header.session1 = Htons(MsgSession & 0x0ffff); \
	Res->header.session2 = Htons(MsgSession / 0x10000); \
	Res->result = 0; \
	Res->reason = Reason; \
	Res->tail1 = Htons(0x0909); \
	Res->tail2 = Htons(0xD7D7); \
	if (write(Sockfd, Buff, sizeof(RecConnResponse)) <= 0) \
	{ \
		close(Sockfd); \
		printf("TY-3012S CONNECT_FAILED write ClientXmlSockfd error: %d!\n", Sockfd); \
		break; \
	} \
}while(0)

#define	RESPONSE_CONNECT_OK(Res, Sockfd, Buff)		\
do \
{ \
	Res->header.head = Htons(0xF6F62828); \
	Res->header.length = Htons(0x000a); \
	Res->header.msgId = Htons(MS_FILETRANS_CONNECT_RSP); \
	MsgSession++; \
	Res->header.session1 = Htons(MsgSession & 0x0ffff); \
	Res->header.session2 = Htons(MsgSession / 0x10000); \
	Res->result = 1; \
	Res->reason = 0; \
	Res->tail1 = Htons(0x0909); \
	Res->tail2 = Htons(0xD7D7); \
	if (write(Sockfd, Buff, sizeof(RecConnResponse)) <= 0) \
	{ \
		close(Sockfd); \
		printf("TY-3012S CONNECT_OK write ClientXmlSockfd error: %d!\n", Sockfd); \
		break; \
	} \
}while(0)

#define	BS_SV_NEED_TO_RETURN(Qid, info) \
do \
{ \
	if (msgctl(Qid, IPC_STAT, &info) < 0) \
	{ \
		PLH_DEBUG("SvUploadQid msgctl failed, errno:%d, %s, ---lineNo:%d\n", errno, strerror(errno), __LINE__); \
	} \
	if (info.msg_cbytes == 0) \
	{ \
		return 0; \
	} \
}while(0)


/*SV�ϴ�����*/
#define	MAX_UPLOAD_NUM			50				/*һ����Ϣ�ϴ�sv�������*/
#define	TYPE_SVUPLOAD				6				/*��Ϣ���н�����Ϣ������,sv�ϴ�Ϊ6*/
#define	MSG_RQST_SVUPLOAD 		1				/*��Ϣ����sv�ϴ�*/
#define	MSG_RQST_SVUPLOAD_STOP 	2				/*��Ϣ����ֹͣ�ϴ�*/

typedef struct
{
	long mtype;									/*������Ϣ����*/
	unsigned char request;							/*��������*/
	unsigned char chNo;							/*ͨ����*/
	unsigned char reverse[2];						/*����*/
	unsigned char *packet[MAX_UPLOAD_NUM];		/*֡ͷָ��,һ������ϴ�10��*/
	unsigned long length[MAX_UPLOAD_NUM];			/*ÿ֡�ĳ���*/
	unsigned int count;							/*��Ҫ�ϴ��ĸ���,������10��*/
}MsgSvInfo;									/*Sv��Ϣ*/


#if 0
#define	MSGHEAD		0x1a2b3c4d	//��Ϣͷ��4�ֽڣ�1a2b3c4d
#define	MSGSESSIONID	0x0		//��Ϣ�Ự4�ֽڣ��������˷���0���ͻ��˼�1���ͻ�ȥ
#define	MSGDEVICEID	0x0		//��Ϣװ��4�ֽڣ��������͵�װ��ר�ã�����Ϊ0
#define	MSGVERSION	0x1		//��Ϣ�汾4�ֽڣ���Ϣ�İ汾�ţ�����Ϊ1.0�汾
#define	MSGID		0x0		//��Ϣ����4�ֽڣ��ն���Ϊ0
#define	MSGCHNO1	0x1		//��Ϣͨ��4�ֽڣ�ͨ��1��ֵ=1
#define	MSGCHNO2	0x2		//��Ϣͨ��4�ֽڣ�ͨ��2��ֵ=2
#define	MSGTAIL		0xa1b2c3d4	//��Ϣβ��4�ֽڣ�a1b2c3d4

#define	EINTERFACE	0x00		//�ӿ�����4�ֽڣ���̫����ӿ�=0
#define	OINTERFACE	0x01		//�ӿ�����4�ֽڣ���̫����ӿ�=1

#define	TRIGERTIME	0x55		//ͨ���ⴥ������ʱ�䣬��λus����С0�����256

#define	GPSTTLINTERFACE	0x0		//�ӿ�����4�ֽڣ�GPS�ӿ����ͣ�TTL��485�ӿڣ�0=TTL��1=485
#define	GPS485INTERFACE	0x1		//�ӿ�����4�ֽڣ�GPS�ӿ����ͣ�TTL��485�ӿڣ�0=TTL��1=485

#define	GPSYEAR		2011		//GPSʱ�䣬��
#define	GPSMONTH	5		//GPSʱ�䣬��
#define	GPSDAY		18		//GPSʱ�䣬��
#define	GPSHOUR		16		//GPSʱ�䣬ʱ
#define	GPSMINIT	36		//GPSʱ�䣬��
#define	GPSSECOND	0		//GPSʱ�䣬��
#define	GPSMILLISECOND	0		//GPSʱ�䣬����
#define	GPSMICROSECOND	0		//GPSʱ�䣬΢��

#define	CHRESET		0x01		//��λ���ƣ�0��Ҫ��λ��1Ҫ��λ
#define	CHMODE		0x01		//����ģʽ��1�����ˣ�0����
#define	MINLENGTH	64		//��С֡����������64�ֽ�
#define	MAXLENGTH	1518		//���֡����������1518�ֽ�
#define	VLAN1ENABLE	0x01		//Vlan1ʹ�ܣ�D0=1ʹ��
#define	VLAN2ENABLE	0x02		//Vlan2ʹ�ܣ�D1=1ʹ��
#define	GOOSEENABLE	0x04		//Gooseʹ�ܣ�D2=1ʹ��
#define	SMVENABLE	0x08		//Smv  ʹ�ܣ�D3=1ʹ��
#define	MMSENABLE	0x10		//Mms  ʹ�ܣ�D4=1ʹ��
#define	VLAN1ID		0x0355		//Vlan1��ǩ
#define	VLAN2ID		0x03aa		//Vlan2��ǩ
#define	MACENABLE	0x55		//Mac  ʹ�ܣ�D0��D15=1ʹ��
#endif

#define	LOCALPORT	5678		//������udp�˿ڣ��Ժ�����idprom�ļ�
//#define	REMOTEIP	"127.0.0.1"	//������ip��ַ��ʹ���ڲ���ַ
//#define	MSGMAXLEN	0x100		//��Ϣ����󳤶ȣ�Ӧ�ô��͸�������������������ݳ���256�ֽ�

/*interface list*/
int bs_tcpserverInit(pthread_t *id1, pthread_t *id2);

#endif
