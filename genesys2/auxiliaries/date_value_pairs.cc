/*
 * date_value_pairs.cc
 *
 *  Created on: Nov 16, 2015
 *      Author: pst
 */

#include <auxiliaries/date_value_pairs.h>

#include <exception>
#include <iostream>

namespace aux {

std::string DateValuePairs::PrintToStringDVP(std::string type) const {
  std::string rval("#type;" + type + ";#data;");
  for (const auto& i : data_)
    rval += aux::SimulationClock::time_point_to_string(i.first) + ";" + std::to_string(i.second) + ";";
  return rval;
}

DateValuePairs::DateValuePairs(const std::vector<std::pair<SimulationClock::time_point, double> >& data)
    : data_(data),
      pair_count_(data.size()) {
  if (data_.empty()) {
    std::cerr << "Error in aux::DateValuePairs::DateValuePairs :" << std::endl
        << "Trying to create empty DateValuePairs object." << std::endl;
    std::terminate();
  }
}

std::unique_ptr<TimeBasedData> DateValuePairs::clone() const {
  std::cerr << "Error in aux::DateValuePairs::clone :" << std::endl
      << "Call to this dummy member function is undefined." << std::endl;
  std::terminate();
  return (std::unique_ptr<TimeBasedData>(new DateValuePairs(*this)));
}

DateValuePairs::index_type DateValuePairs::locate_time(const SimulationClock::time_point& time_point,
                                                       double& fraction) const {
  if (pair_count_ == static_cast<index_type>(1)) {
    return (static_cast<index_type>(0));
  } else {
    index_type i;
    for (i = 1; i < pair_count_; ++i)
      if (data_[i].first > time_point)
        break;
    --i;
    fraction = std::chrono::duration<double, SimulationClock::period>(time_point - data_[i].first).count()
        / std::chrono::duration<double, SimulationClock::period>(data_[i + 1].first - data_[i].first).count();
    return i;
  }
}

DateValuePairs::index_type DateValuePairs::locate_time(const SimulationClock::time_point& time_point) const {
  if (pair_count_ == static_cast<index_type>(1)) {
    return (static_cast<index_type>(0));
  } else {
    index_type i;
    for (i = 1; i < pair_count_; ++i)
      if (data_[i].first > time_point)
        break;
    return --i;
  }
}

} /* namespace aux */
