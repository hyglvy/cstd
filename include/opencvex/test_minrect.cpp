
#include<iostream>
#include<vector>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int test_minrect()
{
  Mat srcImage(Size(600, 600), CV_8UC3, Scalar(0));

  RNG &rng = theRNG();

  char key;
  while (1)
  {
    //�������һЩ��
    //���Ⱦ���������ɵ��������
    int g_nPointCount = rng.uniform(3, 200);
    //�����������������һЩ�������
    vector<Point> points;
    for (int i = 0; i < g_nPointCount; i++)
    {
      Point midPoint;

      midPoint.x = rng.uniform(srcImage.cols / 4, srcImage.cols * 3 / 4);
      midPoint.y = rng.uniform(srcImage.rows / 4, srcImage.rows * 3 / 4);

      points.push_back(midPoint);
    }

    //��ʾ�ո�������ɵ���Щ��
    for (int i = 0; i < g_nPointCount; i++)
    {
      circle(srcImage, points[i], 0, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 3);
    }

    //�����ɵ���Щ�������Ѱ����С��Χ����
    //rectPoint�����еõ��˾��ε��ĸ���������
    RotatedRect rectPoint = minAreaRect(points);
    //����һ���洢�����ĸ��������ı���
    Point2f fourPoint2f[4];
    //��rectPoint�����д洢������ֵ�ŵ� fourPoint��������
    rectPoint.points(fourPoint2f);

    //���ݵõ����ĸ��������  ���ƾ���
    for (int i = 0; i < 3; i++)
    {
      line(srcImage, fourPoint2f[i], fourPoint2f[i + 1]
        , Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 3);
    }
    line(srcImage, fourPoint2f[0], fourPoint2f[3]
      , Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 3);

    imshow("�����ƽ������ͼ��", srcImage);

    key = waitKey();
    if (key == 27)
      break;
    else
      srcImage = Scalar::all(0);
  }

  return 0;
}


