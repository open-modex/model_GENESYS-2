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
// primary_energy.cc
//
// This file is part of the genesys-framework v.2
#include <dynamic_model_hsm/primary_energy.h>
#include <program_settings.h>
#include <auxiliaries/functions.h>

#include <iomanip>

namespace dm_hsm {
void PrimaryEnergy::resetCurrentTP(const aux::SimulationClock& clock) {
	if (annual_potential_ > genesys::ProgramSettings::approx_epsilon()) {
		potential_current_tp_ = annual_potential_;
	} else {
		potential_current_tp_ = 0.;
	}
  //  if(code() == "CO2") {
  //        std::cout << aux::SimulationClock::time_point_to_string(clock.now()) << "\tco2 found \ttResetTP:" << code() << " | current potential:\t" << potential_current_tp_ << std::endl;
  //  }
}

void PrimaryEnergy::set_annual_lookups(const aux::SimulationClock& clock){
	annual_potential_ = potential(clock.now());
    if (annual_potential_ == -1.) {
    	annual_potential_ = std::numeric_limits<double>::infinity();
    }

    annual_vopex_ = cost(clock.now());

  //  if(code() == "CO2") {
  //    std::cout << aux::SimulationClock::time_point_to_string(tp_now) << "\tco2 found \set_annual_potential:" << code() << " | current potential:\t" << potential_current_tp_ << std::endl;
  //  }
}

void PrimaryEnergy::resetSequencedPrimaryEnergy(const aux::SimulationClock& clck) {
  //std::cout << "PrimaryEnergy::resetSequencedPrimaryEnergy " << code()<< " : " << aux::SimulationClock::time_point_to_string(clck.now())<< std::endl;
  annual_potential_ = 0.;
  annual_vopex_ = 0.;
  potential_current_tp_ = 0.;
  reserved_capacity_current_tp_ = 0.;
  consumed_energy_ = aux::TimeSeriesConstAddable();
  internal_vopex_ = aux::TimeSeriesConstAddable();
  total_consumed_ = 0.;
}

bool PrimaryEnergy::reserveCapacity(const double request,
                                    const aux::SimulationClock& clock) {
  if (potential_current_tp_ > (request - genesys::ProgramSettings::approx_epsilon())) {
    potential_current_tp_ -= request;
    reserved_capacity_current_tp_ = request;
    //std::cout << "END FUNC success PrimaryEnergy::reserveCapacity" << std::endl;
    return true;
  } else{
    std::cerr << "ERROR in PrimaryEnergy::reserveCapacity of " << code()
                  << " insufficient PrimaryEnergy available!" << std::endl;
   std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
   std::terminate();//failue due to preceeding request ( get_potential_current_tp() )

  }
  //std::cout << "END FUNC failure PrimaryEnergy::reserveCapacity" << std::endl;
  return false;
}

bool PrimaryEnergy::useCapacity(const double request,
                                const aux::SimulationClock& clock) {
  //DEBUG std::cout << "FUNC-ID: PrimaryEnergy::useCapacity for " << code() << " | request = " << request << std::endl;
  if (reserved_capacity_current_tp_ >= (request - genesys::ProgramSettings::approx_epsilon() )) {
    add_internal_counters(request, clock);
    reserved_capacity_current_tp_ = 0.;
    // std::cout << "END FUNC success PrimaryEnergy::useCapacity" << std::endl;
    return true;
  } else {
    std::cerr << "ERROR in PrimaryEnergy::useCapacity" << std::endl;
    std::cout << "reserved_capacity_current_tp_= " << reserved_capacity_current_tp_ << std::endl;
    std::cout << "request                      = " << request << " GWh_th | t " <<  std::endl;
    std::cerr << "ERROR in PrimaryEnergy::useCapacity of " << code()
              << " failed due to insufficient reserved_capacity!" << std::endl;
    std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
    std::terminate();
  }
  //std::cout << "END FUNC failure PrimaryEnergy::useCapacity" << std::endl;
  return false;
}

void PrimaryEnergy::add_internal_counters (double request,const aux::SimulationClock& clock) {
  //update annual_potential_, internal_vopex_ ,  consumed_energy_ , total_consumed_
  double vopex = request*cost(clock.now(), request);
  //std::cout << "\t\t vopex: " << vopex << " | for " << code() << std::endl;
  if (vopex > 1) { //check if cost are defined for respective timepoint
      internal_vopex_ += aux::TimeSeriesConst(std::vector<double>{vopex,0.}, clock.now(),  clock.tick_length());
      //consumed_energy_ += aux::TimeSeriesConst(request_vec, clock.now(), clock.tick_length());
    } else {
      internal_vopex_ += aux::TimeSeriesConst(std::vector<double>{0.,0.}, clock.now(),  clock.tick_length());
      //consumed_energy_ += aux::TimeSeriesConst(std::vector<double>{0.,0.}, clock.now(), clock.tick_length());
    }
  annual_potential_ -= request;
  consumed_energy_ += aux::TimeSeriesConst(std::vector<double>{request, 0.}, clock.now(), clock.tick_length());
  total_consumed_ += request;
}

void PrimaryEnergy::print_current_capacities(const aux::SimulationClock& clock) {
  std::cout << "\t\t\t | PrimaryEnergy: " << code() << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< potential_current_tp_ << "\t potential_current_tp_" << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< reserved_capacity_current_tp_   << "\t reserved_capacity_current_tp_" << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< potential(clock.now()) << "\t potential[tp]" << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< cost(clock.now()) << "\t cost[tp]" << std::endl;
  std::cout << "\t\t\t | \t--------------------------------" << std::endl;
}

} /* namespace dm_hsm */
