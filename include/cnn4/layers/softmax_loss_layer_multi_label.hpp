#ifndef CAFFE_SOFTMAX_WITH_LOSS_LAYER_MULTI_LABEL_HPP_
#define CAFFE_SOFTMAX_WITH_LOSS_LAYER_MULTI_LABEL_HPP_







#include "caffe/layers/loss_layer.hpp"
#include "caffe/layers/softmax_layer.hpp"

namespace
{

  /**
   * @brief Computes the multinomial logistic loss for a one-of-many
   *        classification task, passing real-valued predictions through a
   *        softmax to get a probability distribution over classes.
   *
   * This layer should be preferred over separate
   * SoftmaxLayer + MultinomialLogisticLossLayer
   * as its gradient computation is more numerically stable.
   * At test time, this layer can be replaced simply by a SoftmaxLayer.
   *
   * @param bottom input Blob vector (length 2)
   *   -# @f$ (N \times C \times H \times W) @f$
   *      the predictions @f$ x @f$, a Blob with values in
   *      @f$ [-\infty, +\infty] @f$ indicating the predicted score for each of
   *      the @f$ K = CHW @f$ classes. This layer maps these scores to a
   *      probability distribution over classes using the softmax function
   *      @f$ \hat{p}_{nk} = \exp(x_{nk}) /
   *      \left[\sum_{k'} \exp(x_{nk'})\right] @f$ (see SoftmaxLayer).
   *   -# @f$ (N \times 1 \times 1 \times 1) @f$
   *      the labels @f$ l @f$, an integer-valued Blob with values
   *      @f$ l_n \in [0, 1, 2, ..., K - 1] @f$
   *      indicating the correct class label among the @f$ K @f$ classes
   * @param top output Blob vector (length 1)
   *   -# @f$ (1 \times 1 \times 1 \times 1) @f$
   *      the computed cross-entropy classification loss: @f$ E =
   *        \frac{-1}{N} \sum\limits_{n=1}^N \log(\hat{p}_{n,l_n})
   *      @f$, for softmax output class probabilites @f$ \hat{p} @f$
   */
  template <typename Dtype>
  class SoftmaxWithLossMultiLabelLayer : public LossLayer
  {
  public:
    /**
     * @param param provides LossParameter loss_param, with options:
     *  - ignore_label (optional)
     *    Specify a label value that should be ignored when computing the loss.
     *  - normalize (optional, default true)
     *    If true, the loss is normalized by the number of (nonignored) labels
     *    present; otherwise the loss is simply summed over spatial locations.
     */
    explicit SoftmaxWithLossMultiLabelLayer()
      : LossLayer() {}
    virtual void LayerSetUp(const vector<Blob*> & bottom,
                            const vector<Blob*> & top);
    virtual void Reshape(const vector<Blob*> & bottom,
                         const vector<Blob*> & top);

    virtual inline const char* type() const { return "SoftmaxWithLossMultiLabel"; }
    virtual inline int ExactNumTopBlobs() const { return -1; }
    virtual inline int MinTopBlobs() const { return 1; }
    virtual inline int MaxTopBlobs() const { return 2; }

  public:
    virtual void Forward(CPUContext* context, const vector<Blob*> & bottom,
                             const vector<Blob*> & top);
    virtual void Forward(GPUContext* context, const vector<Blob*> & bottom,
                             const vector<Blob*> & top);
    /**
     * @brief Computes the softmax loss error gradient w.r.t. the predictions.
     *
     * Gradients cannot be computed with respect to the label inputs (bottom[1]),
     * so this method ignores bottom[1] and requires !bottom[1]->propagate_down_, crashing
     * if bottom[1]->propagate_down_ is set.
     *
     * @param top output Blob vector (length 1), providing the error gradient with
     *      respect to the outputs
     *   -# @f$ (1 \times 1 \times 1 \times 1) @f$
     *      This Blob's diff will simply contain the loss_weight* @f$ \lambda @f$,
     *      as @f$ \lambda @f$ is the coefficient of this layer's output
     *      @f$\ell_i@f$ in the overall Net loss
     *      @f$ E = \lambda_i \ell_i + \mbox{other loss terms}@f$; hence
     *      @f$ \frac{\partial E}{\partial \ell_i} = \lambda_i @f$.
     *      (*Assuming that this top Blob is not used as a bottom (input) by any
     *      other layer of the Net.)
     * @param propagate_down see Layer::Backward.
     *      bottom[1]->propagate_down_ must be false as we can't compute gradients with
     *      respect to the labels.
     * @param bottom input Blob vector (length 2)
     *   -# @f$ (N \times C \times H \times W) @f$
     *      the predictions @f$ x @f$; Backward computes diff
     *      @f$ \frac{\partial E}{\partial x} @f$
     *   -# @f$ (N \times 1 \times 1 \times 1) @f$
     *      the labels -- ignored as we can't compute their error gradients
     */
    virtual void Backward(CPUContext* context, const vector<Blob*> & top,
                              const vector<Blob*> & bottom);
    virtual void Backward(GPUContext* context, const vector<Blob*> & top,
                              const vector<Blob*> & bottom);

    /// Read the normalization mode parameter and compute the normalizer based
    /// on the blob size.  If normalization_mode is VALID, the count of valid
    /// outputs will be read from valid_count, unless it is -1 in which case
    /// all outputs are assumed to be valid.
    virtual Dtype get_normalizer(
      LossParameter_NormalizationMode normalization_mode, int valid_count);

    /// The internal SoftmaxLayer used to map predictions to a distribution.
    SHARED_PTR<Layer > softmax_layer_;
    /// prob stores the output probability predictions from the SoftmaxLayer.
    Blob prob_;
    /// bottom vector holder used in call to the underlying SoftmaxLayer::Forward
    vector<Blob*> softmax_bottom_vec_;
    /// top vector holder used in call to the underlying SoftmaxLayer::Forward
    vector<Blob*> softmax_top_vec_;
    /// Whether to ignore instances with a certain label.
    bool has_ignore_label_;
    /// The label indicating that an instance should be ignored.
    int ignore_label_;
    /// How to normalize the output loss.
    LossParameter_NormalizationMode normalization_;

    int softmax_axis_, outer_num_, inner_num_, label_num_;
  };

}  // namespace

#endif  // CAFFE_SOFTMAX_WITH_LOSS_LAYER_HPP_
