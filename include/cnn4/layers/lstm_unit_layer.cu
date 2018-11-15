#include <algorithm>
#include <cmath>



#include "caffe/layers/lstm_layer.hpp"

namespace {

template <typename Dtype>
__device__ Dtype sigmoid1(const Dtype x) {
  return Dtype(1) / (Dtype(1) + exp(-x));
}

template <typename Dtype>
__device__ Dtype tanh1(const Dtype x) {
  return Dtype(2) * sigmoid(Dtype(2) * x) - Dtype(1);
}

template <typename Dtype>
__global__ void LSTMActsForward(const int nthreads, const int dim,
                                const Dtype* X, Dtype* X_acts) {
  CUDA_KERNEL_LOOP(index, nthreads) {
    const int x_dim = 4 * dim;
    const int d = index % x_dim;
    if (d < 3 * dim) {
      X_acts[index] = sigmoid(X[index]);
    } else {
      X_acts[index] = tanh(X[index]);
    }
  }
}

template <typename Dtype>
__global__ void LSTMUnitForward(const int nthreads, const int dim,
    const Dtype* C_prev, const Dtype* X, const Dtype* cont,
    Dtype* C, Dtype* H) {
  CUDA_KERNEL_LOOP(index, nthreads) {
    const int n = index / dim;
    const int d = index % dim;
    const Dtype* X_offset = X + 4 * dim * n;
    const Dtype i = X_offset[d];
    const Dtype f = X_offset[1 * dim + d];
    const Dtype o = X_offset[2 * dim + d];
    const Dtype g = X_offset[3 * dim + d];
    const Dtype c_prev = C_prev[index];
    const Dtype c = cont[n] * f * c_prev + i * g;
    C[index] = c;
    const Dtype tanh_c = tanh(c);
    H[index] = o * tanh_c;
  }
}

template <typename Dtype>
void LSTMUnitLayer::Forward_(GPUContext* context, const vector<Blob*>& bottom,
    const vector<Blob*>& top) {
  const int count = top[1]->count();
  const Dtype* C_prev = bottom[0]->data();
  const Dtype* X = bottom[1]->data();
  const Dtype* cont = bottom[2]->data();
  Dtype* X_acts = X_acts_.mdata();
  Dtype* C = top[0]->mdata();
  Dtype* H = top[1]->mdata();
  const int X_count = bottom[1]->count();
  // NOLINT_NEXT_LINE(whitespace/operators)
  LSTMActsForward<Dtype><<<CAFFE_GET_BLOCKS(X_count), CAFFE_CUDA_NUM_THREADS>>>(
      X_count, hidden_dim_, X, X_acts);
  CUDA_POST_KERNEL_CHECK;
  // NOLINT_NEXT_LINE(whitespace/operators)
  LSTMUnitForward<Dtype><<<CAFFE_GET_BLOCKS(count), CAFFE_CUDA_NUM_THREADS>>>(
      count, hidden_dim_, C_prev, X_acts, cont, C, H);
  CUDA_POST_KERNEL_CHECK;
}

template <typename Dtype>
__global__ void LSTMUnitBackward(const int nthreads, const int dim,
    const Dtype* C_prev, const Dtype* X, const Dtype* C, const Dtype* H,
    const Dtype* cont, const Dtype* C_diff, const Dtype* H_diff,
    Dtype* C_prev_diff, Dtype* X_diff) {
  CUDA_KERNEL_LOOP(index, nthreads) {
    const int n = index / dim;
    const int d = index % dim;
    const Dtype* X_offset = X + 4 * dim * n;
    const Dtype i = X_offset[d];
    const Dtype f = X_offset[1 * dim + d];
    const Dtype o = X_offset[2 * dim + d];
    const Dtype g = X_offset[3 * dim + d];
    const Dtype c_prev = C_prev[index];
    const Dtype c = C[index];
    const Dtype tanh_c = tanh(c);
    Dtype* c_prev_diff = C_prev_diff + index;
    Dtype* X_diff_offset = X_diff + 4 * dim * n;
    Dtype* i_diff = X_diff_offset + d;
    Dtype* f_diff = X_diff_offset + 1 * dim + d;
    Dtype* o_diff = X_diff_offset + 2 * dim + d;
    Dtype* g_diff = X_diff_offset + 3 * dim + d;
    const Dtype c_term_diff =
        C_diff[index] + H_diff[index] * o * (1 - tanh_c * tanh_c);
    const Dtype cont_n = cont[n];
    *c_prev_diff = cont_n * c_term_diff * f;
    *i_diff = c_term_diff * g;
    *f_diff = cont_n * c_term_diff * c_prev;
    *o_diff = H_diff[index] * tanh_c;
    *g_diff = c_term_diff * i;
  }
}

template <typename Dtype>
__global__ void LSTMActsBackward(const int nthreads, const int dim,
    const Dtype* X_acts, const Dtype* X_acts_diff, Dtype* X_diff) {
  CUDA_KERNEL_LOOP(index, nthreads) {
    const int x_dim = 4 * dim;
    const int d = index % x_dim;
    const Dtype X_act = X_acts[index];
    if (d < 3 * dim) {
      X_diff[index] = X_acts_diff[index] * X_act * (Dtype(1) - X_act);
    } else {
      X_diff[index] = X_acts_diff[index] * (Dtype(1) - X_act * X_act);
    }
  }
}

template <typename Dtype>
void LSTMUnitLayer::Backward_(GPUContext* context, const vector<Blob*>& top,
    const vector<bool>& propagate_down,
    const vector<Blob*>& bottom) {
  CHECK(!propagate_down[2]) << "Cannot backpropagate to sequence indicators.";
  if (!bottom[0]->propagate_down_ && !bottom[1]->propagate_down_) { return; }

  const int count = top[1]->count();
  const Dtype* C_prev = bottom[0]->data();
  const Dtype* X_acts = X_acts_.data();
  const Dtype* cont = bottom[2]->data();
  const Dtype* C = top[0]->data();
  const Dtype* H = top[1]->data();
  const Dtype* C_diff = top[0]->gpu_diff();
  const Dtype* H_diff = top[1]->gpu_diff();
  Dtype* C_prev_diff = bottom[0]->gpu_mdiff();
  Dtype* X_acts_diff = X_acts_.gpu_mdiff();
  LSTMUnitBackward<Dtype>  // NOLINT_NEXT_LINE(whitespace/operators)
      <<<CAFFE_GET_BLOCKS(count), CAFFE_CUDA_NUM_THREADS>>>(count, hidden_dim_,
      C_prev, X_acts, C, H, cont, C_diff, H_diff, C_prev_diff, X_acts_diff);
  CUDA_POST_KERNEL_CHECK;
  const int X_count = bottom[1]->count();
  Dtype* X_diff = bottom[1]->gpu_mdiff();
  LSTMActsBackward<Dtype>  // NOLINT_NEXT_LINE(whitespace/operators)
      <<<CAFFE_GET_BLOCKS(X_count), CAFFE_CUDA_NUM_THREADS>>>(
      X_count, hidden_dim_, X_acts, X_acts_diff, X_diff);
  CUDA_POST_KERNEL_CHECK;
}

INSTANTIATE_LAYER_GPU_FUNCS(LSTMUnitLayer);

}  // namespace
