
#ifndef _ASMFIT_INL_
#define _ASMFIT_INL_
//#include <stdlib.h>
//#include <assert.h>
#include "cstd.h"
// ��ǰ�Ƕ�λ��˫�ۣ��ٰ�ͼƬ���뵽˫�������ϡ�
// ˫�۶�λ�����������������ͼƬҲ������
// �����Ƕ�λ������101���ؼ������㣨����˫�ۣ�������ͣ����������ȵȣ�������߾͸��Ӿ�ȷ��
#define MAXSCANFBUFSIZE (1<<12)

#define HEADSIZE 76

typedef struct asmfit_param_t {
  int height, width, datastep;
  const unsigned char* data; /*???????????????????????*/
  IPOINT leye, reye; // ??????????????????��?????
  const void* asm_mode; // asm???????
  // 
  // 
  FPOINT* shape_result; //? Shape_result_len/2????x??????Shape_result_len/2????y????
  int shape_result_len; // ??????????????
  int* contours_ending_points; //?????????????????????[ContoursEndingPointsNum]
  int contours_ending_points_len;
  // ????????????????????, Shape_result????????????????buf?��?
  int buflen;// ??????????????�� ??asmfit_memsize??????????
  char* buf; // ?????????????(??????????��???>=buflen)
} asmfit_param_t;

typedef struct asm_t {
  //Ŀǰѵ���������ø߶�400���Ϊ320��ͼ��
  int max_iter; // = 5;//����ʱ������������(��������)
  int nPelsSearch; // = 3;//profileģ�͵���������profile�����������͸�������ƶ���(��������)
  int nQualifyingDisp; // = 95;//�����ж��Ƿ������Ĳ���
  float coef_lim; // = 1.8;//Ϊ��״����������ϵ��,����Ӧ��Ϊ1.8
  int train_image_height, train_image_width; // ѵ�������߶ȺͿ��
  int NbOfPoints; //������
  int contours_ending_points_len;
  int TrnPntsAbove, TrnPntsBelow; //profile�������Ϻ�������������
  int NumPyramidLevels; //ͼ��������ֲ���
  int if_filter; //ͼ��������ֲ�ʱ�Ƿ���ø�˹�˲�
  int shapes_eigen_values_len;
  int* contours_ending_points; //ÿ�����ٵ����һ���������[contours_ending_points_len]
  float* MeanShape; //ƽ����״,����Ϊx����Ϊy [2*NbOfPoints]
  float* shapes_eigen_values; //pca��״����ֵ[shapes_eigen_values_len]
  float* ShapesEigenVectors; //pca��״��������[(NbOfPoints * 2)*shapes_eigen_values_len]
  float* meanProfiles; //ƽ����һ��profile����[(NbOfPoints * NumPyramidLevels)*(TrnPntsAbove + TrnPntsBelow + 1)]
  float* covProfiles_inv; //profileЭ�������[( s->NbOfPoints * s->NumPyramidLevels ) * ( k * k )] k= (TrnPntsAbove + TrnPntsBelow + 1)
} asm_t;

static int GetBeforeAfterPts(int index, const int* _contours_ending_points,
    int contours_ending_points_len, int* pBefore, int* pAfter)
{
  //�������ܣ���õ�ǰ�������index��ǰ��������
  //������    indexΪ��ǰ�������
  //          contours_ending_points �洢ÿ�����ٵ����һ���������������˳��Ϊ��ü�����ۡ���ü�����ۡ����ӡ����졢���졢����+�°�,�Ǵӱ����ѵ�������ж�ȡ��
  //����ֵ:   pt1��pt2Ϊ��ǰ�������index��ǰ��������

  int i, end = contours_ending_points_len, first, last;
  int pt1 = -1, pt2 = -1;
#if 1

  if (10 == index) {
    int aa = 0;
  }

  for (i = 0; i < end && index >= _contours_ending_points[ i ]; ++i) {}

  first = (0 == i) ? 0 : (_contours_ending_points[ i - 1 ]);
  last = _contours_ending_points[ i ] - 1;

  if (index == first) {
    pt1 = last;
    pt2 = index + 1;
  }
  else if (index == last) {
    pt1 = index - 1;
    pt2 = first;
  }
  else {
    pt1 = index - 1;
    pt2 = index + 1;
  }

#else

  if (index == 1) {   //�����ǰ��Ϊ��һ����
    pt1 = contours_ending_points(1);   //�����һ����������������赱ǰ���ǰһ�㣨ѭ���ṹ��...5 1 2 3 4 5 1 2 3...)��contours_ending_pointsΪԤ�궨��ĸ���
    pt2 = 2;
  }
  else if (index == contours_ending_points(1)) {       //�����ǰ��Ϊ���һ����
    pt1 = contours_ending_points(1) - 1;
    pt2 = 1;
  }
  else if (index == contours_ending_points(end)) {
    pt1 = contours_ending_points(end) - 1;
    pt2 = contours_ending_points(end - 1) + 1;
  }
  else if (ismember(index - 1, contours_ending_points, contours_ending_points_len)) {       //ismember(a,b)���a��Ԫ���Ƿ���b�ĳ�Ա
    tmp = find(index - 1 == contours_ending_points);
    pt1 = contours_ending_points(tmp + 1);
    pt2 = index + 1;
  }
  else if (ismember(index, contours_ending_points)) {
    pt1 = index - 1;
    tmp = find(index == contours_ending_points);
    pt2 = contours_ending_points(tmp - 1) + 1;
  }
  else {
    pt1 = index - 1;
    pt2 = index + 1;
  }

#endif
  *pBefore = pt1;
  *pAfter = pt2;
  return 0;
}

// 1 5 8 5 1
/* 1/16[1    4    6    4    1]       */
/* ...| x0 | x1 | x2 | x3 | x4 |...  */
#define  PD_FILTER( x0, x1, x2, x3, x4 ) ((x2)*6+((x1)+(x3))*4+(x0)+(x4))
//#define SET_ROW_PD_FILTER(_B, _j, _b, x0, x1, x2, x3, x4) (_b = rowbuf + (_j), _B[_j] =PD_FILTER( _b[x0], _b[x1], _b[x2], _b[x3], _b[x4])>>8)
#define SET_ROW_PD_FILTER(_B, _j, _b, x0, x1, x2, x3, x4) (_b = rowbuf + (_j), _B[_j] =_b[x2]>>4)
#define SET_COL_PD_FILTER(_B, _j, _b, x0, x1, x2, x3, x4) (_b = A + (cn2*_j), _B[_j] =PD_FILTER( _b[x0], _b[x1], _b[x2], _b[x3], _b[x4]))

CC_INLINE int PyramidDown(int h, int w, unsigned char* B, int bl, const unsigned char* A, int al, int cn, int if_filter, void* buf)
{
  int i, j;
  int cn1 = cn;
  int cn2 = cn * 2;
  int al1 = al;
  int al2 = al * 2;
  const unsigned char* a = NULL;
  ushort* b = NULL;
  ushort* rowbuf = NULL;
  void* mybuf = NULL;
  ASSERT(bl > (w * cn / 2));

  if (!if_filter) {
    int alcn = al + cn;

    for (i = 0; i < h; ++i, A += al2, B += bl) {
      for (j = 0; j < w; ++j) {
        a = A + j * cn2;
#if 0

        B[ j ] = (a[ 0 ] + a[ cn ] + a[ al ] + a[ alcn ]) >> 2;
#else

        B[ j ] = (a[ 0 ]);
#endif

      }
    }

    return 0;
  }

  if (buf) {
    rowbuf = (ushort*)buf;
  }
  else {
    rowbuf = (ushort*)malloc(sizeof(ushort) * w);
    mybuf = rowbuf;
  }

  //0 line and h-1 line
  for (i = 0; i < h; ++i, A += al2, B += bl) {
    b = rowbuf;

    if (i > 1 && i < h - 2) {
      for (j = 0; j < w; ++j) {
        SET_COL_PD_FILTER(b, j, a, -al2, -al1, 0, al1, al2);
      }
    }
    else if (0 == i) {
      for (j = 0; j < w; ++j) {
        SET_COL_PD_FILTER(b, j, a, 0, 0, 0, al1, al2);
      }
    }
    else if (1 == i) {
      for (j = 0; j < w; ++j) {
        SET_COL_PD_FILTER(b, j, a, -al1, -al1, 0, al1, al2);
      }
    }
    else if ((h - 2) == i) {
      for (j = 0; j < w; ++j) {
        SET_COL_PD_FILTER(b, j, a, -al2, -al1, 0, al1, al1);
      }
    }
    else if ((h - 1) == i) {
      for (j = 0; j < w; ++j) {
        SET_COL_PD_FILTER(b, j, a, -al2, -al1, 0, 0, 0);
      }
    }

    SET_ROW_PD_FILTER(B, 0, b, 0, 0, 0, cn1, cn2);
    SET_ROW_PD_FILTER(B, 1, b, -cn1, -cn1, 0, cn1, cn2);
    SET_ROW_PD_FILTER(B, w - 2, b, -cn2, -cn1, 0, cn1, cn1);
    SET_ROW_PD_FILTER(B, w - 1, b, -cn2, -cn1, 0, 0, 0);

    for (j = 2; j < w - 2; ++j) {
      SET_ROW_PD_FILTER(B, j, b, -cn2, -cn1, 0, cn1, cn2);
    }
  }

  if (mybuf) {
    free(mybuf);
  }
  return 0;
}

// �������ڹ�һ���ķ���任����
CC_INLINE MATRIX3X2 get_std1(IPOINT s1, IPOINT e1, IPOINT s2, IPOINT e2)
{
  MATRIX3X2 m;
  double c = 0, s = 0;
  double dd = POINT_DIST(s1, e1) / (POINT_DIST(s2, e2));
  ANGLE_SIN_COS(e1.x - s1.x, e1.y - s1.y, e2.x - s2.x, e2.y - s2.y, s, c);
  c *= dd;
  s *= dd;
  MATRIX3X2_GET_STD_TOINT(m, c, s, s1, s2);
  return m;
}

CC_INLINE int CPM_FaceStd1(int ah, int aw, const unsigned char* A, int al, int ai,
    int bh, int bw, unsigned char* B, int bl, int bi,
    IPOINT leye, IPOINT reye,
    float lx, float ly, float rx, float ry)
{
  IPOINT s2, e2;
  MATRIX3X2 m;
  s2.x = (int)(bw * lx);
  s2.y = (int)(bh * ly);
  e2.x = (int)(bw * rx);
  e2.y = (int)(bh * ry);
  m = get_std1(leye, reye, s2, e2);
  IMAFFINE(ah, aw, A, al, ai, bh, bw, B, bl, bi, m, INTER_BILINEARITY);
  return 0;
}

CC_INLINE int affine_point(int n, const FPOINT* rpt, FPOINT* pt, IPOINT s1, IPOINT e1, IPOINT s2, IPOINT e2)
{
  int i;
  float x, y;
  MATRIX3X2 m;
  m = get_std1(s1, e1, s2, e2);

  for (i = 0; i < n; ++i) {
    x = rpt[ i ].x;
    y = rpt[ i ].y;
    MATRIX3X2_MUL_POINT(m, x, y, pt[ i ].x, pt[ i ].y);
    pt[ i ].x /= CC_SHIFT1;
    pt[ i ].y /= CC_SHIFT1;
  }

  return 0;
}
#undef DOT2
#define DOT2(_X0, _Y0, _X1, _Y1)  ((_X0)*(_X1)+(_Y0)*(_Y1))

CC_INLINE int facestd2(int ah, int aw, const unsigned char* A, int al, int ai,
    int bh, int bw, unsigned char* B, int bl, int bi,
    IPOINT leye, IPOINT reye, float rot0)
{
  float rot = (rot0) * (float) M_PI / 180.f;
  float lx = 0.25f, ly = 0.25f, rx = 0.75f, ry = 0.25f;
  float cx = 0.5f, cy = 0.5f, s;
  float sinr = (float) sin(rot), cosr = (float) cos(rot);
  lx -= cx;
  ly -= cy;
  rx -= cx;
  ry -= cy;
  lx = DOT2(lx, ly, cosr, sinr);
  ly = DOT2(lx, ly, -sinr, cosr);
  rx = DOT2(rx, ry, cosr, sinr);
  ry = DOT2(rx, ry, -sinr, cosr);
  lx += cx;
  ly += cy;
  rx += cx;
  ry += cy;
  s = 0.5f / (float) sqrt((lx - rx) * (lx - rx) + (ly - ry) * (ly - ry));
  lx *= s;
  ly *= s;
  rx *= s;
  ry *= s;
  //printf("%f : %13.9f, %13.9f, %13.9f, %13.9f\n", rot0, lx, ly, rx, ry);
  return CPM_FaceStd1(ah, aw, A, al, ai, bh, bw, B, bl, bi, leye, reye, lx, ly, rx, ry);
}

static int GetImagePyramid(int ah, int aw, const unsigned char* Img, int al, int NumPyramidLevels, int if_filter,
    int h, int w, unsigned char* ImagePyramid, IPOINT leye, IPOINT reye,
    float lx, float ly, float rx, float ry)
{
  //�������ܣ���ý������ֲ�ͼ��
  //������   ImgΪ���ֲ�ĻҶ�ͼ��
  //         NumPyramidLevelsΪͼ��������ֲ���
  //         if_filter Ϊͼ��������ֲ�ʱ�Ƿ���ø�˹�˲�,1Ϊ�ǣ���������Ϊ��
  //����ֵ:  ImagePyramid�洢�ֲ��ĸ���ͼ��
  unsigned char* out;
  int i;
  out = ImagePyramid;

  if (h > 0 && w > 0 && leye.x > 0) {
    CPM_FaceStd1(ah, aw, Img, al, 1, h, w, out, w, 1,
        leye, reye, lx, ly, rx, ry);
  }
  else {
    SKIP_COPY(h, w, Img, al, 1, out, w, 1);
  }

  for (i = 1; i < NumPyramidLevels; ++i) {
    PyramidDown(h / 2, w / 2, out + h * w, w / 2, out, w, 1, if_filter, 0);
    out += h * w;
    h /= 2;
    w /= 2;
    //imwrite("./asdf.bmp", h, w, out, w, 1);
#if 0
    {
      char buf[ 256 ];
      _sntprintf(buf, 256, "Pyr%d.bmp", i);
      imwrite(buf, h, w, out, w, 1);
    }
#endif

  }

  return 0;
}

CC_INLINE int GetProfStepSize(const FPOINT* _sub_shape, int NbOfPoints, int ind_markpoint,
    const int* contours_ending_points, int contours_ending_points_len,
    float* pDeltaX, float* pDeltaY)
{
  //�������ܣ�����DeltaX��DeltaY
  //����:    sub_shapeΪ��ǰ����������ͼ���Ӧ�ı����״����
  //         ind_markpointΪ��ǰ�ı������
  //         contours_ending_points�洢ÿ�����ٵ����һ���������������˳��Ϊ��ü�����ۡ���ü�����ۡ����ӡ����졢���졢����+�°�
  //����ֵ:  DeltaX��DeltaYΪprofile����������ĵ�λ�ƶ�����
  double temp_x, temp_y, d_x, d_y;
  double d0_x, d0_y, p_x, p_y;
  double d1_x, d1_y, q_x, q_y;
  double DeltaX, DeltaY;
  double AbsDeltaX, AbsDeltaY;
  int pt1, pt2;
  ASSERT(ind_markpoint < NbOfPoints);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //������ind_markpoint��ǰһ���pt1�ͺ�һ���pt2 (����)//
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  GetBeforeAfterPts(ind_markpoint, contours_ending_points, contours_ending_points_len, &pt1, &pt2);

  //////////////////////////////////////////////////
  // ������(DeltaX��DeltaY) //
  //////////////////////////////////////////////////
  temp_x = _sub_shape[(ind_markpoint)].x - _sub_shape[(pt1)].x;
  temp_y = _sub_shape[(ind_markpoint)].y - _sub_shape[(pt1)].y;
  d0_x = temp_x / (sqrt(temp_x * temp_x + temp_y * temp_y));
  d0_y = temp_y / (sqrt(temp_x * temp_x + temp_y * temp_y));
  p_x = d0_y; //��ת90��
  p_y = -d0_x;

  temp_x = _sub_shape[(pt2)].x - _sub_shape[(ind_markpoint)].x;
  temp_y = _sub_shape[(pt2)].y - _sub_shape[(ind_markpoint)].y;
  d1_x = temp_x / (sqrt(temp_x * temp_x + temp_y * temp_y));
  d1_y = temp_y / (sqrt(temp_x * temp_x + temp_y * temp_y));
  q_x = d1_y; ////��ת90��
  q_y = -d1_x;

  temp_x = p_x + q_x;
  temp_y = p_y + q_y;
  d_x = temp_x / (sqrt(temp_x * temp_x + temp_y * temp_y));
  d_y = temp_y / (sqrt(temp_x * temp_x + temp_y * temp_y));

  if (fabs(d_x) < 1e-10 && fabs(d_y) < 1e-10) {       //�����һ��DeltaX��DeltaY��Ϊ�˷�ֹ��ĸΪ0
    d_x = d0_x;
    d_y = d0_y;
  }

  //���¼��㲢��һ��DeltaX��DeltaY
  DeltaX = d_x;
  DeltaY = d_y;

  AbsDeltaX = fabs(DeltaX);
  AbsDeltaY = fabs(DeltaY);

  if (AbsDeltaX >= AbsDeltaY) {
    DeltaY = DeltaY / AbsDeltaX;
    DeltaX = DeltaX / AbsDeltaX;
  }
  else {
    DeltaX = DeltaX / AbsDeltaY;
    DeltaY = DeltaY / AbsDeltaY;
  }

  *pDeltaX = (float) DeltaX;
  *pDeltaY = (float) DeltaY;
  return 0;
}


typedef int int32;
CC_INLINE int CalcProfileGrad1D_for_fit(int nSamplePoints, const FPOINT* _sub_shape, int NbOfPoints, int ind_markpoint,
    const int* contours_ending_points, int contours_ending_points_len,
    int height, int width, const unsigned char* _image_curLevel, int nProfWidth,
    float* gradient_vector, float* pDeltaX, float* pDeltaY)
{
  //��������:����1D profile�ݶ����������ؼ���õ���DeltaX��DeltaY
  //����:    nSamplePointsΪprofile�������ϻ����²��������,
  //         sub_shapeΪ��ǰ����������ͼ���Ӧ�ı����״����
  //         ind_markpointΪ��ǰ�ı������
  //         contours_ending_points�洢ÿ�����ٵ����һ���������������˳��Ϊ��ü�����ۡ���ü�����ۡ����ӡ����졢���졢����+�°�
  //         image_curLevel ��ǰ����ͼ��ĵ�ǰ����������ͼ��
  //         nProfWidthΪprofile��ȼ�2*nSamplePoints +1
  //����ֵ:  gradient_vector Ϊδ��һ����profile�ݶ�����
  //         DeltaX��DeltaYΪprofile����������ĵ�λ�ƶ�����
#ifndef ROUND
//#undef ROUND
#define ROUND(_X) (int)(_X+0.5f)
#endif

  float x, y;
  float DeltaX, DeltaY;
  int temp_x, temp_y;
  int PrevPels, tempPels;
  int i;
  //[height,width] = size(image_curLevel);

  ////////////////////////////////////////
  //����DeltaX��DeltaY //
  ////////////////////////////////////////
  GetProfStepSize(_sub_shape, NbOfPoints, ind_markpoint,
      contours_ending_points, contours_ending_points_len, &DeltaX, &DeltaY);

  //////////////////////////////////////////////////////
  //����profile�ݶ����� //
  //////////////////////////////////////////////////////
  x = _sub_shape[(ind_markpoint)].x;
  y = _sub_shape[(ind_markpoint)].y;

  temp_x = ROUND(x + (-nSamplePoints - 1) * DeltaX);
  temp_y = ROUND(y + (-nSamplePoints - 1) * DeltaY);

  if (temp_y < 1) {
    temp_y = 1;
  }

  if (temp_x < 1) {
    temp_x = 1;
  }

  if (temp_y > height) {
    temp_y = height;
  }

  if (temp_x > width) {
    temp_x = width ;
  }

  PrevPels = (int32) _image_curLevel[(temp_y - 1) * width + (temp_x - 1)];

  for (i = -nSamplePoints; i <= nSamplePoints; ++i) {
    temp_x = ROUND(x + i * DeltaX);
    temp_y = ROUND(y + i * DeltaY);

    if (temp_y < 1) {
      temp_y = 1;
    }

    if (temp_x < 1) {
      temp_x = 1;
    }

    if (temp_y > height) {
      temp_y = height;
    }

    if (temp_x > width) {
      temp_x = width ;
    }

    tempPels = (int32) _image_curLevel[(temp_y - 1) * width + (temp_x - 1)];

    gradient_vector[ i + nSamplePoints ] = (float)(tempPels - PrevPels);
    PrevPels = tempPels;
  }


  *pDeltaX = DeltaX;
  *pDeltaY = DeltaY;
  return 0;
}

CC_INLINE int AlignTransformation1(const FPOINT* _ref, const FPOINT* _SubShape, int NbOfPoints, float* a, float* b, float* tx, float* ty)
//��������:��������(SubShape��ref����)
//����:��״SubShape��ref
//     NbOfPointsΪ������
//����ֵ:a,b,tx,ty
{
  int i;
  double Params[ 4 ];
  double X1, Y1, X2, Y2, Z, C1, C2, W;
  double x1, y1, x2, y2;
  X1 = 0;
  Y1 = 0;
  X2 = 0;
  Y2 = 0;
  Z = 0;
  C1 = 0;
  C2 = 0;
  W = (double) NbOfPoints;

  for (i = 0; i < NbOfPoints; ++i) {
    x1 = _ref[(i)].x;
    y1 = _ref[(i)].y;
    x2 = _SubShape[(i)].x;
    y2 = _SubShape[(i)].y;

    Z = Z + (x2 * x2 + y2 * y2);
    X1 = X1 + x1;
    Y1 = Y1 + y1;
    X2 = X2 + x2;
    Y2 = Y2 + y2;
    C1 = C1 + (x1 * x2 + y1 * y2);
    C2 = C2 + (y1 * x2 - x1 * y2);
  }

#if 0
  //SolnA*Params=SolnB
  SolnA = [ X2 - Y2 W 0;
      Y2 X2 0 W;
      Z 0 X2 Y2;
      0 Z - Y2 X2 ];
  SolnB = [ X1; Y1; C1; C2 ];

  Params = SolnA\SolnB;
#else

  {
#define SOLVE_4X4(a0,a1,a2,a3,b0,b1,b2,b3,c0,c1,c2,c3,d0,d1,d2,d3) \
  -(+(b2)*(c0)*(a3)*(d1)-(a0)*(b1)*(c2)*(d3)+(a2)*(c1)*(b3)*(d0)+(b1)*(a2)*(c0)*(d3) \
      +(a0)*(b1)*(c3)*(d2)+(c2)*(b1)*(a3)*(d0)+(c2)*(a0)*(b3)*(d1)-(a0)*(b3)*(c1)*(d2) \
      -(b3)*(c0)*(a2)*(d1)-(a3)*(c1)*(b2)*(d0)-(b1)*(a3)*(c0)*(d2)+(a0)*(b2)*(c1)*(d3) \
      -(c3)*(b1)*(a2)*(d0)-(c3)*(a0)*(b2)*(d1)-(b0)*(c2)*(a3)*(d1)+(b0)*(a3)*(c1)*(d2) \
      -(b0)*(a2)*(c1)*(d3)+(b0)*(c3)*(a2)*(d1)+(a1)*(b0)*(c2)*(d3)-(a1)*(b0)*(c3)*(d2) \
      +(a1)*(b3)*(c0)*(d2)-(a1)*(b2)*(c0)*(d3)+(a1)*(c3)*(b2)*(d0)-(a1)*(c2)*(b3)*(d0))

    double dd;
    dd = SOLVE_4X4(X2, Y2, Z, 0, -Y2, X2, 0, Z, W, 0, X2, -Y2, 0, W, Y2, X2);

    if (0. == dd) {
      return 0;
    }

    // d[0], d[1], d[2]
    Params[ 0 ] = SOLVE_4X4(X1, Y1, C1, C2, -Y2, X2, 0, Z, W, 0, X2, -Y2, 0, W, Y2, X2) / dd;
    Params[ 1 ] = SOLVE_4X4(X2, Y2, Z, 0, X1, Y1, C1, C2, W, 0, X2, -Y2, 0, W, Y2, X2) / dd;
    Params[ 2 ] = SOLVE_4X4(X2, Y2, Z, 0, -Y2, X2, 0, Z, X1, Y1, C1, C2, 0, W, Y2, X2) / dd;
    Params[ 3 ] = SOLVE_4X4(X2, Y2, Z, 0, -Y2, X2, 0, Z, W, 0, X2, -Y2, X1, Y1, C1, C2) / dd;
  }
#endif

  *a = (float) Params[ 0 ];
  *b = (float) Params[ 1 ];
  *tx = (float) Params[ 2 ];
  *ty = (float) Params[ 3 ];
  return 1;
}


//���Ͼ���
// A[1��h]
// B[h��w]
// C[w��1]
// A��B��C
CC_INLINE float MaDist(int h, int w, const float* A, const float* B, int bl, const float* C)
{
  int i, j;
  float d = 0.f;

  for (i = 0; i < h; ++i, B += bl) {
    float bc = 0.f;

    for (j = 0; j < w; ++j) {
      bc += B[ j ] * C[ j ];
    }

    d += A[ i ] * bc;
  }

  return d;
}

CC_INLINE int GetSuggestedShape(const FPOINT* _sub_shape, int height, int width, const unsigned char* image_curLevel,
    const int* contours_ending_points, int contours_ending_points_len,
    int nSamplePoints, int NbOfPoints, int nPelsSearch,
    int nProfWidth_model, const float* meanProfiles,
    const float* covProfiles_inv, int ind_Level,
    FPOINT* _SuggestedShape, int* pnGoodLandmarks
                               )
{
  //��������:���SuggestedShape��״�Լ�������ĸ���
  //����:    sub_shapeΪ��ǰ����������ͼ���Ӧ�ı����״����
  //         image_curLevelΪ��ǰ����������ͼ��
  //         contours_ending_points�洢ÿ�����ٵ����һ���������������˳��Ϊ��ü�����ۡ���ü�����ۡ����ӡ����졢���졢����+�°�,�Ǵӱ����ѵ�������ж�ȡ��
  //         nSamplePointsΪ����ʱprofile�������ϻ����²��������
  //         NbOfPoints������,�Ǵӱ����ѵ�������ж�ȡ��
  //         nProfWidth_modelΪ��ѵ���еõ���ƽ��profile�Ŀ��
  //         meanProfiles�洢ÿһ���������ÿһ����Ӧ������ѵ������ͼ���ƽ��profile�ݶ�,�Ǵӱ����ѵ�������ж�ȡ��
  //         covProfiles_inv�洢ÿһ���������ÿһ����Ӧ������ѵ������ͼ���profile�ݶȵ�Э�������������,�Ǵӱ����ѵ�������ж�ȡ��
  //         ind_Level��������������
  //         nPelsSearchΪprofileģ�͵���������profile�����������͸�������ƶ���
  //����ֵ:  SuggestedShapeΪ���б������profile�����ƶ������״(����)
  //         nGoodLandmarksΪ�������������
  char buf[1 << 9];
  int buflen = 1 << 9;
  int nGoodLandmarks = 0;
  const float* ModelProfVector;
  const float* ModelCov_inv;
  int ixBest;
  float BestFit;
  float DeltaX, DeltaY;
  int i, ind_markpoint, ix;
  float* gradient_vector;
  float* sub_gradient_vector;
  int gradient_vector_len = nPelsSearch * 2 + nProfWidth_model;
  BUFUSEBEGIN(buf, buflen);
  ASSERT((gradient_vector_len + nProfWidth_model) < (buflen));
  gradient_vector = BUFMALLOC(float, gradient_vector_len);
  sub_gradient_vector = BUFMALLOC(float, nProfWidth_model);

  // SuggestedShape = zeros(2,NbOfPoints);
  //FILL( NbOfPoints, _SuggestedShape, 0.f );
  for (i = 0; i < NbOfPoints; ++i) {
    _SuggestedShape[i].x = 0.f;
    _SuggestedShape[i].y = 0.f;
  }

  for (ind_markpoint = 0; ind_markpoint < NbOfPoints; ++ind_markpoint) {
    ModelProfVector = meanProfiles + (ind_markpoint * nProfWidth_model);   //תΪ������
    ModelCov_inv = covProfiles_inv + (ind_markpoint * nProfWidth_model * nProfWidth_model);   //ѵ���õ���profileЭ����,�߶�ΪnProfWidth_model*nProfWidth_model

    ixBest = 0;
    BestFit = (float)(1.7976931348623158e+30);      //��ʼ��Ϊ����󼴿�
    CalcProfileGrad1D_for_fit(nSamplePoints, _sub_shape, NbOfPoints, ind_markpoint,
        contours_ending_points, contours_ending_points_len,
        height, width, image_curLevel, 2 * nSamplePoints + 1,
        gradient_vector, &DeltaX, &DeltaY);

    if (ind_markpoint == 100) {
      int aaa = 0;
    }

    for (ix = -nPelsSearch; ix <= nPelsSearch; ++ix) {
      float MahDistance_Fit;
      float sum_abs = 0.f;
      int ind_sub_grad = ix + nPelsSearch;
      memcpy(sub_gradient_vector, gradient_vector + ind_sub_grad, sizeof(float) * nProfWidth_model);

      //(ind_sub_grad:nProfWidth_model+ind_sub_grad-1);
      //if length(find(sub_gradient_vector)~=0) ~= 0
      // sub_gradient_vector = sub_gradient_vector/(sum(abs(sub_gradient_vector)));
      //�ݶ������Ĺ�һ��
      for (i = 0; i < nProfWidth_model; ++i) {
        sum_abs += (float) fabs(sub_gradient_vector[ i ]);
      }

      // sub_gradient_vector-ModelProfVector
      for (i = 0; i < nProfWidth_model; ++i) {
        sub_gradient_vector[ i ] = sub_gradient_vector[ i ] / sum_abs - ModelProfVector[ i ];
      }

      // MahDistance_Fit = (sub_gradient_vector)' * ModelCov_inv * (sub_gradient_vector);//���Ͼ���
      MahDistance_Fit = MaDist(nProfWidth_model, nProfWidth_model, sub_gradient_vector,
          ModelCov_inv, nProfWidth_model, sub_gradient_vector);

      if (MahDistance_Fit < BestFit) {
        ixBest = ix;
        BestFit = MahDistance_Fit;
      }
    }

    _SuggestedShape[(ind_markpoint)].x = _sub_shape[(ind_markpoint)].x + ixBest * DeltaX;
    _SuggestedShape[(ind_markpoint)].y = _sub_shape[(ind_markpoint)].y + ixBest * DeltaY;

    if (abs(ixBest) <= nPelsSearch / 2) {
      nGoodLandmarks = nGoodLandmarks + 1;
    }

    *pnGoodLandmarks = nGoodLandmarks;

  }

  //FREE( gradient_vector );
  //FREE( sub_gradient_vector );
  BUFUSEEND();
  return 0;
}

//�������ܣ����ݶ������a,b,tx,ty��SubShape��ĳ����״����
//����ֵ:  NewSubShapeΪ�µ���״
CC_INLINE int TransformPose(float a, float b, float tx, float ty, const FPOINT* _SubShape, int NbOfPoints, FPOINT* _NewSubShape)
{
  //�������ܣ����ݶ������a,b,tx,ty��SubShape��ĳ����״����
  //����ֵ:  NewSubShapeΪ�µ���״
  int i;
  double c00, c01, c10, c11;

  c00 = a;
  c01 = -b;
  c10 = b;
  c11 = a;

  for (i = 0; i < NbOfPoints; ++i) {
    double x, y;
    x = _SubShape[(i)].x;
    y = _SubShape[(i)].y;
    _NewSubShape[(i)].x = c00 * x + c01 * y + tx;
    _NewSubShape[(i)].y = c10 * x + c11 * y + ty;
  }

  return 0;
}

// A[h��w]
// B[w��1]
// C[h��1]
// D[h��1]
// D = A * B + C
CC_INLINE int mat_mul_vec(int h, int w, const float* A, int al, const float* B, const float* C, float* D)
{
  int i, j;

  for (i = 0; i < h; ++i, A += al) {
    double d = 0.f;

    for (j = 0; j < w; ++j) {
      d += A[ j ] * B[ j ];
    }

    D[ i ] = (float)(C[ i ] + d);
  }

  return 0;
}

CC_INLINE int findResultShape_1DP(const asm_t* s, unsigned char** ImagePyramid, FPOINT* shape_result, char* buf, int buflen)
{
  float* MeanShape = s->MeanShape;
  float* meanProfiles = s->meanProfiles;
  float* covProfiles_inv = s->covProfiles_inv;
  float* ShapesEigenVectors = s->ShapesEigenVectors;
  float* shapes_eigen_values = s->shapes_eigen_values;
  float* StartShape = s->MeanShape;
  int* contours_ending_points = s->contours_ending_points;
  float coef_lim_temp = s->coef_lim;
  int NbOfPoints = s->NbOfPoints;
  int max_iter = s->max_iter;
  int contours_ending_points_len = s->contours_ending_points_len;
  int TrnPntsAbove = s->TrnPntsAbove;
  int TrnPntsBelow = s->TrnPntsBelow;
  int shapes_eigen_values_len = s->shapes_eigen_values_len;
  int nPelsSearch = s->nPelsSearch;
  int nQualifyingDisp = s->nQualifyingDisp;
  int train_image_height = s->train_image_height;
  int train_image_width = s->train_image_width;
  //�������ܣ���1D profile��������
  //������ IΪ������(����)ͼ��
  // if_filterͼ��������ֲ�ʱ�Ƿ���ø�˹�˲����Ǵӱ����ѵ�������ж�ȡ��
  // if_Prof_BilinearΪ����profile�����ϵ���������ʱ�Ƿ����˫���Բ�ֵ���Ǵӱ����ѵ�������ж�ȡ��
  // NumPyramidLevelsΪ�������ֲ���,�Ǵӱ����ѵ�������ж�ȡ��
  // StartShapeΪ��ʼ��״
  // MeanShapeΪƽ����״
  // NbOfPoints������,�Ǵӱ����ѵ�������ж�ȡ��
  // max_iter����ʱ������������
  // contours_ending_points�洢ÿ�����ٵ����һ���������������˳��Ϊ��ü�����ۡ���ü�����ۡ����ӡ����졢���졢����+�°�,�Ǵӱ����ѵ�������ж�ȡ��
  // TrnPntsAboveΪѵ��ʱ1D profile���������
  // TrnPntsBelowΪѵ��ʱ1D profile���������
  // nPelsSearchΪprofileģ�͵���������profile�����������͸�������ƶ���
  // meanProfiles�洢ÿһ���������ÿһ����Ӧ������ѵ������ͼ���ƽ��profile�ݶ�,�Ǵӱ����ѵ�������ж�ȡ��
  // covProfiles_inv �洢ÿһ���������ÿһ����Ӧ������ѵ������ͼ���profile�ݶȵ�Э�������������,�Ǵӱ����ѵ�������ж�ȡ��
  // ShapesEigenVectorsΪ���ضϵ���״��������,�Ǵӱ����ѵ�������ж�ȡ��
  // shapes_eigen_valuesΪ���ضϵ���״����ֵ,�Ǵӱ����ѵ�������ж�ȡ��
  // nQualifyingDispΪ�����ж��Ƿ������Ĳ���
  // coef_lim_tempΪ��״����������ϵ��
  // train_image_height��train_image_widthΪѵ��ͼ���Ⱥ͸߶�
  //����ֵ: shape_resultΪ������״���(����)

  int i, j, ind_Level;
  int nProfWidth_model = TrnPntsAbove + TrnPntsBelow + 1; //profile(����)������
  int nSamplePoints, converge_num;
  int h = train_image_height;
  int w = train_image_width;

  BUFUSEBEGIN(buf, buflen);
  float* sub_MeanShape_ascol = BUFMALLOC(float, 2 * NbOfPoints);
  float* xxx_ascol = BUFMALLOC(float, 2 * NbOfPoints);
  float* sp = BUFMALLOC(float, shapes_eigen_values_len);
  FPOINT* _xxx = BUFMALLOC(FPOINT, NbOfPoints);
  FPOINT* yyy = BUFMALLOC(FPOINT, NbOfPoints);
  FPOINT* tempShape = BUFMALLOC(FPOINT, NbOfPoints);
  FPOINT* SuggestedShape = BUFMALLOC(FPOINT, NbOfPoints);

  nSamplePoints = (nProfWidth_model - 1) / 2 + nPelsSearch;   //����ʱprofile(����)�������������
  converge_num = (int) floor(nQualifyingDisp * NbOfPoints / 100);     //��������
  // if ndims(I)==3, I=rgb2gray(I); end

  //MeanShape_ascol = MeanShape;//д����������ʽ,x0,y0,x1,y1,...,xn,yn
  //sub_shape      = StartShape/(2^(s->NumPyramidLevels-1));//��õ�ǰ����������ͼ���Ӧ�ı����״����
  {
    int k = 1 << (s->NumPyramidLevels - 1);

    for (i = 0; i < NbOfPoints; ++i) {
      shape_result[i].x = StartShape[ 0 + 2 * i ] / k;
      shape_result[i].y = StartShape[ 1 + 2 * i ] / k;
    }
  }

  //xxx = zeros(2,NbOfPoints);//��ʱ��״��ʼ��
  for (ind_Level = s->NumPyramidLevels - 1; ind_Level >= 0; --ind_Level) {   //���ڵ�ǰ����������ÿһ��������
    int iter;
    int nGoodLandmarks;
    float coef_lim;
    int k = 1 << (ind_Level);
    float* _meanProfiles = meanProfiles + (s->NumPyramidLevels - ind_Level - 1) * NbOfPoints * nProfWidth_model;
    float* _covProfiles_inv = covProfiles_inv + (s->NumPyramidLevels - ind_Level - 1) * NbOfPoints * nProfWidth_model * nProfWidth_model;
    //sub_MeanShape_ascol = MeanShape_ascol/(2^(ind_Level-1));//��ǰ��ƽ����״
    //sub_StartShape = StartShape/(2^(ind_Level-1));//��ǰ���ʼ��״
    unsigned char* image_curLevel;
    image_curLevel = ImagePyramid[ ind_Level ]; //��ǰ��ͼ��

    for (i = 0; i < NbOfPoints * 2; ++i) {
      sub_MeanShape_ascol[ i ] = MeanShape[ i ] / k;
      //sub_StartShape[i] = StartShape[i]/k;
    }

    //sp = zeros(length(shapes_eigen_values),1);//��ʼ����״����
    FILL(shapes_eigen_values_len, sp, 0.f);
    iter = 1;
    nGoodLandmarks = 0;

    while (iter <= max_iter && nGoodLandmarks <= converge_num) {
      //int ind_temp;
      float a=0, b=0, tx=0, ty=0;
      GetSuggestedShape(shape_result, h / k, w / k, image_curLevel,
          contours_ending_points, contours_ending_points_len,
          nSamplePoints, NbOfPoints, nPelsSearch, nProfWidth_model,
          _meanProfiles, _covProfiles_inv,
          ind_Level, SuggestedShape, &nGoodLandmarks);

      if (iter == max_iter) {
        coef_lim = 3.f;
      }
      else {
        coef_lim = coef_lim_temp;
      }

      // xxx_ascol = sub_MeanShape_ascol + ShapesEigenVectors*sp;
#if 0
      mat_mul_vec(NbOfPoints * 2, shapes_eigen_values_len, ShapesEigenVectors, shapes_eigen_values_len,
          sp, sub_MeanShape_ascol, xxx_ascol);
#else
      {
        int i, j;

        for (i = 0; i < NbOfPoints * 2; ++i) {
          double d = 0.f;

          for (j = 0; j < shapes_eigen_values_len; ++j) {
            d += ShapesEigenVectors[ i * shapes_eigen_values_len + j ] * sp[ j ];
          }

          xxx_ascol[ i ] = (float)(sub_MeanShape_ascol[ i ] + d);
        }
      }

#endif

      for (i = 0; i < NbOfPoints; ++i) {
        _xxx[(i)].x = xxx_ascol[ 0 + 2 * i ];
        _xxx[(i)].y = xxx_ascol[ 1 + 2 * i ];
      }

      AlignTransformation1(_xxx, SuggestedShape, NbOfPoints, &a, &b, &tx, &ty);   //SuggestedShape��xxx����Ĳ���
      TransformPose(a, b, tx, ty, SuggestedShape, NbOfPoints, yyy);   //SuggestedShapeʹ��4��������xxx����,��������״Ϊyyy

      for (i = 0; i < NbOfPoints; ++i) {
        xxx_ascol[ 0 + 2 * i ] = yyy[ i ].x - sub_MeanShape_ascol[ 0 + 2 * i ];
        xxx_ascol[ 1 + 2 * i ] = yyy[ i ].y - sub_MeanShape_ascol[ 1 + 2 * i ];
      }

      //sp = ShapesEigenVectors'*(yyy(:) - sub_MeanShape_ascol);//ʵ����sp = inv(ShapesEigenVectors'*ShapesEigenVectors)*ShapesEigenVectors'*(yyy(:) - sub_MeanShape_ascol)ֻ��inv(ShapesEigenVectors'*ShapesEigenVectors)ΪE����(���Խ�Ԫ��Ϊ1������Ԫ��Ϊ0)
      for (j = 0; j < shapes_eigen_values_len; ++j) {
        float d = 0.f;

        for (i = 0; i < NbOfPoints * 2; ++i) {
          d += ShapesEigenVectors[ i * shapes_eigen_values_len + j ] * xxx_ascol[ i ];
        }

        sp[ j ] = d;
      }

      // LIM=coef_lim*sqrt(fabs(shapes_eigen_values));//������״����
      for (i = 0; i < shapes_eigen_values_len; ++i) {
        float LIM = coef_lim * (float) sqrt(fabs(shapes_eigen_values[ i ]));

        if (sp[ i ] > LIM) {
          sp[ i ] = LIM;
        }

        if (sp[ i ] < -LIM) {
          sp[ i ] = -LIM;
        }
      }

      // xxx_ascol = sub_MeanShape_ascol + ShapesEigenVectors*sp;
      for (i = 0; i < NbOfPoints * 2; ++i) {
        float d = 0.f;

        for (j = 0; j < shapes_eigen_values_len; ++j) {
          d += ShapesEigenVectors[ i * shapes_eigen_values_len + j ] * sp[ j ];
        }

        xxx_ascol[ i ] = sub_MeanShape_ascol[ i ] + d;
      }

      //ind_temp = 0;

      for (i = 0; i < NbOfPoints; ++i) {
        tempShape[ i ].x = xxx_ascol[ 0 + 2 * i ];
        tempShape[ i ].y = xxx_ascol[ 1 + 2 * i ];
      }

      AlignTransformation1(SuggestedShape, _xxx, NbOfPoints, &a, &b, &tx, &ty);   //xxx��SuggestedShape����Ĳ���
      TransformPose(a, b, tx, ty, tempShape, NbOfPoints, shape_result);

      iter = iter + 1;
    }

    if (ind_Level == 0) {
      break;
    }

    for (i = 0; i < NbOfPoints; ++i) {
      shape_result[ i ].x *=  2;
      shape_result[ i ].y *=  2;
    }
  }

  //BUFFREE2( yyy, 2 * NbOfPoints );
  //BUFFREE2( _xxx, 2 * NbOfPoints );
  //BUFFREE2( sub_MeanShape_ascol, 2 * NbOfPoints );
  //BUFFREE(sub_StartShape);
  //BUFFREE2( SuggestedShape, 2 * NbOfPoints );
  //BUFFREE2( xxx_ascol, 2 * NbOfPoints );
  //BUFFREE2( tempShape, 2 * NbOfPoints );
  //BUFFREE2( xxx_ascol, 2 * NbOfPoints );
  //BUFFREE2( sp, shapes_eigen_values_len );

  BUFUSEEND();

  return 0;
}

static int asm_fix_inl(asm_t* s, const void* buf)
{
  int k, len = 1<<20;
  char* cbuf = (char*)buf;
  *s = *(asm_t*)buf;
  cbuf += HEADSIZE;
  BUFUSEBEGIN(cbuf, len);
  k = (s->TrnPntsAbove + s->TrnPntsBelow + 1);
  BUFMALLOC2(s->contours_ending_points, s->contours_ending_points_len);
  BUFMALLOC2(s->MeanShape, s->NbOfPoints*2);
  BUFMALLOC2(s->shapes_eigen_values, s->shapes_eigen_values_len);
  BUFMALLOC2(s->ShapesEigenVectors, s->NbOfPoints * 2 * s->shapes_eigen_values_len);
  BUFMALLOC2(s->meanProfiles, (s->NbOfPoints * s->NumPyramidLevels) * k);
  BUFMALLOC2(s->covProfiles_inv, (s->NbOfPoints * s->NumPyramidLevels) * (k * k));
  BUFUSEEND();
#if 0
  {
    int i;
    printf("test GetBeforeAfterPts\n");

    for (i = 0; i < s->NbOfPoints; ++i) {
      int Before, After;
      GetBeforeAfterPts(i, s->contours_ending_points, s->contours_ending_points_len, &Before, &After);
      printf("%3d %3d %3d\n", Before, i, After);
    }

    return 0;
  }
#endif
  return 0;
}

static int* scanf_imat(FILE* pf, int* prow, int* pcol)
{
  int i, n, _row = 0, _col = 0;
  int* imat;
  char buf[ MAXSCANFBUFSIZE ];

  while (!strchr(fgets(buf, MAXSCANFBUFSIZE, pf), '"'))
    ;

  fscanf(pf, "%d %d", &_row, &_col);
  n = _row * _col;
  imat = MALLOC(int, n);

  for (i = 0; i < n; ++i) {
    fscanf(pf, "%d", (imat + i));
  }

  if (prow) {
    *prow = _row;
  }

  if (pcol) {
    *pcol = _col;
  }

  return imat;
}

static float* scanf_fmat(FILE* pf, int* prow, int* pcol)
{
  int i, n, _row = 0, _col = 0;
  float* fmat;
  char buf[ MAXSCANFBUFSIZE ];

  while (!strchr(fgets(buf, MAXSCANFBUFSIZE, pf), '"'))
    ;

  fscanf(pf, "%d %d", &_row, &_col);
  n = _row * _col;
  fmat = MALLOC(float, n);

  for (i = 0; i < n; ++i) {
    float f = 0.;
    fscanf(pf, "%f", &f);
    fmat[ i ] = f;
  }

  if (prow) {
    *prow = _row;
  }

  if (pcol) {
    *pcol = _col;
  }

  return fmat;
}

CC_INLINE void asm_free(asm_t** pasm)
{
  if (pasm && *pasm) {
    asm_t* s = *pasm;
    FREE(s->contours_ending_points);
    FREE(s->MeanShape);
    FREE(s->shapes_eigen_values);
    FREE(s->ShapesEigenVectors);
    FREE(s->meanProfiles);
    FREE(s->covProfiles_inv);
    FREE(*pasm);
  }
}

CC_INLINE void asm_set_defalt(asm_t* s)
{
  ASSERT(NULL != s);
  s->max_iter = 5; //����ʱ������������(��������)
  s->nPelsSearch = 3; //profileģ�͵���������profile�����������͸�������ƶ���(��������)
  s->nQualifyingDisp = 95; //�����ж��Ƿ������Ĳ���
  s->coef_lim = 1.8f; //Ϊ��״����������ϵ��,����Ӧ��Ϊ1.8
}

CC_INLINE int save_inl(FILE* pf, void* p, int len)
{
  int* pi = (int*)p;
  int i, leni = len / 4;
  ASSERT(0 == len % 4);

  for (i = 0; i < leni; ++i) {
    fprintf(pf, "0x%08x,\r\n", pi[i]);
  }

  return 0;
}

#define SAVE_INL(_F, _P, _N) save_inl(_F, (_P), (_N) * sizeof(*(_P)))

static int asm_save_inl(asm_t* s, const char* txtfilename)
{
  FILE* pf = NULL;
  pf = fopen(txtfilename, "wb");

  if (pf) {
    int k = (s->TrnPntsAbove + s->TrnPntsBelow + 1);
    SAVE_INL(pf, s, 1);
    SAVE_INL(pf, s->contours_ending_points, s->contours_ending_points_len);
    SAVE_INL(pf, s->MeanShape, s->NbOfPoints*2);
    SAVE_INL(pf, s->shapes_eigen_values, s->shapes_eigen_values_len);
    SAVE_INL(pf, s->ShapesEigenVectors, s->NbOfPoints * 2 * s->shapes_eigen_values_len);
    SAVE_INL(pf, s->meanProfiles, (s->NbOfPoints * s->NumPyramidLevels) * k);
    SAVE_INL(pf, s->covProfiles_inv, (s->NbOfPoints * s->NumPyramidLevels) * (k * k));
    fclose(pf);
  }

  return 0;
}

CC_INLINE asm_t* asm_load_txt(const char* txtfilename)
{
  FILE* pf = NULL;
  asm_t* s = NULL;
  pf = fopen(txtfilename, "rb");

  if (pf) {
    int* imat = NULL;
    float* fmat = NULL;
    int row, col;
    s = MALLOC(asm_t, 1);
    ASSERT(NULL != s && "s = MALLOC(asm_t, 1)");
    asm_set_defalt(s);
    {
      //"ѵ�������߶ȺͿ��(train_image_height��train_image_width)"
      imat = scanf_imat(pf, &row, &col);
      ASSERT(1 == row);
      ASSERT(2 == col);
      s->train_image_height = imat[ 0 ];
      s->train_image_width = imat[ 1 ];
      FREE(imat);
    }
    {
      //"������(NbOfPoints)"
      imat = scanf_imat(pf, &row, &col);
      ASSERT(1 == row);
      ASSERT(1 == col);
      s->NbOfPoints = imat[ 0 ];
      FREE(imat);
    }
    {
      //"ÿ�����ٵ����һ���������(contours_ending_points)"
      s->contours_ending_points = scanf_imat(pf, &row, &col);
      ASSERT(1 == row);
      s->contours_ending_points_len = col;
    }
    {
      //"profile�������Ϻ�������������(TrnPntsAbove��TrnPntsBelow)"
      imat = scanf_imat(pf, &row, &col);
      ASSERT(1 == row);
      ASSERT(2 == col);
      s->TrnPntsAbove = imat[ 0 ];
      s->TrnPntsBelow = imat[ 1 ];
      FREE(imat);
    }
    {
      //"ͼ��������ֲ���(NumPyramidLevels)"
      imat = scanf_imat(pf, &row, &col);
      ASSERT(1 == row);
      ASSERT(1 == col);
      s->NumPyramidLevels = imat[ 0 ];
      FREE(imat);
    }
    {
      //"ͼ��������ֲ�ʱ�Ƿ���ø�˹�˲�(if_filter)"
      imat = scanf_imat(pf, &row, &col);
      ASSERT(1 == row);
      ASSERT(1 == col);
      s->if_filter = imat[ 0 ];
      FREE(imat);
    }
    {
      //"ƽ����״,����Ϊx����Ϊy(MeanShape)"
      int i;
      float* MeanShape = scanf_fmat(pf, &row, &col);
      s->MeanShape = MALLOC(float, 2 * s->NbOfPoints);
      ASSERT(2 == row);
      ASSERT(s->NbOfPoints == col);

      for (i = 0; i < s->NbOfPoints; ++i) {
        s->MeanShape[ 0 + 2 * i ] = MeanShape[ i ];
        s->MeanShape[ 1 + 2 * i ] = MeanShape[ s->NbOfPoints + i ];
      }

      FREE(MeanShape);
    }
    {
      //"pca��״����ֵ(shapes_eigen_values)"
      s->shapes_eigen_values = scanf_fmat(pf, &row, &col);
      ASSERT(1 == row);
      s->shapes_eigen_values_len = col;
    }
    {
      //"pca��״��������(ShapesEigenVectors)"
      s->ShapesEigenVectors = scanf_fmat(pf, &row, &col);
      ASSERT((s->NbOfPoints * 2) == row);
      ASSERT((s->shapes_eigen_values_len) == col);
    }
    {
      //"ƽ����һ��profile����(meanProfiles)"
      s->meanProfiles = scanf_fmat(pf, &row, &col);
      ASSERT((s->NbOfPoints * s->NumPyramidLevels) == row);
      ASSERT((s->TrnPntsAbove + s->TrnPntsBelow + 1) == col);
    }
    {
      //"profileЭ�������(covProfiles_inv)"
      int k = (s->TrnPntsAbove + s->TrnPntsBelow + 1);
      s->covProfiles_inv = scanf_fmat(pf, &row, &col);
      ASSERT((s->NbOfPoints * s->NumPyramidLevels) == row);
      ASSERT((k * k) == col);
    }
    fclose(pf);
  }

  asm_save_inl(s, "./asmTrainData_all.inl");
  asm_free(&s);
  return s;
}

static int asmfit_memsize(void* p0)
{
  asmfit_param_t* p = (asmfit_param_t*)p0;
  asm_t s[1] = {0};
  asm_fix_inl(s, p->asm_mode);
  ASSERT(NULL != p->asm_mode);
  p->buflen = ARRSIZE(float, 6 * 2 * s->NbOfPoints + s->shapes_eigen_values_len)
      + ARRSIZE(unsigned char*, s->NumPyramidLevels)
      + (4 * s->train_image_height * s->train_image_width) / 3;
  p->shape_result_len = s->NbOfPoints;
  p->contours_ending_points = s->contours_ending_points;
  p->contours_ending_points_len = s->contours_ending_points_len;
  return 0;
}

static int asmfit_process(void* p0)
{
  asmfit_param_t* p = (asmfit_param_t*)p0;
  asm_t s[1] = {0};
  int asmlen = 0;
  int h = p->height;
  int w = p->width;
  const unsigned char* I = p->data;
  int al = p->datastep;
  IPOINT leye = p->leye;
  IPOINT reye = p->reye;
  int ret;
  asm_fix_inl(s, p->asm_mode);

  //printf("ptr size = %d\n", sizeof(void*));
  //asm_load_txt("./asmTrainData_all.txt");
  BUFUSEBEGIN(p->buf, p->buflen);
#if 1
#if 1
  unsigned char* ImagePyramid[23];
  unsigned char* ImagePyramidBuf = NULL;
  float lx, ly, rx, ry;
  int buflen = (6 * 2 * s->NbOfPoints + s->shapes_eigen_values_len) * sizeof(float);
  char* buf = BUFMALLOC(char, buflen);
  lx = 120 / 320.f;
  ly = 0.5f;
  rx = 200 / 320.f;
  ry = 0.5f;
  ImagePyramidBuf = BUFMALLOC(unsigned char, (4 * s->train_image_height * s->train_image_width)/3);
  {
    int i, h, w;
    h = s->train_image_height, w = s->train_image_width;
    ImagePyramid[0] = ImagePyramidBuf;

    for (i=1; i<s->NumPyramidLevels; ++i) {
      ImagePyramid[i] = ImagePyramid[i - 1] + h * w;
      h /= 2, w /= 2;
    }
  }
  GetImagePyramid(h, w, I, al, s->NumPyramidLevels, s->if_filter,
      s->train_image_height, s->train_image_width, ImagePyramidBuf,
      leye, reye, lx, ly, rx, ry);  //��ý������ֲ�ͼ��
  ret = findResultShape_1DP(s, ImagePyramid, p->shape_result, buf, buflen);

  //BUFFREE(ImagePyramidBuf, (4 * s->train_image_height * s->train_image_width)/3 );
  //BUFFREE( buf, buflen );
  {
    IPOINT s1, e1;
    s1.x = (int)(lx * s->train_image_width);
    s1.y = (int)(ly * s->train_image_height);
    e1.x = (int)(rx * s->train_image_width);
    e1.y = (int)(ry * s->train_image_height);
    affine_point(s->NbOfPoints, p->shape_result, p->shape_result,
        leye, reye, s1, e1);
  }
#endif

#else
  memcpy(shape_result, StartShape, sizeof(float) * 2 * s->NbOfPoints);
#endif

  BUFUSEEND();
  return ret;
}

#endif // _ASMFIT_INL_
