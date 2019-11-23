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
// cma_connect.cc
//
// This file is part of the genesys-framework v.2

#include <optim_cmaes/cma_connect.h>

#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>
#include <typeinfo>
#include <unordered_map>
//for timers
#include <time.h>
#include <sys/time.h>

#include <cmd_parameters.h>
#include <auxiliaries/functions.h>
#include <dynamic_model_hsm/hsm_operation.h>
#include <io_routines/csv_output_line.h>

namespace optim_cmaes {

CMA_connect::CMA_connect(const std::string& filename,
                         const am::AbstractModel& model)
    : file_(filename),
      installation_list_(file_),
      model_(model),
      problem_dimensionality_(installation_list_.optim_variables().size()) {
	my_fitness_function_ = std::bind(&optim_cmaes::CMA_connect::MyFitnessFunction, this,
	                                 std::placeholders::_1,std::placeholders::_2);
  my_progress_function_ = std::bind(&optim_cmaes::CMA_connect::MyProgressFunction, this,
                                    std::placeholders::_1,std::placeholders::_2);
}

void CMA_connect::RunOptimiser() {
   //start timeer for cpu-time calculation
   double cpu_time0 = double(std::clock())/CLOCKS_PER_SEC;

  //std::cout << "CMA_connect::RunOptimiser()" << std::endl;
  SplitVariableVectors();
  libcmaes::GenoPheno<libcmaes::pwqBoundStrategy, libcmaes::linScalingStrategy> gp(&lbounds_.front(),
                                                                                   &ubounds_.front(),
                                                                                   problem_dimensionality_);
  ///=======================CMA Constructor=================================
  uint64_t seed = 0;
  if (genesys::ProgramSettings::use_deterministic_cmaes()) {
    seed = 42;    // 42 makes random generator deterministic
  }//else seed remains 0 -> auto generated seed from current time.
  libcmaes::CMAParameters<libcmaes::GenoPheno<libcmaes::pwqBoundStrategy, libcmaes::linScalingStrategy> >
  cmaparams(problem_dimensionality_, &init_x0_.front(), genesys::ProgramSettings::cma_init_sigma(),
            genesys::ProgramSettings::cma_lambda(), seed, gp);
  cmaparams.set_algo(sepaCMAES);
  ///=======================MULTI-THREADING ON/OFF=================================
  if (genesys::CmdParameters::availableThreads() > 1) {
    cmaparams.set_mt_feval(true); //enables multi-threading
  } else {
    cmaparams.set_mt_feval(false); //disables multi-threading
  }
  cmaparams.set_max_fevals(genesys::ProgramSettings::cma_max_fevals()); // limits the number of objective function evaluations  (individuals)
  cmaparams.set_max_iter(genesys::ProgramSettings::cma_max_iter()); // limits the number of iterations of the algorithm (generations)
  cmaparams.set_ftarget(genesys::ProgramSettings::cma_ftarget()); // stops the optimization whenever the objective function values gets below 1e-8
  //#cmaparams.set_ftarget(1e-8); // stops the optimization whenever the objective function values gets below 1e-8
  cmaparams.set_fplot("result.dat");
  ///=======================run optimizer====================================
  auto cma_solution =
      libcmaes::cmaes<libcmaes::GenoPheno<libcmaes::pwqBoundStrategy, libcmaes::linScalingStrategy> >(
          my_fitness_function_, cmaparams, my_progress_function_);
  ///=======================optimizer finished====================================
  //second timer
  double cpu_time1 = double(std::clock())/CLOCKS_PER_SEC;

  std::cout << "CMA-ES returned, optimisation took (wall-time) " << aux::pretty_time_string(cma_solution.elapsed_time()) << std::endl;
  libcmaes::Candidate best_candidate(cma_solution.get_best_seen_candidate().get_fvalue(),
                                     cma_solution.get_best_seen_candidate().get_x_pheno_dvec(cmaparams));
  writeCandidate(best_candidate.get_x());
  //DEBUG
  //  Eigen::VectorXd bestparameters = gp.pheno(cma_solution.get_best_seen_candidate().get_x_dvec());
  //  libcmaes::Candidate test_cand(cma_solution.get_best_seen_candidate().get_fvalue(), bestparameters);
  //  writeParameters(cma_solution.get_best_seen_candidate().get_fvalue(), "Result_BestParametersFitness.csv", bestparameters);
  std::cout << "finished...CMA_connect::RunOptimiser()" << std::endl;
  std::cout << "Optimisation CPU time was: " << cpu_time1-cpu_time0 << " sec"<< std::endl;
  std::cout << "Optimisation CPU time was: " << aux::pretty_time_string(1000*(cpu_time1-cpu_time0))<< std::endl;
}

double CMA_connect::MyFitnessFunction(const double *x, const int N) {
  //std::cout << "FUNC-ID: CMA_connect::MyFitnessFunction()" << std::endl;
  if (static_cast<int>(problem_dimensionality_) != N) {
    // error mismatch in variable count
  } else {
    std::vector<double> current_x;
    for (std::vector<double>::size_type i = 0; static_cast<int>(i) < N; ++i)
      current_x.push_back(x[i]);
    InstallationList tmp_inst_list(installation_list_);
    tmp_inst_list.WriteValues(current_x);
    sm::StaticModel my_model(model_, tmp_inst_list.installations());
    if (genesys::ProgramSettings::get_operation_algorithm().compare("old_hierarchy_hsm") == 0) {
      std::cout<<"DEBUG CBU: new default setting ist hsm_total_cost_min"<<std::endl;
      std::cerr<<"use new operation_algorithm !" << std::endl;
      std::cout << "FUNC-ID: MyFitnessFunction\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
      std::terminate();
      //std::cout << "HSM-Operation Algorithm active!" << std::endl;
      dm_hsm::HSMOperation hsm_operation(my_model);
      //CalculateFitness returns map with all results of toplevel (fitness, lcoe capex, opex etc)
      //analyse
      //bool analyse = false;
      //return hsm_operation.CalculateFitness(analyse).find("fitness")->second;
    } else if (genesys::ProgramSettings::get_operation_algorithm().compare("hsm_total_cost_min") == 0) {
      //std::cout << "HSM-by_total_cost_minimisation" << std::endl;
      dm_hsm::HSMOperation hsm_operation(my_model);
      //CalculateFitnessMinCost returns map with all results of toplevel (fitness, lcoe capex, opex etc)
      //analyse
      bool analyse = false;
      return hsm_operation.CalculateFitnessMinCost(analyse).find("fitness")->second;
    } else if (genesys::ProgramSettings::get_operation_algorithm().compare("hsm_lcoe_min") == 0) {
      dm_hsm::HSMOperation hsm_operation(my_model);
      bool analyse = false;
      return hsm_operation.CalculateFitnessMinLCOE(analyse).find("fitness")->second; // returns map with all results of toplevel (fitness, lcoe capex, opex etc)
    } else if (genesys::ProgramSettings::get_operation_algorithm().compare("something_else") == 0) {
          std::cout << "HSM-new_algo Algorithm active!" << std::endl;
    } else {
      std::cout << "FUNC-ID: MyFitnessFunction\n\tFROM\t" << __FILE__ << "\n\tLINE\t"<<(__LINE__-1)<<std::endl;
      std::cerr << "CMA_connect::MyFitnessFunction() : could not identify method to calculate fitness" << std::endl;
      std::terminate();
    }
  }
  return 0.0; // dummy return
}

int CMA_connect::MyProgressFunction(const libcmaes::CMAParameters<libcmaes::GenoPheno<libcmaes::pwqBoundStrategy,
                                                                  libcmaes::linScalingStrategy> >& cmaparams,
                                    const libcmaes::CMASolutions& cmasols) {
  std::cout << "generation " << cmasols.niter() << " done, took " << cmasols.elapsed_last_iter()
            << "ms, best fitness=" << cmasols.best_candidate().get_fvalue() << ", sigma=" << cmasols.sigma() << std::endl;
  std::cout << "\tbest fitness sofar= " << cmasols.get_best_seen_candidate().get_fvalue() << std::endl;

  //write result every nth iteration but not after the first=0
  if (!cmasols.niter() == 0){
	  if(cmasols.niter() % genesys::ProgramSettings::cma_niter_write_full_result() == 0){
	  	  std::cout << "Writing best candidate after niter= " << cmasols.niter() << " generations." << std::endl;
	  	  libcmaes::Candidate best_candidate(cmasols.get_best_seen_candidate().get_fvalue(),
	  			                                 cmasols.get_best_seen_candidate().get_x_pheno_dvec(cmaparams));
	  	    writeCandidate(best_candidate.get_x(), cmasols.niter());
	 }
  }
  return 0;
}

void CMA_connect::SplitVariableVectors() {
  //std::cout << "FUNC-ID: CMA_connect::SplitVariableVectors()" << std::endl;
  for(const auto& i : installation_list_.optim_variables()) {
    lbounds_.push_back(i.lbound());
    ubounds_.push_back(i.ubound());
    init_x0_.push_back(i.init_point());
  }
}// END CMA_connect::SplitVariableVector()

void CMA_connect::writeCandidate(std::vector<double> x_opt, int niter) {
	//reset input file cursor and empty previous output lines
	file_.reset_cursor_start();
	output_file_.clear();
    installation_list_.WriteValues(x_opt, true); //true activates save2disk of optim_variable statistics
     // do output to csv
     std::reverse(x_opt.begin(), x_opt.end());
     output_file_.add_line(io_routines::CsvOutputLine(file_.get_line()));
     // omitting error handling for file_.next_line() since file_ has be successfully read by InstallationList already
     unsigned int var_lines_counter = 0;
     std::string current_field = "";
     std::stringstream outstream_double;
     outstream_double.precision(std::numeric_limits<double>::max_digits10);

     while (file_.next_line()) {
       output_file_.new_line();
       var_lines_counter = 0;
       do {
         current_field = file_.get_field();
         if (current_field.find("var") != std::string::npos) {
           outstream_double << std::scientific << x_opt.back();
           output_file_.push_back(outstream_double.str());
           x_opt.pop_back();
           outstream_double.str(std::string());
           outstream_double.clear();
           ++var_lines_counter;
         } else {
           output_file_.push_back(current_field);
         }
       } while (file_.next_field());
       for (unsigned int i = 0; i < var_lines_counter; ++i){
         file_.next_line();
       }
     }
     std::string output_filename = file_.filename();
     if (niter > 0) {
    	 output_filename.insert(output_filename.find("."), "Result_niter"+std::to_string(niter));
     } else {
    	 output_filename.insert(output_filename.find("."), "Result");
     }
     output_file_.writeToDisk(output_filename);

}

} /* namespace optim_cmaes */
