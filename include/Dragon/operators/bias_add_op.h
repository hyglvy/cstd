// --------------------------------------------------------
// Dragon
// Copyright(c) 2017 SeetaTech
// Written by Ting Pan
// --------------------------------------------------------

#ifndef DRAGON_OPERATORS_ARITHMETIC_BIAS_ADD_OP_H_
#define DRAGON_OPERATORS_ARITHMETIC_BIAS_ADD_OP_H_

#include "core/operator.h"

namespace dragon {

template <class Context>
class BiasAddOp : public Operator<Context> {
 public:
    BiasAddOp(const OperatorDef& op_def, Workspace* ws)
        : Operator<Context>(op_def, ws),
          data_format(OperatorBase::GetSingleArg<string>("data_format", "NCHW")) {}

    void RunOnDevice() override;
    template <typename T> void RunWithType();

 protected:
    TIndex outer_dim, dim, inner_dim;
    string data_format;
    Tensor* bias_multiplier;
};

template <class Context>
class BiasAddGradientOp final : public Operator<Context> {
 public:
    BiasAddGradientOp(const OperatorDef& op_def, Workspace* ws)
        : Operator<Context>(op_def, ws),
          data_format(OperatorBase::GetSingleArg<string>("data_format", "NCHW")) {
        DISABLE_SHARE_GRADIENT;
    }

    void RunOnDevice() override;
    template <typename T> void RunWithType();

 protected:
    int outer_dim, dim, inner_dim;
    string data_format;
    Tensor* bias_multiplier;
};

}    // namespace dragon

#endif    // DRAGON_OPERATORS_ARITHMETIC_BIAS_ADD_OP_H_