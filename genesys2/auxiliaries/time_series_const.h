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
// time_series_const.h
//
// This file is part of the genesys-framework v.2

#ifndef AUXILIARIES_TIME_SERIES_CONST_H_
#define AUXILIARIES_TIME_SERIES_CONST_H_

#include <memory>
#include <string>
#include <vector>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_series.h>

namespace aux {

class TimeSeriesConst : public TimeSeries {
 public:
  friend class TimeSeriesConstAddable;

  /**
   * @brief Constructor with meta variables
   *
   * @param[in] values Data to store as time based data
   * @param[in] start Start point of the time based data
   * @param[in] interval Constant interval between two points to the time based data
   */
  template <typename Duration, typename = typename std::enable_if<std::ratio_divide<typename Duration::period,
      SimulationClock::period>::den == 1>::type >
  TimeSeriesConst(const std::vector<double>& values,
                  SimulationClock::time_point start,
                  Duration interval = aux::hours(1))
      : TimeSeries(values, start, interval) {}

  /**
  * @brief Default deconstructor
  */
  virtual ~TimeSeriesConst() = default;

  /**
   * @brief Overloaded operator defining the selection of an element from the data
   *
   * @param[in] time_point Local time point defining the position of a value in the time based data
   * @return The value of the local time point specified by the input variable
   */
  using TimeBasedData::operator [];
  virtual double operator [](const SimulationClock::time_point& time_point) const override;

  /**
   * @brief Copies and stores the time based data with constant interval in a new unique pointer
   *
   * @return Unique pointer containing the copied time based data
   */
  std::unique_ptr<TimeBasedData> clone() const override {
    return std::unique_ptr<TimeBasedData>(new TimeSeriesConst(*this));
  }

  virtual double Mean(SimulationClock::time_point tp_start,
                      SimulationClock::time_point tp_end) const override;

  virtual std::string PrintToString() const override {return PrintToStringTS("TS_const");}
  //virtual std::tuple<std::string, std::string, std::string> toXmlString() const override {return toXmlString("TS_const");};

 protected:

  /**
  * @brief Default constructor
  */
  TimeSeriesConst() = default;
};

} /* namespace aux */

#endif /* AUXILIARIES_TIME_SERIES_CONST_H_ */

