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
// cmd_parameters.cc
//
// This file is part of the genesys-framework v.2

#include <cmd_parameters.h>

#include <omp.h> //openmp directives
#include <cstring>
#include <string>

namespace genesys {

//default values
std::string CmdParameters::mode_ = "operation";
std::string CmdParameters::output_filename_ = "AnalysedResult";
std::string CmdParameters::input_filename_  = "InstallationListResult.csv";
std::string CmdParameters::scenario_name_ = "default-scenario-name";
int CmdParameters::threads_ = 1;
bool CmdParameters::verbose_output_ = false;

CmdParameters::CmdParameters(int input_argc, const char* input_argv[])
    : program_settings_file_("ProgramSettings.dat") {

  if (input_argc < 2) {
    printusage(input_argv[0]);
    std::cerr << "Exiting program now..."<< std::endl;
    std::terminate();
  }

  //Reading command line parameters
	std::cout << input_argc << " cmd-arguments given."<< std::endl;
	for (int i = 1; i < input_argc; ++i) {
		//cut out char of string
		std::string SInput = input_argv[i];
		std::string SDelimiter = "=";
		std::string sParameter = SInput.substr(0, SInput.find(SDelimiter));
		std::string sValue = SInput.substr(SInput.find(SDelimiter)+1, SInput.size());
		if (sParameter == "--threads" || sParameter == "-j" || sParameter == "--j" ) {
		  std::cout << "sValue = " << sValue << std::endl;
			auto max = omp_get_max_threads();
		  if (sValue == "max" || sValue == "all" ) {
			  threads_ = max;
			} else if (std::stoi(sValue) > max ) {
			  std::cout << "usage of " << sValue << " threads requested; this system only offers " << max
			      << " threads, overriding user request" << std::endl;
        threads_ = max;
			} else if (0 < std::stoi(sValue) && std::stoi(sValue) <= max) {
			  threads_ = std::stoi(sValue);
			} else {
			  std::cerr << "ERROR in cmd_parameters: Value given for '--threads' not in allowed range" << std::endl;
			  std::terminate();
			}
		  omp_set_num_threads(threads_);
		  std::cout << "threads_ = " << threads_ << std::endl;
		}	else if (sParameter == "--mode") {
		  auto mode = sValue;
		  if (mode == "analysis"  || mode == "optimisation" || mode == "optimization" || mode == "optim" || mode=="test") {
			  if (mode == "optimization" || mode == "optim") {
				  mode_ = "optimisation";
			  } else {
		      mode_ = sValue;
			  }
		  } else {
		    std::cerr << "ERROR in cmd_parameters: Value given for '--mode' could not be recognised," << std::endl
		        << "use either 'optimisation' or 'analysis'!" << std::endl;
		    std::terminate();
		  }
		} else if (sParameter == "--settings") {
			program_settings_file_ = sValue;
		} else if (sParameter == "--input") {
		  //TODO check for valid parameter
			input_filename_ = sValue;
		} else if (sParameter == "--output") {
		  //TODO check for valid parameter
			output_filename_ = sValue;
		} else if (sParameter == "--scenario" || sParameter == "--scenario-name") {
		  //TODO check for valid parameter
			scenario_name_ = sValue;
		} else if (sParameter == "--verbose-output" || sParameter == "verbose") {
			auto verbosity = sValue;
			if (verbosity == "yes" || verbosity == "y")
				verbose_output_ = true;
			else if (verbosity == "no" || verbosity == "n")
				verbose_output_ = false;
			else {
				std::cerr << "ERROR in cmd_parameters: Value given for '--verbose' could not be recognised," << std::endl
						   << "use either 'yes' or 'no'! DEFAULT: no" << std::endl;
		        std::terminate();
			}

		} else {
			std::cerr << "Error while reading command line parameters: "<< sParameter << std::endl;
			std::terminate();
		}
	}
	PrintAll();
}

void CmdParameters::PrintAll() {
	std::cout << "======CmdParameters::PrintAll===============" << "\n"
			  << "Current setup: " << "\n"
			  << "program_settings_file_ = " <<  program_settings_file_ << "\n"
			  << "mode_ = " << mode_ << "\n"
			  << "input_filename_ = " << input_filename_ << "\n"
			  << "output_filename_ = " << output_filename_ << ".xml\n"
			  << "threads_ = " << threads_ << "\n"
			  << "scenario_name_ = " << scenario_name_ << "\n"
			  << "verbose_output_ = " << verbose_output_ << "\n"
			  << "======CmdParameters::PrintAll===============" << "\n"<< std::endl;
}

void CmdParameters::printusage(const char *prog) const {
  std::cout << "Use with options: \n"<< prog << std::endl;
  std::cout << "       --mode= <optimisation| optim | analysis : run mode >" << std::endl;
  std::cout << "       --threads= <number of threads to calculate optimisation | max | all : analysis is always running on 1 thread>" << std::endl;
  std::cout << "       --input= <input_filename of InstallationListResult.csv>" << std::endl;
  std::cout << "       --output= <output filename of analysedResult(.xml)>" << std::endl;
  std::cout << "       --settings= <filename of ProgramSettings.dat>" << std::endl;
  std::cout << "       --settings= <filename of ProgramSettings.dat>" << std::endl;
  std::cout << "       --scenario= <string name of scenario>" << std::endl;
  std::cout << "       --verbose= <yes|no : verbose output of object results in csv>" << std::endl;
}

} /* namespace genesys */

