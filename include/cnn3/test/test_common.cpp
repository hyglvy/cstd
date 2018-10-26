#include "gtest/gtest.h"

#include "common.hpp"
#include "syncedmem.hpp"
#include "math_functions.hpp"

#include "test/test_caffe_main.hpp"

namespace caffe
{

  class CommonTest : public ::testing::Test {};

#ifndef CPU_ONLY  // GPU Caffe singleton test.

  TEST_F(CommonTest, TestCublasHandlerGPU)
  {
    int cuda_device_id;
    CUDA_CHECK(cudaGetDevice(&cuda_device_id));
    EXPECT_TRUE(cublas_handle());
  }

#endif

  TEST_F(CommonTest, TestBrewMode)
  {
    set_mode(CPU);
    EXPECT_EQ(mode(), CPU);
    set_mode(GPU);
    EXPECT_EQ(mode(), GPU);
  }

  TEST_F(CommonTest, TestRandSeedCPU)
  {
    SyncedMemory data_a(10 * sizeof(int));
    SyncedMemory data_b(10 * sizeof(int));
    set_random_seed(1701);
    caffe_rng_bernoulli(10, 0.5, static_cast<int*>(data_a.mutable_cpu_data()));
    set_random_seed(1701);
    caffe_rng_bernoulli(10, 0.5, static_cast<int*>(data_b.mutable_cpu_data()));
    for (int i = 0; i < 10; ++i) {
      EXPECT_EQ(static_cast<const int*>(data_a.cpu_data())[i],
                static_cast<const int*>(data_b.cpu_data())[i]);
    }
  }

#ifndef CPU_ONLY  // GPU Caffe singleton test.

  TEST_F(CommonTest, TestRandSeedGPU)
  {
    SyncedMemory data_a(10 * sizeof(unsigned int));
    SyncedMemory data_b(10 * sizeof(unsigned int));
    set_random_seed(1701);
    CURAND_CHECK(curandGenerate(curand_generator(),
                                static_cast<unsigned int*>(data_a.mutable_gpu_data()), 10));
    set_random_seed(1701);
    CURAND_CHECK(curandGenerate(curand_generator(),
                                static_cast<unsigned int*>(data_b.mutable_gpu_data()), 10));
    for (int i = 0; i < 10; ++i) {
      EXPECT_EQ(((const unsigned int*)(data_a.cpu_data()))[i],
                ((const unsigned int*)(data_b.cpu_data()))[i]);
    }
  }

#endif

}  // namespace caffe
