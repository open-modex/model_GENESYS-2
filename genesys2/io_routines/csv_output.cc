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
// csv_output.cc
//
// This file is part of the genesys-framework v.2

#include "io_routines/csv_output.h"

#include <exception>
#include <fstream>
#include <iostream>

namespace io_routines {

void CsvOutput::push_back(const std::string& field) {
  if (lines_.empty())
    new_line();
  lines_.back().add_field(field);
}

void CsvOutput::writeToDisk(const std::string& filename) const {
  std::ofstream filestream(filename);
  if (!filestream.is_open()) {
    std::cerr << "ERROR in io_routines::CsvOutput::CsvOutput : Cannot write to " << filename << std::endl;
    std::terminate();
  } else if (lines_.empty()) {
    // do nothing
  } else {
    auto lines = lines_;
    auto last_line = lines.back();
    lines.pop_back();
    for (const auto& i : lines) {
      i.write(filestream);
      filestream << std::endl;
    }
    last_line.write(filestream);
  }
  // filestream.flush; // should be implicitly called, if not (incomplete output-file.csv), uncomment
}

} /* namespace io_routines */

