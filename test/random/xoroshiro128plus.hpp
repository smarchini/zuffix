
#pragma once

#include <gtest/gtest.h>
#include <random>
#include <zuffix/random/xoroshiro128plus_engine.hpp>

// TODO these is not testing

TEST(xoroshiro128plus, uniform_int_0_100) {
  std::random_device rd;
  zarr::xoroshiro128plus_engine rng(rd());
  std::uniform_int_distribution<int> dist(0, 100);

  for (int i=0; i<10; ++i)
    std::cout << dist(rng) << "\n";
}

TEST(xoroshiro128plus, uniform_real_0_1) {
  std::random_device rd;
  zarr::xoroshiro128plus_engine rng(rd());
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  for (int i=0; i<10; ++i)
    std::cout << dist(rng) << "\n";
}

TEST(xoroshiro128plus, uniform_int_50_100) {
  std::random_device rd;
  zarr::xoroshiro128plus_engine rng(rd());
  std::uniform_int_distribution<int> dist(50, 100);

  for (int i=0; i<10; ++i)
    std::cout << dist(rng) << "\n";
}

TEST(xoroshiro128plus, uniform_real_50_100) {
  std::random_device rd;
  zarr::xoroshiro128plus_engine rng(rd());
  std::uniform_real_distribution<double> dist(50.0, 100.0);

  for (int i=0; i<10; ++i)
    std::cout << dist(rng) << "\n";
}
