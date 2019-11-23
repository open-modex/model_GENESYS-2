/*
 * tbd_lookup_table.cc
 *
 *  Created on: Jul 22, 2016
 *      Author: pst
 */

#include <auxiliaries/tbd_lookup_table.h>
#include <program_settings.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>

namespace aux {

TBDLookupTable::TBDLookupTable(const TBDLookupTable& other) {
  if (!other.data_.empty()) {
    for (const auto& i : other.data_) {
      data_.emplace_back(std::move((*(i.first)).clone()), std::move((*(i.second)).clone()));
    }
  }
}

TBDLookupTable& TBDLookupTable::operator=(const TBDLookupTable& other) {
  data_.clear();
  if (!other.data_.empty()) {
    for (const auto& i : other.data_) {
      data_.emplace_back(std::move((*(i.first)).clone()), std::move((*(i.second)).clone()));
    }
  }
  return *this;
}

TBDLookupTable& TBDLookupTable::operator=(TBDLookupTable&& other) {
  data_ = std::move(other.data_);
  return *this;
}

double TBDLookupTable::operator [](const SimulationClock::time_point& time_point) const {
  double rval = 0.0;
  double sum_of_prev_bases = 0.0;
  for (const auto& i : data_) {
    double current_base = (*i.first)[time_point];
    if (current_base != 0.0) {
      sum_of_prev_bases += current_base;
      rval += (current_base / sum_of_prev_bases) * ((*i.second)[time_point] - rval);
    }
  }
  return rval;
}
//for use with efficiencies
double TBDLookupTable::rlookup(const SimulationClock::time_point& time_point,
                              double query,
                              double offset) const {
  if (query < -genesys::ProgramSettings::approx_epsilon()) {
    std::cerr << "ERROR in TBDLookupTable::rlookup: query < 0.0 not allowed - query = "<< query  << std::endl;
    std::terminate();
  }
  // iterate over offset
  double current_base;
  auto it = data_.rbegin();
  for (; it < data_.rend(); ++it) {
    if ((current_base = (*it->first)[time_point]) <= offset) {
      offset -= current_base;
    } else {
      break;
    }
  }
  // zero query is a special case
  if (query == 0.0) {
    if (it != data_.rend())
      return (*it->second)[time_point]; // offset is within current element
    return 0.0; // this->data_ is empty or offset past last element
  }
  double rval(0.0);
  double sum_of_prev_bases(0.0);
  // the element that iterator currently points to is (first) part of query
  auto query_countdown = query + offset;
  double current_base_active;
  for (; it < data_.rend(); ++it) {
    if ((current_base = (*it->first)[time_point]) < query_countdown) {
      // current element inside query
      query_countdown -= current_base;
      current_base_active = current_base - offset;
      sum_of_prev_bases += current_base_active;
      rval += (current_base_active / sum_of_prev_bases) * ((*it->second)[time_point] - rval);
    } else {
      // query finished within element that iterator currently points to
      current_base_active = std::min(current_base - offset, std::min(query_countdown, query));
      rval += (current_base_active / (sum_of_prev_bases + current_base_active)) * ((*it->second)[time_point] - rval);
      break;
    }
    offset = 0.0;
  }
  // iterator at last element and offset/query go past last element
  return rval;
}
//for capacities
double TBDLookupTable::lookup(const SimulationClock::time_point& time_point,
                              double query,
                              double offset) const {
  if (query < -genesys::ProgramSettings::approx_epsilon()) {
      std::cerr << "ERROR in TBDLookupTable::lookup: query < 0.0 not allowed - query = "<< query  << std::endl;
	    std::terminate();
	  }
	  // iterate over offset
	  double current_base;
	  auto it = data_.cbegin();
	  for (; it < data_.cend(); ++it) {
	    if ((current_base = (*it->first)[time_point]) <= offset) {
	      offset -= current_base;
	    } else {
	      break;
	    }
	  }
	  // zero query is a special case
	  if (query == 0.0) {
	    if (it != data_.cend())
	      return (*it->second)[time_point]; // offset is within current element
	    return 0.0; // this->data_ is empty or offset past last element
	  }
	  double rval(0.0);
	  double sum_of_prev_bases(0.0);
	  // the element that iterator currently points to is (first) part of query
	  auto query_countdown = query + offset;
	  double current_base_active;
	  for (; it < data_.cend(); ++it) {
	    if ((current_base = (*it->first)[time_point]) < query_countdown) {
	      // current element inside query
	      query_countdown -= current_base;
	      current_base_active = current_base - offset;
	      sum_of_prev_bases += current_base_active;
	      rval += (current_base_active / sum_of_prev_bases) * ((*it->second)[time_point] - rval);
	    } else {
	      // query finished within element that iterator currently points to
	      current_base_active = std::min(current_base - offset, std::min(query_countdown, query));
	      rval += (current_base_active / (sum_of_prev_bases + current_base_active)) * ((*it->second)[time_point] - rval);
	      break;
	    }
	    offset = 0.0;
	  }
	  // iterator at last element and offset/query go past last element
	  return rval;
}

} /* namespace aux */
