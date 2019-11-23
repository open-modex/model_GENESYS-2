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
// cma_connect.h
//
// This file is part of the genesys-framework v.2

#ifndef CMA_CONNECT_H_
#define CMA_CONNECT_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <libcmaes/cmaes.h>

#include <program_settings.h>
#include <abstract_model/abstract_model.h>
#include <io_routines/csv_input.h>
#include <io_routines/csv_output.h>
#include <optim_cmaes/installation_list.h>
#include <optim_cmaes/variable.h>

namespace optim_cmaes {

class CMA_connect {
public:
	CMA_connect(const std::string& filename,
	            const am::AbstractModel& model);
	~CMA_connect() = default;
	CMA_connect(const CMA_connect&) = delete;
  CMA_connect(CMA_connect&&) = delete;
  CMA_connect& operator =(const CMA_connect&) = delete;
  CMA_connect& operator =(CMA_connect&&) = delete;

	void RunOptimiser();

  const InstallationList& installation_list() const {return installation_list_;}

private:
  double MyFitnessFunction(const double *x,
                           const int N);
  int MyProgressFunction(const libcmaes::CMAParameters<libcmaes::GenoPheno<libcmaes::pwqBoundStrategy,
                                                       libcmaes::linScalingStrategy> >& cmaparams,
                         const libcmaes::CMASolutions& cmasols);
  void SplitVariableVectors();

  void writeCandidate(std::vector<double> x_opt, int niter = 0); // intentionally copying the argument
  //void writeParameters(const double fitness, const std::string add_filename, const Eigen::VectorXd x_Vec);

  io_routines::CsvInput file_;
  InstallationList installation_list_;
  am::AbstractModel model_;
  libcmaes::FitFunc my_fitness_function_;
  libcmaes::ProgressFunc<libcmaes::CMAParameters<libcmaes::GenoPheno<libcmaes::pwqBoundStrategy,
                                                 libcmaes::linScalingStrategy> >,
                         libcmaes::CMASolutions> my_progress_function_;

  std::vector<Variable>::size_type problem_dimensionality_;
  std::vector<double> lbounds_;
  std::vector<double> ubounds_;
  std::vector<double> init_x0_;
  io_routines::CsvOutput output_file_;

  //std::vector<io_routines::CsvOutputLine> result_lines;
};

} /* namespace optim_cmaes */

#endif /* CMA_CONNECT_H_ */
