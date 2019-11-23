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

#ifndef ANALYSIS_HSM_SYS_COMPONENT_ACTIVE_H_
#define ANALYSIS_HSM_SYS_COMPONENT_ACTIVE_H_

#include <auxiliaries/time_series_const.h>
#include <auxiliaries/time_series_const_addable.h>
#include <dynamic_model_hsm/sys_component_active.h>
#include <io_routines/xml_writer.h>

namespace analysis_hsm {

class SysComponentActive : public virtual dm_hsm::SysComponentActive {
 public:
  SysComponentActive() = delete;
  virtual ~SysComponentActive() = default;
  SysComponentActive& operator=(const SysComponentActive&) = delete;
  SysComponentActive& operator=(SysComponentActive&&) = delete;

  double getDiscountedValue(std::string query,
                            aux::SimulationClock::time_point start,
                            aux::SimulationClock::time_point end);

 protected:
  SysComponentActive(const SysComponentActive&) = delete;
  SysComponentActive(SysComponentActive&&) = default;
  SysComponentActive(const dm_hsm::SysComponentActive& origin)
      : builder::SysCompActPrototype(origin), // virtual inheritance and deleted default c'tor
        am::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        sm::SysComponentActive(origin), // virtual inheritance and deleted default c'tor
        dm_hsm::SysComponentActive(origin) {}

  void writeXmlElement(io_routines::XmlWriter& xml_target, std::string data_code, const aux::TimeSeriesConstAddable& dataObj);
  virtual void writeXmlVariables(io_routines::XmlWriter& xmlout, bool dynamic_output);
};

} /* namespace analysis_hsm */

#endif /* ANALYSIS_HSM_SYS_COMPONENT_ACTIVE_H_ */
