#ifndef CAFFE_CUDNN_POOLING_LAYER_HPP_
#define CAFFE_CUDNN_POOLING_LAYER_HPP_







#include "caffe/layers/pooling_layer.hpp"

namespace
{

#ifdef USE_CUDNN
  /*
   * @brief cuDNN implementation of PoolingLayer.
   *        Fallback to PoolingLayer for CPU mode.
  */
  template <typename Dtype>
  class CuDNNPoolingLayer : public PoolingLayer<Dtype>
  {
  public:
    explicit CuDNNPoolingLayer()
      : PoolingLayer<Dtype>(param), handles_setup_(false) {}
    virtual void LayerSetUp(const vector<Blob<Dtype>*> & bottom,
                            const vector<Blob<Dtype>*> & top);
    virtual void Reshape(const vector<Blob<Dtype>*> & bottom,
                         const vector<Blob<Dtype>*> & top);
    virtual ~CuDNNPoolingLayer();
    // Currently, cuDNN does not support the extra top blob.
    virtual inline int MinTopBlobs() const { return -1; }
    virtual inline int ExactNumTopBlobs() const { return 1; }

  public:
    virtual void Forward(GPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);
    virtual void Backward(GPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<bool> & propagate_down, const vector<Blob<Dtype>*> & bottom);

    bool handles_setup_;
    cudnnHandle_t             handle_;
    cudnnTensorDescriptor_t bottom_desc_, top_desc_;
    cudnnPoolingDescriptor_t  pooling_desc_;
    cudnnPoolingMode_t        mode_;
  };
#endif

}  // namespace

#endif  // CAFFE_CUDNN_POOLING_LAYER_HPP_
