#ifndef CAFFE_CUDNN_LCN_LAYER_HPP_
#define CAFFE_CUDNN_LCN_LAYER_HPP_







#include "caffe/layers/lrn_layer.hpp"
#include "caffe/layers/power_layer.hpp"

namespace
{

#ifdef USE_CUDNN
  template <typename Dtype>
  class CuDNNLCNLayer : public LRNLayer<Dtype>
  {
  public:
    explicit CuDNNLCNLayer()
      : LRNLayer<Dtype>(param), handles_setup_(false), tempDataSize(0),
        tempData1(NULL), tempData2(NULL) {}
    virtual void LayerSetUp(const vector<Blob<Dtype>*> & bottom,
                            const vector<Blob<Dtype>*> & top);
    virtual void Reshape(const vector<Blob<Dtype>*> & bottom,
                         const vector<Blob<Dtype>*> & top);
    virtual ~CuDNNLCNLayer();

  public:
    virtual void Forward(GPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);
    virtual void Backward(GPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);

    bool handles_setup_;
    cudnnHandle_t             handle_;
    cudnnLRNDescriptor_t norm_desc_;
    cudnnTensorDescriptor_t bottom_desc_, top_desc_;

    int size_, pre_pad_;
    Dtype alpha_, beta_, k_;

    size_t tempDataSize;
    void* tempData1, *tempData2;
  };
#endif

}  // namespace

#endif  // CAFFE_CUDNN_LCN_LAYER_HPP_