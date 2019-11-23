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

#include <dynamic_model_hsm/sys_component_active.h>
#include <auxiliaries/functions.h>
#include <program_settings.h>

#include <chrono>
#include <iomanip>

namespace dm_hsm {

double SysComponentActive::usable_capacity_out_tp(const aux::SimulationClock& clock, double reqInput) const {
  if (capacity(clock.now()) > 0) {
    return std::min(reqInput*efficiency(clock.now()),usable_capacity_tp());
  }
  return 0.0;
}

double SysComponentActive::usable_capacity_el(const aux::SimulationClock& clock) const {
  if (capacity(clock.now()) > genesys::ProgramSettings::approx_epsilon()) {
    return usable_capacity_el_tp_;
  }
  return 0.; //else
}

double SysComponentActive::usable_capacity_in_tp(const aux::SimulationClock& clock, double requestedOutput) const {
  if (capacity(clock.now()) > genesys::ProgramSettings::approx_epsilon()) {
    //CBU forstorage not correct: return (std::min(requestedOutput/efficiency(clock.now()), usable_capacity_in_tp(clock))) ;
    return (std::min(requestedOutput, usable_capacity_el(clock))) ;
  }
  return 0.;
}

void SysComponentActive::resetCurrentTP(const aux::SimulationClock& clock) {
  //DEBUG  std::cout << aux::SimulationClock::time_point_to_string(clock.now()) <<"\t converter/componentActive = " << std::setw(20)<< code() << " | capacity= " << capacity(clock.now()) << std::endl;
  set_usable_capacity_tp(capacity(clock.now()));
  set_reserve_capacity_tp(clock,0.);
}

void SysComponentActive::set_annaul_lookups(const aux::SimulationClock& clock) {
	set_mean_efficiency(clock);
}

bool SysComponentActive::useCapacity(double capacity_request,
                                     const aux::SimulationClock& clock,
                                     const bool charging) {
	//std::cout << "SysComponentActive::useCapacity capacity_request " << capacity_request << std::endl;
  if ((0.0 < capacity_request) && (capacity_request <= reserved_capacity_tp() +genesys::ProgramSettings::approx_epsilon())) {
    if (charging) {
      add_used_capacity(clock, -capacity_request);
    } else {
      add_used_capacity(clock, capacity_request);
    }
    set_usable_capacity_tp(usable_capacity_el_tp_ - capacity_request);
    set_reserve_capacity_tp(clock,0.0);
    set_active_current_year(true);
    return true;
  }
  return false;
}

void SysComponentActive::resetSequencedSysComponent() {
  //reset all information that is stored on high resolution from operation simulation.
  used_capacity_ = aux::TimeSeriesConstAddable();
}


void SysComponentActive::add_OaM_cost(aux::SimulationClock::time_point tp_now) {
	//std::cout << "SysComponentActive::add_OaM_cost for converter " << code() << " with ";
	double fopex = capex(tp_now) * OaM_rate(tp_now);
	//	std::cout << "\t\t fopex" << code() << " fuer: " << aux::SimulationClock::time_point_to_string(tp_now)  <<" : " << fopex<< std::endl;
	auto year = aux::SimulationClock::year(tp_now);
	auto new_tp = aux::SimulationClock::time_point_from_string(std::to_string(year)+"-01-01_00:00");
	//std::cout << aux::SimulationClock::time_point_to_string(new_tp) << std::endl;
	fopex_ += aux::TimeSeriesConst(std::vector<double>{fopex, 0.}, new_tp, aux::years(1));
 	//std::cout << "\tfopex" << code() << " : "<< fopex_.PrintToString() << std::endl;
}

void SysComponentActive::add_vopex(double value,
                                   const aux::SimulationClock& clock) {
    //  std::cout << "FUNC-ID: dm_hsm::SysComponentActive::add_vopex() for " << code() << " with value= " << value << std::endl;
    if (value < 1.) {
      //std::cout << "\t*\there with : " << value << std::endl;
      add_vopex_zero(clock.now(), clock.tick_length());
    } else {
      if (value != value){//value = nan
        std::cout << "ERROR in SysComponentActive::add_vopex:: value indefined! NAN =" << value << std::endl;
        std::cerr << "EXIT" << std::endl;
        std::terminate();
      }
      vopex_ += aux::TimeSeriesConst(std::vector<double>{value, 0.}, clock.now(), clock.tick_length());
      //std::cout << "VOPEX = " << value << " \t" << aux::SimulationClock::time_point_to_string(tp_now) << std::endl;
    }
    //std::cout << "END FUNC SysComponentActive::add_vopex for "<< code() << std::endl;
  }

void SysComponentActive::print_current_capacities(const aux::SimulationClock& clock) {
  std::cout << "\t\t\t | SysComponentActive: " << code() << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< reserved_capacity_tp() << "\t reserved_capacity_tp_" << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< usable_capacity_out_tp()   << "\t usable_capacity_out_tp" << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< capacity(clock.now()) << "\t installed_capacity_cacitve[tp]" << std::endl;
  std::cout << "\t\t\t | \t--------------------------------" << std::endl;
}


double SysComponentActive::get_sum(std::string query,
                                       aux::SimulationClock::time_point start,
                                       aux::SimulationClock::time_point end,
                                       aux::SimulationClock::duration interval) const {
  //std::cout << "FUNC-ID: SysComponentActive::get_sum" << std::endl;
  double sum_value = 0.;
  if(query == "CAPEX") {
    sum_value += aux::sum(capex(start, end),
                                start,
                                end,
                                aux::years(1));
    //std::cout << "sum CAPEX = " << sum_value << std::endl;
  } else if (query == "FOPEX") {
    if (!fopex_.empty()) {
      sum_value += aux::sum(fopex_,
                            start,
                            end,
                            aux::years(1));
      //std::cout << "sum FOPEX = " << sum_value << std::endl;
    }
  } else if (query == "ENERGY") {
      //std::cerr << "SysComponentActive::getDiscountedValue - not implemented for ENERGY query > " << query << std::endl;
  } else if (query == "VOPEX") {
    if (!vopex_.empty()) {
      sum_value += 8760*aux::sum_means(vopex_,
                                       start,
                                       end,
                                       aux::years(1));
      //std::cout << "sum VOPEX = " << sum_value << std::endl;
    }
  } else if (query == "generation") {
    if(! used_capacity_.empty()) {
      sum_value += aux::sum(used_capacity_,
                                           start,
                                           end,
                                           interval);

      //comment: before was aux::sum_positiveValues() - but this is not respecting the power going into  storage!
      //std::cout << "\t\tsum generation = " << sum_value  << " GWh"<< std::endl;
    }
  } else {
       std::cerr << "ERROR in dm_hsm::SysComponentActive::get_sum() : query " << query
                  <<" could not be identified!" << std::endl;
       std::terminate();
    }

  return sum_value;

}

double SysComponentActive::getDiscountedValue(std::string query,
                                              aux::SimulationClock::time_point start,
                                              aux::SimulationClock::time_point end,
                                              const aux::SimulationClock::time_point present) const {
  //std::cout << "FUNC-ID: SysComponentActive::getDiscountedValue for query " << query << " in " << code() << std::endl;
  double discounted_value = 0.0;
  if(query == "CAPEX") {
    discounted_value += aux::DiscountFuture2(capex(start, end),
                                             start,
                                             end,
                                             aux::years(1),
                                             internal_rate_of_return(start),
                                             present);
  } else if (query == "FOPEX") {
    discounted_value += aux::DiscountFuture2(fopex_,
                                             start,
                                             end,
                                             aux::years(1),
                                             internal_rate_of_return(start),
                                             present);
  } else if (query == "ENERGY") {
    //std::cerr << "SysComponentActive::getDiscountedValue - not implemented for ENERGY query > " << query << std::endl;
  } else if (query == "VOPEX") {
     //   std::cout << "\t VOPEX \t Component=  " << code() <<  std::endl;
    std::vector<double> annual_vopex_vec;
    aux::SimulationClock printClock(start, aux::years(1));
    do {
      //Mean per hour of the year from total sum of TBD
      double current_val = 8760*vopex_.Mean(printClock.now(), printClock.now()+aux::years(1));
      if (current_val >= 1) {
        annual_vopex_vec.push_back(current_val);
      } else {
        //std::cout << "value was : " << current_val << std::endl;
        annual_vopex_vec.push_back(0.);
      }
    } while (printClock.tick() < end);
    ;

    discounted_value += aux::DiscountFuture2(aux::TimeSeriesConst(annual_vopex_vec,
                                                                 start,
                                                                 aux::years(1)),
                                                                 start,
                                                                 end,
                                                                 aux::years(1),
                                                                 genesys::ProgramSettings::interest_rate(),
                                                                 present);
   } else {
     std::cerr << "ERROR in dm_hsm::SysComponentActive::getDiscountedValue() : query " << query
                <<" could not be identified!" << std::endl;
     std::terminate();
   }
  //std::cout << "END FUNC SysComponentActive::getDiscountedValue " << std::endl;
  return (discounted_value);

}

void SysComponentActive::calculated_annual_disc_capex(aux::SimulationClock::time_point start) {
  //std::cout << "FUNC-ID SysComponentActive::calculated_annual_disc_capex for " << code() << std::endl;
  double discounted_value = 0.;
  discounted_value += aux::DiscountFuture2(capex(start, start+aux::years(1)),
                                               start,
                                               start+aux::years(1),
                                               aux::years(1),
                                               internal_rate_of_return(start),
                                               genesys::ProgramSettings::simulation_start());
  discounted_capex_ += aux::TimeSeriesConst(std::vector<double>{discounted_value, 0.}, start, aux::years(1));
}

bool SysComponentActive::set_reserve_capacity_tp(const aux::SimulationClock& clock, double capacity_to_reserve) {
  if (capacity_to_reserve < genesys::ProgramSettings::approx_epsilon()  ) {
	// std::cout << "SysComponentActive::set_reserve_capacity_tp : trying to reserve small cap for " << code() << std::endl;
    reserved_capacity_el_tp_ = 0.;
    return false;//case reset to zero!
  } else if ( capacity_to_reserve > capacity(clock.now())+ genesys::ProgramSettings::approx_epsilon()) {
    std::cerr << "ERROR in SysComponentActive::set_reserve_capacity_tp " << code() << " - reserving "<< capacity_to_reserve << " >100% capacity!"<< capacity(clock.now()) << std::endl;
    std::terminate();
  } else {
    reserved_capacity_el_tp_ += capacity_to_reserve;
  }
  return true;
}

//void SysComponentActive::set_reserved_cap_input(const aux::SimulationClock::time_point tp,
//                                                double input_rated) {
//  if (input_rated < genesys::ProgramSettings::approx_epsilon()  ) {
//    reserved_capacity_el_tp_ = 0.;
//    return;
//  } else if (input_rated > capacity(tp)+ genesys::ProgramSettings::approx_epsilon()){
//    //std::cerr << "ERROR in SysComponentActive::set_reserved_capacity_tp - reserving >100% capacity!" << std::endl;
//    std::cerr << "ERROR in SysComponentActive::set_reserved_cap_input " << code() << " - input= "<< input_rated  << " | reserving = " << input_rated << " >100% capacity!"<< capacity(tp) << std::endl;
//    std::terminate();
//  }
//  reserved_capacity_el_tp_ = input_rated;
//}

double SysComponentActive::usable_capacity_tp() const {
  if (usable_capacity_el_tp_ < genesys::ProgramSettings::approx_epsilon()) {
    //DEBUG    std::cout << "SysComponentActive::usable_capacity_tp - small usable_capacity = " << usable_capacity_tp_ << std::endl;
    return 0.;
  }
  return usable_capacity_el_tp_;
}

} /* namespace dm_hsm */
