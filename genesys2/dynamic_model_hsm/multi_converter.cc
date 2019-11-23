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
// multi_converter.cc
//
// This file is part of the genesys-framework v.2

#include <dynamic_model_hsm/multi_converter.h>

namespace dm_hsm {

bool MultiConverter::connectCo2Reservoir(std::shared_ptr<dm_hsm::PrimaryEnergy> primenergy_ptr) {
  //std::cout << "\tFUNC-ID: MultiConverter::connectCo2Reservoir " << code() << "  with ptr= " << primenergy_ptr->code() << std::endl;
  //if (co2_connected_)
    //std::cerr << "\t***WARNING this multi-converter is already connected to a co2 reservoir "
    //             "- switching ptrs now!" << std::endl;
  if(primenergy_ptr){
    co2ptr_ = primenergy_ptr;
    co2_connected_ = true;//this should be the only function that modifies the bool to true!
    //std::cout << "DEBUG\t\t\t\t*****SUCCESS" << std::endl;
    return co2_connected_;
  }
  return false;
}

double MultiConverter::usable_power_out_tp(const aux::SimulationClock& clock,
                                           const double reqPwr_el_output) {
  //std::cout << "FUNC-ID: MultiConverter::usable_power_out_tp with reqPwr_el_output = "<< reqPwr_el_output << " GW" << std::endl;
  if(reqPwr_el_output < -genesys::ProgramSettings::approx_epsilon()){
    std::cerr << "ERROR in MultiConverter::usable_power_out_tp with negative request= " << reqPwr_el_output << std::endl;
    std::terminate();
  } else if (reqPwr_el_output < genesys::ProgramSettings::approx_epsilon()           //CASE small request
          || capacity(clock.now()) < genesys::ProgramSettings::approx_epsilon()) { //CASE converter pwr too small
    // std::cout << "MultiConverter::usable_power_out_tp has requestedPower very small! " << reqPwr_el_output<< std::endl;
    return 0.;//premature termination
  }
  double rval_reserved_output_el = 0.0;
  if(!co2ptr_.expired() && !primenergyptr().expired()) {
    double maxOutPwrConv = std::min(usable_capacity_el(clock), reqPwr_el_output);
    //std::cout << "\t| maxOutPwrConv = " << maxOutPwrConv << " GW" << std::endl;
    if (maxOutPwrConv > genesys::ProgramSettings::approx_epsilon()) {
      //transform output power to primary energy input
      double prim_pwr_input = maxOutPwrConv / efficiency(clock.now(), maxOutPwrConv);
      double prim_energy_input = aux::SimulationClock::p2e(prim_pwr_input, clock.tick_length());
      double primEnergy_consumation_limited=0.;
      //Check Primary Energy & CO2 Reservoir for capacity:
          //-----------------Limitation from Primary energy availability-------------------------------------
          std::string co2_code = co2ptr_.lock()->code();
          double co2_output = std::min(prim_energy_input * output_conversion().find(co2_code)->second,
                                       co2ptr_.lock()->get_potential_current_tp() );
          //std::cout << "co2 output: " << co2_output << std::endl;
          double prim_energy_input_co2_limited = co2_output / output_conversion().find(co2_code)->second;
				if(prim_energy_input > prim_energy_input_co2_limited + genesys::ProgramSettings::approx_epsilon()) {
					  std::cerr << aux::SimulationClock::time_point_to_string(clock.now())<< "\t*WARNING -Multiconverter "
							   "Output limited by CO2 potential: "<< co2ptr_.lock()->get_potential_current_tp() << std::endl;
				}
          //-----------------Limitation from Primary energy availability-------------------------------------
          primEnergy_consumation_limited = std::min(prim_energy_input_co2_limited,
                                                    primenergyptr().lock()->get_potential_current_tp() );
          //          if(prim_energy_input_co2_limited > primEnergy_consumation_limited + genesys::ProgramSettings::approx_epsilon()) {
          //                std::cerr << aux::SimulationClock::time_point_to_string(clock.now())<< "\t*WARNING - Multiconverter "
          //                "Output limited by Primary Energy potential \t" << primenergyptr().lock()->code() << "\t"
          //                << primenergyptr().lock()->get_potential_current_tp()  << std::endl;
          //          }
      //Transform from energy to power -->then from primary input to electrical output
      double maxOutputPwr_primary = aux::SimulationClock::e2p(primEnergy_consumation_limited, clock.tick_length());;
      if (maxOutputPwr_primary > genesys::ProgramSettings::approx_epsilon())
        rval_reserved_output_el = maxOutputPwr_primary * efficiency(clock.now(), maxOutputPwr_primary);
      else
        rval_reserved_output_el = 0;
      //Reserve Converter with applied limits
      if (rval_reserved_output_el > genesys::ProgramSettings::approx_epsilon()) {
        //reserve converter
    	  if (set_reserve_capacity_tp(clock, rval_reserved_output_el) ) {
          //reserve primary energy & co2 emission
          if (primenergyptr().lock()->reserveCapacity(prim_energy_input_co2_limited, clock)) {
            double co2 = prim_energy_input_co2_limited* output_conversion().find(co2ptr_.lock()->code())->second;
            if (co2ptr_.lock()->reserveCapacity(co2, clock)) {
              //std::cout << "\t|-->success: rval_reserved_output_el=" << rval_reserved_output_el << " GW" << std::endl;
              return rval_reserved_output_el;
            }
          }
        } else {
          std::cerr << "ERROR in MultiConverter::usable_power_out_tp - reserve failed!" << std::endl;
        }

      }
    }
    return 0.; //cannot use any output
  } else {
    std::cerr << "ERROR: MultiConverter::usable_power_out_tp - could not identify primary energy source!" << std::endl;
    std::cerr << "CO2 or Primary energy expired:" << std::endl;
    std::cerr << "co2ptr_.expired():         "<< co2ptr_.expired()         << std::endl;
    std::cerr << "primenergyptr().expired(): "<< primenergyptr().expired() << std::endl;
    std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
    std::terminate();

  }
  return rval_reserved_output_el;

}

/* Heat Integrationsversuch: kja
double MultiConverter::get_output_conversion(const std::string sType) {
  //std::cout << "conversion result: " << output_conversion().find(sType)->second << std::endl;
  return output_conversion().find(sType)->second;
}
*/

bool MultiConverter::useConverterOutput(const aux::SimulationClock& clock,
                                        const double output_request) {
  //std::cout << "\t||FUNC-ID: MultiConverter::useConverterOutput for " << code() << "  with output_request= " << output_request << std::endl;
  if (reserved_capacity_tp() > (output_request + genesys::ProgramSettings::approx_epsilon())) {
  	std::cout << "ERROR MultiConverter::useConverterOutput - request decreased somwhere - this should not happen!"
  	          << std::endl;
  	std::terminate();
	}

  if(!co2ptr_.expired() && !primenergyptr().expired()) {
    double prim_pwr_input = reserved_capacity_tp()/efficiency(clock.now(), reserved_capacity_tp());
    if (prim_pwr_input > genesys::ProgramSettings::approx_epsilon()) {
        //transform output power to primary energy input
        double prim_energy_input = aux::SimulationClock::p2e(prim_pwr_input, clock.tick_length());
        double co2 = prim_energy_input * output_conversion().find(co2ptr_.lock()->code())->second;
        //------Use capacity from primary energy and co2
        double vopex = 0.0;
        if (co2ptr_.lock()->useCapacity(co2, clock) ) {
              vopex += co2ptr_.lock()->get_vopex(clock.now());
        } else {
          std::cerr << "ERROR in MultiConverter::useConverterOutput could not use CO2-Emission "
                        << co2ptr_.lock()->code() << std::endl;
          std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
          std::terminate();
        }
        if (primenergyptr().lock()->useCapacity(prim_energy_input, clock) ) {
          vopex += primenergyptr().lock()->get_vopex(clock.now());
          set_reserve_capacity_tp(clock, 0.);//converter variable reset
          set_active_current_year(true);//converter activity set
          add_vopex(vopex, clock);
          add_used_capacity(clock, output_request);
          set_co2_emissions(aux::TimeSeriesConst(std::vector<double>{co2, 0.}, clock.now(), clock.tick_length()));
          //DEBUG std::cout << "END FUNC sucess MultiConverter::useConverterOutput " << code() << "  with vopex= " << vopex << std::endl;
          return true;
        } else {
          std::cerr << "ERROR in MultiConverter::useConverterOutput could not use PrimaryEnergy "
                    << primenergyptr().lock()->code() << std::endl;
          std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
          std::terminate();
        }
    }
  } else {
    std::cerr << "ERROR: MultiConverter::useConverterOutput - could not identify primary energy source!" << std::endl;
    std::cerr << "CO2 or Primary energy expired:" << std::endl;
    std::cerr << "co2ptr_.expired():         "<< co2ptr_.expired()         << std::endl;
    std::cerr << "primenergyptr().expired(): "<< primenergyptr().expired() << std::endl;
    std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
    std::terminate();
  }
  return false;
}

double MultiConverter::get_spec_vopex(const aux::SimulationClock& clock) {
  //std::cout << "FUNC-ID: MultiConverter::get_spec_vopex in " << code() << std::endl;
  double spec_vopex = 0.;//init
  //add spec_vopex for co2 and primary energy:
  if(!co2ptr_.expired()){
    //conversion-relation for 1 GW * specific_vopex_co2
		//	for(auto it : output_conversion()){
		//		std::cout << it.first << " : " << it.second << std::endl;
		//	}
		//std::cout << "co2-prt = " << 	co2ptr_.lock()->code() << " in converter \t" << code()<< std::endl;
		//spec_vopex += output_conversion().find("CO2")->second*co2ptr_.lock()->get_specific_vopex(clock);
    spec_vopex += output_conversion().find(co2ptr_.lock()->code())->second*co2ptr_.lock()->get_specific_vopex(clock);
    	//std::cout << spec_vopex << " spec_vopex" << std::endl;
  } else {
        std::cerr << "ERROR in MultiConverter::get_spec_vopex could not get specific CO2-Emission - ptr expired!"
                  << co2ptr_.lock()->code() << std::endl;
        std::cout << "Clock: " << aux::SimulationClock::time_point_to_string(clock.now()) << std::endl;
        std::terminate();
  }
  if (!primenergyptr().expired()) {
    if (efficiency(clock.now()) > 0.) {
      spec_vopex += primenergyptr().lock()->get_specific_vopex(clock);
    } else {
      std::cerr << "ERROR in MultiConverter::get_spec_vopex : efficiency = 0" << std::endl;
      std::terminate();
    }
  }

  return spec_vopex/ efficiency(clock.now());
}


} /* namespace dm_hsm */
