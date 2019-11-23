// ==================================================================
//
//  GENESYS2 is an optimisation tool and model of the European electricity supply system.
//
//  Copyright (C) 2015, 2016, 2017.  Robin Beer, Christian Bussar, Zhuang Cai, Kevin
//  Jacque, Luiz Moraes Jr., Philipp Stöcker
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation; either version 3 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//  02110-1301 USA.
//
//  Project host: RWTH Aachen University, Aachen, Germany
//  Website: http://www.genesys.rwth-aachen.de
//
// ==================================================================
//
// time_series.h
//
// This file is part of the genesys-framework v.2

#ifndef AUXILIARIES_TIME_SERIES_H_
#define AUXILIARIES_TIME_SERIES_H_

#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <tuple>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_based_data.h>

namespace aux {

class TimeSeries : public TimeBasedData {
 public:

  /**
    * @brief Default deconstructor
    */
  virtual ~TimeSeries() = default;

  /**
   * @brief Gets the start time point of the time based data
   *
   * @return Start time point
   */
  virtual SimulationClock::time_point start() const override final {return (start_);}
 
  /**
   * @brief Stores the time based data into a string

   * @return String containing the information of the time based data
   */
  virtual std::string PrintToString() const override {return "dummy return - this is plain TS, should not be instantiated";}
  std::tuple<std::string, std::string, std::string> toXml() const;

  virtual bool empty() const override{return ((size() > 0) ? false : true);}

 protected:
  std::string PrintToStringTS(std::string type) const; // deliberately passing by value

  typedef std::vector<double>       data_structure;
  typedef data_structure::size_type index_type;
  typedef SimulationClock::duration interval_type;

  /**
  * @brief Default constructor
  */
  TimeSeries()
      : values_(std::vector<double>(1, 0.0)),
        size_(),
        interval_(),
        start_(),
        end_() {} // note: This constructor should only be used by TimeSeriesConstAddable

  /**
  * @brief Constructor
  *
  * @param[in] values Data to store as time based data
  * @param[in] start Start point of the time based data
  * @param[in] interval Interval between two points to the time based data
  */
  template <typename Duration, typename = typename std::enable_if<std::ratio_divide<typename Duration::period,
      SimulationClock::period>::den == 1>::type >
  TimeSeries(const std::vector<double>& values,
             SimulationClock::time_point start,
             Duration interval = aux::hours(1))
      : values_(values),
        size_(values_.size()),
        interval_(interval),
        start_(start),
        end_(start + interval * (size_ - 1)) {
    if (values_.empty()) {
      std::cerr << "Error in aux::TimeSeries::TimeSeries :" << std::endl
          << "Trying to create empty TimeSeries object." << std::endl;
      std::terminate();
    }
  }

  /**
   * @brief Copies and stores the time based data in a new unique pointer
   *
   * @return Unique pointer containing the copied time based data
   */
  virtual std::unique_ptr<TimeBasedData> clone() const;

  /**
   * @brief Determines the time point index of the time based data
   *
   * @param[in] time_point Local time point defining the position of a value in the time based data
   * @param[out] fraction Fraction part of the division of the index position and the interval length
   * @return The time point index of the data specified by the input variable
   */
  index_type locate_time(const SimulationClock::time_point& time_point,
                         double& fraction) const;

  /**
   * @brief Determines the time point index of the time based data
   *
   * @param[in] time_point Local time point defining the position of a value in the time based data
   * @return The time point index of the data specified by the input variable
   */
  index_type locate_time(const SimulationClock::time_point& time_point) const;

  /**
   * @brief Gets the end time point of the time based data
   *
   * @return End time point
   */
  SimulationClock::time_point end() const {return (end_);}

  /**
   * @brief Gets the length of the time based data
   *
   * @return Length of the time based data
   */
  index_type size() const {return (size_);}

  /**
   * @brief Gets the values of the time based data
   *
   * @return Values of the time based data
   */
  data_structure values() const {return (values_);}

  /**
   * @brief Gets the value of the time based data specified by the input variable
   *
   * @return Value of the time based data specified by the input variable
   */
  double value(index_type index) const {return (values_[index]);}

  /**
   * @brief Gets the interval length of the time based data
   *
   * @return Interval length of the time based data
   */
  interval_type interval() const {return (interval_);}

  /**
   * @brief Exchanges the time based data with the data of the input variable
   *
   * @param[in] other_values Value of the new time based data
   * @param[in] other_size Length of the new time based data
   * @param[in] other_start Start time point of the new time based data
   */
  void swap_values(data_structure& other_values,
                   index_type& other_size,
                   SimulationClock::time_point other_start);

  /**
   * @brief Appends a new value to the actual time based data

   * @param[in] value New value to add
   */
  void append(const double value);

  /**
   * @brief Adds a value to a point of the actual time based data

   * @param[in] value New value to add
   * @param[in] index Position of the time based data where the new data will be added
   */
  void add_assign_value(double value,
                        index_type index) {values_[index] += value;}

  /**
   * @brief Update the end of the actual time based data

   * @param[in] new_end New value for the end
   */
  void update_end(SimulationClock::time_point new_end) {end_ = new_end;}

  /**
   * @brief Update the interval length of the actual time based data

   * @param[in] new_end New value for the interval length
   */
  void update_interval(SimulationClock::duration interval);

 private:
  data_structure values_;
  index_type size_;
  interval_type interval_;
  SimulationClock::time_point start_;
  SimulationClock::time_point end_;
};

} /* namespace aux */

#endif /* AUXILIARIES_TIME_SERIES_H_ */

