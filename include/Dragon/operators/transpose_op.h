// --------------------------------------------------------
// Dragon
// Copyright(c) 2017 SeetaTech
// Written by Ting Pan
// --------------------------------------------------------

#ifndef DRAGON_OPERATORS_NDARRAY_TRANSPOSE_OP_H_
#define DRAGON_OPERATORS_NDARRAY_TRANSPOSE_OP_H_

#include "core/operator.h"

namespace dragon {

template <class Context>
class TransposeOp final: public Operator<Context> {
 public:
    TransposeOp(const OperatorDef& op_def, Workspace* ws)
        : Operator<Context>(op_def, ws),
          perms(OperatorBase::GetRepeatedArg<int>("perms")) {
        if (perms.size() > 0) reverse_dims = false;
        else reverse_dims = true;
    }

    void RunOnDevice() override;
    template <typename T> void RunWithType();

 protected:
    vector<int> perms;
    bool reverse_dims;
    Tensor* order, *old_steps, *new_steps;
};


template <class Context>
class TransposeGradientOp final : public Operator<Context> {
 public:
    TransposeGradientOp(const OperatorDef& op_def, Workspace* ws) 
        : Operator<Context>(op_def, ws) {}

    void RunOnDevice() override;
    template <typename T> void RunWithType();

 protected:
    Tensor* order, *old_steps, *new_steps;
};

}    // namespace dragon

#endif    // DRAGON_OPERATORS_NDARRAY_TRANSPOSE_OP_H_