/*
 * arithmetic_aggregation.h
 *
 *  Created on: Oct 13, 2015
 *      Author: pst
 */

#ifndef AUXILIARIES_ARITHMETIC_AGGREGATION_H_
#define AUXILIARIES_ARITHMETIC_AGREGGATION_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_based_data.h>

namespace aux {

class ArithmeticAggregation : public TimeBasedData {
 public:
  /**
   * @brief Default constructor
   */
  ArithmeticAggregation() = delete;
  virtual ~ArithmeticAggregation() override = default;
  ArithmeticAggregation(const ArithmeticAggregation&);
  ArithmeticAggregation(ArithmeticAggregation&&);
  ArithmeticAggregation& operator=(const ArithmeticAggregation& other);
  ArithmeticAggregation& operator=(ArithmeticAggregation&& other);
  /**
  * @brief Constructor
  *
  * @param[in] tbd_a Time based Data
  * @param[in] tbd_b Time based Data
  * @param[in] binary_arithmetic_function Metafunction defining the binary arithmetic function to execute
  */
  ArithmeticAggregation(std::unique_ptr<TimeBasedData> tbd_a,
                        std::unique_ptr<TimeBasedData> tbd_b,
                        const std::function<double(double, double)>& binary_arithmetic_function)
    : tbd_a_(std::move(tbd_a)),
      tbd_b_(std::move(tbd_b)),
      binary_arithmetic_function_(binary_arithmetic_function) {}

  /**
   * @brief Overloaded operator defining the selection of an element from the data
   *
   * @param[in] time_point Local time point defining the position of a value in the time based data
   * @return The value of the local time defined by the input variable
   */
  using TimeBasedData::operator [];
  virtual double operator[](const SimulationClock::time_point& time_point) const override {
    return (binary_arithmetic_function_((*tbd_a_)[time_point], (*tbd_b_)[time_point]));
  }

  /**
   * @brief Copies and stores the time based data in a new unique pointer
   *
   * @return Unique pointer containing the copied time based data
   */
  virtual std::unique_ptr<TimeBasedData> clone() const override {
    return std::unique_ptr<TimeBasedData>(new ArithmeticAggregation(*this));
  }

  virtual SimulationClock::time_point start() const override final {
    return std::min((*tbd_a_).start(), (*tbd_b_).start());
  }

  /**
   * @brief Overridden virtual function
   */
  virtual std::string PrintToString() const override {return "dummy return - AA is not printable (yet?)";}

 private:
  std::unique_ptr<TimeBasedData> tbd_a_;
  std::unique_ptr<TimeBasedData> tbd_b_;
  std::function<double(double, double)> binary_arithmetic_function_;
};

/**
 * @brief Overloaded operator defining the addition of time based data
 *
 * @return By the overloaded operator modified time based data
 */
std::unique_ptr<TimeBasedData> operator +(std::unique_ptr<TimeBasedData> tbd_a,
                                          std::unique_ptr<TimeBasedData> tbd_b);
/**
 * @brief Overloaded operator defining the subtraction of time based data
 *
 * @return By the overloaded operator modified time based data
 */
std::unique_ptr<TimeBasedData> operator -(std::unique_ptr<TimeBasedData> tbd_a,
                                          std::unique_ptr<TimeBasedData> tbd_b);
/**
 * @brief Overloaded operator defining the multiplication of time based data
 *
 * @return By the overloaded operator modified time based data
 */
std::unique_ptr<TimeBasedData> operator *(std::unique_ptr<TimeBasedData> tbd_a,
                                          std::unique_ptr<TimeBasedData> tbd_b);
} /* namespace aux */

#endif /* AUXILIARIES_ARITHMETIC_AGREGGATION_H_ */
