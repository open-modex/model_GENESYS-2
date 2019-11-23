/*
 * date_value_pairs.h
 *
 *  Created on: Aug 6, 2015
 *      Author: pst
 */

#ifndef AUXILIARIES_DATE_VALUE_PAIRS_H_
#define AUXILIARIES_DATE_VALUE_PAIRS_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_based_data.h>

namespace aux {

class DateValuePairs : public TimeBasedData {
 public:

  /**
  * @brief Default constructor
  */
  DateValuePairs() = delete;
  /**
   * @brief Default destructor
   */
  virtual ~DateValuePairs() override = default;

  virtual SimulationClock::time_point start() const override final {return time(0);}

  /**
   * @brief Prints the data, saved as pair of date and value, in a string
   *
   * @return output String containing the time based data
   */
  virtual std::string PrintToString() const override {
    return "dummy return - this is plain DVP, should not be instantiated";
  }
  virtual bool empty() const override{return ((data_.size() > 0) ? false : true);}

 protected:
  std::string PrintToStringDVP(std::string type) const; // deliberately passing by value

  typedef std::vector<std::pair<SimulationClock::time_point, double> > data_structure;
  typedef data_structure::size_type index_type;

  /**
   * @brief Constructor
   *
   * @param[in] time_point Time point defining the position, at which to save the data
   * @param[in] data Variable containing the information, saved as pair of date and value
   */
  DateValuePairs(const std::vector<std::pair<SimulationClock::time_point,
                 double> >& data);

  /**
   * @brief Copies and stores the time based data in a new unique pointer
   *
   * @return Unique pointer with the copied time based data, saved as pair of date and value
   */
  virtual std::unique_ptr<TimeBasedData> clone() const;

  index_type locate_time(const SimulationClock::time_point& time_point,
                         double& fraction) const;

  /**
   * @brief Calculates the local time relative to the start point of the time based data
   *
   * @param[in] time_point Absolute time point
   */
  index_type locate_time(const SimulationClock::time_point& time_point) const;

  /**
   * @brief Gets the data of a specific pair, consisting of data and date
   *
   * @param[in] index Index of the searched pair, consisting of data and date
   * @return Data of the specific pair
   */
  double data(index_type index) const {return (data_[index].second);}

  const data_structure& data() const {return data_;}

  /**
   * @brief Gets the time of a specific pair, consisting of data and date
   *
   * @param[in] index Index of the searched pair, consisting of data and date
   * @return Time of the specific pair
   */
  SimulationClock::time_point time(index_type index) const {return (data_[index].first);}

  /**
   * @brief Gets number of pairs
   *
   * @return Number of pairs
   */
  index_type pair_count() const {return (pair_count_);}

 private:
  data_structure data_;
  index_type pair_count_;
};

} /* namespace aux */

#endif /* AUXILIARIES_DATE_VALUE_PAIRS_H_ */

