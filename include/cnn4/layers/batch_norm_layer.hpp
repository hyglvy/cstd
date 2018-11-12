#ifndef CAFFE_BATCHNORM_LAYER_HPP_
#define CAFFE_BATCHNORM_LAYER_HPP_







namespace
{

  /**
   * @brief Normalizes the input to have 0-mean and/or unit (1) variance across
   *        the batch.
   *
   * This layer computes Batch Normalization described in [1].  For
   * each channel in the data (i.e. axis 1), it subtracts the mean and divides
   * by the variance, where both statistics are computed across both spatial
   * dimensions and across the different examples in the batch.
   *
   * By default, during training time, the network is computing global mean/
   * variance statistics via a running average, which is then used at test
   * time to allow deterministic outputs for each input.  You can manually
   * toggle whether the network is accumulating or using the statistics via the
   * use_global_stats option.  IMPORTANT: for this feature to work, you MUST
   * set the learning rate to zero for all three parameter blobs, i.e.,
   * param {lr_mult: 0} three times in the layer definition.
   *
   * Note that the original paper also included a per-channel learned bias and
   * scaling factor.  It is possible (though a bit cumbersome) to implement
   * this in caffe using a single-channel DummyDataLayer filled with zeros,
   * followed by a Convolution layer with output the same size as the current.
   * This produces a channel-specific value that can be added or multiplied by
   * the BatchNorm layer's output.
   *
   * [1] S. Ioffe and C. Szegedy, "Batch Normalization: Accelerating Deep Network
   *     Training by Reducing Internal Covariate Shift." arXiv preprint
   *     arXiv:1502.03167 (2015).
   *
   * TODO(dox): thorough documentation for Forward, Backward, and proto params.
   */
  template <typename Dtype>
  class BatchNormLayer : public Layer
  {
  public:
    explicit BatchNormLayer()
      : Layer() {}
    virtual void LayerSetUp(const vector<Blob*> & bottom,
                            const vector<Blob*> & top);
    virtual void Reshape(const vector<Blob*> & bottom,
                         const vector<Blob*> & top);

    virtual inline const char* type() const { return "BatchNorm"; }
    virtual inline int ExactNumBottomBlobs() const { return 1; }
    virtual inline int ExactNumTopBlobs() const { return 1; }

  public:
    virtual void Forward(CPUContext* context, const vector<Blob*> & bottom,
                             const vector<Blob*> & top);
    virtual void Forward(GPUContext* context, const vector<Blob*> & bottom,
                             const vector<Blob*> & top);
    virtual void Backward(CPUContext* context, const vector<Blob*> & top,
                              const vector<Blob*> & bottom);
    virtual void Backward(GPUContext* context, const vector<Blob*> & top,
                              const vector<Blob*> & bottom);

    Blob mean_, variance_, temp_, x_norm_;
    bool use_global_stats_;
    Dtype moving_average_fraction_;
    int channels_;
    Dtype eps_;

    // extra temporarary variables is used to carry out sums/broadcasting
    // using BLAS
    Blob batch_sum_multiplier_;
    Blob num_by_chans_;
    Blob spatial_sum_multiplier_;
  };

}  // namespace

#endif  // CAFFE_BATCHNORM_LAYER_HPP_
