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
// region.h
//
// This file is part of the genesys-framework v.2/*

#ifndef ANALYSIS_HSM_REGION_H_
#define ANALYSIS_HSM_REGION_H_

#include <map>
#include <memory>
#include <unordered_map>

#include <analysis_hsm/multi_converter.h>
#include <analysis_hsm/converter.h>
#include <analysis_hsm/link.h>
#include <analysis_hsm/primary_energy.h>
#include <analysis_hsm/storage.h>
#include <analysis_hsm/transmission_converter.h>
#include <auxiliaries/time_tools.h>
#include <dynamic_model_hsm/region.h>
#include <io_routines/xml_writer.h>

namespace analysis_hsm{

class Region : public dm_hsm::Region {
 public:
  Region() = delete;
  virtual ~Region() override = default;
  Region(const Region& other) = delete; // if this copy c'tor is needed, special implementation is necessary, taking care of correct handling of the weak pointers of the converter
  Region(Region&& other) = delete; //  if this move c'tor is needed, special implementation is necessary, taking care of correct handling of the weak pointers of the converter
  Region& operator=(const Region&) = delete;
  Region& operator=(Region&&) = delete;
  Region(const dm_hsm::Region& origin);

  /** \name Output Interface */
  ///@{
  void writeXmlRegion(io_routines::XmlWriter&, const bool dynamic_output = false);
  ///@}

 private:
  void writeXmlElement(io_routines::XmlWriter& xml_target, std::string data_code, const aux::TimeSeriesConst& dataObj);
  aux::TimeSeriesConst tscAnnualConsumption();
  aux::TimeSeriesConst tsc_annual_co2_potential();
  std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> > primary_energy_ptrs_;
  std::unordered_map<std::string, std::shared_ptr<Converter> > converter_ptrs_;
  std::unordered_map<std::string, std::shared_ptr<analysis_hsm::Storage> > storage_ptrs_;
};

} /* namespace analysis_hsm */

#endif /* ANALYSIS_HSM_REGION_H_ */
