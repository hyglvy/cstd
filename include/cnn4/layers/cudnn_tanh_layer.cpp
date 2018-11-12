#ifdef USE_CUDNN


#include "caffe/layers/cudnn_tanh_layer.hpp"

namespace
{

  template <typename Dtype>
  void CuDNNTanHLayer::LayerSetUp(const vector<Blob*> & bottom,
                                         const vector<Blob*> & top)
  {
    TanHLayer::LayerSetUp(bottom, top);
    // initialize cuDNN
    CUDNN_CHECK(cudnnCreate(&handle_));
    cudnn::createTensor4dDesc<Dtype>(&bottom_desc_);
    cudnn::createTensor4dDesc<Dtype>(&top_desc_);
    cudnn::createActivationDescriptor<Dtype>(&activ_desc_, CUDNN_ACTIVATION_TANH);
    handles_setup_ = true;
  }

  template <typename Dtype>
  void CuDNNTanHLayer::Reshape(const vector<Blob*> & bottom,
                                      const vector<Blob*> & top)
  {
    TanHLayer::Reshape(bottom, top);
    const int N = bottom[0]->num();
    const int K = bottom[0]->channels();
    const int H = bottom[0]->height();
    const int W = bottom[0]->width();
    cudnn::setTensor4dDesc<Dtype>(&bottom_desc_, N, K, H, W);
    cudnn::setTensor4dDesc<Dtype>(&top_desc_, N, K, H, W);
  }

  template <typename Dtype>
  CuDNNTanHLayer::~CuDNNTanHLayer()
  {
    // Check that handles have been setup before destroying.
    if (!handles_setup_) { return; }
    cudnnDestroyTensorDescriptor(this->bottom_desc_);
    cudnnDestroyTensorDescriptor(this->top_desc_);
    cudnnDestroy(this->handle_);
  }

  INSTANTIATE_CLASS(CuDNNTanHLayer);

}  // namespace
#endif
