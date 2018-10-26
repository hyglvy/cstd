#include "operators/ndarray/stack_op.h"
#include "core/workspace.h"
#include "utils/op_kernel.h"

namespace dragon {

template <class Context> template <typename T>
void StackOp<Context>::RunWithType() {
    auto* Ydata = output(0)->template mutable_data<T, Context>();
    for (int i = 0; i < nin; i++) {
        auto* Xdata = input(i).template data<T, Context>();
        TIndex count = input(i).count();
        x_concat_dim = 1;
        kernel::Concat<T, Context>(count, 
                               outer_dim, 
                               inner_dim,
                            x_concat_dim,
                            y_concat_dim, 
                           concat_offset, 
                                   Xdata, 
                                   Ydata, 
                                 &ctx());
        concat_offset += x_concat_dim;
    }
}

template <class Context>
void StackOp<Context>::RunOnDevice() {
    while (axis < 0) axis += (input(0).ndim() + 1);
    stack_dims = concat_dims =  input(0).dims();
    concat_dims.insert(concat_dims.begin() + axis, nin);
    for (int i = 1; i < nin; i++) {
        CHECK_EQ(stack_dims.size(), input(i).ndim())
            << "\nAll inputs should have the same ndim.";
        for (int j = 0; j < stack_dims.size(); j++) 
            CHECK_EQ(stack_dims[j], input(i).dim(j))
                << "\nAll inputs should have the same dimensions.";
    }
    y_concat_dim = concat_dims[axis];
    outer_dim = input(0).count(0, axis);
    inner_dim = input(0).count(axis);
    concat_offset = 0;
    output(0)->Reshape(concat_dims);
    if (nin == 1) {
        output(0)->Share(input(0));
        return;
    }

    if (input(0).template IsType<float>()) RunWithType<float>();
#ifdef WITH_CUDA_FP16
    else if (input(0).template IsType<float16>()) RunWithType<float16>();
#endif
    else LOG(FATAL) << "Unsupported input types.";
}

DEPLOY_CPU(Stack);
#ifdef WITH_CUDA
DEPLOY_CUDA(Stack);
#endif
OPERATOR_SCHEMA(Stack).NumInputs(1, INT_MAX).NumOutputs(1);

template <class Context> template <typename T>
void StackGradientOp<Context>::RunWithType() {
    auto* dYdata = input(-1).template data<T, Context>();
    for (int i = 0; i < nin; i++) {
        x_concat_dim = 1;
        if (output(i)->name() != "ignore") {
            auto* dXdata = output(i)->template mutable_data<T, Context>();
            TIndex count = output(i)->count();
            kernel::ConcatGrad<T, Context>(count, 
                                       outer_dim, 
                                       inner_dim,
                                    x_concat_dim, 
                                    y_concat_dim, 
                                   concat_offset,
                                          dYdata, 
                                          dXdata, 
                                         &ctx());
        }
        concat_offset += x_concat_dim;
    }
}

template <class Context>
void StackGradientOp<Context>::RunOnDevice() {
    if (input(-1).name() == "ignore") return;
    while (axis < 0) axis += input(-1).ndim();
    concat_dims = input(-1).dims();
    y_concat_dim = concat_dims[axis];
    outer_dim = input(0).count(0, axis);
    inner_dim = input(0).count(axis);
    concat_offset = 0;
    for (int i = 0; i < nin; i++) output(i)->ReshapeLike(input(i));
    if (nin == 1) {
        output(0)->Share(input(-1));
        return;
    }

    if (input(0).template IsType<float>()) RunWithType<float>();
#ifdef WITH_CUDA_FP16
    else if (input(0).template IsType<float16>()) RunWithType<float16>();
#endif
    else LOG(FATAL) << "Unsupported input types.";
}

template <class Context>
void StackGradientOp<Context>::ShareGradient() {
    for (int i = 0; i < OutputSize(); i++) {
        if (output(i)->name() != "ignore") {
            Tensor* dX = ws()->GetBuffer("Grad");
            ws()->CreateAvatar(output(i), dX);
            break;
        }
    }
}

DEPLOY_CPU(StackGradient);
#ifdef WITH_CUDA
DEPLOY_CUDA(StackGradient);
#endif
OPERATOR_SCHEMA(StackGradient).NumInputs(2, INT_MAX).NumOutputs(1, INT_MAX);

class GetStackGradient final : public GradientMakerBase {
 public:
    GRADIENT_MAKER_CTOR(GetStackGradient);
    vector<OperatorDef> MakeDefs() override {
        vector<string> inputs, outputs;
        for (int i = 0; i < def.input_size(); i++) {
            inputs.push_back(def.input(i));
            outputs.push_back(GI(i));
        }
        inputs.push_back(GO(0));
        return SingleDef(def.type() + "Gradient", "", inputs, outputs);
    }
};
REGISTER_GRADIENT(Stack, GetStackGradient);

}    // namespace dragon