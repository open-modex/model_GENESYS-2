/*
 * tbd_lookup_table.h
 *
 *  Created on: Jul 22, 2016
 *      Author: pst
 */

#ifndef AUXILIARIES_TBD_LOOKUP_TABLE_H_
#define AUXILIARIES_TBD_LOOKUP_TABLE_H_

#include <memory>
#include <utility>
#include <vector>

#include <auxiliaries/simulation_clock.h>
#include <auxiliaries/time_based_data.h>

namespace aux {

class TBDLookupTable {
 public:
  TBDLookupTable() = default;
  ~TBDLookupTable() = default;
  TBDLookupTable(const TBDLookupTable& other);
  TBDLookupTable(TBDLookupTable&&) = default;
  TBDLookupTable& operator=(const TBDLookupTable& other);
  TBDLookupTable& operator=(TBDLookupTable&& other);

  double operator [](const SimulationClock::time_point& time_point) const;

  double rlookup(const SimulationClock::time_point& time_point,
				double query,
				double offset = 0.) const;

  double lookup(const SimulationClock::time_point& time_point,
                double query,
                double offset = 0.0) const;

  void complement(std::unique_ptr<TimeBasedData> base,
              std::unique_ptr<TimeBasedData> value) {data_.emplace_back(std::move(base), std::move(value));}

 private:
  std::vector<std::pair<std::unique_ptr<TimeBasedData>, std::unique_ptr<TimeBasedData> > > data_;
};

} /* namespace aux */

#endif /* AUXILIARIES_TBD_LOOKUP_TABLE_H_ */
