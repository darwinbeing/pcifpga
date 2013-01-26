/* PCIFPGA��⿨����������1 */

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

//����װ��������Ϣ
#define SET_ETH_INTERFACE	0x01	//01IO�����ü��ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define SET_GPS_INTERFACE	0x02	//02IO������GPS��1588�ӿڣ��Լ��ӿ�������TTL��485�ӿ�
#define SET_FPGA_TIME		0x03	//03WR������FPGA��ʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define SET_TRIGGER_TIME	0x04	//04IO�����ü��ͨ���ⴥ������ʱ�䣬��λus
#define SET_ALARM_OUT		0x05	//05IO�����ü��ͨ������澯��������̵����պ�
#define SET_BEEP_ALARM		0x06	//06IO�������ڲ��������澯�������������
#define SET_WARCH_DOG		0x07	//07IO�����ü�����ⲿ���Ź������Ź�Ҫ��ʱ���
#define SET_CH_FILTER		0x08	//08WR�����ü��ͨ���Ĺ�����������������
#define SET_FILTER_MAC		0x09	//09WR�����ü��ͨ����ԴMAC���˵�ַ�������ո�MAC��ַ����
#define SET_BUFFER_EMPTY	0x0A	//0AIO���������FIFO�е����ݣ������FIFO
//����������ϢӦ��
#define ACK_ETH_INTERFACE	0x81	//01IO��Ӧ����ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define ACK_GPS_INTERFACE	0x82	//02IO��Ӧ��GPS��1588�ӿڣ��Լ��ӿ�������TTL��485�ӿ�
#define ACK_FPGA_TIME		0x83	//03WR��Ӧ��FPGA��ʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define ACK_TRIGGER_TIME	0x84	//04IO��Ӧ����ͨ���ⴥ������ʱ�䣬��λus
#define ACK_ALARM_OUT		0x85	//05IO��Ӧ����ͨ������澯��������̵����պ�
#define ACK_BEEP_ALARM		0x86	//06IO��Ӧ���ڲ��������澯�������������
#define ACK_WARCH_DOG		0x87	//07IO��Ӧ������ⲿ���Ź������Ź�Ҫ��ʱ���
#define ACK_SET_FILTER		0x88	//08WR��Ӧ����ͨ���Ĺ�����������������
#define ACK_FILTER_MACSET	0x89	//09WR��Ӧ����ͨ����ԴMAC��ַ���ˣ������ո�MAC��ַ����
#define ACK_BUFFER_EMPTY	0x8A	//0AIO��Ӧ�����FIFO�е����ݣ������FIFO
//����״̬��ѯ��Ϣ
#define READ_CH_BUFFER		0x11	//11RD����ȡ¼������FIFO�е����ݣ�ÿ�����2M�ֽ�
#define CHECK_CH_INTERFACE	0x12	//12IO�������ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define CHECK_CH_STATUS		0x13	//13IO�������ͨ�����ӿڵ�����״̬�����ӡ��ٶȡ�˫��
#define CHECK_FPGA_TIMER	0x14	//14RD�����FPGAʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define CHECK_TRIGGER_STATUS	0x15	//15IO�������ͨ���ⴥ��״̬�����޴������Ƿ����ڴ���
#define CHECK_KEY_STATUS	0x16	//16IO�����ǰ��尴��״̬�����ް������Ƿ����ڰ���
#define CHECK_CH_FILTER		0x17	//17RD�������ͨ���Ĺ�������
#define CHECK_FILTER_MAC	0x18	//18RD�������ͨ����ԴMAC���˵�ַ��һ��MAC��������
#define CHECK_FRAME_COUNTERS	0x19	//19RD�������ͨ��֡���ֽڼ�������ͳ������
//�����ѯ��ϢӦ��
#define ACK_CH_BUFFER		0x90	//10RD��Ӧ��¼������FIFO�е����ݣ�ÿ�����2M�ֽ�
#define ACK_CH_INTERFACE	0x91	//11IO��Ӧ����ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
#define ACK_CH_STATUS		0x92	//12IO��Ӧ�����ͨ�����ӿڵ�����״̬�����ӡ��ٶȡ�˫��
#define ACK_FPGA_TIMER		0x93	//13RD��Ӧ��FPGAʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
#define ACK_TRIGGER_STATUS	0x95	//15IO��Ӧ����ͨ���ⴥ��״̬�����޴������Ƿ����ڴ���
#define ACK_KEY_STATUS		0x96	//16IO��Ӧ��ǰ��尴��״̬�����ް������Ƿ����ڰ���
#define ACK_CH_FILTER		0x97	//17RD��Ӧ����ͨ���Ĺ�������
#define ACK_FILTER_MAC		0x98	//18RD�������ͨ����ԴMAC��ַ���ˣ�һ��MAC��������
#define ACK_FRAME_COUNTERS	0x99	//19RD��Ӧ����ͨ��֡���ֽڼ�������ͳ������
//��������ѯ��Ϣ
#define SET_DEVICE_LED		0x21	//23WR������ǰ���LED״̬���ܹ�64��LED
#define CHECK_LED_STATUS	0x22	//24RD�����ǰ���LED״̬���ܹ�64��LED
//��������ѯӦ��
#define ACK_DEVICE_LED		0x0a1	//23WR��Ӧ��ǰ���LED״̬���ܹ�64��LED
#define ACK_LED_STATUS		0x0a2	//24RD��Ӧ��ǰ���LED״̬���ܹ�64��LED

typedef struct Command			//��������Ϣ��ʽ64�ֽ�
{					//��������Ϣ������֡ͷ��Ӳ���汾�������汾���Ự����Ϣ�����Ϣ����������
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾��2�ֽڻỰ��Ӧ�ö˷�0�������˷�1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	Reserve[12];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgCommand;

typedef struct ChInterface		//���û�����ͨ������̫���ӿ�����
{					//���û�����ͨ������̫���ӿ������ǹ�ӿڻ��ǵ�ӿ�
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	Ch1Interface;	//�ӿ�����4�ֽڣ����ͨ��1�Ľӿ����ͣ���ӿ�1����ӿ�0
	unsigned int	Ch2Interface;	//�ӿ�����4�ֽڣ����ͨ��2�Ľӿ����ͣ���ӿ�1����ӿ�0
	unsigned int	Reserve[10];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgChInterface;

typedef struct GpsInterface		//���û���ʹ��GPS��1588
{					//���û���װ����ʹ��GPS����1588ʱ��
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	Gps1588;	//ʱ��ѡ��4�ֽڣ�ѡ��GPS�ӿڻ�1588�ӿڣ�1ѡ1588��0ѡIRIGB
	unsigned int	InterfaceType;	//�ӿ�����4�ֽڣ�ѡ��ƽ����ƽ�⣬1ƽ��ӿڣ�0��ƽ��75ŷ�ӿ�
	unsigned int	Reserve[10];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgGpsInterface;

typedef struct FpgaTime			//���û���ϵͳʱ�䣬��֤ϵͳ�����������ʱ��ȷ
{					//����GPS��1588ʱ�䣬����FPGA���ꡢ�¡��ա�ʱ���֡��롢���롢΢��
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	Day;		//ͨ��ʱ�䣬��
	unsigned int	Hour;		//ͨ��ʱ�䣬ʱ
	unsigned int	Minute;		//ͨ��ʱ�䣬��
	unsigned int	Second;		//ͨ��ʱ�䣬��
	unsigned int	Millisecond;	//ͨ��ʱ�䣬����
	unsigned int	Microsecond;	//ͨ��ʱ�䣬΢��
	unsigned int	GpsStatus;	//ʱ��״̬4�ֽڣ�GPS�澯��0=GPS������1=GPSʱ�Ӷ�ʧ
	unsigned int	PtpStatus;	//ʱ��״̬4�ֽڣ�1588�澯��0=������1=1588ʱ�Ӷ�ʧ
	unsigned int	ErrStatus;	//ʱ�Ӵ���4�ֽڣ�FPGA����0=����ʱ�ӵ���GPS��=1����GPS
	unsigned int	ErrValue;	//ʱ�Ӵ���4�ֽڣ���������ʱ��ÿ��ƫ������10MHz��λ
	unsigned int	Reserve[2];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgFpgaTime;

typedef struct TriggerTime		//�����ⴥ������ӿ�1��2������ʱ��
{					//�����ⴥ���̵���1��2������ʱ��
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	Sw1TrigerTime;	//����ʱ��4�ֽڣ��ⴥ���ӿ�1����λus����С0�����256
	unsigned int	Sw2TrigerTime;	//����ʱ��4�ֽڣ��ⴥ���ӿ�2����λus����С0�����256
	unsigned int	Reserve[10];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgTriggerTime;

typedef struct AlarmOut			//������澯�ӿ�1��2���
{					//������澯�ӿ�1��2�ļ̵����պ�
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	Sw1AlarmOut;	//�澯���4�ֽڣ���澯�ӿ�1��1=�պϣ�0=�Ͽ�
	unsigned int	Sw2AlarmOut;	//�澯���4�ֽڣ���澯�ӿ�2��1=�պϣ�0=�Ͽ�
	unsigned int	Reserve[10];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgAlarmOut;

typedef struct BeepAlarm		//�����ڷ�������
{					//�����ڷ������澯
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	BeepAlarm;	//��������4�ֽڣ��ڷ������澯��D1=1��������һ�������������������D0=1����
	unsigned int	Reserve[11];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgBeepAlarm;

typedef struct WarchDog			//�����ڿ��Ź�оƬ���
{					//����ڿ��Ź�оƬ�ļ�����
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	WarchDog;	//�忴�Ź�4�ֽڣ�д0xFF������Ź���Ȼ���Զ����D7��D0����
	unsigned int	Reserve[11];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgWarchDog;

typedef struct ClearFifo		//����FIFO�գ������FIFO���ݣ�ָ�����
{					//���FPGA��64K�ֽ�FIFO������
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	ClearFifo1;	//�忴FIFO4�ֽڣ�д0xFF���FIFO��Ȼ���Զ����D7��D0����
	unsigned int	ClearFifo2;	//�忴FIFO4�ֽڣ�д0xFF���FIFO��Ȼ���Զ����D7��D0����
	unsigned int	Reserve[10];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgClearFifo;

typedef struct ChFilter			//���û�����ͨ���Ĺ�������
{					//���������������ˣ��㲥֡��VLAN��GOOSE��SMV��MMS��61850���ͣ�MACԭ��MAC��61850֡��VLAN��61850֡
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	ChNo;		//��Ϣͨ��4�ֽڣ�ͨ��1��ͨ��2
	unsigned int	Reset;
	unsigned int	Mode;		//1:����ģʽ��DO=1�����ˣ�0���ˣ�D1=0ֻ�չ㲥֡��1���������������ˣ�00���չ㲥֡��01�����ˣ�10������������
	unsigned int	MinLength;	//2:��С֡����������64�ֽ�
	unsigned int	MaxLength;	//3:���֡����������1518�ֽ�
	unsigned int	FilterEnable;	//4:����������1ʹ�ܣ�D0��9: VLAN1��VLAN2��GOOSE��SMV��MMS�����ͣ�MACԴ��VLAN���ˣ�MACԴ�����ͣ�VLAN������
	unsigned int	Vlan1Id;	//5:Vlan1��ǩ��D11��D0��VID
	unsigned int	Vlan2Id;	//6:Vlan2��ǩ��D11��D0��VID
	unsigned int	MacEnable;	//7:16��MACԴ��ַʹ�ܣ�bitλ��1ʹ�ܣ�D15��D0��SourceMac16��1��1=enable��0=disenable
	unsigned int	Reserve[3];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgChFilter;

typedef struct FilterMac		//���û�����ͨ���Ĺ���MAC
{					//D31��D16������D15��D0��SourceMacAdd��47��32 bit��D31��D00ʹ�ã�D31��D0��SourceMacAdd��31��00 bit
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	ChNo;		//��Ϣͨ��4�ֽڣ�ͨ��1��ͨ��2
	unsigned int	GroupNo;	//��Ϣ���4�ֽڣ�ͨ��1��ͨ��2�ĵڼ��飬1��4
	unsigned int	MacAddr1H;	//MACԴ��ַ��ÿ��MAC��6�ֽڣ�ǰ�棨��λ����2�ֽ�
	unsigned int	MacAddr1L;	//MACԴ��ַ��ÿ��MAC��6�ֽڣ����棨��λ����4�ֽ�
	unsigned int	MacAddr2H;	//MACԴ��ַ��ÿ��MAC��6�ֽڣ�ǰ�棨��λ����2�ֽ�
	unsigned int	MacAddr2L;	//MACԴ��ַ��ÿ��MAC��6�ֽڣ����棨��λ����4�ֽ�
	unsigned int	MacAddr3H;	//MACԴ��ַ��ÿ��MAC��6�ֽڣ�ǰ�棨��λ����2�ֽ�
	unsigned int	MacAddr3L;	//MACԴ��ַ��ÿ��MAC��6�ֽڣ����棨��λ����4�ֽ�
	unsigned int	MacAddr4H;	//MACԴ��ַ��ÿ��MAC��6�ֽڣ�ǰ�棨��λ����2�ֽ�
	unsigned int	MacAddr4L;	//MACԴ��ַ��ÿ��MAC��6�ֽڣ����棨��λ����4�ֽ�
	unsigned int	Reserve[2];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgFilterMac;

typedef struct ChStatus			//�����ͨ������̫���ӿ�״̬��Los/Link/Speed/Duplex
{					//�����ͨ��1��2����̫���ӿ�״̬��Los/Link/Speed/Duplex
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	EthOLink;	//�ӿ�״̬4�ֽڣ�0���Los��1���Link
	unsigned int	EthELink;	//�ӿ�״̬4�ֽڣ�0��Link��1=Link10��˫����2=Link10ȫ˫����3=Link100��˫����4Link100ȫ˫����
	unsigned int	Reserve[10];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgChStatus;

typedef struct TriggerStatus		//����ⴥ���ӿ�1��2��״̬
{					//����ⴥ���ӿ�1��2�ĵ�ǰ����ʱ״̬
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	Sw1NowStatus;	//�ӿ�״̬4�ֽڣ��ⴥ���ӿ�1��ǰ״̬��1��ʾĿǰ�̵����պ�
	unsigned int	Sw1HisStatus;	//�ӿ�״̬4�ֽڣ��ⴥ���ӿ�1��ʷ״̬��1��ʾ�����̵����պ�
	unsigned int	Sw2NowStatus;	//�ӿ�״̬4�ֽڣ��ⴥ���ӿ�2��ǰ״̬��1��ʾĿǰ�̵����պ�
	unsigned int	Sw2HisStatus;	//�ӿ�״̬4�ֽڣ��ⴥ���ӿ�2��ʷ״̬��1��ʾ�����̵����պ�
	unsigned int	Reserve[8];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgTriggerStatus;

typedef struct KeyStatus		//��鰴��1��2��״̬
{					//��鰴��1��2�ĵ�ǰ����ʱ״̬
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	Key1NowStatus;	//����״̬4�ֽڣ�����1��ǰ״̬��1��ʾĿǰ�����պ�
	unsigned int	Key1HisStatus;	//����״̬4�ֽڣ�����1��ʷ״̬��1��ʾ���������պ�
	unsigned int	Key2NowStatus;	//����״̬4�ֽڣ�����2��ǰ״̬��1��ʾĿǰ�����պ�
	unsigned int	Key2HisStatus;	//����״̬4�ֽڣ�����2��ʷ״̬��1��ʾ���������պ�
	unsigned int	Reserve[8];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgKeyStatus;

typedef struct FrameCounters		//�����ͨ���ĸ��ּ�������ֵ
{					//�����ͨ��1��2�ĸ�����������ֵ
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	ChNo;		//��Ϣͨ��4�ֽڣ�ͨ��1��ͨ��2
	unsigned int	AllFrame;	//0:��֡��L
	unsigned int	AllByte;	//1:���ֽ���L
	unsigned int	BroadcastFrame;	//2:�㲥֡��L
	unsigned int	BroadcastByte;	//3:�㲥�ֽ���L
	unsigned int	MulticastFrame;	//4:�ಥ֡��L
	unsigned int	MulticastByte;	//5:�ಥ�ֽ���L
	unsigned int	CrcErrFrame;	//6:CRC����֡��L
	unsigned int	ErrFrame;	//7:����֡��L
	unsigned int	AllignErrFrame;	//8:��λ����֡��L
	unsigned int	ShortFrame;	//9:����֡��L
	unsigned int	LongFrame;	//10:����֡��L
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgFrameCounters;

typedef struct LedStatus		//���û���װ��ǰ����LED
{					//��ַ0x0BΪ��λ˫�֣���ַ0x0CΪ��λ˫��
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	LedL;		//32��LED
	unsigned int	LedH;		//32��LED
	unsigned int	Reserve[10];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgLedStatus;

typedef struct ReadBuffer		//��ȡͨ����FIFO����
{
	unsigned int	Head;		//��Ϣͷ��4�ֽڣ�1a2b3c4d
	unsigned int	VersionSid;	//��Ϣ�Ự4�ֽڣ�1�ֽ�Ӳ���汾��1�ֽ������汾���Ự2�ֽڣ���������Ӧ�ö�+1
	unsigned int	Id;		//��Ϣ����4�ֽڣ���Ϣ����ʹ�����1�ֽ�
	unsigned int	ChNo;		//��Ϣͨ��4�ֽڣ�ͨ��1��ͨ��2
	unsigned int	Byte;		//��Ϣͨ��4�ֽڣ���ȡ�ֽ���
	unsigned int	Reserve[10];	//�����ֽ�x�ֽڣ������ֽ���Ϊ�˱�֤��ϢΪ�̶���64�ֽ�
	unsigned int	Tail;		//��Ϣβ��4�ֽڣ�a1b2c3d4
}MsgReadBuffer;

//��Ϣ����
#define	MSG_BUF_LEN	0x40		//������Ϣ����󳤶�
#define	CH_BUF_LEN	0x200000	//������ձ���֡����󳤶�
#define	READ_BUF_LEN	0x10000		//����ÿ�ζ�ȡ����֡�ĳ���
MsgCommand		*AppMsgCommand;
MsgChInterface		*AppChInterface;
MsgGpsInterface		*AppGpsInterface;
MsgFpgaTime		*AppFpgaTime;
MsgTriggerTime		*AppTriggerTime;
MsgAlarmOut		*AppAlarmOut;
MsgBeepAlarm		*AppBeepAlarm;
MsgWarchDog		*AppWarchDog;
MsgClearFifo		*AppClearFifo;
MsgChFilter		*AppChFilter;
MsgFilterMac		*AppFilterMac;
MsgChStatus		*AppChStatus;
MsgTriggerStatus	*AppTriggerStatus;
MsgKeyStatus		*AppKeyStatus;
MsgFrameCounters	*AppFrameCounters;
MsgLedStatus		*AppLedStatus;
MsgReadBuffer		*AppReadBuffer;

//�趨��ͬ�Ļ�����
unsigned char		MsgBuffer[MSG_BUF_LEN];
unsigned char		Ch1Buffer[CH_BUF_LEN];
unsigned char		Ch2Buffer[CH_BUF_LEN];

static int SetMsgBuffer(void)
{
	AppMsgCommand = (MsgCommand *)MsgBuffer;
	memset(MsgBuffer, 0x0, MSG_BUF_LEN);
	AppMsgCommand->Head		= 0x01a2b3c4d;
	AppMsgCommand->VersionSid	= 0x00;
	AppMsgCommand->Id		= 0x00;
	AppMsgCommand->Tail		= 0x0a1b2c3d4;
	return 0;
}

//��ȡ��������,��ȡSV/SMV/GOOSE/SM��������
int main(int argc, char *argv[])
{
	int fd = 0;
	int i, len1, len2;

	AppReadBuffer = (MsgReadBuffer *)MsgBuffer;
	SetMsgBuffer();
	AppReadBuffer->Id   = READ_CH_BUFFER;			//�����յ��ı���
	fd = open("/dev/pcifpga", O_RDONLY);				//����������
	if (fd == -1) {
		printf("TY-3012S App open /dev/pcifpga error!\n");	//������ʹ��App��Ϊ��ʶ
		return -1;						//��������,��������ʧ
	}
	printf("TY-3012S App open /dev/pcifpga successfully!\n");
	sleep(1);
	while(1) {
		//���ͨ��1
		AppReadBuffer->ChNo = 1;				//���μ��ͨ��1
		AppReadBuffer->Byte = READ_BUF_LEN;			//256K�ֽ�,ÿ�����Ӧ�ü��512K�ֽ�
		memcpy(Ch1Buffer, MsgBuffer, MSG_BUF_LEN);
		len1 = read(fd, Ch1Buffer, READ_BUF_LEN);		//ÿ�δ�������ȡ64K�ֽ�
		#if 0
		if(len1 == 0) {
			printf("TY-3012S App founds ch1 Empty: %d!\n", len1);
			sleep(1);
		}
		#endif
		#if 1
		//sleep(1);
		if(len1 >= 0) {
			printf("TY-3012S App read ch1: %d!", len1);
			for(i = 0; i < len1; i++) {
				if((i % 0x10) == 0) printf("\n%02x:", i);
				printf(" %02x", Ch1Buffer[i]);
			}
			printf("\n");
			printf("TY-3012S App READ_CH1_BUF ok!\n");
		} else {
			close(fd);
			printf("TY-3012S App read ch1 error\n!");	//len1���ظ�ֵ,��������,���´�����
			return 0;
		}
		#endif
		//���ͨ��2
		AppReadBuffer->ChNo = 2;				//���μ��ͨ��2
		AppReadBuffer->Byte = READ_BUF_LEN;			//256K�ֽ�,ÿ�����Ӧ�ü��512K�ֽ�
		memcpy(Ch2Buffer, MsgBuffer, MSG_BUF_LEN);
		len2 = read(fd, Ch2Buffer, READ_BUF_LEN);		//ÿ�δ�������ȡ64K�ֽ�
		//if(len2 == 0) {
		//	printf("TY-3012S App founds ch2 Empty: %d!\n", len2);
		//	sleep(1);
		//}
		#if 1
		//sleep(1);
		if(len2 >= 0) {
			printf("TY-3012S App read ch2: %d!", len2);
			for(i = 0; i < len2; i++) {
				if((i % 0x10) == 0) printf("\n%02x:", i);
				printf(" %02x", Ch2Buffer[i]);
			}
			printf("\n");
			printf("TY-3012S App READ_CH2_BUF ok!\n");
		} else {
			close(fd);
			printf("TY-3012S App read ch2 error\n!");	//len2���ظ�ֵ,��������,���´�����
			return 0;
		}
		#endif
		usleep(1000);
	}
	close(fd);
	return 0;
}
