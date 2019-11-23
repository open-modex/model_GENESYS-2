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
// converter.cc
//
// This file is part of the genesys-framework v.2

#include <dynamic_model_hsm/converter.h>

#include <algorithm>
#include <iomanip>
#include <iostream>

//DEBUG:
#include <thread>
#include <chrono>

namespace dm_hsm {

//Converter::Converter(const Converter& other)


void Converter::ResetSequencedConverter() {
  resetSequencedSysComponent();
  price_per_unit_current_tp_ = std::numeric_limits<double>::infinity();
  co2_price_per_unit_current_tp_ = std::numeric_limits<double>::infinity();
}

void Converter::check_for_decommission(){
  if(!active_current_year() ){
    //DEBUG
    std::cout << code() << " PowerPlant was not used in the preceding year / decommissioning for future years!" << std::endl;
    hsm_category_ = HSMCategory::UNAVAILABLE;
  }
}

void Converter::setHSMcategory(dm_hsm::HSMCategory category) {
  hsm_category_ = category;
  if (category == dm_hsm::HSMCategory::DISPATCHABLE_GENERATOR) {
    //is_multi_converter = true;
  }
}

void Converter::connectReservoir(){
  std::cout << "Converter::connectReservoir() not defined!" << std::endl;
  std::terminate();
  //auto input = input();
}

void Converter::connectStorage(std::shared_ptr<dm_hsm::Storage> storage_ptr) {
  //std::cout << "\tDEBUG FUNC-ID: DynamicModel::Converter::connectStorage" << std::endl;
  if (storage_connected_)
    std::cerr << "\t***WARNING this converter is already connected to a storage - switching ptrs now!" << std::endl;
  storageptr_ = storage_ptr;
  if (storageptr_.expired()) {
    std::cerr << "ERROR in Converter::connectStorage - storageptr_ expired in " << code() << std::endl;
    std::terminate();
  } else {
    storage_connected_ = true; //this should be the only function that modifies the bool to true!
  }
}

void Converter::connectPrimaryEnergy(std::shared_ptr<dm_hsm::PrimaryEnergy> primenergy_ptr) {
  if (primary_energy_connected_)
    std::cerr << "\t***WARNING this converter is already connected to a primary energy - switching ptrs now!" << std::endl;
  primenergyptr_ = primenergy_ptr;
  if (primenergyptr_.expired()) {
    std::cerr << "ERROR in Converter::connectPrimaryEnergy - primenergyptr_ expired in " << code() << std::endl;
    std::terminate();
  } else {
    primary_energy_connected_ = true;//this should be the only function that modifies the bool to true!
    //std::cout << "DEBUG\t\t\t\t*****SUCCESS" << std::endl;
  }

}

bool Converter::connectCo2Reservoir(std::shared_ptr<dm_hsm::PrimaryEnergy>) {
  std::cerr << "Cannot Converter::connectLocalCo2Reservoir - only defined for multi-converter" << std::endl;
  std::terminate();
  return false;
}

double Converter::generation_fromPotential(const aux::SimulationClock& clock){
  if (hsm_category_ == dm_hsm::HSMCategory::RE_GENERATOR) {
	  //RE_GENERATOR does not use the active_current_year_-flag, OaM cost are added every year!
    double re_generation = capacity(clock.now()) * primenergyptr_.lock()->potential(clock.now(), capacity(clock.now()) );
	//  std::cout << "RE Generator\t" << code() << " generating\t" << re_generation << std::endl;
	//  std::cout << "	\t capacity         \t" << capacity(clock.now()) << std::endl;
	//  std::cout << "	\t potential - mean?\t" << primenergyptr_.lock()->potential(clock.now())<< std::endl;
	//  std::cout << "	\t potential - look \t" << primenergyptr_.lock()->potential(clock.now(), capacity(clock.now()))<< std::endl;
    add_used_capacity(clock, re_generation);
    return (re_generation);
  } else {
    std::cerr << "ERROR in Converter::getGeneration - no behaviour defined for this category: "
              << static_cast<int>(hsm_category_) << std::endl;
    std::terminate();
    return 0.0; // dummy return
  }
}

bool Converter::useConverterOutput(const aux::SimulationClock& clock,
                                   const double output_request) { //returns true if successfull
  //function only used for storages -other converter use the multiConverter function!!
  //std::cout << "\tFUNC: Converter::useConverterOutput - code: " << code() << " | cap: " << capacity(clock.now())
  //          << " | reserved = " << reserved_capacity_tp()<< "\t|request=" << output_request<< " GW"<< std::endl;
  //TODO GRIDBALANCE PROBLEM?
  if ( output_request > ( reserved_capacity_tp()+ genesys::ProgramSettings::approx_epsilon())) {
    std::cerr << "ERROR Converter::useConverterOutput - output_request > reserved_capacity_tp!" << std::endl;
    std::cout << "output_request = " << output_request << " > " << reserved_capacity_tp() << " reserved capacity_tp " << std::endl;
    std::terminate();
  }
  if(!storageptr_.expired()){
    double DchgInputFromStorage = output_request / efficiency(clock.now(), output_request);
    //std::cout << "\t| DchgInputFromStorage=\t" << DchgInputFromStorage << " Gw"<< std::endl;
    double EDchg = aux::SimulationClock::p2e(DchgInputFromStorage, clock.tick_length());
    //std::cout << "\t| EDchg               =\t" << EDchg << " Gwh"<< std::endl;
    if (storageptr_.lock()->useCapacityDischarge(EDchg, clock) ) {
      return SysComponentActive::useCapacity(output_request, clock);
    }
  } else if (!primenergyptr_.expired()) {
    std::cerr << "ERROR Converter::useConverterOutput:  not implemented for use of PrimaryEnergy with storage Converter" << std::endl;
    std::terminate();
  } else {
    std::cerr << "ERROR in Converter::useConverterOutput - storageptr_ has expired!" << std::endl;
    std::terminate();
  }
  return false;
}

double Converter::reserveDischarger(const aux::SimulationClock& clock,
                                const double output_request){
  //std::cout << "\tFUNC-ID: Converter::reserveChargerOutput for " << code() << "  with requestPower= " << output_request << std::endl;
    if(output_request < -genesys::ProgramSettings::approx_epsilon()){
      std::cerr << "ERROR in Converter::reserveDischarger with negative request= " << output_request << std::endl;
      std::terminate();
    } else if (output_request < genesys::ProgramSettings::approx_epsilon()           //CASE small request
            || capacity(clock.now()) < genesys::ProgramSettings::approx_epsilon()) { //CASE converter pwr too small
      // std::cout << "Converter::usable_power_out_tp has requestedPower very small! " << output_request<< std::endl;
      return 0.;//premature termination
    }
    double rval_reserved_output = 0.0;
    //implicit  if ((hsm_category_ == dm_hsm::HSMCategory::LT_STORAGE) || (hsm_category_ == dm_hsm::HSMCategory::ST_STORAGE)) {
    if (!storageptr_.expired()) {
      //Discharger Power
      double maxDchgPwrConv = std::min(usable_capacity_el(clock), output_request);
      //reduce power to output
      double DchgInputFromStorage = maxDchgPwrConv / efficiency(clock.now(), maxDchgPwrConv);
      double EDchg = aux::SimulationClock::p2e(DchgInputFromStorage, clock.tick_length());
      double maxEDchgStorage = storageptr_.lock()->getCapacityDischarge(EDchg, clock);
      //std::cout << "\t| maxDchgPwrConv=\t" << maxDchgPwrConv << " Gw"<< std::endl;
      //std::cout << "\t| DchgInputFromStorage=\t" << DchgInputFromStorage << " Gw"<< std::endl;
      //std::cout << "\t| EDchg=\t" << EDchg << " Gwh"<< std::endl;
      //std::cout << "\t| maxEDchgStorage=\t" << maxEDchgStorage << " Gwh"<< std::endl;
      if (maxEDchgStorage > genesys::ProgramSettings::approx_epsilon()) {
        if (storageptr_.lock()->reserveCapacityDischarge(maxEDchgStorage, clock)) {
          rval_reserved_output = aux::SimulationClock::e2p(maxEDchgStorage, clock.tick_length());
          //reduce to the el_output side
          rval_reserved_output = rval_reserved_output * efficiency(clock.now(), rval_reserved_output);
          //std::cout << "\t|reduced power to be reserved from converter= "<< rval_reserved_output << " GW" << std::endl;
          //after storage was reserved successfully, now reserve converter
          if (set_reserve_capacity_tp(clock, rval_reserved_output) ) {
            //std::cout << "\t|rval_reserved_output=" << rval_reserved_output << " GW" << std::endl;
            return rval_reserved_output;
          }
        }
      } else {
        return 0.; // storage does not allow discharge!
      }
    } else {
      std::cerr << "ERROR in Converter::reserveDischarger - storageptr_ has expired!" << std::endl;
      std::terminate();
    }
    return 0.;
}

/* Heat Integrationsversuch: kja
bool Converter::useConverterOutput_el2heat(const aux::SimulationClock& clock,
                                   const double output_request) {
	set_reserve_capacity_tp(clock, 0.);//converter variable reset
	set_active_current_year(true);//converter activity set
	add_used_capacity(clock, output_request);
	return true;
}
*/

/* Heat Integrationsversuch: kja
double Converter::usable_power_out_tp_el2heat(const aux::SimulationClock& clock, const double requestPowerHeat) {
	double rval_reserved_output_heat = std::min(usable_capacity_el(clock), requestPowerHeat);
		//Reserve Converter with applied limits
	    if (rval_reserved_output_heat > genesys::ProgramSettings::approx_epsilon()) {
	      //reserve converter
	  	  // Kevin muss doch nicht die Kap nach Effizienz sonder vorher speichern ???
	      if (set_reserve_capacity_tp(clock, rval_reserved_output_heat) ) {
	        return rval_reserved_output_heat;
	      }
	    }
	  //return
}
*/

double Converter::usable_power_out_tp(const aux::SimulationClock& clock, const double requestPower) {
  if(get_HSMSubCategory() != dm_hsm::HSMSubCategory::UNAVAILABLE ){
   std::cerr << "ERROR Converter::usable_power_out_tp - not defined for storages!" << std::endl;
   //HINT: see MultiConverter::usable_power_out_tp if looking for a converter with fuel!
   std::terminate();
  }
  /* Heat Integrationsversuch: kja
	double rval_reserved_output = std::min(usable_capacity_el(clock), requestPower);
	//Reserve Converter with applied limits
    if (rval_reserved_output > genesys::ProgramSettings::approx_epsilon()) {
      //reserve converter
      if (set_reserve_capacity_tp(clock, rval_reserved_output) ) {
        return rval_reserved_output;
      }
    }
  */
  return -1;
}

/* Heat Integrationsversuch: kja
double Converter::get_output_conversion(const std::string sType) {
  return 0.;
}
*/

double Converter::reserveChargerInput( const aux::SimulationClock& clock,
                                       double inRequestEl ) {
  //std::cout << "\tFUNC-ID: Converter::reserveChargerInput with inputRequest= " << inRequestEl << std::endl;
  double reservableInputPwr = 0.0;
  //implicit  if ((hsm_category_ == dm_hsm::HSMCategory::LT_STORAGE) || (hsm_category_ == dm_hsm::HSMCategory::ST_STORAGE)) {
    if (!storageptr_.expired()) {
      //Charge Acceptance of the Converter
      double maxAcceptConvPwr = std::min(usable_capacity_el(clock), inRequestEl);
      //reduce power to output
      double maxOutputConv = maxAcceptConvPwr * efficiency(clock.now(), maxAcceptConvPwr);
      //Charge Acceptance of Storage Reservoir
      double maxAcceptStorageEngy  = storageptr_.lock()->getCapacityCharge(aux::SimulationClock::p2e(maxOutputConv,
    		                                                                                         clock.tick_length()), clock);
      if (maxAcceptStorageEngy > genesys::ProgramSettings::approx_epsilon()) {
        if (storageptr_.lock()->reserveCapacityCharge(clock, maxAcceptStorageEngy)) {
          reservableInputPwr = aux::SimulationClock::e2p(maxAcceptStorageEngy, clock.tick_length());
          //expand to the el_input side
          reservableInputPwr = reservableInputPwr / efficiency(clock.now(), reservableInputPwr);
        }
        //storage was reserved successfully / now reserve converter
        if (set_reserve_capacity_tp(clock, reservableInputPwr) ) {
          return reservableInputPwr;
        } else {
        	//could not reserve - try next
		    //          std::cerr << "ERROR in Converter::reserveChargerInput: could not set_reserve_capacity_tp of charger " << code() << std::endl;
	     	//          std::terminate();
          return 0.;//reserve storage unsuccessful
        }
      } else {
        return 0.; // storage does not accept any more charge!
      }

  } else {
    std::cout << "\t***WARNING: Storage ptr expired! --- Could not acceess Converter or Storage" << code() << std::endl;
    return 0.;
  }
  return -1.;
}

bool Converter::useChargeStorage(double inputPower,
                                 const aux::SimulationClock& clock) {
  //std::cout << "\tFUNC-ID: Converter::useChargeStorage for " << code() << " with inputPower= " << inputPower <<  " GW" << std::endl;
  //TODO implement CO2 Emissions for Storages like Methane
  bool useSucceded = false;
  if ((std::abs(inputPower - reserved_capacity_tp())) < genesys::ProgramSettings::approx_epsilon()) {
    //std::cout << "\t| accepted reserved capacity sufficient in converter " << code() << std::endl;
    //reduce input (electrical energy) with efficiency
    double Energy_To_Storage = aux::SimulationClock::p2e(inputPower*efficiency(clock.now(), inputPower), clock.tick_length());
    if(storageptr_.lock()->useCapacityCharge(Energy_To_Storage, clock)) {
      useSucceded = useCapacity(inputPower, clock, true);
    }
  } else {
        std::cerr << "ERROR Converter::useStorablePower: requested power of " << inputPower
                  << " GW exceeds reserved_power of " <<  reserved_capacity_tp() << " GW" <<std::endl;
        std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
        std::terminate();
  }
  return useSucceded;
}
//  if ((std::abs(inputPower - reserved_capacity_tp())) < genesys::ProgramSettings::approx_epsilon()) {
//    // change charge in storage -> transform inputPower 2 Energy (inflated by efficiency)
//    if (storageptr_.lock()->useCapacityCharge(aux::SimulationClock::p2e(inputPower/efficiency(clock.now()), clock.tick_length()), clock)){
//      //todo use negative power for charging of storage!!!
//      useSucceded = useCapacity(inputPower, clock, true);
//    }
//  } else {
//      std::cerr << "ERROR Converter::useStorablePower: requested power of " << inputPower<<  " \n "
//          "\t\t transformed to inputRated: " << inputPower << "\n"
//          "\t\t exceeds reserved_power of " <<  reserved_capacity_tp() << std::endl;
//      std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
//      std::terminate();
//  }
//  return useSucceded;
//}

double Converter::get_efficiency(const aux::SimulationClock& clock) const {
  //  std::cout << "FUNC-ID: Converter::getEfficiencyMaxPower" << std::endl;
  return efficiency(clock.now());
}

double Converter::get_capacity (const aux::SimulationClock& clock) const {
  //  std::cout << "FUNC-ID: Converter::getEfficiencyMaxPower" << std::endl;
  return capacity(clock.now());
}

} /* namespace dm_hsm */
