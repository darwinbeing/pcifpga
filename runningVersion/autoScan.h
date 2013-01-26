#ifndef __AUTOSCAN_H__
#define __AUTOSCAN_H__

//װ������Rom�����ļ����ء����ء���ѯ�������Ϣ����
#define	MSG_DN_RESET		0x03	//ǿ��ϵͳ���и�λ������װ�ý��и�λ�����г�������������Ϣ����Ҫ��Ӧ
#define	MSG_DN_IP_RESET		0xE0	//�ָ�װ�ó������ã��ָ�����Ĭ�����ã�Ȼ��ϵͳ��λ������Ϣ����Ҫ��Ӧ
#define	MSG_QUERY_DN		0xF6	//��ѯװ�õ�DN��Ϣ������Ϣ����Ҫ��������ҪӦ��
#define	MSG_QUERY_DN_ACK	0xF7	//��ѯDN��Ϣ��Ӧ��Ӧ��Name��Type��Mac��Ip1��Mask1��Gateway1��Ip2��Mask2��Gateway2��Note��
#define	MSG_QUERY_CFG_INFO	0xF8	//��ѯװ��������Ϣ������Ϣ����Ҫ��������ҪӦ��
#define	MSG_QUERY_CFG_INFO_ACK	0xF9	//��ѯ������ϢӦ��Ӧ��Name��Type��Mac��Note��
#define	MSG_PARAM_CFG		0xFA	//����װ�õ��������
#define	MSG_PARAM_CFG_ACK	0xFB	//����װ�õ����������Ӧ��
#define	MSG_SET_IP		0xFC	//����װ�õ�IP��ַ������Ϣ����Ҫ��Ӧ����ɺ�ϵͳҪ���и�λ
#define	MSG_SET_IP_AND_PARAM	0xFD	//����װ�õ�IP��ַ�Ͳ���������Ϣ����Ҫ��Ӧ����ɺ�ϵͳҪ���и�λ
#define	MSG_RESTART_SERVICE	0xFE	//��������װ�õ�һЩ������̣�Ŀǰû�иù���

typedef struct FactoryInfo		// �ļ�����DeviceConfig_factory.rom
{					// 256�ֽ�
	unsigned char flag[4]; 		// device flag:��0000��
	unsigned char name[16];		// device name:"Testing System1 "
	unsigned char type[16];		// device type:"TY-3012S-000001 "
	unsigned char sn[16];		// device serial number:"Ctdt 0123456789 "
	unsigned char passwd[4];	// device super password:"ctdt"
	unsigned char mac1[6];		// device mac address:00:80:c8:e1:cf:5d
	unsigned char mac2[6];		// device mac address:00:80:c8:e1:cf:5e
	unsigned char ip1[4];		// device ip address:192.168.0.100
	unsigned char ip2[4];		// device ip address:192.168.1.100
	unsigned char mask1[4];		// device subnet mask: 255.255.255.0
	unsigned char mask2[4];		// device subnet mask: 255.255.255.0
	unsigned char gateway1[4];	// device gateway ip address: 192.168.0.1
	unsigned char gateway2[4];	// device gateway ip address: 192.168.1.1
	unsigned char udp_port1[2];	// device udp port: 10001ɨ���������DeviceConfig_user.rom�ļ�
	unsigned char udp_port2[2];	// server udp port: 30718������չ��Ŀǰ����
	unsigned char tcp_port1[2];	// device tcp port: 53456�ϱ�ʵʱ���ݣ��ϱ�SMV����
	unsigned char tcp_port2[2];	// device tcp port: 53457�ϱ�ʵʱ�澯���ϱ�ʵʱ�澯
	unsigned char tcp_port3[2];	// device tcp port: 53458�����������ݣ��ϴ�xmlDocunment�ļ�
	unsigned char tcp_port4[2];	// device tcp port: 53459�ϱ���ʷ���ݣ��ϴ��洢�ı����ļ�
	unsigned char tcp_port5[2];	// device tcp port: 53460װ�ò�ѯ
	unsigned char tcp_port6[2];	// device tcp port: 53460������չ��Ŀǰ����
	unsigned char hardware[4];	// device hardware version:"010a"
	unsigned char software[4];	// device software version:"010a"
	unsigned char date[10];		// device date:"2010.10.20"
	unsigned char note[32];		// device notes:"http://www.dxyb.com 10-62301187!"
	unsigned char reserved1[32];	// device "www.ctdt.com.cn www.ctdt.com.cn!"
	unsigned char reserved2[64];	// device "Testing System for Communication Network of Digital Substation! "		
	unsigned char crc[2];		// idprom default section crc16 code:		
}RomFactoryInfo;

typedef struct UserInfo			// DeviceConfig_user.rom
{					// 256�ֽ�
	unsigned char name[16];		// device name:"Testing System1 "
	unsigned char type[16];		// device type:"TY-3012S-000001 "
	unsigned char passwd[4];	// device super password:"ctdt"
	unsigned char ip1[4];		// device ip address:192.168.0.100
	unsigned char ip2[4];		// device ip address:192.168.1.100
	unsigned char mask1[4];		// device subnet mask:255.255.255.0
	unsigned char mask2[4];		// device subnet mask:255.255.255.0
	unsigned char gateway1[4];	// device gateway ip address:192.168.0.1
	unsigned char gateway2[4];	// device gateway ip address:192.168.1.1
	unsigned char note[32];		// device notes:"http://www.dxyb.com 10-62301187!"
	unsigned char reserved1[32];	// device "www.ctdt.com.cn www.ctdt.com.cn!"
	unsigned char reserved2[130];	// device reserved2:all zero
	unsigned char crc[2];		// idprom default section crc16 code: 00
}RomUserInfo;

typedef struct MsgAckHeader		// ��Ϣ�������Ӧ��ͷ��16�ֽڣ��̶��ĸ�ʽ
{					// ��256�ֽڣ�ֻǰ���16�ֽ��ж���
	unsigned char Header[3];	// ��Ϣ��ͷ����3�ֽڣ��̶���3�ֽڶ���0
	unsigned char MsgID;		// ��Ϣ�����8�����Ŀǰʵ��7������3��Ӧ�𣬹�������10��
	unsigned char Passwd[4];	// ��Ϣ���룬��4�ֽڣ��̶�Ϊctdt
	unsigned char Mac[6];		// Mac ��ַ����6�ֽڣ����Լ���Mac��ַƥ�����ִ����Ӧ������
	unsigned char Reserved[2];	// �����ֽڣ���2�ֽڣ��̶���2�ֽڶ���0
	unsigned char Parameter[240];	// ��Ϣ��������240�ֽ�
}RomMsgAckHeader;

typedef struct DeviceConfInfo		// ����װ�õ�����Ͳ�������ѯװ�õ�DNʱʹ��
{					// 152 byte��16 + 136 byte
	unsigned char Header[3];	// ��Ϣ��ͷ����3�ֽڣ��̶���3�ֽڶ���0
	unsigned char MsgID;		// ��Ϣ�����8�����3��Ӧ��
	unsigned char Passwd[4];	// ��Ϣ���룬��4�ֽڣ��̶�Ϊctdt
	unsigned char Mac[6];		// Mac ��ַ����6�ֽڣ����Լ���Mac��ַƥ�����ִ����Ӧ������
	unsigned char Reserved[2];	// �����ֽڣ���2�ֽڣ��̶���2�ֽڶ���0
	unsigned char name[16];		// device name
	unsigned char type[16];		// device type
	unsigned char mac[6];		// device mac address, eth0
	unsigned char reserved0[2];
	unsigned char yearh;		// year, max byte
	unsigned char yearl;		// year, min byte
	unsigned char mouth;		// month,   date of this program running
	unsigned char date;		// date,    date of this program running
	unsigned char hours;		// hours,   time of this program running
	unsigned char minutes;		// minutes, time of this program running
	unsigned char seconds;		// seconds, time of this program running
	unsigned char reserved1;
	unsigned char ip1[4];		// eth0 Ip address
	unsigned char mask1[4];		// eth0 Sub mask
	unsigned char gateway1[4];	// eth0 Gateway ip address
	unsigned char ip2[4];		// eth1 Ip address
	unsigned char mask2[4];		// eth1 Sub mask
	unsigned char gateway2[4];	// eth1 Gateway ip address
	unsigned char note[32];		// First 4 byte are Telnet login password. 4 byte 0 means not set
	unsigned char reserved2[32];
}RomDeviceConfInfo;

typedef struct ParamConfInfo		// ����װ�õĲ�������ѯװ�õĲ���ʱʹ��
{					// 112 byte��16 + 96 byte
	unsigned char Header[3];	// ��Ϣ��ͷ����3�ֽڣ��̶���3�ֽڶ���0
	unsigned char MsgID;		// ��Ϣ�����8�����3��Ӧ��
	unsigned char Passwd[4];	// ��Ϣ���룬��4�ֽڣ��̶�Ϊctdt
	unsigned char Mac[6];		// Mac ��ַ����6�ֽڣ����Լ���Mac��ַƥ�����ִ����Ӧ������
	unsigned char Reserved[2];	// �����ֽڣ���2�ֽڣ��̶���2�ֽڶ���0
	unsigned char name[16];		// device name
	unsigned char type[16];		// device type
	unsigned char note[32];		// device mac address, eth0
	unsigned char reserved[32];	//
}RomParamConfInfo;

typedef struct ConfIp			// ����װ�õ�������Ϣ�Ͳ���ʱʹ��
{					// 52 byte��16 + 36 byte
	unsigned char Header[3];	// ��Ϣ��ͷ����3�ֽڣ��̶���3�ֽڶ���0
	unsigned char MsgID;		// ��Ϣ�����8�����3��Ӧ��
	unsigned char Passwd[4];	// ��Ϣ���룬��4�ֽڣ��̶�Ϊctdt
	unsigned char Mac[6];		// Mac ��ַ����6�ֽڣ����Լ���Mac��ַƥ�����ִ����Ӧ������
	unsigned char Reserved[2];	// �����ֽڣ���2�ֽڣ��̶���2�ֽڶ���0
	unsigned char ipsetup[8];	// �̶�Ϊ�ַ���:IP-SETUP
	unsigned char signal;		// �����ǣ�=1:ֻ�������ڣ�=2��ֻ����ʱ�ӿڣ�=3�������ã�
	unsigned char reserved[3];	// �����ֽڣ���3�ֽڣ��̶���3�ֽڶ���0
	unsigned char ip1[4];		// eth0 Ip address
	unsigned char mask1[4];		// eth0 Sub mask
	unsigned char gateway1[4];	// eth0 Gateway ip address
	unsigned char ip2[4];		// eth1 Ip address
	unsigned char mask2[4];		// eth1 Sub mask
	unsigned char gateway2[4];	// eth1 Gateway ip address
}RomConfIp;

//#define RomUdpPort		5677		//Ӧ������IDPROM
//#define RomUdpPort		3456		//Ӧ������IDPROM

#define FACTORY_USER_LEN	256		//���ó�������������160�ֽ�
#define MSG_ACK_LEN		256		//������Ϣ���壬ȷ����Ϣ����Ӧ�ĳ��Ȳ�����128�ֽ�

void ConfigRomFileUdp(int RomSockfd);
#endif
