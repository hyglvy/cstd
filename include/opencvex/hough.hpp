
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

struct KVNode
{
  int index;
  int pixelSum;
};
//�������Ի���任ȱ�㣺���к�ʱ�����ܶ�λ�߶ε���ʼ��ֹ�㡣


void drawLine(cv::Mat& img, const Vec2f& lines, cv::Scalar& color, int thickness = 1, int lineType = 8)
{
  float rho = lines[0], theta = lines[1];
  Point pt1, pt2;
  double a = cos(theta), b = sin(theta);
  double x0 = a*rho, y0 = b*rho;
  pt1.x = cvRound(x0 + 2000 * (-b));  //�Ѹ�����ת��������
  pt1.y = cvRound(y0 + 2000 * (a));
  pt2.x = cvRound(x0 - 2000 * (-b));
  pt2.y = cvRound(y0 - 2000 * (a));
  line(img, pt1, pt2, color, thickness, lineType);
}


bool Compare(KVNode& aNode, KVNode& bNode)
{
  return (aNode.pixelSum > bNode.pixelSum);
}

 int HoughLinesByPoints(const Point* pts, int npt, int height, int width, float rho, float theta, int threshold, Vec2f* pLines, int linesMax)
{
	//tho��ʾ����̶ȣ�theta��ʾ�Ƕȿ̶ȣ�pLines��ʾ���ص�ֱ������ָ��,threshold��ʾֱ�ߵ���������ֵ��linesMax��ʾҪ���ص�������Ŀ��
	int* pAccm = NULL;
	KVNode* pKVNodeSort = NULL;
	float* tabSin = NULL;
	float* tabCos = NULL;
 
	int total = 0;
	int numAngle, numRho;
	int i, j;
	double scale = 0.0;
	float iRho = 1 / rho;
 
	numAngle = round(CV_PI / theta);
	numRho = round(((width + height) * 2 + 1) / rho);
 
	pAccm = new int[(numAngle + 2)*(numRho + 2)]{0};
	pKVNodeSort = new KVNode[numAngle*numRho];
 
	tabSin = new float[numAngle]{0.0f};
	tabCos = new float[numAngle]{0.0f};

	float ang = 0;//�����ظ�����Ƕȣ����ȼ����sin��i/�Ѻ�cos��i/��
	for (int n = 0; n < numAngle; ang += theta, n++)
	{
		tabSin[n] = sinf(ang)*iRho;
		tabCos[n] = cosf(ang)*iRho;
	}
	//��һ���������л����ռ�任���������д���ۼ���������
	for (int k = 0; k < npt; ++k)
  {
    int i = pts[k].y;
    int j = pts[k].x;
    for (int n = 0; n < numAngle; n++)
    {
      int r = round(j*tabCos[n] + i*tabSin[n]);
      r += (numRho - 1) / 2;
      ++pAccm[(n + 1)*(numRho + 2) + r + 1];
    }
  }
	//�ڶ������ҵ��ֲ�����ֵ�������ķ���ķǼ���ֵ����
	for (int r = 0; r < numRho; ++r)
	{
		for (int n = 0; n < numAngle; ++n)
		{
			int base = (n + 1)*(numRho + 2) + r + 1;
			if (pAccm[base]>threshold && pAccm[base]>pAccm[base - 1] && pAccm[base] > pAccm[base + 1] && pAccm[base] > pAccm[base - numRho - 2] && pAccm[base] > pAccm[base + numRho + 2])
			{
				KVNode tempKVNode;
				tempKVNode.index = base;
				tempKVNode.pixelSum = pAccm[base];
				pKVNodeSort[total++] = tempKVNode;
			}
		}
	}
 
	//�����������ۼ���ֵ���ɴ�С����
	sort(pKVNodeSort, pKVNodeSort + total, Compare);
 
	//���Ĳ������linesMax��ֱ�ߣ����linesMax����total,���������ֱ��
	linesMax = (linesMax <= total) ? linesMax : total;
	//pLines = new Vec2f[linesMax];
	scale = 1.0 / (numRho + 2);
  //linesMax = total;
	for (int i = 0; i < linesMax; ++i)
	{
		Vec2f tempVec2f;
		int index = pKVNodeSort[i].index;//������ü���ֵ�ڻ���ռ��е�λ��
		int n = floor(index*scale) - 1;
		int r = index - (n + 1)*(numRho + 2) - 1;
		tempVec2f[0] = (r - (numRho - 1)*0.5f)*rho;
		tempVec2f[1] = n*theta;
		pLines[i] = tempVec2f;
    //pLines.push_back(tempVec2f);
	}
  //linesMax = pLines.size();

  delete[]tabSin;
  delete[]tabCos;
  delete[]pKVNodeSort;
  delete[]pAccm;

  return linesMax;
}

 //int width, height;        //ͼ��Ŀ�͸�
static int icvHoughLinesProbabilistic(const Point* pts, int npt, int height, int width,
  float rho, float theta, int threshold,
  int lineLength, int lineGap,
  vector<Vec4i>& lines, int linesMax)
{
   //accumΪ�ۼ�������maskΪ�������
   Mat accum, mask;
   vector<float> trigtab;    //���ڴ洢���ȼ���õ����Һ�����ֵ
                             //����һ���ڴ�ռ�

   int numangle, numrho;     //�ǶȺ;������ɢ����
   float ang;
   int r, n, count;
   CvPoint pt;
   float irho = 1 / rho;     //����ֱ��ʵĵ���
   CvRNG rng = cvRNG(-1);    //�����
   const float* ttab;        //����trigtab�ĵ�ַָ��
   uchar* mdata0;    //����mask�ĵ�ַָ��
                     //ȷ������ͼ�����ȷ��
   //CV_Assert(CV_IS_MAT(image) && CV_MAT_TYPE(image->type) == CV_8UC1);

                            //�ɽǶȺ;���ֱ��ʣ��õ��ǶȺ;������ɢ����
   numangle = cvRound(CV_PI / theta);
   numrho = cvRound(((width + height) * 2 + 1) / rho);
   //�����ۼ������󣬼�����ռ�
   accum.create(numangle, numrho, CV_32SC1);
   //����������󣬴�С������ͼ����ͬ
   mask.create(height, width, CV_8UC1);
   //����trigtab�Ĵ�С����ΪҪ�洢���Һ�����ֵ�����Գ���Ϊ�Ƕ���ɢ����2��
   trigtab.resize(numangle * 2);
   //�ۼ�����������
   mask = cv::Scalar(1);
   accum = cv::Scalar(0);
   //�����ظ����㣬���ȼ����������������Һ�����ֵ
   for (ang = 0, n = 0; n < numangle; ang += theta, n++)
   {
     trigtab[n * 2] = (float)(cos(ang) * irho);
     trigtab[n * 2 + 1] = (float)(sin(ang) * irho);
   }
   //��ֵ�׵�ַ
   ttab = &trigtab[0];
   mdata0 = mask.data;

   // stage 1. collect non-zero image points
   //�ռ�ͼ���е����з���㣬��Ϊ����ͼ���Ǳ�Եͼ�����Է������Ǳ�Ե��
   if (0) {
     for (int i = 0; i < npt; ++i)
     {
       //��ȡ������ͼ�����������ÿ�е�ַָ��
       Point pt = pts[i];
       if (pt.x > 0 && pt.y > 0 && pt.x < width - 1 && pt.y < height - 1) {
         uchar* mdata = mdata0 + pt.y*width + pt.x;
         uchar* mdata1 = mdata - width;
         uchar* mdata2 = mdata + width;
         //�������Ӧλ����0
         mdata[0] = 1;
         mdata[1] = 1;
         mdata[-1] = 1;

         mdata1[0] = 1;
         mdata1[1] = 1;
         mdata1[-1] = 1;

         mdata2[0] = 1;
         mdata2[1] = 1;
         mdata2[-1] = 1;
       }
     }
   }
   //��ֹд���У�seqΪ���б�Ե������λ�õ�����
   count = npt;    //�õ���Ե�������
   CvPoint* seq = (CvPoint*)malloc(npt * sizeof(CvPoint));
   memcpy(seq, pts, npt * sizeof(CvPoint));
   // stage 2. process all the points in random order
   //����������еı�Ե��
   for (; count > 0; count--)
   {
     // choose random point out of the remaining ones
     //����1����ʣ�µı�Ե�������ѡ��һ���㣬idxΪ������count�������
     int idx = cvRandInt(&rng) % count;
     //max_valΪ�ۼ��������ֵ��max_nΪ���ֵ����Ӧ�ĽǶ�
     int max_val = threshold - 1, max_n = 0;
     //�������idx����������ȡ������Ӧ�������
     CvPoint* point = seq + idx;
     //����ֱ�ߵ������˵�
     CvPoint line_end[2] = { { 0,0 },{ 0,0 } };
     float a, b;
     //�ۼ����ĵ�ַָ�룬Ҳ���ǻ���ռ�ĵ�ַָ��
     int* adata = (int*)accum.data;
     int i, j, k, x0, y0, dx0, dy0, xflag;
     int good_line;
     const int shift = 16;
     //��ȡ�������ĺᡢ������
     i = point->y;
     j = point->x;

     // "remove" it by overriding it with the last element
     //�������е����һ��Ԫ�ظ��ǵ��ղ���ȡ��������������
     *point = *(CvPoint*)(seq + count - 1);

     // check if it has been excluded already (i.e. belongs to some other line)
     //������������Ƿ��Ѿ��������Ҳ�������Ѿ���������ֱ��
     //��Ϊ����������������������mask�����Ӧλ������
     if (!mdata0[i*width + j])    //������㱻�����
       continue;    //�����κδ���������ѭ��

                    // update accumulator, find the most probable line
                    //����2�������ۼ��������ҵ����п��ܵ�ֱ��
     for (n = 0; n < numangle; n++, adata += numrho)
     {
       //�ɽǶȼ������
       r = cvRound(j * ttab[n * 2] + i * ttab[n * 2 + 1]);
       r += (numrho - 1) / 2;
       //���ۼ����������Ӧλ������ֵ��1������ֵ��val
       int val = ++adata[r];
       //�������ֵ�����õ����ĽǶ�
       if (max_val < val)
       {
         max_val = val;
         max_n = n;
       }
     }

     // if it is too "weak" candidate, continue with another point
     //����3���������õ������ֵС����ֵ��������õ㣬������һ����ļ���
     if (max_val < threshold)
       continue;

     // from the current point walk in each direction
     // along the found line and extract the line segment
     //����4���ӵ�ǰ�����������������ֱ�ߵķ���ǰ����ֱ���ﵽ�˵�Ϊֹ
     a = -ttab[max_n * 2 + 1];    //a=-sin��
     b = ttab[max_n * 2];    //b=cos��
                             //��ǰ��ĺᡢ������ֵ
     x0 = j;
     y0 = i;
     //ȷ����ǰ������ֱ�ߵĽǶ�����45�ȡ�135��֮�䣬������0��45��135�ȡ�180��֮��
     if (fabs(a) > fabs(b))    //��45�ȡ�135��֮��
     {
       xflag = 1;    //�ñ�ʶλ����ʶֱ�ߵĴ��Է���
                     //ȷ���ᡢ�������λ����
       dx0 = a > 0 ? 1 : -1;
       dy0 = cvRound(b*(1 << shift) / fabs(a));
       //ȷ��������
       y0 = (y0 << shift) + (1 << (shift - 1));
     }
     else    //��0��45��135�ȡ�180��֮��
     {
       xflag = 0;   //���ʶλ
                    //ȷ���ᡢ�������λ����
       dy0 = b > 0 ? 1 : -1;
       dx0 = cvRound(a*(1 << shift) / fabs(b));
       //ȷ��������
       x0 = (x0 << shift) + (1 << (shift - 1));
     }
     //����ֱ�ߵ������˵�
     for (k = 0; k < 2; k++)
     {
       //gap��ʾ����ֱ�ߵļ�϶��x��yΪ����λ�ã�dx��dyΪλ����
       int gap = 0, x = x0, y = y0, dx = dx0, dy = dy0;
       //�����ڶ����˵��ʱ�򣬷�����λ��
       if (k > 0)
         dx = -dx, dy = -dy;

       // walk along the line using fixed-point arithmetics,
       // stop at the image border or in case of too big gap
       //����ֱ�ߵķ���λ�ƣ�ֱ������ͼ��ı߽���ļ�϶Ϊֹ
       for (;; x += dx, y += dy)
       {
         uchar* mdata;
         int i1, j1;
         //ȷ���µ�λ�ƺ������λ��
         if (xflag)
         {
           j1 = x;
           i1 = y >> shift;
         }
         else
         {
           j1 = x >> shift;
           i1 = y;
         }
         //���������ͼ��ı߽磬ֹͣλ�ƣ��˳�ѭ��
         if (j1 < 0 || j1 >= width || i1 < 0 || i1 >= height)
           break;
         //��λλ�ƺ��������λ��
         mdata = mdata0 + i1*width + j1;

         // for each non-zero point:
         //    update line end,
         //    clear the mask element
         //    reset the gap
         //�����벻Ϊ0��˵���õ��������ֱ����
         if (*mdata)
         {
           gap = 0;    //���ü�϶Ϊ0
                       //����ֱ�ߵĶ˵�λ��
           line_end[k].y = i1;
           line_end[k].x = j1;
         }
         //����Ϊ0��˵������ֱ�ߣ����Լ���λ�ƣ�ֱ����϶���������õ���ֵΪֹ
         else if (++gap > lineGap)    //��϶��1
           break;
       }
     }
     //����5���ɼ�⵽��ֱ�ߵ������˵���Լ���ֱ�ߵĳ���
     //��ֱ�߳��ȴ��������õ���ֵʱ��good_lineΪ1������Ϊ0
     good_line = abs(line_end[1].x - line_end[0].x) >= lineLength ||
       abs(line_end[1].y - line_end[0].y) >= lineLength;
     //�ٴ������˵㣬Ŀ���Ǹ����ۼ�������͸�����������Ա���һ��ѭ��ʹ��
     for (k = 0; k < 2; k++)
     {
       int x = x0, y = y0, dx = dx0, dy = dy0;

       if (k > 0)
         dx = -dx, dy = -dy;

       // walk along the line using fixed-point arithmetics,
       // stop at the image border or in case of too big gap
       for (;; x += dx, y += dy)
       {
         uchar* mdata;
         int i1, j1;

         if (xflag)
         {
           j1 = x;
           i1 = y >> shift;
         }
         else
         {
           j1 = x >> shift;
           i1 = y;
         }

         mdata = mdata0 + i1*width + j1;

         // for each non-zero point:
         //    update line end,
         //    clear the mask element
         //    reset the gap
         if (*mdata)
         {
           //if���������������Щ�Ѿ��ж��Ǻõ�ֱ���ϵĵ��Ӧ���ۼ�����ֵ�������ٴ�������Щ�ۼ�ֵ
           if (good_line)    //�ڵ�һ���������Ѿ�ȷ���Ǻõ�ֱ��
           {
             //�õ��ۼ��������ַָ��
             adata = (int*)accum.data;
             for (n = 0; n < numangle; n++, adata += numrho)
             {
               r = cvRound(j1 * ttab[n * 2] + i1 * ttab[n * 2 + 1]);
               r += (numrho - 1) / 2;
               adata[r]--;    //��Ӧ���ۼ�����1
             }
           }
           //��������λ�ã������Ǻõ�ֱ�ߣ����ǻ���ֱ�ߣ�������Ӧλ�ö���0�������´ξͲ������ظ�������Щλ���ˣ��Ӷ��ﵽ��С�����Ե���Ŀ��
           *mdata = 0;
         }
         //����Ѿ�������ֱ�ߵĶ˵㣬���˳�ѭ��
         if (i1 == line_end[k].y && j1 == line_end[k].x)
           break;
       }
     }
     //����Ǻõ�ֱ��
     if (good_line)
     {
       Vec4i lr(line_end[0].x, line_end[0].y, line_end[1].x, line_end[1].y );
       //�������˵�ѹ��������
       //cvSeqPush(lines, &lr);
       lines.push_back(lr);
       //�����⵽��ֱ������������ֵ�����˳��ú���
       if (lines.size() >= linesMax)
         return linesMax;
     }
   }
   free(seq);
   return lines.size();
 }


