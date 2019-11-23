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
// sys_component_active.h
//
// This file is part of the genesys-framework v.2

#ifndef ABSTRACT_MODEL_SYS_COMPONENT_ACTIVE_H_
#define ABSTRACT_MODEL_SYS_COMPONENT_ACTIVE_H_

#include <cmath>
#include <memory>

#include <auxiliaries/time_tools.h>
#include <builder/sys_comp_act_prototype.h>
#include <builder/transm_conv_prototype.h>

namespace am {

class SysComponentActive : public virtual builder::SysCompActPrototype {
 public:
  SysComponentActive() = delete;
  virtual ~SysComponentActive() override = default;
  SysComponentActive(const SysComponentActive& other)
      : SysCompActPrototype(other),
        capacity_(other.capacity_),
        capex_(other.capex_),
        efficiency_(other.efficiency_),
		mean_efficiency_(0.){}
  SysComponentActive(SysComponentActive&&) = default;
  SysComponentActive& operator=(const SysComponentActive&) = delete;
  SysComponentActive& operator=(SysComponentActive&&) = delete;
  SysComponentActive(const SysCompActPrototype& origin,
                     std::unique_ptr<aux::TimeBasedData> installation,
                     aux::SimulationClock::time_point start_installation,
                     aux::SimulationClock::time_point end_installation,
                     aux::SimulationClock::duration interval_installation)
      : SysCompActPrototype(origin) {MapInstallation(std::move(installation), start_installation, end_installation,
                                                     interval_installation);}
  double efficiency(aux::SimulationClock::time_point tp) const ;

 protected:
  void MapInstallation(std::unique_ptr<aux::TimeBasedData> installation,
                       aux::SimulationClock::time_point start_installation,
                       aux::SimulationClock::time_point end_installation,
                       aux::SimulationClock::duration interval_installation = aux::years(1));
  double capacity(aux::SimulationClock::time_point tp) const {return (capacity_[tp]);}
  double capex(aux::SimulationClock::time_point tp) const {return (capex_[tp]);}
  aux::TimeSeriesConst capex(aux::SimulationClock::time_point tp_start,
                             aux::SimulationClock::time_point tp_end) const;
  double efficiency(aux::SimulationClock::time_point tp,
                    double query,
                    double offset = 0.0) const {return (efficiency_.rlookup(tp, query, offset));}
  const aux::TimeSeriesConstAddable& get_capacity_() const { return capacity_;}
  const aux::TimeSeriesConstAddable& get_capex_() const { return capex_;}
  void set_mean_efficiency(const aux::SimulationClock& clock) {mean_efficiency_ = efficiency_[clock.now()];}

 private:
  aux::TimeSeriesConstAddable capacity_;
  aux::TimeSeriesConstAddable capex_;
  aux::TBDLookupTable efficiency_;
  double mean_efficiency_;
};

} /* namespace am */

#endif /* ABSTRACT_MODEL_SYS_COMPONENT_ACTIVE_H_ */
