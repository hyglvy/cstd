
//�ֲ���ֵ���㷨Niblack OpenCVʵ��

//Niblack �㷨��Ӧ�����ı�ͼ���ֵ������϶࣬���ǱȽϾ���ľֲ���ֵ������������ֲ���ֵ�����������Ҳ���н�����壬����������һЩ����Ľ�������Sauvola �㷨��Nick �㷨��
//����˼���ǣ�����ͼ�����ص�������ڵ�ƽ���ҶȺͱ�׼ƫ��������һ����ֵ������ж�ֵ����������ֵ�������£�

//point 1�����㴰�������ؾ�ֵ��
//point 2�����㴰�������ر�׼�
//��������ļ��㷽�����˶��졣

void niBlackThreshold(InputArray _src, OutputArray _dst, double maxValue,
  int type, int blockSize, double delta)
{
  // Input grayscale image
  Mat src = _src.getMat();
  CV_Assert(src.channels() == 1);
  CV_Assert(blockSize % 2 == 1 && blockSize > 1);
  type &= THRESH_MASK;

  // Compute local threshold (T = mean + k * stddev)
  // using mean and standard deviation in the neighborhood of each pixel
  // (intermediate calculations are done with floating-point precision)
  Mat thresh;
  {
    // note that: Var[X] = E[X^2] - E[X]^2
    Mat mean, sqmean, stddev;
    boxFilter(src, mean, CV_32F, Size(blockSize, blockSize),
      Point(-1, -1), true, BORDER_REPLICATE);
    sqrBoxFilter(src, sqmean, CV_32F, Size(blockSize, blockSize),
      Point(-1, -1), true, BORDER_REPLICATE);
    sqrt(sqmean - mean.mul(mean), stddev);
    thresh = mean + stddev * static_cast<float>(delta);
    thresh.convertTo(thresh, src.depth());
  }

  // Prepare output image
  _dst.create(src.size(), src.type());
  Mat dst = _dst.getMat();
  CV_Assert(src.data != dst.data);  // no inplace processing

  // Apply thresholding: ( pixel > threshold ) ? foreground : background
  Mat mask;
  switch (type)
  {
  case THRESH_BINARY:      // dst = (src > thresh) ? maxval : 0
  case THRESH_BINARY_INV:  // dst = (src > thresh) ? 0 : maxval
    compare(src, thresh, mask, (type == THRESH_BINARY ? CMP_GT : CMP_LE));
    dst.setTo(0);
    dst.setTo(maxValue, mask);
    break;
  case THRESH_TRUNC:       // dst = (src > thresh) ? thresh : src
    compare(src, thresh, mask, CMP_GT);
    src.copyTo(dst);
    thresh.copyTo(dst, mask);
    break;
  case THRESH_TOZERO:      // dst = (src > thresh) ? src : 0
  case THRESH_TOZERO_INV:  // dst = (src > thresh) ? 0 : src
    compare(src, thresh, mask, (type == THRESH_TOZERO ? CMP_GT : CMP_LE));
    dst.setTo(0);
    src.copyTo(dst, mask);
    break;
  default:
    CV_Error(CV_StsBadArg, "Unknown threshold type");
    break;
  }
}

