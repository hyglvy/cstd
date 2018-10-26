// --------------------------------------------------------
// Dragon
// Copyright(c) 2017 SeetaTech
// Written by Ting Pan
// --------------------------------------------------------

#ifndef DRAGON_CONTRIB_RCNN_BBOX_UTILS_H_
#define DRAGON_CONTRIB_RCNN_BBOX_UTILS_H_

#include "core/context.h"
#include "core/operator.h"

namespace dragon {

namespace rcnn {

#define ROUND(x) ((int)((x) + (T)0.5))

/******************** BBox ********************/

template <typename T>
int BBoxTransform(const T dx, const T dy,
                  const T d_log_w, const T d_log_h,
                  const T im_w, const T im_h,
                  const T min_box_w, const T min_box_h,
                  T* bbox) {
    const T w = bbox[2] - bbox[0] + (T)1;
    const T h = bbox[3] - bbox[1] + (T)1;
    const T ctr_x = bbox[0] + (T)0.5 * w;
    const T ctr_y = bbox[1] + (T)0.5 * h;

    const T pred_ctr_x = dx * w + ctr_x;
    const T pred_ctr_y = dy * h + ctr_y;
    const T pred_w = exp(d_log_w) * w;
    const T pred_h = exp(d_log_h) * h;

    bbox[0] = pred_ctr_x - (T)0.5 * pred_w;
    bbox[1] = pred_ctr_y - (T)0.5 * pred_h;
    bbox[2] = pred_ctr_x + (T)0.5 * pred_w;
    bbox[3] = pred_ctr_y + (T)0.5 * pred_h;

    bbox[0] = std::max((T)0, std::min(bbox[0], im_w - (T)1));
    bbox[1] = std::max((T)0, std::min(bbox[1], im_h - (T)1));
    bbox[2] = std::max((T)0, std::min(bbox[2], im_w - (T)1));
    bbox[3] = std::max((T)0, std::min(bbox[3], im_h - (T)1));

    const T bbox_w = bbox[2] - bbox[0] + (T)1;
    const T bbox_h = bbox[3] - bbox[1] + (T)1;
    return (bbox_w >= min_box_w) * (bbox_h >= min_box_h);
}

/******************** Anchor ********************/

template <typename T>
void GenerateAnchors(int base_size,
                     const int num_ratios,
                     const int num_scales,
                     const T* ratios,
                     const T* scales,
                     T* anchors) {
    const T base_area = (T)(base_size * base_size);
    const T center = (T)0.5 * (base_size - (T)1);
    T* offset_anchors = anchors;
    for (int i = 0; i < num_ratios; ++i) {
        const T ratio_w = (T)ROUND(sqrt(base_area / ratios[i]));
        const T ratio_h = (T)ROUND(ratio_w * ratios[i]);
        for (int j = 0; j < num_scales; ++j) {
            const T scale_w = (T)0.5 * (ratio_w * scales[j] - (T)1);
            const T scale_h = (T)0.5 * (ratio_h * scales[j] - (T)1);
            offset_anchors[0] = center - scale_w;
            offset_anchors[1] = center - scale_h;
            offset_anchors[2] = center + scale_w;
            offset_anchors[3] = center + scale_h;
            offset_anchors += 4;
        }
    }
}

template <typename T>
void GenerateGridAnchors(const int A, const int feat_h, const int feat_w,
                         const int stride,
                         const T* anchors,
                         T* proposals) {
    T* proposal = proposals;
    for (int a = 0; a < A; ++a) {
        for (int h = 0; h < feat_h; ++h) {
            for (int w = 0; w < feat_w; ++w) {
                const T x = w * stride;
                const T y = h * stride;
                proposal[0] = x + anchors[a * 4 + 0];
                proposal[1] = y + anchors[a * 4 + 1];
                proposal[2] = x + anchors[a * 4 + 2];
                proposal[3] = y + anchors[a * 4 + 3];
                proposal += 5;
            }
        }
    }
}

/******************** Proposal ********************/

template <typename T, class Context>
void GenerateProposals(const int A, const int feat_h, const int feat_w,
                       const int stride,
                       const float im_h, const float im_w,
                       const float min_box_h, const float min_box_w,
                       const T* scores,
                       const T* bbox_deltas,
                       const T* anchors,
                       T* proposals);

template <typename T, class Context>
void GenerateProposals_v2(const int total_anchors,
                          const float im_h, const float im_w,
                          const float min_box_h, const float min_box_w,
                          const T* scores,
                          const T* bbox_deltas,
                          T* proposals);

template <typename T>
void SortProposals(const int start,
                   const int end,
                   const int num_top,
                   T* proposals) {
    const T pivot_score = proposals[start * 5 + 4];
    int left = start + 1, right = end;
    while (left <= right) {
        while (left <= end && proposals[left * 5 + 4] >= pivot_score) ++left;
        while (right > start && proposals[right * 5 + 4] <= pivot_score) --right;
        if (left <= right) {
            for (int i = 0; i < 5; ++i)
                std::swap(proposals[left * 5 + i], proposals[right * 5 + i]);
            ++left;
            --right;
        }
    }
    if (right > start) {
        for (int i = 0; i < 5; ++i)
            std::swap(proposals[start * 5 + i], proposals[right * 5 + i]);
    }
    if (start < right - 1) SortProposals(start, right - 1, num_top, proposals);
    if (right + 1 < num_top && right + 1 < end) SortProposals(right + 1, end, num_top, proposals);
}

template <typename T>
void RetrieveRoIs(const int num_rois,
                  const int roi_batch_ind,
                  const T* proposals,
                  const int* roi_indices,
                  T* rois) {
    for (int i = 0; i < num_rois; ++i) {
        const T* proposal = proposals + roi_indices[i] * 5;
        rois[i * 5 + 0] = roi_batch_ind;
        rois[i * 5 + 1] = proposal[0];
        rois[i * 5 + 2] = proposal[1];
        rois[i * 5 + 3] = proposal[2];
        rois[i * 5 + 4] = proposal[3];
    }
}

template <typename T>
int roi_level(const int min_level,  //  e.g. 2
              const int max_level,  //  e.g. 5
              const int canonical_level,  //  e.g. 4
              const int canonical_scale,  //  e.g. 224
              T* roi) {
    T w = roi[3] - roi[1] + 1;
    T h = roi[4] - roi[2] + 1;
    //  reference the settings of paper
    int level = canonical_level + std::log(std::max(std::sqrt(w * h), T(1)) / T(canonical_scale));
    return std::min(max_level, std::max(min_level, level));
}

template <typename T>
void CollectRoIs(const int num_rois,
                 const int min_level,
                 const int max_level,
                 const int canonical_level,
                 const int canonical_scale,
                 const T* rois,
                 vector< vector<TIndex> >& roi_bins) {
    const T* roi = rois;
    for (int i = 0; i < num_rois; ++i) {
        int bin_idx = roi_level(min_level, max_level,
                    canonical_level, canonical_scale,
                                                roi);
        bin_idx = std::max(bin_idx - min_level, 0);
        roi_bins[bin_idx].push_back(i);
        roi += 5;
    }
}

template <typename T>
void DistributeRoIs(const vector< vector<TIndex> >& roi_bins,
                    const T* rois,
                    vector<T*> outputs) {
    for (int i = 0; i < roi_bins.size(); i++) {
        auto* y = outputs[i];
        if (roi_bins[i].size() == 0) {
            //  fake a tiny roi to avoid empty roi pooling
            y[0] = 0, y[1] = 0, y[2] = 0, y[3] = 1, y[4] = 1;
        } else {
            for (int j = 0; j < roi_bins[i].size(); ++j) {
                const T* roi = rois + roi_bins[i][j] * 5;
                for (int k = 0; k < 5; ++k) y[k] = roi[k];
                y += 5;
            }
        }
    }
}

/******************** NMS ********************/

template <typename T, class Context>
void NMS(const int num_boxes,
         const int max_keeps,
         const T thresh,
         const T* proposals,
         int* roi_indices,
         int& num_rois,
         Tensor* mask);

}    // namespace rcnn

}    // namespace dragon

#endif    //  DRAGON_OPERATORS_CONTRIB_BBOX_UTILS_H_