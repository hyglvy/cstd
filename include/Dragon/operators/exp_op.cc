#include "operators/arithmetic/exp_op.h"
#include "core/workspace.h"
#include "utils/math_functions.h"

namespace dragon {

template <class Context> template <typename T>
void ExpOp<Context>::RunWithType() {
    auto* Xdata = input(0).template data<T, Context>();
    auto* Ydata = output(0)->template mutable_data<T, Context>();
    math::Exp<T, Context>(output(0)->count(), Xdata, Ydata);
}

template <class Context>
void ExpOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (input(0).template IsType<float>()) RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(Exp);
#ifdef WITH_CUDA
DEPLOY_CUDA(Exp);
#endif
OPERATOR_SCHEMA(Exp).NumInputs(1).NumOutputs(1);

template <class Context> template <typename T>
void ExpGradientOp<Context>::RunWithType() {
    auto* Ydata = input(0).template data<T, Context >();
    auto* dYdata = input(-1).template data<T, Context>();
    auto* dXdata = output(0)->template mutable_data<T, Context>();
    math::Mul<T, Context>(output(0)->count(), dYdata, Ydata, dXdata);
}

template <class Context>
void ExpGradientOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (input(0).template IsType<float>()) RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(ExpGradient);
#ifdef WITH_CUDA
DEPLOY_CUDA(ExpGradient);
#endif
OPERATOR_SCHEMA(ExpGradient).NumInputs(2).NumOutputs(1);

class GetExpGradient final : public GradientMakerBase {
 public:
    GRADIENT_MAKER_CTOR(GetExpGradient);
    vector<OperatorDef> MakeDefs() override {
        return SingleDef(def.type() + "Gradient", "",
            vector<string> {O(0), GO(0)},
            vector<string> {GI(0)});
    }
};
REGISTER_GRADIENT(Exp, GetExpGradient);

}    // namespace dragon