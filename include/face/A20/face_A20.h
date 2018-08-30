
// ���ݰ�(data packet)��֡�ṹ(���ݰ��ܳ���С�� 65536 (1<<16) �ֽ�)
//   PREFIX  LEN  CKS CMD ERR SID DID DATA
// 0xC5 0x5C L H  L H L H L H L H L H   n
// 

// ������ Command
#define COMMAND_DEF_DEF(COMMAND_DEF) \
COMMAND_DEF(CMD_TEST_CONNECTION,�������豸��ͨѶ����)\
COMMAND_DEF(CMD_SET_PARAM,�����豸����)\
COMMAND_DEF(CMD_GET_PARAM,��ȡ�豸����)\
COMMAND_DEF(CMD_GET_DEVICE_INFO,��ȡ�豸��Ϣ)\
COMMAND_DEF(CMD_ENTER_IAP_MODE,���豸����ΪIAP״̬)


enum {
#define COMMAND_DEF(a, b) a,
COMMAND_DEF_DEF(COMMAND_DEF)
#undef COMMAND_DEF
};

// ������ Error
#define ERROR_DEF_DEF(ERROR_DEF) \
ERROR_DEF(ERR_SUCCESS,0x00,ָ���ɹ�)\
ERROR_DEF(ERR_FAIL,0x01,ָ���ʧ��)\
ERROR_DEF(ERR_VERIFY,0x10,��ָ�������Template��1:1�ȶ�ʧ��)\
ERROR_DEF(ERR_DUPLICATION_ID,0x18,��������ע��)\
ERROR_DEF(ERR_BAD_QUALITY,0x19,����ͼ����������)\
ERROR_DEF(ERR_MEMORY,0x1C,�ⲿFlash��д����)\
ERROR_DEF(ERR_INVALID_TMPL_NO,0x1D,ָ��Template�����Ч)\
ERROR_DEF(ERR_INVALID_PARAM,0x22,ʹ���˲���ȷ�Ĳ���)\
ERROR_DEF(ERR_GEN_COUNT,0x25,�����ϳɸ�����Ч)\
ERROR_DEF(ERR_INVALID_BUFFER_ID,0x26,BufferIDֵ����ȷ)\
ERROR_DEF(ERR_FP_NOT_DETECTED,0x28,�ɼ�����û����������)

enum {
#define ERROR_DEF(a, b, c) a = b,
  ERROR_DEF_DEF(ERROR_DEF)
#undef ERROR_DEF
};