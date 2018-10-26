#include "operators/arithmetic/square_op.h"
#include "core/workspace.h"
#include "utils/math_functions.h"

namespace dragon {

template <class Context> template <typename T>
void SquareOp<Context>::RunWithType() {
    auto* Xdata = input(0).template data<T, Context>();
    auto* Ydata = output(0)->template mutable_data<T, Context>();
    math::Pow<T, Context>(output(0)->count(), 2.0, Xdata, Ydata);
}

template <class Context>
void SquareOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (input(0).template IsType<float>()) RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(Square);
#ifdef WITH_CUDA
DEPLOY_CUDA(Square);
#endif
OPERATOR_SCHEMA(Square).NumInputs(1).NumOutputs(1);

template <class Context> template <typename T>
void SquareGradientOp<Context>::RunWithType() {
    auto* Xdata = input(0).template data<T, Context>();
    auto* dYdata = input(-1).template data<T, Context>();
    auto* dXdata = output(0)->template mutable_data<T, Context>();
    math::Mul<T, Context>(output(0)->count(), dYdata, Xdata, dXdata);
    math::Scal<T, Context>(output(0)->count(), 2.0, dXdata);
}

template <class Context>
void SquareGradientOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (input(0).template IsType<float>()) RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(SquareGradient);
#ifdef WITH_CUDA
DEPLOY_CUDA(SquareGradient);
#endif
OPERATOR_SCHEMA(SquareGradient).NumInputs(2).NumOutputs(1);

class GetSquareGradient final : public GradientMakerBase {
 public:
    GRADIENT_MAKER_CTOR(GetSquareGradient);
    vector<OperatorDef> MakeDefs() override{
        return SingleDef(def.type() + "Gradient", "",
            vector<string> {I(0), GO(0)},
            vector<string> {GI(0)});
    }
};
REGISTER_GRADIENT(Square, GetSquareGradient);

}    // namespace dragon