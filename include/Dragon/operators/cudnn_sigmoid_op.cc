#ifdef WITH_CUDNN

#include "operators/activation/sigmoid_op.h"

namespace dragon {

template <class Context> template <typename T>
void CuDNNSigmoidOp<Context>::RunWithType() {
    cudnnSetTensorDesc<T>(&input_desc, &input(0));
    cudnnSetTensorDesc<T>(&output_desc, output(0));
    auto* Xdata = input(0).template data<T, Context>();
    auto* Ydata = output(0)->template mutable_data<T, Context>();

#if CUDNN_VERSION_MIN(5, 0, 0)
    CUDNN_CHECK(cudnnActivationForward(cudnn_handle(), act_desc,
                           CUDNNType<T>::one, input_desc, Xdata,
                       CUDNNType<T>::zero, output_desc, Ydata));
#else
    CUDNN_CHECK(cudnnActivationForward_v4(cudnn_handle(), act_desc,
                          CUDNNType<Dtype>::one, input_desc, Xdata,
                      CUDNNType<Dtype>::zero, output_desc, Ydata));
#endif
}

template <class Context>
void CuDNNSigmoidOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (input(0).template IsType<float>()) return RunWithType<float>();
#ifdef WITH_CUDA_FP16
    else if (input(0).template IsType<float16>()) return RunWithType<float16>();
#endif
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CUDNN(Sigmoid);

template <class Context> template <typename T>
void CuDNNSigmoidGradientOp<Context>::RunWithType() {
    cudnnSetTensorDesc<T>(&input_desc, &input(-1));
    cudnnSetTensorDesc<T>(&output_desc, output(0));
    auto* dYdata = input(-1).template data<T, Context>();
    auto* Ydata = input(0).template data<T, Context>();
    auto* dXdata = output(0)->template mutable_data<T, Context>();

#if CUDNN_VERSION_MIN(5, 0, 0)
    CUDNN_CHECK(cudnnActivationBackward(cudnn_handle(), act_desc,
                            CUDNNType<T>::one, input_desc, Ydata,
                                              input_desc, dYdata,
                                              output_desc, Ydata,
                       CUDNNType<T>::zero, output_desc, dXdata));
#else
    CUDNN_CHECK(cudnnActivationBackward_v4(cudnn_handle(), act_desc,
                               CUDNNType<T>::one, input_desc, Ydata,
                                                 input_desc, dYdata,
                                                 output_desc, Ydata,
                          CUDNNType<T>::zero, output_desc, dXdata));
#endif
}

template <class Context>
void CuDNNSigmoidGradientOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (input(0).template IsType<float>()) return RunWithType<float>();
#ifdef WITH_CUDA_FP16
    else if (input(0).template IsType<float16>()) return RunWithType<float16>();
#endif
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CUDNN(SigmoidGradient);

}    // namespace dragon

#endif    // WITH_CUDNN