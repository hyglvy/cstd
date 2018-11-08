#ifndef CAFFE_POWER_LAYER_HPP_
#define CAFFE_POWER_LAYER_HPP_







#include "caffe/layers/neuron_layer.hpp"

namespace
{

  /**
   * @brief Computes @f$ y = (\alpha x + \beta) ^ \gamma @f$,
   *        as specified by the scale @f$ \alpha @f$, shift @f$ \beta @f$,
   *        and power @f$ \gamma @f$.
   */
  template <typename Dtype>
  class PowerLayer : public NeuronLayer<Dtype>
  {
  public:
    /**
     * @param param provides PowerParameter power_param,
     *     with PowerLayer options:
     *   - scale (\b optional, default 1) the scale @f$ \alpha @f$
     *   - shift (\b optional, default 0) the shift @f$ \beta @f$
     *   - power (\b optional, default 1) the power @f$ \gamma @f$
     */
    explicit PowerLayer()
      : NeuronLayer<Dtype>() {}
    virtual void LayerSetUp(const vector<Blob<Dtype>*> & bottom,
                            const vector<Blob<Dtype>*> & top);

    virtual inline const char* type() const { return "Power"; }

  public:
    /**
     * @param bottom input Blob vector (length 1)
     *   -# @f$ (N \times C \times H \times W) @f$
     *      the inputs @f$ x @f$
     * @param top output Blob vector (length 1)
     *   -# @f$ (N \times C \times H \times W) @f$
     *      the computed outputs @f$
     *        y = (\alpha x + \beta) ^ \gamma
     *      @f$
     */
    virtual void Forward(CPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);
    virtual void Forward(GPUContext* context, const vector<Blob<Dtype>*> & bottom,
                             const vector<Blob<Dtype>*> & top);

    /**
     * @brief Computes the error gradient w.r.t. the power inputs.
     *
     * @param top output Blob vector (length 1), providing the error gradient with
     *      respect to the outputs
     *   -# @f$ (N \times C \times H \times W) @f$
     *      containing error gradients @f$ \frac{\partial E}{\partial y} @f$
     *      with respect to computed outputs @f$ y @f$
     * @param propagate_down see Layer::Backward.
     * @param bottom input Blob vector (length 1)
     *   -# @f$ (N \times C \times H \times W) @f$
     *      the inputs @f$ x @f$; Backward fills their diff with
     *      gradients @f$
     *        \frac{\partial E}{\partial x} =
     *            \frac{\partial E}{\partial y}
     *            \alpha \gamma (\alpha x + \beta) ^ {\gamma - 1} =
     *            \frac{\partial E}{\partial y}
     *            \frac{\alpha \gamma y}{\alpha x + \beta}
     *      @f$ if bottom[0]->propagate_down_
     */
    virtual void Backward(CPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);
    virtual void Backward(GPUContext* context, const vector<Blob<Dtype>*> & top,
                              const vector<Blob<Dtype>*> & bottom);

    /// @brief @f$ \gamma @f$ from param_->power_param()
    Dtype power_;
    /// @brief @f$ \alpha @f$ from param_->power_param()
    Dtype scale_;
    /// @brief @f$ \beta @f$ from param_->power_param()
    Dtype shift_;
    /// @brief Result of @f$ \alpha \gamma @f$
    Dtype diff_scale_;
  };

}  // namespace

#endif  // CAFFE_POWER_LAYER_HPP_