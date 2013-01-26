#ifndef __XML_HANDLER__
#define __XML_HANDLER__

#include <libxml/xmlmemory.h>	//�����ӵĿ⺯��
#include <libxml/parser.h>	//�����ӵĿ⺯��



#define XML_RECORD_FILE1A	"/sda/diskA/fileRecord.xml"
#define XML_RECORD_FILE1C	"/sda/diskC/fileRecord.xml"
#define XML_RECORD_FILE2A	"/sdb/diskA/fileRecord.xml"
#define XML_RECORD_FILE2C	"/sdb/diskC/fileRecord.xml"

#define XML_CONFIGFILE		"/root/hp/DeviceConfig.xml"

#define DISKA_STRING		"DiskA"
#define DISKB_STRING		"DiskB"
#define DISKC_STRING		"DiskC"
#define DISKD_STRING		"DiskD"
#define DISKE_STRING		"DiskE"
#define DISKF_STRING		"DiskF"

#define nMAC				16	//����ÿ��ͨ����Mac����Դ��ַ
#define nVLAN				2	//����ÿ��ͨ����Vlan���˵�ַ
#define nIED					16	//ÿ̨װ��ÿ��ͨ�����Բ���IED��������Ŀǰ��2Uװ��ÿ��ͨ�����Բ���16��IED
#define nCHANNEL			2	//ÿ̨װ�õ�ͨ��������Ŀǰ��2Uװ����2��ͨ��
#define NAMELEN				32	//���Ƶȵ��ַ�������

//˵��:
//Device���Dev,Version���Ver,LocalIp(LocIp)���LoIp,SubnetMask���SnMask,Gateway���Gway
//RemoteIp���ReIp,RemotePort���RePort,SubNo���SubN,SubName���Sname,ChannelNum���ChNum
//FileFormat���DevFileF,FileSavedPeriod���FileP,FileMaxSize���FileS
//HarddiskCheckedPercentage���Hdisk,TimeZone���DevTimeZ,HeartBeatPeriod���HeartBeatP
//GpsInterface���GpsIf,ChannelNo���ChNo,ChannelInterfaceType���ChIfType
//ChannelTriggerTime���ChTrtime,ChannelCapturePacket���ChPacket
//ChannelMinFrameLen���ChMinLen,ChannelMaxFrameLen���ChMaxLen
//ChannelVlanNo���ChVlanNo,ChannelVlanId���ChVlanId,ChannelMacNo���ChMacNo
//ChannelMacAddr���ChMacAdd,ChannelPacketType���ChMacPkt
//ChannelIedName���ChIedName,ChannelIedMac���ChIedMac,ChannelIedType���ChIedType
//ChannelIedBay���ChIedBay,ChannelIedManufacturer���ChIedMan
//ChannelIedDesc���ChIedDesc,ChIedBayIp���ChIedBip
//ChannelIedMuType���ChMuType,ChannelIedMuDestMac���ChIedMuDmac,ChannelIedMuSourceMac���ChMuSmac
//ChannelIedMuSmpRate���ChMuSmpR,ChannelIedMuSmpDeviation���ChMuSmpD
//ChannelIedMuVlanId���ChMuVlanI,ChannelIedMuPriority���ChMuPrio
//ChannelIedMuAppId���ChMuApId,ChannelIedMuSmvId���ChMuSvId
//ChannelIedMuDatasetRef���ChMuDref,ChannelIedMuChannelDesc���ChMuDesc
//
//
#if 0
//����װ��������Ϣ
#define SET_CH_INTERFACE		0x01		//01������ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
//#define SET_TRIGGER_TIME		0x02		//02������ͨ���ⴥ������ʱ�䣬��λus
//#define SET_GPS_INTERFACE		0x03		//03������GPS�ӿ����ͣ�TTL��485�ӿ�
//#define SET_TRIGGER_TIME			0x04		//04������GPSʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
//#define SET_CH_FILTER			0x05		//05������ͨ���Ĺ�����������������
//����������ϢӦ��
#define ACK_CH_INTERFACE		0x81		//01��Ӧ������ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
//#define ACK_TRIGGER_TIME		0x82		//02��Ӧ������ͨ���ⴥ������ʱ�䣬��λus
//#define ACK_GPS_INTERFACE		0x83		//03��Ӧ������GPS�ӿ����ͣ�TTL��485�ӿ�
#define ACK_GPS_TIME			0x84		//04��Ӧ������GPSʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define ACK_CH_FILT				0x85		//05��Ӧ������ͨ���Ĺ�������
//����״̬��ѯ��Ϣ
#define READ_CH_BUFF			0x10		//06����ȡ����FIFO�е����ݣ�ÿ�����2M�ֽ�
#define CHECK_CH_BUFFER		0x11		//06����黺��FIFO�е����ݣ�ÿ�μ��128�ֽ�
#define CHECK_CH_INTERFACE		0x12		//07�����ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define CHECK_OPTICAL_STATUS	0x13		//08�����ͨ����ӿڵ�״̬���й���޹��źţ��Ա�ȷ���Ƿ���źŶ�ʧ
#define CHECK_LINK_SPEED		0x14		//09�����ͨ�����ӿڵ�����״̬�����ӡ��ٶȡ�˫��
#define CHECK_GPS_TIMER		0x15		//10�����GPSʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define CHECK_GPS_STATUS		0x16		//11�����GPS�ӿڵĸ澯״̬����������ʧ������
//#define CHECK_TRIGGER_STATUS	0x17		//12�����ͨ���ⴥ��״̬�����޴������Ƿ����ڴ���
//#define CHECK_CH_FILTER		0x18		//13�����ͨ���Ĺ�������
#define CHECK_FRAME_COUNTERS	0x19		//14�����֡���ֽڼ�������ͳ������
//�����ѯ��ϢӦ��
#define ACK_CH_BUFFER			0x91		//06��Ӧ���黺��FIFO�е����ݣ�ÿ��Ӧ��128�ֽ�
#define ACK_CH_STATUS			0x92		//07��Ӧ����ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define ACK_OPTICAL_STATUS		0x93		//08��Ӧ����ͨ����ӿڵ�״̬���й���޹��ź�
#define ACK_LINK_SPEED			0x94		//09��Ӧ����ͨ�����ӿڵ�����״̬�����ӡ��ٶȡ�˫��
#define ACK_GPS_TIMER			0x95		//10��Ӧ����GPSʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define ACK_GPS_STATUS			0x96		//11��Ӧ����GPS�澯״̬����������ʧ������
#define ACK_TRIGGER_STATUS	0x97		//12��Ӧ����ͨ���ⴥ��״̬�����޴������Ƿ����ڴ���
#define ACK_CH_FILTER			0x98		//13��Ӧ����ͨ���Ĺ�������
#define ACK_FRAME_COUNTERS	0x99		//14��Ӧ����֡���ֽڼ�������ͳ������
//��������ѯ��Ϣ
#define CHECK_LED_STATUS		0x21		//15�����ǰ���LED״̬���ܹ�36��LED
//��������ѯӦ��
#define ACK_LED_STATUS			0xa1		//15��Ӧ��ǰ���LED״̬���ܹ�36��LED

#endif

#define	MSGHEAD				0x1a2b3c4d		//��Ϣͷ��4�ֽڣ�1a2b3c4d
#define	MSGSESSIONID			0x0			//��Ϣ�Ự4�ֽڣ��������˷���0���ͻ��˼�1���ͻ�ȥ
#define	MSGDEVICEID			0x0			//��Ϣװ��4�ֽڣ��������͵�װ��ר�ã�����Ϊ0
#define	MSGVERSION			0x1			//��Ϣ�汾4�ֽڣ���Ϣ�İ汾�ţ�����Ϊ1.0�汾
#define	MSGID					0x0			//��Ϣ����4�ֽڣ��ն���Ϊ0
#define	MSGCHNO1				0x1			//��Ϣͨ��4�ֽڣ�ͨ��1��ֵ=1
#define	MSGCHNO2				0x2			//��Ϣͨ��4�ֽڣ�ͨ��2��ֵ=2
#define	MSGTAIL				0xa1b2c3d4		//��Ϣβ��4�ֽڣ�a1b2c3d4

#define	EINTERFACE				0x00			//�ӿ�����4�ֽڣ���̫����ӿ�=0
#define	OINTERFACE				0x01			//�ӿ�����4�ֽڣ���̫����ӿ�=1
#define	TRIGERTIME				0x5a			//ͨ���ⴥ������ʱ�䣬��λus����С0�����256
#define	GPSTTLINTERFACE		0x0			//�ӿ�����4�ֽڣ�GPS�ӿ����ͣ�TTL��485�ӿڣ�0=TTL��1=485
#define	GPS485INTERFACE		0x1			//�ӿ�����4�ֽڣ�GPS�ӿ����ͣ�TTL��485�ӿڣ�0=TTL��1=485

#define	GPSYEAR				2011			//GPSʱ�䣬��
#if 0
#define	GPSMONTH				11			//GPSʱ�䣬��
#define	GPSDAY					07			//GPSʱ�䣬��
#define	GPSHOUR				10			//GPSʱ�䣬ʱ
#define	GPSMINIT				30			//GPSʱ�䣬��
#endif

#define	GPSSECOND				0			//GPSʱ�䣬��
#define	GPSMILLISECOND		0			//GPSʱ�䣬����
#define	GPSMICROSECOND		0			//GPSʱ�䣬΢��

#define	CHRESET				0x01			//��λ���ƣ�0��Ҫ��λ��1Ҫ��λ
#define	CHMODE					0x01			//����ģʽ��1�����ˣ�0����
#define	MINLENGTH				64			//��С֡����������64�ֽ�
#define	MAXLENGTH				1518			//���֡����������1518�ֽ�
#define	VLAN1ENABLE			0x01			//Vlan1ʹ�ܣ�D0=1ʹ��
#define	VLAN2ENABLE			0x02			//Vlan2ʹ�ܣ�D1=1ʹ��
#define	GOOSEENABLE			0x04			//Gooseʹ�ܣ�D2=1ʹ��
#define	SMVENABLE				0x08			//Smv  ʹ�ܣ�D3=1ʹ��
#define	MMSENABLE				0x10			//Mms  ʹ�ܣ�D4=1ʹ��
#define	VLAN1ID					0x0255			//Vlan1��ǩ
#define	VLAN2ID					0x01aa			//Vlan2��ǩ
#define	MACENABLE				0x55			//Mac  ʹ�ܣ�D0��D15=1ʹ��


//#define MAXRECORDS		3400		//¼���ļ�������
#define PATITION_NUMS			6			//��������
#define MAXFILES					400			//ÿ����������ļ�����
#define NODE_DISKA				0
#define NODE_DISKB				1
#define NODE_DISKC				2
#define NODE_DISKD				3
#define NODE_DISKE				4
#define NODE_DISKF				5

typedef struct tagDevice
{
	//1��װ�õ��б��֣����ơ�Ip��ַ����ţ�3����
	char*	DevName;				//װ�����ƣ���¼����װ�õ�����!
	char*	DevIp;					//װ�õ�ip����¼����װ�����ܽӿڵ�ip��ַ!
	char*	DevNo;					//װ�ñ�ţ���¼����װ�õı�ţ�ÿ̨װ�ø�һ�����?
	//2��װ�õĹ������֣��汾�ţ�1����
	char*	DevVer;				//װ�ð汾����¼����װ�ð汾�ţ�xml�ļ��İ汾��!
	//3��װ�õĵ�ַ���֣�Ip��ַ���������롢���ء����ܷ�����Ip��ַ�����ܷ������˿ڣ�5����
	char*	DevLoIp;			//װ�õ�ip����¼����װ�����ܽӿڵ�ip��ַ!
	char*	DevSnMask;			//װ�����룬��¼����װ�����ܽӿ���������!
	char*	DevGway;			//װ�����أ���¼����װ�����ܽӿ�����!
	char*	DevReIp;			//װ��Զ��Ip�����ü����������Ip��ַ��˭���������ļ�!
	char*	DevRePort;			//װ��Զ�˿ڣ����ü�����������˿ںţ�����Ҫ!
	//4��װ�õĲ������֣���š����ơ�ͨ�����������Ĵ洢��ʽ���洢���ڡ������������μ�顢ʱ����GPS�ӿڣ�10����
	char*	DevSubN;			//װ�õ��ӱ�ţ���¼����װ�õı�ţ�ÿ̨װ�ø�һ�����!
	char*	DevSname;			//װ�õ������ƣ���¼����װ�õ�����!
	char*	DevChNum;			//װ��ͨ����������¼����װ����2��ͨ��������2��ͨ��!
	char*	DevFileF;			//װ���ļ���ʽ����¼����װ���ļ��洢��ʽ�������Զ���!
	char*	DevFileP;			//װ�ô洢���ڣ���¼����װ���ļ��洢���ڣ�����600��!
	char*	DevFileS;			//װ���ļ���С����¼����װ���ļ��洢��С��50M!
	char*	DevHdisk;			//װ�ô��̼�飬��¼����װ�ô��̰ٷֱȣ�50��!
	char*	DevTimeZ;			//װ������ʱ������¼����װ��ʱ���ʱ��������ʱ��8!
	char*	DevBeatP;			//װ���������ڣ���¼����װ�úͷ���������ϵ���ڣ�����3��!
	char*	DevGpsIf;			//װ��GPS�ӿڣ� ��¼����װ��GPS�ӿ����ͣ�ÿͨ��1��GPS�ӿڣ�2ͨ����ͬ��0:TTL��1:485��ȱʡΪ0!
	//5��װ�õ�ͨ�����֣�ͨ���š����ӿ����͡�����ʱ�䡢ץ�����ͣ�2��ͨ��8����
	char*	ChNo[nCHANNEL];			//װ��ͨ����ţ�ÿ̨�豸����Կ���2��ͨ����No1Ϊ1ͨ��!
	char*	ChIfType[nCHANNEL];		//װ�ýӿ����ͣ�ÿ������ͨ����2�ֽӿڣ���ӿڻ��ӿڣ�0�磬1�⣬������!
	char*	ChTrTime[nCHANNEL];		//װ������ʱ�䣬ÿ������ͨ����1���ⴥ���ӿڣ��趨���ⴥ���ӿڵ�����ʱ��?Ŀǰû��
	char*	ChPacket[nCHANNEL];		//װ��ץ�����ͣ�ÿ������ͨ������ץȡSMV/GOOSE/MMS���ֱ��ģ���ζ���!?
	char*  	ChAcsLocation[nCHANNEL];	//���������
	//6��װ��ͨ��n���˲���
	char*	ChMinLen[nCHANNEL];		//װ����С֡����������С֡��64�ֽڣ�����С��64�ֽ�!
	char*	ChMaxLen[nCHANNEL];		//װ�����֡�����������֡��1514�ֽ�!
	char*	ChVlanNo[nCHANNEL][nVLAN];	//װ�õ�Vlan1�� ͨ��1��Vlan1�ı�ţ��б����ʹ��Vlan1!
	char*	ChVlanId[nCHANNEL][nVLAN];	//װ��Vlan��ǩ��Vlan1��ֵ/Vlan2��ֵ!
	char*	ChMacNo [nCHANNEL][nMAC];	//װ��MAC���˺ţ��б����ʹ����ӦMac��ַ��1��16������!
	char*	ChMacAdd[nCHANNEL][nMAC];	//װ��MACԴ��ַ��1��16��MacԴ���˵�ַ!
	char*	ChMacPkt[nCHANNEL];		//װ��ԴMAC��ַ����ʹ��!!!?
	//7��װ��ͨ��n�ı���IED���֣�1��ͨ����¼����16��IED
	char*	ChIedName[nCHANNEL][nIED];	//����ͨ��n�ı���IED������!
	char*	ChIedMac [nCHANNEL][nIED];	//����ͨ��n�ı���IED��Mac��ַ!
	char* 	ChIedType[nCHANNEL][nIED];	//����ͨ��n�ı���IED�����ͣ��ϲ���Ԫ�������豸��MMS�ͻ��˻���������༭������!
	char* 	ChIedBay [nCHANNEL][nIED];	//����ͨ��n�ı���IED�ļ���������!
	char* 	ChIedMan [nCHANNEL][nIED];	//����ͨ��n�ı���IED�ĳ��ң���������!
	char* 	ChIedDesc[nCHANNEL][nIED];	//����ͨ��n�ı���IED��˵��������װ���ͺŻ�˵��!
	char* 	ChIedBip1[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ļ��IP1!
	char*	ChIedBip2[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ļ��IP2!
	//8��װ��ͨ��n�ı���IED��SMV����
	char*	ChMuType[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���Ԫ����!
	char*	ChMuDmac[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���ԪĿ��Mac��ַ!
	char* 	ChMuSmac[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���ԪԴMac��ַ!
	char* 	ChMuSmpR[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���Ԫ����ֵ����!
	char* 	ChMuSmpD[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���Ԫ����ֵ���!
	char* 	ChMuVlId[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���ԪVlan!
	char* 	ChMuPrio[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���Ԫ���ȼ�!
	char* 	ChMuApId[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���ԪAppId!
	char*	ChMuSvId[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���ԪSmvId(Smv92)!
	char*	ChMuDref[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���ԪIedMuDatasetRef(Smv92)!
	char*	ChMuDesc[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���ԪIedMuChannelDesc(Smv92)!
	//9��װ��ͨ��1�ı���IED��GOOSE����
	//	add by haopeng 2012.10.6
	//���Բ���
	char*	ChGSAref[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���ԪGOOSE����cbRef
	char*	ChGSADref[nCHANNEL][nIED];  //����ͨ��n�ı���IED�ϲ���ԪGOOSE����datasetRef
	char*	ChGSAGoId[nCHANNEL][nIED];	//GOOSE����goID
	//GOOSE�ڵ㲿��
	char*	ChGSGoId[nCHANNEL][nIED];	//gooseId
	char*	ChGSDmac[nCHANNEL][nIED];	//gooseĿ��mac
	char* 	ChGSSmac[nCHANNEL][nIED];	//gooseԴmac1
	char*	ChGSSmac2[nCHANNEL][nIED];	//gooseԴmac2
	char*	ChGSVlId[nCHANNEL][nIED];	//VlanId
	char*	ChGSPrio[nCHANNEL][nIED];	//Priority
	char*	ChGSApId[nCHANNEL][nIED];	//appId
	char* 	ChGSref[nCHANNEL][nIED];	//CbRef
	char*	ChGSDref[nCHANNEL][nIED];	//datasetRef
	char*	ChGSLdInst[nCHANNEL][nIED];	//LdInst
	char*	ChGSminTm[nCHANNEL][nIED];	//minTime
	char*	ChGSmaxTm[nCHANNEL][nIED];	//maxTime
	#if 0
	char*	ChGSChDesc					//no use
	#endif
	//
}DeviceStr;

typedef struct parDevice
{
	//1��װ�õ��б��֣����ơ�Ip��ַ����ţ�3������40�ֽڣ�
	char	DevName[NAMELEN];			//װ�����ƣ���¼����װ�õ�����!
	char	DevIp[NAMELEN];				//װ�õ�ip����¼����װ�����ܽӿڵ�ip��ַ!
	int	DevNo;					//װ�ñ�ţ���¼����װ�õı�ţ�ÿ̨װ�ø�һ�����?
	//2��װ�õĹ������֣��汾�ţ�1������4�ֽڣ�
	char	DevVer[NAMELEN];			//װ�ð汾����¼����װ�ð汾�ţ�xml�ļ��İ汾��!
	//3��װ�õĵ�ַ���֣�Ip��ַ���������롢���ء����ܷ�����Ip��ַ�����ܷ������˿ڣ�5������20�ֽڣ�
	char	DevLoIp[NAMELEN];			//װ�õ�ip����¼����װ�����ܽӿڵ�ip��ַ!
	char	DevSnMask[NAMELEN];			//װ�����룬��¼����װ�����ܽӿ���������!
	char	DevGway[NAMELEN];			//װ�����أ���¼����װ�����ܽӿ�����!
	char	DevReIp[NAMELEN];			//װ��Զ��Ip�����ü����������Ip��ַ��˭���������ļ�!
	int	DevRePort;				//װ��Զ�˿ڣ����ü�����������˿ںţ�����Ҫ!
	//4��װ�õĲ������֣���š����ơ�ͨ�����������Ĵ洢��ʽ���洢���ڡ������������μ�顢ʱ����GPS�ӿڣ�10������68�ֽڣ�
	int	DevSubN;				//װ�õ��ӱ�ţ���¼����װ�õı�ţ�ÿ̨װ�ø�һ�����!
	char	DevSname[NAMELEN];			//װ�õ������ƣ���¼����װ�õ�����!
	int	DevChNum;				//װ��ͨ����������¼����װ����2��ͨ��������2��ͨ��!
	int	DevFileF;				//װ���ļ���ʽ����¼����װ���ļ��洢��ʽ�������Զ���!
	int	DevFileP;				//װ�ô洢���ڣ���¼����װ���ļ��洢���ڣ�����600��!
	int	DevFileS;				//װ���ļ���С����¼����װ���ļ��洢��С��50M!
	int	DevHdisk;				//װ�ô��̼�飬��¼����װ�ô��̰ٷֱȣ�50��!
	int	DevTimeZ;				//װ������ʱ������¼����װ��ʱ���ʱ��������ʱ��8!
	int	DevBeatP;				//װ���������ڣ���¼����װ�úͷ���������ϵ���ڣ�����3��!
	int	DevGpsIf;				//װ��GPS�ӿڣ� ��¼����װ��GPS�ӿ����ͣ�ÿͨ��1��GPS�ӿڣ�2ͨ����ͬ��0:TTL��1:485��ȱʡΪ0!
	//5��װ�õ�ͨ�����֣�ͨ���š����ӿ����͡�����ʱ�䡢ץ�����ͣ�2��ͨ��8������16�ֽڣ�
	int	ChNo[nCHANNEL];				//װ��ͨ����ţ�ÿ̨�豸����Կ���2��ͨ����No1Ϊ1ͨ��!
	int	ChIfType[nCHANNEL];			//װ�ýӿ����ͣ�ÿ������ͨ����2�ֽӿڣ���ӿڻ��ӿڣ�0�磬1�⣬������!
	int	ChTrTime[nCHANNEL];			//װ������ʱ�䣬ÿ������ͨ����1���ⴥ���ӿڣ��趨���ⴥ���ӿڵ�����ʱ��?Ŀǰû��
	int	ChPacket[nCHANNEL];			//װ��ץ�����ͣ�ÿ������ͨ������ץȡSMV/GOOSE/MMS���ֱ��ģ���ζ���!?
	char ChAcsLocation[nCHANNEL][NAMELEN];	//��������Ƽ�¼

	//6��װ��ͨ��n���˲���
		//ChPacket[nChannel]:
		//0:����ȫ��֡,�����㲥֡;1:����SMV֡,��SV����ʹ��,2:����GOOSE֡,��GOOSE����ʹ��;3:����MMS֡,��MMS֡ʹ��
		//4:����SMV֡��GOOSE֡,��SV��GOOSE����ʹ��,5:����VLAN1֡,��VLAN1ʹ��,6:����VLAN2֡,��VLAN2ʹ��
		//7:����VLAN1��VLAN2֡,��VLAN2��VLAN2ʹ��,8:����MAC��
		//ChMacPkt[nChannel]:MAC��,16bit����16��MAC��
	int	ChMinLen[nCHANNEL];			//װ����С֡����������С֡��64�ֽڣ�����С��64�ֽ�!
	int	ChMaxLen[nCHANNEL];			//װ�����֡�����������֡��1514�ֽ�!
	int	ChVlanNo[nCHANNEL][nVLAN];		//װ�õ�Vlan1�� ͨ��1��Vlan1�ı�ţ��б����ʹ��Vlan1!
	int	ChVlanId[nCHANNEL][nVLAN];		//װ��Vlan��ǩ��Vlan1��ֵ/Vlan2��ֵ!
	int	ChMacNo [nCHANNEL][nMAC];		//װ��MAC���˺ţ��б����ʹ����ӦMac��ַ��1��16������!
	int	ChMacAdd[nCHANNEL][nMAC][6];		//װ��MACԴ��ַ��1��16��MacԴ���˵�ַ!
	int	ChMacPkt[nCHANNEL];			//װ��ץ�����͡����˷�ʽ�͹���ʹ��!!!?
	//7��װ��ͨ��n�ı���IED���֣�1��ͨ����¼����16��IED
	char	ChIedName[nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED������!
	char	ChIedMac [nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED��Mac��ַ!
	char	ChIedType[nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED�����ͣ��ϲ���Ԫ�������豸��MMS�ͻ��˻���������༭������!
	char	ChIedBay [nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED�ļ���������!
	char	ChIedMan [nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED�ĳ��ң���������!
	char	ChIedDesc[nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED��˵��������װ���ͺŻ�˵��!
	char	ChIedBip1[nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED�ļ��IP1!
	char	ChIedBip2[nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED�ļ��IP2!
	//8��װ��ͨ��n�ı���IED��SMV����
	int	ChMuType[nCHANNEL][nIED];		//����ͨ��n�ı���IED�ϲ���Ԫ����!
	char	ChMuDmac[nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED�ϲ���ԪĿ��Mac��ַ!
	char 	ChMuSmac[nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED�ϲ���ԪԴMac��ַ!
	int 	ChMuSmpR[nCHANNEL][nIED];		//����ͨ��n�ı���IED�ϲ���Ԫ����ֵ����!
	int 	ChMuSmpD[nCHANNEL][nIED];	//����ͨ��n�ı���IED�ϲ���Ԫ����ֵ���!
	int 	ChMuVlId[nCHANNEL][nIED];		//����ͨ��n�ı���IED�ϲ���ԪVlan!
	int 	ChMuPrio[nCHANNEL][nIED];		//����ͨ��n�ı���IED�ϲ���Ԫ���ȼ�!
	int 	ChMuApId[nCHANNEL][nIED];		//����ͨ��n�ı���IED�ϲ���ԪAppId!
	int	ChMuSvId[nCHANNEL][nIED];		//����ͨ��n�ı���IED�ϲ���ԪSmvId(Smv92)!
	int	ChMuDref[nCHANNEL][nIED];		//����ͨ��n�ı���IED�ϲ���ԪIedMuDatasetRef(Smv92)!
	char	ChMuDesc[nCHANNEL][nIED][NAMELEN];//����ͨ��n�ı���IED�ϲ���ԪIedMuChannelDesc(Smv92)!
	//9��װ��ͨ��1�ı���IED��GOOSE����
	
	char	ChGSAref[nCHANNEL][nIED][NAMELEN];	//����ͨ��n�ı���IED�ϲ���ԪGOOSE����cbRef
	char	ChGSADref[nCHANNEL][nIED][NAMELEN];  //����ͨ��n�ı���IED�ϲ���ԪGOOSE����datasetRef
	int		ChGSAGoId[nCHANNEL][nIED];	//GOOSE����goID
	//GOOSE�ڵ㲿��
	int		ChGSGoId[nCHANNEL][nIED];	//gooseId
	char	ChGSDmac[nCHANNEL][nIED][NAMELEN];	//gooseĿ��mac
	char 	ChGSSmac[nCHANNEL][nIED][NAMELEN];	//gooseԴmac1
	char	ChGSSmac2[nCHANNEL][nIED][NAMELEN];	//gooseԴmac2
	int		ChGSVlId[nCHANNEL][nIED];	//VlanId
	int		ChGSPrio[nCHANNEL][nIED];	//Priority
	int		ChGSApId[nCHANNEL][nIED];	//appId
	char 	ChGSref[nCHANNEL][nIED][NAMELEN];	//CbRef
	char	ChGSDref[nCHANNEL][nIED][NAMELEN];	//datasetRef
	char	ChGSLdInst[nCHANNEL][nIED][NAMELEN];	//LdInst
	int		ChGSminTm[nCHANNEL][nIED];	//minTime
	int		ChGSmaxTm[nCHANNEL][nIED];	//maxTime
	//
}DevicePar;

typedef struct pRec{
	char * amount;							//��¼�ļ��м�¼���ļ�����
	char * currentPos;					//��ǰ�������洢�ļ����̷�
	char *diskAmount[PATITION_NUMS];		//�ݶ���Ϊ6����
}pRecord;

typedef struct diskPtr {					/*��¼��¼�ļ��и��ڵ�ָ��,������ɾ�Ĳ����*/
	xmlNodePtr diskx[PATITION_NUMS];
	xmlNodePtr amtx[PATITION_NUMS];
	xmlNodePtr total;
	xmlNodePtr curDisk;
	xmlNodePtr diskList;
	xmlDocPtr doc;
}diskPtr;

typedef struct nodePtr {			//���ڽ�������ʱ���¼�¼�ã��ɱ����ظ�����������ٶ�
	xmlNodePtr status;			
	xmlNodePtr packCnt;
	xmlAttrPtr endTime;
}nodePtr;

typedef struct curFileInfo{
	int attrNo;					/*����:���*/
	char attrStartTm[16];			/*����:��ʼʱ��*/
	char attrEndTm[16];			/*����:����ʱ��*/
	int status;					/*״̬,0δ����,1�Ѵ���*/
	char position[8];				/*����λ��,diskA,B,C,D,E,F*/
	char filename[24];				/*�ļ���:XXX-D-C-YYYYMMDDHHmmSS*/
	char path[12];				/*·��*/
	long packCnt;					/*�ܰ���*/
	nodePtr	xmlnodePtr;			/*xml�ļ�ָ��*/
}curFileInfo;


int xml_getConfigDevFileS();
char  xml_getCurrentDiskInfo();
int xml_setCurrentDiskInfo(int chNo, char position);
int xml_getConfigDevFileF();

#endif
