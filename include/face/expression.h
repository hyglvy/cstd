
#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#ifdef __cplusplus
extern "C" {;
#endif

enum { ET_EFFECT1 = 0x01,   // ������
       ET_EFFECT2 = 0x02,   // ���
       ET_EFFECT3 = 0x04,   // ��ǰ��
       ET_EFFECT4 = 0x08,   // �����ɽ�
       ET_EFFECT5 = 0x10,   // ������
       ET_EFFECT6 = 0x20,   // �绯Ч��
       ET_EFFECT7 = 0x40,   // ��״Ч��
       ET_EFFECT8 = 0x80    // ��������ߣ�����ָ��һ����������ͼƬ
};

typedef struct tagEFFECTA {
  int height, width;     // ͼ��ĸ߿�
  int type;              // ������ ET_EFFECT1|ET_EFFECT2|ET_EFFECT3���������
  int picidx;
  int splash_radius;     // �����ɽ��뾶
  int mosaic_size;       // �����˴�С
  int windy_step;        // �绯�̶�
  int stripe_swing;      // ��״���[0..100]
  int stripe_frequency;  // ��״Ƶ�� >=0
  float eyes_sphere;     // �۾����滯����[-1..1]
  float face_sphere;     // �������滯����[-1..1]
  CvPoint leye, reye;
  char picspec[ 256 ];
  int fg_h;              // ǰ��ͼƬ��
  int fg_w;              // ǰ��ͼƬ��
  int fg_cn;             // ǰ��ͼƬͨ����
  unsigned char* foreground;  // ǰ��ͼƬ
  int starface_height, starface_width;
  unsigned char* starface;  // ������ͼƬ
}
EFFECTA;
typedef struct tagEFFECTW {
  int height, width;     // ͼ��ĸ߿�
  int type;              // ������ ET_EFFECT1|ET_EFFECT2|ET_EFFECT3���������
  int picidx;
  int splash_radius;     // �����ɽ��뾶
  int mosaic_size;       // �����˴�С
  int windy_step;        // �绯�̶�
  int stripe_swing;      // ��״���[0..100]
  int stripe_frequency;  // ��״Ƶ�� >=0
  float eyes_sphere;     // �۾����滯����[-1..1]
  float face_sphere;     // �������滯����[-1..1]
  CvPoint leye, reye;
  wchar_t picspec[ 256 ];
  int fg_h;              // ǰ��ͼƬ��
  int fg_w;              // ǰ��ͼƬ��
  int fg_cn;             // ǰ��ͼƬͨ����
  unsigned char* foreground;     // ǰ��ͼƬ
  int starface_height, starface_width;
  unsigned char* starface;  // ������ͼƬ
}
EFFECTW;

#ifdef UNICODE
#define EFFECT  EFFECTW
#define PEFFECT  EFFECTW*
#else
#define EFFECT  EFFECTA
#define PEFFECT  EFFECTA*
#endif // !UNICODE
/*
ʹ�����ӣ�
PEFFECT expr = new_expression ();
config_expression (expr, ET_EFFECT3, _T ("�����ļ�.ini"), _T ("���1"));
add_expression (expr, h, w, img, al, ai);
del_expression (&expr); // ʹ������Ҫ�ͷ�
assert (NULL==expr);    // ȷ��ָ��Ϊ��
*/ 
// �������ṹ
EFFECTA* new_expressionA(int height, int width);
EFFECTW* new_expressionW(int height, int width);

// �ͷű���ṹ
// pexpr               - ָ��PEFFECT�ṹ��ָ��
void del_expressionA( EFFECTA** pexpr );
void del_expressionW( EFFECTW** pexpr );

// ���ñ���
// expr                - ָ��EFFECT�ṹ��ָ��
// type                - ������ ET_EFFECT1|ET_EFFECT2|ET_EFFECT3���������
// inifile             - ini�ļ�·��
// effect_name         - ini�ļ��������е��ַ���
void config_expressionA( EFFECTA* expr, int type, const char* inifile, const char* effect_name );
void config_expressionW( EFFECTW* expr, int type, const wchar_t* inifile, const wchar_t* effect_name );
// �ӱ���
// expr                - ָ��EFFECT�ṹ��ָ��
// [h, w, img, al, ai] - ͼƬ[��,��,���Ͻ�ָ��,������,ͨ����]
int add_expressionA( EFFECTA* expr, unsigned char * img, int al, int ai );
int add_expressionW( EFFECTW* expr, unsigned char * img, int al, int ai );
#ifdef UNICODE
#define new_expression new_expressionW
#define del_expression del_expressionW
#define config_expression  config_expressionW
#define add_expression  add_expressionW
#else
#define new_expression new_expressionA
#define del_expression del_expressionA
#define config_expression  config_expressionA
#define add_expression  add_expressionA
#endif // !UNICODE


#ifdef __cplusplus
}
#endif

#endif // _EXPRESSION_H_
