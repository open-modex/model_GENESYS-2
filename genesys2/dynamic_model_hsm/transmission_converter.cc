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
// transmission_converter.cc
//
// This file is part of the genesys-framework v.2

#include <auxiliaries/functions.h>
#include <dynamic_model_hsm/transmission_converter.h>

#include <algorithm>

#include <dynamic_model_hsm/region.h>

namespace dm_hsm {

//TransmissionConverter::TransmissionConverter(const TransmissionConverter& other)

void TransmissionConverter::ResetSequencedTransmConverter() {
	ResetSequencedConverter();
}

double TransmissionConverter::get_transmittable_pwr_infeed(const aux::SimulationClock& clock, double req_output) const {
   if (capacity(clock.now()) > genesys::ProgramSettings::approx_epsilon()) {
     double maxOutput = std::min(usable_capacity_el(clock), req_output);
     double request_transformed_to_infeed  = maxOutput/efficiency(clock.now(),maxOutput);
     return request_transformed_to_infeed;
   }
   return 0.;//else
}

double TransmissionConverter::reserveTransmission(const aux::SimulationClock& clock,
                                         const double transmission_pwr_infeed) {
  double el_output = transmission_pwr_infeed*efficiency(clock.now(),transmission_pwr_infeed);
  if(el_output > genesys::ProgramSettings::approx_epsilon()) {
    double maxPowerOut_el = std::min(usable_capacity_el(clock), el_output);
    if (set_reserve_capacity_tp(clock, maxPowerOut_el) ) {
      return maxPowerOut_el;//good case: this is the import pwr we can deliver to the requesting region
    } else {
      std::cout << "ERROR in Converter::reserveConverterOutput - could not reserve for transmission" << std::endl;
    }
  }
  return 0.;//else cases
}

bool TransmissionConverter::useTransmission(const dm_hsm::HSMCategory cat,
                                            const double import_request,
                                            const aux::SimulationClock& clock,
                                            const std::weak_ptr<Region>& supply_region,
                                            const std::weak_ptr<Link>& active_Link){
  //std::cout << "call FUNC-ID: TransmissionConverter::useTransmission" << std::endl;
  bool transmission_succeedes = false;
  if (!(import_request > reserved_capacity_tp())) {
    //inflate import to infeed
    double requested_infeed = import_request / efficiency(clock.now(), import_request);
    transmission_succeedes = supply_region.lock()->useExportCapacity(cat, requested_infeed, clock, active_Link.lock());
    if (!transmission_succeedes){
      //          std::cout << "**WARNING : "<< supply_region.lock()->code() <<" failed in useExportCapacity() TP:\t\t"<<
      //                    aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
      //          std::cout << "ERROR in TransmissionConverter::useTransmission " << std::endl;
    }
    return update_current_state(transmission_succeedes, import_request, clock);//update tr-conv state with uninflated value!
  }
  return transmission_succeedes;
}

void TransmissionConverter::print_current_capacities() const {
  std::cout << code() << "\t | " << usable_capacity_out_tp() <<"\t\t usable_capacity_out_tp " << "\n"
      << "\t\t\t | "  << reserved_capacity_tp()          <<"\t\t reserved_capacity_tp " << "\n" ;
  std::cout << "----------------------------------------------------------------------------------" << std::endl;
}


bool TransmissionConverter::update_current_state(const bool region_provide_status,
                                                 const double pwr_transfer,
                                                 const aux::SimulationClock& clock) {
  //std::cout << "FUNC: TransmissionConverter::update_current_state " << output_request << " | " << region_provide_status << std::endl;
  if (region_provide_status) {
    auto signed_pwr_transfer = 0.;//init
    if (forward_) {
      signed_pwr_transfer = pwr_transfer;
    } else {
      signed_pwr_transfer = -pwr_transfer;
    }
    delivered_energy_ += aux::TimeSeriesConst(std::vector<double>{signed_pwr_transfer, 0.},
                                              clock.now(),
                                              clock.tick_length());
    //DEBUG std::cout << pwr_transfer << std::endl;
    double losses = pwr_transfer * (-1 + 1/efficiency(clock.now(), pwr_transfer));
    auto TSA_losses = aux::TimeSeriesConst(std::vector<double>{losses, 0.},
                                           clock.now(),
                                           clock.tick_length());
    set_losses(TSA_losses);
    set_reserve_capacity_tp(clock,0.);
    set_active_current_year(true);
    add_used_capacity(clock, pwr_transfer);
    return true;
  }
  return false;//region does not provide!
}

bool TransmissionConverter::activate_forward() {
  if (active_ && !forward_) {
    return false;
  } else {
    active_ = true;
    forward_ = true;
    return active_;
  }

}
bool TransmissionConverter::activate_backwards() {
  if (active_ && forward_) {
    return false;
  } else {
    active_ = true;
    forward_ = false;
    return active_;
  }

}

} /* namespace dm_hsm */
