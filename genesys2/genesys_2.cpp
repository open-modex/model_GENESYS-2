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


#include <iostream> //std::cout
#include <memory> //smart_ptr

#include <omp.h> //openmp directives

#include <cmd_parameters.h>
#include <program_settings.h>
#include <abstract_model/abstract_model.h>
#include <analysis_hsm/hsm_analysis.h>
#include <builder/model_builder.h>
#include <optim_cmaes/cma_connect.h>
#include <optim_cmaes/installation_list.h>
#include <io_routines/xml_writer.h>

#include <auxiliaries/time_tools.h>
#include <version.h>


int main( int argc, const char* argv[]) {
  std::cout << " The new GENESYS-2 Framework by RWTH-Aachen University \n Contact us via genesys(a)isea.rwth-aachen.de\n"
            << " Version " << VERSION_STRING<< " compiled on " << __DATE__ << "\n"
            << " ----------------------------------------------------------------------------------------\n"
            << " This software is licensed under GNU Lesser General Public License.\n"
            << " For details please consult our website: http://www.genesys.rwth-aachen.de\n"
            << " ----------------------------------------------------------------------------------------\n"
            << "Let's Go!\n\n\n" << std::endl;

//First Input are  the command line arguments to choose --mode= and --threads=//
genesys::CmdParameters MyCmdParameters(argc, argv);

//Second Input is the ProgramSettings.dat file(or other .dat specified by --settings= argument//
genesys::ProgramSettings MySettings(MyCmdParameters.getProgramSettingsFile());

//Third is the initialisation of the abstract model
auto TheModel = builder::ModelBuilder().Create();

std::cout << "GENESYS-2 running in mode: " << MyCmdParameters.Mode() << std::endl;
std::cout << "\t--->Scenario =" << genesys::CmdParameters::GetScenarioName() << std::endl;

if (MyCmdParameters.Mode() == "optimisation") {
  //use the given optimisation algorithm to calculate system evolution
  optim_cmaes::CMA_connect MyCMA_Connect("InstallationList.csv", TheModel);
  std::cout << "Number of CPU-threads used = " << omp_get_max_threads() << std::endl;
  MyCMA_Connect.RunOptimiser();
  analysis_hsm::HSMAnalysis MyAnalysis(MyCMA_Connect.installation_list(), TheModel);
  MyAnalysis.RunAnalysis(MyCmdParameters.OutputFile());
}

if (MyCmdParameters.Mode() == "analysis") {
  analysis_hsm::HSMAnalysis MyAnalysis(optim_cmaes::InstallationList(MyCmdParameters.InputFile()), TheModel);
  MyAnalysis.RunAnalysis(MyCmdParameters.OutputFile());
}
if (MyCmdParameters.Mode() == "test") {
  std::cout << "test mode - doing nothing for now!" << std::endl;
  aux::print_map_elements(genesys::ProgramSettings::modules());

//  std::cout << MySettings.modules().find("heat")->second << std::endl;
//  std::cout << MySettings.modules().find("dc_grid")->second << std::endl;//
//  std::cout << genesys::ProgramSettings::modules().size() << std::endl;
   //genesys::ProgramSettings::modules2().insert({"test", false});
//  std::unordered_map<std::string, bool> test;
//  test.insert({"eins", true});
//  test.insert({"zwei", true});
//  test.insert({"drei", false});
//  aux::print_map_elements(test);
//  std::unordered_map<std::string, bool > genesys_modules_ = {std::make_pair<std::string, bool>("heat",false)};
//  aux::print_map_elements(genesys_modules_);
//  std::cout << genesys::ProgramSettings::get_modules().size() << std::endl;
  //aux::print_map_elements(genesys::ProgramSettings::modules());
  //std::cout << "map has " << genesys::ProgramSettings::modules().size() << " elements" << std::endl;
  //aux::print_map_elements(genesys::ProgramSettings::modules());
  //  io_routines::CsvInput myInput(MyCmdParameters.InputFile());
  //  optim_cmaes::InstallationList myList(myInput);
  //  analysis_hsm::HSMAnalysis MyAnalysis(myList, TheModel);
  //  MyAnalysis.RunAnalysis(MyCmdParameters.OutputFile());
}


std::cout <<"End Main Function!" << std::endl;
return (0);
}
