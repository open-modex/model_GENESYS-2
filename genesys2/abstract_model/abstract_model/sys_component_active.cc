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
// sys_component_active.cc
//
// This file is part of the genesys-framework v.2

#include <abstract_model/sys_component_active.h>
#include <auxiliaries/functions.h>
#include <program_settings.h>

#include <exception>
#include <iostream>
#include <utility>

namespace am {

double SysComponentActive::efficiency(aux::SimulationClock::time_point tp) const {
  //std::cout << "FUNC-ID: SysComponentActive::efficiency called for" << aux::SimulationClock::time_point_to_string(tp)<<  code()<< std::endl;
  if (capacity(tp) > genesys::ProgramSettings::approx_epsilon()) {//efficiency only defined for tp with capacity > 0
    if (mean_efficiency_ > genesys::ProgramSettings::approx_epsilon()) {
      return (mean_efficiency_);
    } else {
      std::cerr << aux::SimulationClock::time_point_to_string(tp) << " - mean Efficiency was too small: " << mean_efficiency_ << std::endl;
      std::cerr << "ERROR in SysComponentActive::efficiency is zero ur nan : "
                << efficiency_[tp] << " in "<<  code()<< std::endl;
      std::terminate();
    }
  } return -1;
}

//double SysComponentActive::efficiency(aux::SimulationClock::time_point tp, double query) const {
//  //std::cout << "FUNC-ID: SysComponentActive::efficiency called for" << aux::SimulationClock::time_point_to_string(tp)<<  code()<< std::endl;
//  if (capacity(tp) >= query) {
//	 return efficiency_.rlookup(tp, query);
//  } else if (capacity(tp) > genesys::ProgramSettings::approx_epsilon()) {
//	 if (efficiency_[tp] > genesys::ProgramSettings::approx_epsilon()) {
//		return (efficiency_[tp]);
//	 } else {
//	    std::cerr << "ERROR in SysComponentActive::efficiency is zero ur nan : "
//	              << efficiency_[tp] << " in "<<  code()<< std::endl;
//	    std::terminate();
//    }
//  }
//  return -1;
//}

void SysComponentActive::MapInstallation(std::unique_ptr<aux::TimeBasedData> installation,
                                         aux::SimulationClock::time_point start_installation,
                                         aux::SimulationClock::time_point end_installation,
                                         aux::SimulationClock::duration interval_installation) {
  //std::cout << "SysComponentActive::MapInstallation for " << code() << std::endl;
  std::vector<double> capacity_vec;
  std::vector<double> capex_vec;
  do {
    // add new unit from installation to capacity and capex
    auto lifetime_int = static_cast<int>(lifetime(start_installation));
    unsigned int lifetime_uint;
    if (lifetime_int > 0) {
      lifetime_uint = static_cast<unsigned int>(lifetime_int);
    } else {
      std::cerr << "ERROR in am::SysComponentActive::MapInstalltion :" << std::endl << "negative lifetime or zero for "
          << code() << " at time " << aux::SimulationClock::time_point_to_string(start_installation) << std::endl;
      std::terminate();
    }
    //fill vector with lifetime-repetition of the installtion capacity
    capacity_vec = std::vector<double>(lifetime_uint, (*installation)[start_installation]);
    //add zero to close TimeSeriesConst
    capacity_vec.push_back(0.0);
    //fill in TSC capacity
    capacity_ += aux::TimeSeriesConst(capacity_vec, start_installation, aux::years(1));
    //calculate times value of investment cost with respect to present day/simulation_start()
      //todo alternative would be a inflation interest rate separate from investment interest rate
    double invest_present_value = aux::timed_value_annual(cost(start_installation),
														  genesys::ProgramSettings::simulation_start(),
														  start_installation,
														  genesys::ProgramSettings::interest_rate());

    //fill vector with lifetime-repetition of the installtion cost
	      //version without expansion of cost to installation date (virtual credit at simulation start)
	      //capex_vec = std::vector<double>(lifetime_uint, cost(start_installation)
        //                                * aux::anf(internal_rate_of_return(start_installation), lifetime_int));
	      //version with cost expation to present value
	      capex_vec = std::vector<double>(lifetime_uint, invest_present_value
                                                * aux::anf(internal_rate_of_return(start_installation), lifetime_int));


    //add zero to close TimeSeriesConst
    capex_vec.push_back(0.0);
    auto capacity_it = capacity_vec.cbegin();
    for (auto&& i : capex_vec)
      i *= *(capacity_it++);
    //fill in TSC capex
    capex_ += aux::TimeSeriesConst(capex_vec, start_installation, aux::years(1));
    // complement efficiency TBDLookupTable with newly installed unit
    std::vector<std::pair<aux::SimulationClock::time_point, double> > base_new_cap;
    base_new_cap.emplace_back(start_installation, (*installation)[start_installation]);
    base_new_cap.emplace_back(start_installation + lifetime_uint * interval_installation, 0.0);
    std::unique_ptr<aux::TimeBasedData> base(new aux::DateValuePairsConst(base_new_cap));
    std::vector<std::pair<aux::SimulationClock::time_point, double> > value_new_eff {
      std::make_pair(start_installation, efficiency_new(start_installation))};
    std::unique_ptr<aux::TimeBasedData> value(new aux::DateValuePairsConst(value_new_eff));
    efficiency_.complement(std::move(base), std::move(value));
  } while ((start_installation += interval_installation) < end_installation);
}

aux::TimeSeriesConst SysComponentActive::capex(aux::SimulationClock::time_point tp_start,
                                               aux::SimulationClock::time_point tp_end) const {
    aux::SimulationClock sub_clock(tp_start, aux::years(1));
    std::vector<double> capex_vec;
    do {
      capex_vec.push_back(capex(sub_clock.now()));
    } while (sub_clock.tick() < tp_end);
    capex_vec.push_back(0.0);
    return aux::TimeSeriesConst(capex_vec, tp_start, aux::years(1));
}

} /* namespace am */
