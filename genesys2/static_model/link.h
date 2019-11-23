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
// link.h
//
// This file is part of the genesys-framework v.2

#ifndef STATIC_MODEL_LINK_H_
#define STATIC_MODEL_LINK_H_

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include <abstract_model/link.h>
#include <static_model/converter.h>
#include <static_model/region.h>

namespace sm {

class Link : public am::Link {
 public:
  Link() = delete;
  virtual ~Link() override = default;
  Link(const Link& other);
  Link(Link&& other) = default;
  Link& operator=(const Link&) = delete;
  Link& operator=(Link&&) = delete;
  Link(const am::Link& origin,
       const std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                        aux::SimulationClock::time_point,
                                                        aux::SimulationClock::time_point,
                                                        aux::SimulationClock::duration> >& installations,
       const std::unordered_map<std::string, std::shared_ptr<Region> >& region);
  Link(const am::Link& origin,
       const std::unordered_map<std::string, std::shared_ptr<Region> >& region);

 protected:
  const std::unordered_map<std::string, std::shared_ptr<Converter> >& converter_ptrs_sm() const {return converter_ptrs_;}

 private:
  void RegisterRegions(const std::unordered_map<std::string, std::shared_ptr<Region> >& region);

  std::unordered_map<std::string, std::shared_ptr<Converter> > converter_ptrs_;
  std::shared_ptr<Region> region_A_ptr_;
  std::shared_ptr<Region> region_B_ptr_;
};

} /* namespace sm */

#endif /* STATIC_MODEL_LINK_H_ */
