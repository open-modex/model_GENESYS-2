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
// static_model.h
//
// This file is part of the genesys-framework v.2

#ifndef STATIC_MODEL_STATIC_MODEL_H_
#define STATIC_MODEL_STATIC_MODEL_H_

#include <memory>
#include <tuple>
#include <unordered_map>

#include <abstract_model/abstract_model.h>
#include <static_model/link.h>
#include <static_model/region.h>
#include <static_model/global.h>

namespace sm {

class StaticModel : public am::AbstractModel {
 public:
  StaticModel() = delete;
  virtual ~StaticModel() = default;
  StaticModel(const StaticModel& other);
  StaticModel(StaticModel&&) = default;
  StaticModel& operator=(const StaticModel&) = delete;
  StaticModel& operator=(StaticModel&&) = delete;
  StaticModel(const am::AbstractModel& origin,
              std::unordered_map<std::string,
                                       std::unordered_map<std::string,
                                                          std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                                     aux::SimulationClock::time_point,
                                                                     aux::SimulationClock::time_point,
                                                                     aux::SimulationClock::duration>
                                                          >
                                       > installations);

 protected:
  const std::unordered_map<std::string, std::shared_ptr<Region> >& regions() const {return regions_;}
  const std::unordered_map<std::string, std::shared_ptr<Link> >& links() const {return links_;}
  const std::unordered_map<std::string, std::shared_ptr<Global> >& global() const {return global_;}

 private:
  std::unordered_map<std::string, std::shared_ptr<Region> > regions_;
  std::unordered_map<std::string, std::shared_ptr<Link> > links_;
  std::unordered_map<std::string, std::shared_ptr<Global> > global_;
};

} /* namespace sm */

#endif /* STATIC_MODEL_STATIC_MODEL_H_ */
