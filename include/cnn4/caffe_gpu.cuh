

#include <thrust/device_vector.h>
#include <thrust/functional.h>  // thrust::plus
#include <thrust/reduce.h>
#include <cmath>
#include "wstd/logging.hpp"
#include "cpu.hpp"
#include "math_functions.h"


#define _CONTEXT GPUContext* context

#include "math_functions.cuh"

#define Dtype float
#include "math_functions.cuh"
#undef Dtype
#define Dtype double
#include "math_functions.cuh"
#undef Dtype

#undef _CONTEXT

