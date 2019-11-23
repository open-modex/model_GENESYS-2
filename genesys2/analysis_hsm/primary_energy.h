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
// primary_energy.h
//
// This file is part of the genesys-framework v.2

#ifndef ANALYSIS_HSM_PRIMARY_ENERGY_H_
#define ANALYSIS_HSM_PRIMARY_ENERGY_H_

#include <abstract_model/primary_energy.h>
#include <dynamic_model_hsm/primary_energy.h>
#include <io_routines/xml_writer.h>
#include <auxiliaries/time_series_const_addable.h>

namespace analysis_hsm {

class PrimaryEnergy : public dm_hsm::PrimaryEnergy {
 public:
  PrimaryEnergy() = delete;
  virtual ~PrimaryEnergy() override = default;
  PrimaryEnergy(const PrimaryEnergy& other) = delete;
  PrimaryEnergy(PrimaryEnergy&& other) = default;
  PrimaryEnergy& operator=(const PrimaryEnergy&) = delete;
  PrimaryEnergy& operator=(PrimaryEnergy&&) = delete;
  //PrimaryEnergy(const am::PrimaryEnergy& origin)
  PrimaryEnergy(const dm_hsm::PrimaryEnergy& origin)
      : dm_hsm::PrimaryEnergy(origin) {}

  void writeDynamicVariables(io_routines::XmlWriter& xmlout);

 private:
  void writeXmlElement(io_routines::XmlWriter& xml_target, std::string data_code, const aux::TimeSeriesConstAddable& dataObj);
};

} /* namespace analysis_hsm */

#endif /* ANALYSIS_HSM_PRIMARY_ENERGY_H_ */
