////////////////////////////////////////////////////////////////////////////////////////
//�� ��:slgdev.dll
//����:����˸�dsp�˵�ҵ���߼�ͨ�Žӿڣ�Ϊ������ṩ���������ϴ����ع��ܣ�
// ͬʱ����dsp���û������Լ���תģʽ�¸�����ת����
//˵��::
//��ʹ�ñ�������У���������������
//1. �ϴ��û���Ƭʱ����Ƭ�ڴ��С������60KB����
//2. �û������ϴ����ع����У��벻Ҫ��ջ�Ϸ������150kb���ڴ�
//3. �����������кŲ���Ϊ0��0����ڲ�ռ��
////////////////////////////////////////////////////////////////////////////////////////
//14:15 2016-9-20
//1. IPv6 DNS
// ֧��ͨ�� ipv4 ipv6 ���� �����豸��ַ�����豸��
// ����:
//  SLG_CNDev("192.168.1.150""); //ipv4
//  SLG_CNDev("2001::51d6:120a:e9f4:d0ce"); //ipv6
//  SLG_CNDev("mr-bubblehead-2.example.com"); //hostname
// ���� cszDevAddr ��������ʹ�����Ϸ�ʽ��
//2. TLS��rootca
// ֧��ssl, ��DevConnectStatus �ص�������Ӧ DEV_QUERY_ENCRYPT ʱ
// ����0   don't encrypt
// ����1   need encrypt and it is server-side
// ����2   need encrypt and it is client-side
// ����3   need encrypt and it is client-side �����и�֤����֤����Ѹ�֤��� "C:\\rootca"(SSL_ROOTCA_PATH) Ŀ¼��
// ���Ӵ���:
//  int SLGDEV_CALL DevConnectNotify (const char* dev_addr, DEV_CONNECT_STATUS flag, void* pvContext) {
// ....
//  switch (flag) {
// ....
//  case DEV_QUERY_ENCRYPT: return 3; //����Ӧ�ó�����Ҫ���ز�ͬ��ֵ
// ....
// }
//  return 0;
// }
//3. �ϴ�֤��: ͨ������ SLG_ULUpdate ����
//3. HID card number and facility code
//  card number:facility code , Example 9001:99
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Name:slgdev.dll
//Function::the logical communication interface between administrative end and DSP end, it is used to provide the data upload and download function for the administrative end, and to control the DSP end user operation, as well as all kinds of transfers in the repeater mode operation.
//Comment:s:
//Limitations
//1. The size of each user picture is limited less than 60KB when uploading it.
//2. During the user data uploading and downloading, the internal storage distributed in stack cannot over 150KB.
//3. All commands serial number cannot be 0, 0 has been internal used.
////////////////////////////////////////////////////////////////////////////////////////
//14:15 2016-9-20
//1. IPv6 DNS
//  It supports three device addresses: ipv4, ipv6 and domain name to connect devices.
//  Examples:
//  SLG_CNDev("192.168.1.150""); //ipv4
//  SLG_CNDev("2001::51d6:120a:e9f4:d0ce"); //ipv6
//  SLG_CNDev("mr-bubblehead-2.example.com"); //hostname
//  All cszDevAddr parameters can work as the way above.
//2. TLS��rootca
//  It supports ssl; when callback function DevConnectStatus responses to DEV_QUERY_ENCRYPT
//  return to 0   don't encrypt
//  return to 1   need encrypt and it is server-side
//  return to 2   need encrypt and it is client-side
//  return to 3   need encrypt and it is client-side and processing root certificate validation (need to put the root certificate in the catalogue "C:\\rootca"(SSL_ROOTCA_PATH))
//  example codes:
//  int SLGDEV_CALL DevConnectNotify (const char* dev_addr, DEV_CONNECT_STATUS flag, void* pvContext) {
// ....
//  switch (flag) {
// ....
//  case DEV_QUERY_ENCRYPT: return 3; //Based on the need of the applications to return to different values
// ....
// }
//  return 0;
// }
//3. Upload certificate: by calling SLG_ULUpdate function
//3. HID card number and facility code
//  card number:facility code , Example 9001:99
////////////////////////////////////////////////////////////////////////////////////////
#ifndef __SLGDEV_H__
#define __SLGDEV_H__
#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif
EXTERN_C_BEGIN
//#define WINCE
#define SDK_VERSION  "1.4"
///////////////////////////////////////////////////////////////////////////////////////
//  config
///////////////////////////////////////////////////////////////////////////////////////
// for WINCE
#ifdef WINCE
#define WIN32
#endif
///////////////////////////////////////////////////////////////////////////////////////
//��֤��·��
#define SSL_ROOTCA_PATH  "C:\\rootca"
///////////////////////////////////////////////////////////////////////////////////////
// for openssl
//#define USE_SSL
///////////////////////////////////////////////////////////////////////////////////////
// macro
#ifdef _WIN32
// for windows
#define STDCALL_CALL  __stdcall
#define CDECL_CALL __cdecl
#define SLGDEV_CALL  __stdcall
typedef unsigned long /**/(__stdcall* thread_cb)(void* pParam);
#include "winsock2.h"
#ifndef _WINSOCK2API_
typedef unsigned int SOCKET;
#endif
#else
// for linux
#define SLGDEV_CALL
#define STDCALL_CALL
#define CDECL_CALL
#define GetLastError() errno
#define WINAPI
#define _snprintf snprintf
#define _vsnprintf vsnprintf
typedef int SOCKET; //linux
typedef void* (*thread_cb)(void* pParam);
typedef long LONG;
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef unsigned char UCHAR;
#endif
///////////////////////////////////////////////////////////////////////////////////////
// BOOL
#ifndef BOOL
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
///////////////////////////////////////////////////////////////////////////////////////
// net struct
#define IP_LEN 16
#define IP_LEN64 64
#define URL_LEN 127
typedef struct
{
    unsigned short family;
    unsigned char data[26];
} addr_in;
typedef struct
{
    unsigned short Port;                 //�˿�
    char           IP_Address[IP_LEN64]; //���ʮ����IP��ַ
} SOCK_ADDRESS, *PSOCK_ADDRESS;
typedef SOCK_ADDRESS NET_ADDRESS;
typedef SOCK_ADDRESS PEERADR;
struct CSockBase;
typedef void (SLGDEV_CALL* ListenCallBack)(SOCKET hSOCK, const addr_in* Addr, void* pvContext, struct CSockBase* user);
typedef void (SLGDEV_CALL* DataRecvCallBack)(SOCKET hSOCK, const char* pbData, int nLen, void* pvContext, void* user);
typedef void (SLGDEV_CALL* DataRecvFromCallBack)(SOCKET hSOCK, const char* pbData, int nLen, const addr_in* addr, void* pvContext, void* user);
///////////////////////////////////////////////////////////////////////////////////////
#define F_APP_PACKAEG  0
#define F_IMG_STANDBY  1
#define F_PSD_CERTIFI  2
///////////////////////////////////////////////////////////////////////////////////////
#define DEV_REGION_ADDR       ("224.0.1.100") //Ĭ�������ַ,�û��豸�Զ��Զ����� //The default multicasting address,automatic search  
#define DEV_ID_LEN            24              //ID����//ID length
#define DEV_TIMEGROUP_NUMS    8               //ʱ������� //number of the timegroup
#define DEV_USER_COMBINS      6               //�û���ϸ��� //number of the user group
#define DL_DEV_PIC            0x01            //��Ƭ//picture
#define DL_DEV_USER_FEAT      0x02            //��������//facial features
#define DL_DEV_USER_TEXT      0x04            //�û�������Ϣ//texts
#define DEV_CHECK_TIME        0x01            //���ʱ��//Testing time
#define DEV_CHECK_WEEK        0x02            //�������//Testing week
#define DEV_CHECK_DAY         0x04            //�������//Testing date
#define DEV_CHECK_MONTH       0x08            //����·�//Testing month
#define DEV_CHECK_YEAR        0x10            //������//Testing year
#define DEV_WEEK_1            0x01            //����һ��Ч//Monday is effective 
#define DEV_WEEK_2            0x02            //���ڶ���Ч//Tuesday is effective
#define DEV_WEEK_3            0x04            //��������Ч//Wendesday is effective
#define DEV_WEEK_4            0x08            //��������Ч//Thursday is effective
#define DEV_WEEK_5            0x10            //��������Ч//Friday is effective
#define DEV_WEEK_6            0x20            //��������Ч//Saturday is effective
#define DEV_WEEK_7            0x40            //��������Ч//Sunday is effective
#define DEV_RECORD_SAVEFAIL   0x00000001      //�Ƿ񱣴�ʶ��ʧ�ܼ�¼//If to save the record of recognition failure
#define DEV_SUPER_PASSWORD    0x00000002      //�������뿪����Ч//Super password to open door is effective
#define DEV_HDBEEP_OPEN       0x00000004      //����/����//Deployment/Disarming
#define DEV_REALTIME_RECORD   0x00000010      //ʵʱ��̬��¼����//Real time dynamic record displ
#define DEV_REALTIME_USERLOAD 0x00000020
//�����Ż��߿���ʶ��ʱ������޷��ڵ�ǰdsp�л����������,����������״̬�£����ӹ����ʵʱ������������
//Input serial number or card number for identification, if failure to gain the facial features from local DSP, it will load in real-time from the administrative end when its network is connecting.
#define DEV_REALTIME_USERSEND       0x00000040 //ͨ���豸�ɼ����û�ʵʱ���͵������ն�//Collect users for real time transmiting to administrative end
#define DEV_DOORMANGET_OPEN         0x00000080 //�����Ŵż�ⱨ��//Door magnet detection alarm
#define DEV_DOORFORCE_OPEN          0x00000100 //����в�ȿ��Ź���//Door is forced to open
#define DEV_REMOTE_CAP_SAVE         0x00000200 //Զ�̲ɼ����û��Ƿ񱣴浽DSP//If the remote users collection from the administrative end save to the local DSP
#define DEV_GPRS_OPEN               0x00000400 //����GPRSģ��//Open GPRS module
#define DEV_UPDATE_USERSEND         0x00000800 //��������ʱ���Ƿ�ʵʱ�����û�����//If the facial features update, to send users�� features in real time or notIf the facial features update, to send users�� features in real time or not
#define DEV_REALTIME_USERLOAD_PRIOR 0x00001000 //������������£������������ //Network loading is priority once the  network is connecting.
#define DEV_AB_LOCK                 0x00002000 //AB����//AB interlocking 
#define DEV_DOOR1_NOPEN             0x00004000 //��һN+1//Door one N+1
#define DEV_DOOR2_NOPEN             0x00008000 //�Ŷ�N+1//Door two N+1
#define DEV_ENABLE_COUNT            0x00010000 //���߼�¼��ͳ��//Online records statistics
#define DEV_ENCRYP_DISPALY          0x00020000 //�Ƿ���û���Ϣ���м�����ʾ//If display user information via encryption
#define DEV_ARMED_ENABLE_AWAYS      0x00400000 //��������ʱ���������� //Trigger alarm, alarm lasting
#define DEV_OPENSSL_ENABLE          0x00800000 //ssl����//ssl encryption
#define DEV_WAKE_INPUT_ENABLED      0x20000000 //���������������ܼ�����֤//Activate verification only by auxiliary input
//��֤ģʽ���ȼ���-��:UID->CARD->11->MIX->1N
//Verification mode priority low-high: UID->CARD->11->MIX->1N
#define DEV_VERIFY_USERID           0x01       //����//Work number
#define DEV_VERIFY_CARD             0x02       //ˢ��//Swipe card
#define DEV_VERIFY_FACE_11          0x04       //11��������//11 facial features loading
#define DEV_VERIFY_FACE_MIX         0x08       //�����������//Mixed facial features loading
#define DEV_VERIFY_FACE_1N          0x10       //1:N�������� //1:N facial features loading
#define DEV_VERIFY_FINGER           0x20       //ָ����֤
#define DEV_VERIFY_PLUSALL          0x40       //1:1���ʱ������֤//1:1 Mixed facial identification
#define DEV_VERIFY_REMOTE           0x80    //Զ�̼��ط�ʽ
//�û�״̬��Ϣ//User status information
#define DEV_USER_CARD_INVALID       0x01       //����ʧ //Card reporting loss
#define DEV_USER_DLine_INVALID      0x02       //ʧЧʱ����Ч//Time of expiry
#define DEV_USER_BLACK              0x04       //������//Blacklist
#define DEV_USER_MIX1N              0x08       //���ģʽ��1N�û�//Mixed Facial identification mode, 1:N user
#define DEV_USER_VIPCARD            0x10       //VIP�û�//VIP user
#define DEV_USER_FINGER_PRINT       0x20    //ָ����֤
#define DEV_USER_SYNC_FINGEER       0x40    //ָ��ͬ��
#define DEV_USER_MAX_MID            0x40    //���ó����е���ֵ
#define DEV_USER_MAX_LOW            0x80    //���ó����ϸ���ֵ
//SDK�Զ���
#define DEV_USER_FEAT_BASE64        0x40       //��������base64����//Used base64 to code the features
#define DEV_USER_IMAGE_BASE64       0x80       //ע���վ���base64����//Used base64 to code the registered pictures
///////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    DEV_CONNECT_CUT = 0x00,                  //�Ͽ�����//Disconnecting
    DEV_CONNECT_SUCCESS,   //��������//Connection successful
    DEV_CONNECT_FAILUE,    //����ʧ��// Connection failed
    DEV_CONNECT_NONE,      //δ��������// Unconnected
    DEV_QUERY_ENCRYPT,     //���豸�Ƿ���м��ܴ���// If the terminal use the encryption transmission
    //0 : don't encrypt
    //1 : need encrypt and it is server-side
    //2 : need encrypt and it is client-side
    //3 : need encrypt and it is client-side and processing root certificate validation (need to put the root certificate in the catalogue "C:\\rootca"(SSL_ROOTCA_PATH))
    SRV_QUERY_ENCRYPT,     //����ת�������Ƿ���м��ܴ���// If the transit server use the encryption transmission
    DEV_VERIFYCERT_FAILUE, //֤����֤ʧ��// Certificate validation fail
} DEV_CONNECT_STATUS;

typedef enum
{
    DEV_OPER_UPLOAD   = 0x01, //�ϴ�// Upload
    DEV_OPER_DOWNLOAD = 0x02, //����// Download
    DEV_OPER_DELETE   = 0x04, //ɾ��//Delete
} DEV_OPER_FLAG;

typedef enum
{
    DEV_AUTH_OPER = 0x01,                  //�豸��֤����// Device Verification
    DEV_AUTH_SET_OPER,     //�豸��֤�û����ò���// Device Verification of user setup operation
    DEV_REGION_OPER,       //�豸�������// Device region operation
    DEV_FORMAT_OPER,       //�豸��ʽ������// Device formatting operation
    DEV_UPDATE_OPER,       //�豸������²���// Device program update operation
    DEV_SYS_TIME_OPER,     //�豸ϵͳʱ�����// Device system time operation
    DEV_BASEINFO_OPER,     //������Ϣ����// Basic information operation
    DEV_STATISINFO_OPER,   //ͳ����Ϣ����// Statistical information operation
    DEV_WORKATT_OPER,      //�������Բ���// Work attribute operation
    DEV_USER_OPER,         //�û�����// User operation
    DEV_USER_RT_OPER,      //�û�ʵʱ���ز���// User real-time loading operation
    DEV_USER_RANGE_OPER,   //�û���������������������// User download data number feedback operation
    DEV_RECORD_OPER,       //��¼����// Record operation
    DEV_RECORD_RANGE_OPER, //��¼��������������������// Record download data number feedback operation
    DEV_ALARM_OPER  ,
    DEV_ALARM_RANGE_OPER  ,
    DEV_LOG_OPER,          //��־����// Log operation
    DEV_LOG_RANGE_OPER,    //��־��������������������// Log download data number feedback operation
    DEV_CHANGEIMAGE_OPER,  //���ע���ղ���// Modify registered image operation
    DEV_CIMAGE_RANGE_OPER, //���ע������������������������// Modify registered image download data number feedback operation
    DEV_TIME_GROUP_OPER,   //ʱ�������// Time group operation
    DEV_RIGHT_OPER,        //Ȩ�޲���// Access operation
    DEV_USERGROUP_OPER,    //�û������// User group operation
    DEV_DOOR_STATE_OPER,   //����״̬����// Door-open operation
    DEV_REMOTE_OPEN_DOOR,  //Զ�̿��Ų���// Remote open door operation
    DEV_VIDEO_TRANSFER,    //��Ƶ����// Video transmission
    DEV_USER_EX_OPER,      //�û���������// Massive user operation
    DEV_RECORD_EX_OPER,    //��¼��������// Massive record operation
    DEV_LOG_EX_OPER,       //��־��������// Massive log operation
    DEV_CIMAGE_EX_OPER,    //���ע������������// Massive modify registered images operation
    DEV_REBOOT_OPER,       //�豸����// Reset
    DEV_USER_REMOTE_CAP,   //�û�Զ�̲ɼ�// User remotely collect
    DEV_NOPEN_OPER,        //N+1����// N+1 door-open
    DEV_IOCTRL_OPER,       //IOCTRL������Ϣ // IOCTRL information control
    CLI_AUTH_OPER,         //client����ת��������֤����// client as transfer server validation operation
    DEV_NETPRO_OPER,       //�豸������������// Network propery setup
    DEV_SCREEN_OPER,       //��ĻУ��// Screen verification
    DEV_USER_REMOTE_RECAP, //�ɼ����Ȼظ�, DEV_RCAPINFO//Collection schedule feedback, DEV_RCAPINFO
    DEV_WAPINFO_OPER,      // for GetWpaIn
} DEV_OPER_TYPE;

//�������//Operate result
typedef enum
{
    OPER_SUCCESS           = 0x00, //�ɹ�// Success
    OPER_ERR_BUSY          = 0x01, //�豸æ//Device is unavailable
    OPER_ERR_LIMIT         = 0x02, //�Ѵ�����//Reach upper limitation
    OPER_ERR_NOFIND        = 0x03, //û���ҵ���Ӧ����//No data
    OPER_ERR_SAVEFAIL      = 0x04, //���ݱ���ʧ��//Data save failure
    OPER_ERR_SETFAIL       = 0x05, //����ʧ��//Setup failure
    OPER_ERR_FROMAT        = 0x07, //��ʽ��ʧ�� //Formatting failure
    OPER_ERR_PARAM         = 0x08, //����:���� //Parameter: error
    OPER_ERR_DISABLE       = 0x09, //Ҫ��ִ�еĹ���û��ʹ��//Requiring function disabled
    OPER_ERR_EXECUT        = 0x0A, //ʧ��//Failure
    OPER_ERR_SUPPORT       = 0x10, //��֧�ֵ�����//Nonsupport order
    OPER_ERR_INPUTDATA     = 0x11, //����˴�����������쳣//Network input data abnormal
    OPER_BATCH_DL_COMPLETE = 0x1F, //�����������//Massive download complete
} DEV_OPER_RESULT;

//ʱ��������//Time group type
typedef enum
{
    DEV_NORMAL   = 0x01, //����ʱ��// Normal time
    DEV_HOLIDY   = 0x02, //�ڼ���// Holiday
    DEV_ANY_TIME = 0x03, //����ʱ��//Anytime
} DEV_TIMEGROUP_TYPE;

//�û����� //User type
typedef enum
{
    DEV_USER_NOMAL      = 0, //��ͨ// Normal User
    DEV_USER_ADMIN      = 1, //����Ա// Administrator
    DEV_USER_SUPERADMIN = 2, //��������Ա//Super Administrator
} DEV_USER_TYPE;

//Ĭ��Ȩ��//Default access
typedef enum
{
    DEV_DEFAULT_NO_RIGHT    = -2, //��Ȩ��// No Access
    DEV_DEFAULT_SINGLE_OPEN = -1, //��һ����Ȩ��//Single open access
} DEV_DEFAULT_RIGHT;

//��¼���� // Record type
//0x01 - 0x20: �ճ���¼ Normal records
//0x20 - 0xFF: ������¼ Alarm records
#if 0
typedef enum
{
    DEV_VERIFY_SUCC              = 0x01, //��֤�ɹ�// Verify successful
    DEV_VERIFY_FAIL              = 0x02, //��֤ʧ��// Verify failed
    DEV_ADMIN_SUCC               = 0x03, //����Ա��֤�ɹ�// Administrator verify successful
    DEV_EMER_OPEN                = 0x04, //��������// Urgent door-open
    DEV_RIGHT_OPEN               = 0x05, //Ȩ�޿���// Access door-open
    DEV_GROUP_OPEN               = 0x06, //��Ͽ���// Combined door-open
    DEV_BUTTON_OPEN              = 0x07, //��ť����// Button door-open
    DEV_START_VERIFY             = 0x0c, //��ʼ��֤// Start verification
    DEV_STOP_VERIFY              = 0x0d, //ֹͣ��֤// Stop verification
    DEV_ALARM_HD_MANGET_TIMEOUT  = 0x20, //�Ŵų�ʱ// Door-magnet time-out
    DEV_ALARM_HD_MANGET_ILLOPEN  = 0x21, //�ŴŷǷ�����// Door-magnet illegally door-open
    DEV_ALARM_HD_OFFLINE         = 0x22, //ǰ�˵��߱���// Faceplate offline alarm
    DEV_ALARM_HD_BREAK           = 0x30, //���𱨾�// Tamper alarm
    DEV_ALARM_HD_SHOCK           = 0x31, //�𶯱���// Vibration alarm
    DEV_ALARM_HD_FPOWR           = 0x36, //ǰ�˹����쳣����// Faceplate power supply abnormal alarm
    DEV_ALARM_HD_UPS_ON          = 0x37, //UPS���õ�ؿ���// UPS standby battery open
    DEV_ALARM_HD_UPS_OFF         = 0x38, //UPS���õ�عر�// UPS standby battery close
    DEV_ALARM_HD_ASSIST          = 0x40, //�������봥��// Auxiliary input trigger
    DEV_ALARM_SF_BACKLIST        = 0xF0, //��������֤����// Blacklist verify alarm
    DEV_ALARM_SF_ILLCARD         = 0xF1, //��Ч��(��ʧ)// Invalid card (report loss)
    DEV_ALARM_SF_ILLTIME         = 0xF2, //�Ƿ�ʱ��ʶ��// Illegal time identification
    DEV_ALARM_SF_DEADLINE        = 0xF3, //ʧЧʱ��// Time of expiry
    DEV_ALARM_SF_DANGER_OPEN     = 0xF4, //в�ȿ���// Forced to open
    DEV_ALARM_SF_SUPER_OPEN      = 0xF5, //�������뿪�� //Super password door-open
} DEV_REC_TYPE;
#else

//��¼����:
//0x01 - 0x20: �����¼�
//0x20 - 0x40: Ӳ�������¼�
//0x40 - 0x50: ��������
//0x50 - 0x60: �������
//0xf0 - 0xff: ��������¼�
//����: δ����
typedef enum
{
    DEV_VERIFY_SUCC               = 0x01, //��֤�ɹ�
    DEV_VERIFY_FAIL               = 0x02, //��֤ʧ��
    DEV_ADMIN_SUCC                = 0x03, //����Ա��֤�ɹ�
    DEV_EMER_OPEN                 = 0x04, //��������
    DEV_RIGHT_OPEN                = 0x05, //Ȩ�޿���
    DEV_GROUP_OPEN                = 0x06, //��Ͽ���
    DEV_BUTTON_OPEN               = 0x07, //��ť����
    DEV_START_VERIFY              = 0x0c, //��ʼ��֤// Start verification
    DEV_STOP_VERIFY               = 0x0d, //ֹͣ��֤// Stop verification
    DEV_ALARM_HD_MANGET_TIMEOUT   = 0x20, //�Ŵų�ʱ
    DEV_ALARM_HD_MANGET_ILLOPEN   = 0x21, //�ŴŷǷ�����
    DEV_ALARM_HD_OFFLINE          = 0x22, //ǰ�˵��߱���
    DEV_ALARM_HD_BREAK            = 0x30, //���𱨾�
    DEV_ALARM_HD_SHOCK            = 0x31, //�𶯱���
    DEV_ALARM_HD_FPOWR            = 0x36, //ǰ�˹����쳣����
    DEV_ALARM_HD_UPS_ON           = 0x37, //UPS���õ�ؿ���
    DEV_ALARM_HD_UPS_OFF          = 0x38, //UPS���õ�عر�
    DEV_ALARM_HD_ASSIST           = 0x40, //�������봥��
    DEV_ALARM_HD_WL_DOOR_MAGNETIC = 0x41, //����
    DEV_ALARM_HD_WL_SMOKE         = 0x42,
    DEV_ALARM_HD_WL_GAS           = 0x43,
    DEV_ALARM_HD_WL_BUTTON1       = 0x44,
    DEV_ALARM_HD_WL_BUTTON2       = 0x45,
    DEV_ALARM_HD_WL_BUTTON3       = 0x46,
    DEV_ALARM_HD_WL_BUTTON4       = 0x47,
    DEV_ALARM_HD_WL_AREA1         = 0x48,
    DEV_ALARM_HD_WL_AREA2         = 0x49,
    DEV_ALARM_HD_WL_AREA3         = 0x4A,
    DEV_ALARM_HD_WL_AREA4         = 0x4B,
    DEV_ALARM_HD_WL_AREA5         = 0x4C,
    DEV_ALARM_HD_WL_AREA6         = 0x4D,
    DEV_ALARM_HD_WL_AREA7         = 0x4E,
    DEV_ALARM_LINK_OUT1           = 0x50,   //�������1
    DEV_ALARM_LINK_OUT2           = 0x51,   //�������2
    DEV_ALARM_LINK_GPRS           = 0x52,   //����GPRS
    DEV_ALARM_LINK_2OPEN          = 0x53,   //˫��
    DEV_ALARM_LINK_2CLOSE         = 0x54,   //˫��
    DEV_ALARM_LINK_2NORMAL        = 0x55,   //����
    DEV_ALARM_AB_RULE             = 0xE0,
    DEV_ALARM_SF_ILLREMOTE        = 0xE1,   //Զ�̼�����Ч
    DEV_ALARM_SF_ILLGROUP         = 0xE2,   //���ʧ�ܣ�����
    DEV_ALARM_SF_ILLCARD          = 0xE3,   //��Ч��
    DEV_ALARM_SF_ERRSTATE         = 0xE4,   //������״̬
    DEV_ALARM_SF_INTERLOCK        = 0xE5,   //AB����
    DEV_ALARM_SF_BACKLIST         = 0xF0,   //��������֤����
    DEV_ALARM_SF_LOSSCARD         = 0xF1,   //��ʧ��
    DEV_ALARM_SF_ILLTIME          = 0xF2,   //�Ƿ�ʱ��ʶ��
    DEV_ALARM_SF_DEADLINE         = 0xF3,   //ʧЧʱ��
    DEV_ALARM_SF_DANGER_OPEN      = 0xF4,   //в�ȿ���
    DEV_ALARM_SF_SUPER_OPEN       = 0xF5,   //�������뿪��
    DEV_ALARM_SF_CAP_SHAKE        = 0xF6,   //�ɼ�����
} DEV_REC_TYPE;
#endif

typedef enum
{
    DEV_DOOR_SOURCE = 0x01,                  //�������Դ // Source of the door
    DEV_SIGNAL_SOURCE,     //�ź�������������Դ // Source of signal input and output
    DEV_NO_NUM_DEV_SOURCE  //�ޱ���豸��Դ//Source of no serial number devi
} DEV_REC_SOURCE_TYPE;

typedef enum
{
    DEV_LOG_ADDUSER     = 0x01, //�����û�// Add user
    DEV_LOG_DELUSER     = 0x02, //ɾ���û�// Delete user
    DEV_LOG_REREGIST    = 0x03, //���²ɼ�// Re-collection
    DEV_LOG_CAHNGETYPE  = 0x04, //�ı��û�����// Modify user type
    DEV_UDISK_ULUSER    = 0x05, //U���ϴ��û�// USB flash disk upload user
    DEV_UDISK_DLUSER    = 0x06, //U�������û�// USB flash disk download user
    DEV_UDISK_DLRECORD  = 0x07, //U�����ؼ�¼// USB flash disk download record
    DEV_UDISK_UPDATEAPP = 0x08, //U�̸��³���//USB flash disk update program
} DEV_LOG_TYPE;

typedef enum
{
    DEV_DOOR_NOMAL = 0x00000000, //����״̬// Normal door status
    DEV_DOOR_OPEN  = 0x00000100, //��״̬// Open door status
    DEV_DOOR_CLOSE = 0x00000200  //��״̬//Close door status
} DEV_DOOR_STATE;

//ID ��ʶ// identification
typedef struct
{
    char m_ID[ DEV_ID_LEN ];
} DEV_CID;

//���� //Date
typedef struct
{
    int m_Year;
    char m_Month;
    char m_Day;
} DEV_DATE;

//ʱ��//Time
typedef struct
{
    int m_Msec; //���� //millisecond
    char m_Hour;
    char m_Minute;
    char m_Second;
} DEV_TIME;

//���ں�ʱ�� //Date and time
typedef struct
{
    DEV_DATE m_Date;
    DEV_TIME m_Time;
} DEV_DATETIME;

//�豸������Ϣ//Basic information
typedef struct
{
    int     m_DevType;     //�豸����//Device type
    int     m_LimitUser;   //��ʹ����������//Total user number upper limitation
    int     m_Limit1NUser; //1:N�û�����//1:N user upper limitation
    DEV_CID m_SN;          //�豸���//Device serial number
    DEV_CID m_Ver;         //DSP����汾//DSP software version
    DEV_CID m_Space;       //����������Ϣ//Disk size informati
} DEV_BASEINFO;
//DEV���ͳ����Ϣ//DEV related statis information
typedef struct
{
    int m_TotalUsers;       //��ǰ�豸�����û���//Total users number of current device
    int m_NoFeatUser;       //û�вɼ������������û���//Number of no facial features users
    int m_TotalDays;        //ʶ���¼����������//Total days for identification record retention
    int m_TotalRecords;     //�ܼ�¼��//Total record
    int m_TotalAlarm;       //�ܱ�����¼��//Total alarm record
    int m_TotalDspLog;      //�ܲ�����־��Ŀ //Total number of operational log
    int m_TotalChangeImage; //�ܱ��ע����//Total modified registered imag
} DEV_STATIS;

//�豸Ĭ��Ȩ��//Device default access
typedef struct
{
    int     m_TimeGID[DEV_TIMEGROUP_NUMS]; //ʱ����ID //Time group ID
    int     m_BaseSet;                     //�μ��豸���ܱ�Ƕ���//Refer to device functional marker definition
    int     m_DoorMangetTime;              //�Ŵ���ʱʱ�䣬��λ��//Door magnet delay time, seconds
    int     m_LockTime;                    //��������ʱ�䣬��λ��//Electric lock persistent time, seconds
    int     m_VerifyMode;                  //�û������֤ģʽ,����:DEV_VERIFY_USERID|DEV_VERIFY_CARD|DEV_VERIFY_FACE_11
    int     m_nWGType;                     //Τ��Э������(0���Τ��26,1���Τ��34)//Wiegand Protocol Type (0 output Wiegand 26, 1 output Wiegand 34)
    int     m_nWGOutType;                  //Τ���������(0���ID��1���WG����)//Wiegand Protocol type (0 output ID��1 output WG content)
    int     m_nWGOutIDType;                //���ID����(0�������,1����û�ID)//Output ID type (0 output card number,1 output user ID)
    int     m_nWGOutContent;               //WG�������//WG output content
    BOOL    m_bWGOutPut;                   //�Ƿ�WG���//If WG output
    DEV_CID m_szSuperPWD;                  //��������//Super password
    DEV_DEFAULT_RIGHT m_DefaultRight;
} DEV_WORKATT;

typedef struct
{
    DEV_USER_TYPE m_UserType;    //�ն��豸�ϵ��û�����(��ͨ���������� -- 0, 1, 2) //User type in terminal device (normal, administrator, super administrator -- 0 , 1 , 2)
    DEV_CID       m_UserID;      //�û�ID  //User ID
    DEV_DATETIME  m_TypeTime;    //�û����͸���ʱ��//Time for modifying user type
    DEV_DATETIME  m_RegistTime;  //����ע��ʱ�䣬�������ɼ�ʱ�䣬�ɼ��������²ɼ�Ҫ���´�ʱ�� //Time for registering image, which is the time for collecting facial features, collection and re-collection all need to update the time
    DEV_DATETIME  m_LastUpdTime; //�����µ�ʱ�䣬ʶ��ʱ�����������»������²ɼ�Ҫ���´�ʱ�� //Last-update time, features update in the recognition process or re-collect need to update the ti
} DEV_VUSER;

//�û���Ϣ//User information
typedef struct
{
    int           m_FeatLen;      //������С//Feature Length
    int           m_PicLen;       //��Ƭ��С//Picture Size
    int           m_RightID;      //�û�Ȩ��ID //User Right ID
    DEV_CID       m_ImageSID;     //�ϴ�ע���գ���Ӧ�ı��ID //uploading reg. pictures that correspond to ID of change and globally unique
    DEV_CID       m_UserID;       //�û�ΨһID,�ؼ��� //the unique User ID-Keyword
    DEV_CID       m_Admin;        //����ע�����Ա//Administrator of face registration
    DEV_CID       m_AppendID;     //���Ż�����������1:1�ĸ������ȷ����Ϣ//Card Num. or other appended info for 1:1 identifications
    DEV_CID       m_UserName;     //�û���//User Name
    DEV_CID       m_Department;   //��������//Department Name
    DEV_DATETIME  m_DeadLineTime; //ʧЧʱ��//Time of expiry
    DEV_DATETIME  m_RegistTime;   //����ע��ʱ��//Time of Face Registration
    DEV_DATETIME  m_LastUpdTime;  //�����µ�ʱ��//last-update Time
    DEV_DATETIME  m_TypeUpdTime;  //�û����ͱ��ʱ��//Update Time for User Types
    char          m_UserFlag;     //�û�״̬���//User Status Flag
    DEV_USER_TYPE m_UserType;     //�ն��豸�ϵ��û�����//User Type on terminals
    char*         m_FeatData;     //��������//Feature Data
    char*         m_PicData;      //��Ƭ����//Picture Data
    int           m_bRTUser;      //�Ƿ�Ϊʵʱ�û�(0 ��ʵʱ�û� 1 ʵʱ�ɼ��û� 2ʵʱ���������û�) //if it is real-time user (0 for non real-time user, 1 for real-time image-collecting user and 2 for real-time feature-updating user
} DEV_USER;

//�û����ؼ���//User download array
typedef struct
{
    int        m_Count;  //Array Size
    DEV_VUSER* m_pArray; //ID����//Array Starting Addre
} DEV_DL_USER_RANGE;

#define DEV_TYPE_FACE1 0x0 //����ǰ��1//Facial Terminal 1
#define DEV_TYPE_FACE2 0x1 //����ǰ��2//Facial Terminal 2
#define DEV_TYPE_CARD1 0x2 //ˢ����1//Card Scanner 1
#define DEV_TYPE_CARD2 0x3 //ˢ����2//Card Scanner 2
#define DEV_TYPE_IN    0x4 //��������//Auxiliary Input
#define DEV_TYPE_OUT   0x5 //�������//Auxiliary Output
#define DEV_TYPE_INWL  0x6 //��������//Wireless Input
#define DEV_TYPE_DOOR  0xF //�ŵ㱾��//Door

///////////////////////////////////////////////////////////////////////////////////////
//���ڼ�¼���¼���Դ485��ַ����:
//1.485��ַ��һ���ֽڱ�ʾ����4λ[4-7]��ʾ�豸������1-15, ��4λ[0-3]��ʾ֧��16�ֲ�ͬ�͵��豸
//����: 0x11 ��ʾ1��������ʶ��ǰ��1��INDEX[4-7] == 1 TYPE[0-3] == 1
//0x10 ��ʾ1��������ʶ��ǰ��0��INDEX[4-7] == 1 TYPE[0-3] == 0
//0x12 ��ʾ1����ˢ����0 INDEX[4-7] == 1 TYPE[0-3] == 2
//0x13 ��ʾ1����ˢ����1 INDEX[4-7] == 1 TYPE[0-3] == 3
//0x1F ��ʾ�ŵ㱾�� INDEX[4-7] == 1 TYPE[0-3] == F
//0x14 ��ʾ��������1 INDEX[4-7] == 1 TYPE[0-3] == 4
//0x25 ��ʾ�������2 INDEX[4-7] == 2 TYPE[0-3] == 5
//2. �����豸����Ŵ�1��ʼ����Э���֧��15���ţ�16�в�ͬ���͵��ŵ������豸��15�����������15���������
//3. ��������ĵ�ַ: [0-7] == 0x00 ��ʾ�Ƿ���ַ��[0-7] ���� 0x01��ʾ��˰屾��, [0-7] ���� 0x02��ʾGPRSģ��
//7_______3________0
//|_INDEX_|__type__|
///////////////////////////////////////////////////////////////////////////////////////
//RS485 address (1 byte) parsing of the event-recording devices::
//1.41. The 4 most significant bits[4-7] of the RS485 address represent the device indices 1-15; The 4 Least Significant bits[0-3] represent the devices of 16 different types.
// Examples: 0x11 denoting recog. terminal 1 of Door No.1��INDEX[4-7] == 1 TYPE[0-3] == 1
// 0x10 denoting recog. terminal 0 of Door No.1��INDEX[4-7] == 1 TYPE[0-3] == 0
// 0x12 denoting card scanner 0 for Door. No.1 INDEX[4-7] == 1 TYPE[0-3] == 2
// 0x13 denoting card scanner 1 for Door. No.1 INDEX[4-7] == 1 TYPE[0-3] == 3
// 0x1F denoting the Door Point INDEX[4-7] == 1 TYPE[0-3] == F
// 0x14 denoting auxiliary input 1 INDEX[4-7] == 1 TYPE[0-3] == 4
// 0x25 denoting auxiliary input 2 INDEX[4-7] == 2 TYPE[0-3] == 5
//2. All device numbers start with 1. This protocol supports 15 doors, 16 different door terminals and 15 input devices and output devices.
//3. Reserved Addresses of special purposes: [0-7] == 0x00  for illegal addresses��[0-7] ���� 0x01 for internal circuitry boards, [0-7] ���� 0x02 for GPRS modules
//7_______3________0
//|_INDEX_|__type__|
///////////////////////////////////////////////////////////////////////////////////////
//ʵʱ�û�����//User real-time loading
typedef struct
{
    int     m_LoadFlag;  //DL_DEV_PIC|DL_DEV_USER_FEAT=��Ҫ����ע����Ƭ���������� //DL_DEV_PIC|DL_DEV_USER_FEAT=Loading registration pictures and facial features
    int     m_ReqSource; //������Դ,���485��ַ����//Request source, please refer to parts for 485 address parsing
    BOOL    m_bApeendID; //TRUE=����, FALSE=����//TRUE=card number, FALSE=work number
    DEV_CID m_ID;        //����ID//Loading
} DEV_USER_REAL_LOAD;

//��¼��Ϣ//Log Information
typedef struct
{
    DEV_REC_TYPE        m_RecType;       //��¼����//Record Type
    DEV_REC_SOURCE_TYPE m_Source;        //��¼��Դ(�Ѿ���ʱ,����ʹ��)//Source of Recording (obsolete)
    BOOL                m_bRealTime;     //�Ƿ�Ϊʵʱ��¼ //if it is real-time recording
    int                 m_Score;         //ʶ��÷�//Score of recognition
    int                 m_PicLen;        //��Ƭ��С//Picture Size
    int                 m_VerifyMode;    //��֤ģʽ,����:DEV_VERIFY_USERID|DEV_VERIFY_CARD|DEV_VERIFY_FACE_11  //Modes of verification, For Example: DEV_VERIFY_USERID|DEV_VERIFY_CARD|DEV_VERIFY_FACE_11
    DEV_CID             m_ChangID;       //�����¼��//Change Record ID
    DEV_CID             m_SerialID;      //ʶ���¼��ˮ��ID//Serial ID for Recognition records
    DEV_CID             m_UserID;        //�û�ID,���Ϊ�գ���ʾ�Ƿ���¼//User ID (if empty, it means illegal records)
    DEV_CID             m_AppendID;      //���Ż�����������1:1�ĸ������ȷ����Ϣ//Card Num. or other or other appended info for 1:1 identifications
    DEV_CID             m_UserName;      //�û���//User Name
    DEV_CID             m_Department;    //��������//Department Name
    DEV_DATETIME        m_RecTime;       //��¼ʱ��//Record Time
    char                m_ConcretSource; //��¼��Դ485��ַ�����485��ַ����//485 Address for record source, please refer to parts for 485 address parsing
    char*               m_PicData;       //ԭʼJPGͼ������(δ��base64����)//Original JPG Image Data (without base 64 encodin
} DEV_RECORD;

typedef struct
{
    BOOL         m_bRTLog;    //�Ƿ�Ϊʵʱ������־//If it is real-time operational log
    DEV_LOG_TYPE m_LogType;   //��־����//Log Type
    DEV_CID      m_SerialID;  //��ˮ��ID//Serial ID
    DEV_CID      m_Admin;     //����Ա //Operator
    DEV_CID      m_BeOptUser; //������Ա//Be operated user
    DEV_DATETIME m_RecTime;   //��¼ʱ�� //Record Ti
} DEV_LOG;

//���ע����//Modify Registration Picture
typedef struct
{
    int          m_PicBytes;       //ע���մ�С//Registration Picture Size
    BOOL         m_bRTChangeImage; //�Ƿ�Ϊʵʱ���ע����//If it is real-time modified registration picture
    DEV_CID      m_UserID;         //�û�ID//User ID
    DEV_CID      m_SerialID;       //��ˮ��ID//Serial ID
    DEV_CID      m_AppendID;       //���Ż�����������1:1�ĸ������ȷ����Ϣ//Card Num. or other or other appended info for 1:1 identifications
    DEV_CID      m_UserName;       //�û���//User Name
    DEV_CID      m_Department;     //��������//Department Name
    DEV_CID      m_Admin;          //����ע�����Ա,��ʶ���û��������������ĸ�����Ա�ɼ�
    //Facial feature registration administrator, to mark which administrator collects the facial feature of certain user
    DEV_DATETIME m_RecTime;        //��¼ʱ��//Record Time
    char*        m_PicData;        //JPGͼ������(δ��base64����)//Original JPG Image Data (without base64 encodin
} DEV_CHANGEIMAGE;

//��¼�������� //Downloading records interval
typedef struct
{
    int     m_Count;      //���ض�����//Amount of the downloading
    BOOL    m_bOpenRange; //�Ƿ�Ϊ������(true=�ǣ� false=��)//it is an open interval? (true or false)
    DEV_CID m_SID;        //����һ����ʼ��m_SID��ʾ��¼��ȷ����ˮ�� SID��ɣ�SN + TYPE + DATE + ID = ( 6BYTE + 1BYTE + 8BYTE + 4BYTE + \0) //From which m_SID represents the exact serial number for records. SID��SN + TYPE + DATE + ID = ( 6BYTE + 1BYTE + 8BYTE + 4BYTE
} DEV_DL_RECORD_RANGE;

//��¼�������伯//Set of Intervals for downloading records
typedef struct
{
    int m_Count;
    DEV_DL_RECORD_RANGE* m_pRange;
} DEV_DL_RECORD_ARRAY;

//ʱ���� //Time Group
typedef struct
{
    int                m_TGID;      //ʱ����ID //Time group ID
    DEV_DATETIME       m_Start;     //ʱ���鿪ʼʱ��//Time group start time
    DEV_DATETIME       m_End;       //ʱ�������ʱ��//Time group stop time
    DEV_TIMEGROUP_TYPE m_TGType;    //ʱ��������//Time group type
    char               m_CheckFlag; //ʱ����������m_CheckFlag = CHECK_TIME|CHECK_WEEK//Time check mark e.g.: m_CheckFlag = CHECK_TIME|CHECK_WEEK
    char               m_WeekFlag;  //�������ʱ�������Щ������Ч������m_WeekFlag = WEEK_1|WEEK_5  //Check week, mark the effective weeks. For Example: m_WeekFlag = WEEK_1|WEEK
} DEV_TIMEGROUP;

//ʱ��������//Time Group Array
typedef struct
{
    int m_nCount;
    DEV_TIMEGROUP* m_pTGArray;
} DEV_TIMEGROUP_ARRAY;

#define DEV_CHECK_HOLIDY      0x01     //Ȩ�޼��ڼ���
#define DEV_ACTION_LOCK       0x02     //�������
#define DEV_ACTION_OUTPUT     0x04     //�������
#define DEV_ACTION_N_1        0x08     //��Ȩ����ҪN+1

//Ȩ��//Access
typedef struct
{
    int  m_RightID;                     //Ȩ��ID // Access ID
    int  m_TimeGID[DEV_TIMEGROUP_NUMS]; //ʱ����ID, (m_TimeGID[0]==ANY_TIME)δָ��ʱ���飬���ŷ�ʽ����ʱ�����ƣ�����ʱ�����֤�ɹ�ִ�п��ŷ�ʽ. //Time Group ID, (m_TimeGID[0]==ANY_TIME)Unspecified time group, no time limitation for door open pattern, if verify successfully, door opens in anytime
    BOOL m_bHolidyValid;                //�ڼ����Ƿ���Ч//If holidays are effective
    BOOL m_bActionLock;                 //�������//Electric lock output
    BOOL m_bActionOutPut;               //�����������//Electric lock auxiliary ou
} DEV_RIGHT;

//Ȩ������//Access Array
typedef struct
{
    int m_nCount;
    DEV_RIGHT* m_pRtArray;
} DEV_RIGHT_ARRAY;

typedef struct
{
    int     m_GroupID;                       //��ID //Group ID
    int     m_NormalValid;                   //��ͨ�û�����Ч�û���//Amount of effective users in normal users
    int     m_ForceValid;                    //ǿ���û�����Ч�û���//Amount of effective users in mandatory users
    DEV_CID m_NormalUsers[DEV_USER_COMBINS]; //��ͨ�û���ϣ����ȼ���//Normal user combination, low priority
    DEV_CID m_ForceUsers[DEV_USER_COMBINS];  //ǿ���û���ϣ����ȼ���//Mandatory users combination, high priority
    BOOL    m_bGroupOrder;                   //����Ƿ����� 1����0���� //If combination in order, 1=order, 0=no ord
} DEV_USERGROUP;

typedef struct
{
    int m_nCount;
    DEV_USERGROUP* m_pUGArray;
} DEV_USERGROUP_ARRAY; //�û������� //Array Starting Address

//������½ṹ��
//Program update Structures
typedef struct
{
    int m_Bytes; //file size
    int m_Type;  //file type definition is as below
    // 0  �̼��ļ� firmware document
    // 1  ���������ļ� Standby interface document
    // 2  ֤���ļ�(.pk12) Certificate document (.pk12)
    DEV_CID m_CtxInfo;
    DEV_CID m_FileName; //�ļ���//File Name
    char*   m_Buf;      //�ļ����ݻ����׵�ַ//File data-caching starting address
} DEV_APPUPDATE;

//�û���������//Batch User Manipulations
typedef struct
{
    int m_nCount;
    DEV_USER* m_pUserArray;
} DEV_BATCH_USER;

//������¼//Batch Record
typedef struct
{
    int m_nCount;
    DEV_RECORD* m_pRecordArray;
} DEV_BATCH_RECORD;

//������־//Batch Log
typedef struct
{
    int m_nCount;
    DEV_LOG* m_pLogArray;
} DEV_BATCH_LOG;

//����ע����//Batch Registration Pictures
typedef struct
{
    int m_nCount;
    DEV_CHANGEIMAGE* m_pCImageArray;
} DEV_BATCH_CIMAGE;

//IO�豸״̬//IO device status
#define DEV_IO_MODE_NORMAL 0x00  //����״̬//Normal status
#define DEV_IO_MODE_OPEN   0x01  //����״̬//Open status
#define DEV_IO_MODE_CLOSE  0x02  //����״̬//Close status
//�Ŵ�״̬
#define DEV_IO_STATE_CLOSE 0x00  //�ŴŹ�//Door magnet close
#define DEV_IO_STATE_OPEN  0x01  //�Ŵſ�//Door magnet open
//ִ�ж���
#define DEV_ACT_IO_OPEN    0x02  //ִ�д򿪶���//Perform open action
#define DEV_ACT_IO_OPENEX  0x04  //ִ�и�������//Perform underaction
#define DEV_ACT_IO_CLOSE   0x10  //ִ�йرն���//Perform close action
#define DEV_ACT_MODE_SET   0x20  //����IO�豸����ģʽ//Setup IO device work pattern
#define DEV_ACT_MODE_GET   0x40  //���IO�豸����ģʽ//Acquire IO device work pattern
#define DEV_ACT_STATE_GET  0x80  //���IO�豸��ǰ״̬//Acquire IO device current stat
typedef struct
{
    char m_Source; //IO�豸485��ַ//IO device 485 address
    char m_Action; //ִ�ж���//Perform action
    char m_IOMode; //IO�豸��ǰģʽ//IO device current pattern
    char m_IOState;//IO�豸��ǰ״̬״̬ //IO device current status
} DEV_IOCTRL;

//NOPEN:Զ���������˿���
//NOPEN:Request open door remotely through Administrative End
typedef struct
{
    int                              m_CtxID; //m_Users==1Ȩ��ID, m_Users>1���ID//m_Users==1 access ID, m_Users>1 combined ID
    int                              m_Users; //��֤�û���Ŀ�� 0���뿪�ţ�1Ȩ�޿��ţ�>1��Ͽ��� //Amount of verified users: 0 open with password��1 open with access��>1 combined way to open
    DEV_IOCTRL                       m_XOpen; //������Ϣ//Control information
    DEV_CID m_IDS[DEV_USER_COMBINS * 2];      //��֤�û�����//Verify user arr
} DEV_NOPEN;

//Զ�̲ɼ�������Ϣ//Remote collection schedule information
typedef struct
{
    int     m_Total;  //�ɼ�����//collection sum
    int     m_Count;  //��ǰ�ɼ���//collection number at present
    DEV_CID m_UserID; //�û��ǼǱ��//User registration serial numb
} DEV_RCAPINFO;

#define DEV_WEB_LOADUSER         0x01 //�����û�//Loading users
#define DEV_WEB_WRITEUSER        0x02 //д�û�//write user
#define DEV_WEB_WRITEREC         0x04 //д��¼//write record
#define DEV_WEB_SYNCTIME         0x08 //ͬ��ʱ��//synchronization time
#define DEV_WEB_JUDGE            0x10 //ʶ��ɹ����ɷ����Ȩ����֤
#define DEV_WEB_RECORD_TXT       0x20 //��¼�Զ��ϴ�ʱ��ֻ�ϴ�������Ϣ
#define DEV_WEB_ENABLE           0x80 //
//��������//Network property
//��������ipv6��ַ
typedef struct
{
    char m_WebPro1;
    char m_WebPro2;
    char m_WebUrl1[URL_LEN];
    char m_WebUrl2[URL_LEN];
    char m_IPAdr[IP_LEN];
    char m_MIPAdr[IP_LEN];
    char m_NetMask[IP_LEN];
    char m_GateWay[IP_LEN];
    char m_Dns[IP_LEN];
} DEV_NETPRO;

////////////////////////////////////////////////////////////////////////////////////////
//����: �豸��������
//����:
//cszDevAddr �豸��ַ
//eCNStatus ����״̬
//pvContext Ӧ��������
//˵��:
//���� <0 �������� 0�����ܣ�1���ܷ�������, 2���ܿͻ���
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Connection warning
//Parameter:
//cszDevAddr Address
//eCNStatus Connection status
//pvContext parameter for context
//Comment:
//return <0 stop connecting 0 no encryption��1 Encrypt server-side, 2 Encrypt terminal
////////////////////////////////////////////////////////////////////////////////////////
typedef int (SLGDEV_CALL* DevConnectStatus)(const char* cszDevAddr, DEV_CONNECT_STATUS eCNStatus, void* pvContext);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ѯ�豸��ת�����������������豸������ת������IP��ַ
//����:
//cszDevAddr �豸��ַ
//eCNStatus ���ص���ת������IP
//pvContext Ӧ��������
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function:  inquire about the transfer server which the current device belongs to and return the IP address of the server
//Parameter:
//cszDevAddr Device address
//eCNStatus IP address of transfer server to be returned
//pvContext parameter for context
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
typedef BOOL (SLGDEV_CALL* QuerySrvCB)(const char* cszDevAddr, SOCK_ADDRESS* srvAddr, void* pvContext);

////////////////////////////////////////////////////////////////////////////////////////
//����: �豸��������ص�
//����: cszDevAddr �豸��ַ
//eType ��������
//eFlag ������ʶ
//pvContent �����������
//nSeq ������ˮ��
//eResult �������
//pvContext Ӧ�ò�������
//˵��: �ϲ�Ӧ���յ�����Ϣ�󼴿�֪��ǰһ�β����Ƿ�ɹ�
//eType��eFlag��eResult����pvContent����
////////////////////////////////////////////////////////////////////////////////////////
//Function: Call-back the device operation results
//Parameter: cszDevAddr Device address
//eType operational type
//eFlag operational flag
//pvContent contents of operation results
//nSeq Serial Number of operations
//eResult operation results
//pvContext context
//Comment: The upper-layer applications will know if the prev. operation has succeeded by receiving the message.
//eType��eFlag��eResult determine the content of pvContent
////////////////////////////////////////////////////////////////////////////////////////
//��eResult=OPER_SUCCESS && eFlag=DEV_OPER_DOWNLOADʱ��pvContent != NULL, eType�������ͽ�������
//DEV_AUTH_OPER pvContent=NULL �ͻ��������֤����
//DEV_AUTH_SET_OPER pvContent=NULL �ͻ��������û����������óɹ�����
//DEV_REGION_OPER pvContent=NULL
//DEV_FORMAT_OPER pvContent=NULL
//DEV_UPDATE_OPER pvContent=NULL
//DEV_SYS_TIME_OPER pvContent=DEV_DATETIME* or = NULL
//DEV_BASEINFO_OPER pvContent=DEV_BASEINFO* or = NULL
//DEV_STATISINFO_OPER pvContent=DEV_STATIS* or = NULL
//DEV_WORKATT_OPER pvContent=DEV_WORKATT* or = NULL
//DEV_USER_OPER pvContent=DEV_USER* or = NULL
//DEV_USER_RT_OPER pvContent=DEV_USER_REAL_LOAD*
//DEV_USER_RANGE_OPER pvContent=DEV_DL_USER_RANGE* or = NULL
//DEV_RECORD_OPER pvContent=DEV_RECORD* or = NULL
//DEV_RECORD_RANGE_OPER pvContent=DEV_DL_RECORD_RANGE* or = NULL
//DEV_LOG_OPER pvContent=DEV_LOG* or = NULL
//DEV_LOG_RANGE_OPER pvContent=DEV_DL_RECORD_RANGE* or = NULL
//DEV_CHANGEIMAGE_OPER pvContent=DEV_CHANGEIMAGE* or = NULL
//DEV_CIMAGE_RANGE_OPER pvContent=DEV_DL_RECORD_RANGE* or = NULL
//DEV_TIME_GROUP_OPER pvContent=DEV_TIMEGROUP_ARRAY* or = NULL
//DEV_RIGHT_OPER pvContent=DEV_RIGHT_ARRAY* or = NULL
//DEV_USERGROUP_OPER pvContent=DEV_USERGROUP_ARRAY* or = NULL
//DEV_DOOR_STATE_OPER pvContent=DEV_DOOR_STATE* or = NULL
//DEV_NOPEN_OPER pvContent=DEV_NOPEN* �豸��������Զ�̿���
//DEV_IOCTRL_OPER pvContent=DEV_IOCTRL* IO������Ϣ����
//CLI_AUTH_OPER pvContent=AUTH* nSeq����client�˿�
////////////////////////////////////////////////////////////////////////////////////////
typedef BOOL (SLGDEV_CALL* DevOperResultNotify)(const char* cszDevAddr, DEV_OPER_TYPE eType,
        DEV_OPER_FLAG eFlag, void* pvContent, int nSeq,
        DEV_OPER_RESULT eResult, void* pvContext);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ʼ���ڷ���ʼ��ϵͳ
//����: bFlag true=��ʼ��,false=����ʼ��
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: System initialization and anti-initialization
//Parameter: bFlag true=initialization,false=anti-initialization
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_InitSys(BOOL bFlag);

////////////////////////////////////////////////////////////////////////////////////////
//����: ע���豸����״̬���ѻص�
//����: pfnCNNotify �ص�����ָ��
//pvContext Ӧ��������
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Call-back the registered device connection status warning
//Parameter: pfnCNNotify Callback function pointer
//pvContext context
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
void SLGDEV_CALL SLG_RegDevConnectStatusCB(DevConnectStatus pfnCNNotify, void* pvContext);

////////////////////////////////////////////////////////////////////////////////////////
//����: ע���豸�������֪ͨ
//����: pfnOperNotify �ص�����ָ��
//pvContext Ӧ��������
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: notification of results of the registered device operations
//Parameter: pfnOperNotify callback function pointer
//pvContext context
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
void SLGDEV_CALL SLG_RegOperResultNotifyCB(DevOperResultNotify pfnOperNotify, void* pvContext);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��������ģ��Ϊ��תģʽ
//����:
//srvMode ����ģʽ TRUE ��תģʽ�� FALSE����ģʽ
//pfn ��ת�����ѯ�ص�����
//pvContext Ӧ��������
//˵��: �����øú�����Ĭ��Ϊ����ģʽ
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: configure the operation mode for the whole module (transfer mode)
//Parameter:
//srvMode work pattern TRUE transfer mode�� FALSE local mode
//pfn  the call-back inquiry function for transfer service
//pvContext context
//Comment: The default mode is the local mode if the function is not called.
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_SetMode(BOOL srvMode, QuerySrvCB pfn, void* pvContext);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���ӵ���ת������
//����:
//bFlag true=������ת������,false=�Ͽ�����ת������������
//ip ��ת������IP��ַ
//name client��֤�û���
//psw client��֤����
//˵��: ��ת���������ӳɹ�����TRUE������FALSE�������֤�ɹ����ͨ��DevOperResultNotify�ص�֪ͨ�ⲿ.
//ֻҪ���øú�����������ת���������ӳɹ����ģʽ�Զ��л�Ϊ��תģʽ����ʾ���� SLG_CnSrv(FALSE,NULL,NULL,NULL)
//�����л�������ģʽ.�����øú���Ϊ����ģʽ
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: connect to the transfer server
//Parameter:
//bFlag true=connect with transfer server  false=disconnect with transfer server
//ip IP address of the transfer server
//name client  verification of client name
//psw client verification of client password
//Comment:  the transfer server will return TRUE for successful connection and FALSE for unsuccessful connection. The status of the verification (passed or failed) will be notified through the call-back function DevOperResultNotify.
//If the function is called, the mode will change to transfer mode automatically and shows calling SLG_CnSrv (FALSE,NULL,NULL,NULL),without concerning the connection status of the server.
//If the function is not called, it will be the local mode.
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_CnSrv(BOOL bFlag, const char* ip, const char* name, const char* psw);

////////////////////////////////////////////////////////////////////////////////////////
//����: �����豸�������
//����: bFlag (true=���� false=ֹͣ)
//cszRgnAddr �豸���������ַ
//˵��: ����3����������ǰ�汾����ʱ��ʹ��
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: If starting search for devices
//Parameter: bFlag (true=start false=stop)
//cszRgnAddr address of the interval the device is in
//Comment: These 3 functions cannot be used in this current version
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_StartDevSerch(BOOL bFlag, const char* cszRgnAddr = DEV_REGION_ADDR);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���豸��������
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Connecting with the facial recog. Device
//Parameter: cszDevAddr Device address
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_CNDev(const char* cszDevAddr);
////////////////////////////////////////////////////////////////////////////////////////
//����: �Ͽ��豸����
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Disconnecting
//Parameter: cszDevAddr Device address
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
void SLGDEV_CALL SLG_DCNDev(const char* cszDevAddr);

////////////////////////////////////////////////////////////////////////////////////////
//����: �豸��֤��������֤
//����: cszDevAddr�豸��ַ
//cName �û���
//cPws ����
//nFlag 1=��֤ 2=����
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Device verification or setup verification
//Parameter: cszDevAddrDevice address
//cName user name
//cPws password
//nFlag 1=verification 2=setup
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULDevAuth(const char* cszDevAddr, const DEV_CID& cName, const DEV_CID& cPws, int nFlag, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �����豸���������ַ
//����:
//cszDevAddr �豸��ַ
//cszRgnAddr �豸�����ַ
//˵��: ���豸�������Ӻ�ɽ��豸���õ�ĳһ�������Ժ󼰿ɶ����������������豸
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: setup interval address of the device
//Parameter:
//cszDevAddr Device address
//cszRgnAddr interval address of the device
//Comment: to set the device to certain interval after the connection, and it can be searched under this interval
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULDevRegionAddr(const char* cszDevAddr, const char* cszRgnAddr = DEV_REGION_ADDR, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �����豸ϵͳʱ��
//����:
//cszDevAddr �豸��ַ
//cSysTime ʱ����Ϣ
//˵��: DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_SYS_TIME_OPER
//eFlag = DEV_OPER_UPLOAD ��
//pvContent = NULL
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Configuration of System Time
//Parameter:
//cszDevAddr Device address
//cSysTime Time info
//Comment: DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_SYS_TIME_OPER
//eFlag = DEV_OPER_UPLOAD ��
//pvContent = NULL
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULDevTime(const char* cszDevAddr, const DEV_DATETIME& cSysTime, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ȡ�豸ϵͳʱ��
//����: cszDevAddr �豸��ַ
//˵��: DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_SYS_TIME_OPER
//eFlag = DEV_OPER_DOWNLOAD��
//pvContent = DEV_DATETIME*
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Get the System Time
//Parameter: cszDevAddr Device address
//Comment: DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_SYS_TIME_OPER
//eFlag = DEV_OPER_DOWNLOAD��
//pvContent = DEV_DATETIME*
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLDevTime(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ȡ�豸������Ϣ
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLDevBaseInfo(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ȡ�豸ͳ����Ϣ
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Get basic information
//Parameter: cszDevAddr Device address
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLDevStatisInfo(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ȡ�豸����������Ϣ
//����:
//cszDevAddr ��������ַ
//nSeq �������к�
//˵��: ��������DEV_WORKATTͨ���ص�����������
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Get Device work attribute information
//Parameter:
//cszDevAddr Device address
//nSeq Operation Serial Num
//Comment: Work attribute DEV_WORKATT  is given by callback funtion
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLDevWorkAttInfo(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �����豸����������Ϣ
//����:
//cszDevAddr �豸��ַ
//cWorkAtt �������Խṹ��
//nSeq �������к�
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Device work attribute information setup
//Parameter:
//cszDevAddr Device address
//cWorkAtt Work attribute structure
//nSeq Operation serial number
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULDevWorkAttInfo(const char* cszDevAddr, const DEV_WORKATT& cWorkAtt, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �ϴ�ָ���û�
//����:
//cszDevAddr �豸��ַ
//cUser �û���Ϣ�ṹ��
//nSeq �������к�
//˵��:
//�û���������ʱ��DEV_USER�е� m_FeatLen = 0�� m_FeatData = NULL
//�û���ע����ʱ��DEV_USER�е� m_PicLen= 0�� m_PicData = NULL
//DevOperResultNotify�ص�������Ӧ���ͣ�eType = DEV_USER_OPER
//eFlag = DEV_OPER_UPLOAD��pvContent= NULL
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Uploading specific user
//Parameter:
//cszDevAddr Device address
//cUser user information structure
//nSeq Operation serial number
//Comment:
//User without face library ��DEV_USER: m_FeatLen = 0�� m_FeatData = NULL
//User without registration picture��DEV_USER: m_PicLen= 0�� m_PicData = NULL
//DevOperResultNotify Callback parameter corresponding type: eType = DEV_USER_OPER
//eFlag = DEV_OPER_UPLOAD��pvContent= NULL
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULUser(const char* cszDevAddr, const DEV_USER& cUser, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �ϴ�ʵʱ�����û�
//����:
//cszDevAddr �豸��ַ
//cUser �û���Ϣ�ṹ��
//cRtLoad ʵʱ������Ϣ
//nSeq �������к�
//˵��: �豸������ͻ����ϴ�ĳ�û�ʱ��ͨ���˽ӿ��ϴ��������û���Ϣ��
//cRtLoadΪ�豸������ʱ����������Ϣ���ͻ��˽�ֹ�޸ġ��˺���ʵ��Զ���û����ع��ܡ�
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Uploading real-time user
//Parameter:
//cszDevAddr Device address
//cUser user info structure
//cRtLoad loading information in real time
//nSeq Operation Serial Num
//Comment: If the device requests the terminal to upload certain user, it uses this port to upload the requested user information.
//cRtLoad is the information sent from the device, terminal forbids to modify. This function allows to loading user remotely.
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULRealTimeUser(const char* cszDevAddr, const DEV_USER& cUser, const DEV_USER_REAL_LOAD& cRtLoad);

////////////////////////////////////////////////////////////////////////////////////////
//����: Զ��ʵʱ�ɼ��û�����
//����:
//cszDevAddr �豸��ַ
//cUser �û���Ϣ
//˵��: �����¼���û�����ʱ����ͨ���ú������û����Ϸ��͵��������������������ɼ����û���������
//Զ���û��ɼ����ܣ�ʹ��ֱ���ڹ���˾Ϳ��Բɼ��û��������Զ����Ƶ������²ɼ��Ŀ��ӻ��̶Ⱥ�ֱ���ڻ����ϲɼ�һ����
//���ʹ����(�û��ɼ�ʵʱͬ��)��Զ�̲ɼ����û�Ҳ��ʵʱ�ķ��ع����.
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Remotely collect user features in real-time
//Parameter:
//cszDevAddr Device address
//cUser User Info
//Comment: The user information input via administrative end can be sent to the terminal
// by this function, then to control the terminal to collect the user��s facial features.
//The remote user collection function allows the administrative end to collect users,
// the collection visualization via remote video is the same as via the terminal.
//If enabling the User Collection Real-time Synchronization, the users that are
// collected remotely will be sent to administrative end in real-time, too.
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULRealTimeUserCap(const char* cszDevAddr, const DEV_USER& cUser, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ɾ��ָ���û�
//����:
//cszDevAddr �豸��ַ
//cID �û����
//nSeq �������к�
//˵��: ɾ���ɹ����ͨ�������ص������������
//DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_USER_OPER
//eFlag = DEV_OPER_DELETE��
//pvContent = NULL
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function:  Deleting specific user
//Parameter:
//cszDevAddr Device address
//cID User ID
//nSeq Operation Serial Num
//Comment: Deleting or not��manipulating callback function to Acquire result
//DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_USER_OPER
//eFlag = DEV_OPER_DELETE��
//pvContent = NULL
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DELUser(const char* cszDevAddr, const DEV_CID& cID, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����ָ���û�
//����:
//cszDevAddr �豸��ַ
//cID �û�ID
//nFlag DL_DEV_USER_PIC �������û���Ƭ
//DL_DEV_USER_FEAT ��Ҫ������������
//DL_DEV_USER_PIC|DL_DEV_USER_FEAT=����
//Ϊ0Ĭ������������Ϣ
//nSeq �������к�
//˵��: DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_USER_OPER
//eFlag = DEV_OPER_DOWNLOAD��
//pvContent = DEV_USER*
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Downloading specific user
//Parameter:
//cszDevAddr Device address
//cID user ID
//nFlag DL_DEV_USER_PIC needs to downloading user picture
// DL_DEV_USER_FEAT  needs to downloading facial features
// DL_DEV_USER_PIC|DL_DEV_USER_FEAT=both
// =0 means to downloading text messages
//nSeq Operation Serial Num
//Comment: DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_USER_OPER
//eFlag = DEV_OPER_DOWNLOAD��
//pvContent = DEV_USER*
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLSingleUser(const char* cszDevAddr, const DEV_CID& cID, int nFlag, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���������û�
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Batch Downloading Users
//Parameter: cszDevAddr Device address
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLAllUser(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����ĳһʱ����ڵ�ע���û�
//����:
//cszDevAddr �豸��ַ
//cStart ��ʼ����
//cEnd ��ֹ����
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Downloading users registered in certain time
//Parameter:
//cszDevAddr Device address
//cStart start date
//cEnd  end date
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLSegTimeUser(const char* cszDevAddr, const DEV_DATE& cStart, const DEV_DATE& cEnd, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����ʶ���¼����
//����:
//cszDevAddr �豸��ַ
//nSeq ��������
//˵��: DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_RECORD_OPER
//eFlag = DEV_OPER_DOWNLOAD
//pvContent = DEV_DL_RECORD_ARRAY*
//�ο�DEV_DL_RECORD_ARRAY�ṹ��һ��Ϊһ�����䣬�ú�������ʶ���¼���������䡣
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Downloading identification record interval
//Parameter:
//cszDevAddr Device address
//nSeq Operation sequence
//Comment: DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_RECORD_OPER
//eFlag = DEV_OPER_DOWNLOAD
//pvContent = DEV_DL_RECORD_ARRAY*
//Refer to DEV_DL_RECORD_ARRAY structure��one day is a interval, this function returns to all intervals of identification record.
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLAllIdentifyRecord(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����ָ��ʱ���ʶ���¼
//����: cszDevAddr �豸��ַ
//cStart ��ʼ����
//cEnd ��ֹ����
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Downloading specific identification record interval
//Parameter: cszDevAddr Device address
//cStart start date
//cEnd end date
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLSegTimeIdentifyRecord(const char* cszDevAddr, const DEV_DATE& cStart, const DEV_DATE& cEnd, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ȡ����ʶ���¼/������¼ָ��ID�ĺ�������
//����:
//cszDevAddr �豸��ַ
//cRange ID ����
//flag DL_DEV_RECORD_PIC=��Ҫ���ؼ�¼��Ƭ
//bKnowRange true=�������ݣ� false=���伯��
//nSeq ��������
//˵��: ��bKnowRange Ϊtrue�ú������ؾ�������ݣ�Ϊfalseʱ��ѯָ���������豸�ϵľ����С��
//DevOperResultNotify�ص�������Ӧ���ͣ�
//��������ʱ
//pvContent = DEV_DL_RECORD_ARRAY*
//cRange.m_SID = ָ��SN
//cRange.m_Count = -1;
//cRange.m_bOpenRange = TRUE;
//bKnowRange = FALSE��flag = 1����Ч������
//���ؾ����¼ʱ
//DevOperResultNotify�ص�������Ӧ���ͣ�
//pvContent = DEV_RECORD*
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Get interval identification record / follow-up interval of alarm record of appointed ID
//Parameter:
//cszDevAddr Device address
//cRange ID interval
//flag DL_DEV_RECORD_PIC=Downloading record picture
//bKnowRange true=specific data, false=interval set
//nSeq Operation sequence
//Comment:  If bKnowRange is true, this function downloads specific data, if false, it queries the specific interval size of the device.
// DevOperResultNotify Callback parameter corresponding type:
//Downloading interval
//pvContent = DEV_DL_RECORD_ARRAY*
//cRange.m_SID = appointed SN
//cRange.m_Count = -1;
//cRange.m_bOpenRange = TRUE;
//bKnowRange = FALSE��flag = 1��invalid parameter��
//Downloading specific record
//DevOperResultNotify Callback parameter corresponding type:
//pvContent = DEV_RECORD*
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLRangeRec(const char* cszDevAddr, const DEV_DL_RECORD_RANGE& cRange, int nFlag, BOOL bKnowRange = FALSE, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���ر�����¼����
//����:
//cszDevAddr �豸��ַ
//nSeq ��������
//˵��: DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_ALARM_OPER
//eFlag = DEV_OPER_DOWNLOAD��
//pvContent = DEV_DL_RECORD_ARRAY*
//�ο�DEV_DL_RECORD_ARRAY�ṹ��������¼ֻ��һ������
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Downloading interval alarm record
//Parameter:
//cszDevAddr Device address
//nSeq Operation sequence
//Comment: DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_ALARM_OPER
//eFlag = DEV_OPER_DOWNLOAD��
//pvContent = DEV_DL_RECORD_ARRAY*
//Refer to DEV_DL_RECORD_ARRAY structure��the alarm record has only one interval
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLAllAlarmRecord(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����ָ��ʱ��α�����¼
//����: cszDevAddr �豸��ַ
//cStart ��ʼ����
//cEnd ��ֹ����
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Downloading specific time interval alarm record
//Parameter: cszDevAddr Device address
//cStart start date
//cEnd end date
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLSegTimeAlarmRecord(const char* cszDevAddr, const DEV_DATE& cStart, const DEV_DATE& cEnd, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �������䱨����¼
//����: cszDevAddr �豸��ַ
//cRange ID����
//flag DL_DEV_RECORD_PIC=��Ҫ���ؼ�¼��Ƭ 0x00=��������Ƭ, 0x01������Ƭ
//bKnowRange ָ�������ؾ������ݻ�������ID����(true=�������ݣ� false=ID����)
//˵��: �˽ӿ����ڴ�ĳһ����ȷ�ļ�¼ID��ʼ����nRang������
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: downloading interval alarm record
//Parameter: cszDevAddr Device address
//cRange ID interval
//flag DL_DEV_RECORD_PIC= need to download record picture 0x00= not download picture, 0x01download picture
//bKnowRange to appoint to download specific data or ID set (true=specific data�� false=ID set)
//Comment: this port is used to download from a specific record to download nRang data
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLRangeAlarm(const char* cszDevAddr, const DEV_DL_RECORD_RANGE& cRange, int nFlag, BOOL bKnowRange = FALSE, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����������־
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Download all logs
//Parameter: cszDevAddr Device address
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLLog(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����������־
//����: cszDevAddr �豸��ַ
//cRange ID����
//bKnowRange ָ�������ؾ������ݻ�������ID����(true=�������ݣ� false=ID����)
//˵��: �˽ӿ����ڴ�ĳһ����ȷ�ļ�¼ID��ʼ����nRang������
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Download interval logs
//Parameter: cszDevAddr Device address
//cRange ID interval
//bKnowRange to appoint to download specific data or ID set (true=specific data�� false=ID set)
//Comment: this port is used to download from a specific record ID to download nRang data
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLRangeLog(const char* cszDevAddr, const DEV_DL_RECORD_RANGE& cRange, BOOL bKnowRange = FALSE, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ȡ���ע���ռ�¼����
//����:
//cszDevAddr �豸��ַ
//nSeq ��������
//˵��:
//DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_CHANGEIMAGE_OPER
//eFlag = DEV_OPER_DOWNLOAD��
//pvContent = DEV_DL_RECORD_ARRAY*
//�ο�DEV_DL_RECORD_ARRAY�ṹ�����ע����ֻ��һ������
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Acquire the record interval of modified registration pictures
//Parameter:
//cszDevAddr Device address
//nSeq Operation sequence
//Comment:
//DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_CHANGEIMAGE_OPER
//eFlag = DEV_OPER_DOWNLOAD��
//pvContent = DEV_DL_RECORD_ARRAY*
//Refering to DEV_DL_RECORD_ARRAY structure, the modified registration pictures have only one interval
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLChangeImage(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����ָ������ı��ע���ռ�¼���߲�ѯ�������ݵľ����С
//����:
//cszDevAddr �豸��ַ
//cRange ����, �ο����䶨��
//flag DL_DEV_RECORD_PIC=��Ҫ���ؼ�¼��Ƭ
//bKnowRange true=�������ݣ� false=���伯��
//nSeq �������к�
//˵��: ��bKnowRange Ϊtrue�ú������ؾ�������ݣ�Ϊfalseʱ��ѯָ���������豸�ϵľ����С��
//��������ʱ
//DevOperResultNotify�ص�������Ӧ���ͣ�
//pvContent = DEV_DL_RECORD_ARRAY*
//cRange.m_SID = ָ��SN
//cRange.m_Count = -1;
//cRange.m_bOpenRange = TRUE;
//bKnowRange = FALSE��flag = 1����Ч������
//���ؾ����¼ʱ
//DevOperResultNotify�ص�������Ӧ���ͣ�
//pvContent=DEV_RECORD*
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Downloading the record of modified registration pictures in specific interval or querying interval data size
//Parameter:
//cszDevAddr Device address
//cRange  Interval, refer to the definition of interval
//flag DL_DEV_RECORD_PIC=download recording pictures
//bKnowRange true=specific data�� false=set of interval
//nSeq  Operation sequence
//Comment: If bKnowRange is true, the function downloading specific data, if it is false, the function querying the real size of the specific interval in the device.
//Downloading interval
//DevOperResultNotify Callback parameter corresponding type:
//pvContent = DEV_DL_RECORD_ARRAY*
//cRange.m_SID = appointed SN
//cRange.m_Count = -1;
//cRange.m_bOpenRange = TRUE;
//bKnowRange = FALSE��flag = 1��Invalid parameter��
//Downloading specific record
//DevOperResultNotify Callback parameter corresponding type:
//pvContent=DEV_RECORD*
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLRangeCImage(const char* cszDevAddr, const DEV_DL_RECORD_RANGE& cRange, int nFlag, BOOL bKnowRange = FALSE, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����ʱ����
//����: cszDevAddr �豸��ַ
//cTGArray ʱ����
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Set time group
//Parameter: cszDevAddr Device address
//cTGArray time group
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULTimeGroup(const char* cszDevAddr, const DEV_TIMEGROUP_ARRAY& cTGArray, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ȡʱ����
//����: dev_addr �豸��ַ
//cTGArray ʱ����
//eType ʱ��������
//˵��: cTGArray.m_nCount=0ʱ��ʾ����eType���͵�����ʱ����
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Get time group
//Parameter: dev_addr Device address
//cTGArray time group array
//eType time group type
//Comment: cTGArray.m_nCount=0 means to download all the eType time groups
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLTimeGroup(const char* cszDevAddr, const DEV_TIMEGROUP_ARRAY& cTGArray, DEV_TIMEGROUP_TYPE eType, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ɾ��ʱ����
//����: cszDevAddr �豸��ַ
//cTGArray ʱ����
//eType ʱ��������
//˵��: eType.m_nCount=0ʱ��ʾɾ��tg_type��������ʱ����
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Deleting time group
//Parameter: cszDevAddr Device address
//cTGArray Time group
//eType  Time group type
//Comment: eType.m_nCount=0 means deleting all tg_type time groups
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DELTimeGroup(const char* cszDevAddr, const DEV_TIMEGROUP_ARRAY& cTGArray, DEV_TIMEGROUP_TYPE eType, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����Ȩ��
//����: cszDevAddr �豸��ַ
//cRTArray Ȩ��
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Set access
//Parameter: cszDevAddr Device address
//cRTArray Access
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULRight(const char* cszDevAddr, const DEV_RIGHT_ARRAY& cRTArray, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���ͻ�ȡȨ��
//����: cszDevAddr �豸��ַ
//cRTArray Ȩ��
//˵��: ��rt_array.m_nCount=0ʱ��ʾ��������Ȩ��
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: sending acquire access
//Parameter: cszDevAddr Device address
//cRTArray access
//Comment:  rt_array.m_nCount=0 means downloading all access
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLRight(const char* cszDevAddr, const DEV_RIGHT_ARRAY& cRTArray, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ɾ��Ȩ��
//����: cszDevAddr �豸��ַ
//cRTArray Ȩ��
//˵��: ��rt_array.m_nCount=0ʱ��ʾɾ������Ȩ��
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Deleting access
//Parameter: cszDevAddr Device address
//cRTArray  Access
//Comment: rt_array.m_nCount=0 means deleting all access
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DELRight(const char* cszDevAddr, const DEV_RIGHT_ARRAY& cRTArray, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �����û���
//����: cszDevAddr �豸��ַ
//cUGrray �û���
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: setup user group
//Parameter: cszDevAddr Device address
//cUGrray user group
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULUserGroup(const char* cszDevAddr, const DEV_USERGROUP_ARRAY& cUGrray, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���ͻ�ȡ�û���
//����: cszDevAddr �豸��ַ
//ug_array �û���
//˵��: ��rt_array.m_nCount=0ʱ��ʾ���������û���
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: sending acquire user group
//Parameter: cszDevAddr Device address
//ug_array user group
//Comment: rt_array.m_nCount=0 means downloading all user groups
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLUserGroup(const char* cszDevAddr, const DEV_USERGROUP_ARRAY& cUGrray, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ɾ���û���
//����: cszDevAddr �豸��ַ
//ug_array �û���
//˵��: ��rt_array.m_nCount=0ʱ��ʾɾ�������û���
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function:  Deleting user group
//Parameter: cszDevAddr Device address
//ug_array user group
//Comment: rt_array.m_nCount=0 means deleting all user groups
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DELUserGroup(const char* cszDevAddr, const DEV_USERGROUP_ARRAY& cUGrray, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���ÿ�����״̬
//����:
//cszDevAddr �豸��ַ
//bEOpen true=�������ţ� false=��������
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Set door open&close status
//Parameter:
//cszDevAddr Device address
//bEOpen true=urgent open�� false=urgent close
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULOpenDoorState(const char* cszDevAddr, DEV_DOOR_STATE eState, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ȡ������״̬
//����: cszDevAddr �豸��ַ
//bEOpen true=�������ţ� false=��������
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: acquire door status open/close
//Parameter: cszDevAddr Device address
//bEOpen true=urgent open�� false=urgent close
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLOpenDoorState(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: Զ�̿���
//����: cszDevAddr �豸��ַ
//bEOpen true=���ţ� false=����
//pDoorArray �ŵ�����
//size �����С
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Remote Open Door
//Parameter: cszDevAddr Device address
//bEOpen true=Open��false=Close
//pDoorArray door point array
//size array size
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULRemoteOpenDoor(const char* cszDevAddr, BOOL bOpen, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: IO�豸����
//����:
//cszDevAddr �豸��ַ
//DEV_IOCTRL ������Ϣ
//nSeq ����ִ�����к�
//˵��: DEV_IOCTRL:m_Sourceָ��Ҫ�������Ķ˿ڣ�DEV_IOCTRL:m_ActionҪִ�еĶ���
//����1��m_Source = 0x1f m_Action = DEV_ACT_IO_OPEN
//����1��m_Source = 0x15 m_Action = DEV_ACT_IO_OPEN
//0x1f��015������ô�����ģ���ο�485��ַ����
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: IO device control
//Parameter:
//cszDevAddr Device address
//DEV_IOCTRL control information
//nSeq order execute serial number
//Comment: DEV_IOCTRL:m_Source to point operational port��DEV_IOCTRL:m_Action to operate actions
//Open door 1��m_Source = 0x1f m_Action = DEV_ACT_IO_OPEN
//Open auxiliary 1��m_Source = 0x15 m_Action = DEV_ACT_IO_OPEN
//Refer to 485 address resolution to resolute 0x1f and 015
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_IOCtrl(const char* cszDevAddr, const DEV_IOCTRL& ioctrl, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ʽ���豸
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Format the device
//Parameter: cszDevAddr Device address
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULFormat(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �豸�������
//����: cszDevAddr �豸��ַ
//cAppData ������½ṹ��
//˵��: ���� 0�ɹ�  1���緢��ʧ�� 2 ��֤����֤ʧ��
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: device program update
//Parameter: cszDevAddr Device address
//cAppData program update structure
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
int SLGDEV_CALL SLG_ULUpdate(const char* cszDevAddr, const DEV_APPUPDATE& cAppData, int nSeq = -1);
////////////////////////////////////////////////////////////////////////////////////////
//����: ������Ƶ
//����: cszDevAddr �豸��ַ
//hwnd ��Ƶ��ʾ���ھ��
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Start Video
//Parameter: cszDevAddr Device address
//hwnd Video display window handle
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
typedef struct VideoPro_ VideoPro;
typedef int (SLGDEV_CALL* VideoPro_CallBack)(const VideoPro* pro);
struct VideoPro_
{
    SOCKET m_hVideoHandle;
    unsigned char* m_pRGBTMP;
    unsigned char* m_pRGBTRANS;
    void* m_hWnd;
    int biWidth;
    int biHeight;
    addr_in m_Addr;
    VideoPro_CallBack VideoCB;
};
BOOL SLGDEV_CALL SLG_StartVideo(const char* cszDevAddr, void* hwnd, VideoPro_CallBack VideoCB, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ֹͣ��Ƶ
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Stop video
//Parameter: cszDevAddr Device address
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_StopVideo(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �����ϴ��û�
//����:
//cszDevAddr �豸��ַ
//cUsers �����û�����
//nFlag DL_DEV_PIC //��Ƭ
//DL_DEV_USER_FEAT //��������
//DL_DEV_USER_TEXT //�û�������Ϣ
//˵��:: �����ϴ��û���������û�������Ϣ���������
//nFlag = DL_DEV_USER_TEXT �ϴ�������Ϣ
//nFlag = DL_DEV_USER_TEXT|DL_DEV_PIC ����+ע����
//nFlag = DL_DEV_USER_TEXT|DL_DEV_USER_FEAT ����+������
//nFlag = DL_DEV_USER_TEXT|DL_DEV_PIC|DL_DEV_USER_FEAT����+ע����+������
//DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_USER_EX_OPER
//eFlag = DEV_OPER_UPLOAD ��
//pvContent = DEV_BATCH_USER*��
//��OPER_SUCCESS ����£������ϴ����û������뷵�ص��û�����һ�¡������豸����ʧ�ܡ�
//�����ϴ��û����趨��5������(������������������ٶ��й�)��
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Batch uploading users
//Parameter:
//cszDevAddr Device address
//cUsers batch users data
//nFlag DL_DEV_PIC //picture
// DL_DEV_USER_FEAT //facial features
// DL_DEV_USER_TEXT //text
//Comment:: Text must be included by batch uploading users, of which the combinations are:
//nFlag = DL_DEV_USER_TEXT uploading text
//nFlag = DL_DEV_USER_TEXT|DL_DEV_PIC text+reg. picture
//nFlag = DL_DEV_USER_TEXT|DL_DEV_USER_FEAT  text+face library
//nFlag = DL_DEV_USER_TEXT|DL_DEV_PIC|DL_DEV_USER_FEA Text+reg. picture+face library
//DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_USER_EX_OPER
//eFlag = DEV_OPER_UPLOAD ��
//pvContent = DEV_BATCH_USER*��
//In OPER_SUCCESS condition, the amount of batch uploading users is the same as the returned users. Or the device saves failure.
// The amount of batch uploading users is limited to 5 (it is related to the network bandwidth and mainframe processing speed).
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ULUserEx(const char* cszDevAddr, const DEV_BATCH_USER& cUsers, int nFlag, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���������û�
//����:
//cszDevAddr �豸��ַ
//pUserIDArray �û�ID�����׵�ַ
//nIDCount ����Ԫ�ظ���
//nFlag ͬSLG_ULUserEx
//˵��: DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_USER_EX_OPER
//eFlag = DEV_OPER_DOWNLOAD ��
//pvContent= DEV_BATCH_USER*��
//OPER_SUCCESS ����£�����������ص��û�����Ҫ�����ص��û�����һ�£�����ͨ��
// SLG_DLAllUser(const char* cszDevAddr, int nSeq = -1) ��ȷ���û��Ƿ���Ĵ������豸�ϡ�
//���������û����趨��5������(������������������ٶ��й�)��
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Batch Downloading Users
//Parameter:
//cszDevAddr Device address
//pUserIDArray  User ID array start address
//nIDCount Amount of array elements
//nFlag  with SLG_ULUserEx
//Comment: DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_USER_EX_OPER
//eFlag = DEV_OPER_DOWNLOAD ��
//pvContent= DEV_BATCH_USER*��
//In OPER_SUCCESS condition��if the amount of batch downloading users is not the same as the amount of requested downloading users,
// it can use SLG_DLAllUser(const char* cszDevAddr, int nSeq = -1) to confirm whether the user exists in the device.
// The amount of batch downloading users is limited to 5 (it is related to the network bandwidth and mainframe processing speed).
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLUserEx(const char* cszDevAddr, DEV_CID* pUserIDArray, int nIDCount, int nFlag, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ����ɾ���û�
//����:
//cszDevAddr �豸��ַ
//pUserIDArray ��ɾ���û��ı������
//nIDCount ����Ԫ�ظ���
//˵��: DevOperResultNotify�ص�������Ӧ���ͣ�
//eType = DEV_USER_EX_OPER
//eFlag = DEV_OPER_DELETE��
//pvContent = DEV_BATCH_USER*��
//��OPER_SUCCESS ����£�����ɾ�����û������뷵�ص��û�������һ�£�
//����ͨ�� SLG_DLAllUser(const char* cszDevAddr, int nSeq = -1)
//��ȷ���û��Ƿ���Ĵ������豸�ϡ���������ɾ���û����趨��10������Ϊ�á�
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Batch Deleting Users
//Parameter:
//cszDevAddr Device address
//pUserIDArray Waiting for deleting users serial number array
//nIDCount  The amount of array elements
//Comment: DevOperResultNotify Callback parameter corresponding type:
//eType = DEV_USER_EX_OPER
//eFlag = DEV_OPER_DELETE��
//pvContent = DEV_BATCH_USER*��
//In OPER_SUCCESS condition��if the amount of batch deleting users is not the same as the amount of requested deleting users,
// it can use SLG_DLAllUser(const char* cszDevAddr, int nSeq = -1)
//to confirm whether the user exists in the device. The amount of batch deleting users is limited to 10
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DELUserEx(const char* cszDevAddr, DEV_CID* pUserIDArray, int nIDCount, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �������ؼ�¼
//����: cszDevAddr �豸��ַ
//cRange ID����
//˵��: ������֤��¼��������¼��������־�����ע����
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function:  batch download records
//Parameter: cszDevAddr Device address
//cRange ID interval
//Comment: including verification records, alarm records ,operation log and reg. image changing records
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_DLRecEx(const char* cszDevAddr, const DEV_DL_RECORD_RANGE& cRange, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: �����豸
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Restart device
//Parameter: cszDevAddr Device address
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_RebootDev(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���/���� �豸����������Ϣ
//����: cszDevAddr �豸��ַ
//pNetPro NULL��ʾ��ã���Ϊ�ձ�ʾ����
//˵��:: ��������ipv6��ַ
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: Acquire/setup device network information
//Parameter: cszDevAddr Device address
// pNetPro NULL means to acquire, NOT NULL means to setup
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_NetPro(const char* cszDevAddr, DEV_NETPRO* pNetPro, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ��ĻУ��
//����: cszDevAddr �豸��ַ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function: screen verify
//Parameter: cszDevAddr Device address
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
BOOL SLGDEV_CALL SLG_ResetScreen(const char* cszDevAddr, int nSeq = -1);

////////////////////////////////////////////////////////////////////////////////////////
//����: ���ò���
//����: type �������� �� SLG_DEBUG (value=[0 1])
//value ������ֵ
//˵��:
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//Function:  setup Parameter
//Parameter: type Parameter type, as SLG_DEBUG (value=[0 1])
// value  the value of Parameter
//Comment:
////////////////////////////////////////////////////////////////////////////////////////
#define SLG_DEBUG -1 //��ӡ������Ϣ
long SLGDEV_CALL SLG_SetLong(long type, long value);

////////////////////////////////////////////////////////////////////////////////////////
// SetWapInfo ���ڻ�ȡ������802.1x��֤��ص���������������/�ر�802.1x��֤���û���������
// ���ڻ�ȡ������802.1x��֤��ص���������������/�ر�802.1x��֤���û��������롣��Ӧ������
//��ΪS_GET��S_SET�����óɹ�ʱ���� SUC_EXECUT.
//�����ù����ڸ߼�������ʱֻ�ܻ�ȡ���������á��豸�ڸ߼�������ʱ��������ز����᷵��ERR_BUSY
////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    int  isopen;      //WPA���أ������� sizeof��int) 0��ʾ�رգ�1��ʾ����
    char username[32];// �û������ַ����� 32�ֽ�ASCII���ַ��� �ܳ���32�ֽڣ���Ч�������31���ֽڡ���'\n'��ռ��һ���ֽڡ�
    char password[32];// ���루�ַ����� 32�ֽ�ASCII���ַ��� �ܳ���32�ֽڣ���Ч�������31���ֽڡ���'\n'��ռ��һ���ֽڡ�
} WAPINFO;
BOOL SLGDEV_CALL SLG_SetWapInfo(const char* cszDevAddr, const WAPINFO* pWapInfo, int nSeq);
BOOL SLGDEV_CALL SLG_GetWapInfo(const char* cszDevAddr, int nSeq);

////////////////////////////////////////////////////////////////////////////////////////
// end
EXTERN_C_END
#endif
