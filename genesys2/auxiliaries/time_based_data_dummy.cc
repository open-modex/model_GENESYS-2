/*
 * time_based_data_dummy.cc
 *
 *  Created on: Dec 1, 2015
 *      Author: pst
 */

#include <auxiliaries/time_based_data_dummy.h>

#include <exception>
#include <iostream>

namespace aux {

double TimeBasedDataDummy::operator [](const aux::SimulationClock::time_point&) const {
  std::cerr << "Error in aux::TimeBasedDataDummy::operator[] :" << std::endl
      << "Trying to evaluate dummy object." << std::endl;
  std::terminate();
  return 0.0;
}

} /* namespace aux */
