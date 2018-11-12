#ifndef CAFFE_SCALE_LAYER_HPP_
#define CAFFE_SCALE_LAYER_HPP_







#include "caffe/layers/bias_layer.hpp"

namespace
{

  /**
   * @brief Computes a product of two input Blobs, with the shape of the
   *        latter Blob "broadcast" to match the shape of the former.
   *        Equivalent to tiling the latter Blob, then computing the elementwise
   *        product.
   *
   * The second input may be omitted, in which case it's learned as a parameter
   * of the layer.
   */
  template <typename Dtype>
  class ScaleLayer: public Layer
  {
  public:
    explicit ScaleLayer()
      : Layer() {}
    virtual void LayerSetUp(const vector<Blob*> & bottom,
                            const vector<Blob*> & top);
    virtual void Reshape(const vector<Blob*> & bottom,
                         const vector<Blob*> & top);

    virtual inline const char* type() const { return "Scale"; }
    // Scale
    virtual inline int MinBottomBlobs() const { return 1; }
    virtual inline int MaxBottomBlobs() const { return 2; }
    virtual inline int ExactNumTopBlobs() const { return 1; }

  public:
    /**
     * In the below shape specifications, @f$ i @f$ denotes the value of the
     * `axis` field given by `this->param_->scale_param().axis()`, after
     * canonicalization (i.e., conversion from negative to positive index,
     * if applicable).
     *
     * @param bottom input Blob vector (length 2)
     *   -# @f$ (d_0 \times ... \times
     *           d_i \times ... \times d_j \times ... \times d_n) @f$
     *      the first factor @f$ x @f$
     *   -# @f$ (d_i \times ... \times d_j) @f$
     *      the second factor @f$ y @f$
     * @param top output Blob vector (length 1)
     *   -# @f$ (d_0 \times ... \times
     *           d_i \times ... \times d_j \times ... \times d_n) @f$
     *      the product @f$ z = x y @f$ computed after "broadcasting" y.
     *      Equivalent to tiling @f$ y @f$ to have the same shape as @f$ x @f$,
     *      then computing the elementwise product.
     */
    virtual void Forward(CPUContext* context, const vector<Blob*> & bottom,
                             const vector<Blob*> & top);
    virtual void Forward(GPUContext* context, const vector<Blob*> & bottom,
                             const vector<Blob*> & top);
    virtual void Backward(CPUContext* context, const vector<Blob*> & top,
                              const vector<Blob*> & bottom);
    virtual void Backward(GPUContext* context, const vector<Blob*> & top,
                              const vector<Blob*> & bottom);

    SHARED_PTR<Layer > bias_layer_;
    vector<Blob*> bias_bottom_vec_;
    vector<bool> bias_propagate_down_;
    int bias_param_id_;

    Blob sum_multiplier_;
    Blob sum_result_;
    Blob temp_;
    int axis_;
    int outer_dim_, scale_dim_, inner_dim_;
  };


}  // namespace

#endif  // CAFFE_SCALE_LAYER_HPP_
