/*
 * time_series_repeat_linear.h
 *
 *  Created on: Oct 12, 2015
 *      Author: pst
 */

#ifndef AUXILIARIES_TIME_SERIES_REPEAT_LINEAR_H_
#define AUXILIARIES_TIME_SERIES_REPEAT_LINEAR_H_

#include <memory>
#include <string>
#include <vector>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_series.h>

namespace aux {

class TimeSeriesRepeatLinear : public TimeSeries {
 public:
  /**
   * @brief Default constructor
   */
  TimeSeriesRepeatLinear() = delete;

  /**
  * @brief Constructor
  *
  * @param[in] values Vector of numeric values
  * @param[in] start Start time point
  * @param[in] interval Duration with standard default of 1 hour
  */
  template <typename Duration, typename = typename std::enable_if<std::ratio_divide<typename Duration::period,
      SimulationClock::period>::den == 1>::type >
  TimeSeriesRepeatLinear(const std::vector<double>& values,
                         SimulationClock::time_point start,
                         Duration interval = aux::hours(1))
      : TimeSeries(values, start, interval) {}

  /**
   * @brief Overloaded operator defining the selection of an element from the data
   * @details Linear Interpolation is used for the determination of the unknown parameters
   *
   * @param[in] time_point Local time point defining the position of a value in the time based data
   * @return The value of the local time defined by the input variable
   */
  using TimeBasedData::operator [];
  virtual double operator[](const SimulationClock::time_point& time_point) const override;

  /**
   * @brief Copies and stores the time based data in a new unique pointer
   *
   * @return Unique pointer containing the copied time based data
   */
  virtual std::unique_ptr<TimeBasedData> clone() const override {
    return std::unique_ptr<TimeBasedData>(new TimeSeriesRepeatLinear(*this));
  }

  virtual double Mean(SimulationClock::time_point tp_start,
                      SimulationClock::time_point tp_end) const override;

  virtual std::string PrintToString() const override {return PrintToStringTS("TS_repeat_linear");}
};

} /* namespace aux */

#endif /* AUXILIARIES_TIME_SERIES_H_ */

