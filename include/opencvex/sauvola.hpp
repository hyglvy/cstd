//����opencv��sauvola��ֵ�㷨
//sauvola�㷨��һ���Ƚϲ���ľֲ���ֵ���㷨���в�����Ҳ����ʵ��������룬���Ǳ��˾����������һЩbug�Ͳ�����opencv���ã����Զ����ʵ����޸ġ� 
//�����޸�֮����Ȼ���Ǵ���һЩ���⣬���������ҵ�һ���Ƚϴ����������ȥ�洢�����bug���Ժ��п��ٴ�������ͨ���޸ĺ�Ĵ������ѧϰ�����Ǻܲ���ġ��������£�


#include "opencv2/opencv.hpp"

static int CalcMaxValue(int a, int b)
{
    return (a > b) ? a : b;
}

static double CalcMaxValue(double a, double b)
{
    return (a > b) ? a : b;
}

static int CalcMinValue(int a, int b)
{
    return (a < b) ? a : b;
}

static double CalcMinValue(double a, double b)
{
    return (a < b) ? a : b;
}


/** @brief SauvolaThresh��ֵ�㷨

�˴��벻������ֱ��ʽϴ��ͼ��, ��bug׼���п��ٴ���

@param src ��ͨ���Ҷ�ͼ
@param dst ��ͨ��������ͼ
@param k  threshold = mean*(1 + k*((std / 128) - 1))
@param wndSize ����������, һ��������

*/
void SauvolaThresh(const cv::Mat& src, cv::Mat& dst, const int k, const cv::Size wndSize)
{
    CV_Assert(src.type() == CV_8UC1);
    CV_Assert((wndSize.width % 2 == 1) && (wndSize.height % 2 == 1));
    CV_Assert((wndSize.width <= src.cols) && (wndSize.height <= src.rows));

    dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

    // ������־λͼ��
    unsigned long* integralImg = new unsigned long[src.rows * src.cols];
    unsigned long* integralImgSqrt = new unsigned long[src.rows * src.cols];
    std::memset(integralImg, 0, src.rows *src.cols*sizeof(unsigned long));
    std::memset(integralImgSqrt, 0, src.rows *src.cols*sizeof(unsigned long));

    // ����ֱ��ͼ��ͼ��ֵƽ���ĺ�
    for (int y = 0; y < src.rows; ++y)
    {
        unsigned long sum = 0;
        unsigned long sqrtSum = 0;
        for (int x = 0; x < src.cols; ++x)
        {
            int index = y * src.cols + x;
            sum += src.at<uchar>(y, x);
            sqrtSum += src.at<uchar>(y, x)*src.at<uchar>(y, x);
            if (y == 0)
            {
                integralImg[index] = sum;
                integralImgSqrt[index] = sqrtSum;
            }
            else
            {
                integralImgSqrt[index] = integralImgSqrt[(y - 1)*src.cols + x] + sqrtSum;
                integralImg[index] = integralImg[(y - 1)*src.cols + x] + sum;
            }
        }
    }

    double diff         = 0.0;
    double sqDiff       = 0.0;
    double diagSum      = 0.0;
    double iDiagSum     = 0.0;
    double sqDiagSum    = 0.0;
    double sqIDiagSum   = 0.0;
    for (int x = 0; x < src.cols; ++x)
    {
        for (int y = 0; y < src.rows; ++y)
        {
            int xMin = CalcMaxValue(0, x - wndSize.width / 2);
            int yMin = CalcMaxValue(0, y - wndSize.height / 2);
            int xMax = CalcMinValue(src.cols - 1, x + wndSize.width / 2);
            int yMax = CalcMinValue(src.rows - 1, y + wndSize.height / 2);
            double area = (xMax - xMin + 1)*(yMax - yMin + 1);
            if (area <= 0)
            {
                // blog�ṩԴ����biImage[i * IMAGE_WIDTH + j] = 255;����i��ʾ������, j����
                dst.at<uchar>(y, x) = 255;
                continue;
            }

            if (xMin == 0 && yMin == 0)
            {
                diff = integralImg[yMax*src.cols + xMax];
                sqDiff = integralImgSqrt[yMax*src.cols + xMax];
            }
            else if (xMin > 0 && yMin == 0)
            {
                diff = integralImg[yMax*src.cols + xMax] - integralImg[yMax*src.cols + xMin - 1];
                sqDiff = integralImgSqrt[yMax * src.cols + xMax] - integralImgSqrt[yMax * src.cols + xMin - 1];
            }
            else if (xMin == 0 && yMin > 0)
            {
                diff = integralImg[yMax * src.cols + xMax] - integralImg[(yMin - 1) * src.cols + xMax];
                sqDiff = integralImgSqrt[yMax * src.cols + xMax] - integralImgSqrt[(yMin - 1) * src.cols + xMax];;
            }
            else
            {
                diagSum = integralImg[yMax * src.cols + xMax] + integralImg[(yMin - 1) * src.cols + xMin - 1];
                iDiagSum = integralImg[(yMin - 1) * src.cols + xMax] + integralImg[yMax * src.cols + xMin - 1];
                diff = diagSum - iDiagSum;
                sqDiagSum = integralImgSqrt[yMax * src.cols + xMax] + integralImgSqrt[(yMin - 1) * src.cols + xMin - 1];
                sqIDiagSum = integralImgSqrt[(yMin - 1) * src.cols + xMax] + integralImgSqrt[yMax * src.cols + xMin - 1];
                sqDiff = sqDiagSum - sqIDiagSum;
            }
            double mean = diff / area;
            double stdValue = sqrt((sqDiff - diff*diff / area) / (area - 1));
            double threshold = mean*(1 + k*((stdValue / 128) - 1));
            if (src.at<uchar>(y, x) < threshold)
            {
                dst.at<uchar>(y, x) = 0;
            }
            else
            {
                dst.at<uchar>(y, x) = 255;
            }

        }
    }

    delete[] integralImg;
    delete[] integralImgSqrt;
}
