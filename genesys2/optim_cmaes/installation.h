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
// installation.h
//
// This file is part of the genesys-framework v.2

#ifndef OPTIM_CMAES_INSTALLATION_H_
#define OPTIM_CMAES_INSTALLATION_H_

#include <auxiliaries/time_tools.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <optim_cmaes/variable.h>

namespace optim_cmaes {

class Installation {
 public:
  enum class TBDtype {
    DVPconst,
    DVPlinear,
    TSconst,
    TSlinear,
    TSrepeatconst,
    TSrepeatlinear
  };

  Installation() = delete;
  ~Installation() = default;
  Installation(const std::string& type,
               const std::vector<aux::SimulationClock::duration>& other_type_data,
               const std::vector<std::pair<unsigned int, unsigned int> >& var_index);

  const std::vector<std::pair<unsigned int, unsigned int> >& var_index() const {return var_index_;}
  std::pair<std::unique_ptr<aux::TimeBasedData>, aux::SimulationClock::time_point>
  get(const std::vector<double>& var) const;

 private:
  void IssueError(std::string calling_function_name,
                  std::string error_message) const;

  TBDtype type_;
  std::vector<aux::SimulationClock::duration> other_type_data_;
  std::vector<std::pair<unsigned int, unsigned int> > var_index_;
};

} /* namespace optim_cmaes */

#endif /* OPTIM_CMAES_INSTALLATION_H_ */
