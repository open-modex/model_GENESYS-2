/*
 * time_series_const_addable.cc
 *
 *  Created on: Jul 15, 2016
 *      Author: pst
 */

#include <auxiliaries/time_series_const_addable.h>

#include <cmath>

namespace aux{

double TimeSeriesConstAddable::operator [](const SimulationClock::time_point& time_point) const {
  if (size() > 0) {
    return (TimeSeriesConst::operator [](time_point));
  } else {
    // should empty TSCA evaluate to 0? Until we decide on this, it returns an error:
    std::cerr << "ERROR in aux::TimeSeriesConstAddable::operator [] -> cannot evaluate empty object! "  << std::endl;
    std::terminate();
    return 0.0; // dummy return
  }
}

TimeSeriesConstAddable& TimeSeriesConstAddable::operator +=(const TimeSeriesConst& rhs) {
  // check if intervals match (if this->interval is zero, this is empty dummy TCSA, adapting rhs interval
  if(rhs.interval() != interval()) {
    if (interval() == aux::SimulationClock::duration::zero()) {
      auto temp = rhs.values();
      auto temp_size = rhs.size();
      auto temp_start = rhs.start();
      swap_values(temp, temp_size, temp_start);
      update_end(rhs.end());
      update_interval(rhs.interval());
      return *this;
    } else {
      std::cerr << "ERROR in aux::TimeSeriesConstAddable::operator += -> intervals don't match! "  << std::endl;
      std::terminate();
    }
  }
  // start_ difference:
  auto offset = static_cast<int>(std::ceil(
      std::chrono::duration_cast<std::chrono::duration< double,
      interval_type::period> >(rhs.start() - start()) / interval()));
  // end_ difference:
  auto trailing_offset = static_cast<int>(std::ceil(
      std::chrono::duration_cast<std::chrono::duration< double,
      interval_type::period> >(rhs.end() - end()) / interval()));
  if (trailing_offset > 0) update_end(rhs.end()); // if necessary, start gets updated below via swap_values
  // store values_ of TS with earlier start date in this->values_
  auto temp = rhs.values();
  auto temp_size = rhs.size();
  if (offset > 0) { // this->values_ begins earlier, no additional action
  } else { // swap data between *this and rhs
    swap_values(temp, temp_size, rhs.start());
    offset = -offset;
    trailing_offset = -trailing_offset;
  }
  // compute overlap distance
  auto signed_size = static_cast<int>(size());
  auto overlap = (offset < signed_size) ? std::min(signed_size - offset, static_cast<int>(temp_size)) :
      signed_size - offset;
  auto i = std::min(overlap, 0);
  auto uoverlap = static_cast<index_type>(std::max(overlap, 0));
  auto old_end = value(size() - 1);
  for (; i < 0; ++i) { // fill gap between timeseries
    append(old_end);
  }
  auto ui = static_cast<index_type>(i);
  for (; ui < uoverlap; ++ui) { // add values in overlapping part
    add_assign_value(temp[ui], ui + offset);
  }
  if (trailing_offset > 0) { // append remaining part if existent
    auto new_end_index = static_cast<index_type>(overlap + trailing_offset);
    for (; ui < new_end_index; ++ui) {
      append(temp[ui] + old_end);
    }
  }
  return *this;
}

}//end namespace aux
