//Version 1
//PCIFPGA����������2012��8��16��
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <asm/unaligned.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/signal.h>
#include <asm/irq.h>
#include <linux/mm.h>

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
	unsigned int	DD;		//ͨ��ʱ�䣬��
	unsigned int	HH;		//ͨ��ʱ�䣬ʱ
	unsigned int	MM;		//ͨ��ʱ�䣬��
	unsigned int	SS;		//ͨ��ʱ�䣬��
	unsigned int	MS;		//ͨ��ʱ�䣬����
	unsigned int	US;		//ͨ��ʱ�䣬΢��
	unsigned int	GS;		//ʱ��״̬4�ֽڣ�GPS�澯��0=GPS������1=GPSʱ�Ӷ�ʧ
	unsigned int	PS;		//ʱ��״̬4�ֽڣ�1588�澯��0=������1=1588ʱ�Ӷ�ʧ
	unsigned int	ES;		//ʱ�Ӵ���4�ֽڣ�FPGA����0=����ʱ�ӵ���GPS��=1����GPS
	unsigned int	EV;		//ʱ�Ӵ���4�ֽڣ���������ʱ��ÿ��ƫ������10MHz��λ
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

//PCIFPGA���Կ���PCI��ض��壬�й�PCI��
#define CTD_VENDOR_ID		0x1100		//Vendor ID
#define CTD_SUB_VENDOR		0x1100		//SubVendor ID
#define CTD_DEVICE_ID		0x4258		//Device ID
#define CTD_SUB_DEVICE		0x4258		//SubDevice ID
#define CTD_PCI_NAME		"CTDPCI-0x1100"
#define CTD_REVISION_ID		0x00		//Revision ID��Ӳ���汾				��������
#define CTD_CLASS_CODE		0x04		//Base Class Code 04h Multimedia Device��	��������
#define CTD_SUB_CLASS		0x00		//Sub-class Code 00h Video Device��		��������

//PCIFPGA�ĳ��Һ���������
int			ctd_pci_vendor;		//PCI�豸���ÿռ��е�Device ID
int			ctd_pci_device;		//PCI�豸���ÿռ��е�Vendor ID
//PCIFPGA�洢���ռ䣬����ַ0��bar0����1K�ֽڣ�
static unsigned long	chx_mem0_base;		//PCI�Ĵ洢���ռ����ַ0�������ַ
static unsigned int	chx_mem0_range;		//PCI�Ĵ洢���ռ����ַ��Χ��1K�ֽ�
static void	__iomem	*ChxBaseAddr0;		//PCI�Ĵ洢���ռ����ַ0�����ַ��Linux��ַ
//PCIFPGA�洢���ռ䣬����ַ1��bar1����1K�ֽڣ�
static unsigned long	ch1_mem1_base;		//PCI�Ĵ洢���ռ����ַ1�������ַ
static unsigned int	ch1_mem1_range;		//PCI�Ĵ洢���ռ����ַ��Χ��1K�ֽ�
static void	__iomem	*Ch1BaseAddr1;		//PCI�Ĵ洢���ռ����ַ1�����ַ��Linux��ַ
//PCIFPGA�洢���ռ䣬����ַ2��bar0����1K�ֽڣ�
static unsigned long	ch2_mem2_base;		//PCI�Ĵ洢���ռ����ַ2�������ַ
static unsigned int	ch2_mem2_range;		//PCI�Ĵ洢���ռ����ַ��Χ��1K�ֽ�
static void	__iomem	*Ch2BaseAddr2;		//PCI�Ĵ洢���ռ����ַ2�����ַ��Linux��ַ
//PCIFPGA�洢���ռ䣬����ַ3��bar3����64K�ֽڣ�
static unsigned long	ch1_mem3_base;		//PCI�Ĵ洢���ռ����ַ3�������ַ0x90020000
static unsigned int	ch1_mem3_range;		//PCI�Ĵ洢���ռ����ַ��Χ��64K�ֽ�
static void	__iomem	*Ch1BaseAddr3;		//PCI�Ĵ洢���ռ����ַ3�����ַ��Linux��ַ
//PCIFPGA�洢���ռ䣬����ַ4��bar4����64K�ֽڣ�
static unsigned long	ch2_mem4_base;		//PCI�Ĵ洢���ռ����ַ4�������ַ0x90040000
static unsigned int	ch2_mem4_range;		//PCI�Ĵ洢���ռ����ַ��Χ��64K�ֽ�
static void	__iomem	*Ch2BaseAddr4;		//PCI�Ĵ洢���ռ����ַ3�����ַ��Linux��ַ
//PCIFPGA���ж�����
static int		pcifpga_irq;		//ͨ��1��ͨ��2��PCI���жϺ�
static int		m_second = 0;		//4����һ���жϣ��жϺ�m_second��1
static int		second_int = 0;		//1���1
#define MAX_M_SECOND	250			//ÿ����Ҫ��������������ֵ��4����*250=1��

//PCIFPGA����ر���
struct pci_dev		*pcifpga_card_dev;
int    major;

#define	DEVICE_NAME	"pcifpga"		//�豸����������,�ж�ʹ��,��PCIFPGA��û����
#define	PCIFPGA_MAJOR 	245			//�豸��������

struct pcifpga_dev {				/* �豸�ṹ��淶��ʽ */
	struct cdev cdev;			/* cdev�ṹ�� */
};
struct pcifpga_dev *pcifpga_devp;		/* �豸�ṹ��ָ�� */

//MPC8377CPU�йأ�MPC8377�����ַ��Ӧ�����ַ
#define SYSREGADDR	0xff400000		//IMMR reg address��System Configuration
#define DMAMR0		0x8100			//dma0 mode        reg,DMA0��ģʽ�Ĵ���
#define DMASR0		0x8104			//dma0 status      reg,DMA0��״̬�Ĵ���
#define DMASAR0		0x8110			//dma0 source      reg,DMA0��ԭ��ַ�Ĵ���
#define DMADAR0		0x8118			//dma0 destination reg,DMA0Ŀ�ĵ�ַ�Ĵ���
#define DMABCR0		0x8120			//dma0 byte count  reg,DMA0�ļ������Ĵ���
#define	SYSREGADDRLEN	0x100000		//sysreg base address��Ӧ��1M�ռ�
static void __iomem	*SysRegaddr;		//CPUϵͳ�Ĵ������ַ,����ַ0xff400000
//DMA��Ҫ�ı���
static unsigned int	*pcifpga_card_dma1;	//����һ�������Ļ���,2M�ֽ�����DMA����FIFO1����
static unsigned long	PciDmaBufPhyAddr1;	//FIFO1��DMA�����Ӧ�������ַ
static unsigned int	*pcifpga_card_dma2;	//����һ�������Ļ���,2M�ֽ�����DMA����FIFO2����
static unsigned long	PciDmaBufPhyAddr2;	//FIFO2��DMA�����Ӧ�������ַ
#define PCIFPGA_FIFO_BYTELEN	0x10000		//PCIFPGA���Կ���FIFO��16K˫��,û���õ�
#define PCIFPGA_FIFO_DWORDLEN	0x4000		//PCIFPGA���Կ���FIFO��64K�ֽ�,û���õ�
#define PCIFPGA_DMABUF_BYTELEN	0x80000		//0x1000	//0x80000	//����128K˫��DMA������,512K�ֽڵ�DMA������
#define PCIFPGA_DMABUF_DWORDLEN	0x20000		//0x400		//0x20000	//����128K˫��DMA������,512K�ֽڵ�DMA������
//PCIFPGA���Կ�ͨ��1·����
static unsigned int	ReadCh1Pointer = 0;	//��һ·��FIFO��ָ��,˫��ָ��
static unsigned int	WriteCh1Pointer = 0;	//��һ·��FIFOдָ��,˫��ָ��
static unsigned int	Ch1FifoFull = 0;	//��һ·��FIFO����־
static unsigned int	Ch1Cnt = 0;		//��һ·FIFO���յ���˫����
static unsigned int	Ch1DmaBufFull = 0;	//��һ·��DMABUF����־
//PCIFPGA���Կ�ͨ��2·����
static unsigned int	ReadCh2Pointer = 0;	//�ڶ�·��FIFO��ָ��,˫��ָ��
static unsigned int	WriteCh2Pointer = 0;	//�ڶ�·��FIFOдָ��,˫��ָ��
static unsigned int	Ch2FifoFull = 0;	//�ڶ�·��FIFO����־
static unsigned int	Ch2Cnt = 0;		//�ڶ�·FIFO���յ���˫����
static unsigned int	Ch2DmaBufFull = 0;	//��һ·��DMABUF����־

//��֡��Ҫ�ı���
#define PCIFPGA_PACKET_BYTELEN	0x800		//����2K˫�ֻ�����,�ݴ����İ�,���2K�ֽ�
#define PCIFPGA_PACKET_DWORDLEN	0x200		//����512˫�ֻ�����,�ݴ����İ�,���2K�ֽ�
#define PCIFPGA_BUF_BYTELEN	0x200000	//0x1000	//0x200000	//����512K˫�ֻ���,�洢��Ӧ�ö�ȡ�߽���İ�
#define PCIFPGA_BUF_DWORDLEN	0x80000		//0x400		//0x80000	//����512K˫�ֻ���,�洢��Ӧ�ö�ȡ�߽���İ�
#define	PACKET_MIN_LEN		0x18		//�趨����С����˫����,24˫��
#define	PACKET_OVERHEAD_LEN	0x08		//�趨����������˫����,08˫��
#define	PACKET_HEAD_LEN		0x01		//�趨����ͷ���˫����
#define	PACKET_FCS_LEN		0x01		//�趨��У�����˫����
#define	PACKET_TAIL_LEN		0x01		//�趨����β���˫����
#define	FRAME_HEAD_LEN		0x04		//�趨֡ͷ������ֽ���
#define	FRAME_LEN_LEN		0x02		//�趨֡֡������ֽ���
#define	FRAME_TIME_LEN		0x10		//�趨֡ʱ������ֽ���
#define	FRAME_STATUS_LEN	0x02		//�趨֡״̬����ֽ���
#define	FRAME_TAIL_LEN		0x04		//�趨֡β������ֽ���
#define	FRAME_MAX_LEN		0x612		//�趨��������˫����,16+1536+2=1554�ֽ�
#define	PACKET_HEADE		0x01a2b3c4d	//��ͷ
#define	PACKET_TAIL0		0x0a1b2c3d4	//��β1
#define	PACKET_TAIL1		0x0b2c3d4ff	//��β2
#define	PACKET_TAIL2		0x0c3d4ffff	//��β3
#define	PACKET_TAIL3		0x0d4ffffff	//��β4
#define	UTC20130101		0x050e1b680	//2013��1��1��0ʱ0��0���UTCʱ��
//��֡������
static unsigned int	*pcifpga_card_buf0;	//����һ��������1K˫�ֻ����֡
static unsigned int	*pcifpga_card_buf1;	//����һ��������512K˫�ֻ����֡
static unsigned int	*pcifpga_card_buf2;	//����һ��������512K˫�ֻ����֡
//PCIFPGA���Կ�ͨ��1·����
static unsigned int	ReadBuf1Pointer = 0;	//��һ·��BUF��ָ��,˫��ָ��
static unsigned int	ReadCh1Dword = 0;	//��һ·��BUF��˫����
static unsigned int	WriteBuf1Pointer = 0;	//��һ·��BUFдָ��,˫��ָ��
static unsigned int	Ch1PacketCnt = 0;	//��һ·�Ѿ�����İ���
static unsigned int	Ch1PacketDw = 0;	//��һ·�Ѿ������˫����
static unsigned int	Ch1BufFull = 0;		//��һ·��BUF����־
//PCIFPGA���Կ�ͨ��2·����
static unsigned int	ReadBuf2Pointer = 0;	//�ڶ�·��BUF��ָ��,˫��ָ��
static unsigned int	ReadCh2Dword = 0;	//�ڶ�·��BUF��˫����
static unsigned int	WriteBuf2Pointer = 0;	//�ڶ�·��BUFдָ��,˫��ָ��
static unsigned int	Ch2PacketCnt = 0;	//�ڶ�·�Ѿ�����İ���
static unsigned int	Ch2PacketDw = 0;	//�ڶ�·�Ѿ������˫����
static unsigned int	Ch2BufFull = 0;		//�ڶ�·��BUF����־

//Base Address 0 0��256�ֽڣ�˫�ֶ�д
//Configure.vģ��
#define SW1TRIGGERTIME		0x00	//D31��D08������D7��0����������ʱ��,ȱʡ1us,��С1us,���255us
#define SW2TRIGGERTIME		0x01	//D31��D08������D7��0����������ʱ��,ȱʡ1us,��С1us,���255us
#define SWALARMOUT		0x02	//D31��D02������D1��0�澯��2��1,ȱʡΪ��,1�պ�,0�Ͽ�
#define BEEPALARMOUT		0x03	//D31��D02������D1��1��һ��(����)�����,D0��1����,ȱʡ����
#define CHINTERFACESEL		0x04	//D31��D02������D1��0���ͨ��1��2,ȱʡ��,1��,0��(00ȫ��,01һ�����,02һ�����,03ȫ��)
#define GPSINTERFACESEL		0x05	//D31��D02������D1��1ѡ1588,0ѡIRIGB,D0��1ƽ��,0��ƽ��,ȱʡGPSƽ��
#define WARCHDOGOUT		0x06	//D31��D08������D7��0��ȫ1���Ź��������1��,Ȼ���Զ����D7��0����
#define SWTRIGGERTIME		0x0064	//����ʱ��1����
//Sw_in.vģ��,�����趨������ʱ������·�������ź�,������״̬�źź��ж��ź�
#define SW1STATUS		0x07	//D31��D02������D1=1���п�������,D0=1Ŀǰ�̵����պ�,D1�����ж�,��ѯ��D1���
#define SW2STATUS		0x08	//D31��D02������D1=1���п�������,D0=1Ŀǰ�̵����պ�,D1�����ж�,��ѯ��D1���
//Key_in.vģ��,��尴��ɨ��,����尴����15ms����������,��������״̬�źź��ж��ź�
#define KEY1STATUS		0x09	//D31��D02������D1=1���а�������,D0=1Ŀǰ�����պ�,D1�����ж�,��ѯ��D1���
#define KEY2STATUS		0x0A	//D31��D02������D1=1���а�������,D0=1Ŀǰ�����պ�,D1�����ж�,��ѯ��D1���
//Display.vģ��,���LED��ʾ,��ʾ����64bit,д������˫��,���ո�λ��ǰ��λ�ں��˳����λ��ʾ,ÿ����һ�����ݵı仯,����ʾ
#define LED1DISPLAY		0x0B	//D31��D00ʹ�ã�ǰ���LED��ʾ�ĵ�λ˫��,ȫ��ʹ��
#define LED2DISPLAY		0x0C	//D31��D00ʹ�ã�ǰ���LED��ʾ�ĵ�λ˫��,ȫ��ʹ��
//Irq.vģ��,���ø��ж���ز���,�ж�ʹ��,�����ж��ź�
#define INTERRUPTTIMER		0x0D	//D31��D16��������ʱ�ж�ʱ��,D15��0����1��65535ms,ȱʡ0���ж�
#define INTERRUPTENABLE		0x0E	//D31��D08������D7��,D6/5��2/1·FIFO��,D4/3����2/1,D2/1����2/1,0��ʱ,1ʹ��,ȱʡ0��ֹ
#define INTERRUPTSTATUS		0x0F	//D31��D07������D6/5��2/1·64K����D4/3����2/1��D2/1����2/1��0��ʱ��1�жϣ��������
#define INTTIMERV		0x0004	//4�����ж�
#define INTTIMERENABLE		0x00ff	//��ʱ�ж�ʹ��
#define INTTIMERDISABLE		0x0000	//��ʱ�жϽ�ֹ
//Eth_miim.vģ��,��̫���ӿڹ���ģ�飬ģ����̫��PHY����ӿڣ�MDC��MDIO������ɶ��ⲿ��̫��PHY�����DP83640���Ŀ��Ƽ�״̬��ȡ
#define ETHMIIMMDC		0x10	//D31��D08������MDC��Ƶ�Ĵ���,D7��0����ʱ�ӷ�Ƶ��(MDC),MDC����ʱ��33MHz��2��255��Ƶ���
#define ETHMIIMPHYADDREG	0x11	//D31��D13������PHY�ͼĴ�����ַ,D12��8PHY��ַ,D4��0�Ĵ�����ַ
#define ETHMIIMWRDATA		0x12	//D31��D16������D15��D0����д���������
#define ETHMIIMCOMMAND		0x13	//D31��D04������D3��1ɨ��,���ָܻ�,��0�˳�,D2��1��PHY,D1��1дPHY,D0��1��32bitȫ1ǰ����
#define ETHMIIMRDDATA		0x14	//D31��D16������D15��0�ⲿPHY״̬����
#define ETHMIIMPHYSTS		0x15	//D31��D03������PHY״̬�Ĵ���,D2=1Nvalid(��Ч),D1=1Busy(æ),D0=1LinkFail(����ʧ��)
//Timer.vģ��,���IRIG_B�����,��ʱ������ʱ�ӵļ�ʱ,Ϊ��·��̫��¼���ź��ṩʵʱ��ʱ��
#define IRIGB1588STATUS		0x20	//D31��D04������GPS��1588�澯,D3��0��Ӧ1588����,1588ʱ�Ӷ�ʧ,GPS����,GPSʱ�Ӷ�ʧ,����Ч
#define IRIGB1588TIMEERR	0x21	//D31��D25������D24=0����ʱ�ӵ���GPS,=1����GPS,D23��D0��������ʱ��ÿ��ƫ����,10MHz��λ
//CPU����IEEE1588��TOD(UTC)�����(ʮ����8421��)��һ����ʱ����(100ms)д��üĴ���,��IEEE1588��һʱ�̵�PPS����ʱ���뱾��ʱ�������
#define IRIGB1588WRSECOND	0x22	//D31��D31������D31-28��ʮ,D27-24��,D23-20����,D19-16ʮ,D15-12��,D11-8΢��,D7-4ʮ,D3-0��
#define IRIGB1588WRDAY		0x23	//D31��D26������D25-24���,D23-20ʮ.D19��16��,D13-12ʱʮ,D11-8��,D7-4��ʮ,D3��0��
#define IRIGB1588TIMEPUT	0x24	//D31��D08������D7��0��ȫ1,��0x22��0x23��ַ��ʱ�������ʱ��,D7��0д�������
#define IRIGB1588E		0x00ff
//CPU����IEEE1588��TOD��UTC�������ʮ����8421�룩��һ����ʱ���ڣ�100ms��д��üĴ�������IEEE1588��һʱ�̵�PPS����ʱ���뱾��ʱ�������
#define IRIGB1588TIMEGET	0x25	//D31��D08������D7��0��ȫ1,�ѵ�ǰ��ʱ��ʱ������0x26��0x27��ַ,D7��0д�������
#define IRIGB1588RDSECOND	0x26	//D31��D00ʹ�ã�D31-28��ʮ,D27-24��,D23-20����,D19-16ʮ,D15-12��,D11-8΢��,D7-4ʮ,D3-0��
#define IRIGB1588RDDAY		0x27	//D31��D26������D25-24���,D23-20ʮ.D19��16��,D13-12ʱʮ,D11-8��,D7-4��ʮ,D3��0��
//FIFO���ݡ�״̬������ͼ������Ĵ�����ƫ����
#define CH1FIFORESET		0x28	//D31��D08������D7��0��ȫ1���Ĵ洢FIFO��λ,״̬�ͼ�������λ,���Զ����
#define CH1FIFOSTATUS		0x29	//D31��D05����������FIFO״̬,D4��,D3��,D2����,D1����,D0����,����Ч,�����ж�
#define CH1FIFOCOUNT		0x2A	//D31��D30������ȡFIFO����,D29-16Ϊ��[13:0],D13-0Ϊд[13:0],��ǰд0xff,��ʱ1us,�ٶ�
#define CH2FIFORESET		0x2B	//D31��D08������D7��0��ȫ1���Ĵ洢FIFO��λ,״̬�ͼ�������λ,���Զ����
#define CH2FIFOSTATUS		0x2C	//D31��D05����������FIFO״̬,D4��,D3��,D2����,D1����,D0����,����Ч,�����ж�
#define CH2FIFOCOUNT		0x2D	//D31��D30������ȡFIFO����,D29-16Ϊ��[13:0],D13-0Ϊд[13:0],��ǰд0xff,��ʱ1us,�ٶ�
#define CHFIFORESET		0x00ff	//����FIFO�����ݸ�λʹ��,0xFF,D7��D0д���Զ����
#define CHFIFOREADE		0x00ff	//����FIFO��������ȡʹ��,0xFF,D7��D0д���Զ����

//Base Address 1 0��256�ֽڣ�˫�ֶ�д����1·��̫�����ڽ��ղ��ֿ��������״̬
//Base Address 2 0��256�ֽڣ�˫�ֶ�д����2·��̫�����ڽ��ղ��ֿ��������״̬
//Eth_rxregisters.vģ��
#define CHFILTERRESET		0x00	//D31��D00ʹ�ã�д0xA5A5A5A5��λ���Ĵ���ģ���������ģ��,������֮����1��
#define CHFILTERMODE		0x01	//D31��D02������D0=0Fileter,=1NoFilerer,D1=0ReceiveAllBroadcast,=1RejiectAllBroadcast
#define CHFILTERMINLEN		0x02	//D31��D16���������֡��,D15��0д��MinimumEthernetPacket,ȱʡ64
#define CHFILTERMAXLEN		0x03	//D31��D16�������֡��,D15��0д��MaximumEthernetPacket,ȱʡ1536
#define CHFILTERENABLE		0x04	//D31��D16������1=e,0=d,D0��9:VLAN1,VLAN2,GOOSE,SMV,MMS,����,MACԴ,VLAN����,MACԴ������,VLAN������
#define CHFILTERVID1		0x05	//D31��D12������D11��0,VlanID1
#define CHFILTERVID2		0x06	//D31��D12������D11��0,VlanID2
#define CHFILTERMACEN		0x07	//D31��D16������D15��0,SourceMac16��1,1=enable,0=disenable
#define CHFILTERMACH		0x08	//D31��D16������D15��0,SourceMacAdd,47��32bit
#define CHFILTERMACL		0x09	//D31��D00ʹ�ã�D31��0,SourceMacAdd,31��00bit
//�¹���
#define STOPRX			0x28	//D31��D01������D0=0��������֡,D0=1ֹͣ����֡,������������������
#define RESETCOUNTER		0x29	//D31��D08ʹ�ã�D7��0=AC����������ȫ����λ,������
//Eth_rxstatistics.vģ��,��������ͳ��ģ��ļ�������ֵ,ÿ��������4�ֽڳ���,ѭ������
//��֡���ֽڼ���,�㲥֡���ֽڼ���,�ಥ֡���ֽڼ���,CRC����֡����,���մ���֡����,�������ֽ�֡����,����֡����,����֡����
//���淽ʽ:ϵͳÿ�ζ���һ�ε�����ֵ,ֻ��4���ֽ�ȫ������ʱ��������,�������һ���ֽڶ�֮���������ֵ,�ṩ��һ�ζ�������ֵ
#define CHALLFRAMEH		0x30	//[63:32]
#define CHALLFRAMEL		0x31	//[31:00]
#define CHALLBYTEH		0x32	//[63:32]
#define CHALLBYTEL		0x33	//[31:00]
#define CHBROADCASTFRAMEH	0x34	//[63:32]
#define CHBROADCASTFRAMEL	0x35	//[31:00]
#define CHBROADCASTBYTEH	0x36	//[63:32]
#define CHBROADCASTBYTEL	0x37	//[31:00]
#define CHMULTCASTFRAMEH	0x38	//[63:32]
#define CHMULTCASTFRAMEL	0x39	//[31:00]
#define CHMULTCASTBYTEH		0x3A	//[63:32]
#define CHMULTCASTBYTEL		0x3B	//[31:00]
#define CHCRCERRFRAMEH		0x3C	//[63:32]
#define CHCRCERRFRAMEL		0x3D	//[31:00]
#define CHERRORFRAMEH		0x3E	//[63:32]
#define CHERRORFRAMEL		0x3F	//[31:00]
#define CHALLIGNERRFRAMEH	0x40	//[63:32]
#define CHALLIGNERRFRAMEL	0x41	//[31:00]
#define CHSHORTFRAMEH		0x42	//[63:32]
#define CHSHORTFRAMEL		0x43	//[31:00]
#define CHLONGFRAMEH		0x44	//[63:32]
#define CHLONGFRAMEL		0x45	//[31:00]

//��Ϣ����
#define	MSG_BUF_LEN	0x40		//������Ϣ����󳤶�
MsgCommand		*DrvMsgCommand;
MsgChInterface		*DrvChInterface;
MsgGpsInterface		*DrvGpsInterface;
MsgFpgaTime		*DrvFpgaTime;
MsgTriggerTime		*DrvTriggerTime;
MsgAlarmOut		*DrvAlarmOut;
MsgBeepAlarm		*DrvBeepAlarm;
MsgWarchDog		*DrvWarchDog;
MsgClearFifo		*DrvClearFifo;
MsgChFilter		*DrvChFilter;
MsgFilterMac		*DrvFilterMac;
MsgChStatus		*DrvChStatus;
MsgTriggerStatus	*DrvTriggerStatus;
MsgKeyStatus		*DrvKeyStatus;
MsgFrameCounters	*DrvFrameCounters;
MsgLedStatus		*DrvLedStatus;
MsgReadBuffer		*DrvReadBuffer;
static unsigned char	*MsgBuffer;

static int ReadFifo1(unsigned int FifoCnt)//FifoCnt��˫��Ϊ��λ
{
	unsigned int BufRemainLen, RemLen;
	//unsigned int i;

	//ֱ�Ӷ���FIFO����,�������������ݴ洢��DMA������1��
	if(ReadCh1Pointer <= WriteCh1Pointer) {
		if(((WriteCh1Pointer + FifoCnt) == PCIFPGA_DMABUF_DWORDLEN) && (ReadCh1Pointer == 0)) {
			Ch1DmaBufFull++;//���潫��
			//printk("TY-3012S PciDrv WriteCh1DmaBuf full!\n");//��ʾ������
			return 0;
		}
		if((WriteCh1Pointer + FifoCnt) <= PCIFPGA_DMABUF_DWORDLEN) {
			memcpy((pcifpga_card_dma1 + WriteCh1Pointer), Ch1BaseAddr3, 4*FifoCnt);
			#if 0
			printk("TY-3012S PciDrv Test Fifo1:    %08x!\n", FifoCnt);
			for(i = 0; i < FifoCnt; i++) {
				if((i % 0x04) == 0) printk("\n");
				printk(".%08x", *(pcifpga_card_dma1 + WriteCh1Pointer + i));
			}
			printk("!\n");
			#endif
			WriteCh1Pointer += FifoCnt;
			if(WriteCh1Pointer == PCIFPGA_DMABUF_DWORDLEN) WriteCh1Pointer = 0;
			Ch1Cnt += FifoCnt;
			//printk("TY-3012S PciDrv WrCh1Pointer:  %08x!\n", WriteCh1Pointer);//��ʾд��������
			return FifoCnt;
		}
		if((WriteCh1Pointer + FifoCnt) > PCIFPGA_DMABUF_DWORDLEN) {
			BufRemainLen = PCIFPGA_DMABUF_DWORDLEN - WriteCh1Pointer;
			RemLen = FifoCnt - BufRemainLen;
			if(RemLen >= ReadCh1Pointer) {//���潫��
				Ch1DmaBufFull++;
				//printk("TY-3012S PciDrv WriteCh1DmaBuf full!!\n");//��ʾ������
				return 0;
			}
			memcpy((pcifpga_card_dma1 + WriteCh1Pointer), Ch1BaseAddr3, 4*BufRemainLen);
			WriteCh1Pointer = 0;
			memcpy((pcifpga_card_dma1 + WriteCh1Pointer), Ch1BaseAddr3, 4*RemLen);
			WriteCh1Pointer = RemLen;
			Ch1Cnt += FifoCnt;
			//printk("TY-3012S PciDrv WrCh1Pointer:  %08x!!\n", WriteCh1Pointer);//��ʾд��������
			return FifoCnt;
		}
	} else {
		if((WriteCh1Pointer + FifoCnt) >= ReadCh1Pointer) {//���潫��
			Ch1DmaBufFull++;
			//printk("TY-3012S PciDrv WriteCh1DmaBuf full!!!\n");//��ʾ������
			return 0;
		} else {
			memcpy((pcifpga_card_dma1 + WriteCh1Pointer), Ch1BaseAddr3, 4*FifoCnt);
			WriteCh1Pointer += FifoCnt;
			Ch1Cnt += FifoCnt;
			//printk("TY-3012S PciDrv WrCh1Pointer:  %08x!!!\n", WriteCh1Pointer);//��ʾд��������
			return FifoCnt;
		}
	}
	return 0;
}

static int ReadFifo2(unsigned int FifoCnt)
{
	unsigned int BufRemainLen, RemLen;

	//ֱ�Ӷ���FIFO����,�������������ݴ洢��DMA������2��
	if(ReadCh2Pointer <= WriteCh2Pointer) {
		if(((WriteCh2Pointer + FifoCnt) == PCIFPGA_DMABUF_DWORDLEN) && (ReadCh2Pointer == 0)) {
			Ch2DmaBufFull++;//���潫��
			//printk("TY-3012S PciDrv WriteCh2DmaBuf full!\n");
			return 0;
		}
		if((WriteCh2Pointer + FifoCnt) <= PCIFPGA_DMABUF_DWORDLEN) {
			memcpy((pcifpga_card_dma2 + WriteCh2Pointer), Ch2BaseAddr4, 4*FifoCnt);
			WriteCh2Pointer += FifoCnt;
			if(WriteCh2Pointer == PCIFPGA_DMABUF_DWORDLEN) WriteCh2Pointer = 0;
			Ch2Cnt += FifoCnt;
			//printk("TY-3012S PciDrv WrCh2Pointer:  %08x!\n", WriteCh2Pointer);
			return FifoCnt;
		}
		if((WriteCh2Pointer + FifoCnt) > PCIFPGA_DMABUF_DWORDLEN) {
			BufRemainLen = PCIFPGA_DMABUF_DWORDLEN - WriteCh2Pointer;
			RemLen = FifoCnt - BufRemainLen;
			if(RemLen >= ReadCh2Pointer) {//���潫��
				Ch2DmaBufFull++;
				//printk("TY-3012S PciDrv WriteCh2DmaBuf full!!\n");
				return 0;
			}
			memcpy((pcifpga_card_dma2 + WriteCh2Pointer), Ch2BaseAddr4, 4*BufRemainLen);
			WriteCh2Pointer = 0;
			memcpy((pcifpga_card_dma2 + WriteCh2Pointer), Ch2BaseAddr4, 4*RemLen);
			WriteCh2Pointer = RemLen;
			Ch2Cnt += FifoCnt;
			//printk("TY-3012S PciDrv WrCh2Pointer:  %08x!!\n", WriteCh2Pointer);
			return FifoCnt;
		}
	} else {
		if((WriteCh2Pointer + FifoCnt) >= ReadCh2Pointer) {//���潫��
			Ch2DmaBufFull++;
			//printk("TY-3012S PciDrv WriteCh2DmaBuf full!!!\n");
			return 0;
		} else {
			memcpy((pcifpga_card_dma2 + WriteCh2Pointer), Ch2BaseAddr4, 4*FifoCnt);
			WriteCh2Pointer += FifoCnt;
			Ch2Cnt += FifoCnt;
			//printk("TY-3012S PciDrv WrCh2Pointer:  %08x!!!\n", WriteCh2Pointer);
			return FifoCnt;
		}
	}
	return 0;
}

static int ReadCh1Buf(unsigned int *buf, unsigned int len)//��1·���ݸ�Ӧ��,˫����
{
	unsigned int BufRemainLen, RemLen;

	if(ReadBuf1Pointer == WriteBuf1Pointer) {
		//printk("TY-3012S PciDrv ReadBuf1 Empty!\n");
		return 0;
	}
	if(ReadBuf1Pointer < WriteBuf1Pointer) {
		if((ReadBuf1Pointer + len) <= WriteBuf1Pointer) {
			copy_to_user(buf, pcifpga_card_buf1 + ReadBuf1Pointer, 4*len);
			ReadBuf1Pointer += len;
			ReadCh1Dword += len;
			//printk("TY-3012S PciDrv RdBuf1Pointer: %08x!\n", ReadBuf1Pointer);
			return len;
		} else {
			BufRemainLen = WriteBuf1Pointer - ReadBuf1Pointer;
			copy_to_user(buf, pcifpga_card_buf1 + ReadBuf1Pointer, 4*BufRemainLen);
			ReadBuf1Pointer += BufRemainLen;
			ReadCh1Dword += BufRemainLen;
			//printk("TY-3012S PciDrv RdBuf1Pointer: %08x!!\n", ReadBuf1Pointer);
			return BufRemainLen;
		}
	}
	if(ReadBuf1Pointer > WriteBuf1Pointer) {
		if((ReadBuf1Pointer + len) <= PCIFPGA_BUF_DWORDLEN) {
			copy_to_user(buf, pcifpga_card_buf1 + ReadBuf1Pointer, 4*len);
			ReadBuf1Pointer += len;
			if(ReadBuf1Pointer == PCIFPGA_BUF_DWORDLEN) ReadBuf1Pointer = 0;
			ReadCh1Dword += len;
			//printk("TY-3012S PciDrv RdBuf1Pointer: %08x!!!\n", ReadBuf1Pointer);
			return len;
		}
		if((ReadBuf1Pointer + len) > PCIFPGA_BUF_DWORDLEN) {
			BufRemainLen = PCIFPGA_BUF_DWORDLEN - ReadBuf1Pointer;
			RemLen = len - BufRemainLen;
			copy_to_user(buf, pcifpga_card_buf1 + ReadBuf1Pointer, 4*BufRemainLen);
			ReadBuf1Pointer = 0;
			ReadCh1Dword += BufRemainLen;
			return BufRemainLen;
			if(RemLen >= WriteBuf1Pointer) {
				copy_to_user((buf + BufRemainLen), pcifpga_card_buf1, 4*WriteBuf1Pointer);
				ReadBuf1Pointer += WriteBuf1Pointer;
				ReadCh1Dword += WriteBuf1Pointer;
				return BufRemainLen + WriteBuf1Pointer;
			} else {
				copy_to_user((buf + BufRemainLen), pcifpga_card_buf1, 4*RemLen);
				ReadBuf1Pointer += RemLen;
				ReadCh1Dword += RemLen;
				return len;
			}
			//printk("TY-3012S PciDrv RdBuf1Pointer: %08x!!!!\n", ReadBuf1Pointer);
		}
	}
	return 0;
}

static int ReadCh2Buf(unsigned int *buf, unsigned int len)//��2·���ݸ�Ӧ��,˫����
{
	unsigned int BufRemainLen, RemLen;

	if(ReadBuf2Pointer == WriteBuf2Pointer) {
		//printk("TY-3012S PciDrv ReadBuf2 Empty!\n");
		return 0;
	}
	if(ReadBuf2Pointer < WriteBuf2Pointer) {
		if((ReadBuf2Pointer + len) <= WriteBuf2Pointer) {
			copy_to_user(buf, pcifpga_card_buf2 + ReadBuf2Pointer, 4*len);
			ReadBuf2Pointer += len;
			ReadCh2Dword += len;
			//printk("TY-3012S PciDrv RdBuf2Pointer: %08x!\n", ReadBuf2Pointer);
			return len;
		} else {
			BufRemainLen = WriteBuf2Pointer - ReadBuf2Pointer;
			copy_to_user(buf, pcifpga_card_buf2 + ReadBuf2Pointer, 4*BufRemainLen);
			ReadBuf2Pointer += BufRemainLen;
			ReadCh2Dword += BufRemainLen;
			//printk("TY-3012S PciDrv RdBuf2Pointer: %08x!!\n", ReadBuf2Pointer);
			return BufRemainLen;
		}
	}
	if(ReadBuf2Pointer > WriteBuf2Pointer) {
		if((ReadBuf2Pointer + len) <= PCIFPGA_BUF_DWORDLEN) {
			copy_to_user(buf, pcifpga_card_buf2 + ReadBuf2Pointer, 4*len);
			ReadBuf2Pointer += len;
			if(ReadBuf2Pointer == PCIFPGA_BUF_DWORDLEN) ReadBuf2Pointer = 0;
			ReadCh2Dword += len;
			//printk("TY-3012S PciDrv RdBuf2Pointer: %08x!!!\n", ReadBuf2Pointer);
			return len;
		}
		if((ReadBuf2Pointer + len) > PCIFPGA_BUF_DWORDLEN) {
			BufRemainLen = PCIFPGA_BUF_DWORDLEN - ReadBuf2Pointer;
			RemLen = len - BufRemainLen;
			copy_to_user(buf, pcifpga_card_buf2 + ReadBuf2Pointer, 4*BufRemainLen);
			ReadBuf2Pointer = 0;
			ReadCh2Dword += BufRemainLen;
			return BufRemainLen;
			if(RemLen >= WriteBuf2Pointer) {
				copy_to_user((buf + BufRemainLen), pcifpga_card_buf2, 4*WriteBuf2Pointer);
				ReadBuf2Pointer += WriteBuf2Pointer;
				ReadCh2Dword += WriteBuf2Pointer;
				return BufRemainLen + WriteBuf2Pointer;
			} else {
				copy_to_user((buf + BufRemainLen), pcifpga_card_buf2, 4*RemLen);
				ReadBuf2Pointer += RemLen;
				ReadCh2Dword += RemLen;
				return len;
			}
			//printk("TY-3012S PciDrv RdBuf2Pointer: %08x!!!!\n", ReadBuf2Pointer);
		}
	}
	return 0;
}

static int WriteCh1Buf(unsigned int PacketLen)//˫��
{
	unsigned int BufRemainLen, RemLen;
	unsigned char *ptr0, *ptr1;
	unsigned int PacketStatusH, PacketStatusL;
	unsigned int FrameLen, PacketPadLen;
	unsigned int dd, hh, mm, ss, ms, us;
	unsigned int pcapss, pcapus, pcaplen;
	//unsigned int i;

	FrameLen = *(pcifpga_card_buf0 + (2*PACKET_HEAD_LEN)) >> 0x10;//ȷ������,��Ϊ����4�ֽ�,˫�ֵ�λ
	PacketPadLen = (FrameLen + FRAME_LEN_LEN) % 4;//ȷ��������ֽ���
	ptr0 = (unsigned char *)(pcifpga_card_buf0) + FrameLen + 2*FRAME_HEAD_LEN;//��״̬λ��
	PacketStatusH = *ptr0;
	PacketStatusL = *(ptr0 + 1);
	ptr0 = (unsigned char *)(pcifpga_card_buf0);//�����ֽڿ���
	ptr1 = (unsigned char *)(pcifpga_card_buf1);//�����ֽڿ���
	memcpy((ptr0 + FRAME_STATUS_LEN), (ptr0 + FRAME_HEAD_LEN), (FRAME_HEAD_LEN + FRAME_LEN_LEN));//��λ��ͷ��֡��λ��,�ڳ�֡״̬�ֽڵ�λ��
	*(ptr0 + 2*FRAME_HEAD_LEN) = PacketStatusH;
	*(ptr0 + 2*FRAME_HEAD_LEN + 1) = PacketStatusL;
	memcpy((ptr0 + 2*FRAME_HEAD_LEN + FrameLen), (ptr0 + 2*FRAME_HEAD_LEN + FRAME_LEN_LEN + FrameLen), (PacketPadLen + 3*FRAME_TAIL_LEN));//��λȥ��β��֡״̬
	#if 0
	printk("TY-3012S PciDrv WriteCh1Buf:   %08x!", 4*PacketLen);
	for(i = 0; i < 4*PacketLen; i++) {
		if((i % 16) == 0) printk("\n");
		printk(".%02x", *(ptr0 + 2 + i));
	}
	printk("!\n");
	#endif
	//����ʱ��
	dd = (*(ptr0 + 17) & 0x0f)*100 + (*(ptr0 + 18) >> 4)*10 + (*(ptr0 + 18) & 0x0f);//4*FRAME_HEAD_LEN+FRAME_LEN_LEN+0=18;
	if(dd > 0) dd -= 1;
	hh = (*(ptr0 + 19) >> 4)*10 + (*(ptr0 + 19) & 0x0f);//4*FRAME_HEAD_LEN+FRAME_LEN_LEN+1=19;
	mm = (*(ptr0 + 20) >> 4)*10 + (*(ptr0 + 20) & 0x0f);//4*FRAME_HEAD_LEN+FRAME_LEN_LEN+2=20;
	ss = (*(ptr0 + 21) >> 4)*10 + (*(ptr0 + 21) & 0x0f);//4*FRAME_HEAD_LEN+FRAME_LEN_LEN+3=21;
	ms = (*(ptr0 + 22)*100) + (*(ptr0 + 23) >> 4)*10 + (*(ptr0 + 23) & 0x0f);//5*FRAME_HEAD_LEN+FRAME_LEN_LEN+0/1=22/23;
	us = (*(ptr0 + 24)*100) + (*(ptr0 + 25) >> 4)*10 + (*(ptr0 + 25) & 0x0f);//5*FRAME_HEAD_LEN+FRAME_LEN_LEN+2/3=24/25;
	pcapss = UTC20130101 + 86400*dd + 3600*hh + 60*mm + ss;//24*3600*dd + 3600*hh + 60*mm + ss;
	*(ptr0 + 10) = pcapss & 0x000000ff;
	pcapss >>= 8;
	*(ptr0 + 11) = pcapss & 0x000000ff;
	pcapss >>= 8;
	*(ptr0 + 12) = pcapss & 0x000000ff;
	pcapss >>= 8;
	*(ptr0 + 13) = pcapss & 0x000000ff;
	pcapus = 1000*ms + us;
	*(ptr0 + 14) = pcapus & 0x000000ff;
	pcapus >>= 8;
	*(ptr0 + 15) = pcapus & 0x000000ff;
	pcapus >>= 8;
	*(ptr0 + 16) = pcapus & 0x000000ff;
	pcapus >>= 8;
	*(ptr0 + 17) = pcapus & 0x000000ff;
	pcaplen = FrameLen - 18;//16�ֽ�ʱ��+n�ֽ�֡+2�ֽ�״̬
	*(ptr0 + 22) = *(ptr0 + 18) = pcaplen & 0x000000ff;
	pcaplen >>= 8;
	*(ptr0 + 23) = *(ptr0 + 19) = pcaplen & 0x000000ff;
	*(ptr0 + 24) = *(ptr0 + 20) = 0;
	*(ptr0 + 25) = *(ptr0 + 21) = 0;
	#if 0
	printk("TY-3012S PciDrv WriteCh1Buf:   %08x!!", 4*PacketLen);
	for(i = 0; i < 4*PacketLen; i++) {
		if((i % 16) == 0) printk("\n");
		printk(".%02x", *(ptr0 + 2 + i));
	}
	printk("!\n");
	#endif
	//�����������֡���͸�ͨ��1�����ݻ�����
	if(ReadBuf1Pointer <= WriteBuf1Pointer) {
		if(((WriteBuf1Pointer + PacketLen) == PCIFPGA_BUF_DWORDLEN) && (ReadBuf1Pointer == 0)) {
			Ch1BufFull++;//���潫��
			//printk("TY-3012S PciDrv WriteCh1Buf full!\n");//��ʾ������
			return 0;
		}
		if((WriteBuf1Pointer + PacketLen) <= PCIFPGA_BUF_DWORDLEN) {
			memcpy((ptr1 + 4*WriteBuf1Pointer), (ptr0 + 2), 4*PacketLen);//��������:ʱ��+����+У��+֡β
			#if 0
			printk("TY-3012S PciDrv WriteCh1Buf:   %08x!!!", PacketLen);
			for(i = 0; i < PacketLen; i++) {
				if((i % 0x04) == 0) printk("\n");
				printk(".%08x", *(pcifpga_card_buf1 + WriteBuf1Pointer + i));
			}
			printk("!\n");
			#endif
			WriteBuf1Pointer += PacketLen;
			if(WriteBuf1Pointer == PCIFPGA_BUF_DWORDLEN) WriteBuf1Pointer = 0;
			Ch1PacketCnt ++;
			Ch1PacketDw += PacketLen;
			//printk("TY-3012S PciDrv WrBuf1Pointer: %08x!\n", WriteBuf1Pointer);//��ʾд��������
			return PacketLen;
		}
		if((WriteBuf1Pointer + PacketLen) > PCIFPGA_BUF_DWORDLEN) {
			BufRemainLen = PCIFPGA_BUF_DWORDLEN - WriteBuf1Pointer;
			if((PacketLen - BufRemainLen) >= ReadBuf1Pointer) {//���潫��
				Ch1BufFull++;
				//printk("TY-3012S PciDrv WriteCh1Buf full!!\n");//��ʾ������
				return 0;
			}
			memcpy((ptr1 + 4*WriteBuf1Pointer), (ptr0 + 2), 4*BufRemainLen);
			WriteBuf1Pointer = 0;
			RemLen = PacketLen - BufRemainLen;
			memcpy(ptr1, (ptr0 + 2 + 4*BufRemainLen), 4*RemLen);
			WriteBuf1Pointer = RemLen;
			Ch1PacketCnt ++;
			Ch1PacketDw += PacketLen;
			//printk("TY-3012S PciDrv WrBuf1Pointer: %08x!!\n", WriteBuf1Pointer);//��ʾд��������
			return PacketLen;
		}
	} else {
		if((WriteBuf1Pointer + PacketLen) >= ReadBuf1Pointer) {//���潫��
			Ch1BufFull++;
			printk("TY-3012S PciDrv WriteCh1Buf full!!!\n");//��ʾ������
			return 0;
		} else {
			memcpy((ptr1 + 4*WriteBuf1Pointer), (ptr0 + 2), 4*PacketLen);//��������:ʱ��+����+У��+֡β
			WriteBuf1Pointer += PacketLen;
			Ch1PacketCnt ++;
			Ch1PacketDw += PacketLen;
			//printk("TY-3012S PciDrv WrBuf1Pointer: %08x!!!\n", WriteBuf1Pointer);//��ʾд��������
			return PacketLen;
		}
	}
	return 0;
}

static int WriteCh2Buf(unsigned int PacketLen)//˫��
{
	unsigned int BufRemainLen, RemLen;
	unsigned char *ptr0, *ptr2;
	unsigned int PacketStatusH, PacketStatusL;
	unsigned int FrameLen, PacketPadLen;
	unsigned int dd, hh, mm, ss, ms, us;
	unsigned int pcapss, pcapus, pcaplen;

	FrameLen = *(pcifpga_card_buf0 + (2*PACKET_HEAD_LEN)) >> 0x10;//ȷ������,��Ϊ����4�ֽ�,˫�ֵ�λ
	PacketPadLen = (FrameLen + FRAME_LEN_LEN) % 4;//ȷ��������ֽ���
	ptr0 = (unsigned char *)(pcifpga_card_buf0) + FrameLen + 2*FRAME_HEAD_LEN;//��״̬λ��
	PacketStatusH = *ptr0;
	PacketStatusL = *(ptr0 + 1);
	ptr0 = (unsigned char *)(pcifpga_card_buf0);//�����ֽڿ���
	ptr2 = (unsigned char *)(pcifpga_card_buf2);//�����ֽڿ���
	memcpy((ptr0 + FRAME_STATUS_LEN), (ptr0 + FRAME_HEAD_LEN), (FRAME_HEAD_LEN + FRAME_LEN_LEN));//��λ��ͷ��֡��λ��,�ڳ�֡״̬�ֽڵ�λ��
	*(ptr0 + 2*FRAME_HEAD_LEN) = PacketStatusH;
	*(ptr0 + 2*FRAME_HEAD_LEN + 1) = PacketStatusL;
	memcpy((ptr0 + 2*FRAME_HEAD_LEN + FrameLen), (ptr0 + 2*FRAME_HEAD_LEN + FRAME_LEN_LEN + FrameLen), (PacketPadLen + 3*FRAME_TAIL_LEN));//��λȥ��β��֡״̬
	//����ʱ��
	dd = (*(ptr0 + 17) & 0x0f)*100 + (*(ptr0 + 18) >> 4)*10 + (*(ptr0 + 18) & 0x0f);//4*FRAME_HEAD_LEN+FRAME_LEN_LEN+0=18;
	if(dd > 0) dd -= 1;
	hh = (*(ptr0 + 19) >> 4)*10 + (*(ptr0 + 19) & 0x0f);//4*FRAME_HEAD_LEN+FRAME_LEN_LEN+1=19;
	mm = (*(ptr0 + 20) >> 4)*10 + (*(ptr0 + 20) & 0x0f);//4*FRAME_HEAD_LEN+FRAME_LEN_LEN+2=20;
	ss = (*(ptr0 + 21) >> 4)*10 + (*(ptr0 + 21) & 0x0f);//4*FRAME_HEAD_LEN+FRAME_LEN_LEN+3=21;
	ms = (*(ptr0 + 22)*100) + (*(ptr0 + 23) >> 4)*10 + (*(ptr0 + 23) & 0x0f);//5*FRAME_HEAD_LEN+FRAME_LEN_LEN+0/1=22/23;
	us = (*(ptr0 + 24)*100) + (*(ptr0 + 25) >> 4)*10 + (*(ptr0 + 25) & 0x0f);//5*FRAME_HEAD_LEN+FRAME_LEN_LEN+2/3=24/25;
	pcapss = UTC20130101 + 86400*dd + 3600*hh + 60*mm + ss;//24*3600*dd + 3600*hh + 60*mm + ss;
	*(ptr0 + 10) = pcapss & 0x000000ff;
	pcapss >>= 8;
	*(ptr0 + 11) = pcapss & 0x000000ff;
	pcapss >>= 8;
	*(ptr0 + 12) = pcapss & 0x000000ff;
	pcapss >>= 8;
	*(ptr0 + 13) = pcapss & 0x000000ff;
	pcapus = 1000*ms + us;
	*(ptr0 + 14) = pcapus & 0x000000ff;
	pcapus >>= 8;
	*(ptr0 + 15) = pcapus & 0x000000ff;
	pcapus >>= 8;
	*(ptr0 + 16) = pcapus & 0x000000ff;
	pcapus >>= 8;
	*(ptr0 + 17) = pcapus & 0x000000ff;
	pcaplen = FrameLen - 18;//16�ֽ�ʱ��+n�ֽ�֡+2�ֽ�״̬
	*(ptr0 + 22) = *(ptr0 + 18) = pcaplen & 0x000000ff;
	pcaplen >>= 8;
	*(ptr0 + 23) = *(ptr0 + 19) = pcaplen & 0x000000ff;
	*(ptr0 + 24) = *(ptr0 + 20) = 0;
	*(ptr0 + 25) = *(ptr0 + 21) = 0;
	//�����������֡���͸�ͨ��2�����ݻ�����
	if(ReadBuf2Pointer <= WriteBuf2Pointer) {
		if(((WriteBuf2Pointer + PacketLen) == PCIFPGA_BUF_DWORDLEN) && (ReadBuf2Pointer == 0)) {
			Ch2BufFull++;//���潫��
			//printk("TY-3012S PciDrv WriteCh2Buf full!\n");
			return 0;
		}
		if((WriteBuf2Pointer + PacketLen) <= PCIFPGA_BUF_DWORDLEN) {
			memcpy((ptr2 + 4*WriteBuf2Pointer), (ptr0 + 2), 4*PacketLen);//��������:ʱ��+����+У��+֡β
			WriteBuf2Pointer += PacketLen;
			if(WriteBuf2Pointer == PCIFPGA_BUF_DWORDLEN) WriteBuf2Pointer = 0;
			Ch2PacketCnt ++;
			Ch2PacketDw += PacketLen;
			//printk("TY-3012S PciDrv WrBuf2Pointer: %08x!\n", WriteBuf2Pointer);
			return PacketLen;
		}
		if((WriteBuf2Pointer + PacketLen) > PCIFPGA_BUF_DWORDLEN) {
			BufRemainLen = PCIFPGA_BUF_DWORDLEN - WriteBuf2Pointer;
			if((PacketLen - BufRemainLen) >= ReadBuf2Pointer) {//���潫��
				Ch2BufFull++;
				//printk("TY-3012S PciDrv WriteCh2Buf full!!\n");
				return 0;
			}
			memcpy((ptr2 + 4*WriteBuf2Pointer), (ptr0 + 2), 4*BufRemainLen);
			WriteBuf2Pointer = 0;
			RemLen = PacketLen - BufRemainLen;
			memcpy(ptr2 , (ptr0 + 2 + 4*BufRemainLen), 4*RemLen);
			WriteBuf2Pointer = RemLen;
			Ch2PacketCnt ++;
			Ch2PacketDw += PacketLen;
			//printk("TY-3012S PciDrv WrBuf2Pointer: %08x!!\n", WriteBuf2Pointer);
			return PacketLen;
		}
	} else {
		if((WriteBuf2Pointer + PacketLen) >= ReadBuf2Pointer) {//���潫��
			Ch2BufFull++;
			//printk("TY-3012S PciDrv WriteCh2Buf full!!!\n");
			return 0;
		} else {
			memcpy((ptr2 + 4*WriteBuf2Pointer), (ptr0 + 2), 4*PacketLen);//��������:ʱ��+����+У��+֡β
			WriteBuf2Pointer += PacketLen;
			Ch2PacketCnt ++;
			Ch2PacketDw += PacketLen;
			//printk("TY-3012S PciDrv WrBuf2Pointer: %08x!!!\n", WriteBuf2Pointer);
			return PacketLen;
		}
	}
	return 0;
}

static int ReadFrameBuf1(void)
{
	unsigned int CanReadLen;//Ŀǰ����������Ч���ֽ���
	unsigned int *pPacketHead, *pPacketTail;//�����ͷ�Ͱ�βָ��
	unsigned int FrameLen, FramePadLen, PacketPadLen;//����֡���ֽ�,16�ֽ�ʱ��+�����ֽ�+2�ֽ�״̬,����ֽ���0��3,����˫����
	unsigned int PacketLen, bufRemLen, remLen, PacketTailTrue;

	//���ı�������˫��(4Byte)Ϊ��λ,������С����PACKET_MIN_LEN=24��˫��=96�ֽ�
	//ԭ��:4�ֽڰ�ͷ+2�ֽڳ���+16�ֽ�ʱ��+64�ֽ�֡+2�ֽ�״̬+4�ֽ�У��+4�ֽڰ�β=96�ֽ�=24��˫��
	//�°�:4�ֽڰ�ͷ+2�ֽڳ���+2�ֽ�״̬+16�ֽ�ʱ��+64�ֽ�֡+4�ֽ�У��+4�ֽڰ�β=96�ֽ�=24��˫��
	//FrameLen=���յ���֡�ֽ���+16�ֽ�ʱ��+2�ֽ�֡״̬
	if(ReadCh1Pointer <= WriteCh1Pointer)
		CanReadLen = WriteCh1Pointer - ReadCh1Pointer;//��������ܵĳ���,˫��Ϊ��λ
	else	CanReadLen = PCIFPGA_DMABUF_DWORDLEN - ReadCh1Pointer + WriteCh1Pointer;//˫�ּ���
	if(CanReadLen < PACKET_MIN_LEN) return 0;//������С��24��˫��,��Ҫ�ȴ�
	//��ʼ�ڰ���Ѱ��֡,���ҽ��������
	while(CanReadLen >= PACKET_MIN_LEN) {//�ﵽ24��˫���ٿ�ʼѰ��֡
		if(ReadCh1Pointer < WriteCh1Pointer) {
			pPacketHead = pcifpga_card_dma1 + ReadCh1Pointer;//��Ϊ˫�ֱ���,��ͷλ��ָ��
			if(*pPacketHead == PACKET_HEADE) {//�ҵ���ͷ0x1a2b3c4d
				//printk("TY-3012S PciDrv *pPacketHead:  %08x!\n", *pPacketHead);//��ʾ�ҵ���ͷ
				FrameLen = *(pcifpga_card_dma1 + ReadCh1Pointer + PACKET_HEAD_LEN) >> 0x10;//����2�ֽ�,�ڸ�λ
				//printk("TY-3012S PciDrv FrameLen:      %08x!\n", FrameLen);//��ʾ֡����
				if(FrameLen > FRAME_MAX_LEN) {//��̫��֡��+32�ֽڿ���
					printk("TY-3012S PciDrv FrameLenE:     %08d!\n", FrameLen);//��ʾ�����ȴ���,�ǳ�����
					ReadCh1Pointer++;//֡��̫������,����1��˫��,����
					CanReadLen--;
					continue;//����ִ��while
				}
				//��������ֽ���:֡ͷ+֡����+֡ʱ��+֡����+֡״̬+֡У��+֡β+���:������˫�ֽ�
				FramePadLen = (FRAME_LEN_LEN + FrameLen) % 4;//��˫��,��������ֽ�����
				if(FramePadLen == 0) PacketPadLen = 0;//�����,˫�ֵı���
				else PacketPadLen = 1;//�����,��˫�ֱ���,1����˫�ֽ����
				//��������ĳ���,˫����
				PacketLen = PACKET_HEAD_LEN + ((FRAME_LEN_LEN + FrameLen) / 0x04) + PacketPadLen + PACKET_FCS_LEN + PACKET_TAIL_LEN;
				//printk("TY-3012S PciDrv PacketLen:     %08x!\n", PacketLen);//��ʾ���ܳ���
				if(PacketLen <= CanReadLen) {//�����ȷ���Ҫ��,�����ҵ���β
					pPacketTail = pcifpga_card_dma1 + ReadCh1Pointer + PacketLen - PACKET_TAIL_LEN;
					PacketTailTrue = 0;
					switch(FramePadLen) {
						case 0: if(*pPacketTail == PACKET_TAIL0) PacketTailTrue = 1; break;//0xa1b2c3d4
						case 1: if(*pPacketTail == PACKET_TAIL3) PacketTailTrue = 1; break;//0xd4ffffff
						case 2: if(*pPacketTail == PACKET_TAIL2) PacketTailTrue = 1; break;//0xc3d4ffff
						case 3: if(*pPacketTail == PACKET_TAIL1) PacketTailTrue = 1; break;//0xb2c3d4ff
						default: PacketTailTrue = 0; break;
					}
					if(PacketTailTrue == 1) {//�ҵ���һ����Ч�İ�β
						//printk("TY-3012S PciDrv *pPacketTail:  %08x!\n", *pPacketTail);//��ʾ�ҵ���β
						memcpy((pcifpga_card_buf0 + PACKET_HEAD_LEN), (pcifpga_card_dma1 + ReadCh1Pointer), 4*PacketLen);
						ReadCh1Pointer += PacketLen;
						return PacketLen;
					} else {//û���ҵ�֡β
						printk("TY-3012S PciDrv *pPacketTailE: %08x!\n", *pPacketTail);//��ʾ��β����,�ǳ�����
						ReadCh1Pointer++;//���ǰ�β
						CanReadLen--;
					}
				} else {
					return 0;//���յ�����̫��,��Ҫ�ȴ���һ��������
				}
			} else {
				//printk("TY-3012S PciDrv *pPacketHeadE: %08x!\n", *pPacketHead);//��ʾ���ְ�ͷ����,��̫����
				ReadCh1Pointer++;//���ǰ�ͷ,����1��˫��
				CanReadLen--;
			}
		} else {
			pPacketHead = pcifpga_card_dma1 + ReadCh1Pointer;//��Ϊ˫�ֱ���,��ͷλ��ָ��
			if(*pPacketHead == PACKET_HEADE) {//�ҵ���ͷPACKET_HEADE=0x1a2b3c4d
				//printk("TY-3012S PciDrv *pPacketHead:  %08x!!\n", *pPacketHead);//��ʾ�ҵ���ͷ
				if((ReadCh1Pointer + PACKET_HEAD_LEN) == PCIFPGA_DMABUF_DWORDLEN)
					FrameLen = *pcifpga_card_dma1 >> 0x10;//����2�ֽ�,�ڸ�λ,�ֽڼ���
				else	FrameLen = *(pcifpga_card_dma1 + ReadCh1Pointer + PACKET_HEAD_LEN) >> 0x10;//����2�ֽ�,�ڸ�λ,�ֽڼ���
				//printk("TY-3012S PciDrv FrameLen:      %08x!!\n", FrameLen);//��ʾ֡����
				if(FrameLen > FRAME_MAX_LEN) {//��̫��֡��+32�ֽڿ���
					printk("TY-3012S PciDrv FrameLenE:     %08d!!\n", FrameLen);//��ʾ֡���ȴ���,�ǳ�����
					if((ReadCh1Pointer + PACKET_HEAD_LEN) == PCIFPGA_DMABUF_DWORDLEN) ReadCh1Pointer = 0;
					else ReadCh1Pointer++;//֡��̫������,����1��˫��,����
					CanReadLen--;
					continue;//����ִ��while
				}
				//��������ֽ���:֡ͷ+֡����+֡ʱ��+֡����+֡״̬+֡У��+֡β+���:������˫�ֽ�
				FramePadLen = (FRAME_LEN_LEN + FrameLen) % 4;//��˫��,��������ֽ�����
				if(FramePadLen == 0) PacketPadLen = 0;//�����,˫�ֵı���
				else PacketPadLen = 1;//�����,��˫�ֱ���,1����˫�ֽ����
				//��������ĳ���,˫����
				PacketLen = PACKET_HEAD_LEN + ((FRAME_LEN_LEN + FrameLen) / 0x04) + PacketPadLen + PACKET_FCS_LEN + PACKET_TAIL_LEN;
				//printk("TY-3012S PciDrv PacketLen:     %08x!!\n", PacketLen);//��ʾ���ܳ���
				if(PacketLen <= CanReadLen) {//�����ȷ���Ҫ��,�����ҵ���β
					if((ReadCh1Pointer + PacketLen) > PCIFPGA_DMABUF_DWORDLEN) {
						pPacketTail = pcifpga_card_dma1 + ReadCh1Pointer + PacketLen - PCIFPGA_DMABUF_DWORDLEN - PACKET_TAIL_LEN;
					} else	{
						pPacketTail = pcifpga_card_dma1 + ReadCh1Pointer + PacketLen - PACKET_TAIL_LEN;
					}
					PacketTailTrue = 0;
					switch(FramePadLen) {
						case 0: if(*pPacketTail == PACKET_TAIL0) PacketTailTrue = 1; break;//0xa1b2c3d4
						case 1: if(*pPacketTail == PACKET_TAIL3) PacketTailTrue = 1; break;//0xd4ffffff
						case 2: if(*pPacketTail == PACKET_TAIL2) PacketTailTrue = 1; break;//0xc3d4ffff
						case 3: if(*pPacketTail == PACKET_TAIL1) PacketTailTrue = 1; break;//0xb2c3d4ff
						default: PacketTailTrue = 0; break;
					}
					if(PacketTailTrue == 1){//�ҵ���һ����Ч�İ�β
						//printk("TY-3012S PciDrv *pPacketTail:  %08x!!\n", *pPacketTail);//��ʾ�ҵ���β
						if((ReadCh1Pointer + PacketLen) <= PCIFPGA_DMABUF_DWORDLEN) {
							memcpy((pcifpga_card_buf0 + PACKET_HEAD_LEN), (pcifpga_card_dma1 + ReadCh1Pointer), 4*PacketLen);
							ReadCh1Pointer += PacketLen;
							if(ReadCh1Pointer == PCIFPGA_DMABUF_DWORDLEN) ReadCh1Pointer = 0;
							return PacketLen;
						}
						if((ReadCh1Pointer + PacketLen) > PCIFPGA_DMABUF_DWORDLEN) {
							bufRemLen = PCIFPGA_DMABUF_DWORDLEN - ReadCh1Pointer;
							memcpy((pcifpga_card_buf0 + PACKET_HEAD_LEN), (pcifpga_card_dma1 + ReadCh1Pointer), 4*bufRemLen);
							ReadCh1Pointer = 0;
							remLen = PacketLen - bufRemLen;
							memcpy((pcifpga_card_buf0 + PACKET_HEAD_LEN + bufRemLen), pcifpga_card_dma1, 4*remLen);
							ReadCh1Pointer += remLen;
							return PacketLen;
						}
					} else {//��Ӧû���ҵ���һ����ͷ
						printk("TY-3012S PciDrv *pPacketTailE: %08x!!\n", *pPacketTail);//��ʾ��β����,�ǳ�����
						if((ReadCh1Pointer + 1) == PCIFPGA_DMABUF_DWORDLEN) ReadCh1Pointer = 0;
						else ReadCh1Pointer++;//����1��˫��
						CanReadLen--;
					}
				} else {//���յ�����̫��,��Ҫ�ȴ���һ��������
					return 0;
				}
			} else {//��Ӧû���ҵ�֡ͷ
				//printk("TY-3012S PciDrv *pPacketHeadE: %08x!!\n", *pPacketHead);//��ʾ���ְ�ͷ����,��̫����
				if((ReadCh1Pointer + 1) == PCIFPGA_DMABUF_DWORDLEN) ReadCh1Pointer = 0;
				else ReadCh1Pointer++;
				CanReadLen--;
			}
		}
	}
	return 0;
}

static int ReadFrameBuf2(void)
{
	unsigned int CanReadLen;//Ŀǰ����������Ч���ֽ���
	unsigned int *pPacketHead, *pPacketTail;//�����ͷ�Ͱ�βָ��
	unsigned int FrameLen, FramePadLen, PacketPadLen;//����֡���ֽ�,16�ֽ�ʱ��+�����ֽ�+2�ֽ�״̬,����ֽ���0��3,����˫����
	unsigned int PacketLen, bufRemLen, remLen, PacketTailTrue;

	//���ı�������˫��(4Byte)Ϊ��λ,������С����PACKET_MIN_LEN=24��˫��=96�ֽ�
	//ԭ��:4�ֽڰ�ͷ+2�ֽڳ���+16�ֽ�ʱ��+64�ֽ�֡+2�ֽ�״̬+4�ֽ�У��+4�ֽڰ�β=96�ֽ�=24��˫��
	//�°�:4�ֽڰ�ͷ+2�ֽڳ���+2�ֽ�״̬+16�ֽ�ʱ��+64�ֽ�֡+4�ֽ�У��+4�ֽڰ�β=96�ֽ�=24��˫��
	//FrameLen=���յ���֡�ֽ���+16�ֽ�ʱ��+2�ֽ�֡״̬
	if(ReadCh2Pointer <= WriteCh2Pointer)
		CanReadLen = WriteCh2Pointer - ReadCh2Pointer;//��������ܵĳ���,˫��Ϊ��λ
	else	CanReadLen = PCIFPGA_DMABUF_DWORDLEN - ReadCh2Pointer + WriteCh2Pointer;//˫�ּ���
	if(CanReadLen < PACKET_MIN_LEN) return 0;//������С��24��˫��,��Ҫ�ȴ�
	//��ʼ�ڰ���Ѱ��֡,���ҽ��������
	while(CanReadLen >= PACKET_MIN_LEN) {//�ﵽ24��˫���ٿ�ʼѰ��֡
		if(ReadCh2Pointer < WriteCh2Pointer) {
			pPacketHead = pcifpga_card_dma2 + ReadCh2Pointer;//��Ϊ˫�ֱ���,��ͷλ��ָ��
			if(*pPacketHead == PACKET_HEADE) {//�ҵ���ͷ0x1a2b3c4d
				//printk("TY-3012S PciDrv *pPacketHead:  %08x!\n", *pPacketHead);//��ʾ�ҵ���ͷ
				FrameLen = *(pcifpga_card_dma2 + ReadCh2Pointer + PACKET_HEAD_LEN) >> 0x10;//����2�ֽ�,�ڸ�λ
				//printk("TY-3012S PciDrv FrameLen:      %08x!\n", FrameLen);//��ʾ֡����
				if(FrameLen > FRAME_MAX_LEN) {//��̫��֡��+32�ֽڿ���
					printk("TY-3012S PciDrv FrameLenE:     %08d!\n", FrameLen);//��ʾ�����ȴ���,�ǳ�����
					ReadCh2Pointer++;//֡��̫������,����1��˫��,����
					CanReadLen--;
					continue;//����ִ��while
				}
				//��������ֽ���:֡ͷ+֡����+֡ʱ��+֡����+֡״̬+֡У��+֡β+���:������˫�ֽ�
				FramePadLen = (FRAME_LEN_LEN + FrameLen) % 4;//��˫��,��������ֽ�����
				if(FramePadLen == 0) PacketPadLen = 0;//�����,˫�ֵı���
				else PacketPadLen = 1;//�����,��˫�ֱ���,1����˫�ֽ����
				//��������ĳ���,˫����
				PacketLen = PACKET_HEAD_LEN + ((FRAME_LEN_LEN + FrameLen) / 0x04) + PacketPadLen + PACKET_FCS_LEN + PACKET_TAIL_LEN;
				//printk("TY-3012S PciDrv PacketLen:     %08x!\n", PacketLen);//��ʾ���ܳ���
				if(PacketLen <= CanReadLen) {//�����ȷ���Ҫ��,�����ҵ���β
					pPacketTail = pcifpga_card_dma2 + ReadCh2Pointer + PacketLen - PACKET_TAIL_LEN;
					PacketTailTrue = 0;
					switch(FramePadLen) {
						case 0: if(*pPacketTail == PACKET_TAIL0) PacketTailTrue = 1; break;//0xa1b2c3d4
						case 1: if(*pPacketTail == PACKET_TAIL3) PacketTailTrue = 1; break;//0xd4ffffff
						case 2: if(*pPacketTail == PACKET_TAIL2) PacketTailTrue = 1; break;//0xc3d4ffff
						case 3: if(*pPacketTail == PACKET_TAIL1) PacketTailTrue = 1; break;//0xb2c3d4ff
						default: PacketTailTrue = 0; break;
					}
					if(PacketTailTrue == 1) {//�ҵ���һ����Ч�İ�β
						//printk("TY-3012S PciDrv *pPacketTail:  %08x!\n", *pPacketTail);//��ʾ�ҵ���β
						memcpy((pcifpga_card_buf0 + PACKET_HEAD_LEN), (pcifpga_card_dma2 + ReadCh2Pointer), 4*PacketLen);
						ReadCh2Pointer += PacketLen;
						return PacketLen;
					} else {//û���ҵ�֡β
						printk("TY-3012S PciDrv *pPacketTailE: %08x!\n", *pPacketTail);//��ʾ��β����,�ǳ�����
						ReadCh2Pointer++;//���ǰ�β
						CanReadLen--;
					}
				} else {
					return 0;//���յ�����̫��,��Ҫ�ȴ���һ��������
				}
			} else {
				//printk("TY-3012S PciDrv *pPacketHeadE: %08x!\n", *pPacketHead);//��ʾ���ְ�ͷ����,��̫����
				ReadCh2Pointer++;//���ǰ�ͷ,����1��˫��
				CanReadLen--;
			}
		} else {
			pPacketHead = pcifpga_card_dma2 + ReadCh2Pointer;//��Ϊ˫�ֱ���,��ͷλ��ָ��
			if(*pPacketHead == PACKET_HEADE) {//�ҵ���ͷPACKET_HEADE=0x1a2b3c4d
				//printk("TY-3012S PciDrv *pPacketHead:  %08x!!\n", *pPacketHead);//��ʾ�ҵ���ͷ
				if((ReadCh2Pointer + PACKET_HEAD_LEN) == PCIFPGA_DMABUF_DWORDLEN)
					FrameLen = *pcifpga_card_dma2 >> 0x10;//����2�ֽ�,�ڸ�λ,�ֽڼ���
				else	FrameLen = *(pcifpga_card_dma2 + ReadCh2Pointer + PACKET_HEAD_LEN) >> 0x10;//����2�ֽ�,�ڸ�λ,�ֽڼ���
				//printk("TY-3012S PciDrv FrameLen:      %08x!!\n", FrameLen);//��ʾ֡����
				if(FrameLen > FRAME_MAX_LEN) {//��̫��֡��+32�ֽڿ���
					printk("TY-3012S PciDrv FrameLenE:     %08d!!\n", FrameLen);//��ʾ֡���ȴ���,�ǳ�����
					if((ReadCh2Pointer + PACKET_HEAD_LEN) == PCIFPGA_DMABUF_DWORDLEN) ReadCh2Pointer = 0;
					else ReadCh2Pointer++;//֡��̫������,����1��˫��,����
					CanReadLen--;
					continue;//����ִ��while
				}
				//��������ֽ���:֡ͷ+֡����+֡ʱ��+֡����+֡״̬+֡У��+֡β+���:������˫�ֽ�
				FramePadLen = (FRAME_LEN_LEN + FrameLen) % 4;//��˫��,��������ֽ�����
				if(FramePadLen == 0) PacketPadLen = 0;//�����,˫�ֵı���
				else PacketPadLen = 1;//�����,��˫�ֱ���,1����˫�ֽ����
				//��������ĳ���,˫����
				PacketLen = PACKET_HEAD_LEN + ((FRAME_LEN_LEN + FrameLen) / 0x04) + PacketPadLen + PACKET_FCS_LEN + PACKET_TAIL_LEN;
				//printk("TY-3012S PciDrv PacketLen:     %08x!!\n", PacketLen);//��ʾ���ܳ���
				if(PacketLen <= CanReadLen) {//�����ȷ���Ҫ��,�����ҵ���β
					if((ReadCh2Pointer + PacketLen) > PCIFPGA_DMABUF_DWORDLEN) {
						pPacketTail = pcifpga_card_dma2 + ReadCh2Pointer + PacketLen - PCIFPGA_DMABUF_DWORDLEN - PACKET_TAIL_LEN;
					} else	{
						pPacketTail = pcifpga_card_dma2 + ReadCh2Pointer + PacketLen - PACKET_TAIL_LEN;
					}
					PacketTailTrue = 0;
					switch(FramePadLen) {
						case 0: if(*pPacketTail == PACKET_TAIL0) PacketTailTrue = 1; break;//0xa1b2c3d4
						case 1: if(*pPacketTail == PACKET_TAIL3) PacketTailTrue = 1; break;//0xd4ffffff
						case 2: if(*pPacketTail == PACKET_TAIL2) PacketTailTrue = 1; break;//0xc3d4ffff
						case 3: if(*pPacketTail == PACKET_TAIL1) PacketTailTrue = 1; break;//0xb2c3d4ff
						default: PacketTailTrue = 0; break;
					}
					if(PacketTailTrue == 1){//�ҵ���һ����Ч�İ�β
						//printk("TY-3012S PciDrv *pPacketTail:  %08x!!\n", *pPacketTail);//��ʾ�ҵ���β
						if((ReadCh2Pointer + PacketLen) <= PCIFPGA_DMABUF_DWORDLEN) {
							memcpy((pcifpga_card_buf0 + PACKET_HEAD_LEN), (pcifpga_card_dma2 + ReadCh2Pointer), 4*PacketLen);
							ReadCh2Pointer += PacketLen;
							if(ReadCh2Pointer == PCIFPGA_DMABUF_DWORDLEN) ReadCh2Pointer = 0;
							return PacketLen;
						}
						if((ReadCh2Pointer + PacketLen) > PCIFPGA_DMABUF_DWORDLEN) {
							bufRemLen = PCIFPGA_DMABUF_DWORDLEN - ReadCh2Pointer;
							memcpy((pcifpga_card_buf0 + PACKET_HEAD_LEN), (pcifpga_card_dma2 + ReadCh2Pointer), 4*bufRemLen);
							ReadCh2Pointer = 0;
							remLen = PacketLen - bufRemLen;
							memcpy((pcifpga_card_buf0 + PACKET_HEAD_LEN + bufRemLen), pcifpga_card_dma2, 4*remLen);
							ReadCh2Pointer += remLen;
							return PacketLen;
						}
					} else {//��Ӧû���ҵ���һ����ͷ
						printk("TY-3012S PciDrv *pPacketTailE: %08x!!\n", *pPacketTail);//��ʾ��β����,�ǳ�����
						if((ReadCh2Pointer + 1) == PCIFPGA_DMABUF_DWORDLEN) ReadCh2Pointer = 0;
						else ReadCh2Pointer++;//����1��˫��
						CanReadLen--;
					}
				} else {//���յ�����̫��,��Ҫ�ȴ���һ��������
					return 0;
				}
			} else {//��Ӧû���ҵ�֡ͷ
				//printk("TY-3012S PciDrv *pPacketHeadE: %08x!!\n", *pPacketHead);//��ʾ���ְ�ͷ����,��̫����
				if((ReadCh2Pointer + 1) == PCIFPGA_DMABUF_DWORDLEN) ReadCh2Pointer = 0;
				else ReadCh2Pointer++;
				CanReadLen--;
			}
		}
	}
	return 0;
}

static int ReadFpgaTimer(void)
{
	//��FPGA����ʱ�����ݣ��죬Сʱ�����ӣ��룬���룬΢��
	//ͬʱ��8421����ת���ɶ����Ʊ���
	unsigned int tt;

	//����ʱ�䣬�Ա��ȡ��ȷ��GPSʱ��
	iowrite32(IRIGB1588E, ChxBaseAddr0 + 4 * IRIGB1588TIMEGET);
	tt = ioread32(ChxBaseAddr0 + 4 * IRIGB1588RDDAY);
	DrvFpgaTime->DD = 0x00;
	DrvFpgaTime->DD = DrvFpgaTime->DD + ((tt & 0x03000000) >> 24) * 100;
	DrvFpgaTime->DD = DrvFpgaTime->DD + ((tt & 0x00f00000) >> 20) * 10;
	DrvFpgaTime->DD = DrvFpgaTime->DD + ((tt & 0x000f0000) >> 16);
	DrvFpgaTime->HH = 0x00;
	DrvFpgaTime->HH = DrvFpgaTime->HH + ((tt & 0x00003000) >> 12) * 10;
	DrvFpgaTime->HH = DrvFpgaTime->HH + ((tt & 0x00000f00) >>  8);
	DrvFpgaTime->MM = 0x00;
	DrvFpgaTime->MM = DrvFpgaTime->MM + ((tt & 0x000000f0) >>  4) * 10;
	DrvFpgaTime->MM = DrvFpgaTime->MM + ((tt & 0x0000000f) >>  0);
	tt = ioread32(ChxBaseAddr0 + 4 * IRIGB1588RDSECOND);
	DrvFpgaTime->SS = 0x00;
	DrvFpgaTime->SS = DrvFpgaTime->SS + ((tt & 0x70000000) >> 28) * 10;
	DrvFpgaTime->SS = DrvFpgaTime->SS + ((tt & 0x0f000000) >> 24);
	DrvFpgaTime->MS = 0x00;
	DrvFpgaTime->MS = DrvFpgaTime->MS + ((tt & 0x00f00000) >> 20) * 100;
	DrvFpgaTime->MS = DrvFpgaTime->MS + ((tt & 0x000f0000) >> 16) * 10;
	DrvFpgaTime->MS = DrvFpgaTime->MS + ((tt & 0x0000f000) >> 12);
	DrvFpgaTime->US = DrvFpgaTime->US + ((tt & 0x00000f00) >>  8) * 100;
	DrvFpgaTime->US = DrvFpgaTime->US + ((tt & 0x000000f0) >>  4) * 10;
	DrvFpgaTime->US = DrvFpgaTime->US + ((tt & 0x0000000f) >>  0);
	//��ȡ��ƫ��״̬��ֵ
	tt = ioread32(ChxBaseAddr0 + 4 * IRIGB1588TIMEERR);
	if((tt & 0x01000000) == 0x01000000) DrvFpgaTime->ES = 1;
	else DrvFpgaTime->ES = 0;
	DrvFpgaTime->EV = tt & 0x0ffffff;
	//��ȡGPS״̬
	DrvFpgaTime->PS = 0;
	DrvFpgaTime->GS = 0;
	tt = ioread32(ChxBaseAddr0 + 4 * IRIGB1588STATUS);
	if((tt & 0x08) == 0x08) DrvFpgaTime->PS = 1;
	if((tt & 0x04) == 0x04) DrvFpgaTime->PS = 0;
	if((tt & 0x02) == 0x02) DrvFpgaTime->GS = 1;
	if((tt & 0x01) == 0x01) DrvFpgaTime->GS = 0;
	return MSG_BUF_LEN;
}

static int WriteFpgaTimer(void)
{
	//��FPGAд��ʱ�����ݣ��죬Сʱ�����ӣ��룬���룬΢��
	//ͬʱ�������Ʊ������ת����8421����
	unsigned int t;
	int i, j, k;

	//�����졢Сʱ���롢���롢΢��
	i = DrvFpgaTime->DD / 100;//��İ�λ
	j = (DrvFpgaTime->DD - (i * 100)) / 10;//���ʮλ
	k = DrvFpgaTime->DD % 10;//��ĸ�λ
	t = (i << 24) + (j << 20) + (k << 16);//���8421����
	//����ʱ
	i = DrvFpgaTime->HH / 10;//ʱ��ʮλ
	j = DrvFpgaTime->HH % 10;//ʱ�ĸ�λ
	t = t + (i << 12) + (j << 8);//�졢ʱ��8421����
	//�����
	i = DrvFpgaTime->MM / 10;//���ʮλ
	j = DrvFpgaTime->MM % 10;//��ĸ�λ
	t = t + (i << 4) + j;//�졢ʱ���ֵ�8421����
	iowrite32(t, ChxBaseAddr0 + 4 * IRIGB1588WRDAY);
	//������
	i = DrvFpgaTime->SS / 10;//���ʮλ
	j = DrvFpgaTime->SS % 10;//��ĸ�λ
	t = (i << 28) + (j << 24);//���8421����
	//�������
	i = DrvFpgaTime->MS / 100;//����İ�λ
	j = (DrvFpgaTime->MS - (i * 100)) / 10;//�����ʮλ
	k = DrvFpgaTime->MS % 10;//����ĸ�λ
	t = t + (i << 20) + (j << 16) + (k << 12);//�롢�����8421����
	//����΢��
	i = DrvFpgaTime->US / 100;//΢��İ�λ
	j = (DrvFpgaTime->US - (i * 100)) / 10;//΢���ʮλ
	k = DrvFpgaTime->US % 10;//΢��ĸ�λ
	t = t + (i << 8) + (j << 4) + k;//�롢���롢΢���8421����
	iowrite32(t, ChxBaseAddr0 + 4 * IRIGB1588WRSECOND);
	iowrite32(IRIGB1588E, ChxBaseAddr0 + 4 * IRIGB1588TIMEPUT);
	return MSG_BUF_LEN;
}

static int DisplayFpgaTimer(void)
{
	unsigned int tt, dd, hh, mm, ss, ms, us;
	unsigned int ErrStatus, ErrValue, PtpStatus, GpsStatus = 0;

	//��ʾʱ��
	iowrite32(IRIGB1588E, ChxBaseAddr0 + 4 * IRIGB1588TIMEGET);
	tt = ioread32(ChxBaseAddr0 + 4 * IRIGB1588RDDAY);
	dd = 0x00;
	dd = dd + ((tt & 0x03000000) >> 24) * 100;
	dd = dd + ((tt & 0x00f00000) >> 20) * 10;
	dd = dd + ((tt & 0x000f0000) >> 16);
	printk("\nTY-3012S PciDrv Fpga Day:      %08d!\n", dd);
	hh = 0x00;
	hh = hh + ((tt & 0x00003000) >> 12) * 10;
	hh = hh + ((tt & 0x00000f00) >>  8);
	mm = 0x00;
	mm = mm + ((tt & 0x000000f0) >>  4) * 10;
	mm = mm + ((tt & 0x0000000f) >>  0);
	tt = ioread32(ChxBaseAddr0 + 4 * IRIGB1588RDSECOND);
	ss = 0x00;
	ss = ss + ((tt & 0x70000000) >> 28) * 10;
	ss = ss + ((tt & 0x0f000000) >> 24);
	printk("TY-3012S PciDrv Fpga Time:     %02d:%02d:%02d!\n", hh, mm, ss);
	ms = 0x00;
	ms = ms + ((tt & 0x00f00000) >> 20) * 100;
	ms = ms + ((tt & 0x000f0000) >> 16) * 10;
	ms = ms + ((tt & 0x0000f000) >> 12);
	printk("TY-3012S PciDrv Millisecond:   %08d!\n", ms);
	us = 0x00;
	us = us + ((tt & 0x00000f00) >>  8) * 100;
	us = us + ((tt & 0x000000f0) >>  4) * 10;
	us = us + ((tt & 0x0000000f) >>  0);
	printk("TY-3012S PciDrv Microsecond:   %08d!\n", us);
	//��ȡ��ƫ��״̬��ֵ
	//D24=0����ʱ�ӵ���GPS,=1����GPS,D23��D0��������ʱ��ÿ��ƫ����,10MHz��λ
	tt = ioread32(ChxBaseAddr0 + 4 * IRIGB1588TIMEERR);
	if((tt & 0x01000000) == 0x01000000) ErrStatus = 1;
	else ErrStatus = 0;
	ErrValue = tt & 0x0ffffff;
	printk("TY-3012S PciDrv TimeErrStatus: %08d!\n", ErrStatus);
	printk("TY-3012S PciDrv TimeErrValue:  %08d!\n", ErrValue);
	//��ȡGPS״̬
	//D3��0��Ӧ1588����,1588ʱ�Ӷ�ʧ,GPS����,GPSʱ�Ӷ�ʧ,����Ч
        PtpStatus = 0;
        GpsStatus = 0;
	tt = ioread32(ChxBaseAddr0 + 4 * IRIGB1588STATUS);
	printk("TY-3012S PciDrv PtpGpsStatus:  %08d!\n\n", tt);
	if((tt & 0x08) == 0x08) PtpStatus = 1;
	if((tt & 0x04) == 0x04) PtpStatus = 0;
	if((tt & 0x02) == 0x02) GpsStatus = 1;
	if((tt & 0x01) == 0x01) GpsStatus = 0;
	//printk("TY-3012S PciDrv GpsStatus:     %08d!\n", GpsStatus);
	//printk("TY-3012S PciDrv PtpStatus:     %08d!\n", PtpStatus);
	return 0;
}

static int ReadChFilter(unsigned int ch)
{
	//��ȡ���˼Ĵ���
	if(ch == 1) {
		DrvChFilter->Reset        = ioread32(Ch1BaseAddr1 + 4 * CHFILTERRESET);
		DrvChFilter->Mode         = ioread32(Ch1BaseAddr1 + 4 * CHFILTERMODE);
		DrvChFilter->MinLength    = ioread32(Ch1BaseAddr1 + 4 * CHFILTERMINLEN);
		DrvChFilter->MaxLength    = ioread32(Ch1BaseAddr1 + 4 * CHFILTERMAXLEN);
		DrvChFilter->FilterEnable = ioread32(Ch1BaseAddr1 + 4 * CHFILTERENABLE);
		DrvChFilter->Vlan1Id      = ioread32(Ch1BaseAddr1 + 4 * CHFILTERVID1);
		DrvChFilter->Vlan2Id      = ioread32(Ch1BaseAddr1 + 4 * CHFILTERVID2);
		DrvChFilter->MacEnable    = ioread32(Ch1BaseAddr1 + 4 * CHFILTERMACEN);
	}
	if(ch == 2) {
		DrvChFilter->Reset        = ioread32(Ch2BaseAddr2 + 4 * CHFILTERRESET);
		DrvChFilter->Mode         = ioread32(Ch2BaseAddr2 + 4 * CHFILTERMODE);
		DrvChFilter->MinLength    = ioread32(Ch2BaseAddr2 + 4 * CHFILTERMINLEN);
		DrvChFilter->MaxLength    = ioread32(Ch2BaseAddr2 + 4 * CHFILTERMAXLEN);
		DrvChFilter->FilterEnable = ioread32(Ch2BaseAddr2 + 4 * CHFILTERENABLE);
		DrvChFilter->Vlan1Id      = ioread32(Ch2BaseAddr2 + 4 * CHFILTERVID1);
		DrvChFilter->Vlan2Id      = ioread32(Ch2BaseAddr2 + 4 * CHFILTERVID2);
		DrvChFilter->MacEnable    = ioread32(Ch2BaseAddr2 + 4 * CHFILTERMACEN);
	}
	return MSG_BUF_LEN;
}

static int WriteChFilter(unsigned int ch)
{
	//д����˼Ĵ���
	if(ch == 1) {
		iowrite32(DrvChFilter->Reset,        Ch1BaseAddr1 + 4 * CHFILTERRESET);
		iowrite32(DrvChFilter->Mode,         Ch1BaseAddr1 + 4 * CHFILTERMODE);
		iowrite32(DrvChFilter->MinLength,    Ch1BaseAddr1 + 4 * CHFILTERMINLEN);
		iowrite32(DrvChFilter->MaxLength,    Ch1BaseAddr1 + 4 * CHFILTERMAXLEN);
		iowrite32(DrvChFilter->FilterEnable, Ch1BaseAddr1 + 4 * CHFILTERENABLE);
		iowrite32(DrvChFilter->Vlan1Id,      Ch1BaseAddr1 + 4 * CHFILTERVID1);
		iowrite32(DrvChFilter->Vlan2Id,      Ch1BaseAddr1 + 4 * CHFILTERVID2);
		iowrite32(DrvChFilter->MacEnable,    Ch1BaseAddr1 + 4 * CHFILTERMACEN);
	}
	if(ch == 2) {
		iowrite32(DrvChFilter->Reset,        Ch2BaseAddr2 + 4 * CHFILTERRESET);
		iowrite32(DrvChFilter->Mode,         Ch2BaseAddr2 + 4 * CHFILTERMODE);
		iowrite32(DrvChFilter->MinLength,    Ch2BaseAddr2 + 4 * CHFILTERMINLEN);
		iowrite32(DrvChFilter->MaxLength,    Ch2BaseAddr2 + 4 * CHFILTERMAXLEN);
		iowrite32(DrvChFilter->FilterEnable, Ch2BaseAddr2 + 4 * CHFILTERENABLE);
		iowrite32(DrvChFilter->Vlan1Id,      Ch2BaseAddr2 + 4 * CHFILTERVID1);
		iowrite32(DrvChFilter->Vlan2Id,      Ch2BaseAddr2 + 4 * CHFILTERVID2);
		iowrite32(DrvChFilter->MacEnable,    Ch2BaseAddr2 + 4 * CHFILTERMACEN);
	}
	return MSG_BUF_LEN;
}

static int ReadFilterMac(unsigned int ch, unsigned int group)
{
	unsigned int i;
	//д��MAC���˼Ĵ���
	if((group >= 1) && (group <= 4)) i = group - 1;
	else return 0;
	if(ch == 1) {
		DrvFilterMac->MacAddr1H = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACH + i*4 + 0));
		DrvFilterMac->MacAddr1L = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACL + i*4 + 0));
		DrvFilterMac->MacAddr2H = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACH + i*4 + 1));
		DrvFilterMac->MacAddr2L = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACL + i*4 + 1));
		DrvFilterMac->MacAddr3H = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACH + i*4 + 2));
		DrvFilterMac->MacAddr3L = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACL + i*4 + 2));
		DrvFilterMac->MacAddr4H = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACH + i*4 + 3));
		DrvFilterMac->MacAddr4L = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACL + i*4 + 3));
	}
	if(ch == 2) {
		DrvFilterMac->MacAddr1H = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACH + i*4 + 0));
		DrvFilterMac->MacAddr1L = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACL + i*4 + 0));
		DrvFilterMac->MacAddr2H = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACH + i*4 + 1));
		DrvFilterMac->MacAddr2L = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACL + i*4 + 1));
		DrvFilterMac->MacAddr3H = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACH + i*4 + 2));
		DrvFilterMac->MacAddr3L = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACL + i*4 + 2));
		DrvFilterMac->MacAddr4H = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACH + i*4 + 3));
		DrvFilterMac->MacAddr4L = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACL + i*4 + 3));
	}
	return MSG_BUF_LEN;
}

static int WriteFilterMac(unsigned int ch, unsigned int group)
{
	unsigned int i;
	//д��MAC���˼Ĵ���
	if((group >= 1) && (group <= 4)) i = group - 1;
	else return 0;
	if(ch == 1) {
		iowrite32(DrvFilterMac->MacAddr1H, Ch1BaseAddr1 + 4*(CHFILTERMACH + i*4 + 0));
		iowrite32(DrvFilterMac->MacAddr1L, Ch1BaseAddr1 + 4*(CHFILTERMACL + i*4 + 0));
		iowrite32(DrvFilterMac->MacAddr2H, Ch1BaseAddr1 + 4*(CHFILTERMACH + i*4 + 1));
		iowrite32(DrvFilterMac->MacAddr2L, Ch1BaseAddr1 + 4*(CHFILTERMACL + i*4 + 1));
		iowrite32(DrvFilterMac->MacAddr3H, Ch1BaseAddr1 + 4*(CHFILTERMACH + i*4 + 2));
		iowrite32(DrvFilterMac->MacAddr3L, Ch1BaseAddr1 + 4*(CHFILTERMACL + i*4 + 2));
		iowrite32(DrvFilterMac->MacAddr4H, Ch1BaseAddr1 + 4*(CHFILTERMACH + i*4 + 3));
		iowrite32(DrvFilterMac->MacAddr4L, Ch1BaseAddr1 + 4*(CHFILTERMACL + i*4 + 3));
	}
	if(ch == 2) {
		iowrite32(DrvFilterMac->MacAddr1H, Ch2BaseAddr2 + 4*(CHFILTERMACH + i*4 + 0));
		iowrite32(DrvFilterMac->MacAddr1L, Ch2BaseAddr2 + 4*(CHFILTERMACL + i*4 + 0));
		iowrite32(DrvFilterMac->MacAddr2H, Ch2BaseAddr2 + 4*(CHFILTERMACH + i*4 + 1));
		iowrite32(DrvFilterMac->MacAddr2L, Ch2BaseAddr2 + 4*(CHFILTERMACL + i*4 + 1));
		iowrite32(DrvFilterMac->MacAddr3H, Ch2BaseAddr2 + 4*(CHFILTERMACH + i*4 + 2));
		iowrite32(DrvFilterMac->MacAddr3L, Ch2BaseAddr2 + 4*(CHFILTERMACL + i*4 + 2));
		iowrite32(DrvFilterMac->MacAddr4H, Ch2BaseAddr2 + 4*(CHFILTERMACH + i*4 + 3));
		iowrite32(DrvFilterMac->MacAddr4L, Ch2BaseAddr2 + 4*(CHFILTERMACL + i*4 + 3));
	}
	return MSG_BUF_LEN;
}

static int ReadChCounter(unsigned int ch)
{
	if(ch == 1) {
		DrvFrameCounters->AllFrame	 = ioread32(Ch1BaseAddr1 + 4 * CHALLFRAMEL);
		DrvFrameCounters->AllByte        = ioread32(Ch1BaseAddr1 + 4 * CHALLBYTEL);
		DrvFrameCounters->BroadcastFrame = ioread32(Ch1BaseAddr1 + 4 * CHBROADCASTFRAMEL);
		DrvFrameCounters->BroadcastByte  = ioread32(Ch1BaseAddr1 + 4 * CHBROADCASTBYTEL);
		DrvFrameCounters->MulticastFrame = ioread32(Ch1BaseAddr1 + 4 * CHMULTCASTFRAMEL);
		DrvFrameCounters->MulticastByte  = ioread32(Ch1BaseAddr1 + 4 * CHMULTCASTBYTEL);
		DrvFrameCounters->CrcErrFrame    = ioread32(Ch1BaseAddr1 + 4 * CHCRCERRFRAMEL);
		DrvFrameCounters->AllignErrFrame = ioread32(Ch1BaseAddr1 + 4 * CHERRORFRAMEL);
		DrvFrameCounters->ShortFrame     = ioread32(Ch1BaseAddr1 + 4 * CHALLIGNERRFRAMEL);
		DrvFrameCounters->LongFrame      = ioread32(Ch1BaseAddr1 + 4 * CHSHORTFRAMEL);
		DrvFrameCounters->ErrFrame       = ioread32(Ch1BaseAddr1 + 4 * CHLONGFRAMEL);
	}
	if(ch == 2) {
		DrvFrameCounters->AllFrame	 = ioread32(Ch2BaseAddr2 + 4 * CHALLFRAMEL);
		DrvFrameCounters->AllByte        = ioread32(Ch2BaseAddr2 + 4 * CHALLBYTEL);
		DrvFrameCounters->BroadcastFrame = ioread32(Ch2BaseAddr2 + 4 * CHBROADCASTFRAMEL);
		DrvFrameCounters->BroadcastByte  = ioread32(Ch2BaseAddr2 + 4 * CHBROADCASTBYTEL);
		DrvFrameCounters->MulticastFrame = ioread32(Ch2BaseAddr2 + 4 * CHMULTCASTFRAMEL);
		DrvFrameCounters->MulticastByte  = ioread32(Ch2BaseAddr2 + 4 * CHMULTCASTBYTEL);
		DrvFrameCounters->CrcErrFrame    = ioread32(Ch2BaseAddr2 + 4 * CHCRCERRFRAMEL);
		DrvFrameCounters->AllignErrFrame = ioread32(Ch2BaseAddr2 + 4 * CHERRORFRAMEL);
		DrvFrameCounters->ShortFrame     = ioread32(Ch2BaseAddr2 + 4 * CHALLIGNERRFRAMEL);
		DrvFrameCounters->LongFrame      = ioread32(Ch2BaseAddr2 + 4 * CHSHORTFRAMEL);
		DrvFrameCounters->ErrFrame       = ioread32(Ch2BaseAddr2 + 4 * CHLONGFRAMEL);
	}
	return MSG_BUF_LEN;
}

/* �豸�򿪺������淶�ṹ */
static int pcifpga_open(struct inode *inode, struct file *filp)  
{
	//printk("TY-3012S PciDrv pcifpga_open opend!\n");
	return 0;
}

/* �豸�����������淶�ṹ */
static ssize_t pcifpga_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	int len = 0;

	if(copy_from_user(MsgBuffer, buff, MSG_BUF_LEN)) {//����Ӧ�ó���������Ϣ��������ݣ��64�ֽ�
		printk("TY-3012S PciDrv copy_from_user error!\n");
		return -1;
	}
	switch(DrvMsgCommand->Id) {
		case READ_CH_BUFFER://0x11����ȡ¼������FIFO�е����ݣ�ÿ�����2M�ֽ�
			#if 0
			printk("TY-3012S PciDrv DrvReadBuffer->Head: %08x!\n",  DrvReadBuffer->Head);
			printk("TY-3012S PciDrv DrvReadBuffer->Sid:  %08x!\n",  DrvReadBuffer->VersionSid);
			printk("TY-3012S PciDrv DrvReadBuffer->Id:   %08x!\n",  DrvReadBuffer->Id);
			printk("TY-3012S PciDrv DrvReadBuffer->ChNo: %08x!\n",  DrvReadBuffer->ChNo);
			printk("TY-3012S PciDrv DrvReadBuffer->Byte: %08x!\n",  DrvReadBuffer->Byte);
			printk("TY-3012S PciDrv DrvReadBuffer->Tail: %08x!\n",  DrvReadBuffer->Tail);
			#endif
			if(DrvReadBuffer->ChNo == 1) {
				len = ReadCh1Buf((unsigned int *)buff, count);//��ͨ��1����FIFO1������
			} else {
				if(DrvReadBuffer->ChNo == 2) {
					len = ReadCh2Buf((unsigned int *)buff, count);//��ͨ��2����FIFO2������
				} else {//ͨ������
					len = 0;
					printk("TY-3012S PciDrv READ_CH_BUFFER error %x!\n", DrvReadBuffer->ChNo);
				}
			}
			return len;
			break;
		case CHECK_FPGA_TIMER://0x14�����FPGAʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
			#if 1
			printk("TY-3012S PciDrv DrvFpgaTime->Head:   %08x!\n",  DrvFpgaTime->Head);
			printk("TY-3012S PciDrv DrvFpgaTime->Sid:    %08x!\n",  DrvFpgaTime->VersionSid);
			printk("TY-3012S PciDrv DrvFpgaTime->Id:     %08x!\n",  DrvFpgaTime->Id);
			printk("TY-3012S PciDrv DrvFpgaTime->Tail:   %08x!\n",  DrvFpgaTime->Tail);
			#endif
			len = ReadFpgaTimer();
			DrvFpgaTime->Id = ACK_FPGA_TIMER;
			#if 1
			printk("TY-3012S PciDrv DrvFpgaTime->Head:   %08x!!\n", DrvFpgaTime->Head);
			printk("TY-3012S PciDrv DrvFpgaTime->Sid:    %08x!!\n", DrvFpgaTime->VersionSid);
			printk("TY-3012S PciDrv DrvFpgaTime->Id:     %08x!!\n", DrvFpgaTime->Id);
			printk("TY-3012S PciDrv DrvFpgaTime->DD:     %08x!!\n", DrvFpgaTime->DD);
			printk("TY-3012S PciDrv DrvFpgaTime->HH:     %08x!!\n", DrvFpgaTime->HH);
			printk("TY-3012S PciDrv DrvFpgaTime->MM:     %08x!!\n", DrvFpgaTime->MM);
			printk("TY-3012S PciDrv DrvFpgaTime->SS:     %08x!!\n", DrvFpgaTime->SS);
			printk("TY-3012S PciDrv DrvFpgaTime->MS:     %08x!!\n", DrvFpgaTime->MS);
			printk("TY-3012S PciDrv DrvFpgaTime->US:     %08x!!\n", DrvFpgaTime->US);
			printk("TY-3012S PciDrv DrvFpgaTime->GS:     %08x!!\n", DrvFpgaTime->GS);
			printk("TY-3012S PciDrv DrvFpgaTime->PS:     %08x!!\n", DrvFpgaTime->PS);
			printk("TY-3012S PciDrv DrvFpgaTime->ES:     %08x!!\n", DrvFpgaTime->ES);
			printk("TY-3012S PciDrv DrvFpgaTime->EV:     %08x!!\n", DrvFpgaTime->EV);
			printk("TY-3012S PciDrv DrvFpgaTime->Tail:   %08x!!\n", DrvFpgaTime->Tail);
			#endif
			break;
		case CHECK_CH_FILTER://0x17�������ͨ���Ĺ�������
			#if 1
			printk("TY-3012S PciDrv DrvChFilter->Head:   %08x!\n",  DrvChFilter->Head);
			printk("TY-3012S PciDrv DrvChFilter->Sid:    %08x!\n",  DrvChFilter->VersionSid);
			printk("TY-3012S PciDrv DrvChFilter->Id:     %08x!\n",  DrvChFilter->Id);
			printk("TY-3012S PciDrv DrvChFilter->ChNo:   %08x!\n",  DrvChFilter->ChNo);
			printk("TY-3012S PciDrv DrvChFilter->Tail:   %08x!\n",  DrvChFilter->Tail);
			#endif
			if(DrvChFilter->ChNo == 1) len = ReadChFilter(1);//��ͨ��1�Ĺ�������
			else if(DrvChFilter->ChNo == 2) len = ReadChFilter(2);//��ͨ��2�Ĺ�������
				else {//ͨ������
					DrvChFilter->ChNo = 0;
					DrvChFilter->Reset = 0;
					DrvChFilter->Mode = 0;
					DrvChFilter->MinLength = 0;
					DrvChFilter->MaxLength = 0;
					DrvChFilter->FilterEnable = 0;
					DrvChFilter->Vlan1Id = 0;
					DrvChFilter->Vlan2Id = 0;
					DrvChFilter->MacEnable = 0;
					len = 0;
					printk("TY-3012S PciDrv CHECK_CH_FILTER error %x!\n", DrvChFilter->ChNo);
				}
			DrvChFilter->Id = ACK_CH_FILTER;
			#if 1
			printk("TY-3012S PciDrv DrvChFilter->Head:   %08x!!\n", DrvChFilter->Head);
			printk("TY-3012S PciDrv DrvChFilter->Sid:    %08x!!\n", DrvChFilter->VersionSid);
			printk("TY-3012S PciDrv DrvChFilter->Id:     %08x!!\n", DrvChFilter->Id);
			printk("TY-3012S PciDrv DrvChFilter->ChNo:   %08x!!\n", DrvChFilter->ChNo);
			printk("TY-3012S PciDrv DrvChFilter->Reset:  %08x!!\n", DrvChFilter->Reset);
			printk("TY-3012S PciDrv DrvChFilter->Mode:   %08x!!\n", DrvChFilter->Mode);
			printk("TY-3012S PciDrv DrvChFilter->MinLen: %08x!!\n", DrvChFilter->MinLength);
			printk("TY-3012S PciDrv DrvChFilter->MaxLen: %08x!!\n", DrvChFilter->MaxLength);
			printk("TY-3012S PciDrv DrvChFilter->FilEn:  %08x!!\n", DrvChFilter->FilterEnable);
			printk("TY-3012S PciDrv DrvChFilter->Vlan1:  %08x!!\n", DrvChFilter->Vlan1Id);
			printk("TY-3012S PciDrv DrvChFilter->Vlan2:  %08x!!\n", DrvChFilter->Vlan2Id);
			printk("TY-3012S PciDrv DrvChFilter->MacEn:  %08x!!\n", DrvChFilter->MacEnable);
			printk("TY-3012S PciDrv DrvChFilter->Tail:   %08x!!\n", DrvChFilter->Tail);
			#endif
			break;
		case CHECK_FILTER_MAC://0x18�������ͨ����ԴMAC���˵�ַ��һ��MAC��������
			#if 1
			printk("TY-3012S PciDrv DrvFilterMac->Head:  %08x!\n",  DrvFilterMac->Head);
			printk("TY-3012S PciDrv DrvFilterMac->Sid:   %08x!\n",  DrvFilterMac->VersionSid);
			printk("TY-3012S PciDrv DrvFilterMac->Id:    %08x!\n",  DrvFilterMac->Id);
			printk("TY-3012S PciDrv DrvFilterMac->ChNo:  %08x!\n",  DrvFilterMac->ChNo);
			printk("TY-3012S PciDrv DrvFilterMac->Group: %08x!\n",  DrvFilterMac->GroupNo);
			printk("TY-3012S PciDrv DrvFilterMac->Tail:  %08x!\n",  DrvFilterMac->Tail);
			#endif
			DrvFilterMac->ChNo = 0;
			DrvFilterMac->GroupNo = 0;
			DrvFilterMac->MacAddr1L = 0;
			DrvFilterMac->MacAddr1H = 0;
			DrvFilterMac->MacAddr2L = 0;
			DrvFilterMac->MacAddr2H = 0;
			DrvFilterMac->MacAddr3L = 0;
			DrvFilterMac->MacAddr3H = 0;
			DrvFilterMac->MacAddr4L = 0;
			DrvFilterMac->MacAddr4H = 0;
			len = 0;
			if((DrvFilterMac->ChNo == 1) || (DrvFilterMac->ChNo == 2)) {
				len = ReadFilterMac(DrvFilterMac->ChNo, DrvFilterMac->GroupNo);//��ͨ��1��2�Ĺ���MAC
				if(len == 0)
					printk("TY-3012S PciDrv CHECK_FILTER_MAC error %x!\n", DrvFilterMac->GroupNo);
			} else//ͨ������
				printk("TY-3012S PciDrv CHECK_FILTER_MAC error %x!!\n", DrvFilterMac->ChNo);
			DrvFilterMac->Id = ACK_FILTER_MAC;
			#if 1
			printk("TY-3012S PciDrv DrvFilterMac->Head:  %08x!!\n", DrvFilterMac->Head);
			printk("TY-3012S PciDrv DrvFilterMac->Sid:   %08x!!\n", DrvFilterMac->VersionSid);
			printk("TY-3012S PciDrv DrvFilterMac->Id:    %08x!!\n", DrvFilterMac->Id);
			printk("TY-3012S PciDrv DrvFilterMac->ChNo:  %08x!!\n", DrvFilterMac->ChNo);
			printk("TY-3012S PciDrv DrvFilterMac->Group: %08x!!\n", DrvFilterMac->GroupNo);
			printk("TY-3012S PciDrv DrvFilterMac->Mac1L: %08x!!\n", DrvFilterMac->MacAddr1L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac1H: %08x!!\n", DrvFilterMac->MacAddr1H);
			printk("TY-3012S PciDrv DrvFilterMac->Mac2L: %08x!!\n", DrvFilterMac->MacAddr2L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac2H: %08x!!\n", DrvFilterMac->MacAddr2H);
			printk("TY-3012S PciDrv DrvFilterMac->Mac3L: %08x!!\n", DrvFilterMac->MacAddr3L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac3H: %08x!!\n", DrvFilterMac->MacAddr3H);
			printk("TY-3012S PciDrv DrvFilterMac->Mac4L: %08x!!\n", DrvFilterMac->MacAddr4L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac4H: %08x!!\n", DrvFilterMac->MacAddr4H);
			printk("TY-3012S PciDrv DrvFilterMac->Tail:  %08x!!\n", DrvFilterMac->Tail);
			#endif
			break;
		case CHECK_FRAME_COUNTERS://0x19�������ͨ��֡���ֽڼ�������ͳ������
			#if 1
			printk("TY-3012S PciDrv DrvFrameCnts->Head:  %08x!\n",  DrvFrameCounters->Head);
			printk("TY-3012S PciDrv DrvFrameCnts->Sid:   %08x!\n",  DrvFrameCounters->VersionSid);
			printk("TY-3012S PciDrv DrvFrameCnts->Id:    %08x!\n",  DrvFrameCounters->Id);
			printk("TY-3012S PciDrv DrvFrameCnts->ChNo:  %08x!\n",  DrvFrameCounters->ChNo);
			printk("TY-3012S PciDrv DrvFrameCnts->Tail:  %08x!\n",  DrvFrameCounters->Tail);
			#endif
			if((DrvFrameCounters->ChNo == 1) || (DrvFrameCounters->ChNo == 2)) {
				len = ReadChCounter(DrvFrameCounters->ChNo);//��ͨ��1��2��֡���ֽڼ�����
			} else {//ͨ������
				DrvFrameCounters->ChNo = 0;
				DrvFrameCounters->AllFrame = 0;
				DrvFrameCounters->AllByte = 0;
				DrvFrameCounters->BroadcastFrame = 0;
				DrvFrameCounters->BroadcastByte = 0;
				DrvFrameCounters->MulticastFrame = 0;
				DrvFrameCounters->MulticastByte = 0;
				DrvFrameCounters->CrcErrFrame = 0;
				DrvFrameCounters->ErrFrame = 0;
				DrvFrameCounters->AllignErrFrame = 0;
				DrvFrameCounters->ShortFrame = 0;
				DrvFrameCounters->LongFrame = 0;
				len = 0;
				printk("TY-3012S PciDrv CHECK_FRAME_COUNTERS error %x!\n", DrvFrameCounters->ChNo);
			}
			DrvFrameCounters->Id = ACK_FRAME_COUNTERS;
			#if 1
			printk("TY-3012S PciDrv DrvFrameCnts->Head:  %08x!!\n", DrvFrameCounters->Head);
			printk("TY-3012S PciDrv DrvFrameCnts->Sid:   %08x!!\n", DrvFrameCounters->VersionSid);
			printk("TY-3012S PciDrv DrvFrameCnts->Id:    %08x!!\n", DrvFrameCounters->Id);
			printk("TY-3012S PciDrv DrvFrameCnts->ChNo:  %08x!!\n", DrvFrameCounters->ChNo);
			printk("TY-3012S PciDrv DrvFrameCnts->AllF:  %08x!!\n", DrvFrameCounters->AllByte);
			printk("TY-3012S PciDrv DrvFrameCnts->AllB:  %08x!!\n", DrvFrameCounters->BroadcastFrame);
			printk("TY-3012S PciDrv DrvFrameCnts->BroaF: %08x!!\n", DrvFrameCounters->BroadcastByte);
			printk("TY-3012S PciDrv DrvFrameCnts->BroaB: %08x!!\n", DrvFrameCounters->MulticastFrame);
			printk("TY-3012S PciDrv DrvFrameCnts->MultF: %08x!!\n", DrvFrameCounters->MulticastByte);
			printk("TY-3012S PciDrv DrvFrameCnts->MultB: %08x!!\n", DrvFrameCounters->CrcErrFrame);
			printk("TY-3012S PciDrv DrvFrameCnts->CrcF:  %08x!!\n", DrvFrameCounters->ErrFrame);
			printk("TY-3012S PciDrv DrvFrameCnts->ErrF:  %08x!!\n", DrvFrameCounters->AllignErrFrame);
			printk("TY-3012S PciDrv DrvFrameCnts->Allig: %08x!!\n", DrvFrameCounters->ShortFrame);
			printk("TY-3012S PciDrv DrvFrameCnts->Short: %08x!!\n", DrvFrameCounters->LongFrame);
			printk("TY-3012S PciDrv DrvFrameCnts->Long:  %08x!!\n", DrvFrameCounters->Tail);			
			printk("TY-3012S PciDrv DrvFrameCnts->Tail:  %08x!!\n", DrvFrameCounters->Tail);			
			#endif
			break;
		case CHECK_LED_STATUS://0x22�����ǰ���LED״̬���ܹ�64��LED
			#if 1
			printk("TY-3012S PciDrv DrvLedStatus->Head:  %08x!\n",  DrvLedStatus->Head);
			printk("TY-3012S PciDrv DrvLedStatus->Sid:   %08x!\n",  DrvLedStatus->VersionSid);
			printk("TY-3012S PciDrv DrvLedStatus->Id:    %08x!\n",  DrvLedStatus->Id);
			printk("TY-3012S PciDrv DrvLedStatus->Tail:  %08x!\n",  DrvLedStatus->Tail);
			#endif
			DrvLedStatus->LedL =ioread32(ChxBaseAddr0 + 4 * LED1DISPLAY);
			DrvLedStatus->LedH =ioread32(ChxBaseAddr0 + 4 * LED2DISPLAY);
			DrvLedStatus->Id = ACK_LED_STATUS;
			#if 1
			printk("TY-3012S PciDrv DrvLedStatus->Head:  %08x!!\n", DrvLedStatus->Head);
			printk("TY-3012S PciDrv DrvLedStatus->Sid:   %08x!!\n", DrvLedStatus->VersionSid);
			printk("TY-3012S PciDrv DrvLedStatus->Id:    %08x!!\n", DrvLedStatus->Id);
			printk("TY-3012S PciDrv DrvLedStatus->LedL:  %08x!!\n", DrvLedStatus->LedL);
			printk("TY-3012S PciDrv DrvLedStatus->LedH:  %08x!!\n", DrvLedStatus->LedL);
			printk("TY-3012S PciDrv DrvLedStatus->Tail:  %08x!!\n", DrvLedStatus->Tail);
			len = MSG_BUF_LEN;
			#endif
			break;
		default:
			len = 0;
			printk("TY-3012S PciDrv command error!\n");
			break;
	}
	if(copy_to_user(buff, MsgBuffer, len)) {
		printk("TY-3012S PciDrv copy_to_user error!\n");
		return -1;
	}
	return len;
}

/* �豸д�������淶�ṹ */
static ssize_t pcifpga_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
	int len = 0;

	if(copy_from_user(MsgBuffer, buf, count)) {
		printk("TY-3012S PciDrv copy_from_user error!!\n");
		return -1;
	}
	switch(DrvMsgCommand->Id) {
		case SET_FPGA_TIME://03������FPGA��ʱ�䣬�ꡢ�¡��ա�ʱ���֡��롢���롢΢��
			#if 1
			printk("TY-3012S PciDrv DrvFpgaTime->Head:   %08x!\n",  DrvFpgaTime->Head);
			printk("TY-3012S PciDrv DrvFpgaTime->Sid:    %08x!\n",  DrvFpgaTime->VersionSid);
			printk("TY-3012S PciDrv DrvFpgaTime->Id:     %08x!\n",  DrvFpgaTime->Id);
			printk("TY-3012S PciDrv DrvFpgaTime->DD:     %08x!\n",  DrvFpgaTime->DD);
			printk("TY-3012S PciDrv DrvFpgaTime->HH:     %08x!\n",  DrvFpgaTime->HH);
			printk("TY-3012S PciDrv DrvFpgaTime->MM:     %08x!\n",  DrvFpgaTime->MM);
			printk("TY-3012S PciDrv DrvFpgaTime->SS:     %08x!\n",  DrvFpgaTime->SS);
			printk("TY-3012S PciDrv DrvFpgaTime->MS:     %08x!\n",  DrvFpgaTime->MS);
			printk("TY-3012S PciDrv DrvFpgaTime->US:     %08x!\n",  DrvFpgaTime->US);
			printk("TY-3012S PciDrv DrvFpgaTime->GS:     %08x!\n",  DrvFpgaTime->GS);
			printk("TY-3012S PciDrv DrvFpgaTime->PS:     %08x!\n",  DrvFpgaTime->PS);
			printk("TY-3012S PciDrv DrvFpgaTime->ES:     %08x!\n",  DrvFpgaTime->ES);
			printk("TY-3012S PciDrv DrvFpgaTime->EV:     %08x!\n",  DrvFpgaTime->EV);
			printk("TY-3012S PciDrv DrvFpgaTime->Tail:   %08x!\n",  DrvFpgaTime->Tail);
			#endif
 			len = WriteFpgaTimer();
			len = ReadFpgaTimer();
			DrvFpgaTime->Id = ACK_FPGA_TIME;
			#if 1
			printk("TY-3012S PciDrv DrvFpgaTime->Head:   %08x!!\n", DrvFpgaTime->Head);
			printk("TY-3012S PciDrv DrvFpgaTime->Sid:    %08x!!\n", DrvFpgaTime->VersionSid);
			printk("TY-3012S PciDrv DrvFpgaTime->Id:     %08x!!\n", DrvFpgaTime->Id);
			printk("TY-3012S PciDrv DrvFpgaTime->DD:     %08x!!\n", DrvFpgaTime->DD);
			printk("TY-3012S PciDrv DrvFpgaTime->HH:     %08x!!\n", DrvFpgaTime->HH);
			printk("TY-3012S PciDrv DrvFpgaTime->MM:     %08x!!\n", DrvFpgaTime->MM);
			printk("TY-3012S PciDrv DrvFpgaTime->SS:     %08x!!\n", DrvFpgaTime->SS);
			printk("TY-3012S PciDrv DrvFpgaTime->MS:     %08x!!\n", DrvFpgaTime->MS);
			printk("TY-3012S PciDrv DrvFpgaTime->US:     %08x!!\n", DrvFpgaTime->US);
			printk("TY-3012S PciDrv DrvFpgaTime->GS:     %08x!!\n", DrvFpgaTime->GS);
			printk("TY-3012S PciDrv DrvFpgaTime->PS:     %08x!!\n", DrvFpgaTime->PS);
			printk("TY-3012S PciDrv DrvFpgaTime->ES:     %08x!!\n", DrvFpgaTime->ES);
			printk("TY-3012S PciDrv DrvFpgaTime->EV:     %08x!!\n", DrvFpgaTime->EV);
			printk("TY-3012S PciDrv DrvFpgaTime->Tail:   %08x!!\n", DrvFpgaTime->Tail);
			#endif
			break;
		case SET_CH_FILTER://0x08�����ü��ͨ���Ĺ�����������������
			#if 1
			printk("TY-3012S PciDrv DrvChFilter->Head:   %08x!\n",  DrvChFilter->Head);
			printk("TY-3012S PciDrv DrvChFilter->Sid:    %08x!\n",  DrvChFilter->VersionSid);
			printk("TY-3012S PciDrv DrvChFilter->Id:     %08x!\n",  DrvChFilter->Id);
			printk("TY-3012S PciDrv DrvChFilter->ChNo:   %08x!\n",  DrvChFilter->ChNo);
			printk("TY-3012S PciDrv DrvChFilter->Reset:  %08x!\n",  DrvChFilter->Reset);
			printk("TY-3012S PciDrv DrvChFilter->Mode:   %08x!\n",  DrvChFilter->Mode);
			printk("TY-3012S PciDrv DrvChFilter->MinLen: %08x!\n",  DrvChFilter->MinLength);
			printk("TY-3012S PciDrv DrvChFilter->MaxLen: %08x!\n",  DrvChFilter->MaxLength);
			printk("TY-3012S PciDrv DrvChFilter->FilEn:  %08x!\n",  DrvChFilter->FilterEnable);
			printk("TY-3012S PciDrv DrvChFilter->Vlan1:  %08x!\n",  DrvChFilter->Vlan1Id);
			printk("TY-3012S PciDrv DrvChFilter->Vlan2:  %08x!\n",  DrvChFilter->Vlan2Id);
			printk("TY-3012S PciDrv DrvChFilter->MacEn:  %08x!\n",  DrvChFilter->MacEnable);
			printk("TY-3012S PciDrv DrvChFilter->Tail:   %08x!\n",  DrvChFilter->Tail);
			#endif
			if(DrvChFilter->ChNo == 1) {
				len = WriteChFilter(1);
				len = ReadChFilter(1);		//��ͨ��1�Ĺ�������
			} else {
				if(DrvChFilter->ChNo == 2) {
					len = WriteChFilter(2);
					len = ReadChFilter(2);	//��ͨ��2�Ĺ�������
				} else {
					DrvChFilter->ChNo = 0;
					DrvChFilter->Reset = 0;
					DrvChFilter->Mode = 0;
					DrvChFilter->MinLength = 0;
					DrvChFilter->MaxLength = 0;
					DrvChFilter->FilterEnable = 0;
					DrvChFilter->Vlan1Id = 0;
					DrvChFilter->Vlan2Id = 0;
					DrvChFilter->MacEnable = 0;
					len = 0;
					printk("TY-3012S PciDrv SET_CH_FILTER error %x!\n", DrvChFilter->ChNo);//ͨ������
				}
			}
			DrvChFilter->Id = ACK_SET_FILTER;
			#if 1
			printk("TY-3012S PciDrv DrvChFilter->Head:   %08x!!\n", DrvChFilter->Head);
			printk("TY-3012S PciDrv DrvChFilter->Sid:    %08x!!\n", DrvChFilter->VersionSid);
			printk("TY-3012S PciDrv DrvChFilter->Id:     %08x!!\n", DrvChFilter->Id);
			printk("TY-3012S PciDrv DrvChFilter->ChNo:   %08x!!\n", DrvChFilter->ChNo);
			printk("TY-3012S PciDrv DrvChFilter->Reset:  %08x!!\n", DrvChFilter->Reset);
			printk("TY-3012S PciDrv DrvChFilter->Mode:   %08x!!\n", DrvChFilter->Mode);
			printk("TY-3012S PciDrv DrvChFilter->MinLen: %08x!!\n", DrvChFilter->MinLength);
			printk("TY-3012S PciDrv DrvChFilter->MaxLen: %08x!!\n", DrvChFilter->MaxLength);
			printk("TY-3012S PciDrv DrvChFilter->FilEn:  %08x!!\n", DrvChFilter->FilterEnable);
			printk("TY-3012S PciDrv DrvChFilter->Vlan1:  %08x!!\n", DrvChFilter->Vlan1Id);
			printk("TY-3012S PciDrv DrvChFilter->Vlan2:  %08x!!\n", DrvChFilter->Vlan2Id);
			printk("TY-3012S PciDrv DrvChFilter->MacEn:  %08x!!\n", DrvChFilter->MacEnable);
			printk("TY-3012S PciDrv DrvChFilter->Tail:   %08x!!\n", DrvChFilter->Tail);
			#endif
			break;
		case SET_FILTER_MAC://0x09�����ü��ͨ����ԴMAC���˵�ַ�������ո�MAC��ַ����
			#if 1
			printk("TY-3012S PciDrv DrvFilterMac->Head:  %08x!\n",  DrvFilterMac->Head);
			printk("TY-3012S PciDrv DrvFilterMac->Sid:   %08x!\n",  DrvFilterMac->VersionSid);
			printk("TY-3012S PciDrv DrvFilterMac->Id:    %08x!\n",  DrvFilterMac->Id);
			printk("TY-3012S PciDrv DrvFilterMac->ChNo:  %08x!\n",  DrvFilterMac->ChNo);
			printk("TY-3012S PciDrv DrvFilterMac->Group: %08x!\n",  DrvFilterMac->GroupNo);
			printk("TY-3012S PciDrv DrvFilterMac->Mac1L: %08x!\n",  DrvFilterMac->MacAddr1L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac1H: %08x!\n",  DrvFilterMac->MacAddr1H);
			printk("TY-3012S PciDrv DrvFilterMac->Mac2L: %08x!\n",  DrvFilterMac->MacAddr2L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac2H: %08x!\n",  DrvFilterMac->MacAddr2H);
			printk("TY-3012S PciDrv DrvFilterMac->Mac3L: %08x!\n",  DrvFilterMac->MacAddr3L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac3H: %08x!\n",  DrvFilterMac->MacAddr3H);
			printk("TY-3012S PciDrv DrvFilterMac->Mac4L: %08x!\n",  DrvFilterMac->MacAddr4L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac4H: %08x!\n",  DrvFilterMac->MacAddr4H);
			printk("TY-3012S PciDrv DrvFilterMac->Tail:  %08x!\n",  DrvFilterMac->Tail);
			#endif
			if((DrvFilterMac->ChNo == 1) || (DrvFilterMac->ChNo == 2)) {
				len = WriteFilterMac(DrvFilterMac->ChNo, DrvFilterMac->GroupNo);
				len = ReadFilterMac(DrvFilterMac->ChNo, DrvFilterMac->GroupNo);//��ͨ��1��2�Ĺ���MAC
				if(len == 0) {
					DrvFilterMac->ChNo = 0;
					DrvFilterMac->GroupNo = 0;
					DrvFilterMac->MacAddr1L = 0;
					DrvFilterMac->MacAddr1H = 0;
					DrvFilterMac->MacAddr2L = 0;
					DrvFilterMac->MacAddr2H = 0;
					DrvFilterMac->MacAddr3L = 0;
					DrvFilterMac->MacAddr3H = 0;
					DrvFilterMac->MacAddr4L = 0;
					DrvFilterMac->MacAddr4H = 0;
					printk("TY-3012S PciDrv SET_FILTER_MAC error %x!\n", DrvFilterMac->GroupNo);
				}
			} else {
				DrvFilterMac->ChNo = 0;
				DrvFilterMac->GroupNo = 0;
				DrvFilterMac->MacAddr1L = 0;
				DrvFilterMac->MacAddr1H = 0;
				DrvFilterMac->MacAddr2L = 0;
				DrvFilterMac->MacAddr2H = 0;
				DrvFilterMac->MacAddr3L = 0;
				DrvFilterMac->MacAddr3H = 0;
				DrvFilterMac->MacAddr4L = 0;
				DrvFilterMac->MacAddr4H = 0;
				len = 0;
				printk("TY-3012S PciDrv SET_FILTER_MAC error %x!!\n", DrvFilterMac->ChNo);//ͨ������
			}
			DrvFilterMac->Id =  ACK_FILTER_MACSET;
			#if 1
			printk("TY-3012S PciDrv DrvFilterMac->Head:  %08x!!\n", DrvFilterMac->Head);
			printk("TY-3012S PciDrv DrvFilterMac->Sid:   %08x!!\n", DrvFilterMac->VersionSid);
			printk("TY-3012S PciDrv DrvFilterMac->Id:    %08x!!\n", DrvFilterMac->Id);
			printk("TY-3012S PciDrv DrvFilterMac->ChNo:  %08x!!\n", DrvFilterMac->ChNo);
			printk("TY-3012S PciDrv DrvFilterMac->Group: %08x!!\n", DrvFilterMac->GroupNo);
			printk("TY-3012S PciDrv DrvFilterMac->Mac1L: %08x!!\n", DrvFilterMac->MacAddr1L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac1H: %08x!!\n", DrvFilterMac->MacAddr1H);
			printk("TY-3012S PciDrv DrvFilterMac->Mac2L: %08x!!\n", DrvFilterMac->MacAddr2L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac2H: %08x!!\n", DrvFilterMac->MacAddr2H);
			printk("TY-3012S PciDrv DrvFilterMac->Mac3L: %08x!!\n", DrvFilterMac->MacAddr3L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac3H: %08x!!\n", DrvFilterMac->MacAddr3H);
			printk("TY-3012S PciDrv DrvFilterMac->Mac4L: %08x!!\n", DrvFilterMac->MacAddr4L);
			printk("TY-3012S PciDrv DrvFilterMac->Mac4H: %08x!!\n", DrvFilterMac->MacAddr4H);
			printk("TY-3012S PciDrv DrvFilterMac->Tail:  %08x!!\n", DrvFilterMac->Tail);
			#endif
			break;
		case SET_DEVICE_LED://0x21������ǰ���LED״̬,�ܹ�64��LED
			#if 1
			printk("TY-3012S PciDrv DrvLedStatus->Head:  %08x!\n",  DrvLedStatus->Head);
			printk("TY-3012S PciDrv DrvLedStatus->Sid:   %08x!\n",  DrvLedStatus->VersionSid);
			printk("TY-3012S PciDrv DrvLedStatus->Id:    %08x!\n",  DrvLedStatus->Id);
			printk("TY-3012S PciDrv DrvLedStatus->Tail:  %08x!\n",  DrvLedStatus->Tail);
			#endif
			iowrite32(DrvLedStatus->LedL, ChxBaseAddr0 + 4 * LED1DISPLAY);
			iowrite32(DrvLedStatus->LedH, ChxBaseAddr0 + 4 * LED2DISPLAY);
			DrvLedStatus->LedL =ioread32(ChxBaseAddr0 + 4 * LED1DISPLAY);
			DrvLedStatus->LedH =ioread32(ChxBaseAddr0 + 4 * LED2DISPLAY);
			DrvLedStatus->Id = ACK_DEVICE_LED;
			#if 1
			printk("TY-3012S PciDrv DrvLedStatus->Head:  %08x!!\n", DrvLedStatus->Head);
			printk("TY-3012S PciDrv DrvLedStatus->Sid:   %08x!!\n", DrvLedStatus->VersionSid);
			printk("TY-3012S PciDrv DrvLedStatus->Id:    %08x!!\n", DrvLedStatus->Id);
			printk("TY-3012S PciDrv DrvLedStatus->LedL:  %08x!!\n", DrvLedStatus->LedL);
			printk("TY-3012S PciDrv DrvLedStatus->LedH:  %08x!!\n", DrvLedStatus->LedH);
			printk("TY-3012S PciDrv DrvLedStatus->Tail:  %08x!!\n", DrvLedStatus->Tail);
			len = MSG_BUF_LEN;
			#endif
			break;
		default:
			len = 0;
			printk("TY-3012S PciDrv command error!!\n");
			break;
	}
	if(copy_to_user((char *)buf, MsgBuffer, len)) {
		printk("TY-3012S PciDrv copy_to_user error!!\n");
		return -1;
	}
	return len;
}

/* �豸���ƺ������淶�ṹ */
static int pcifpga_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned i, j;
	int err = 0;

	switch(cmd) {
		case SET_ETH_INTERFACE://0x01�����ü��ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
			DrvChInterface->Head		= 0x1a2b3c4d;
			DrvChInterface->VersionSid	= 0;
			DrvChInterface->Id		= ACK_ETH_INTERFACE;
			DrvChInterface->Ch1Interface	= (arg >> 16) & 0x0001;//D1��2·��1ѡ�⣬0ѡ�磬ȱʡ��
			DrvChInterface->Ch2Interface	= (arg >> 00) & 0x0001;//D0��1·��1ѡ�⣬0ѡ�磬ȱʡ��
			DrvChInterface->Tail		= 0x0a1b2c3d4;
			i = (DrvChInterface->Ch2Interface << 1) | DrvChInterface->Ch1Interface;
			iowrite32(i, ChxBaseAddr0 + 4 * CHINTERFACESEL);
			j = ioread32(ChxBaseAddr0 + 4 * CHINTERFACESEL);
			if(i == j) err = 0;
			else err = -1;
			break;
		case SET_GPS_INTERFACE://0x02������GPS��1588�ӿڣ��Լ��ӿ�������TTL��485�ӿ�
			DrvGpsInterface->Head		= 0x1a2b3c4d;
			DrvGpsInterface->VersionSid	= 0;
			DrvGpsInterface->Id		= ACK_GPS_INTERFACE;
			DrvGpsInterface->Gps1588	= (arg >> 16) & 0x0001;//D1=1ѡ1588��D1=0ѡIRIGB��ȱʡIRIGB
			DrvGpsInterface->InterfaceType	= (arg >> 00) & 0x0001;//D0=1ƽ�⣬D0=0��ƽ��75ŷ��ȱʡ��ƽ��
			DrvGpsInterface->Tail		= 0x0a1b2c3d4;
			i = (DrvGpsInterface->Gps1588 << 1) | DrvGpsInterface->InterfaceType;
			iowrite32(i, ChxBaseAddr0 + 4 * GPSINTERFACESEL);
			j = ioread32(ChxBaseAddr0 + 4 * GPSINTERFACESEL);
			if(i == j) err = 0;
			else err = -1;
			break;
		case SET_TRIGGER_TIME://0x04�����ü��ͨ���ⴥ������ʱ�䣬��λus
			DrvTriggerTime->Head		= 0x1a2b3c4d;
			DrvTriggerTime->VersionSid	= 0;
			DrvTriggerTime->Id		= ACK_TRIGGER_TIME;
			DrvTriggerTime->Sw1TrigerTime	= (arg >> 16) & 0x00ff;//�ⴥ���ӿ�1����λus����С0�����256
			DrvTriggerTime->Sw2TrigerTime	= (arg >> 00) & 0x00ff;//�ⴥ���ӿ�2����λus����С0�����256
			DrvTriggerTime->Tail		= 0x0a1b2c3d4;
			iowrite32(DrvTriggerTime->Sw1TrigerTime, ChxBaseAddr0 + 4 * SW1TRIGGERTIME);
			iowrite32(DrvTriggerTime->Sw2TrigerTime, ChxBaseAddr0 + 4 * SW2TRIGGERTIME);
			i = ioread32(ChxBaseAddr0 + 4 * SW1TRIGGERTIME);
			j = ioread32(ChxBaseAddr0 + 4 * SW2TRIGGERTIME);
			if((i == DrvTriggerTime->Sw1TrigerTime) && (j == DrvTriggerTime->Sw2TrigerTime)) err = 0;
			else err = -1;
			break;
		case SET_ALARM_OUT://0x05�����ü��ͨ������澯������̵����պ�
			DrvAlarmOut->Head		= 0x1a2b3c4d;
			DrvAlarmOut->VersionSid		= 0;
			DrvAlarmOut->Id			= ACK_ALARM_OUT;
			DrvAlarmOut->Sw1AlarmOut	= (arg >> 16) & 0x0001;//��澯�ӿ�1��D0=1�պϣ�0=�Ͽ�
			DrvAlarmOut->Sw2AlarmOut	= (arg >> 00) & 0x0001;//��澯�ӿ�2��D1=1�պϣ�0=�Ͽ�
			DrvAlarmOut->Tail		= 0x0a1b2c3d4;
			i = (DrvAlarmOut->Sw2AlarmOut << 1) | DrvAlarmOut->Sw1AlarmOut;
			iowrite32(i, ChxBaseAddr0 + 4 * SWALARMOUT);
			j = ioread32(ChxBaseAddr0 + 4 * SWALARMOUT);
			if(i == j) err = 0;
			else err = -1;
			break;
		case SET_BEEP_ALARM://0x06�������ڲ��������澯�������������
			DrvBeepAlarm->Head		= 0x1a2b3c4d;
			DrvBeepAlarm->VersionSid	= 0;
			DrvBeepAlarm->Id		= ACK_BEEP_ALARM;
			DrvBeepAlarm->BeepAlarm		= arg & 0x03;//D1=1��һ������������D0=1����
			DrvBeepAlarm->Tail		= 0x0a1b2c3d4;
			i = DrvBeepAlarm->BeepAlarm;
			iowrite32(i, ChxBaseAddr0 + 4 * BEEPALARMOUT);
			j = ioread32(ChxBaseAddr0 + 4 * BEEPALARMOUT);
			if(i == j) err = 0;
			else err = -1;
			break;
		case SET_WARCH_DOG://0x07�����ü�����ⲿ���Ź������Ź�Ҫ��ʱ���
			DrvWarchDog->Head		= 0x1a2b3c4d;
			DrvWarchDog->VersionSid		= 0;
			DrvWarchDog->Id			= ACK_WARCH_DOG;
			DrvWarchDog->WarchDog		= arg & 0x0ff;//д0xFF������Ź���Ȼ�������D7��D0����
			DrvWarchDog->Tail		= 0x0a1b2c3d4;
			iowrite32(DrvWarchDog->WarchDog, ChxBaseAddr0 + 4 * WARCHDOGOUT);
			err = 0;
			break;
		case SET_BUFFER_EMPTY://0x0A���������FIFO�е�����
			DrvClearFifo->Head		= 0x1a2b3c4d;
			DrvClearFifo->VersionSid	= 0;
			DrvClearFifo->Id		= ACK_BUFFER_EMPTY;
			DrvClearFifo->ClearFifo1	= (arg >> 16) & 0x00ff;//д0xFF��FIFO��Ȼ�������D7��D0
			DrvClearFifo->ClearFifo2	= (arg >> 00) & 0x00ff;//д0xFF��FIFO��Ȼ�������D7��D0
			DrvClearFifo->Tail		= 0x0a1b2c3d4;
			iowrite32(DrvClearFifo->ClearFifo1, ChxBaseAddr0 + 4 * CH1FIFORESET);
			iowrite32(DrvClearFifo->ClearFifo2, ChxBaseAddr0 + 4 * CH2FIFORESET);
			err = 0;
			break;
		case CHECK_CH_INTERFACE://0x12�������ͨ������̫���ӿ����ͣ���ӿڻ��ӿ�
			DrvChInterface->Head		= 0x1a2b3c4d;
			DrvChInterface->VersionSid	= 0;
			DrvChInterface->Id		= ACK_CH_INTERFACE;
			DrvChInterface->Tail		= 0x0a1b2c3d4;
			i = ioread32(ChxBaseAddr0 + 4 * CHINTERFACESEL);
			DrvChInterface->Ch1Interface	= 0;
			DrvChInterface->Ch2Interface	= 0;
			if((i & 0x01) == 0x01)
				DrvChInterface->Ch1Interface = 0x0001;//D0��1·��1ѡ�⣬0ѡ�磬ȱʡ��
			if((i & 0x02) == 0x02)
				DrvChInterface->Ch2Interface = 0x0001;//D1��2·��1ѡ�⣬0ѡ�磬ȱʡ��
			err = (DrvChInterface->Ch1Interface << 16) | DrvChInterface->Ch2Interface;
			break;
		case CHECK_TRIGGER_STATUS://0x15�������ͨ���ⴥ��״̬�����޴������Ƿ����ڴ���
			DrvTriggerStatus->Head		= 0x1a2b3c4d;
			DrvTriggerStatus->VersionSid	= 0;
			DrvTriggerStatus->Id		= ACK_TRIGGER_STATUS;
			DrvTriggerStatus->Tail		= 0x0a1b2c3d4;
			i = ioread32(ChxBaseAddr0 + 4 * SW1STATUS);
			j = ioread32(ChxBaseAddr0 + 4 * SW2STATUS);
			DrvTriggerStatus->Sw1NowStatus = 0;
			DrvTriggerStatus->Sw1HisStatus = 0;
			DrvTriggerStatus->Sw2NowStatus = 0;
			DrvTriggerStatus->Sw2HisStatus = 0;
			if((i & 0x01) == 0x01)
				DrvTriggerStatus->Sw1NowStatus = 0x01;//D0=1��ʾĿǰ�̵����պ�
			if((i & 0x02) == 0x02)
				DrvTriggerStatus->Sw1HisStatus = 0x01;//D1=1��ʾ�����п�������
			if((j & 0x01) == 0x01)
				DrvTriggerStatus->Sw2NowStatus = 0x01;//D0=1��ʾĿǰ�̵����պ�
			if((j & 0x02) == 0x02)
				DrvTriggerStatus->Sw2HisStatus = 0x01;//D1=1��ʾ�����п�������
			err = 0;
			err = (err << 8) | DrvTriggerStatus->Sw1NowStatus;
			err = (err << 8) | DrvTriggerStatus->Sw1HisStatus;
			err = (err << 8) | DrvTriggerStatus->Sw2NowStatus;
			err = (err << 8) | DrvTriggerStatus->Sw2HisStatus;
			break;
		case CHECK_KEY_STATUS://0x16�����ǰ��尴��״̬�����ް������Ƿ����ڰ�
			DrvKeyStatus->Head		= 0x1a2b3c4d;
			DrvKeyStatus->VersionSid	= 0;
			DrvKeyStatus->Id		= ACK_KEY_STATUS;
			DrvKeyStatus->Tail		= 0x0a1b2c3d4;
			i = ioread32(ChxBaseAddr0 + 4 * KEY1STATUS);
			j = ioread32(ChxBaseAddr0 + 4 * KEY2STATUS);
			DrvKeyStatus->Key1NowStatus = 0;
			DrvKeyStatus->Key1HisStatus = 0;
			DrvKeyStatus->Key2NowStatus = 0;
			DrvKeyStatus->Key2HisStatus = 0;
			if((i & 0x01) == 0x01)
				DrvKeyStatus->Key1NowStatus = 0x01;//D0=1��ʾĿǰ�̵����պ�
			if((i & 0x02) == 0x02)
				DrvKeyStatus->Key1HisStatus = 0x01;//D1=1��ʾ�����п�������
			if((j & 0x01) == 0x01)
				DrvKeyStatus->Key2NowStatus = 0x01;//D0=1��ʾĿǰ�̵����պ�
			if((j & 0x02) == 0x02)
				DrvKeyStatus->Key2HisStatus = 0x01;//D1=1��ʾ�����п�������
			err = 0;
			err = (err << 8) | DrvKeyStatus->Key1NowStatus;
			err = (err << 8) | DrvKeyStatus->Key1HisStatus;
			err = (err << 8) | DrvKeyStatus->Key2NowStatus;
			err = (err << 8) | DrvKeyStatus->Key2HisStatus;
			break;
		case CHECK_CH_STATUS://0x13�������ͨ�����ӿڵ�����״̬�����ӡ��ٶȡ�˫��
			DrvChStatus->Head	= 0x1a2b3c4d;
			DrvChStatus->VersionSid	= 0;
			DrvChStatus->Id		= ACK_CH_STATUS;
			DrvChStatus->EthOLink	= 0;
			DrvChStatus->EthELink	= 4;
			DrvChStatus->Tail	= 0x0a1b2c3d4;
			//����4��PHY��״̬��
			//PHYADD1 = 0x01;	//��ӿ�1��PHY��ַ
			//PHYADD2 = 0x02;	//��ӿ�1��PHY��ַ
			//PHYADD3 = 0x03;	//��ӿ�2��PHY��ַ
			//PHYADD2 = 0x04;	//��ӿ�2��PHY��ַ
			//PHYREG1 = 0x04;	//PHY״̬�Ĵ�����ַ
			//PHYRDC  = 0x02;	//дPHY�ļĴ�������
			//PHYRDC  = 0x04;	//��PHY�ļĴ�������
			//iowrite32(0x10, ChxBaseAddr0 + 4 * ETHMIIMMDC);//��Ƶ��������16��Ƶ=2MHz
			//iowrite32(((PHYADD1 << 8) | PHYREG1), ChxBaseAddr0 + 4 * ETHMIIMPHYADDREG);//PHY��ַ��PHY״̬�Ĵ���
			//iowrite32(PHYRDC, ChxBaseAddr0 + 4 * ETHMIIMCOMMAND);
			//i = ioread32(ChxBaseAddr0 + 4 * ETHMIIMRDDATA);
			break;
		default:
			printk("TY-3012S PciDrv ioctl command err!\n");
			err = -1;
			break;
	}
	return err;
}

/* �豸�رպ������淶�ṹ */
static int pcifpga_release(struct inode *inode, struct file *filp)
{
	//printk("TY-3012S PciDrv pcifpga_release closed!\n");
	return 0;
}

static struct file_operations pcifpga_fops = {
	.owner		= THIS_MODULE,
	.open		= pcifpga_open,
	.read		= pcifpga_read,
	.write		= pcifpga_write,
	.ioctl		= pcifpga_ioctl,
	.release	= pcifpga_release,
};

//ָ�������������õ�PCI�豸ID
static struct pci_device_id pcifpga_table[] __initdata = {
	{ CTD_VENDOR_ID, CTD_DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID, },
	{ 0, }
};	//����ID, �豸ID, �ӳ���ID, ���豸ID

MODULE_DEVICE_TABLE(pci, pcifpga_table);

static unsigned char get_pcifpga_revision(struct pci_dev *pcifpga_dev)
{
	u8 pcifpga_revision;

	pci_read_config_byte(pcifpga_dev, PCI_REVISION_ID, &pcifpga_revision);	//PCI_REVISION_ID�Ѿ�����
	printk("TY-3012S PciDrv PCI_REVISION_ID: %x!\n", pcifpga_revision);	//��ʾ��������İ汾��
	return pcifpga_revision;
}

//PCIFPGA���Կ����жϷ������
static irqreturn_t pcifpga_interrupt(int irq, void *dev_id)
{
	unsigned int ints, fifo1s, fifo2s, fifo1c, fifo2c;
	unsigned int key1s, key2s, rely1, rely2;
	unsigned int ch1fc, ch2fc, ch1bc, ch2bc;
	//unsigned int *dmaptr1, *dmaptr2;
	unsigned int i, j;

	//��PCIFPGA��FIFO1��FIFO2�������ж������ݵ�DMA������
	ints = ioread32(ChxBaseAddr0 + 4 * INTERRUPTSTATUS);
	//D6/5��2/1·64K��,D4/3����2/1,D2/1����2/1,D0��ʱ,1�ж�,�����Զ����
	//printk("TY-3012S PciDrv INTSTATUS:     %08x!\n", ints);
	//���ÿ����Ͱ���������
	fifo1c = 0;
	fifo2c = 0;
	#if 1
	if((ints & 0x02) == 0x02) {
		key1s = ioread32(ChxBaseAddr0 + 4 * KEY1STATUS);
		//D1=1���а�����,D0=1Ŀǰ�������պ�,��ѯ���Զ����
		//printk("TY-3012S PciDrv KEY1TRIGGERS:  %08x!\n", key1s);
		//��ʾ������ͳ��ֵ
		ch1fc = ioread32(Ch1BaseAddr1 + 4 * CHALLFRAMEH);
		printk("\nTY-3012S PciDrv CH1ALLFRAMEH:  %08x!\n", ch1fc);
		ch1fc = ioread32(Ch1BaseAddr1 + 4 * CHALLFRAMEL);
		printk("TY-3012S PciDrv CH1ALLFRAMEL:  %08x!\n", ch1fc);
		ch1bc = ioread32(Ch1BaseAddr1 + 4 * CHALLBYTEH);
		printk("TY-3012S PciDrv CH1ALLBYTEH:   %08x!\n", ch1bc);
		ch1bc = ioread32(Ch1BaseAddr1 + 4 * CHALLBYTEL);
		printk("TY-3012S PciDrv CH1ALLBYTEL:   %08x!\n", ch1bc);
		ch2fc = ioread32(Ch2BaseAddr2 + 4 * CHALLFRAMEH);
		printk("TY-3012S PciDrv CH2ALLFRAMEH:  %08x!\n", ch2fc);
		ch2fc = ioread32(Ch2BaseAddr2 + 4 * CHALLFRAMEL);
		printk("TY-3012S PciDrv CH2ALLFRAMEL:  %08x!\n", ch2fc);
		ch2bc = ioread32(Ch2BaseAddr2 + 4 * CHALLBYTEH);
		printk("TY-3012S PciDrv CH2ALLBYTEH:   %08x!\n", ch2bc);
		ch2bc = ioread32(Ch2BaseAddr2 + 4 * CHALLBYTEL);
		printk("TY-3012S PciDrv CH2ALLBYTEL:   %08x!\n", ch2bc);
		//��FIFOȡ������˫����,����İ���,���˫����,����˫����
		printk("\nTY-3012S PciDrv Ch1: FifoDword, Packet, WrDword, RdDord!\n");
		printk("TY-3012S PciDrv Fifo1DwCnt:    %08x!\n", Ch1Cnt);
		printk("TY-3012S PciDrv Ch1PacketCnt:  %08x!\n", Ch1PacketCnt);
		printk("TY-3012S PciDrv Ch1PacketDw:   %08x!\n", Ch1PacketDw);
		printk("TY-3012S PciDrv ReadCh1Dword:  %08x!\n", ReadCh1Dword);
		printk("\nTY-3012S PciDrv Ch2: FifoDword, Packet, WrDword, RdDord!\n");
		printk("TY-3012S PciDrv Fifo2DwCnt:    %08x!\n", Ch2Cnt);
		printk("TY-3012S PciDrv Ch2PacketCnt:  %08x!\n", Ch2PacketCnt);
		printk("TY-3012S PciDrv Ch2PacketDw:   %08x!\n", Ch2PacketDw);
		printk("TY-3012S PciDrv ReadCh2Dword:  %08x!\n", ReadCh2Dword);
		//����������ͳ��ֵ
	}
	if((ints & 0x04) == 0x04) {
		key2s = ioread32(ChxBaseAddr0 + 4 * KEY2STATUS);
		//D1=1���а�����,D0=1Ŀǰ�������պ�,��ѯ���Զ����
		//printk("TY-3012S PciDrv KEY2TRIGGERS:  %08x!\n", key2s);
		i = DisplayFpgaTimer();
	}
	if((ints & 0x08) == 0x08) {
		rely1 = ioread32(ChxBaseAddr0 + 4 * SW1STATUS);
		//D1=1���п�����,D0=1Ŀǰ�̵����պ�,��ѯ���Զ����
		printk("TY-3012S PciDrv SW1TRIGGERS:   %08x!\n", rely1);
	}
	if((ints & 0x10) == 0x10) {
		rely2 = ioread32(ChxBaseAddr0 + 4 * SW2STATUS);
		//D1=1���п�����,D0=1Ŀǰ�̵����պ�,��ѯ���Զ����
		printk("TY-3012S PciDrv SW2TRIGGERS:   %08x!\n", rely2);
	}
	if((ints & 0x20) == 0x20) {
		fifo1s = ioread32(ChxBaseAddr0 + 4 * CH1FIFOSTATUS);
		//D4��,D3��,D2����,D1������,D0������,����Ч
		printk("TY-3012S PciDrv CH1FIFOSTATUS: %08x!\n", fifo1s);
		iowrite32(CHFIFORESET, ChxBaseAddr0 + 4 * CH1FIFORESET);
	}
	if((ints & 0x40) == 0x40) {
		fifo2s = ioread32(ChxBaseAddr0 + 4 * CH2FIFOSTATUS);
		//D4��,D3��,D2����,D1������,D0������,����Ч
		printk("TY-3012S PciDrv CH2FIFOSTATUS: %08x!\n", fifo2s);
		iowrite32(CHFIFORESET, ChxBaseAddr0 + 4 * CH2FIFORESET);
	}
	#endif
	//��ʱ�ж�
	#if 1
	//FIFO״̬,D4��,D3��,D2����,D1����,D0����,����Ч,�����ж�
	if((ints & 0x01) == 0x01) {
		fifo1s = ioread32(ChxBaseAddr0 + 4 * CH1FIFOSTATUS);
		//printk("TY-3012S PciDrv CH1FIFOSTATUS: %08x!\n", fifo1s);
		fifo2s = ioread32(ChxBaseAddr0 + 4 * CH2FIFOSTATUS);
		//printk("TY-3012S PciDrv CH2FIFOSTATUS: %08x!\n", fifo2s);
		fifo1c = ioread32(ChxBaseAddr0 + 4 * CH1FIFOCOUNT) >> 16;
		if(fifo1c >= 0x3fff) printk("TY-3012S PciDrv CH1FIFOCOUNTE: %08x!\n", fifo1c);
		fifo1c = fifo1c & 0x3fff;//˫����
		//printk("TY-3012S PciDrv CH1FIFOCOUNT:  %08x!\n", fifo1c);
		fifo2c = ioread32(ChxBaseAddr0 + 4 * CH2FIFOCOUNT) >> 16;
		if(fifo2c >= 0x3fff) printk("TY-3012S PciDrv CH2FIFOCOUNTE: %08x!\n", fifo2c);
		fifo2c = fifo2c & 0x3fff;//˫����
		//printk("TY-3012S PciDrv CH2FIFOCOUNT:  %08x!\n", fifo2c);
		if((fifo1s & 0x08) == 0x08) {
			//printk("TY-3012S PciDrv CH2FIFOFULL:   %08x!\n", fifo1s);
			Ch1FifoFull++;
			iowrite32(CHFIFORESET, ChxBaseAddr0 + 4 * CH1FIFORESET);//���FIFO
			//fifo1s = ioread32(ChxBaseAddr0 + 4 * CH1FIFOSTATUS);
			//printk("TY-3012S PciDrv CH1FIFOSTATUS: %08x!!\n", fifo1s);
			fifo1c = 0;
		}
		if(fifo1c > 0) ReadFifo1(fifo1c);
		if((fifo2s & 0x08) == 0x08) {
			//printk("TY-3012S PciDrv CH1FIFOFULL:   %08x!\n", fifo2s);
			Ch2FifoFull++;
			iowrite32(CHFIFORESET, ChxBaseAddr0 + 4 * CH2FIFORESET);//���FIFO
			//fifo2s = ioread32(ChxBaseAddr0 + 4 * CH2FIFOSTATUS);
			//printk("TY-3012S PciDrv CH2FIFOSTATUS: %08x!!\n", fifo2s);
			fifo2c = 0;
		}
		if(fifo2c > 0) ReadFifo2(fifo2c);
	}
	#endif
	#if 1
	i = 1;
	while(i) {
		i = ReadFrameBuf1();
		if(i != 0) {
			//printk("TY-3012S PciDrv ReadFrameBuf1: %08x!\n", i);
			j = WriteCh1Buf(i);//˫��
			//printk("TY-3012S PciDrv WriteCh1Buf:   %08x!\n", j);
		}
	}
	i =1;
	while(i) {
		i = ReadFrameBuf2();
		if(i != 0) {
			//printk("TY-3012S PciDrv ReadFrameBuf2: %08x!\n", i);
			j = WriteCh2Buf(i);//˫��
			//printk("TY-3012S PciDrv WriteCh2Buf:   %08x!\n", j);
		}
	}
	#endif
	//ͨ��DMABUF����FIFO1��FIFO2��ȡ����,����11MByte/s
	#if 0
	if(fifo1c > 0) {
		dmaptr1 = (unsigned int *)pcifpga_card_dma1;
		dmaptr2 = Ch1BaseAddr3;
		printk("TY-3012S PciDrv Read CH1FIFO:  %08d!", fifo1c);
		memcpy(dmaptr1, dmaptr2, fifo1c);
		for(i = 0; i < fifo1c; i++) {
			if((i % 16) == 0) printk("\n");
			printk(".%02x", *(pcifpga_card_dma1 + i));
		}
		printk("!\n");
		printk("TY-3012S PciDrv End Read CH1FIFO!\n\n");
	}
	if(fifo2c > 0) {
		dmaptr1 = (unsigned int *)pcifpga_card_dma2;
		dmaptr2 = Ch2BaseAddr4;
		printk("TY-3012S PciDrv Read CH2FIFO:  %08d!", fifo2c);
		memcpy(dmaptr1, dmaptr2, fifo2c);
		for(i = 0; i < fifo2c; i++) {
			if((i % 16) == 0) printk("\n");
			printk(".%02x", *(pcifpga_card_dma2 + i));
		}
		printk("!\n");
		printk("TY-3012S PciDrv End Read CH2FIFO!\n\n");
	}
	#endif
	//ע����Ҫ������ʱ
	m_second ++;
	if(m_second > MAX_M_SECOND) {
		m_second = 0;
		second_int ++;
		if(Ch1FifoFull > 0)   printk("TY-3012S PciDrv CH1FIFOFULL:   %08x!\n", Ch1FifoFull);
		Ch1FifoFull = 0;
		if(Ch2FifoFull > 0)   printk("TY-3012S PciDrv CH2FIFOFULL:   %08x!\n", Ch2FifoFull);
		Ch2FifoFull = 0;
		if(Ch1DmaBufFull > 0) printk("TY-3012S PciDrv CH1DMABUFFULL: %08x!\n", Ch1DmaBufFull);
		Ch1DmaBufFull = 0;
		if(Ch2DmaBufFull > 0) printk("TY-3012S PciDrv CH2DMABUFFULL: %08x!\n", Ch2DmaBufFull);
		Ch2DmaBufFull = 0;
		if(Ch1BufFull > 0)    printk("TY-3012S PciDrv CH1BUFFULL:    %08x!\n", Ch1BufFull);
		Ch1BufFull = 0;
		if(Ch2BufFull > 0)    printk("TY-3012S PciDrv CH2BUFFULL:    %08x!\n", Ch2BufFull);
		Ch2BufFull = 0;
		//printk("TY-3012S PciDrv interrupt: %d!\n", second_int);
	}
	return IRQ_HANDLED;
}

//̽��PCIFPGA���Կ��豸
static int __init pcifpga_probe(struct pci_dev *pcifpga_dev, const struct pci_device_id *pcifpga_id)
{
	u8	config_reg8;
	u16	config_reg16;
	int	retval;

	pcifpga_card_dev = pcifpga_dev;				//����PCIFPGA���ı�����pcifpga_card_dev
	//����PCI�豸                                           //pcifpga_card_devͨ��Linuxϵͳ��PCI������
	if(pci_enable_device(pcifpga_dev)) {
		printk("TY-3012S PciDrv pci_enable_device Err!\n");
		return -EIO;
	}
	//��ȡPCIFPGA���Կ���������Ϣ
	ctd_pci_vendor = pcifpga_dev->vendor;			//PCI�豸���ÿռ��е�Device ID
	ctd_pci_device = pcifpga_dev->device;			//PCI�豸���ÿռ��е�Vendor ID
	printk("TY-3012S PciDrv Device ID: %x!\n", ctd_pci_vendor);
	printk("TY-3012S PciDrv Vendor ID: %x!\n", ctd_pci_device);
	//���PCIFPGA���Կ��İ汾��
	if(get_pcifpga_revision(pcifpga_dev) != 0x00) {		//�汾ӦΪ0x00
		printk("TY-3012S PciDrv get_pcifpga_revision Err!\n");
		return -ENODEV;
	}
	//ȷ��PCIFPGA���Կ����ж�IRQ
	pcifpga_irq = pcifpga_dev->irq;				//ȷ���жϺţ�ϵͳ�����PCIFPGA��
	printk("PCI TY-3012S PciDrv Interrupt: %d!\n", pcifpga_irq);
	//�趨PCIFPGA���Կ��Ĵ洢��ַ���䷶Χ
	chx_mem0_base = pci_resource_start(pcifpga_dev, 0);	//ȷ���洢���ռ�0(����)�Ļ���ַ,DMA
	chx_mem0_range = pci_resource_len (pcifpga_dev, 0);	//ȷ���洢���ռ�0(����)�ķ�Χ,DMA
	printk("PCI TY-3012S PciDrv chx_mem0_base: %x, and Range: %x!\n", (unsigned int)chx_mem0_base, chx_mem0_range);
	ch1_mem1_base = pci_resource_start(pcifpga_dev, 1);	//ȷ���洢���ռ�1(ͨ��1)�Ļ���ַ,DMA
	ch1_mem1_range = pci_resource_len (pcifpga_dev, 1);	//ȷ���洢���ռ�1(ͨ��1)�ķ�Χ,DMA
	printk("PCI TY-3012S PciDrv ch1_mem1_base: %x, and Range: %x!\n", (unsigned int)ch1_mem1_base, ch1_mem1_range);
	ch2_mem2_base = pci_resource_start(pcifpga_dev, 2);	//ȷ���洢���ռ�2(ͨ��2)�Ļ���ַ,DMA
	ch2_mem2_range = pci_resource_len (pcifpga_dev, 2);	//ȷ���洢���ռ�2(ͨ��2)�ķ�Χ,DMA
	printk("PCI TY-3012S PciDrv ch2_mem2_base: %x, and Range: %x!\n", (unsigned int)ch2_mem2_base, ch2_mem2_range);
	ch1_mem3_base = pci_resource_start(pcifpga_dev, 3);	//ȷ���洢���ռ�2(ͨ��1)�Ļ���ַ,DMA
	ch1_mem3_range = pci_resource_len (pcifpga_dev, 3);	//ȷ���洢���ռ�2(ͨ��1)�ķ�Χ,DMA
	printk("PCI TY-3012S PciDrv ch1_mem3_base: %x, and Range: %x!\n", (unsigned int)ch1_mem3_base, ch1_mem3_range);
	ch2_mem4_base = pci_resource_start(pcifpga_dev, 4);	//ȷ���洢���ռ�4(ͨ��2)�Ļ���ַ,DMA
	ch2_mem4_range = pci_resource_len (pcifpga_dev, 4);	//ȷ���洢���ռ�4(ͨ��2)�ķ�Χ,DMA
	printk("PCI TY-3012S PciDrv ch2_mem4_base: %x, and Range: %x!\n", (unsigned int)ch2_mem4_base, ch2_mem4_range);
	ChxBaseAddr0 = pci_iomap(pcifpga_dev, 0, 0);//�洢����д
	printk("PCI TY-3012S PciDrv ChxBaseAddr0:  %x!\n", (unsigned int)ChxBaseAddr0);
	Ch1BaseAddr1 = pci_iomap(pcifpga_dev, 1, 0);//�洢����д
	printk("PCI TY-3012S PciDrv Ch1BaseAddr1:  %x!\n", (unsigned int)Ch1BaseAddr1);
	Ch2BaseAddr2 = pci_iomap(pcifpga_dev, 2, 0);//�洢����д
	printk("PCI TY-3012S PciDrv Ch2BaseAddr2:  %x!\n", (unsigned int)Ch2BaseAddr2);
	Ch1BaseAddr3 = pci_iomap(pcifpga_dev, 3, 0);//�洢����д
	printk("PCI TY-3012S PciDrv Ch1BaseAddr3:  %x!\n", (unsigned int)Ch1BaseAddr3);
	Ch2BaseAddr4 = pci_iomap(pcifpga_dev, 4, 0);//�洢����д
	printk("PCI TY-3012S PciDrv Ch2BaseAddr4:  %x!\n", (unsigned int)Ch2BaseAddr4);
	//�����ж�IRQ���趨�жϷ����Ӻ���
	retval = request_irq(pcifpga_irq, pcifpga_interrupt, 0, CTD_PCI_NAME, NULL);//IRQF_SHARED
	if(retval) {
		printk ("TY-3012S PciDrv Can't get assigned IRQ %d!\n", pcifpga_irq);
 		return -1;
	}
	printk ("TY-3012S PciDrv Request IRQ: %d, and m_second: %d!\n", retval, m_second);
	//ʹ������������ȡ���üĴ���
	#if 1
	pci_read_config_word(pcifpga_dev, 0x00, &config_reg16);	//�����üĴ��������Һ�
	printk("TY-3012S PciDrv pci_read_config_word00: %x!\n", config_reg16);
	pci_read_config_word(pcifpga_dev, 0x02, &config_reg16);	//�����üĴ�����װ�ú�
	printk("TY-3012S PciDrv pci_read_config_word02: %x!\n", config_reg16);
	pci_read_config_word(pcifpga_dev, 0x04, &config_reg16);	//�����üĴ���������Ĵ���
	printk("TY-3012S PciDrv pci_read_config_word04: %x!\n", config_reg16);
	pci_read_config_word(pcifpga_dev, 0x06, &config_reg16);	//�����üĴ�����״̬�Ĵ���
	printk("TY-3012S PciDrv pci_read_config_word06: %x!\n", config_reg16);
	pci_read_config_byte(pcifpga_dev, 0x3c, &config_reg8);	//�����üĴ������ж��߼Ĵ���
	printk("TY-3012S PciDrv pci_read_config_word3c: %x!\n", config_reg8);
	pci_read_config_byte(pcifpga_dev, 0x3d, &config_reg8);	//�����üĴ������жϽżĴ���
	printk("TY-3012S PciDrv pci_read_config_word3d: %x!\n", config_reg8);
	#endif
	//���ں˿ռ��ж�̬�����ڴ�
	//Ϊ��Ϣ���뻺��
	MsgBuffer = kmalloc(MSG_BUF_LEN, GFP_DMA | GFP_KERNEL);	//����һ�������Ļ��棬64�ֽڣ�Ϊ��ȡ��Ϣ����
	if(!MsgBuffer) {					//MSG_BUF_LEN=0x40Byte
		free_irq(pcifpga_irq, NULL);
		printk("TY-3012S PciDrv kmalloc MsgBuffer Err!\n");
		return -ENOMEM;
	}
	memset(MsgBuffer, 0, MSG_BUF_LEN);
	DrvMsgCommand	= (MsgCommand*)MsgBuffer;
	DrvChInterface	= (MsgChInterface*)MsgBuffer;
	DrvGpsInterface	= (MsgGpsInterface*)MsgBuffer;
	DrvFpgaTime	= (MsgFpgaTime*)MsgBuffer;
	DrvTriggerTime	= (MsgTriggerTime*)MsgBuffer;
	DrvAlarmOut	= (MsgAlarmOut*)MsgBuffer;
	DrvBeepAlarm	= (MsgBeepAlarm*)MsgBuffer;
	DrvWarchDog	= (MsgWarchDog*)MsgBuffer;
	DrvClearFifo	= (MsgClearFifo*)MsgBuffer;
	DrvChFilter	= (MsgChFilter*)MsgBuffer;
	DrvFilterMac	= (MsgFilterMac*)MsgBuffer;
	DrvChStatus	= (MsgChStatus*)MsgBuffer;
	DrvTriggerStatus= (MsgTriggerStatus*)MsgBuffer;
	DrvKeyStatus	= (MsgKeyStatus*)MsgBuffer;
	DrvFrameCounters= (MsgFrameCounters*)MsgBuffer;
	DrvLedStatus	= (MsgLedStatus*)MsgBuffer;
	DrvReadBuffer	= (MsgReadBuffer*)MsgBuffer;
	//ΪFIFO1���뻺��
	if((pcifpga_card_dma1 = kmalloc(PCIFPGA_DMABUF_BYTELEN, GFP_DMA | GFP_KERNEL)) == NULL) {//����128K˫��
		free_irq(pcifpga_irq, NULL);
		kfree(MsgBuffer);
		printk("TY-3012S PciDrv kmalloc pcifpga_card_dma1 Err!\n");
		return -ENOMEM;
	}
	memset(pcifpga_card_dma1, 0, PCIFPGA_DMABUF_BYTELEN);//DMA����������
	ReadCh1Pointer = WriteCh1Pointer = Ch1Cnt = 0;//���ö�дָ���˫�ּ�����Ϊ0
	Ch1FifoFull = Ch1DmaBufFull = Ch1BufFull = 0;//�����������������־
	printk("TY-3012S PciDrv pcifpga_card_dma1: %x!\n", (unsigned int)pcifpga_card_dma1);
	//ΪFIFO2���뻺��
	if((pcifpga_card_dma2 = kmalloc(PCIFPGA_DMABUF_BYTELEN, GFP_DMA | GFP_KERNEL)) == NULL) {//����128K˫��
		free_irq(pcifpga_irq, NULL);
		kfree(MsgBuffer);
		kfree(pcifpga_card_dma1);
		printk("TY-3012S PciDrv kmalloc pcifpga_card_dma2 Err!\n");
		return -ENOMEM;
	}
	memset(pcifpga_card_dma2, 0, PCIFPGA_DMABUF_BYTELEN);//DMA����������
	ReadCh2Pointer = WriteCh2Pointer = Ch2Cnt = 0;//���ö�дָ���˫�ּ�����Ϊ0
	Ch2FifoFull = Ch2DmaBufFull = Ch2BufFull = 0;//�����������������־
	printk("TY-3012S PciDrv pcifpga_card_dma2: %x!\n", (unsigned int)pcifpga_card_dma2);
	//ȷ��FIFO�������������ַ
	PciDmaBufPhyAddr1 = pci_map_single(pcifpga_card_dev, pcifpga_card_dma1,  PCIFPGA_DMABUF_BYTELEN, DMA_FROM_DEVICE);
	printk("PCI TY-3012S PciDrv PciDmaBufPhyAddr1: %x!\n", (unsigned int)PciDmaBufPhyAddr1);
	PciDmaBufPhyAddr2 = pci_map_single(pcifpga_card_dev, pcifpga_card_dma2,  PCIFPGA_DMABUF_BYTELEN, DMA_FROM_DEVICE);
	printk("PCI TY-3012S PciDrv PciDmaBufPhyAddr2: %x!\n", (unsigned int)PciDmaBufPhyAddr2);
	//Ϊ��֡�����������ڴ�
	if((pcifpga_card_buf0 = kmalloc(PCIFPGA_PACKET_BYTELEN, GFP_DMA | GFP_KERNEL)) == NULL) {//����1K˫��
		free_irq(pcifpga_irq, NULL);
		kfree(MsgBuffer);
		kfree(pcifpga_card_dma1);
		kfree(pcifpga_card_dma2);
		printk("TY-3012S PciDrv kmalloc pcifpga_card_buf0 Err!\n");
		return -ENOMEM;
	}
	memset(pcifpga_card_buf0, 0, PCIFPGA_PACKET_BYTELEN);//�������������
	printk("TY-3012S PciDrv pcifpga_card_buf0: %x!\n", (unsigned int)pcifpga_card_buf0);
	//ΪӦ�û�����1�����ڴ�
	if((pcifpga_card_buf1 = kmalloc(PCIFPGA_BUF_BYTELEN, GFP_DMA | GFP_KERNEL)) == NULL) {//����512K˫��
		free_irq(pcifpga_irq, NULL);
		kfree(MsgBuffer);
		kfree(pcifpga_card_dma1);
		kfree(pcifpga_card_dma2);
		kfree(pcifpga_card_buf0);
		printk("TY-3012S PciDrv kmalloc pcifpga_card_buf1 Err!\n");
		return -ENOMEM;
	}
	memset(pcifpga_card_buf1, 0, PCIFPGA_BUF_BYTELEN);//������1����
	ReadBuf1Pointer = WriteBuf1Pointer = Ch1PacketCnt = Ch1PacketDw = ReadCh1Dword = 0;//���ö�дָ��Ͱ�������Ϊ0
	printk("TY-3012S PciDrv pcifpga_card_buf1: %x!\n", (unsigned int)pcifpga_card_buf1);
	//ΪӦ�û�����2�����ڴ�
	if((pcifpga_card_buf2 = kmalloc(PCIFPGA_BUF_BYTELEN, GFP_DMA | GFP_KERNEL)) == NULL) {//����512K˫��
		free_irq(pcifpga_irq, NULL);
		kfree(MsgBuffer);
		kfree(pcifpga_card_dma1);
		kfree(pcifpga_card_dma2);
		kfree(pcifpga_card_buf0);
		kfree(pcifpga_card_buf1);
		printk("TY-3012S PciDrv kmalloc pcifpga_card_buf2 Err!\n");
		return -ENOMEM;
	}
	memset(pcifpga_card_buf2, 0, PCIFPGA_BUF_BYTELEN);//������2����
	ReadBuf2Pointer = WriteBuf2Pointer = Ch2PacketCnt = Ch2PacketDw = ReadCh2Dword = 0;//���ö�дָ��Ͱ�������Ϊ0
	printk("TY-3012S PciDrv pcifpga_card_buf2: %x!\n", (unsigned int)pcifpga_card_buf2);
	//ȷ��ϵͳIO���߼���ַ�ͷ�Χ
	SysRegaddr = ioremap(SYSREGADDR, SYSREGADDRLEN);//sysreg,1M��ַ�ռ�
	return 0;
}

//�Ƴ�PCI�豸
static void __devexit pcifpga_remove(struct pci_dev *dev)
{
	/* clean up any allocated resources and stuff here.
	 * like call release_region();
	 */
	free_irq(pcifpga_irq, NULL);
	kfree(MsgBuffer);
	kfree(pcifpga_card_dma1);
	kfree(pcifpga_card_dma2);
	kfree(pcifpga_card_buf0);
	kfree(pcifpga_card_buf1);
	kfree(pcifpga_card_buf2);
	iounmap(SysRegaddr);//�ͷ�����������ַsysreg
	return;
}

//�豸ģ����Ϣ
static struct pci_driver pcifpga_driver = {
	.name = CTD_PCI_NAME,		//�豸ģ������
	.id_table = pcifpga_table,	//�����豸��
	.probe = pcifpga_probe,		//���Ҳ���ʼ���豸
	.remove = pcifpga_remove,	//ж���豸ģ��
};

//�豸PCIFPGA��ʼ��ģ��
static int pcifpga_card_init(void)
{
	unsigned int i, j;

	//PCIFPGA���Կ���ʼ��
	iowrite32(0x00000064, ChxBaseAddr0 + 4 * SW1TRIGGERTIME);	//RW01,D7��0����������ʱ��,ȱʡ1us,��С1us,���255us(100us)
	iowrite32(0x00000064, ChxBaseAddr0 + 4 * SW2TRIGGERTIME);	//RW02,D7��0����������ʱ��,ȱʡ1us,��С1us,���255us(100us)
	iowrite32(0x00000003, ChxBaseAddr0 + 4 * SWALARMOUT);		//RW03,D1��0�澯��2��1,ȱʡΪ��,1�պ�,0�Ͽ�(ȱʡ)
	iowrite32(0x00000002, ChxBaseAddr0 + 4 * BEEPALARMOUT);		//RW04,D1��1��һ��(����)�����,D0��1����,ȱʡ����(��һ��)
	iowrite32(0x00000000, ChxBaseAddr0 + 4 * CHINTERFACESEL);	//RW05,D1��0���ͨ��1��2,ȱʡ��,1��ӿ�,0��ӿ�(1��2��)
	iowrite32(0x00000001, ChxBaseAddr0 + 4 * GPSINTERFACESEL);	//RW06,D1��1ѡ1588ʱ��,0ѡIRIGB��,D0��1ƽ��,0��ƽ��(ȱʡ)
	iowrite32(0x000000ff, ChxBaseAddr0 + 4 * WARCHDOGOUT);		//RW07,D7��0��ȫ1���Ź��������1��,Ȼ���Զ����D7��0����
	iowrite32(0x55555555, ChxBaseAddr0 + 4 * LED1DISPLAY);		//RW08,ǰ���LED��ʾ�ĵ�λ˫��,ȫ��ʹ��
	iowrite32(0x55555555, ChxBaseAddr0 + 4 * LED2DISPLAY);		//RW09,ǰ���LED��ʾ�ĵ�λ˫��,����ʹ��
	iowrite32(INTTIMERV,  ChxBaseAddr0 + 4 * INTERRUPTTIMER);	//RW10,��ʱ�ж�ʱ��,D15��0����1��65535ms,ȱʡ0���ж�(2ms)
	iowrite32(0x0000009f, ChxBaseAddr0 + 4 * INTERRUPTENABLE);	//RW11,D7��,D6/5��2/1·FIFO��,D4/3����2/1,D2/1����2/1,0��ʱ
	iowrite32(0x00000000, ChxBaseAddr0 + 4 * IRIGB1588WRSECOND);	//RW12,D31-28��ʮ,D27-24��,D23-20����,D19-16ʮ,D15-12��,D11-8΢��,D7-4ʮ,D3-0��
	iowrite32(0x00000000, ChxBaseAddr0 + 4 * IRIGB1588WRDAY);	//RW13,D25-24���,D23-20ʮ.D19��16��,D13-12ʱʮ,D11-8��,D7-4��ʮ,D3��0��
	iowrite32(0x000000ff, ChxBaseAddr0 + 4 * IRIGB1588TIMEPUT);	//RW14,D7��0��ȫ1,��0x22��0x23��ַ��ʱ�������ʱ��,D7��D0д�������
	iowrite32(0x000000ff, ChxBaseAddr0 + 4 * CH1FIFORESET);		//OW15,D7��0��ȫ1���Ĵ洢FIFO��λ,״̬�ͼ�������λ,���Զ����
	iowrite32(0x000000ff, ChxBaseAddr0 + 4 * CH2FIFORESET);		//OW16,D7��0��ȫ1���Ĵ洢FIFO��λ,״̬�ͼ�������λ,���Զ����
	iowrite32(0x000000ff, ChxBaseAddr0 + 4 * CH1FIFOCOUNT);		//RW17,ȡFIFO����,D29-16Ϊ��[13:0],D13-0Ϊд[13:0],��ǰд0xff,��ʱ1us,�ٶ�
	iowrite32(0x000000ff, ChxBaseAddr0 + 4 * CH2FIFOCOUNT);		//RW18,ȡFIFO����,D29-16Ϊ��[13:0],D13-0Ϊд[13:0],��ǰд0xff,��ʱ1us,�ٶ�
	iowrite32(0x000000ff, ChxBaseAddr0 + 4 * IRIGB1588TIMEGET);	//RW19,D7��0��ȫ1,�ѵ�ǰ��ʱ��ʱ������0x26��0x27��ַ,D7��0д�������
	i = ioread32(ChxBaseAddr0 + 4 * SW1TRIGGERTIME);
	printk("TY-3012S PciDrv SW1TRIGGERTIME:  %08x!\n", i);//01
	i = ioread32(ChxBaseAddr0 + 4 * SW2TRIGGERTIME);
	printk("TY-3012S PciDrv SW2TRIGGERTIME : %08x!\n", i);//02
	i = ioread32(ChxBaseAddr0 + 4 * SWALARMOUT);
	printk("TY-3012S PciDrv SWALARMOUT:      %08x!\n", i);//03
	i = ioread32(ChxBaseAddr0 + 4 * BEEPALARMOUT);
	printk("TY-3012S PciDrv BEEPALARMOUT:    %08x!\n", i);//04
	i = ioread32(ChxBaseAddr0 + 4 * CHINTERFACESEL);
	printk("TY-3012S PciDrv CHINTERFACESEL:  %08x!\n", i);//05
	i = ioread32(ChxBaseAddr0 + 4 * GPSINTERFACESEL);
	printk("TY-3012S PciDrv GPSINTERFACESEL: %08x!\n", i);//06
	i = ioread32(ChxBaseAddr0 + 4 * SW1STATUS);
	printk("TY-3012S PciDrv SW1STATUS:       %08x!\n", i);//07
	i = ioread32(ChxBaseAddr0 + 4 * SW2STATUS);
	printk("TY-3012S PciDrv SW2STATUS:       %08x!\n", i);//08
	i = ioread32(ChxBaseAddr0 + 4 * KEY1STATUS);
	printk("TY-3012S PciDrv KEY1STATUS:      %08x!\n", i);//09
	i = ioread32(ChxBaseAddr0 + 4 * KEY2STATUS);
	printk("TY-3012S PciDrv KEY2STATUS:      %08x!\n", i);//10
	i = ioread32(ChxBaseAddr0 + 4 * LED1DISPLAY);
	printk("TY-3012S PciDrv LED1DISPLAY:     %08x!\n", i);//11
	i = ioread32(ChxBaseAddr0 + 4 * LED2DISPLAY);
	printk("TY-3012S PciDrv LED2DISPLAY:     %08x!\n", i);//12
	i = ioread32(ChxBaseAddr0 + 4 * INTERRUPTTIMER);
	printk("TY-3012S PciDrv INTERRUPTTIMER:  %08x!\n", i);//13
	i = ioread32(ChxBaseAddr0 + 4 * INTERRUPTENABLE);
	printk("TY-3012S PciDrv INTERRUPTENABLE: %08x!\n", i);//14
	i = ioread32(ChxBaseAddr0 + 4 * INTERRUPTSTATUS);
	printk("TY-3012S PciDrv INTERRUPTSTATUS: %08x!\n", i);//15
	i = ioread32(ChxBaseAddr0 + 4 * IRIGB1588STATUS);
	printk("TY-3012S PciDrv IRIGB1588STATUS: %08x!\n", i);//16
	i = ioread32(ChxBaseAddr0 + 4 * IRIGB1588TIMEERR);
	printk("TY-3012S PciDrv IRIGB1588ERR:    %08x!\n", i);//17
	i = ioread32(ChxBaseAddr0 + 4 * IRIGB1588RDSECOND);
	printk("TY-3012S PciDrv IRIGB1588RDS:    %08x!\n", i);//18
	i = ioread32(ChxBaseAddr0 + 4 * IRIGB1588RDDAY);
	printk("TY-3012S PciDrv IRIGB1588RDDAY:  %08x!\n", i);//19
	i = ioread32(ChxBaseAddr0 + 4 * CH1FIFOSTATUS);
	printk("TY-3012S PciDrv CH1FIFOSTATUS:   %08x!\n", i);//20
	i = ioread32(ChxBaseAddr0 + 4 * CH2FIFOSTATUS);
	printk("TY-3012S PciDrv CH2FIFOSTATUS:   %08x!\n", i);//21
	i = ioread32(ChxBaseAddr0 + 4 * CH1FIFOCOUNT);
	printk("TY-3012S PciDrv CH1FIFOCOUNT:    %08x!\n", i);//22
	i = ioread32(ChxBaseAddr0 + 4 * CH2FIFOCOUNT);
	printk("TY-3012S PciDrv CH2FIFOCOUNT:    %08x!\n", i);//23
	//���ͨ��1�Ĺ��˼Ĵ���д
	iowrite32(0x00000001, Ch1BaseAddr1 + 4 * CHFILTERMODE);		//RW,D0=0Fileter,=1NoFilerer,D1=0ReceiveAllBroadcast,=1RejiectAllBroadcast
	iowrite32(0x0A5A5A5A5,Ch1BaseAddr1 + 4 * CHFILTERRESET);	//RW,д0xA5A5A5A5��λ���Ĵ���ģ���������ģ��,������֮����1��
	iowrite32(0x00000040, Ch1BaseAddr1 + 4 * CHFILTERMINLEN);	//RW,���֡��,D15��0д��MinimumEthernetPacket,ȱʡ64
	iowrite32(0x00000600, Ch1BaseAddr1 + 4 * CHFILTERMAXLEN);	//RW,�֡��,D15��0д��MaximumEthernetPacket,ȱʡ1536
	iowrite32(0x000000c0, Ch1BaseAddr1 + 4 * CHFILTERENABLE);	//RW,1=e,0=d,D0��9:VLAN1,VLAN2,GOOSE,SMV,MMS,����,MACԴ,VLAN����,MACԴ������,VLAN������
	iowrite32(0x00000555, Ch1BaseAddr1 + 4 * CHFILTERVID1);		//RW,D11��0,VlanID1
	iowrite32(0x00000aaa, Ch1BaseAddr1 + 4 * CHFILTERVID2);		//RW,D11��0,VlanID1
	iowrite32(0x0000ffff, Ch1BaseAddr1 + 4 * CHFILTERMACEN);	//RW,D31��0,SourceMacAdd,31��00bit
	//for(i = 0; i < 16; i++) {
		iowrite32(0x00001122+0, Ch1BaseAddr1 + 4*(CHFILTERMACH+0));	//RW,D15��0,SourceMacAdd,47��32bit
		iowrite32(0x33445500+0, Ch1BaseAddr1 + 4*(CHFILTERMACL+0));	//RW,D31��0,SourceMacAdd,31��00bit
		iowrite32(0x00001122+0, Ch1BaseAddr1 + 4*(CHFILTERMACH+2));	//RW,D15��0,SourceMacAdd,47��32bit
		iowrite32(0x33445501+0, Ch1BaseAddr1 + 4*(CHFILTERMACL+2));	//RW,D31��0,SourceMacAdd,31��00bit
		iowrite32(0x00001122+0, Ch1BaseAddr1 + 4*(CHFILTERMACH+4));	//RW,D15��0,SourceMacAdd,47��32bit
		iowrite32(0x33445502+0, Ch1BaseAddr1 + 4*(CHFILTERMACL+4));	//RW,D31��0,SourceMacAdd,31��00bit
		iowrite32(0x00001122+0, Ch1BaseAddr1 + 4*(CHFILTERMACH+6));	//RW,D15��0,SourceMacAdd,47��32bit
		iowrite32(0x33445503+0, Ch1BaseAddr1 + 4*(CHFILTERMACL+6));	//RW,D31��0,SourceMacAdd,31��00bit
	//}
	//���ͨ��2�Ĺ��˼Ĵ���д
	iowrite32(0x00000001, Ch2BaseAddr2 + 4 * CHFILTERMODE);		//RW,D0=0Fileter,=1NoFilerer,D1=0ReceiveAllBroadcast,=1RejiectAllBroadcast
	iowrite32(0x0A5A5A5A5,Ch2BaseAddr2 + 4 * CHFILTERRESET);	//RW,д0xA5A5A5A5��λ���Ĵ���ģ���������ģ��,������֮����1��
	iowrite32(0x00000040, Ch2BaseAddr2 + 4 * CHFILTERMINLEN);	//RW,���֡��,D15��0д��MinimumEthernetPacket,ȱʡ64
	iowrite32(0x00000600, Ch2BaseAddr2 + 4 * CHFILTERMAXLEN);	//RW,�֡��,D15��0д��MaximumEthernetPacket,ȱʡ1536
	iowrite32(0x000000c0, Ch2BaseAddr2 + 4 * CHFILTERENABLE);	//RW,1=e,0=d,D0��9:VLAN1,VLAN2,GOOSE,SMV,MMS,����,MACԴ,VLAN����,MACԴ������,VLAN������
	iowrite32(0x00000555, Ch2BaseAddr2 + 4 * CHFILTERVID1);		//RW,D11��0,VlanID1
	iowrite32(0x00000aaa, Ch2BaseAddr2 + 4 * CHFILTERVID2);		//RW,D11��0,VlanID1
	iowrite32(0x0000ffff, Ch2BaseAddr2 + 4 * CHFILTERMACEN);	//RW,D31��0,SourceMacAdd,31��00bit
	//for(i = 0; i < 16; i++) {
		iowrite32(0x00006677+0, Ch2BaseAddr2 + 4*(CHFILTERMACH+0));	//RW,D15��0,SourceMacAdd,47��32bit
		iowrite32(0x8899aa00+0, Ch2BaseAddr2 + 4*(CHFILTERMACL+0));	//RW,D31��0,SourceMacAdd,31��00bit
		iowrite32(0x00006677+0, Ch2BaseAddr2 + 4*(CHFILTERMACH+2));	//RW,D15��0,SourceMacAdd,47��32bit
		iowrite32(0x8899aa01+0, Ch2BaseAddr2 + 4*(CHFILTERMACL+2));	//RW,D31��0,SourceMacAdd,31��00bit
		iowrite32(0x00006677+0, Ch2BaseAddr2 + 4*(CHFILTERMACH+4));	//RW,D15��0,SourceMacAdd,47��32bit
		iowrite32(0x8899aa02+0, Ch2BaseAddr2 + 4*(CHFILTERMACL+4));	//RW,D31��0,SourceMacAdd,31��00bit
		iowrite32(0x00006677+0, Ch2BaseAddr2 + 4*(CHFILTERMACH+6));	//RW,D15��0,SourceMacAdd,47��32bit
		iowrite32(0x8899aa03+0, Ch2BaseAddr2 + 4*(CHFILTERMACL+6));	//RW,D31��0,SourceMacAdd,31��00bit
	//}
	//���ͨ��1�Ĺ��˼Ĵ�����
	i = ioread32(Ch1BaseAddr1 + 4 * CHFILTERMODE);
	printk("TY-3012S PciDrv CH1FILTERMODE:   %08x!\n", i);
	i = ioread32(Ch1BaseAddr1 + 4 * CHFILTERRESET);
	printk("TY-3012S PciDrv CH1FILTERRESET:  %08x!\n", i);
	i = ioread32(Ch1BaseAddr1 + 4 * CHFILTERMINLEN);
	printk("TY-3012S PciDrv CH1FILTERMINLEN: %08x!\n", i);
	i = ioread32(Ch1BaseAddr1 + 4 * CHFILTERMAXLEN);
	printk("TY-3012S PciDrv CH1FILTERMAXLEN: %08x!\n", i);
	i = ioread32(Ch1BaseAddr1 + 4 * CHFILTERENABLE);
	printk("TY-3012S PciDrv CH1FILTERENABLE: %08x!\n", i);
	i = ioread32(Ch1BaseAddr1 + 4 * CHFILTERVID1);
	printk("TY-3012S PciDrv CH1FILTERVID1:   %08x!\n", i);
	i = ioread32(Ch1BaseAddr1 + 4 * CHFILTERVID2);
	printk("TY-3012S PciDrv CH1FILTERVID2:   %08x!\n", i);
	i = ioread32(Ch1BaseAddr1 + 4 * CHFILTERMACEN);
	printk("TY-3012S PciDrv CH1FILTERMACEN:  %08x!\n", i);
	//for(i = 0; i < 16; i++) {
		j = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACH+0));
		printk("TY-3012S PciDrv CH1FILTERMAC%02xH: %08x!\n", 0, j);
		j = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACL+0));
		printk("TY-3012S PciDrv CH1FILTERMAC%02xL: %08x!\n", 0, j);
		j = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACH+2));
		printk("TY-3012S PciDrv CH1FILTERMAC%02xH: %08x!\n", 1, j);
		j = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACL+2));
		printk("TY-3012S PciDrv CH1FILTERMAC%02xL: %08x!\n", 1, j);
		j = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACH+4));
		printk("TY-3012S PciDrv CH1FILTERMAC%02xH: %08x!\n", 2, j);
		j = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACL+4));
		printk("TY-3012S PciDrv CH1FILTERMAC%02xL: %08x!\n", 2, j);
		j = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACH+6));
		printk("TY-3012S PciDrv CH1FILTERMAC%02xH: %08x!\n", 3, j);
		j = ioread32(Ch1BaseAddr1 + 4*(CHFILTERMACL+6));
		printk("TY-3012S PciDrv CH1FILTERMAC%02xL: %08x!\n", 3, j);
	//}
	//���ͨ��2�Ĺ��˼Ĵ�����
	i = ioread32(Ch2BaseAddr2 + 4 * CHFILTERMODE);
	printk("TY-3012S PciDrv CH2FILTERMODE:   %08x!\n", i);
	i = ioread32(Ch2BaseAddr2 + 4 * CHFILTERRESET);
	printk("TY-3012S PciDrv CH2FILTERRESET:  %08x!\n", i);
	i = ioread32(Ch2BaseAddr2 + 4 * CHFILTERMINLEN);
	printk("TY-3012S PciDrv CH2FILTERMINLEN: %08x!\n", i);
	i = ioread32(Ch2BaseAddr2 + 4 * CHFILTERMAXLEN);
	printk("TY-3012S PciDrv CH2FILTERMAXLEN: %08x!\n", i);
	i = ioread32(Ch2BaseAddr2 + 4 * CHFILTERENABLE);
	printk("TY-3012S PciDrv CH2FILTERENABLE: %08x!\n", i);
	i = ioread32(Ch2BaseAddr2 + 4 * CHFILTERVID1);
	printk("TY-3012S PciDrv CH2FILTERVID1:   %08x!\n", i);
	i = ioread32(Ch2BaseAddr2 + 4 * CHFILTERVID2);
	printk("TY-3012S PciDrv CH2FILTERVID2:   %08x!\n", i);
	i = ioread32(Ch2BaseAddr2 + 4 * CHFILTERMACEN);
	printk("TY-3012S PciDrv CH2FILTERMACEN:  %08x!\n", i);
	//for(i = 0; i < 16; i++) {
		j = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACH+0));
		printk("TY-3012S PciDrv CH2FILTERMAC%02xH: %08x!\n", 0, j);
		j = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACL+0));
		printk("TY-3012S PciDrv CH2FILTERMAC%02xL: %08x!\n", 0, j);
		j = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACH+2));
		printk("TY-3012S PciDrv CH2FILTERMAC%02xH: %08x!\n", 1, j);
		j = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACL+2));
		printk("TY-3012S PciDrv CH2FILTERMAC%02xL: %08x!\n", 1, j);
		j = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACH+4));
		printk("TY-3012S PciDrv CH2FILTERMAC%02xH: %08x!\n", 2, j);
		j = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACL+4));
		printk("TY-3012S PciDrv CH2FILTERMAC%02xL: %08x!\n", 2, j);
		j = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACH+6));
		printk("TY-3012S PciDrv CH2FILTERMAC%02xH: %08x!\n", 3, j);
		j = ioread32(Ch2BaseAddr2 + 4*(CHFILTERMACL+6));
		printk("TY-3012S PciDrv CH2FILTERMAC%02xL: %08x!\n", 3, j);
	//}
	return 0;
}

/* ��ʼ����ע��cdev */
static void pcifpga_setup_cdev(struct pcifpga_dev *dev, int index)
{
	int err, devno = MKDEV(PCIFPGA_MAJOR, index);

	cdev_init(&dev->cdev, &pcifpga_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &pcifpga_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err) printk("TY-3012S pcifpga_setup_cdev err %d adding devno %d!\n", err, index);
}

//ע��PCIӲ����������
static int __init pcifpga_init(void)
{
	int status, result;
	dev_t devno;

	printk("TY-3012S PciDrv Ver1.0 Copyright!\n");//ע��PCIFPGAӲ����������ʼ
	//ע��CHAR��д����
	devno = MKDEV(PCIFPGA_MAJOR, 0);
	result = register_chrdev_region(devno, 1, "pcifpga");
	if (result < 0) {
		printk("TY-3012S PciDrv register_chrdev_region fail: %d!\n", PCIFPGA_MAJOR);
		return result;
	}
	printk("TY-3012S register_chrdev_region success: %d!\n", PCIFPGA_MAJOR);
	pcifpga_devp = kmalloc(sizeof(struct pcifpga_dev), GFP_KERNEL);//��̬�����豸�ṹ����ڴ�
	if (!pcifpga_devp) {//����ʧ��
		printk("TY-3012S PciDrv kmalloc pcifpga_dev fail!\n");
		result = -ENOMEM;
		unregister_chrdev_region(devno, 1);
		return result;
	}
	memset(pcifpga_devp, 0, sizeof(struct pcifpga_dev));
	pcifpga_setup_cdev(pcifpga_devp, 0);
	printk("TY-3012S PciDrv kmalloc pcifpga_dev success!\n");//Setup pcifpga cdev.
	//���ע��CHAR
	/* ע��PCIFPGAӲ���������� */
	printk("TY-3012S PciDrv Ver1.0 Init!\n");//ע��PCIFPGAӲ����������ʼ
	status = pci_register_driver(&pcifpga_driver);//ע����������
	printk("TY-3012S PciDrv pci_register_driver: %x!\n", status);
	if(status) {
		printk("TY-3012S PciDrv pci_register_driver Err!\n");
		pci_unregister_driver(&pcifpga_driver);
		kfree(pcifpga_devp);//�����ַ�ע��
		unregister_chrdev_region(devno, 1);//�����ַ�ע��
		return -ENODEV;
	}
	printk("TY-3012S PciDrv is loaded successfully!\n");//ע����������ɹ�
	mdelay(10);//��ʱ10����
	printk ("TY-3012S PciDrv m_second: %d!\n", m_second);
	pcifpga_card_init();
	return 0;
	/* ... */
	return pci_register_driver(&pcifpga_driver);
}

static void __exit pcifpga_exit(void)
{
	cdev_del(&pcifpga_devp->cdev);/* ע��cdev */
	kfree(pcifpga_devp);/* �ͷ��豸�ṹ���ڴ� */
	unregister_chrdev_region(MKDEV(PCIFPGA_MAJOR, 0), 1);/* �ͷ��豸�� */
	printk("TY-3012S PciDrv Ver1.0 Exit!\n");
	pci_unregister_driver(&pcifpga_driver);
}

MODULE_AUTHOR("shenxj <shenxj@datanggroup.cn>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Pci Driver for TY-3012S");
module_init(pcifpga_init);
module_exit(pcifpga_exit);
