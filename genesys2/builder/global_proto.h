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
// global_proto.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_GLOBAL_PROTO_H_
#define BUILDER_GLOBAL_PROTO_H_

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <auxiliaries/time_tools.h>
#include <builder/sys_component.h>

namespace builder {

class GlobalProto : public SysComponent {
 public:
  GlobalProto() = delete;
  virtual ~GlobalProto() override = default;
  GlobalProto(const GlobalProto& other);
  GlobalProto(GlobalProto&& other) = default;
  GlobalProto& operator=(const GlobalProto&) = delete;
  GlobalProto& operator=(GlobalProto&&) = delete;
  GlobalProto(const std::string& code,
              const std::string& name,
              std::unordered_map<std::string, aux::TBDLookupTable> primary_energy_list = {},
              std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
														     aux::SimulationClock::time_point,
														     aux::SimulationClock::time_point,
														     aux::SimulationClock::duration> > storage_list = {});

 protected:
  const std::unordered_map<std::string, aux::TBDLookupTable>& primary_energy_list() const {
    return primary_energy_list_; }
  const std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::duration> >& storage_list() const {
    return storage_list_;}

 private:
  std::unordered_map<std::string, aux::TBDLookupTable> primary_energy_list_;
  std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::duration> > storage_list_;
};

} /* namespace builder */

#endif /* BUILDER_GLOBAL_PROTO_H_ */
