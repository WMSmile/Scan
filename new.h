
///////////////////////////////////////////////////////////////////////////////////////////
//These defines & structure definitions are taken from the
//"ipexport.h" header file as provided with the Platform SDK.
//Including them here allows you to compile the code without
//the need to have the full Platform SDK installed.

typedef ULONG IPAddr;       // IP��ַ��An IP address.

typedef struct ip_option_information {
    UCHAR   Ttl;                // Time To Live ����ʱ��
    UCHAR   Tos;                // Type Of Service ��������
    UCHAR   Flags;              // IP header flags ipͷ��־
                                //IP_REVERSE(ip���ݰ������ip·��)
                                //IP_FLAG_DF(�������ݰ���ѹ)
    UCHAR   OptionsSize;        // Size in bytes of options data ���ݴ�С
    UCHAR * OptionsData;        // Pointer to options data ����ָ��
} IP_OPTION_INFORMATION, *PIP_OPTION_INFORMATION;

typedef struct icmp_echo_reply {
    IPAddr  Address;            // Replying address ���ڻظ���IP��ַ
    ULONG   Status;             // Reply IP_STATUS �ظ���״̬
    ULONG   RoundTripTime;      // RTT in milliseconds(����ʱ��)
    USHORT  DataSize;           // Reply data size in bytes �ظ����ݴ�С
    USHORT  Reserved;           // Reserved for system use ����
    PVOID   Data;               // Pointer to the reply data �ָ����ݵ�ָ��
    struct ip_option_information Options; // Reply options �ظ�ѡ��
} ICMP_ECHO_REPLY, *PICMP_ECHO_REPLY;

//��������򿪸�ICMP Echo ������ʹ�õľ��;
typedef HANDLE (WINAPI *lpIcmpCreateFile)(VOID);
//��������ر���IcmpCreateFile �򿪵ľ��;
typedef BOOL (WINAPI *lpIcmpCloseHandle)(HANDLE IcmpHandle);
//�����������Echo ���󲢵ȴ��ظ���ʱ��
typedef DWORD (WINAPI *lpIcmpSendEcho)(HANDLE IcmpHandle,  // IcmpCreateFile �򿪵ľ��
									   IPAddr DestinationAddress, //Echo�����Ŀ�ĵ�ַ
                                       LPVOID RequestData, //��������buffer
									   WORD RequestSize, //�������ݳ���
                                       PIP_OPTION_INFORMATION RequestOptions, // IP_OPTION_INFORMATION ָ��
                                       LPVOID ReplyBuffer,  //���ջظ�buffer
									   DWORD ReplySize,   //���ջظ�buffer��С
									   DWORD Timeout //�ȴ���ʱ
									   );

///////////////////////////////////////////////////////////////////////////////////////////

const int DEF_MAX_HOP = 30;		//�����վ��
const int DATA_SIZE = 32;		//ICMP�������ֶδ�С
const DWORD TIMEOUT= 3000;		//��ʱʱ�䣬��λms
const int MAX_PING=4;            //ping�Ĵ���

#pragma comment (lib,"Ws2_32.lib")