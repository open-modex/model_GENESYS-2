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
// installation_list.h
//
// This file is part of the genesys-framework v.2

#ifndef OPTIM_CMAES_INSTALLATION_LIST_H_
#define OPTIM_CMAES_INSTALLATION_LIST_H_

#include <auxiliaries/time_tools.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <io_routines/csv_input.h>
#include <optim_cmaes/installation.h>
#include <optim_cmaes/variable.h>

namespace optim_cmaes {

class InstallationList {
 public:
  InstallationList() = delete;
  ~InstallationList() = default;
  InstallationList(const io_routines::CsvInput& input_file);

  void WriteValues(const std::vector<double>& values, bool finished_optim = false);
  const std::vector<Variable>& optim_variables() const {return optim_variables_;}
  std::unordered_map<std::string,
                     std::unordered_map<std::string,
                                        std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::duration> > > installations() const;
  void Print();
 private:
  std::string GetKeyValue(std::string key);
  //void ParseAndInsertOptimVar();
  void ParseAndInsertOptimVar( std::string mounting_code, std::string tech_code, aux::SimulationClock::time_point tp);
  void InsertInstallation(const std::string& mounting_code,
                          const std::string& tech_code,
                          const std::string& type,
                          const std::vector<aux::SimulationClock::duration>& other_type_data,
                          const std::vector<std::pair<unsigned int, unsigned int> >& var_index);
  std::vector<std::string> GetData();
  void writeOptimiserResultStats();
  void IssueError(std::string calling_function_name,
                  std::string error_message) const;

  io_routines::CsvInput file_;
  std::vector<Variable> variables_; //0 index
  std::vector<Variable> optim_variables_; //1 index
  std::unordered_map<std::string, std::unordered_map<std::string, Installation> > installations_;
};

} /* namespace optim_cmaes */

#endif /* OPTIM_CMAES_INSTALLATION_LIST_H_ */
