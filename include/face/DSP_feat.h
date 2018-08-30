#ifndef _DSP_FEAT_H_
#define _DSP_FEAT_H_
#define ID_LEN 32
//#define FEATURE_LEN   ((FEAT_SIZE2*sizeof( FEAT_T)+3)&~3)      //��������������С
#define FEATURE_LEN   30000
#define FEATURE_LEN1 (FEAT_SIZE2+FEAT_SIZE3)*sizeof(FEAT_T)      //��������������С
//#define FEATURE_LEN 60000*sizeof(FEAT_T)      //��������������С
#define SPFEAT_LEN  1              //�����û�����������Ŀ(15����������5��������)
//#define REGIST_NUM  1              //�����û�����ע��ʱ�ɼ���������Ŀ

enum { FLAG_BASE = 0, FALG_UPDATE, FLAG_NULL = 255 };

//ID��ʶ�ṹ��
typedef struct {
  char m_ID[ ID_LEN ];
}CID;

//��������
typedef struct {
  FEAT_T m_Feature[ FEATURE_LEN ];  //��������
  uint arg[4];
  CID m_PID;                     //��ӦID��
  int m_Flag;                 //���»������,FLAG_BASE,FALG_UPDATE,FLAG_NULL 
  unsigned int hashid;
  time_t time;
}SPFeatInfo;

#define STDH 200
#define STDW 150
#define STDCN 3
#define STDSTEP (STDW*STDCN)
typedef struct {
  unsigned char m_data[STDH*STDW*STDCN];      //ע����Ƭ
  CID        m_PID;                     //��ӦID��
  unsigned int m_Flag;
}SPFaceRegInfo;

// ʶ���¼
typedef struct {
  unsigned char m_data[STDH*STDW*STDCN]; //ע����Ƭ
  CID        m_PID;                      //��ӦID��
  double score; // �÷�
  time_t time;
}SPFaceRecInfo;

//������������
#define SPInfoID(SP, _I)  ((SP)->m_PFeatArry[_I].m_PID.m_ID)
typedef struct {
  SPFeatInfo m_PFeatArry[ SPFEAT_LEN ];
}SPInfo;
static int CID_set(char* m_ID, const char* name1) {
  if (name1) {
    int len1 = strlen(name1);
    if (len1>0 && len1<ID_LEN) {
      memset(m_ID, 0, ID_LEN);
      strcpy(m_ID, name1);
      return 1;
    }
  }
  return 0;
}
#endif // _DSP_FEAT_H_
