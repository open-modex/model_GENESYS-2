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
// installation_list.cc
//
// This file is part of the genesys-framework v.2

#include <optim_cmaes/installation_list.h>

#include <array>
#include <exception>
#include <iostream>

#include <program_settings.h>
#include <cmd_parameters.h>
#include <io_routines/xml_writer.h>

namespace optim_cmaes {

InstallationList::InstallationList(const io_routines::CsvInput& input_file)
    : file_(input_file) {
  if (file_.get_field().compare("#comment") == 0)
    file_.next_line();
  if (file_.get_field().compare("#empty") == 0)
	  //in case there are no optimisation results present, a #empty flag needs to be inside the InstallationListResult.csv file
	  std::cout << "INFO: found empty in InstallationList: " << file_.get_field() << std::endl;
	  return;
  unsigned int variable_index(0);
  unsigned int optim_variable_index(0);
  do {
    auto code = file_.get_field();
    auto point_pos = code.find(".");
    if (point_pos == std::string::npos) {
      IssueError("InstallationList", "expected MOUNTING_CODE.TECH_CODE in file " + file_.filename()
                 + ", line " + file_.current_line() + " (field " + file_.current_field() + ")");
    }
    auto mounting_code = code.substr(0, point_pos);
    auto tech_code = code.substr(point_pos + 1);
    if (file_.next_field()) {
      auto type = GetKeyValue("#type");
      std::vector<aux::SimulationClock::duration> other_type_data;
      std::vector<std::pair<unsigned int, unsigned int> > var_index;
      if (type.find("DVP") != std::string::npos) {
        if (file_.next_field()) {
          auto data_strings = GetData();
          auto it = data_strings.cbegin();
          do {
            other_type_data.push_back(aux::SimulationClock::time_point_from_string(*it).time_since_epoch());
            if (++it != data_strings.cend()) {
              if ((*it).find("var") != std::string::npos) {//optim variable with different set-points (time point trajectory)
                ParseAndInsertOptimVar(mounting_code, tech_code, aux::SimulationClock::time_point(other_type_data.back()));
                var_index.emplace_back(1, optim_variable_index++);
              } else {//variables_ not optimised insert fixed 'value'
                auto value = std::stod(*it);
                variables_.emplace_back(value, value, value);
                var_index.emplace_back(0, variable_index++);
              }
            } else {
              IssueError("InstallationList", "expected field after " + *--it + " in file " + file_.filename()
                         + ", line " + file_.current_line() + " (field " + file_.current_field() + ")");
            }
          } while (++it != data_strings.end());
          if ((type.find("const") == std::string::npos) && (type.find("linear") == std::string::npos)) {
            IssueError("InstallationList", "unknown DVP sub type in " + type + " in file " + file_.filename()
                       + ", at or before line " + file_.current_line());
          }
          InsertInstallation(mounting_code, tech_code, type, other_type_data, var_index);
        } else {
          IssueError("InstallationList", "expected more fields in file " + file_.filename()
                     + ", line " + file_.current_line() + " (after field " + file_.current_field() + ")");
        }
      } else if (type.find("TS") != std::string::npos) {
        if (file_.next_field()) {
          other_type_data.push_back(aux::SimulationClock::duration_from_string(GetKeyValue("#interval")));
          other_type_data.push_back(aux::SimulationClock::duration_from_string(GetKeyValue("#start")));
          if (file_.next_field()) {
            auto data_strings = GetData();
            for (const auto& i : data_strings) {
              if (i.find("var") != std::string::npos) { //optim variable with different set-points (time point trajectory)
                ParseAndInsertOptimVar(mounting_code, tech_code, aux::SimulationClock::time_point(other_type_data.back()));
                var_index.emplace_back(1, optim_variable_index++);
              } else { //variables_ not optimised insert fixed 'value'
                auto value = std::stod(i);
                variables_.emplace_back(value, value, value);
                var_index.emplace_back(0, variable_index++);
              }
            }
            if ((type.find("const") == std::string::npos) && (type.find("linear") == std::string::npos)
                && ((type.find("repeat") == std::string::npos) || (type.find("const") == std::string::npos))
                && ((type.find("repeat") == std::string::npos) || (type.find("linear") == std::string::npos))) {
              IssueError("InstallationList", "unknown TS sub type in " + type + " in file " + file_.filename()
                         + ", at or before line " + file_.current_line());
            }
            InsertInstallation(mounting_code, tech_code, type, other_type_data, var_index);
          } else {
            IssueError("InstallationList", "expected more fields in file " + file_.filename()
                       + ", line " + file_.current_line() + " (after field " + file_.current_field() + ")");
          }
        } else {
          IssueError("InstallationList", "expected fields after " + type + " in file " + file_.filename()
                     + ", line " + file_.current_line() + " (field " + file_.current_field() + ")");
        }
      } else {
        IssueError("InstallationList", "unknown #type " + type + " in file " + file_.filename()
                   + ", at or before line " + file_.current_line());
      }
    } else {
      IssueError("InstallationList", "expected keyword #type in file " + file_.filename()
                 + ", line " + file_.current_line() + " (field " + file_.current_field() + ")");
    }
  } while (file_.next_line());
  for (auto&& i : variables_)
    i.WriteValue(i.init_point());
  //DEBUG Print();
}

void InstallationList::WriteValues(const std::vector<double>& values, bool finished_optim) {
  if (values.size() == optim_variables_.size()) {
    auto value_it = values.cbegin();
    for (auto&& i : optim_variables_)
      i.WriteValue(*value_it++);
  } else {
    IssueError("WriteValues", "size mismatch for values and optim_variables");
  }
  if (finished_optim) {
    writeOptimiserResultStats();
  }

}

void InstallationList::writeOptimiserResultStats() {
  std::cout << "FUNC-ID:  InstallationList::writeOptimiserResultStats()" << std::endl;
  auto filename = genesys::CmdParameters::OutputFile();
  io_routines::XmlWriter stat_xml(filename.append("_OptimVarStats"));
  stat_xml.addTag("OPTIM_VAR_STATISTICS");
    std::string last_tech_code = "";
    for (auto&& i : optim_variables_){
      //make xml categories from tech_code
      if (last_tech_code == i.tech_code()) {
        //do not make new upper element
      } else if (last_tech_code == "") {
        last_tech_code = i.tech_code();
        //make element
        stat_xml.addElement("tech");
        stat_xml.addAttr("code", i.tech_code());
      } else {
        //close old element
        stat_xml.closeElement();
        last_tech_code = i.tech_code();
        //make element
        stat_xml.addElement("tech");
        stat_xml.addAttr("code", i.tech_code());
      }
      //write the actual variable
      stat_xml.addElement("var");
        stat_xml.addAttr("mounting_code", i.mounting_code());
        stat_xml.addAttr("tech_code", i.tech_code());
        stat_xml.addAttr("time_point", aux::SimulationClock::time_point_to_string(i.time_point()));
        stat_xml.addAttr("init",   std::to_string(i.init_point()));
        stat_xml.addAttr("lbound", std::to_string(i.lbound()));
        stat_xml.addAttr("ubound", std::to_string(i.ubound()));
        stat_xml.addData(std::to_string(i.value()));
      stat_xml.closeElement();
    }
    stat_xml.closeElement();//close the last element
  stat_xml.closeTag();//"OPTIM_VAR_STATISTICS"
  stat_xml.close();
}

std::unordered_map<std::string,
                   std::unordered_map<std::string,
                                      std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                 aux::SimulationClock::time_point,
                                                 aux::SimulationClock::time_point,
                                                 aux::SimulationClock::duration> > >
InstallationList::installations() const {
  std::unordered_map<std::string,
                     std::unordered_map<std::string,
                                        std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::duration> > > return_map;
  for (const auto& i : installations_) {
    std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                               aux::SimulationClock::time_point,
                                               aux::SimulationClock::time_point,
                                               aux::SimulationClock::duration> > sub_map;
    for (const auto& j : i.second) {
      std::vector<double> tmp_var_vec;
      for (const auto& k : j.second.var_index())
        tmp_var_vec.push_back(k.first == 0 ? variables_[k.second].value() : optim_variables_[k.second].value());
      auto installation_return = j.second.get(tmp_var_vec);
      sub_map.emplace(j.first,
                      std::move(std::tuple_cat(std::move(installation_return),
                                               std::make_pair(genesys::ProgramSettings::simulation_end(),
                                                              genesys::ProgramSettings::installation_interval()))));
    }
    return_map.emplace(i.first, std::move(sub_map));
  }
  return return_map;
}

std::string InstallationList::GetKeyValue(std::string key) {
  if (file_.get_field().find(key) != std::string::npos) {
    if (file_.next_field()) {
      return file_.get_field();
    } else {
      IssueError("GetKeyValue", "expected fields after " + key + " in file " + file_.filename()
                 + ", line " + file_.current_line() + " (field " + file_.current_field() + ")");
    }
  } else {
    IssueError("GetKeyValue", "expected keyword " + key + " in file " + file_.filename()
               + ", line " + file_.current_line() + " (field " + file_.current_field() + ")");
  }
  return ""; // dummy return
}

void InstallationList::ParseAndInsertOptimVar( std::string mounting_code, std::string tech_code, aux::SimulationClock::time_point tp) {
  //std::cout << "FUNC-ID: InstallationList::ParseAndInsertOptimVar()" << std::endl;
  if (file_.next_line()) {
    std::vector<std::string> var_strings(file_.GetFieldsUpToEOL());

    if (var_strings.size() == 4) {
      std::array<double, 3> variable_data;
      auto var_string_it = var_strings.cbegin();
      if ((*var_string_it).find("#var") != std::string::npos) {
        unsigned int array_index = 0;
        while (++var_string_it != var_strings.cend()) {
          variable_data[array_index++] = std::stod(*var_string_it);
        }
        if ((variable_data[1] <= variable_data[0]) && (variable_data[0] <= variable_data[2])) {
          //include all necessary information to Variable type - init_value, lbound, ubound, mounting, tech, timepoint
          optim_variables_.emplace_back(variable_data[0], variable_data[1], variable_data[2], mounting_code, tech_code, tp);
        } else {
          IssueError("ParseAndInsertOptimVar", "check l_bound <= init_point <= u_bound at " + file_.filename()
                     + ", line " + file_.current_line());
        }
      } else {
        IssueError("ParseAndInsertOptimVar", "expected keyword #var in first field of file " + file_.filename()
                   + ", line " + file_.current_line());
        // error expected #var... in first field of file, line
      }
    } else {
      IssueError("ParseAndInsertOptimVar", "expected 4 fields in file " + file_.filename()
                 + ", line " + file_.current_line());
    }
  } else {
    IssueError("ParseAndInsertOptimVar", "expected more lines in file " + file_.filename()
               + " after line " + file_.current_line());
  }
}

void InstallationList::InsertInstallation(const std::string& mounting_code,
                                          const std::string& tech_code,
                                          const std::string& type,
                                          const std::vector<aux::SimulationClock::duration>& other_type_data,
                                          const std::vector<std::pair<unsigned int, unsigned int> >& var_index) {
  auto prev_local_installations_pos = installations_.find(mounting_code);
  if (prev_local_installations_pos != installations_.end()) {
    if (prev_local_installations_pos->second.find(tech_code) == prev_local_installations_pos->second.end()) {
      prev_local_installations_pos->second.emplace(tech_code, Installation(type, other_type_data, var_index));
    } else {
      IssueError("InsertInstallation", "Duplicate! Found second " + mounting_code + "." + tech_code
                 + " in file " + file_.filename() + ", at or before line " + file_.current_line());
    }
  } else {
    std::unordered_map<std::string, Installation> installations_mounting;
    installations_mounting.emplace(tech_code, Installation(type, other_type_data, var_index));
    installations_.emplace(mounting_code, installations_mounting);
  }
}

std::vector<std::string> InstallationList::GetData() {
  if (file_.get_field().find("#data") != std::string::npos) {
    if (file_.next_field()) {
      return file_.GetFieldsUpToEOL();
    } else {
      IssueError("GetData", "expected fields after #data in file " + file_.filename()
                 + ", line " + file_.current_line() + " (field" + file_.current_field() + ")");
    }
  } else {
    IssueError("GetData", "expected keyword #data in file " + file_.filename() + ", line " + file_.current_line()
               + " (field" + file_.current_field() + ")");
  }
  return std::vector<std::string>(); // dummy return
}

void InstallationList::IssueError(std::string calling_function_name,
                                  std::string error_message) const {
  std::cerr << "ERROR in optim_cmaes::InstallationList::" << calling_function_name << " :" << std::endl
      << error_message << std::endl;
  std::terminate();
}

void InstallationList::Print() {
  std::cout << "InstallationList::Print()==================" << std::endl;
  for (auto& i : installations_) { //Mounting Code
    std::cout << i.first << "----------------"<< std::endl;
    for (auto& j : i.second) { //Tech Code
      std::cout <<"\t|---" << j.first << std::endl;
      std::vector<double> tmp_var_vec; //Variables*OptimVariables  Temporary Data Structure
      for (const auto& k : j.second.var_index()) {
        tmp_var_vec.push_back(k.first == 0 ? variables_[k.second].value() : optim_variables_[k.second].value());
      }
      auto installation_return = j.second.get(tmp_var_vec); //TBD Content
      std::cout << "\tTBD:" << installation_return.first->PrintToString() << "\n";
    }
  }
  std::cout << "END InstallationList::Print()==================" << std::endl;
}

} /* namespace optim_cmaes */
