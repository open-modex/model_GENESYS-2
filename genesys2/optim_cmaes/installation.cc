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
// installation.cc
//
// This file is part of the genesys-framework v.2

#include <optim_cmaes/installation.h>

namespace optim_cmaes {

Installation::Installation(const std::string& type,
                           const std::vector<aux::SimulationClock::duration>& other_type_data,
                           const std::vector<std::pair<unsigned int, unsigned int> >& var_index)
    : other_type_data_(other_type_data),
      var_index_(var_index) {
  if (var_index_.empty()) {
    IssueError("Installation", "cannot create Installation without any data (would create empty TBD object)");
  }
  if (type.find("DVP_const") != std::string::npos) {
    type_ = TBDtype::DVPconst;
  } else if (type.find("DVP_linear") != std::string::npos) {
    type_ = TBDtype::DVPlinear;
  } else if (type.find("TS_const") != std::string::npos) {
    type_ = TBDtype::TSconst;
  } else if (type.find("TS_linear") != std::string::npos) {
    type_ = TBDtype::TSlinear;
  } else if (type.find("TS_repeatconst") != std::string::npos) {
    type_ = TBDtype::TSrepeatconst;
  } else if (type.find("TS_repeatlinear") != std::string::npos) {
    type_ = TBDtype::TSrepeatlinear;
  } else {
    IssueError("Installation", "unkown TBD #type " + type);
  }
}

std::pair<std::unique_ptr<aux::TimeBasedData>, aux::SimulationClock::time_point>
Installation::get(const std::vector<double>& var) const {
  if (var.size() != var_index_.size()) {
    IssueError("Installation", "size of var does not match expected size");
  }
  std::unique_ptr<aux::TimeBasedData> tmp_pointer;
  switch (type_) {
    case TBDtype::DVPconst :
    case TBDtype::DVPlinear : {
      std::vector<std::pair<aux::SimulationClock::time_point, double> > DVP_data;
      auto var_it = var.cbegin();
      for (const auto& i : other_type_data_)
        DVP_data.emplace_back(aux::SimulationClock::time_point(i), *var_it++);
      switch (type_) {
        case TBDtype::DVPconst :
          tmp_pointer = std::unique_ptr<aux::TimeBasedData>(new aux::DateValuePairsConst(DVP_data));
          break;
        case TBDtype::DVPlinear :
          tmp_pointer = std::unique_ptr<aux::TimeBasedData>(new aux::DateValuePairsLinear(DVP_data));
          break;
        case TBDtype::TSconst :
        case TBDtype::TSlinear :
        case TBDtype::TSrepeatconst :
        case TBDtype::TSrepeatlinear :
          break;
      }
      break;
    }
    case TBDtype::TSconst :
    case TBDtype::TSlinear :
    case TBDtype::TSrepeatconst :
    case TBDtype::TSrepeatlinear : {
      std::vector<double> TS_data;
      for (const auto& i : var)
        TS_data.push_back(i);
      switch (type_) {
        case TBDtype::DVPconst :
        case TBDtype::DVPlinear :
          break;
        case TBDtype::TSconst :
          tmp_pointer = std::unique_ptr<aux::TimeBasedData>(
              new aux::TimeSeriesConst(TS_data, aux::SimulationClock::time_point(other_type_data_[1]),
                                       other_type_data_[0]));
          break;
        case TBDtype::TSlinear :
          tmp_pointer = std::unique_ptr<aux::TimeBasedData>(
              new aux::TimeSeriesLinear(TS_data, aux::SimulationClock::time_point(other_type_data_[1]),
                                        other_type_data_[0]));
          break;
        case TBDtype::TSrepeatconst :
          tmp_pointer = std::unique_ptr<aux::TimeBasedData>(
              new aux::TimeSeriesRepeatConst(TS_data, aux::SimulationClock::time_point(other_type_data_[1]),
                                             other_type_data_[0]));
          break;
        case TBDtype::TSrepeatlinear :
          tmp_pointer = std::unique_ptr<aux::TimeBasedData>(
              new aux::TimeSeriesRepeatLinear(TS_data, aux::SimulationClock::time_point(other_type_data_[1]),
                                              other_type_data_[0]));
          break;
      }
    }
  }
  aux::SimulationClock::time_point tmp_tp(tmp_pointer->start());
  return std::make_pair(std::move(tmp_pointer), tmp_tp);
}

void Installation::IssueError(std::string calling_function_name,
                              std::string error_message) const {
  std::cerr << "ERROR in optim_cmaes::Installation::" << calling_function_name << " :" << std::endl
      << error_message << std::endl;
  std::terminate();
}

} /* namespace optim_cmaes */
