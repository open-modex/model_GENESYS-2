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
// sys_component_activeS.h
//
// This file is part of the genesys-framework v.2

#ifndef STATIC_MODEL_SYS_COMPONENT_ACTIVE_H_
#define STATIC_MODEL_SYS_COMPONENT_ACTIVE_H_

#include <abstract_model/sys_component_active.h>

namespace sm {

class SysComponentActive : public virtual am::SysComponentActive {
 public:
  SysComponentActive() = delete;
  virtual ~SysComponentActive() override = default;
  SysComponentActive(const SysComponentActive&) = default;
  SysComponentActive(SysComponentActive&&) = default;
  SysComponentActive& operator=(const SysComponentActive&) = delete;
  SysComponentActive& operator=(SysComponentActive&&) = delete;
  SysComponentActive(const am::SysComponentActive& origin,
                     std::unique_ptr<aux::TimeBasedData> installation,
                     aux::SimulationClock::time_point start_installation,
                     aux::SimulationClock::time_point end_installation,
                     aux::SimulationClock::duration interval_installation)
      : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(origin) {MapInstallation(std::move(installation), start_installation, end_installation,
                                                        interval_installation);}
  SysComponentActive(const am::SysComponentActive& origin)
      : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(origin) {}
};

} /* namespace sm */

#endif /* STATIC_MODEL_SYS_COMPONENT_ACTIVE_H_ */
