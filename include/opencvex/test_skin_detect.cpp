

//http://www.cnblogs.com/tornadomeet/archive/2012/12/05/2802428.html
//A super - simple skin detector in OpenCV
// http://spottrlabs.blogspot.com/2012/01/super-simple-skin-detector-in-opencv.html
//Ƥ�������˷�����Ӱ�����ͨ��Ѱ��(����Ƕ�д�ķ�����Щ�鷳���Ҵ�����д����������)
// http://blog.csdn.net/wobuaishangdiao/article/details/7669566
//http ://docs.opencv.org/modules/core/doc/drawing_functions.html?highlight=ellipse#cv.Ellipse



#include "wstd/filesystem.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/contrib/contrib.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
 
using namespace std;
using namespace cv;
 
Mat input_image;
Mat output_mask;
Mat output_image;
Mat mask;
Mat skinCrCbHist;
//event����¼����ţ�x,y������꣬flags��ק�ͼ��̲����Ĵ���  
void on_mouse(int event, int x, int y, int flags, void *ustc)
{
  if (event == CV_EVENT_LBUTTONDOWN) {
    skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);
    ellipse(skinCrCbHist, Point(x, y), Size(15, 15), 0, 0.0, 360.0, Scalar(255, 255, 255), -1);
  }
}


int test() {
  const char* path = "C:/outpic/dst_ok/list.txt";
  vector<string> lines;
  wstd::readlines(path, lines);
  Mat skinCrCbHist1 = Mat::zeros(Size(256, 256), CV_32FC1);
  Mat skinCrCbHist2 = Mat::zeros(Size(256, 256), CV_32FC1);
  Mat skinCrCbHist3 = Mat::zeros(Size(256, 256), CV_8UC1);
  for (int i = 0; i < lines.size(); ++i) {
    Mat input_image = cv::imread(lines[i].c_str());
    Mat ycrcb_image;
    cvtColor(input_image, ycrcb_image, CV_BGR2YCrCb); //����ת���ɵ�YCrCb�ռ�
    for (int i = 0; i < input_image.rows; i++) //������ԲƤ��ģ�ͽ���Ƥ�����
    {
      Vec3b* ycrcb = (Vec3b*)ycrcb_image.ptr<Vec3b>(i);
      for (int j = 0; j < input_image.cols; j++)
      {
        skinCrCbHist2.at<int>(ycrcb[j][1], ycrcb[j][2])++;
      }
    }

  }
  double minv = 0.0, maxv = 0.0;
  log(skinCrCbHist2, skinCrCbHist1);
  minMaxIdx(skinCrCbHist1, &minv, &maxv);
  skinCrCbHist1.convertTo(skinCrCbHist3, CV_8UC1, 255. / maxv, 0);
  imshow("skinCrCbHist3", skinCrCbHist3);
  waitKey(0);
  return 0;
}

int test_skin_detect()
{
  //return test();
	VideoCapture cam(0);
  if (!cam.isOpened()) {
    return 0;
  }
 
	/*��ԲƤ��ģ��*/
  skinCrCbHist  = Mat::zeros(Size(256, 256), CV_8UC1);
	ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);
 
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1) );

  imshow("skinCrCbHist", skinCrCbHist);
  setMouseCallback("skinCrCbHist", on_mouse, 0);//���ûص�����  
	while(true) 
	{
		cam >> input_image;
    if (input_image.empty()) {
      break;
    }

    Mat skinCrCbHist2 = Mat::zeros(Size(256, 256), CV_32FC1);
    Mat skinCrCbHist3 = Mat::zeros(Size(256, 256), CV_8UC1);
		Mat ycrcb_image;
		output_mask = Mat::zeros(input_image.size(), CV_8UC1);
		cvtColor(input_image, ycrcb_image, CV_BGR2YCrCb); //����ת���ɵ�YCrCb�ռ�
		
		for(int i = 0; i < input_image.rows; i++) //������ԲƤ��ģ�ͽ���Ƥ�����
		{
			uchar* p = (uchar*)output_mask.ptr<uchar>(i);
			Vec3b* ycrcb = (Vec3b*)ycrcb_image.ptr<Vec3b>(i);
			for(int j = 0; j < input_image.cols; j++)
			{
        skinCrCbHist2.at<int>(ycrcb[j][1], ycrcb[j][2])++;
				if(skinCrCbHist.at<uchar>(ycrcb[j][1], ycrcb[j][2]) > 0)
					p[j] = 255;
			}
		}

    double minv = 0.0, maxv = 0.0;
    log(skinCrCbHist2, skinCrCbHist2);
    minMaxIdx(skinCrCbHist2, &minv, &maxv);
    skinCrCbHist2.convertTo(skinCrCbHist3, CV_8UC1, 255./maxv, 0);
		morphologyEx(output_mask,output_mask,MORPH_CLOSE,element);
		
 
		vector< vector<Point> > contours;   // ����   
		vector< vector<Point> > filterContours; // ɸѡ�������
		vector< Vec4i > hierarchy;    // �����Ľṹ��Ϣ 
		contours.clear();  
		hierarchy.clear(); 
		filterContours.clear();
 
		findContours(output_mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);  
		// ȥ��α���� 
		for (size_t i = 0; i < contours.size(); i++) 
		{
			 if (fabs(contourArea(Mat(contours[i]))) > 1000/*&&fabs(arcLength(Mat(contours[i]),true))<2000*/)  //�ж��ֽ����������ֵ
				 filterContours.push_back(contours[i]);
		}
		
		output_mask.setTo(0);
		drawContours(output_mask, filterContours, -1, Scalar(255,0,0), CV_FILLED); //8, hierarchy);   
 
		input_image.copyTo(output_image, output_mask);
		imshow("input image", input_image);
		imshow("output image", output_image);
    imshow("skinCrCbHist3", skinCrCbHist3);
    imshow("skinCrCbHist2", skinCrCbHist2);
    imshow("skinCrCbHist", skinCrCbHist);
		
		output_image.setTo(0);
    if (27 == waitKey(30)) {
      break;
    }
	}
	return 0;
}

void test_skin_detect2()
{
  VideoCapture cam(0);
  if (!cam.isOpened())
    return;

  /*��ԲƤ��ģ��*/
  Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);
  ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);

  Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));

  while (true)
  {
    cam >> input_image;
    if (input_image.empty())
      return;

    Mat ycrcb_image;
    output_mask = Mat::zeros(input_image.size(), CV_8UC1);
    cvtColor(input_image, ycrcb_image, CV_BGR2YCrCb); //����ת���ɵ�YCrCb�ռ�

    for (int i = 0; i < input_image.rows; i++) //������ԲƤ��ģ�ͽ���Ƥ�����
    {
      uchar* p = (uchar*)output_mask.ptr<uchar>(i);
      Vec3b* ycrcb = (Vec3b*)ycrcb_image.ptr<Vec3b>(i);
      for (int j = 0; j < input_image.cols; j++)
      {
        if (skinCrCbHist.at<uchar>(ycrcb[j][1], ycrcb[j][2]) > 0)
          p[j] = 255;
      }
    }

    morphologyEx(output_mask, output_mask, MORPH_CLOSE, element);


    vector< vector<Point> > contours;   // ����   
    vector< vector<Point> > filterContours; // ɸѡ�������
    vector< Vec4i > hierarchy;    // �����Ľṹ��Ϣ 
    contours.clear();
    hierarchy.clear();
    filterContours.clear();

    findContours(output_mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    // ȥ��α���� 
    for (size_t i = 0; i < contours.size(); i++)
    {
      if (fabs(contourArea(Mat(contours[i]))) > 1000/*&&fabs(arcLength(Mat(contours[i]),true))<2000*/)  //�ж��ֽ����������ֵ
        filterContours.push_back(contours[i]);
    }

    output_mask.setTo(0);
    drawContours(output_mask, filterContours, -1, Scalar(255, 0, 0), CV_FILLED); //8, hierarchy);   

    input_image.copyTo(output_image, output_mask);
    imshow("input image", input_image);
    imshow("output image", output_image);

    output_image.setTo(0);
    if (27 == waitKey(30))
      return;
  }
  return;
}
