
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if 0
#include "face_recog.inl"
#else
#pragma comment(lib,"libface_recog.lib") // dll
#endif
void free_image(void* data);
void* load_image(const char* fn, int cn_req, int* ph, int* pw, int* pstep);
int save_image(const char* fn, int h, int w, const void* data, int step, int cn);
// ���� -1 δע��
// ���� 0 û��⵽�������۾�
// ���� 1 ��⵽�������۾�
int face_recog(int h, int w, const void* data, int step, int cn, const char* s_pixtype, const char* s_mode, int is_trans, double scale,
               int* xywh, void* stdface, void* feature, const void* featlib, int featstep, int featnum, int* pid, int* pdis);

#ifndef _WIN32
#define _snprintf snprintf
#endif

int test_face_recog1() {
  char* path = NULL;
  char buf[256];
  int i, j, k, men_numb=3;
  int maxpics = 10; // ÿ�������ɼ�10��ͼƬ���ɸ�����Ҫ���á�
  int is_trans = 0; //ͼ���Ƿ���Ҫ��ת90�ȡ�
  double scale = 0.2; // ��һ��ͼ�����ű���
  int xywh[12] = {0}; // ���� ���� ���� ���ο�λ�� [x, y, w, h]
  char stdface[100*100]; // ��һ��ͼƬ
  typedef struct myfeat_t {
    char feat[4800];
    char userid[64];
  } myfeat_t;
  int ret, sz = sizeof(myfeat_t)*(men_numb*maxpics);
  myfeat_t* feats = (myfeat_t*)malloc(sz); // �����⡣
  char* data = NULL;
  int h=0, w=0, step=0, cn = 3;
  char* s_pixtype = "BGR"; // cn = 3
  memset(feats, 0, sz);
  path = "E:/pub/bin/face/face_recog";
  {
    // �ɼ�����(�����ļ�ͼƬ�ķ�ʽ��ʾԭ��ʵ��ʹ���п��Բ�����ƵͼƬ)
    for (i=0; i<men_numb; ++i) {
      for (j=0, k=0; k<maxpics; ++j) {
        myfeat_t* feature = feats + i * maxpics + k;
        _snprintf(buf, 256, "%s/%03d/pic_%02d.bmp", path, i+1, j);
        _snprintf(feature->userid, 64, "%03d", i); // �����û�id
        data = (char*)load_image(buf, cn, &h, &w, &step); // ��ȡͼƬ�ļ�
        if (NULL==data) {
          printf("ûͼƬ��\n");
        }
        // ���������������ȡ��������
        ret = face_recog(h, w, data, step, cn, s_pixtype, "REG", is_trans, scale,
          xywh, stdface, feature->feat, NULL, 0, 0, 0, 0); // �ɼ������в���Ҫ���������������
        printf("%d %d ", i, j);
        if (ret<0) {
          printf("δע��\n");
          free(feats);
          return 0;
        }
        if (ret) {
          _snprintf(buf, 256, "%s/std/std_%03d_%02d.bmp", path, i, j);
          save_image(buf, 100, 100, stdface, 100, 1);
          ++k;
          printf("OK\n");
        } else {
          printf("���ʧ��\n");
        }
        free_image(data);
      }
    }
  }
  {
    int id, dis;
    myfeat_t feattmp[1];
    int thds[] = {450, 470, 490}; // ��ֵ [�ϸ�, һ��, ����]
    // ʶ�����
    for (i=0; i<15; ++i) {
      _snprintf(buf, 256, "%s/test/file_%02d.bmp", path, i);
      data = (char*)load_image(buf, cn, &h, &w, &step);
      if (NULL==data) {
        printf("ûͼƬ��\n");
      }
      // ���������������ȡ��������
      ret = face_recog(h, w, data, step, cn, s_pixtype, "REC", is_trans, scale,
        xywh, stdface, feattmp->feat, feats->feat, sizeof(*feats), men_numb*maxpics, &id, &dis);
      if (ret) {
        printf("file_%02d id = %3d userid = %s ���� = %d %s\n", i, id, feats[id].userid, dis, dis<thds[1] ? "ͨ��" : "           �ܾ�");
      } else {
        printf("���ʧ��\n");
      }
    }
  }
  free(feats);
  return 0;
}
