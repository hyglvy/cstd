#include "_cv.h"

typedef struct
{
    int bottom;
    int left;
    float height;
    float width;
    float base_a;
    float base_b;
}
icvMinAreaState;

#define CC_CALIPERS_MAXHEIGHT      0
#define CC_CALIPERS_MINAREARECT    1
#define CC_CALIPERS_MAXDIST        2

/*F///////////////////////////////////////////////////////////////////////////////////////
//    Name:    icvRotatingCalipers
//    Purpose:
//      Rotating calipers algorithm with some applications
//
//    Context:
//    Parameters:
//      points      - convex hull vertices (any orientation)
//      n           - number of vertices
//      mode        - concrete application of algorithm 
//                    can be  CC_CALIPERS_MAXDIST   or   
//                            CC_CALIPERS_MINAREARECT  
//      left, bottom, right, top - indexes of extremal points
//      out         - output info.
//                    In case CC_CALIPERS_MAXDIST it points to float value - 
//                    maximal height of polygon.
//                    In case CC_CALIPERS_MINAREARECT
//                    ((CPoint2D32f*)out)[0] - corner 
//                    ((CPoint2D32f*)out)[1] - vector1
//                    ((CPoint2D32f*)out)[0] - corner2
//                      
//                      ^
//                      |
//              vector2 |
//                      |
//                      |____________\
//                    corner         /
//                               vector1
//
//    Returns:
//    Notes:
//F*/

/* we will use usual cartesian coordinates */
static void
icvRotatingCalipers(CPoint2D32f* points, int n, int mode, float* out)
{
    float minarea = FLT_MAX;
    float max_dist = 0;
    char buffer[32];
    int i, k;
    CPoint2D32f* vect = (CPoint2D32f*)cAlloc(n * sizeof(vect[0]));
    float* inv_vect_length = (float*)cAlloc(n * sizeof(inv_vect_length[0]));
    int left = 0, bottom = 0, right = 0, top = 0;
    int seq[4] = { -1, -1, -1, -1 };

    /* rotating calipers sides will always have coordinates    
       (a,b) (-b,a) (-a,-b) (b, -a)     
     */
    /* this is a first base bector (a,b) initialized by (1,0) */
    float orientation = 0;
    float base_a;
    float base_b = 0;

    float left_x, right_x, top_y, bottom_y;
    CPoint2D32f pt0 = points[0];
    
    left_x = right_x = pt0.x;
    top_y = bottom_y = pt0.y;
    
    for(i = 0; i < n; i++)
    {
        double dx, dy;
        
        if(pt0.x < left_x)
            left_x = pt0.x, left = i;

        if(pt0.x > right_x)
            right_x = pt0.x, right = i;

        if(pt0.y > top_y)
            top_y = pt0.y, top = i;

        if(pt0.y < bottom_y)
            bottom_y = pt0.y, bottom = i;

        CPoint2D32f pt = points[(i+1) & (i+1 < n ? -1 : 0)];
        
        dx = pt.x - pt0.x;
        dy = pt.y - pt0.y;

        vect[i].x = (float)dx;
        vect[i].y = (float)dy;
        inv_vect_length[i] = (float)(1./sqrt(dx*dx + dy*dy));

        pt0 = pt;
    }

    //cvbInvSqrt(inv_vect_length, inv_vect_length, n);

    /* find convex hull orientation */
    {
        double ax = vect[n-1].x;
        double ay = vect[n-1].y;
        
        for(i = 0; i < n; i++)
        {
            double bx = vect[i].x;
            double by = vect[i].y;

            double convexity = ax * by - ay * bx;

            if(convexity != 0)
            {
                orientation = (convexity > 0) ? 1.f : (-1.f);
                break;
            }
            ax = bx;
            ay = by;
        }
        assert(orientation != 0);
    }
    base_a = orientation;

/*****************************************************************************************/
/*                         init calipers position                                        */
    seq[0] = bottom;
    seq[1] = right;
    seq[2] = top;
    seq[3] = left;
/*****************************************************************************************/
/*                         Main loop - evaluate angles and rotate calipers               */

    /* all of edges will be checked while rotating calipers by 90 degrees */
    for(k = 0; k < n; k++)
    {
        /* sinus of minimal angle */
        /*float sinus;*/

        /* compute cosine of angle between calipers side and polygon edge */
        /* dp - dot product */
        float dp0 = base_a * vect[seq[0]].x + base_b * vect[seq[0]].y;
        float dp1 = -base_b * vect[seq[1]].x + base_a * vect[seq[1]].y;
        float dp2 = -base_a * vect[seq[2]].x - base_b * vect[seq[2]].y;
        float dp3 = base_b * vect[seq[3]].x - base_a * vect[seq[3]].y;

        float cosalpha = dp0 * inv_vect_length[seq[0]];
        float maxcos = cosalpha;

        /* number of calipers edges, that has minimal angle with edge */
        int main_element = 0;

        /* choose minimal angle */
        cosalpha = dp1 * inv_vect_length[seq[1]];
        maxcos = (cosalpha > maxcos) ? (main_element = 1, cosalpha) : maxcos;
        cosalpha = dp2 * inv_vect_length[seq[2]];
        maxcos = (cosalpha > maxcos) ? (main_element = 2, cosalpha) : maxcos;
        cosalpha = dp3 * inv_vect_length[seq[3]];
        maxcos = (cosalpha > maxcos) ? (main_element = 3, cosalpha) : maxcos;

        /*rotate calipers*/
        {
            //get next base
            int pindex = seq[main_element];
            float lead_x = vect[pindex].x*inv_vect_length[pindex];
            float lead_y = vect[pindex].y*inv_vect_length[pindex];
            switch(main_element)
            {
            case 0:
                base_a = lead_x;
                base_b = lead_y;
                break;
            case 1:
                base_a = lead_y; 
                base_b = -lead_x;
                break;
            case 2:
                base_a = -lead_x;
                base_b = -lead_y;
                break;
            case 3:
                base_a = -lead_y;
                base_b = lead_x;
                break;
            default: assert(0);
            }
        }                        
        /* change base point of main edge */
        seq[main_element] += 1;
        seq[main_element] = (seq[main_element] == n) ? 0 : seq[main_element];

        
        switch (mode)
        {
        case CC_CALIPERS_MAXHEIGHT:
            {
                /* now main element lies on edge alligned to calipers side */

                /* find opposite element i.e. transform  */
                /* 0->2, 1->3, 2->0, 3->1                */
                int opposite_el = main_element ^ 2;

                float dx = points[seq[opposite_el]].x - points[seq[main_element]].x;
                float dy = points[seq[opposite_el]].y - points[seq[main_element]].y;
                float dist;

                if(main_element & 1)
                    dist = (float)fabs(dx * base_a + dy * base_b);
                else
                    dist = (float)fabs(dx * (-base_b) + dy * base_a);

                if(dist > max_dist)
                    max_dist = dist;

                break;
            }
        case CC_CALIPERS_MINAREARECT:
            /* find area of rectangle */
            {
                float height;
                float area;

                /* find vector left-right */
                float dx = points[seq[1]].x - points[seq[3]].x;
                float dy = points[seq[1]].y - points[seq[3]].y;

                /* dotproduct */
                float width = dx * base_a + dy * base_b;

                /* find vector left-right */
                dx = points[seq[2]].x - points[seq[0]].x;
                dy = points[seq[2]].y - points[seq[0]].y;

                /* dotproduct */
                height = -dx * base_b + dy * base_a;

                area = width * height;
                if(area <= minarea)
                {
                    float *buf = (float *) buffer;

                    minarea = area;
                    /* leftist point */
                    ((int *) buf)[0] = seq[3];
                    buf[1] = base_a;
                    buf[2] = width;
                    buf[3] = base_b;
                    buf[4] = height;
                    /* bottom point */
                    ((int *) buf)[5] = seq[0];
                    buf[6] = area;
                }
                break;
            }
        }                       /*switch */
    }                           /* for */

    switch (mode)
    {
    case CC_CALIPERS_MINAREARECT:
        {
            float *buf = (float *) buffer;

            float A1 = buf[1];
            float B1 = buf[3];

            float A2 = -buf[3];
            float B2 = buf[1];

            float C1 = A1 * points[((int *) buf)[0]].x + points[((int *) buf)[0]].y * B1;
            float C2 = A2 * points[((int *) buf)[5]].x + points[((int *) buf)[5]].y * B2;

            float idet = 1.f / (A1 * B2 - A2 * B1);

            float px = (C1 * B2 - C2 * B1) * idet;
            float py = (A1 * C2 - A2 * C1) * idet;

            out[0] = px;
            out[1] = py;

            out[2] = A1 * buf[2];
            out[3] = B1 * buf[2];

            out[4] = A2 * buf[4];
            out[5] = B2 * buf[4];
        }
        break;
    case CC_CALIPERS_MAXHEIGHT:
        {
            out[0] = max_dist;
        }
        break;
    }

    cFree(&vect);
    cFree(&inv_vect_length);
}


CC_IMPL  CBox2D
cvMinAreaRect2(const img_t* array, CvMemStorage* storage)
{
    CvMemStorage* temp_storage = 0;
    CBox2D box;
    CPoint2D32f* points = 0;
    
    CC_FUNCNAME("cvMinAreaRect2");

    memset(&box, 0, sizeof(box));

    __BEGIN__;

    int i, n;
    CvSeqReader reader;
    CvContour contour_header;
    CvSeqBlock block;
    CvSeq* ptseq = (CvSeq*)array;
    CPoint2D32f out[3];

    if(CC_IS_SEQ(ptseq))
    {
        if(!CC_IS_SEQ_POINT_SET(ptseq) &&
            (CC_SEQ_KIND(ptseq) != CC_SEQ_KIND_CURVE || !CC_IS_SEQ_CONVEX(ptseq) ||
            CC_SEQ_ELTYPE(ptseq) != CC_SEQ_ELTYPE_PPOINT))
            CC_ERROR(CC_StsUnsupportedFormat,
                "Input sequence must consist of 2d points or pointers to 2d points");
        if(!storage)
            storage = ptseq->storage;
    }
    else
    {
        CC_CALL(ptseq = cvPointSeqFromMat(
            CC_SEQ_KIND_GENERIC, array, &contour_header, &block));
    }

    if(storage)
    {
        CC_CALL(temp_storage = cvCreateChildMemStorage(storage));
    }
    else
    {
        CC_CALL(temp_storage = cvCreateMemStorage(1 << 10));
    }

    if(!CC_IS_SEQ_CONVEX(ptseq))
    {
        CC_CALL(ptseq = cvConvexHull2(ptseq, temp_storage, CC_CLOCKWISE, 1));
    }
    else if(!CC_IS_SEQ_POINT_SET(ptseq))
    {
        CvSeqWriter writer;
        
        if(!CC_IS_SEQ(ptseq->v_prev) || !CC_IS_SEQ_POINT_SET(ptseq->v_prev))
            CC_ERROR(CC_StsBadArg,
            "Convex hull must have valid pointer to point sequence stored in v_prev");
        cvStartReadSeq(ptseq, &reader);
        cvStartWriteSeq(CC_SEQ_KIND_CURVE|CC_SEQ_FLAG_CONVEX|CC_SEQ_ELTYPE(ptseq->v_prev),
                         sizeof(CvContour), CC_ELEM_SIZE(ptseq->v_prev->flags),
                         temp_storage, &writer);
            
        for(i = 0; i < ptseq->total; i++)
        {
            CPoint pt = **(CPoint**)(reader.ptr);
            CC_WRITE_SEQ_ELEM(pt, writer);
        }

        ptseq = cvEndWriteSeq(&writer);
    }

    n = ptseq->total;

    CC_CALL(points = (CPoint2D32f*)cAlloc(n*sizeof(points[0])));
    cvStartReadSeq(ptseq, &reader);

    if(CC_SEQ_ELTYPE(ptseq) == CC_32SC2)
    {
        for(i = 0; i < n; i++)
        {
            CPoint pt;
            CC_READ_SEQ_ELEM(pt, reader);
            points[i].x = (float)pt.x;
            points[i].y = (float)pt.y;
        }
    }
    else
    {
        for(i = 0; i < n; i++)
        {
            CC_READ_SEQ_ELEM(points[i], reader);
        }
    }
    
    if(n > 2)
    {
        icvRotatingCalipers(points, n, CC_CALIPERS_MINAREARECT, (float*)out);
        box.center.x = out[0].x + (out[1].x + out[2].x)*0.5f;
        box.center.y = out[0].y + (out[1].y + out[2].y)*0.5f;
        box.size.height = (float)sqrt((double)out[1].x*out[1].x + (double)out[1].y*out[1].y);
        box.size.width = (float)sqrt((double)out[2].x*out[2].x + (double)out[2].y*out[2].y);
        box.angle = (float)atan2(-(double)out[1].y, (double)out[1].x);
    }
    else if(n == 2)
    {
        box.center.x = (points[0].x + points[1].x)*0.5f;
        box.center.y = (points[0].y + points[1].y)*0.5f;
        double dx = points[1].x - points[0].x;
        double dy = points[1].y - points[0].y;
        box.size.height = (float)sqrt(dx*dx + dy*dy);
        box.size.width = 0;
        box.angle = (float)atan2(-dy, dx);
    }
    else
    {
        if(n == 1)
            box.center = points[0];
    }

    box.angle = (float)(box.angle*180/CC_PI);

    __END__; 

    cvReleaseMemStorage(&temp_storage);
    cFree(&points);

    return box;
}
