#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int test_hist1()
{
  //���ȿ϶��ǲ�ɫ��ʽ��ȡͼƬ
  cv::Mat sourcePic = cv::imread("E:/data/testpic/farm2.jpg");
  cv::imshow("Source Picture", sourcePic);

  //���庯����Ҫ��һЩ����
  //ͼƬ����nimages
  int nimages = 1;
  //ͨ������,��������ϰ������������ʾ������ὲԭ��
  int channels[3] = { 0,1,2 };
  //���ֱ��ͼ
  cv::Mat outputHist_red, outputHist_green, outputHist_blue;
  //ά��
  int dims = 1;
  //���ÿ��ά��ֱ��ͼ�ߴ磨bin������������histSize
  int histSize[3] = { 256,256,256 };
  //ÿһά��ֵ��ȡֵ��Χranges
  float hranges[2] = { 0, 255 };
  //ֵ��Χ��ָ��
  const float *ranges[3] = { hranges,hranges,hranges };
  //�Ƿ����
  bool uni = true;
  //�Ƿ��ۻ�
  bool accum = false;

  //����ͼ���ֱ��ͼ(��ɫͨ������)
  cv::calcHist(&sourcePic, nimages, &channels[0], cv::Mat(), outputHist_red, dims, &histSize[0], &ranges[0], uni, accum);
  //����ͼ���ֱ��ͼ(��ɫͨ������)
  cv::calcHist(&sourcePic, nimages, &channels[1], cv::Mat(), outputHist_green, dims, &histSize[1], &ranges[1], uni, accum);
  //����ͼ���ֱ��ͼ(��ɫͨ������)
  cv::calcHist(&sourcePic, nimages, &channels[2], cv::Mat(), outputHist_blue, dims, &histSize[2], &ranges[2], uni, accum);

  //����ÿ������(bin)���飬��������ڿ���̨����ġ�
  //for (int i = 0; i < 256; i++)
  //std::cout << "bin/value:" << i << "=" << outputHist_red.at<float>(i) << std::endl;

  //����ֱ��ͼ
  int scale = 1;
  //ֱ��ͼ��ͼƬ,��Ϊ�ߴ���һ�����,���Ծ���histSize[0]����ʾȫ����.
  cv::Mat histPic(histSize[0], histSize[0] * scale * 3, CV_8UC3, cv::Scalar(0, 0, 0));
  //�ҵ����ֵ����Сֵ,������0��2�ֱ��Ǻ�,��,��
  double maxValue[3] = { 0, 0, 0 };
  double minValue[3] = { 0, 0, 0 };
  cv::minMaxLoc(outputHist_red, &minValue[0], &maxValue[0], NULL, NULL);
  cv::minMaxLoc(outputHist_green, &minValue[1], &maxValue[1], NULL, NULL);
  cv::minMaxLoc(outputHist_blue, &minValue[2], &maxValue[2], NULL, NULL);
  //����
  std::cout << minValue[0] << " " << minValue[1] << " " << minValue[2] << std::endl;
  std::cout << maxValue[0] << " " << maxValue[1] << " " << maxValue[2] << std::endl;

  //���������ű���
  double rate_red = (histSize[0] / maxValue[0])*0.9;
  double rate_green = (histSize[0] / maxValue[1])*0.9;
  double rate_blue = (histSize[0] / maxValue[2])*0.9;

  for (int i = 0; i < histSize[0]; i++)
  {
    float value_red = outputHist_red.at<float>(i);
    float value_green = outputHist_green.at<float>(i);
    float value_blue = outputHist_blue.at<float>(i);
    //�ֱ𻭳�ֱ��
    cv::line(histPic, cv::Point(i*scale, histSize[0]), cv::Point(i*scale, histSize[0] - value_red*rate_red), cv::Scalar(0, 0, 255));
    cv::line(histPic, cv::Point((i + 256)*scale, histSize[0]), cv::Point((i + 256)*scale, histSize[0] - value_green*rate_green), cv::Scalar(0, 255, 0));
    cv::line(histPic, cv::Point((i + 512)*scale, histSize[0]), cv::Point((i + 512)*scale, histSize[0] - value_blue*rate_blue), cv::Scalar(255, 0, 0));
  }
  cv::imshow("histgram", histPic);
  cv::waitKey(0);
  return 0;
}


#include <vector>
using namespace std;
using namespace cv;
int test_hist(int argc, char** argv)
{
  Mat src, dst;
  src = cv::imread("E:/data/testpic/farm2.jpg");

  if (!src.data)
  {
    return -1;
  }

  vector<Mat> rgb_planes;
  split(src, rgb_planes);

  int histSize = 255;
  float range[] = { 0, 255 };
  const float* histRange = { range };
  bool uniform = true;
  bool accumulate = false;

  Mat r_hist, g_hist, b_hist;

  // ����ֱ��ͼ:  
  calcHist(&rgb_planes[0], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
  calcHist(&rgb_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
  calcHist(&rgb_planes[2], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);

  // ����ֱ��ͼ����  
  int hist_w = 400;
  int hist_h = 400;
  int bin_w = cvRound((double)hist_w / histSize);

  Mat histImage(hist_w, hist_h, CV_8UC3, Scalar(0, 0, 0));

  // ��ֱ��ͼ��һ������Χ [ 0, histImage.rows ]  
  normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
  normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
  normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

  // ��ֱ��ͼ�����ϻ���ֱ��ͼ  
  for (int i = 1; i < histSize; i++)
  {
    line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
      Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
      Scalar(0, 0, 255), 2, 8, 0);
    line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
      Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
      Scalar(0, 255, 0), 2, 8, 0);
    line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
      Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
      Scalar(255, 0, 0), 2, 8, 0);
  }

  /// ��ʾֱ��ͼ  
  namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
  imshow("calcHist Demo", histImage);
  waitKey(0);
  return 0;

}
