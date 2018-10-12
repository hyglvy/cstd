
/** @brief �õ������е����ֵ����Сֵ

@param m ��ͨ��CV_8UC1���;���
@param maxValue ���ֵ
@param minValue ��Сֵ
*/
static void GetMatMaxMin(const cv::Mat& m, int& maxValue, int& minValue)
{
  CV_Assert(m.type() == CV_8UC1);

  maxValue = INT_MIN;
  minValue = INT_MAX;

  for (int y = 0; y < m.rows; ++y)
  {
    for (int x = 0; x < m.cols; ++x)
    {
      int v = m.at<uchar>(y, x);
      if (v > maxValue) maxValue = v;
      if (v < minValue) minValue = v;
    }
  }
}

void Bernsen(const cv::Mat & src, cv::Mat & dst, cv::Size wndSize)
{
  CV_Assert(src.type() == CV_8UC1);
  wndSize.width |= 1;
  wndSize.height |= 1;
  CV_Assert((wndSize.width % 2 == 1) && (wndSize.height % 2 == 1));
  CV_Assert((wndSize.width <= src.cols) && (wndSize.height <= src.rows));

  cv::Mat meanMat = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

  for (int y = wndSize.height / 2; y <= src.rows - wndSize.height / 2 - 1; ++y)
  {
    for (int x = wndSize.width / 2; x <= src.cols - wndSize.width / 2 - 1; ++x)
    {
      int value = src.at<uchar>(y, x);
      cv::Point center = cv::Point(x, y);
      cv::Point topLeftPoint = cv::Point(x - wndSize.width / 2, y - wndSize.height / 2);
      cv::Rect wnd = cv::Rect(topLeftPoint.x, topLeftPoint.y, wndSize.width, wndSize.height);
      int maxValue = 0;
      int minValue = 0;
      cv::Mat roiMat = src(wnd);
      GetMatMaxMin(roiMat, maxValue, minValue);
      int meanValue = (maxValue + minValue) / 2.0;
      meanMat.at<uchar>(y, x) = meanValue;
    }
  }

  // ��ֵ�ָ�
  dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
  for (int y = 0; y < src.rows; ++y)
  {
    for (int x = 0; x < src.cols; ++x)
    {
      int value = src.at<uchar>(y, x);
      int meanValue = meanMat.at<uchar>(y, x);
      if (value > meanValue)
      {
        dst.at<uchar>(y, x) = 255;
      }
      else
      {
        dst.at<uchar>(y, x) = 0;
      }
    }
  }
}

void Bernsen(const cv::Mat & src, cv::Mat & dst, cv::Size wndSize, int differMax, int meanMax)
{
  CV_Assert(src.type() == CV_8UC1);
  CV_Assert((wndSize.width % 2 == 1) && (wndSize.height % 2 == 1));
  CV_Assert((wndSize.width <= src.cols) && (wndSize.height <= src.rows));

  // �����ֵ����Ͳ������
  cv::Mat meanMat = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
  cv::Mat differMat = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
  for (int y = wndSize.height / 2; y <= src.rows - wndSize.height / 2 - 1; ++y)
  {
    for (int x = wndSize.width / 2; x <= src.cols - wndSize.width / 2 - 1; ++x)
    {
      int value = src.at<uchar>(y, x);
      cv::Point center = cv::Point(x, y);
      cv::Point topLeftPoint = cv::Point(x - wndSize.width / 2, y - wndSize.height / 2);
      cv::Rect wnd = cv::Rect(topLeftPoint.x, topLeftPoint.y, wndSize.width, wndSize.height);
      int maxValue = 0;
      int minValue = 0;
      cv::Mat roiMat = src(wnd);
      GetMatMaxMin(roiMat, maxValue, minValue);
      int meanValue = (maxValue + minValue) / 2.0;
      int differValue = maxValue - minValue;
      meanMat.at<uchar>(y, x) = meanValue;
      differMat.at<uchar>(y, x) = differValue;
    }
  }

  // ��ֵ
  dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
  for (int y = 0; y < differMat.rows; ++y)
  {
    for (int x = 0; x < differMat.cols; ++x)
    {
      int differValue = differMat.at<uchar>(y, x);
      if (differValue > differMax)
      {
        // blogд�ĺ��Ժ�, ֱ˵meanValue����ֵ
        // ������Ϊ�Ǳ߽粿��,������0,Ҳ������255
        dst.at<uchar>(y, x) = 255;
      }
      else if (differValue < differMax)
      {
        int meanValue = meanMat.at<uchar>(y, x);
        if (meanValue > meanMax)
        {
          dst.at<uchar>(y, x) = 255;
        }
        else
        {
          dst.at<uchar>(y, x) = 0;
        }
      }
      else
      {
        // TODO
        dst.at<uchar>(y, x) = 0;
      }
    }
  }
}
