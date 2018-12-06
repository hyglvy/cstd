#ifndef _LIBFACERECOG_H_
#define _LIBFACERECOG_H_
#include <stdio.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" //
{
#endif

#include "stdc/img_c.h"
#include "stdc/objdetect.h"
#if 0
#define FEAT_T unsigned char
#define FEAT_MAX 255
#define FEAT_WORK_T int
#else
#if 1
#define FEAT_T unsigned char
#define FEAT_WORK_T unsigned int
#define FEAT_MAX 255
#else
#define FEAT_T float
#define FEAT_WORK_T float
#define FEAT_MAX 65532.f
#endif
#endif

  /*
  ���������
  p.cas[1] = pNIRDoubleEYE;
  p.stepxy = 1, p.mincnt = 1, p.fastmode = 0, p.maxoutlen = 50, p.casnum = 1, p.cas[ 0 ] = pvis_nesting_face0701;
  p.isc = FR_F2I( 0.25f ), p.iss = FR_F2I( 1.1f ), p.ithd = FR_F2I( 0.8f ), p.issmin = FR_F2I( 1.f ), p.issmax = FR_F2I( 100.f );
  p.leyecasnum = 3, p.leyecas[ 0 ] = pNIREYEWithoutGlass, p.leyecas[ 1 ] = pNIREYELeftWithGlass , p.leyecas[ 2 ] = pNIRAllEYE;
  p.reyecasnum = 3, p.reyecas[ 0 ] = pNIREYEWithoutGlass, p.reyecas[ 1 ] = pNIREYERightWithGlass, p.reyecas[ 2 ] = pNIRAllEYE;
  */
#define STDIMGH   (100)
#define STDIMGW   (100)
#define FEAT_SIZE1 (2443)
#define FEAT_SIZE2 (2165)
  //#define FEAT_SIZE3 (1332) // ��ֵ=6372 ���=16645
  //#define FEAT_SIZE3 (1670)
  //#define FEAT_SIZE3 (2093)
#define FEAT_SIZE3 (2566) // ��ֵ=11047 ���=27113
#define FR_FACE_DETECT   (1<<0)
#define FR_EYES_DETECT   (1<<1)
#define FR_FACE_STD      (1<<2)
#define FR_FACE_FEATURE  (1<<3)
#define FR_FACE_RECOG    (1<<4)
#define FR_CVTCOLOR      (1<<5)
#define FR_IMRESIZE      (1<<6) // 
#define FR_ODDFIX        (1<<8) // ��ż��
#define FR_ENCODE        (1<<9) // ���� ����[featlib, ��Сfeatstep] ���[feature, featurelen] �����С����>2*featstep
#define FR_DECODE        (1<<10) // ����
#define FR_RECOG_TEST    (1<<11) // ����ʶ��ԱȲ��� ����ȥ�����ԳƵ���� maxdist �������ֵ ��� maxdist>25000*0.59 �򲻽�������Ĳ��裬���Ѽ�����0
#define FR_FEAT_23       (1<<12) // ʹ��2165+1332 �ĸ������� p.leye.count ==id, p.leye.score==�÷�
#define FR_FACE_ALIGNMENT   (1<<13) // ʹ��2165+1332 �ĸ������� p.leye.count ==id, p.leye.score==�÷�
#define FR_HASH_FEATURE  (1<<14)  // ��ϣ����
#define FACERECOG_MODE_SORT_DIST   (1<<3)
#ifndef MAX_CAS_NUM
#define MAX_CAS_NUM (4)
#endif
  typedef struct facerecog_param_t {
    //����
    //������
    int height, width, /*ͼ��ĸ߿�*/
        outheight, outwidth, /*���ͼ��ĸ߿�*/
        datastep, maskstep, outimgstep, /*ͼ����п�*/
        datacn, maskcn, outimgcn, /*ͼ���ͨ��*/
        datacode, outimgcode, /* ͼ��ı������� Ʃ�磺T_YUYV */
        stepxy, mincnt, maxoutlen, casnum, fastmode, /*����������*/
        code, transopt;
    /*stepxy ��������λ���� mincnt �ϲ�����С�ڴ˴�������� casnum ����������*/
    /* ssmin ��С������,ssmax��������,ss �������ű��� */
    /*cs ���ű���, thd ��������ֵ */
#if 0
    real sc, ssmin, ssmax, ss, thd;
#else
    // ����ת�����븡��ת����ĺ�
#define FR_I2F(_X)  (double)((_X)/100.f)
#define FR_F2I(_X)  (int)((_X)*100)
    // ��Ӧ�ĸ���������100 ������ԭsc=0.12 ics=0.12*100��
    int isc, issmin, issmax, iss, ithd;
#endif
    unsigned char* data, *mask;
    unsigned char* outimg; // ����ת�����ͼ��
    void const* cas[MAX_CAS_NUM]; // ����������
    int deyecasnum, leyecasnum, reyecasnum;
    void const* deyecas[MAX_CAS_NUM];// ˫�۷�������
    void const* leyecas[MAX_CAS_NUM];// ���۷�������
    void const* reyecas[MAX_CAS_NUM];// ���۷�������
    XRECT roi;
    int featstart, featnum, featstep; // �ӵڼ���������ʼ��������������������
    const FEAT_T* featlib; // ������
    int mode; // ģʽѡ��
    //int* index;
    //���
    XRECT face, deye, leye, reye; // [������, ˫�ۿ�, ���ۿ�, ���ۿ�]
    unsigned char* stdface; // 100��100�Ĺ�һ������
    int featurelen; // ������С
    FEAT_T* feature; // ��������
    int distlen; // ==featnum
    int* dist; // �����б�
    int maxdist;
    int maxdist_id;
    //int id[10];
    //int ds[10];
    int buflen;
    unsigned char* buf; // ��ʱ�ռ��С
  } facerecog_param_t;
  
  int facerecog_memsize(void* p0);
  int facerecog_process(void* p0);
  void UYVY_to_YUY2(int n, unsigned char* in_Y, unsigned char* out_Y);
  void YUY2_to_UYVY(int n, unsigned char* in_Y, unsigned char* out_Y);
  void face_recog_open();
  void face_recog_close();
  int face_recog_process(void* p0);
  void* dsp_malloc(size_t n);
  void dsp_free(void* buf);
  void* dspmalloc(size_t n);
  void dspfree(void* p, size_t n);
  // maxFrameRate ����ԽСѹ����Խ�ߣ�Ĭ�� 25000
  int venc1_create(int height, int width, int maxFrameRate, int maxBitRate);
  int venc1_process(const unsigned char* src, int slen, unsigned char* dst, int dlen, int type);
  int venc1_delete();
  int vdec2_create(int height, int width);
  int vdec2_process(const unsigned char* src, int slen, unsigned char* dst, int dlen, int type);
  int vdec2_delete();
  int Vdec2a_create(int h, int w);
  int Vdec2a_delete();
  int Vdec2a_process(const char* hInBuf, int hInBufSz, char* hDstBuf, int hDstBufSz);
  typedef int (*decode_callback_fun_t)(unsigned char* data, int w, int h);
  int decode264(const char* filename, decode_callback_fun_t fun);
  // G711���������㷨
  int Senc1_create();
  int Senc1_delete();
  // ѹ�����С=Senc1_process(ԭʼ����ָ��, ԭʼ���ݴ�С, Ŀ��ռ�ָ��, Ŀ��ռ��С, 0);
  int Senc1_process(const unsigned char* src, int slen, unsigned char* dst, int dlen, int type);
  // G711���������㷨
  int Sdec1_create1();
  int Sdec1_delete1();
  // ������С=Sdec1_process(ԭʼ����ָ��, ԭʼ���ݴ�С, Ŀ��ռ�ָ��, Ŀ��ռ��С, 0);
  int Sdec1_process1(const unsigned char* src, int slen, unsigned char* dst, int dlen, int type);

#ifdef __cplusplus
}
#endif
#endif // _LIBFACERECOG_H_

