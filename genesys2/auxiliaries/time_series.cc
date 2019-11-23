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
// time_series.cc
//
// This file is part of the genesys-framework v.2

#include <auxiliaries/time_series.h>
#include <cmath>

namespace aux {

std::string TimeSeries::PrintToStringTS(std::string type) const {
  std::string rval("#type;" + type + ";#interval;");
  rval += aux::SimulationClock::duration_to_string(interval()) + ";#start;";
  rval += aux::SimulationClock::time_point_to_string(start()) + ";#data;";
  for (const auto& i : values_)
    rval += std::to_string(i) + ";";
  return rval;
}

std::tuple<std::string, std::string, std::string> TimeSeries::toXml() const {
  auto my_start = aux::SimulationClock::time_point_to_string(start());
  auto my_interval = aux::SimulationClock::duration_to_string(interval());
  std::string data;
  for (const auto& i : values_) {
    data += std::to_string(i) + ",";
  }
  auto t = std::make_tuple(my_start, my_interval, data);
  return t;
}

std::unique_ptr<TimeBasedData> aux::TimeSeries::clone() const {
  std::cerr << "Error in aux::TimeSeries::clone :" << std::endl
      << "Call to this dummy member function is undefined." << std::endl;
  std::terminate();
  return std::unique_ptr<TimeBasedData>(new TimeSeries(*this));
}

TimeSeries::index_type TimeSeries::locate_time(const SimulationClock::time_point& time_point,
                                               double& fraction) const {
  auto position = std::chrono::duration_cast<std::chrono::duration<
      double, interval_type::period> >(time_point - start_).count();
  double index_temp;
  fraction = std::modf(position / interval_.count(), &index_temp);
  auto index = static_cast<index_type>(index_temp);
  return (index);
}

TimeSeries::index_type TimeSeries::locate_time(const SimulationClock::time_point& time_point) const {
  auto position = std::chrono::duration_cast<interval_type>(time_point - start_).count(); //returns number of minutes between start and time_point
  return (static_cast<index_type>(position / interval_.count()));  //returns the vector index of time_point
}

void TimeSeries::swap_values(data_structure& other_values,
                             index_type& other_size,
                             SimulationClock::time_point other_start) {
  values_.swap(other_values);
  auto temp = size_;
  size_ = other_size;
  other_size = temp;
  start_ = other_start;
}

void TimeSeries::append(const double value) {
  values_.push_back(value);
  ++size_;
}

void TimeSeries::update_interval(SimulationClock::duration new_interval) {
  if (interval() == SimulationClock::duration::zero()) {
    interval_ = new_interval;
  } else {
    std::cerr << "ERROR in TimeSeries::update_interval : update of non-zero interval would be undefined behavior "  << std::endl;
    std::terminate();
  }
}

}/* namespace aux */
