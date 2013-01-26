#ifndef	__DEBUG_CONSOLE_H__
#define	__DEBUG_CONSOLE_H__

typedef struct
{
	long mtype;			/*��Ϣ����*/
	int   request;			/*����*/
	struct msqid_ds msgInfo;	/*��ѯ����״̬����*/
	unsigned long cnt;			/*�ܰ���*/

}MsgDebugConsole;

#define	TYPE_DEBUGCONSOLE_CMD			4		/*telnet���Զ�����Ϣ����*/
#define	TYPE_DEBUGCONSOLE_RSP			5		/*��Ӧtelnet����������*/

							/*�����̲߳��Ӵ�ӡ,�ú�Ϊ��*/
#define	DEBUG_CMD_QUERY_DMMSG			1		/*���̹�����Ϣ����״̬��ѯ*/
#define	DEBUG_CMD_QUERY_APPMSG			2		/*APP������Ϣ����״̬��ѯ*/
#define	DEBUG_CMD_QUERY_PLHMSG			3		/*�澯������Ϣ����״̬��ѯ*/
#define	DEBUG_CMD_PACKETCNT				4		/*��ѯ��ǰ���հ����ܺ�*/

#endif
