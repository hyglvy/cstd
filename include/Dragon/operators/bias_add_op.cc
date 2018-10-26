#include "operators/arithmetic/bias_add_op.h"
#include "core/workspace.h"
#include "utils/filler.h"
#include "utils/op_kernel.h"

namespace dragon {

template <class Context> template <typename T>
void BiasAddOp<Context>::RunWithType() {
    TENSOR_FILL(input(1), vector<TIndex>(1, dim));
    INIT_MULTIPLIER(bias_multiplier, inner_dim);
    auto* Bdata = input(1).template data<T, Context>();
    auto* BMul_data = bias_multiplier->template data<T, Context>();
    auto* Ydata = output(0)->template mutable_data<T, Context>();
    kernel::BiasAdd<T, Context>(output(0)->count(), outer_dim, dim, inner_dim,
                                                                  data_format,
                                                                        Bdata,
                                                                    BMul_data,
                                                                        Ydata);
}

template <class Context>
void BiasAddOp<Context>::RunOnDevice() {
    if (data_format == "NCHW") {
        outer_dim = input(0).dim(0);
        dim = input(0).dim(1);
        inner_dim = input(0).count(2);
    } else if (data_format == "NHWC") {
        outer_dim = input(0).dim(0);
        dim = input(0).dim(-1);
        inner_dim = input(0).count(1) / dim;
    } else LOG(FATAL) << "Unknown data format: " << data_format;
    ws()->CreateAvatar(output(0), &input(0));

    if (input(0).template IsType<float>()) RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(BiasAdd);
#ifdef WITH_CUDA
DEPLOY_CUDA(BiasAdd);
#endif
OPERATOR_SCHEMA(BiasAdd).NumInputs(2).NumOutputs(1);

template <class Context> template <typename T>
void BiasAddGradientOp<Context>::RunWithType() {
    if (output(1)->name() != "ignore") {
        INIT_MULTIPLIER(bias_multiplier, inner_dim);
        auto* BMul_data = this->bias_multiplier->template data<T, Context>();
        auto* dYdata = input(-1).template data<T, Context>();
        auto* dBias = output(1)->template mutable_data<T, Context>();
        const int y_offset = dim * inner_dim;
        for (int n = 0; n < outer_dim; n++) {
            if (data_format == "NCHW") {
                math::Gemv<T, Context>(CblasNoTrans, dim, inner_dim,
                                                                1.0,
                                                  dYdata, BMul_data,
                                                                1.0,
                                                             dBias);
            } else if (data_format == "NHWC") {
                math::Gemv<T, Context>(CblasTrans, inner_dim, dim,
                                                              1.0,
                                                dYdata, BMul_data,
                                                              1.0,
                                                           dBias);
            }
            dYdata += y_offset;
        }
    }

    if (output(0)->name() != "ignore") {
        ws()->CreateAvatar(output(0), &input(-1));
    }
}

template <class Context>
void BiasAddGradientOp<Context>::RunOnDevice() {
    if (data_format == "NCHW") {
        outer_dim = input(0).dim(0);
        dim = input(0).dim(1);
        inner_dim = input(0).count(2);
    } else if (data_format == "NHWC") {
        outer_dim = input(0).dim(0);
        dim = input(0).dim(-1);
        inner_dim = input(0).count(1) / dim;
    } else LOG(FATAL) << "Unknown data format: " << data_format;
    output(1)->ReshapeLike(input(1));

    if (input(0).template IsType<float>()) RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(BiasAddGradient);
#ifdef WITH_CUDA
DEPLOY_CUDA(BiasAddGradient);
#endif
OPERATOR_SCHEMA(BiasAddGradient).NumInputs(3).NumOutputs(2);

class GetBiasAddGradient final : public GradientMakerBase {
 public:
    GRADIENT_MAKER_CTOR(GetBiasAddGradient);
    vector<OperatorDef> MakeDefs() override {
        return SingleDef(def.type() + "Gradient", "",
            vector<string> {I(0), I(1), GO(0)},
            vector<string> {GI(0), GI(1)});
    }
};
REGISTER_GRADIENT(BiasAdd, GetBiasAddGradient);

}    // namespace dragon