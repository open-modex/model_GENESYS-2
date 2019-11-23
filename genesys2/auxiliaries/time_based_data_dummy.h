/*
 * time_based_data_dummy.h
 *
 *  Created on: Dec 1, 2015
 *      Author: pst
 */

#ifndef AUXILIARIES_TIME_BASED_DATA_DUMMY_H_
#define AUXILIARIES_TIME_BASED_DATA_DUMMY_H_

#include <memory>
#include <string>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_based_data.h>

namespace aux {

class TimeBasedDataDummy : public TimeBasedData {
 public:

  /**
  * @brief Default constructor
  */
  TimeBasedDataDummy() = default;

  /**
   * @brief Default deconstructor
   */
  virtual ~TimeBasedDataDummy() = default;

  virtual double operator [](const SimulationClock::time_point& time_point) const override;

  virtual std::unique_ptr<TimeBasedData> clone() const override {
    return std::unique_ptr<TimeBasedData>(new TimeBasedDataDummy(*this));
  }

  /**
   * @brief Overridden virtual function
   */
  virtual std::string PrintToString() const override {return "got me - I'm a TimeBasedDataDummy!";}
};

} /* namespace aux */

#endif /* AUXILIARIES_TIME_BASED_DATA_DUMMY_H_ */

