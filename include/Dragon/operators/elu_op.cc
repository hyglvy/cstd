#include "operators/activation/elu_op.h"
#include "utils/math_functions.h"
#include "utils/op_kernel.h"

namespace dragon {

template <class Context> template <typename T>
void EluOp<Context>::RunWithType() {
    auto* Xdata = input(0).template data<T, Context>();
    auto* Ydata = output(0)->template mutable_data<T, Context>();
    kernel::Elu<T, Context>(output(0)->count(), Xdata, alpha, Ydata);
}

template <class Context>
void EluOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (input(0).template IsType<float>()) RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(Elu);
#ifdef WITH_CUDA
DEPLOY_CUDA(Elu);
#endif
OPERATOR_SCHEMA(Elu).NumInputs(1).NumOutputs(1).Inplace({ { 0, 0 } });

template <class Context> template <typename T>
void EluGradientOp<Context>::RunWithType() {
    auto* Ydata = input(0).template data<T, Context>();
    auto* dYdata = input(1).template data<T, Context>();
    auto* dXdata = output(0)->template mutable_data<T, Context>();
    kernel::EluGrad<T, Context>(output(0)->count(), dYdata, Ydata, alpha, dXdata);
}

template <class Context>
void EluGradientOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (input(0).template IsType<float>()) RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(EluGradient);
#ifdef WITH_CUDA
DEPLOY_CUDA(EluGradient);
#endif
OPERATOR_SCHEMA(EluGradient).NumInputs(2).NumOutputs(1).Inplace({ { 1, 0 }});

class GetEluGradient final : public GradientMakerBase {
 public:
    GRADIENT_MAKER_CTOR(GetEluGradient);
    vector<OperatorDef> MakeDefs() override {
        return SingleDef(def.type() + "Gradient", "",
            vector<string> {O(0), GO(0)},
            vector<string> {GI(0)});
    }
};
REGISTER_GRADIENT(Elu, GetEluGradient);

}    // namespace dragon