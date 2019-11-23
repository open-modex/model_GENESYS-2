/// ==================================================================
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
// csvfile.cpp
//
// This file is part of the genesys-framework v.2


#include <io_routines/csv_file.h>

#include <exception>
#include <iostream>

#include <cmd_parameters.h>

namespace io_routines {

void CsvFile::ReadFromPath(const std::string& path,
                           const std::string& filename,
                           int num_files) {
  path_ = path;
  std::cout << "Reading from path: " << path_ << std::endl;
	for (int file_index = 0; file_index < num_files; ++file_index ) {
	  std::string infilename =  filename + std::to_string(file_index) + ".csv";
	  open_file(path_+infilename);
	  if (file_index == 0) {
	    output_file_.add_line(CsvOutputLine(file_.get_line()));
	  }
	  while (file_.next_line()) {
	    int current_fitness = 0;
	    if (file_.next_field()) {
	      current_fitness = std::stoi(file_.get_field());
	    } else {
	      std::cerr << "Error in io_routines::CsvFile::ReadFromPath" << std::endl;
	      std::terminate();
	    }
	    output_file_.add_line(CsvOutputLine(file_.get_line()));
      if (current_fitness < best_fitness_) {
        std::cout << "found better fitness: " << current_fitness << std::endl;
        best_fitness_ = current_fitness;
        best_solution_ = CsvOutputLine(file_.get_line());
      }
		}
	}
	output_file_best_solution_.add_line(best_solution_);
}

void CsvFile::ToFile() {
	std::cout << "Writing combined_results" << std::endl;
	output_file_.writeToDisk(output_filename_);
}

void CsvFile::WriteBestSolution() const {
	std::cout << "Writing best_solution" << std::endl;
	output_file_best_solution_.writeToDisk("best_" + output_filename_);
}

} /* namespace io_routines */
