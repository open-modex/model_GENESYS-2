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
// hsm_analysis.h
//
// This file is part of the genesys-framework v.2

#ifndef ANALYSIS_HSM_HSM_ANALYSIS_H_
#define ANALYSIS_HSM_HSM_ANALYSIS_H_

#include <memory>
#include <string>

#include <analysis_hsm/analysed_model.h>
#include <abstract_model/abstract_model.h>
#include <dynamic_model_hsm/hsm_operation.h>
#include <optim_cmaes/installation_list.h>
#include <static_model/static_model.h>

namespace analysis_hsm {

class HSMAnalysis : dm_hsm::HSMOperation {
 public:
  HSMAnalysis() = delete;
  virtual ~HSMAnalysis() = default;
  HSMAnalysis(const optim_cmaes::InstallationList& installation_list,
              const am::AbstractModel& am_model)
      : HSMOperation(sm::StaticModel(am_model, installation_list.installations())),
		fitness_results_() {

    model_ = std::move(RunHSMOperation());
  }

  void RunAnalysis(const std::string& out_file_static);
 private:
  std::unique_ptr<analysis_hsm::AnalysedModel> RunHSMOperation();
  std::unordered_map<std::string, double > fitness_results_;
  std::unique_ptr<analysis_hsm::AnalysedModel> model_;
};

} /* namespace analysis_hsm */

#endif /* ANALYSIS_HSM_HSM_ANALYSIS_H_ */
