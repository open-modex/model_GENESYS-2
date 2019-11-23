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
// cmd_parameters.h
//
// This file is part of the genesys-framework v.2

#ifndef CMD_PARAMETERS_H_
#define CMD_PARAMETERS_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <exception>
#include <map>

namespace genesys {

class CmdParameters {
public:
	CmdParameters() = delete;
	CmdParameters(int input_argc, const char* input_argv[]);
	~CmdParameters() {};

	void PrintAll();
	static std::string Mode() {return (mode_);}
	const std::string& InputFile() const {return (input_filename_);}
	static std::string GetScenarioName() {return (scenario_name_);}
	static std::string OutputFile() {return (output_filename_);}
	const std::string& getProgramSettingsFile() const { return (program_settings_file_);}
	static int availableThreads() {return (threads_);}
	static bool verbose_output() {return verbose_output_;}

private:
  void printusage(const char *prog) const;
	std::string program_settings_file_;
	static std::string mode_ ;
	static std::string output_filename_;
	static std::string input_filename_;
	static std::string scenario_name_;
	static int threads_;
	static bool verbose_output_;

};

} /* namespace genesys */

#endif /* CMD_PARAMETERS_H_ */
