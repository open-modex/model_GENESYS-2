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
// region_prototype.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_REGION_PROTOTYPE_H_
#define BUILDER_REGION_PROTOTYPE_H_

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <auxiliaries/time_tools.h>
#include <builder/sys_component.h>

namespace builder {

class RegionPrototype : public SysComponent {
 public:
  RegionPrototype() = delete;
  virtual ~RegionPrototype() override = default;
  RegionPrototype(const RegionPrototype& other);
  RegionPrototype(RegionPrototype&& other) = default;
  RegionPrototype& operator=(const RegionPrototype&) = delete;
  RegionPrototype& operator=(RegionPrototype&&) = delete;
  RegionPrototype(const std::string& code,
                  const std::string& name,
                  std::unique_ptr<aux::TimeBasedData> demand_electric_dyn,
                  std::unique_ptr<aux::TimeBasedData> demand_electric_per_a,
                  std::unordered_map<std::string, aux::TBDLookupTable> primary_energy_list,
                  std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                             aux::SimulationClock::time_point,
                                                             aux::SimulationClock::time_point,
                                                             aux::SimulationClock::duration> > converter_list,
                  std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                             aux::SimulationClock::time_point,
                                                             aux::SimulationClock::time_point,
                                                             aux::SimulationClock::duration> > storage_list,
                  std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                             aux::SimulationClock::time_point,
                                                             aux::SimulationClock::time_point,
                                                             aux::SimulationClock::duration> > multi_converter_list = {}, //optional
                  std::unique_ptr<aux::TimeBasedData> demand_heat_dyn = {},  //optional
                  std::unique_ptr<aux::TimeBasedData> demand_heat_per_a = {},//optional
                  std::unique_ptr<aux::TimeBasedData> ambient_temp_dyn = {},//optional
                  bool module_heat_active_ = false);//optional

 protected:
  double demand_electric(aux::SimulationClock::time_point tp) const {
    return (*demand_electric_dyn_)[tp] * (*demand_electric_per_a_)[tp];
    }
  double demand_electric_per_a(aux::SimulationClock::time_point tp) const {return (*demand_electric_per_a_)[tp];}
  double demand_heat(aux::SimulationClock::time_point tp) const;
  double demand_heat_per_a(aux::SimulationClock::time_point tp) const;
  double ambient_temp(aux::SimulationClock::time_point tp) const;
  bool module_heat_active() const {return module_heat_active_;}

  aux::TimeSeriesConst demand_electric (aux::SimulationClock::time_point start,
                                        aux::SimulationClock::time_point end) const;
  const std::unordered_map<std::string, aux::TBDLookupTable>& primary_energy_list() const {
    return primary_energy_list_; }
  const std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::duration> >& converter_list() const {
    return converter_list_;}
  const std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                     aux::SimulationClock::time_point,
                                                     aux::SimulationClock::time_point,
                                                     aux::SimulationClock::duration> >& multi_converter_list() const {
      return multi_converter_list_;}
  const std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::duration> >& storage_list() const {
    return storage_list_;}

 private:
  std::unique_ptr<aux::TimeBasedData> demand_electric_dyn_;
  std::unique_ptr<aux::TimeBasedData> demand_electric_per_a_;
  std::unique_ptr<aux::TimeBasedData> demand_heat_dyn_;
  std::unique_ptr<aux::TimeBasedData> demand_heat_per_a_;
  std::unique_ptr<aux::TimeBasedData> ambient_temp_dyn_;
  bool module_heat_active_;
  std::unordered_map<std::string, aux::TBDLookupTable> primary_energy_list_;
  std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::duration> > converter_list_;
  std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::duration> > multi_converter_list_;
  std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::duration> > storage_list_;
};

} /* namespace builder */

#endif /* BUILDER_REGION_PROTOTYPE_H_ */
