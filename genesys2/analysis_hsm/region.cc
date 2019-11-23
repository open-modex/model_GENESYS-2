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
// region.cc
//
// This file is part of the genesys-framework v.2

#include <analysis_hsm/region.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <utility>

#include <auxiliaries/functions.h>
#include <dynamic_model_hsm/region.h>

namespace analysis_hsm {

Region::Region(const dm_hsm::Region& origin)
    : dm_hsm::Region(origin) {
  //std::cout << "DEBUG: C-Tor an::Region::Region" << std::endl;
  for (const auto& it : converter_ptrs_dm()) {
    if (auto multi_conv = std::dynamic_pointer_cast<dm_hsm::MultiConverter>(it.second)) {
      converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(new MultiConverter(*multi_conv)));
    } else {
      converter_ptrs_.emplace(it.first, std::shared_ptr<Converter>(
            new Converter(static_cast<dm_hsm::Converter>(*it.second)))); // static_cast is necessary because of multiple inheritance
    }
  }
  for (const auto& it : storage_ptrs_dm()) {
    if (auto storage = std::dynamic_pointer_cast<dm_hsm::Storage>(it.second)) {
      storage_ptrs_.emplace(it.first, std::shared_ptr<Storage>(new Storage(*storage)));
      //std::cout << "copy2 storage_an = " << storage_ptrs_.find(it.first)->second->get_fopex().PrintToString() << std::endl;
    } else {
      storage_ptrs_.emplace(it.first, std::shared_ptr<analysis_hsm::Storage>(
                  new analysis_hsm::Storage(static_cast<dm_hsm::Storage>(*it.second)))); // static_cast is necessary because of multiple inheritance
    }

  }
  for (const auto& it : primary_energy_ptrs_dm()) {
    primary_energy_ptrs_.emplace(it.first, std::shared_ptr<PrimaryEnergy>(new PrimaryEnergy(*it.second)));
  }
  //std::cout << "DEBUG: END C-Tor an::Region::Region" << std::endl;
}

void Region::writeXmlRegion(io_routines::XmlWriter& xmlout, const bool dynamic_output) {
  //std::cout << "FUNC-ID: Region::writeStaticRegion" << std::endl;
  //CONVERTER
    for (const auto& it : converter_ptrs_) {
      xmlout.addElement("converter");
      xmlout.addAttr("code", it.first);
      it.second->writeXmlVariables(xmlout, dynamic_output);
      xmlout.closeElement();//converter
    }
  //STORAGES
    for (const auto& it : storage_ptrs_) {
          xmlout.addElement("storage");
          xmlout.addAttr("code", it.first);
          it.second->writeXmlVariables(xmlout, dynamic_output);
          xmlout.closeElement();//primary energy
    }
  //PRIMARY_ENERGIES
    if (dynamic_output) {
      for (const auto& it : primary_energy_ptrs_) {
                xmlout.addElement("primary_energy");
                xmlout.addAttr("code", it.first);
                it.second->writeDynamicVariables(xmlout);
                xmlout.closeElement();//primary energy
      }
    }
  //REGION_INTERNAL
		//non-dynamic output=========================================================================================
  xmlout.addTag("region_internal");
  writeXmlElement(xmlout, "annual_demand_electricity", tscAnnualConsumption());
  writeXmlElement(xmlout, "annual_imported_electricity", get_annual_imported_electricity_GWh_());
  writeXmlElement(xmlout, "import_for_local_balance", get_import_for_local_balance_());
  try {
    writeXmlElement(xmlout, "annual_co2_potential", tsc_annual_co2_potential());
  } catch (const std::exception& e) {
  }
  writeXmlElement(xmlout, "annual_co2_emissions", get_annual_co2_emissions_());
		//dynamic output===============================================================================================
  if (dynamic_output){
	writeXmlElement(xmlout, "load", get_load());
    writeXmlElement(xmlout, "residual_load", get_residual_load_()); //original load
    writeXmlElement(xmlout, "residual_heat_load", get_residual_heat_load_()); //original load
    writeXmlElement(xmlout, "remaining_residual_load", get_remaining_residual_load_()); //modified load
    writeXmlElement(xmlout, "remaining_residual_heat_load", get_remaining_residual_heat_load_()); //modified load
  }
  //  writeXmlElement(xmlout, "annual_co2_emissions", get_annual_co2_emissions_());
  //  writeXmlElement(xmlout, "annual_exported_electricity", get_annual_exported_electricity_GWh_());
  //  writeXmlElement(xmlout, "annual_annual_exported_gas", get_annual_annual_exported_gas_GWh_());
  //  writeXmlElement(xmlout, "annual_imported_gas", get_annual_imported_gas_GWh_());
  //  writeXmlElement(xmlout, "annual_consumed_heat",  get_annual_consumed_heat_GWh_());
  //  writeXmlElement(xmlout, "annual_generated_heat", get_annual_generated_heat_GWh_());
  xmlout.closeTag();
}

void Region::writeXmlElement(io_routines::XmlWriter& xml_target, std::string data_code, const aux::TimeSeriesConst& dataObj) {
   //std::cout << "DEBUG: writing" << code() << " - output data=" << data_code <<std::endl;
   xml_target.addSubElement("data");
   xml_target.addAttr("code", data_code);
   xml_target.addTBD(dataObj);
   xml_target.closeSubElement();
}

aux::TimeSeriesConst Region::tscAnnualConsumption() {//necessary because demand_electric_per_a is tbd type and cannot be converter to time series
  //std::cout << "FUNC-ID: Region::tscAnnualConsumption" << std::endl;
  std::vector<double> values;
  auto start_tp = genesys::ProgramSettings::simulation_start();
  aux::SimulationClock printClock(start_tp, aux::years(1));
  do {
    values.push_back(demand_electric_per_a(printClock.now()));
  } while (printClock.tick() < genesys::ProgramSettings::simulation_end());
  return aux::TimeSeriesConst(values, start_tp, aux::years(1));
}

aux::TimeSeriesConst Region::tsc_annual_co2_potential() {
  //std::cout << "FUNC-ID: Region::tsc_annual_co2_potential" << std::endl;
  std::vector<double> values;
  auto start_tp = genesys::ProgramSettings::simulation_start();
  aux::SimulationClock printClock(start_tp, aux::years(1));

  bool values_inserted=false;
  if (!primary_energy_ptrs_.empty()) {
	  do {
		  try {
			  if(primary_energy_ptrs_.find("CO2") != primary_energy_ptrs_.end()){
				  //
				  std::cout << "found co2" <<  primary_energy_ptrs_.find("CO2")->second->code() << std::endl;
				  values.push_back(primary_energy_ptrs_.find("CO2")->second->potential(printClock.now()));
				  values_inserted = true;
			  }
		} catch (int  e) {
			std::cout << "ERROR in Region::tscAnnualConsumption: could not access CO2 object : with error " << e << std::endl;
		}
		 // values.push_back(primary_energy_ptrs_.find("CO2")->second->potential(printClock.now()));
	  } while (printClock.tick() < genesys::ProgramSettings::simulation_end());
  }
  if (values_inserted){
	  return aux::TimeSeriesConst(values, start_tp, aux::years(1));
  }//else
  values.push_back(0.);
  return aux::TimeSeriesConst(values, start_tp, aux::years(1));
}

} /* namespace analysis_hsm */
