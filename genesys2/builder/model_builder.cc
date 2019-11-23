/// ==================================================================
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
// model_builder.cc
//
// This file is part of the genesys-framework v.2

#include <builder/model_builder.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <tuple>
#include <utility>
#include <thread> //sleep

#include <program_settings.h>
//#include <builder/model_enums.h>

namespace builder {

am::AbstractModel ModelBuilder::Create() {
  FactoryPrimaryEnergy("PrimaryEnergy.csv");
  FactoryStorage("Storage.csv");
  FactoryConverter("Converter.csv");
  FactoryMultiConverter("MultiConverter.csv");
  FactoryTransmissionConverter("TransmissionConverter.csv");
  FactoryRegions("Region.csv");
  FactoryLinks("Link.csv");
  if (genesys::ProgramSettings::use_global_file()) {
	  FactoryGlobals("global.csv");
  }
  return am::AbstractModel(primary_energy_prototype_cache_,
                           storage_prototype_cache_,
                           converter_prototype_cache_,
                           transm_converter_prototype_cache_,
                           region_prototype_cache_,
                           link_prototype_cache_,
                           multi_converter_prototype_cache_,
                           global_proto_);
}

void ModelBuilder::FactoryPrimaryEnergy(const std::string& filename) {
  //std::cout << "FUNC-ID: ModelBuilder::FactoryPrimaryEnergy\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  open_file(filename);
  skip_comment();
  check_blockwise();
  do {
    auto key_values = PrepareKeywords({"#name"});
    std::vector<std::string> expected_variables({});
    std::vector<std::string> expected_LT_variables({"cost_table"});
    PrepareVariables(expected_variables, expected_LT_variables, {"#endblock"});
    primary_energy_prototype_cache_.emplace(key_values[0], std::unique_ptr<builder::PrimEnergyPrototype>(
        new builder::PrimEnergyPrototype(key_values[0], key_values[1],
                                         std::move(TBDLTvariable_cache_.find(expected_LT_variables[0])->second))));
  } while (file_.next_line());
}

void ModelBuilder::FactoryStorage(const std::string& filename) {
  //std::cout << "FUNC-ID: ModelBuilder::FactoryStorage\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  open_file(filename);
  skip_comment();
  //check_blockwise();
  if(check_blockwise()){
	  //
	  std::cout << "check_blockwise succeeded" << file_.get_field() << " is current field " << std::endl;
	  do {
	    auto key_values = PrepareKeywords({"#name", "#input", "#output"});
	    std::vector<std::string> expected_variables({"efficiency_new", "cost", "lifetime", "internal_rate_of_return",
	                                                 "OaM_rate"});
	    std::vector<std::string> expected_LT_variables({});
	    PrepareVariables(expected_variables, expected_LT_variables, {"#endblock"});
	    storage_prototype_cache_.emplace(key_values[0], std::unique_ptr<builder::SysCompActPrototype>(
	        new builder::SysCompActPrototype(key_values[0], key_values[1], key_values[2], key_values[3],
	                                         std::move(TBDvariable_cache_.find(expected_variables[0])->second),
	                                         std::move(TBDvariable_cache_.find(expected_variables[1])->second),
	                                         std::move(TBDvariable_cache_.find(expected_variables[2])->second),
	                                         std::move(TBDvariable_cache_.find(expected_variables[3])->second),
	                                         std::move(TBDvariable_cache_.find(expected_variables[4])->second))));
	  } while (file_.next_line());
  } else {
	  std::cout << "WARNING: no valid blocks found for storage!" << std::endl;

  }

}

void ModelBuilder::FactoryConverter(const std::string& filename) {
  //std::cout << "FUNC-ID: ModelBuilder::FactoryConverter\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  open_file(filename);
  skip_comment();
  check_blockwise();
  do {
    auto key_values = PrepareKeywords({"#name", "#input", "#output", "#bidirectional"});
    std::vector<std::string> expected_variables({"efficiency_new", "cost", "lifetime", "internal_rate_of_return",
                                                 "OaM_rate"});
    std::vector<std::string> expected_LT_variables({});
    PrepareVariables(expected_variables, expected_LT_variables, {"#endblock"});
      converter_prototype_cache_.emplace(key_values[0], std::unique_ptr<builder::ConverterPrototype>(
          new builder::ConverterPrototype(key_values[0], key_values[1], key_values[2], key_values[3],
                                          (key_values[4] == "true") ? true : false,
                                          std::move(TBDvariable_cache_.find(expected_variables[0])->second),
                                          std::move(TBDvariable_cache_.find(expected_variables[1])->second),
                                          std::move(TBDvariable_cache_.find(expected_variables[2])->second),
                                          std::move(TBDvariable_cache_.find(expected_variables[3])->second),
                                          std::move(TBDvariable_cache_.find(expected_variables[4])->second))));
  } while (file_.next_line());
}

void ModelBuilder::FactoryMultiConverter(const std::string& filename) {
  //std::cout << "FUNC-ID: ModelBuilder::FactoryMultiConverter\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  open_file(filename);
  skip_comment();
  check_blockwise();
  skip_comment();
  do {
    auto key_values = PrepareKeywords({"#name", "#input"});
    auto MC_extra_content = PrepareMCExtraContent();
    std::vector<std::string> expected_variables({"efficiency_new", "cost", "lifetime", "internal_rate_of_return",
                                                 "OaM_rate"});
    std::vector<std::string> expected_LT_variables({});
    PrepareVariables(expected_variables, expected_LT_variables, {"#endblock"});
    multi_converter_prototype_cache_.emplace(key_values[0], std::unique_ptr<MultiConverterProto>(
          new MultiConverterProto(key_values[0], key_values[1], key_values[2], MC_extra_content,
                                  std::move(TBDvariable_cache_.find(expected_variables[0])->second),
                                  std::move(TBDvariable_cache_.find(expected_variables[1])->second),
                                  std::move(TBDvariable_cache_.find(expected_variables[2])->second),
                                  std::move(TBDvariable_cache_.find(expected_variables[3])->second),
                                  std::move(TBDvariable_cache_.find(expected_variables[4])->second))));
  } while (file_.next_line());
}

void ModelBuilder::FactoryTransmissionConverter(const std::string& filename) {
  //std::cout << "FUNC-ID: ModelBuilder::FactoryTransmissionConverter\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  open_file(filename);
  skip_comment();
  //check if blockwise or empty flag set
  if(check_blockwise()){
	  do {
	    auto key_values = PrepareKeywords({"#name", "#input", "#output", "#bidirectional"});
	    std::vector<std::string> expected_variables({"efficiency_new", "cost", "lifetime", "internal_rate_of_return",
	                                                 "OaM_rate", "length_dep_loss", "length_dep_cost"});
	    std::vector<std::string> expected_LT_variables({});
	    PrepareVariables(expected_variables, expected_LT_variables, {"#endblock"});
	      transm_converter_prototype_cache_.emplace(key_values[0], std::unique_ptr<builder::TransmConvPrototype>(
	          new builder::TransmConvPrototype(key_values[0], key_values[1], key_values[2], key_values[3],
	                                           (key_values[4] == "true") ? true : false,
	                                           std::move(TBDvariable_cache_.find(expected_variables[0])->second),
	                                           std::move(TBDvariable_cache_.find(expected_variables[1])->second),
	                                           std::move(TBDvariable_cache_.find(expected_variables[2])->second),
	                                           std::move(TBDvariable_cache_.find(expected_variables[3])->second),
	                                           std::move(TBDvariable_cache_.find(expected_variables[4])->second),
	                                           std::move(TBDvariable_cache_.find(expected_variables[5])->second),
	                                           std::move(TBDvariable_cache_.find(expected_variables[6])->second))));
	  } while (file_.next_line());

  }

}

void ModelBuilder::FactoryRegions(const std::string& filename) {
  //std::cout << "FUNC-ID: ModelBuilder::FactoryRegions\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  open_file(filename);
  skip_comment();
  check_blockwise();
  do {
    auto key_values = PrepareKeywords({"#name"});
    std::vector<std::string> expected_variables({"demand_electric_dyn", "demand_electric_per_a"});
    std::vector<std::string> expected_LT_variables({});
    std::vector<std::string> optional_variables({});
    if(genesys::ProgramSettings::modules().find("heat")->second){
      optional_variables.insert(optional_variables.end(), { "demand_heat_dyn", "demand_heat_per_a", "ambient_temp_dyn"});
    }
    PrepareVariables(expected_variables,
                     expected_LT_variables,
                     {"#primary_energy", "#converter", "#multi-converter", "#storage", "#endblock"},
                     optional_variables);
    auto region_variables = std::move(TBDvariable_cache_);
    //optional variables are not mandatory and therefore might be empty.
    std::unordered_map<std::string, std::unique_ptr<aux::TimeBasedData>> region_optional_variables= {};
    if(!TBD_optional_var_cache_.empty())
      region_optional_variables = std::move(TBD_optional_var_cache_);


    auto region_primary_energy = ParseComponentInfoLT("#primary_energy", {"potential"},
                                                      {"#primary_energy", "#converter", "#multi-converter", "#storage", "#endblock"});
    auto converter_variable = ParseComponentInfo("#converter", {"installation"},
                                                   {"#converter", "#multi-converter", "#storage", "#endblock"});
    std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                               aux::SimulationClock::time_point,
                                               aux::SimulationClock::time_point,
                                               aux::SimulationClock::duration> > region_converter;
    for (auto&& it : converter_variable) {
      auto begin = it.second->start();
      region_converter.emplace(it.first, std::make_tuple(std::move(it.second), begin,
                                                         genesys::ProgramSettings::simulation_start(),
                                                         genesys::ProgramSettings::installation_interval()));
    }
    auto multi_conv_var = ParseComponentInfo("#multi-converter", {"installation"},
                                                         {"#multi-converter", "#storage", "#endblock"});
    std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                               aux::SimulationClock::time_point,
                                               aux::SimulationClock::time_point,
                                               aux::SimulationClock::duration> > region_multi_conv;
    for (auto&& it : multi_conv_var) {
      auto begin = it.second->start();
      region_multi_conv.emplace(it.first, std::make_tuple(std::move(it.second), begin,
                                                         genesys::ProgramSettings::simulation_start(),
                                                         genesys::ProgramSettings::installation_interval()));
    }
    auto storage_variable = ParseComponentInfo("#storage", {"installation"}, {"#storage", "#endblock"});
    std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                               aux::SimulationClock::time_point,
                                               aux::SimulationClock::time_point,
                                               aux::SimulationClock::duration> > region_storage;
    for (auto&& it : storage_variable) {
      auto begin = it.second->start();
      region_storage.emplace(it.first, std::make_tuple(std::move(it.second), begin,
                                                       genesys::ProgramSettings::simulation_start(),
                                                       genesys::ProgramSettings::installation_interval()));
    }
    if(genesys::ProgramSettings::modules().find("heat")->second && !region_optional_variables.empty()){
     //check requirements of oil/gas boiler and primary source for delivering residual heat demand in all cases.
     if(region_multi_conv.find("GAS_HEATER") == region_multi_conv.end()){
       std::cerr << "NO - GAS_HEATER - found in " <<  key_values[0]
                 << " -- required for residual heat demand coverage"<< std::endl;
       std::terminate();
     }
      region_prototype_cache_.emplace(key_values[0], std::unique_ptr<builder::RegionPrototype>(
          new builder::RegionPrototype(key_values[0],
                                       key_values[1],
                                       std::move(region_variables.find(expected_variables[0])->second),
                                       std::move(region_variables.find(expected_variables[1])->second),
                                       std::move(region_primary_energy),
                                       std::move(region_converter),
                                       std::move(region_storage),
                                       std::move(region_multi_conv),
                                       std::move(region_optional_variables.find(optional_variables[0])->second),
                                       std::move(region_optional_variables.find(optional_variables[1])->second),
                                       std::move(region_optional_variables.find(optional_variables[2])->second),
                                       true))); //module flag for heat
      std::cout << "Region " << key_values[0] << " has activated heat module" << std::endl;

    } else if (genesys::ProgramSettings::modules().find("heat")->second && region_optional_variables.empty()){
      std::cerr << "***WARNING module heat activated, but region_optional_variables.empty() in region " << key_values[0] << std::endl;
      std::cerr << "***WARNING heat not considered in region " << key_values[0] << std::endl;
      region_prototype_cache_.emplace(key_values[0], std::unique_ptr<builder::RegionPrototype>(
          new builder::RegionPrototype(key_values[0], key_values[1],
                                       std::move(region_variables.find(expected_variables[0])->second),
                                       std::move(region_variables.find(expected_variables[1])->second),
                                       std::move(region_primary_energy), std::move(region_converter),
                                       std::move(region_storage),
                                       std::move(region_multi_conv))));
    } else {
    region_prototype_cache_.emplace(key_values[0], std::unique_ptr<builder::RegionPrototype>(
        new builder::RegionPrototype(key_values[0], key_values[1],
                                     std::move(region_variables.find(expected_variables[0])->second),
                                     std::move(region_variables.find(expected_variables[1])->second),
                                     std::move(region_primary_energy), std::move(region_converter),
                                     std::move(region_storage),
                                     std::move(region_multi_conv))));
    }
  } while (file_.next_line());
}

void ModelBuilder::FactoryLinks(const std::string& filename) {
  //std::cout << "FUNC-ID: ModelBuilder::FactoryLinks\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  open_file(filename);
  skip_comment();
  //check_blockwise can return false if #empty-flag is found
  if (check_blockwise() ){
	  do {
		auto key_values = PrepareKeywords({"#region_A", "#region_B"});
		std::vector<std::string> expected_variables({"length"});
		std::vector<std::string> expected_LT_variables({});
		PrepareVariables(expected_variables, expected_LT_variables, {"#converter", "#endblock"});
		  auto link_variables = std::move(TBDvariable_cache_);
		  auto converter_variable = ParseComponentInfo("#converter", {"installation"},
																{"#converter", "#endblock"});
		  std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
													 aux::SimulationClock::time_point,
													 aux::SimulationClock::time_point,
													 aux::SimulationClock::duration,
													 model::TransmConverterType> > link_converter;
		  for (auto&& it : converter_variable) {
			auto begin = it.second->start();
			link_converter.emplace(it.first, std::make_tuple(std::move(it.second), begin,
															 genesys::ProgramSettings::simulation_start(),
															 genesys::ProgramSettings::installation_interval(),
															 model::TransmConverterType::BIDIRECTIONAL));
		  }
		  link_prototype_cache_.emplace(key_values[0], std::unique_ptr<builder::LinkPrototype>(
			  new builder::LinkPrototype(key_values[0], key_values[1], key_values[2],
										 std::move(link_variables.find(expected_variables[0])->second),
										 std::move(link_converter))));
	  } while (file_.next_line());
  } else {

  }
}

void ModelBuilder::FactoryGlobals(const std::string& filename) {
    //std::cout << "FUNC-ID: ModelBuilder::GlobalsFactory\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
	  open_file(filename);
	  skip_comment();
	  check_blockwise();
	  do {
	    auto key_values = PrepareKeywords({"#name"});
	    std::vector<std::string> expected_variables({});
	    std::vector<std::string> expected_LT_variables({});
	    PrepareVariables(expected_variables, expected_LT_variables,
	                                             {"#primary_energy", "#converter", "#multi-converter", "#storage", "#endblock"});
    	//primary energy potentials for global reservoir
		auto global_primary_energy = ParseComponentInfoLT("#primary_energy", {"potential"},
														{"#primary_energy", "#storage", "#endblock"});
		//storage reservoir for global
		auto storage_variable = ParseComponentInfo("#storage", {"installation"}, {"#storage", "#endblock"});
		std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
		                                           aux::SimulationClock::time_point,
		                                           aux::SimulationClock::time_point,
		                                           aux::SimulationClock::duration> > global_storage;
		for (auto&& it : storage_variable) {
		        auto begin = it.second->start();
		        global_storage.emplace(it.first, std::make_tuple(std::move(it.second), begin,
		                                                         genesys::ProgramSettings::simulation_start(),
		                                                         genesys::ProgramSettings::installation_interval()));
		}
		global_proto_.emplace(key_values[0], std::unique_ptr<builder::GlobalProto>(
                                                     new builder::GlobalProto(key_values[0],
                                                                  key_values[1],
                                                                  std::move(global_primary_energy),
                                                                  std::move(global_storage))));
	} while (file_.next_line());
}

std::vector<std::string> ModelBuilder::PrepareKeywords(std::vector<std::string>&& expected_keywords_except_code) {
  //std::cout << "FUNC-ID: ModelBuilder::PrepareKeywords\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  //std::cout << "DEBUG: \texpected keys:" << std::endl;
  //for (auto& i : expected_keywords_except_code)
  //   std::cout << "\t\tkey: "<< i << std::endl;
  std::vector<std::string> return_strings;
  skip_comment();
  auto keys_value_pairs = ParseKeywords();
  auto search_code = keys_value_pairs.find("#code");
  if (search_code != keys_value_pairs.end()) {
    return_strings.emplace_back(search_code->second);
    for (const auto& i : expected_keywords_except_code) {
      auto search_key = keys_value_pairs.find(i);
      if (search_key != keys_value_pairs.end()) {
        return_strings.emplace_back(search_key->second);
      } else {
        IssueError("PrepareKeywords", "missing keyword " + i + " in file " + file_.filename()
                   + ", line " + file_.current_line());
      }
    }
  } else {
    IssueError("PrepareKeywords", "expected keyword #code in file " + file_.filename()
               + ", line " + file_.current_line());
  }
  return return_strings;
}

void ModelBuilder::PrepareVariables(const std::vector<std::string>& expected_variables,
                                    const std::vector<std::string>& expected_LT_variables,
                                    std::vector<std::string>&& block_delimiter,
                                    std::vector<std::string> optional_variables) {
  //std::cout << "FUNC-ID: ModelBuilder::PrepareVariables\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-4)<<std::endl;
  //  if (!optional_variables.empty()){
  //    std::cout << "optional variables present" << std::endl;
  //    for(auto& i : optional_variables)
  //      std::cout << "\t opt_var: " << i << std::endl;
  //  }
  if (!file_.next_line()) {
    IssueError("PrepareVariables", "expected more lines in file " + file_.filename()
        + " after line " + file_.current_line());
  }
  if (!optional_variables.empty()){
    ProcessVariablesUpTo(std::forward<std::vector<std::string> >(block_delimiter), optional_variables);
  } else {
    ProcessVariablesUpTo(std::forward<std::vector<std::string> >(block_delimiter));
  }
  if (TBDvariable_cache_.find("internal_rate_of_return") == TBDvariable_cache_.end())
    TBDvariable_cache_.emplace("internal_rate_of_return", std::unique_ptr<aux::TimeBasedData>(
        new aux::DateValuePairsConst(std::vector<std::pair<aux::SimulationClock::time_point, double> >(1,
        std::make_pair(aux::SimulationClock::time_point_from_string("1900-01-01_00:00"),
                       genesys::ProgramSettings::interest_rate())))));
  for (const auto& i : expected_variables) {
    if (TBDvariable_cache_.find(i) == TBDvariable_cache_.end() && optional_variables.empty()) {
      //std::cout << "FUNC-ID: ModelBuilder::PrepareVariables\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__)<<std::endl;
      IssueError("PrepareVariables", "missing variable " + i + " in file " + file_.filename()
                 + ", at or before line " + file_.current_line());
    }
  }
  if(!optional_variables.empty()){ // optional modules might have different requirements
    for (const auto& i : optional_variables) {
      if(genesys::ProgramSettings::modules().find("heat")->second){
        //heat is optional and can be implemented only in some of the regions
        if (TBD_optional_var_cache_.find(i) == TBD_optional_var_cache_.end()) {
          std::string message = "\t***INFO: Module heat activated but no variable found for current object\n\t\t ";
          IssueWarning("PrepareVariables", message + i + " in file " + file_.filename()
                   + ", at or before line " + file_.current_line());
        }
      } else {
        if (TBDvariable_cache_.find(i) == TBDvariable_cache_.end()) {
          std::cout << "FUNC-ID: ModelBuilder::PrepareVariables\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__)<<std::endl;
          IssueError("PrepareVariables", "missing variable " + i + " in file " + file_.filename()
                     + ", at or before line " + file_.current_line());

        }
      }
    }
  }
  for (const auto& i : expected_LT_variables) {
    if (TBDLTvariable_cache_.find(i) == TBDLTvariable_cache_.end()) {
      IssueError("PrepareVariables", "missing LT variable " + i + " for " + " in file " + file_.filename()
                 + ", at or before line " + file_.current_line());
    }
  }
}

std::unordered_map<std::string, double> ModelBuilder::PrepareMCExtraContent() {
  //std::cout << "FUNC-ID: ModelBuilder::PrepareMCExtraContent\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  std::unordered_map<std::string, double> return_map;
  if (!file_.next_line()) {
    IssueError("PrepareMCExtraContent", "expected more lines in file " + file_.filename()
        + " after line " + file_.current_line());
  }
  std::vector<std::string> output_type;
  std::vector<double> conversion_relation;
  if (file_.get_field().compare("#output") == 0) {
    while (file_.next_field()) {
      auto field = file_.get_field();
	//      if(field.find("CO2")!= std::string::npos){
	//    	  std::cout << "found co2 in ModelBuilder::PrepareMCExtraContent()" << std::endl;
	//      }
      output_type.push_back(file_.get_field());
    }
  } else {
    IssueError("PrepareMCExtraContent", "missing keyword #output in file " + file_.filename()
               + ", line " + file_.current_line());
  }
  if (output_type.empty()) {
    IssueError("PrepareMCExtraContent", "expected more fields in file after #output; " + file_.filename()
               + ", line " + file_.current_line() + " (after field" + file_.current_field() + ")");
  }
  if (!file_.next_line()) {
    IssueError("PrepareMCExtraContent", "expected more lines in file " + file_.filename()
               + " after line " + file_.current_line());
  }
  if (file_.get_field().compare("#conversion") == 0) {
    while (file_.next_field()) {
      conversion_relation.push_back(std::stod(file_.get_field()));
    }
  } else {
    IssueError("PrepareMCExtraContent", "missing keyword #conversion in file " + file_.filename()
               + ", line " + file_.current_line());
  }
  if (output_type.size() == conversion_relation.size()) {
    auto conv_rel_it = conversion_relation.cbegin();
    for (const auto& i : output_type) {
      return_map.emplace(i, *conv_rel_it++);
    }
  }
  // DEBUG: print return_map
  //  for(const auto& i : return_map)
  //	  std::cout << "return map: "<< i.first << " | " << i.second << std::endl;


  return return_map;
}

bool ModelBuilder::check_blockwise() {
  //	std::cout << "FUNC-ID: ModelBuilder::check_blockwise\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  //	std::cout << file_.get_field() << std::endl;
  if (file_.get_field().compare("#blockwise") == 0) {
    if (file_.next_line()) {
    } else {
      IssueError("check_blockwise", "expected more lines in file " + file_.filename()
          + " after line " + file_.current_line());
      return false;
    }
  } else if (file_.get_field().compare("#empty") == 0){
    std::cout << "\t***WARNING: File " << file_.filename() << " marked as #empty!" << std::endl;
    return false;
  } else {
    IssueError("check_blockwise", "expected keyword #blockwise in file " + file_.filename()
               + ", line " + file_.current_line());
    return false;
  }
  return true;
}

void ModelBuilder::skip_comment() {
  //std::cout << "FUNC-ID: ModelBuilder::skip_comment\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  if (file_.get_field().compare("#comment") == 0) {
    if (file_.next_line()) {
      // do nothing
    } else {
      IssueError("skip_comment", "expected more lines in file " + file_.filename()
          + " after line " + file_.current_line());
    }
  }
}

std::unordered_map<std::string, std::string> ModelBuilder::ParseKeywords() {
  //std::cout << "FUNC-ID: ModelBuilder::ParseKeywords\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  std::unordered_map<std::string, std::string> return_map;
  std::string key;
  do {
    key = file_.get_field();
    //std::cout << "key=" << key << " | field no: "<< file_.current_field() << std::endl;
    if (key.compare("#data") == 0)
      return return_map;
    if (file_.next_field()) {
      return_map.emplace(key, file_.get_field());
      //std::cout << "field: " << file_.get_field() << std::endl;
    } else {
      IssueError("ParseKeywords", "no value given for keyword " + key + " in file " + file_.filename()
                 + ", line " + file_.current_line() + " (after field " + file_.current_field() + ")");
    }
  } while (file_.next_field());
  //  for(auto&i : return_map)
  //    std::cout << "ModelBuilder::ParseKeywords : "<< i.first << " \t\t" << i.second << std::endl;
  return return_map;
}

std::vector<std::string> ModelBuilder::GetData(const std::unordered_map<std::string, std::string>& var_keys) {
  //std::cout << "FUNC-ID: ModelBuilder::GetData\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  std::vector<std::string> return_vector;
  auto search_data_source_path = var_keys.find("#data_source_path");
  if (search_data_source_path != var_keys.end()) {
    auto data_source_file = io_routines::CsvInput(search_data_source_path->second);
    return_vector = data_source_file.GetFieldsUpToEOL();
  } else if (file_.get_field().compare("#data") == 0) {
    if (file_.next_field()) {
      return_vector = file_.GetFieldsUpToEOL();
    } else {
      IssueError("GetData", "expected more fields in file after #data; " + file_.filename()
                 + ", line " + file_.current_line() + " (after field" + file_.current_field() + ")");
    }
  } else {
    IssueError("GetData", "missing keywords #data or #data_source_path in file " + file_.filename()
               + ", line " + file_.current_line());
  }
  return return_vector;
}

void ModelBuilder::ProcessVariablesUpTo(std::vector<std::string>&& stop_keywords, std::vector<std::string> optional_var) {
  //std::cout << "FUNC-ID: ModelBuilder::ProcessVariablesUpTo\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  TBDvariable_cache_.clear();
  TBDLTvariable_cache_.clear();
  TBD_optional_var_cache_.clear();
  do {
    bool stop_keyword_found = false;
    auto first_field = file_.get_field();
    for (auto&& i : stop_keywords){
      stop_keyword_found = stop_keyword_found || (first_field.compare(i) == 0);
    }
    if (stop_keyword_found)
      return;
    std::unordered_map<std::string, std::string> var_keys;
    if (file_.next_field()) {
      var_keys = ParseKeywords();
    } else {
      IssueError("ProcessVariablesUpTo", "expected more fields in file " + file_.filename()
                 + ", line " + file_.current_line() + " (after field" + file_.current_field() + ")");
    }
    auto search_type = var_keys.find("#type");
    if (search_type != var_keys.end()) {
      auto type_info = search_type->second;
      if (type_info.find("DVP") != std::string::npos) {
        //iterator to find optional variables in vector
        auto it = find (optional_var.begin(), optional_var.end(), first_field);
        if(!optional_var.empty() && it != optional_var.end()){
          //add to optional cache if found
          TBD_optional_var_cache_.emplace(first_field, std::move(ProcessDVPData(var_keys)));
       } else {
         TBDvariable_cache_.emplace(first_field, std::move(ProcessDVPData(var_keys)));
       }
      } else if (type_info.find("TS") != std::string::npos) {
        //iterator to find optional variables in vector
        auto it = find (optional_var.begin(), optional_var.end(), first_field);
        if(!optional_var.empty() && it != optional_var.end()){
          //add to optional cache if found
          TBD_optional_var_cache_.emplace(first_field, std::move(ProcessTSData(var_keys)));
        } else {
          TBDvariable_cache_.emplace(first_field, std::move(ProcessTSData(var_keys)));
        }
      } else if (type_info.find("lookupTable") != std::string::npos) {
        std::string data_source_path;
        auto search_data_source_path = var_keys.find("#data_source_path");
        if (search_data_source_path != var_keys.end()) {
          data_source_path = search_data_source_path->second;
        }
          TBDLTvariable_cache_.emplace(first_field, std::move(ProcessTBDLookupTableData(data_source_path)));
      } else {
        IssueError("ProcessVariablesUpTo", "unknown #type " + type_info + " in file " + file_.filename()
                   + ", line " + file_.current_line());
      }
    } else {
      IssueError("ProcessVariablesUpTo", "missing keyword #type in file " + file_.filename()
                 + ", line " + file_.current_line());
    }
  } while (file_.next_line());
  std::string keywords;
  for (auto i : stop_keywords)
    keywords += i + " ";
  IssueError("ProcessVariablesUpTo", "premature end of file, expected any of [" + keywords + "] before in file "
             + file_.filename());
}

std::unique_ptr<aux::TimeBasedData> ModelBuilder::ProcessDVPData(const std::unordered_map<std::string,
                                                                 std::string>& var_keys) {
  //std::cout << "FUNC-ID: ModelBuilder::ProcessDVPData\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-2)<<std::endl;
  auto var_data = GetData(var_keys);
  std::vector<std::pair<aux::SimulationClock::time_point, double> > dvp_data;
  bool next_entry_is_second = false;
  std::string tmp_string;
  for (auto&& i : var_data) {
    if (next_entry_is_second) {
      next_entry_is_second = false;
      dvp_data.push_back(std::pair<aux::SimulationClock::time_point, double>(
        aux::SimulationClock::time_point_from_string(tmp_string), std::stod(i)));
    } else {
      next_entry_is_second = true;
      tmp_string = i;
    }
  }
  if (next_entry_is_second) {
    IssueError("ProcessDVPData", "no data given for last timepoint - expected (one) more field(s) in file "
               + file_.filename() + ", line " + file_.current_line() + " (after field" + file_.current_field() + ")");
  }
  auto type_info = var_keys.find("#type")->second;
  if (type_info.find("const") != std::string::npos) {
    return std::unique_ptr<aux::TimeBasedData>(new aux::DateValuePairsConst(dvp_data));
  } else if (type_info.find("linear") != std::string::npos) {
    return std::unique_ptr<aux::TimeBasedData>(new aux::DateValuePairsLinear(dvp_data));
  } else {
    IssueError("ProcessDVPData", "unknown DVP sub type in " + type_info + " in file " + file_.filename()
               + ", line " + file_.current_line());
    return std::unique_ptr<aux::TimeBasedData>(new aux::TimeBasedDataDummy()); // dummy return
  }
}

std::unique_ptr<aux::TimeBasedData> ModelBuilder::ProcessTSData(const std::unordered_map<std::string,
                                                                std::string>& var_keys) {
  //std::cout << "FUNC-ID: ModelBuilder::ProcessTSData\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  auto ts_value_strings = GetData(var_keys);
  std::vector<double> ts_values;
  for (auto&& i : ts_value_strings)
    ts_values.push_back(std::stod(i));
  aux::SimulationClock::duration ts_interval;
  aux::SimulationClock::time_point ts_start;
  auto search_interval = var_keys.find("#interval");
  if (search_interval != var_keys.end()) {
      ts_interval = aux::SimulationClock::duration_from_string(search_interval->second);
  } else {
    IssueError("ProcessTSData", "missing keyword #interval in file " + file_.filename()
               + ", line " + file_.current_line());
  }
  auto search_start = var_keys.find("#start");
  if (search_start != var_keys.end()) {
    ts_start = aux::SimulationClock::time_point_from_string(search_start->second);
  } else {
    IssueError("ProcessTSData", "missing keyword #start in file " + file_.filename()
               + ", line " + file_.current_line());
  }
  auto type_info = var_keys.find("#type")->second;
  if (type_info.find("addable") != std::string::npos) {
    IssueError("ProcessTSData", "illegal TBD type - addable not allowed as input in file " + file_.filename()
               + ", line " + file_.current_line());
    return std::unique_ptr<aux::TimeBasedData>(new aux::TimeBasedDataDummy()); // dummy return
  } else if (type_info.find("repeat") != std::string::npos) {
    if (type_info.find("const") != std::string::npos) {
      return std::unique_ptr<aux::TimeBasedData>(new aux::TimeSeriesRepeatConst(ts_values, ts_start, ts_interval));
    } else if (type_info.find("linear") != std::string::npos) {
      return std::unique_ptr<aux::TimeBasedData>(new aux::TimeSeriesRepeatLinear(ts_values, ts_start, ts_interval));
    } else {
      IssueError("ProcessTSData", "unknown TS_repeat sub type in " + type_info + " in file " + file_.filename()
                 + ", line " + file_.current_line());
      return std::unique_ptr<aux::TimeBasedData>(new aux::TimeBasedDataDummy()); // dummy return
    }
  } else {
    if (type_info.find("const") != std::string::npos) {
      return std::unique_ptr<aux::TimeBasedData>(new aux::TimeSeriesConst(ts_values, ts_start, ts_interval));
    } else if (type_info.find("linear") != std::string::npos) {
      return std::unique_ptr<aux::TimeBasedData>(new aux::TimeSeriesLinear(ts_values, ts_start, ts_interval));
    } else {
      IssueError("ProcessTSData", "unknown TS sub type in " + type_info + " in file " + file_.filename()
                 + ", line " + file_.current_line());
      return std::unique_ptr<aux::TimeBasedData>(new aux::TimeBasedDataDummy()); // dummy return
    }
  }
}

aux::TBDLookupTable ModelBuilder::ProcessTBDLookupTableData(const std::string& data_source_path) {
  //std::cout << "FUNC-ID: ModelBuilder::ProcessTBDLookupTableData\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  aux::TBDLookupTable TBDLookupTable_tmp;
  bool next_entry_is_value = false;
  std::unique_ptr<aux::TimeBasedData> base;
  std::unique_ptr<aux::TimeBasedData> value;
  io_routines::CsvInput source_file; // for optional separate data file
  if (data_source_path.empty()) {
    if (!file_.next_line()) {
      IssueError("ProcessTBDLookupTableData", "expected more lines in file " + file_.filename()
          + " after line " + file_.current_line());
    }
  } else {
    source_file = io_routines::CsvInput(data_source_path);
    std::swap(file_, source_file);
  }
  skip_comment();
  do {
    auto field = file_.get_field();
    if (field.compare("#endtable") == 0) {
      if (!data_source_path.empty())
        std::swap(source_file, file_);
      return TBDLookupTable_tmp;
    }
    std::unordered_map<std::string, std::string> var_keys;
    if (file_.next_field()) {
      var_keys = ParseKeywords();
    } else {
      IssueError("ProcessTBDLookupTableData", "expected more fields in file " + file_.filename()
                 + ", line " + file_.current_line() + " (after field" + file_.current_field() + ")");
    }
    auto search_type = var_keys.find("#type");
    if (search_type != var_keys.end()) {
      auto type_info = search_type->second;
      if (type_info.find("DVP") != std::string::npos) {
        value = ProcessDVPData(var_keys);
      } else if (type_info.find("TS") != std::string::npos) {
        value = ProcessTSData(var_keys);
      } else {
        IssueError("ProcessTBDLookupTableData", "unknown #type " + type_info + " in file " + file_.filename()
                   + ", line " + file_.current_line() + " (field" + file_.current_field() + ")");
      }
    } else {
      IssueError("ProcessTBDLookupTableData", "expected keyword #type after field" + field
                 + " in file " + file_.filename() + ", line " + file_.current_line());
    }
    if ((field.compare("base") == 0) && !next_entry_is_value) {
      next_entry_is_value = true;
      base.reset(value.release());
    } else if ((field.compare("value") == 0) && next_entry_is_value) {
      next_entry_is_value = false;
      TBDLookupTable_tmp.complement(std::move(base), std::move(value));
    } else {
      std::string next_entry = next_entry_is_value ? "value" : "base"; // auto declaration not working
      IssueError("ProcessTBDLookupTableData", "expected " + next_entry + " in file " + file_.filename()
                 + ", line " + file_.current_line());
    }
  } while (file_.next_line());
  IssueError("ProcessTBDLookupTableData", "premature end of file - expected #endtable in file " + file_.filename());
  return TBDLookupTable_tmp; // dummy return to avoid warning
}

std::unordered_map<std::string, std::unique_ptr<aux::TimeBasedData> >
ModelBuilder::ParseComponentInfo(std::string type_string,
                                   std::vector<std::string>&& var_name,
                                   std::vector<std::string>&& stop_keywords) {
  //std::cout << "FUNC-ID: ModelBuilder::ParseComponentInfo\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  std::unordered_map<std::string, std::unique_ptr<aux::TimeBasedData> > cache;
  while (file_.get_field().compare(type_string) == 0) {
    if (file_.next_field()) {
      auto code_etc = ParseKeywords();
      auto search_code = code_etc.find("#code");
      if (search_code != code_etc.end()) {
        if (!file_.next_line()) {
          IssueError("ParseComponentInfo", "expected more lines in file " + file_.filename()
                     + ", after line " + file_.current_line());
        }
        ProcessVariablesUpTo(std::forward<std::vector<std::string> >(stop_keywords));
        for (const auto& i : var_name) {
          if (TBDvariable_cache_.find(i) == TBDvariable_cache_.end()) {
            IssueError("ParseComponentInfo", "missing variable " + i + " in file " + file_.filename()
                       + ", at or before line " + file_.current_line());
          }
          cache.emplace(search_code->second, std::move(TBDvariable_cache_.find(i)->second));
        }
      } else {
        IssueError("ParseComponentInfo", "expected keyword #code in file " + file_.filename()
                   + ", line " + file_.current_line() + " (field" + file_.current_field() + ")");
      }
    } else {
      IssueError("ParseComponentInfo", "expected more fields in file " + file_.filename()
                 + ", line " + file_.current_line() + " (after field" + file_.current_field() + ")");
    }
  }
  return cache;
}

std::unordered_map<std::string, aux::TBDLookupTable>
ModelBuilder::ParseComponentInfoLT(std::string type_string,
                                   std::vector<std::string>&& var_name,
                                   std::vector<std::string>&& stop_keywords) {
  //std::cout << "FUNC-ID: ModelBuilder::ParseComponentInfoLT\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  std::unordered_map<std::string, aux::TBDLookupTable> cache;
  while (file_.get_field().compare(type_string) == 0) {
    if (file_.next_field()) {
      auto code_etc = ParseKeywords();
      auto search_code = code_etc.find("#code");
      if (search_code != code_etc.end()) {
        if (!file_.next_line()) {
          IssueError("ParseComponentInfoLT", "expected more lines in file " + file_.filename()
                     + ", after line " + file_.current_line());
        }
        ProcessVariablesUpTo(std::forward<std::vector<std::string> >(stop_keywords));
        for (const auto& i : var_name) {
          if (TBDLTvariable_cache_.find(i) == TBDLTvariable_cache_.end()) {
            IssueError("ParseComponentInfoLT", "missing LT variable " + i + " in file " + file_.filename()
                       + ", at or before line " + file_.current_line());
          }
          cache.emplace(search_code->second, std::move(TBDLTvariable_cache_.find(i)->second));
        }
      } else {
        IssueError("ParseComponentInfoLT", "expected keyword #code in file " + file_.filename()
                   + ", line " + file_.current_line() + " (field" + file_.current_field() + ")");
      }
    } else {
      IssueError("ParseComponentInfoLT", "expected more fields in file " + file_.filename()
                 + ", line " + file_.current_line() + " (after field" + file_.current_field() + ")");
    }
  }
  return cache;
}

void ModelBuilder::IssueError(std::string calling_function_name,
                              std::string error_message) const {
  //std::cout << "FUNC-ID: ModelBuilder::IssueError\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  std::cerr << "ERROR in builder::ModelBuilder::" << calling_function_name << " :" << std::endl
            << error_message << std::endl;
  std::terminate();
}

void ModelBuilder::IssueWarning(std::string calling_function_name,
                                std::string error_message) const {
  //std::cout << "FUNC-ID: ModelBuilder::IssueWarning\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
  std::cerr << "***WARNING in builder::ModelBuilder::" << calling_function_name << " :" << std::endl
            << error_message << std::endl;
}

}; /* namespace builder */
