/*
 * time_series_repeat_const.h
 *
 *  Created on: Oct 12, 2015
 *      Author: pst
 */

#ifndef AUXILIARIES_TIME_SERIES_REPEAT_CONST_H_
#define AUXILIARIES_TIME_SERIES_REPEAT_CONST_H_

#include <memory>
#include <string>
#include <vector>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_series.h>

namespace aux {

class TimeSeriesRepeatConst : public TimeSeries {
 public:
  /**
  * @brief Default constructor
  */
  TimeSeriesRepeatConst() = delete;

  /**
  * @brief Creates time based data with the specifications of the input csv
  *
  * @param[in] values Data to store as time based data
  * @param[in] start Start point of the time based data
  * @param[in] interval Interval between two points to the time based data
  */
  template <typename Duration, typename = typename std::enable_if<std::ratio_divide<typename Duration::period,
      SimulationClock::period>::den == 1>::type >
  TimeSeriesRepeatConst(const std::vector<double>& values,
                        SimulationClock::time_point start,
                        Duration interval = aux::hours(1))
      : TimeSeries(values, start, interval) {}

  /**
   * @brief Overloaded operator defining the selection of an element from the data
   *
   * @param[in] time_point Local time point defining the position of a value in the time based data
   * @return The remainder that results from the division between the local value and the size of the object
   */
  using TimeBasedData::operator [];
  virtual double operator[](const SimulationClock::time_point& time_point) const override;

  /**
   * @brief Copies and stores the time based data in a new unique pointer
   *
   * @return Unique pointer containing the copied time based data
   */
  virtual std::unique_ptr<TimeBasedData> clone() const override {
    return std::unique_ptr<TimeBasedData>(new TimeSeriesRepeatConst(*this));
  }

  virtual double Mean(SimulationClock::time_point tp_start,
                      SimulationClock::time_point tp_end) const override;

  virtual std::string PrintToString() const override {return PrintToStringTS("TS_repeat_const");}
};

} /* namespace aux */

#endif /* AUXILIARIES_TIME_SERIES_H_ */

