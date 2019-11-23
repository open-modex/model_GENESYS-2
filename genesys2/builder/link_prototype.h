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
// link_prototype.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_LINK_PROTOTYPE_H_
#define BUILDER_LINK_PROTOTYPE_H_

#include <auxiliaries/time_based_data.h>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#include <builder/sys_component.h>
#include <builder/model_enums.h>

namespace builder {

class LinkPrototype : public SysComponent {
 public:
  LinkPrototype() = delete;
  virtual ~LinkPrototype() override = default;
  LinkPrototype(const LinkPrototype& other);
  LinkPrototype(LinkPrototype&& other) = default;
  LinkPrototype& operator=(const LinkPrototype&) = delete;
  LinkPrototype& operator=(LinkPrototype&&) = delete;
  LinkPrototype(const std::string& code,
                const std::string& region_A,
                const std::string& region_B,
                std::unique_ptr<aux::TimeBasedData> length,
                std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                           aux::SimulationClock::time_point,
                                                           aux::SimulationClock::time_point,
                                                           aux::SimulationClock::duration,
                                                           model::TransmConverterType> > converter_list);

 protected:
  std::string region_A() const {return region_A_;}
  std::string region_B() const {return region_B_;}
  double length(aux::SimulationClock::time_point tp) const {return (*length_)[tp];}
  std::unique_ptr<aux::TimeBasedData> length() const {return length_->clone();}
  const std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::time_point,
                                                   aux::SimulationClock::duration,
                                                   model::TransmConverterType> >& converter_list() {
    return converter_list_;
  }

 private:
  std::string region_A_;
  std::string region_B_;
  std::unique_ptr<aux::TimeBasedData> length_;
  std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::time_point,
                                             aux::SimulationClock::duration,
                                             model::TransmConverterType> > converter_list_;
};

} /* namespace am */

#endif /* BUILDER_LINK_PROTOTYPE_H_ */
