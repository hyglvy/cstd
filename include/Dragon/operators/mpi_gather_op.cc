#include "operators/mpi/mpi_gather_op.h"
#include "utils/math_functions.h"

#ifdef WITH_MPI

namespace dragon {

template <class Context> template <typename T>
void MPIGatherOp<Context>::RunWithType() {
    if (this->comm_rank == this->comm_root) {
        output(this->comm_rank)->Share(input(0));
        for (int i = 0; i < this->comm_size; i++) {
            if (i == this->comm_root) continue;
#ifdef WITH_MPI_CUDA
            auto* Ydata = output(i)->template mutable_data<T, Context>();
#else
            auto* Ydata = output(i)->template mutable_data<T, CPUContext>();
#endif
            MPI_Recv(Ydata, output(i)->count(), mpi_dtype(), i, 0, this->comm, MPI_STATUS_IGNORE);
        }
    }
    else {
#ifdef WITH_MPI_CUDA
        auto* Xdata = input(0).template data<T, Context>();
#else
        auto* Xdata = input(0).template data<T, CPUContext>();
#endif
        MPI_Send(Xdata, input(0).count(), mpi_dtype(), this->comm_root, 0, this->comm);
    }
}

template <class Context>
void MPIGatherOp<Context>::RunOnDevice() {
    CHECK_EQ(this->comm_size, OutputSize());
    //  reshape from root
    if (this->comm_rank == this->comm_root) 
        output(0)->ReshapeLike(input(0));

    //  reshape from others
    size_t* all_ndim = new size_t[this->comm_size];
    size_t ndim[1];
    if (this->comm_rank != this->comm_root) {
        ndim[0] = input(0).ndim();
        MPI_Send(ndim, 1, MPI_UNSIGNED_LONG_LONG, this->comm_root, 0, this->comm);
    } else {
        for (int i = 1; i < this->comm_size; i++)
            MPI_Recv(all_ndim + i, 1, MPI_UNSIGNED_LONG_LONG, i, 0, this->comm, MPI_STATUS_IGNORE);
    }
    if (this->comm_rank != this->comm_root) {
        MPI_Send(input(0).dims().data(), (int)ndim[0], MPI_LONG_LONG, this->comm_root, 0, this->comm);
    } else {
        for (int i = 1; i < this->comm_size; i++) {
            TIndex* dims = new TIndex[all_ndim[i]];
            MPI_Recv(dims, (int)all_ndim[i], MPI_LONG_LONG, i, 0, this->comm, MPI_STATUS_IGNORE);
            vector<TIndex> dims_;
            for (int j = 0; j < (int)all_ndim[i]; j++)  dims_.push_back(dims[j]);
            output(i)->Reshape(dims_);
        }
    }

    if (this->dtype == "FLOAT32") RunWithType<float>();
    else LOG(FATAL) << "Unsupported input type: " << this->dtype;
}

DEPLOY_CPU(MPIGather);
#ifdef WITH_CUDA
DEPLOY_CUDA(MPIGather);
#endif
OPERATOR_SCHEMA(MPIGather).NumInputs(1).NumOutputs(1, INT_MAX);

template <class Context> template <typename T>
void MPIGatherGradientOp<Context>::RunWithType() {
    if (this->comm_rank == this->comm_root) {
        output(0)->Share(input(this->comm_rank + 1));
        for (int i = 0; i < this->comm_size; i++) {
            if (i == this->comm_root) continue;
#ifdef WITH_MPI_CUDA
            auto* dYdata = input(this->comm_rank + 1).template data<T, Context>();
#else
            auto* dYdata = input(this->comm_rank + 1).template data<T, CPUContext>();
#endif
            MPI_Send(dYdata, input(this->comm_rank + 1).count(), mpi_dtype(), i, 0, this->comm);
        }
    }
    else{
#ifdef WITH_MPI_CUDA
        auto* dXdata = output(0)->template mutable_data<T, Context>();
#else
        auto* dXdata = output(0)->template mutable_data<T, CPUContext>();
#endif
        MPI_Recv(dXdata, output(0)->count(), mpi_dtype(), this->comm_root, 0, this->comm, MPI_STATUS_IGNORE);
    }
}

template <class Context>
void MPIGatherGradientOp<Context>::RunOnDevice() {
    output(0)->ReshapeLike(input(0));

    if (this->dtype == "FLOAT32") RunWithType<float>();
    else LOG(FATAL) << "Unsupported input type: " << this->dtype;
}

DEPLOY_CPU(MPIGatherGradient);
#ifdef WITH_CUDA
DEPLOY_CUDA(MPIGatherGradient);
#endif
OPERATOR_SCHEMA(MPIGatherGradient).NumInputs(2, INT_MAX).NumOutputs(1);

class GetMPIGatherGradient final : public GradientMakerBase {
 public:
    GRADIENT_MAKER_CTOR(GetMPIGatherGradient);
    vector<OperatorDef> MakeDefs() override {
        vector<string> inputs(1, I(0));
        for (auto out : def.output()) inputs.push_back(out + "_grad");
        return SingleDef(def.type() + "Gradient", "", 
            inputs, vector<string> {GI(0)});
    }
};
REGISTER_GRADIENT(MPIGather, GetMPIGatherGradient);

}    // namespace dragon

#endif // WITH_MPI