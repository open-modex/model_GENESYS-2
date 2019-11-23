/*
 * arithmetic_aggregation.cc
 *
 *  Created on: Nov 16, 2015
 *      Author: pst
 */

#include <auxiliaries/arithmetic_aggregation.h>

#include <exception>
#include <iostream>

namespace aux {

ArithmeticAggregation::ArithmeticAggregation(const ArithmeticAggregation& other)
    : tbd_a_(std::move((*other.tbd_a_).clone())),
      tbd_b_(std::move((*other.tbd_b_).clone())),
      binary_arithmetic_function_(other.binary_arithmetic_function_) {}

ArithmeticAggregation::ArithmeticAggregation(ArithmeticAggregation&& other)
    : tbd_a_(std::move(other.tbd_a_)),
      tbd_b_(std::move(other.tbd_b_)),
      binary_arithmetic_function_(std::move(other.binary_arithmetic_function_)) {}

ArithmeticAggregation& ArithmeticAggregation::operator=(const ArithmeticAggregation& other) {
  tbd_a_ = std::move((*other.tbd_a_).clone());
  tbd_b_ = std::move((*other.tbd_b_).clone());
  binary_arithmetic_function_ = other.binary_arithmetic_function_;
  return *this;
}

ArithmeticAggregation& ArithmeticAggregation::operator=(ArithmeticAggregation&& other) {
  tbd_a_ = std::move(other.tbd_a_);
  tbd_b_ = std::move(other.tbd_b_);
  binary_arithmetic_function_ = std::move(other.binary_arithmetic_function_);
  return *this;
}

std::unique_ptr<TimeBasedData> operator +(std::unique_ptr<TimeBasedData> tbd_a,
                                          std::unique_ptr<TimeBasedData> tbd_b) {
  return std::unique_ptr<TimeBasedData>(new ArithmeticAggregation(std::move(tbd_a), std::move(tbd_b),
                                                                  std::plus<double>()));
}

std::unique_ptr<TimeBasedData> operator -(std::unique_ptr<TimeBasedData> tbd_a,
                                          std::unique_ptr<TimeBasedData> tbd_b) {
  return std::unique_ptr<TimeBasedData>(new ArithmeticAggregation(std::move(tbd_a), std::move(tbd_b),
                                                                  std::minus<double>()));
}

std::unique_ptr<TimeBasedData> operator *(std::unique_ptr<TimeBasedData> tbd_a,
                                          std::unique_ptr<TimeBasedData> tbd_b) {
  return std::unique_ptr<TimeBasedData>(new ArithmeticAggregation(std::move(tbd_a), std::move(tbd_b),
                                                                  std::multiplies<double>()));
}

} /* namespace aux */
