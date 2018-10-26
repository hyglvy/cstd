// --------------------------------------------------------
// Dragon
// Copyright(c) 2017 SeetaTech
// Written by Ting Pan
// --------------------------------------------------------

#ifndef DRAGON_OPERATORS_ACTIVATION_DROPOUT_OP_H_
#define DRAGON_OPERATORS_ACTIVATION_DROPOUT_OP_H_

#include "core/operator.h"
#include "utils/math_functions.h"

namespace dragon {

template <class Context>
class DropoutOp final : public Operator<Context> {
 public:
    DropoutOp(const OperatorDef& op_def, Workspace* ws)
        : Operator<Context>(op_def, ws),
          use_scale(OperatorBase::GetSingleArg<bool>("scale", true)) {
        GET_ARGUMENT_WITH_DESC(float, prob, 0.5);
    }

    void RunOnDevice() override;
    template <typename T> void RunWithType();

 protected:
    DECLARE_ARGUMENT_WITH_DESC(float, prob);
    bool use_scale;
    Tensor* mask;
};

template <class Context>
class DropoutGradientOp final : public Operator<Context> {
 public:
    DropoutGradientOp(const OperatorDef& op_def, Workspace* ws)
        : Operator<Context>(op_def, ws),
          use_scale(OperatorBase::GetSingleArg<bool>("scale", true)) {
        GET_ARGUMENT_WITH_DESC(float, prob, 0.5);
        DISABLE_SHARE_GRADIENT;
    }

    void RunOnDevice() override;
    template <typename T> void RunWithType();

 protected:
     DECLARE_ARGUMENT_WITH_DESC(float, prob);
     bool use_scale;
     Tensor* mask;
};

DEFINE_ARGUMENT_WITH_DESC(float, DropoutOp, prob);
DEFINE_ARGUMENT_WITH_DESC(float, DropoutGradientOp, prob);

}    // namespace dragon

#endif    // DRAGON_OPERATORS_ACTIVATION_DROPOUT_OP_H_