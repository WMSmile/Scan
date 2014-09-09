
///////////////////////////////////////////////////////////////////////////////////////////
//These defines & structure definitions are taken from the
//"ipexport.h" header file as provided with the Platform SDK.
//Including them here allows you to compile the code without
//the need to have the full Platform SDK installed.

typedef ULONG IPAddr;       // IP地址，An IP address.

typedef struct ip_option_information {
    UCHAR   Ttl;                // Time To Live 生存时间
    UCHAR   Tos;                // Type Of Service 服务类型
    UCHAR   Flags;              // IP header flags ip头标志
                                //IP_REVERSE(ip数据包中添加ip路由)
                                //IP_FLAG_DF(不将数据包解压)
    UCHAR   OptionsSize;        // Size in bytes of options data 数据大小
    UCHAR * OptionsData;        // Pointer to options data 数据指针
} IP_OPTION_INFORMATION, *PIP_OPTION_INFORMATION;

typedef struct icmp_echo_reply {
    IPAddr  Address;            // Replying address 正在回复的IP地址
    ULONG   Status;             // Reply IP_STATUS 回复的状态
    ULONG   RoundTripTime;      // RTT in milliseconds(往返时间)
    USHORT  DataSize;           // Reply data size in bytes 回复数据大小
    USHORT  Reserved;           // Reserved for system use 保留
    PVOID   Data;               // Pointer to the reply data 恢复数据的指针
    struct ip_option_information Options; // Reply options 回复选项
} ICMP_ECHO_REPLY, *PICMP_ECHO_REPLY;

//这个函数打开个ICMP Echo 请求能使用的句柄;
typedef HANDLE (WINAPI *lpIcmpCreateFile)(VOID);
//这个函数关闭由IcmpCreateFile 打开的句柄;
typedef BOOL (WINAPI *lpIcmpCloseHandle)(HANDLE IcmpHandle);
//这个函数发送Echo 请求并等待回复或超时。
typedef DWORD (WINAPI *lpIcmpSendEcho)(HANDLE IcmpHandle,  // IcmpCreateFile 打开的句柄
									   IPAddr DestinationAddress, //Echo请求的目的地址
                                       LPVOID RequestData, //发送数据buffer
									   WORD RequestSize, //发送数据长度
                                       PIP_OPTION_INFORMATION RequestOptions, // IP_OPTION_INFORMATION 指针
                                       LPVOID ReplyBuffer,  //接收回复buffer
									   DWORD ReplySize,   //接收回复buffer大小
									   DWORD Timeout //等待超时
									   );

///////////////////////////////////////////////////////////////////////////////////////////

const int DEF_MAX_HOP = 30;		//最大跳站数
const int DATA_SIZE = 32;		//ICMP包数据字段大小
const DWORD TIMEOUT= 3000;		//超时时间，单位ms
const int MAX_PING=4;            //ping的次数

#pragma comment (lib,"Ws2_32.lib")