#include "operators/vision/roi_align_op.h"
#include "core/workspace.h"
#include "utils/math_functions.h"
#include "utils/op_kernel.h"

namespace dragon {

template <class Context> template <typename T>
void ROIAlignOp<Context>::RunWithType() {
    kernel::ROIAlign<T, Context>(spatial_scale,
                                pool_h, pool_w,
                                sampling_ratio,
                                     &input(0),
                                     &input(1),
                                    output(0));
}

template <class Context>
void ROIAlignOp<Context>::RunOnDevice() {
    output(0)->Reshape(vector<TIndex>({ input(1).dim(0),
                                        input(0).dim(1),
                                        pool_h, pool_w }));

    if (input(0).template IsType<float>()) return RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(ROIAlign);
#ifdef WITH_CUDA
DEPLOY_CUDA(ROIAlign);
#endif
OPERATOR_SCHEMA(ROIAlign).NumInputs(2).NumOutputs(1);

template <class Context> template <typename T>
void ROIAlignGradientOp<Context>::RunWithType() {
    kernel::ROIAlignGrad<T, Context>(spatial_scale,
                                    pool_h, pool_w,
                                    sampling_ratio,
                                        &input(-1),
                                         &input(1),
                                        output(0));
}

template <class Context>
void ROIAlignGradientOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (input(0).template IsType<float>()) return RunWithType<float>();
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(ROIAlignGradient);
#ifdef WITH_CUDA
DEPLOY_CUDA(ROIAlignGradient);
#endif
OPERATOR_SCHEMA(ROIAlignGradient).NumInputs(3).NumOutputs(1);

class GetROIAlignGradient final : public GradientMakerBase {
 public:
    GRADIENT_MAKER_CTOR(GetROIAlignGradient);
    vector<OperatorDef> MakeDefs() override {
        return SingleDef(def.type() + "Gradient", "",
            vector<string> {I(0), I(1), GO(0)},
            vector<string> {GI(0)});
    }
};
REGISTER_GRADIENT(ROIAlign, GetROIAlignGradient);

}    // namespace dragon