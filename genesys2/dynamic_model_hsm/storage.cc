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
// storage.cc
//
// This file is part of the genesys-framework v.2

#include <dynamic_model_hsm/storage.h>
#include <auxiliaries/functions.h>

#include <iomanip>

//DEBUG:
#include <thread>
#include <chrono>

namespace dm_hsm {

void Storage::ResetSequencedStorage(const aux::SimulationClock& clock) {
  resetSequencedSysComponent();
  if (stored_energy_transfer_ != (-1.0)) { //transfer SOC from prior sequence
    charged_energy_ = aux::TimeSeriesConstAddable();
    if (capacity(clock.now()) < stored_energy_transfer_){ //violation of capacity if energy is not reduced
      //correction of energy in storage: min(stored_energy_transfer_, capacity(clock.now()))
      charged_energy_ += aux::TimeSeriesConst(std::vector<double>{std::min(stored_energy_transfer_, capacity(clock.now()))},
                                              clock.now(),
                                              clock.tick_length());
      //record of the lost energy
      energy_lost_by_transfer_ += aux::TimeSeriesConst(std::vector<double>{std::abs(stored_energy_transfer_- capacity(clock.now())), 0.},
                                                       clock.now(),
                                                       clock.tick_length());
    } else {
    charged_energy_ += aux::TimeSeriesConst(std::vector<double>{stored_energy_transfer_},
                                            clock.now(),
                                            clock.tick_length());
    }
  } else {//no transferSOC from prior sequence
    charged_energy_ += aux::TimeSeriesConst(std::vector<double>{initial_SOC_* capacity(clock.now())},
                                            clock.now(),
                                            clock.tick_length());
  }
}

void Storage::ResetStorageCurrentTP(const aux::SimulationClock& clock){
	//std::cout << "FUNC-ID: Storage::ResetStorageCurrentTP" << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
    if (capacity(clock.now()) < charged_energy_[clock]){
    	//DEBUG std::cout << "FUNC-ID: Storage::ResetStorageCurrentTP violation in TP: "<< aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
    	double old_charge = charged_energy_[clock];
    	charged_energy_ += aux::TimeSeriesConst(std::vector<double>{(capacity(clock.now()) - old_charge)},
    	                                        clock.now(),
    	                                        clock.tick_length());
      //record of the lost energy
      energy_lost_by_transfer_ += aux::TimeSeriesConst(std::vector<double>{std::abs(old_charge - capacity(clock.now())),0.},
                                                         clock.now(),
                                                         clock.tick_length());
	}
  set_usable_capacity_tp(charged_energy_[clock]);
  set_reserve_capacity_tp(clock,0.);
}

void Storage::setTransferStoredEnergy(const aux::SimulationClock& clock) {
  if (std::abs(charged_energy_[clock]) < genesys::ProgramSettings::approx_epsilon()){
    stored_energy_transfer_ = 0.;
  } else {
    stored_energy_transfer_ = charged_energy_[clock];
  }
} ///<sets the persistend_energy_ for transfer between sequences


double Storage::getCapacityCharge (const double InputEnergyRequest, const aux::SimulationClock& clock) const {
  //	std::cout << "\t\t|FUNC-ID:  Storage::getCapacityCharge with chargeRequst= " << InputEnergyRequest   << " storage=" << code() << std::endl;
  if (capacity(clock.now()) > genesys::ProgramSettings::approx_epsilon()) {
    double emptyCapacityCharge = capacity(clock.now()) - charged_energy_[clock]*efficiency(clock.now());
    //std::cout << "\t\t|capacityCharge = " << emptyCapacityCharge  << " current SOC = "<< charged_energy_[clock]/capacity(clock.now()) << std::endl;
    //std::cout << "\t\t|current losses = "<< charged_energy_[clock]*efficiency(clock.now()) << std::endl;
    double acceptedCharge = std::min(InputEnergyRequest, emptyCapacityCharge);
    //std::cout << "\t\t|acceptedCharge = " << acceptedCharge  << std::endl;
    return acceptedCharge;
  }
  return 0.;//case capacity = zero
}

double Storage::getCapacityDischarge (const double OutputEnergyRequest, const aux::SimulationClock& clock ) const  {
  //std::cout << "\t\t|FUNC-ID: Storage::getCapacityDischarge with OutputEnergyRequest = " << OutputEnergyRequest << " GWh" << std::endl;
  //  std::cout << "\t\t| charged_energy = " <<  charged_energy_[clock] << " GWh" << std::endl;
  //  std::cout << "\t\t| capacity       = " <<  capacity(clock.now()) << " GWh" << std::endl;
  //  std::cout << "\t\t| efficiency     = " <<  efficiency(clock.now()) << std::endl;
  //  std::cout << "\t\t| oldSOC     = " <<  charged_energy_[clock]/capacity(clock.now()) << std::endl;
  double rval_reservableCapacityDischarge =0.;
  if (capacity(clock.now()) > genesys::ProgramSettings::approx_epsilon()) {
    double reduced_charged_energy = charged_energy_[clock]*efficiency(clock.now());
    rval_reservableCapacityDischarge = std::min(OutputEnergyRequest, reduced_charged_energy);
    return rval_reservableCapacityDischarge;
  }
  return 0.;
}

bool Storage::reserveCapacityDischarge(double OutputEnergyReserve,
                                       const aux::SimulationClock& clock) {
  //std::cout << "FUNC-ID: Storage::reserveCapacityDischarge" << std::endl;
  if (OutputEnergyReserve < -genesys::ProgramSettings::approx_epsilon())  {
    std::cerr << "ERROR in dm_hsm::Storage::reserveCapacityDischarge :" << std::endl;
    std::cerr << "Energy = " << OutputEnergyReserve << " is negative" << std::endl;
    std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
    std::terminate();
  }
  if (set_reserve_capacity_tp(clock,std::min(OutputEnergyReserve, charged_energy_[clock]))){
    return true;
  }
  //std::cout << "FUNC-ID: Storage::reserveCapacityDischarge reserved " << std::min(OutputEnergyReserve, charge)  << " GWh"<< std::endl;
  return false;
}

bool Storage::reserveCapacityCharge(const aux::SimulationClock& clock,
                                    double energyInput) {
  //  std::cout << "\t|FUNC-ID: Storage::reserveCapacityCharge with energyInput " << energyInput << " GWh" << std::endl;
  if (capacity(clock.now()) > genesys::ProgramSettings::approx_epsilon()
      && energyInput <= (capacity(clock.now()) - charged_energy_[clock]*efficiency(clock.now()))) {
    return set_reserve_capacity_tp(clock, energyInput);
  } else {
    std::cerr << "ERROR in Storage::reserveCapacityCharge --- Input exceeeds chargeAcceptance or capacity = 0 ? = " << capacity(clock.now()) << std::endl;
    std::terminate();
  }
  return false;
    //  double input_to_store_rated = energyInput * efficiency(clock.now());
    //  double chargeAcceptance = (capacity(clock.now())-charged_energy_[clock]);
    //
    //  if (chargeAcceptance - input_to_store_rated + genesys::ProgramSettings::approx_epsilon() >= 0 ) {
    //    //set_reserved_capacity_tp(clock.now(),energyInput);
    //    return set_reserve_capacity_tp(clock, input_to_store_rated);
    //    // true;
    //  } else {
    //    std::cerr << "ERROR in Storage::reserveCapacityCharge: Input exceeeds chargeAcceptance!" << std::endl;
    //  }
    //  //DEBUG std::cout << "\tStorage::reserveCapacityCharge:\n \t\t reserved_capacity_tp() " << reserved_capacity_tp() << "GWh" << std::endl;
}



bool Storage::useCapacityDischarge(const double energyOutput,
                                   const aux::SimulationClock& clock) {
  //  std::cout << "FUNC-ID: Storage::useCapacityDischarge with energyOutput= " << energyOutput << std::endl;
  //std::cout << "\t"<< code() << "\told soc = " << charged_energy_[clock]/capacity(clock.now())*100 << "%" << std::endl;
  if (energyOutput < 0.0) {
    std::cerr << "ERROR in dm_hsm::Storage::useCapacity - request with negative energy is illegal: energy = "
        << energyOutput << std::endl;
    std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
    std::terminate();
  }
    //  else if (std::abs(reserved_capacity_tp()-energyOutput) < genesys::ProgramSettings::approx_epsilon()){
    //    // std::cout << "Storage::useCapacityDischarge :small difference " <<std::endl;
    //    reserveCapacityDischarge(energyOutput, clock);
    //    set_reserved_capacity_tp(clock.now(),energyOutput);
    //  }
   else if (energyOutput > reserved_capacity_tp() + genesys::ProgramSettings::approx_epsilon() ) {
     std::cerr << "ERROR: Storage::useCapacityDischarge outputRequest >> reserved capacity" << std::endl;
     std::cout << "energy output=" << energyOutput << std::endl;
     std::cout << "reserved cap= " << reserved_capacity_tp() << std::endl;
     std::terminate();
   }
  //good conditions:
  double old_charge = charged_energy_[clock];
  //Apply Losses
    double reduced_charged_energy = charged_energy_[clock]*efficiency(clock.now());
    //std::cout << "\t\t| reduced_charged_energy     = " << reduced_charged_energy << " GWh" << std::endl;
    double losses = old_charge - reduced_charged_energy;
    //std::cout << "\t\t| losses     = " << losses << " GWh" << std::endl;
  //Calculate new charge
  double charged_energy_new = 0.;
  if (!aux::cmp_equal((old_charge - energyOutput), 0. , genesys::ProgramSettings::approx_epsilon() )) {
    charged_energy_new = std::abs(reduced_charged_energy - energyOutput);
    if (charged_energy_new < genesys::ProgramSettings::approx_epsilon()) {
      charged_energy_new = 0.;//reset if small
    }
  }
  charged_energy_ += aux::TimeSeriesConst(std::vector<double>{(charged_energy_new - old_charge)},
                                          clock.now(),
                                          clock.tick_length());
  energy_lost_by_transfer_ += aux::TimeSeriesConst(std::vector<double>{losses , 0.},
                                                   clock.now(),
                                                   clock.tick_length());
  //std::cout << "\t"<< code() << "\t\t|new soc = " << charged_energy_[clock]/capacity(clock.now())*100 << "%" << std::endl;
  //Return status of useCapacity, update state variables in SysComponentActive
  return (SysComponentActive::useCapacity(energyOutput, clock, false));
}


bool Storage::useCapacityCharge(double inputEnergyRated,
                                const aux::SimulationClock& clock) {
  //	std::cout << "FUNC-ID: Storage::useCapacityCharge with energy: " << inputEnergyRated << " GWh" << std::endl;
  bool rval_sucess = false;
  if (inputEnergyRated < 0.0) {
      std::cerr << "ERROR in dm_hsm::Storage::useCapacityCharge - storing  negative energy is illegal: energy = " << inputEnergyRated << std::endl;
      std::terminate();
  } else if ( (inputEnergyRated - reserved_capacity_tp() ) > genesys::ProgramSettings::approx_epsilon()) {
    std::cerr << "ERROR in dm_hsm::Storage::useCapacityCharge - insufficient capacity reserved! " << std::endl;
          std::cout << "\t\tStorage::reserved capacity = "<< reserved_capacity_tp() <<std::endl;
          std::cout << "\t\t inputEnergyRated = "<< inputEnergyRated <<std::endl;
          std::cout << "\t\t capacity=" << capacity(clock.now()) << std::endl;
    std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
    std::terminate();
  }
  //good conditions:
  if (charged_energy_[clock] + inputEnergyRated - capacity(clock.now()) <= 0){
    charged_energy_ += aux::TimeSeriesConst(std::vector<double>{(inputEnergyRated)}, clock.now(),
                                            clock.tick_length());
    add_used_capacity(clock, -inputEnergyRated);
    add_usable_capacity(inputEnergyRated, clock);
    set_reserve_capacity_tp(clock,0.);
    set_active_current_year(true);
    rval_sucess = true;
  }
  return rval_sucess;
}

void Storage::print_current_capacities(const aux::SimulationClock& clock){
  std::cout << "\t\t\t | Storage: " << code() << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< initial_SOC_ << "\t initial_SOC_" << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< initial_SOC_ << "\t initial_SOC_" << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< stored_energy_transfer_ << "\t stored_energy_transfer_" << std::endl;
  std::cout << "\t\t\t | \t "<< std::setw(12)<< charged_energy_[clock] << "\t charged_energy_[tp]" << std::endl;
  SysComponentActive::print_current_capacities(clock);
  std::cout << "\t\t\t | \t--------------------------------" << std::endl;
}

} /* namespace dm_hsm */
