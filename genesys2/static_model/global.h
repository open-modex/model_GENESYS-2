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
// global.h
//
// This file is part of the genesys-framework v.2

#ifndef STATIC_MODEL_GLOBAL_H_
#define STATIC_MODEL_GLOBAL_H_

#include <abstract_model/global.h>
#include <static_model/sys_component_active.h>

namespace sm {

class Global: public am::Global {
public:
	Global() = delete;
  virtual ~Global() override = default;
  Global(const Global& other);
  Global(Global&& other) = default;
  Global& operator=(const Global&) = delete;
  Global& operator=(Global&&) = delete;
  Global(const am::Global& origin,
         const std::unordered_map<std::string, std::tuple<std::unique_ptr<aux::TimeBasedData>,
                                                          aux::SimulationClock::time_point,
                                                          aux::SimulationClock::time_point,
                                                          aux::SimulationClock::duration> >& installations);
  Global(const am::Global& origin);

 protected:
  const std::unordered_map<std::string, std::shared_ptr<SysComponentActive> >& storage_ptrs_sm() const {
    return storage_ptrs_;
  }
//  const std::unordered_map<std::string, std::shared_ptr<Storage> >& storage_ptrs_sm() const {
//      return storage_ptrs_;
//    }

 private:
  std::unordered_map<std::string, std::shared_ptr<SysComponentActive> > storage_ptrs_;
  //std::unordered_map<std::string, std::shared_ptr<Storage> > storage_ptrs2_;
};

} /* namespace sm */

#endif /* STATIC_MODEL_GLOBAL_H_ */
