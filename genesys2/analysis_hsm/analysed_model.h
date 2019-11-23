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
// analysed_model.h
//
// This file is part of the genesys-framework v.2

#ifndef ANALYSIS_HSM_ANALYSED_MODEL_H_
#define ANALYSIS_HSM_ANALYSED_MODEL_H_

#include <map>

#include <analysis_hsm/link.h>
#include <analysis_hsm/region.h>
#include <analysis_hsm/global.h>
#include <dynamic_model_hsm/dynamic_model.h>
#include <io_routines/csv_output.h>
#include <io_routines/xml_writer.h>

namespace analysis_hsm {

class AnalysedModel: public dm_hsm::DynamicModel {
 public:
	AnalysedModel() = delete;
  virtual ~AnalysedModel() = default;
  AnalysedModel(const AnalysedModel& other) = delete;
  AnalysedModel(AnalysedModel&&) = default;
  AnalysedModel& operator=(const AnalysedModel&) = delete;
  AnalysedModel& operator=(AnalysedModel&&) = delete;
  AnalysedModel(const DynamicModel& origin);

  ///@{ Output relevant
  //  void XmlOutput(const std::string& outFile = "xml-default-output", bool  dynamic = false);
  void XmlOutput(std::unordered_map<std::string, double >fitness_results, const std::string& outFile= "xml-default-output", const bool  dynamic = false);
  ///@}
 private:
  void write_model_internal(io_routines::XmlWriter xml_out);
  std::unordered_map<std::string, std::shared_ptr<Region> > regions_;
  std::unordered_map<std::string, std::shared_ptr<Link> > links_;
  std::unordered_map<std::string, std::shared_ptr<Global> > global_;
};

} /* namespace analysis_hsm */
#endif /* ANALYSIS_HSM_ANALYSED_MODEL_H_ */
