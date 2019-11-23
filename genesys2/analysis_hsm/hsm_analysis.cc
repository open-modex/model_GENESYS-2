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
// hsm_analysis.cc
//
// This file is part of the genesys-framework v.2


#include <analysis_hsm/hsm_analysis.h>

#include <iostream>
//for cpu timers
#include <time.h>
#include <sys/time.h>

namespace analysis_hsm {

std::unique_ptr<analysis_hsm::AnalysedModel> HSMAnalysis::RunHSMOperation() {
  std::cout << "GENESYS calculating fitness without system optimisation...please wait!" << std::endl;
     //start timeer for cpu-time calculation
     double cpu_time0 = double(std::clock())/CLOCKS_PER_SEC;
  bool analyse = true;

  if (genesys::ProgramSettings::get_operation_algorithm().compare("old_hierarchy_hsm") == 0) {
    std::cout<<"DEBUG CBU: new default setting is hsm_total_cost_min"<<std::endl;
    std::cerr<<"use new operation_algorithm !" << std::endl;
    std::cout << "FUNC-ID: HSMAnalysis::RunHSMOperation\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__)<<std::endl;
    std::terminate();
  } else if (genesys::ProgramSettings::get_operation_algorithm().compare("hsm_total_cost_min") == 0) {
    std::cout << "HSM-by total_cost_minimisation" << std::endl;
        //CalculateFitnessMinCost returns map with all results of toplevel (fitness, lcoe capex, opex etc)
    fitness_results_ = CalculateFitnessMinCost(analyse);
  } else if (genesys::ProgramSettings::get_operation_algorithm().compare("hsm_lcoe_min") == 0) {
    std::cout << "HSM-by LCOE min" << std::endl;
    // returns map with all results of toplevel (fitness, lcoe capex, opex etc)
    fitness_results_ = CalculateFitnessMinLCOE(analyse);
  } else if (genesys::ProgramSettings::get_operation_algorithm().compare("something_else") == 0) {
    std::cout << "HSM-new_algo Algorithm active!" << std::endl;
    std::cout << "FUNC-ID: HSMAnalysis::RunHSMOperation\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__)<<std::endl;
    std::cerr<<"use other option for operation_algorithm !" << std::endl;
    std::terminate();
  } else {
    std::cout << "FUNC-ID: HSMAnalysis::RunHSMOperation\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__)<<std::endl;
    std::cerr << "HSMAnalysis::RunHSMOperation() : could not identify method to calculate fitness" << std::endl;
    std::terminate();
  }
  //deprecated: fitness_results_ = CalculateFitness(analyse);
     //second timer
     double cpu_time1 = double(std::clock())/CLOCKS_PER_SEC;
     std::cout << "Analysis CPU time was: " << cpu_time1-cpu_time0 << " sec"<< std::endl;
     std::cout << "Analysis CPU time was: " << aux::pretty_time_string(1000*(cpu_time1-cpu_time0))<< std::endl;
  return std::move(std::unique_ptr<analysis_hsm::AnalysedModel> (new analysis_hsm::AnalysedModel(model())));
}

void HSMAnalysis::RunAnalysis(const std::string& out_file_static) {

  std::cout << "Writing System with fitness " << fitness() << " to file..."<< "Static"+out_file_static << std::endl;
  model_->XmlOutput(fitness_results_, out_file_static, genesys::ProgramSettings::analysis_hsm_output_detail());
}

} /* namespace analysis_hsm */
