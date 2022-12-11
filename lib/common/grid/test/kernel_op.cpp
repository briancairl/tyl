/**
 * @copyright 2022-present Brian Cairl
 *
 * @file kernel_op.cpp
 */

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/grid/kernel_op.hpp>

// Eigen
#include <Eigen/Dense>

using namespace tyl;

static constexpr long int N = 10;
static constexpr long int M = 8;

TEST(KernelOp, TopLeftCornerWrapBehavior)
{
  const Eigen::Matrix<float, N, M> src = [] {
    Eigen::Matrix<float, N, M> m = Eigen::Matrix<float, N, M>::Zero();
    m(0, 0) = 1;
    return m;
  }();

  Eigen::Matrix<float, N, M> dst = Eigen::Matrix<float, N, M>::Zero();

  kernel_op(
    dst.rows(),
    dst.cols(),
    [&src, &dst](const auto i, const auto j, const auto ni, const auto nj, const auto distance) {
      if (src(i, j) == 1)
      {
        dst(ni, nj) = distance;
      }
    });

  ASSERT_GE(dst(0, M - 1), 1) << dst;
  ASSERT_GE(dst(1, M - 1), 1) << dst;
  ASSERT_GE(dst(N - 1, 0), 1) << dst;
  ASSERT_GE(dst(N - 1, 1), 1) << dst;
  ASSERT_GE(dst(N - 1, M - 1), 1) << dst;
}

TEST(KernelOp, TopRightCornerWrapBehavior)
{
  const Eigen::Matrix<float, N, M> src = [] {
    Eigen::Matrix<float, N, M> m = Eigen::Matrix<float, N, M>::Zero();
    m(0, M - 1) = 1;
    return m;
  }();

  Eigen::Matrix<float, N, M> dst = Eigen::Matrix<float, N, M>::Zero();

  kernel_op(
    dst.rows(),
    dst.cols(),
    [&src, &dst](const auto i, const auto j, const auto ni, const auto nj, const auto distance) {
      if (src(i, j) == 1)
      {
        dst(ni, nj) = distance;
      }
    });

  ASSERT_GE(dst(0, 0), 1) << dst;
  ASSERT_GE(dst(1, 0), 1) << dst;
  ASSERT_GE(dst(N - 1, 0), 1) << dst;
  ASSERT_GE(dst(N - 1, M - 2), 1) << dst;
  ASSERT_GE(dst(N - 1, M - 1), 1) << dst;
}

TEST(KernelOp, BottomLeftCornerWrapBehavior)
{
  const Eigen::Matrix<float, N, M> src = [] {
    Eigen::Matrix<float, N, M> m = Eigen::Matrix<float, N, M>::Zero();
    m(N - 1, 0) = 1;
    return m;
  }();

  Eigen::Matrix<float, N, M> dst = Eigen::Matrix<float, N, M>::Zero();

  kernel_op(
    dst.rows(),
    dst.cols(),
    [&src, &dst](const auto i, const auto j, const auto ni, const auto nj, const auto distance) {
      if (src(i, j) == 1)
      {
        dst(ni, nj) = distance;
      }
    });

  ASSERT_GE(dst(0, 0), 1) << dst;
  ASSERT_GE(dst(0, 1), 1) << dst;
  ASSERT_GE(dst(0, M - 1), 1) << dst;
  ASSERT_GE(dst(N - 2, M - 1), 1) << dst;
  ASSERT_GE(dst(N - 1, M - 1), 1) << dst;
}

TEST(KernelOp, BottomRightCornerWrapBehavior)
{
  const Eigen::Matrix<float, N, M> src = [] {
    Eigen::Matrix<float, N, M> m = Eigen::Matrix<float, N, M>::Zero();
    m(N - 1, 0) = 1;
    return m;
  }();

  Eigen::Matrix<float, N, M> dst = Eigen::Matrix<float, N, M>::Zero();

  kernel_op(
    dst.rows(),
    dst.cols(),
    [&src, &dst](const auto i, const auto j, const auto ni, const auto nj, const auto distance) {
      if (src(i, j) == 1)
      {
        dst(ni, nj) = distance;
      }
    });

  ASSERT_GE(dst(0, 0), 1) << dst;
  ASSERT_GE(dst(0, 1), 1) << dst;
  ASSERT_GE(dst(0, M - 1), 1) << dst;
  ASSERT_GE(dst(N - 2, M - 1), 1) << dst;
  ASSERT_GE(dst(N - 1, M - 1), 1) << dst;
}

TEST(KernelOp, TopRowWrapBehavior)
{
  const Eigen::Matrix<float, N, M> src = [] {
    Eigen::Matrix<float, N, M> m = Eigen::Matrix<float, N, M>::Zero();
    m(0, M / 2) = 1;
    return m;
  }();

  Eigen::Matrix<float, N, M> dst = Eigen::Matrix<float, N, M>::Zero();

  kernel_op(
    dst.rows(),
    dst.cols(),
    [&src, &dst](const auto i, const auto j, const auto ni, const auto nj, const auto distance) {
      if (src(i, j) == 1)
      {
        dst(ni, nj) = distance;
      }
    });

  ASSERT_GE(dst(N - 1, M / 2), 1) << dst;
  ASSERT_GE(dst(N - 1, M / 2 - 1), 1) << dst;
  ASSERT_GE(dst(N - 1, M / 2 + 1), 1) << dst;
  ASSERT_GE(dst(0, M / 2 - 1), 1) << dst;
  ASSERT_GE(dst(0, M / 2 + 1), 1) << dst;
  ASSERT_GE(dst(1, M / 2), 1) << dst;
  ASSERT_GE(dst(1, M / 2 - 1), 1) << dst;
  ASSERT_GE(dst(1, M / 2 + 1), 1) << dst;
}

TEST(KernelOp, BottomRowWrapBehavior)
{
  const Eigen::Matrix<float, N, M> src = [] {
    Eigen::Matrix<float, N, M> m = Eigen::Matrix<float, N, M>::Zero();
    m(N - 1, M / 2) = 1;
    return m;
  }();

  Eigen::Matrix<float, N, M> dst = Eigen::Matrix<float, N, M>::Zero();

  kernel_op(
    dst.rows(),
    dst.cols(),
    [&src, &dst](const auto i, const auto j, const auto ni, const auto nj, const auto distance) {
      if (src(i, j) == 1)
      {
        dst(ni, nj) = distance;
      }
    });

  ASSERT_GE(dst(N - 2, M / 2), 1) << dst;
  ASSERT_GE(dst(N - 2, M / 2 - 1), 1) << dst;
  ASSERT_GE(dst(N - 2, M / 2 + 1), 1) << dst;
  ASSERT_GE(dst(N - 1, M / 2 - 1), 1) << dst;
  ASSERT_GE(dst(N - 1, M / 2 + 1), 1) << dst;
  ASSERT_GE(dst(0, M / 2), 1) << dst;
  ASSERT_GE(dst(0, M / 2 - 1), 1) << dst;
  ASSERT_GE(dst(0, M / 2 + 1), 1) << dst;
}

TEST(KernelOp, LeftColWrapBehavior)
{
  const Eigen::Matrix<float, N, M> src = [] {
    Eigen::Matrix<float, N, M> m = Eigen::Matrix<float, N, M>::Zero();
    m(N / 2, 0) = 1;
    return m;
  }();

  Eigen::Matrix<float, N, M> dst = Eigen::Matrix<float, N, M>::Zero();

  kernel_op(
    dst.rows(),
    dst.cols(),
    [&src, &dst](const auto i, const auto j, const auto ni, const auto nj, const auto distance) {
      if (src(i, j) == 1)
      {
        dst(ni, nj) = distance;
      }
    });

  ASSERT_GE(dst(N / 2 + 1, 1), 1) << dst;
  ASSERT_GE(dst(N / 2, 1), 1) << dst;
  ASSERT_GE(dst(N / 2 - 1, 1), 1) << dst;
  ASSERT_GE(dst(N / 2 + 1, 0), 1) << dst;
  ASSERT_GE(dst(N / 2 - 1, 0), 1) << dst;
  ASSERT_GE(dst(N / 2 + 1, M - 1), 1) << dst;
  ASSERT_GE(dst(N / 2, M - 1), 1) << dst;
  ASSERT_GE(dst(N / 2 - 1, M - 1), 1) << dst;
}

TEST(KernelOp, RightColWrapBehavior)
{
  const Eigen::Matrix<float, N, M> src = [] {
    Eigen::Matrix<float, N, M> m = Eigen::Matrix<float, N, M>::Zero();
    m(N / 2, M - 1) = 1;
    return m;
  }();

  Eigen::Matrix<float, N, M> dst = Eigen::Matrix<float, N, M>::Zero();

  kernel_op(
    dst.rows(),
    dst.cols(),
    [&src, &dst](const auto i, const auto j, const auto ni, const auto nj, const auto distance) {
      if (src(i, j) == 1)
      {
        dst(ni, nj) = distance;
      }
    });

  ASSERT_GE(dst(N / 2 + 1, M - 2), 1) << dst;
  ASSERT_GE(dst(N / 2, M - 2), 1) << dst;
  ASSERT_GE(dst(N / 2 - 1, M - 2), 1) << dst;
  ASSERT_GE(dst(N / 2 + 1, M - 1), 1) << dst;
  ASSERT_GE(dst(N / 2 - 1, M - 1), 1) << dst;
  ASSERT_GE(dst(N / 2 + 1, 0), 1) << dst;
  ASSERT_GE(dst(N / 2, 0), 1) << dst;
  ASSERT_GE(dst(N / 2 - 1, 0), 1) << dst;
}
