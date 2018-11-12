#ifndef CAFFE_DROPOUT_LAYER_HPP_
#define CAFFE_DROPOUT_LAYER_HPP_







#include "caffe/layers/neuron_layer.hpp"

namespace
{

  /**
   * @brief During training only, sets a random portion of @f$x@f$ to 0, adjusting
   *        the rest of the vector magnitude accordingly.
   *
   * @param bottom input Blob vector (length 1)
   *   -# @f$ (N \times C \times H \times W) @f$
   *      the inputs @f$ x @f$
   * @param top output Blob vector (length 1)
   *   -# @f$ (N \times C \times H \times W) @f$
   *      the computed outputs @f$ y = |x| @f$
   */
  template <typename Dtype>
  class DropoutLayer : public NeuronLayer
  {
  public:
    /**
     * @param param provides DropoutParameter dropout_param,
     *     with DropoutLayer options:
     *   - dropout_ratio (\b optional, default 0.5).
     *     Sets the probability @f$ p @f$ that any given unit is dropped.
     */
    explicit DropoutLayer()
      : NeuronLayer() {}
    virtual void LayerSetUp(const vector<Blob*> & bottom,
                            const vector<Blob*> & top);
    virtual void Reshape(const vector<Blob*> & bottom,
                         const vector<Blob*> & top);

    virtual inline const char* type() const { return "Dropout"; }

  public:
    /**
     * @param bottom input Blob vector (length 1)
     *   -# @f$ (N \times C \times H \times W) @f$
     *      the inputs @f$ x @f$
     * @param top output Blob vector (length 1)
     *   -# @f$ (N \times C \times H \times W) @f$
     *      the computed outputs. At training time, we have @f$
     *      y_{\mbox{train}} = \left\{
     *         \begin{array}{ll}
     *            \frac{x}{1 - p} & \mbox{if } u > p \\
     *            0 & \mbox{otherwise}
     *         \end{array} \right.
     *      @f$, where @f$ u \sim U(0, 1)@f$ is generated independently for each
     *      input at each iteration. At test time, we simply have
     *      @f$ y_{\mbox{test}} = \mathbb{E}[y_{\mbox{train}}] = x @f$.
     */
    virtual void Forward(CPUContext* context, const vector<Blob*> & bottom,
                             const vector<Blob*> & top);
    virtual void Forward(GPUContext* context, const vector<Blob*> & bottom,
                             const vector<Blob*> & top);
    virtual void Backward(CPUContext* context, const vector<Blob*> & top,
                              const vector<Blob*> & bottom);
    virtual void Backward(GPUContext* context, const vector<Blob*> & top,
                              const vector<Blob*> & bottom);

    /// when divided by UINT_MAX, the randomly generated values @f$u\sim U(0,1)@f$
    Blob<unsigned int> rand_vec_;
    /// the probability @f$ p @f$ of dropping any input
    Dtype threshold_;
    /// the scale for undropped inputs at train time @f$ 1 / (1 - p) @f$
    Dtype scale_;
    unsigned int uint_thres_;
  };

}  // namespace

#endif  // CAFFE_DROPOUT_LAYER_HPP_
