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
// csv_input.cc
//
// This file is part of the genesys-framework v.2

#include "io_routines/csv_input.h"

#include <exception>
#include <fstream>
#include <iostream>

namespace io_routines {

CsvInput::CsvInput()
                   : filename_(""),
                     line_count_(0),
                     current_line_(0),
                     current_field_(0),
                     lines_() {}

CsvInput::CsvInput(const std::string& filename)
                   : filename_(filename),
                     line_count_(0),
                     current_line_(0),
                     current_field_(0),
                     lines_() {
  std::cout << "io_routines::CsvInput::CsvInput reading file " << filename_ << std::endl;
  std::ifstream filestream(filename_);
  if(!filestream.is_open()) {
    std::cerr << "ERROR in io_routines::CsvInput::CsvInput : Cannot read " << filename_ << std::endl;
    std::terminate();
  } else {
    std::string line;
    while (std::getline(filestream, line)) {
      //allow segmentation into sub-files:
      if (line.substr(0,9) == "/include(") {
        std::size_t fn_length = line.find_first_of(")") -9;
        std::string input_file = line.substr(9, fn_length);
        std::cout << "\tFound /include() external file:" << std::endl;
        std::cout << "\topen external file= " << input_file << std::endl;
        std::ifstream ext_fstream(input_file);
        if(!ext_fstream.is_open()) {
            std::cerr << "ERROR in io_routines::CsvInput::CsvInput : Cannot read " << filename_ << std::endl;
            std::cerr << "\tCursor position: line=" << line_count_ << std::endl;//<< " field=" << current_field()
            std::cerr << "\tERROR in external sub file: "<< input_file << std::endl;
            std::terminate();
        } else {
          std::string ext_line;
          int ext_line_count = 0;
          while (std::getline(ext_fstream, ext_line)) {
            lines_.push_back(CsvInputLine(ext_line));
            ++line_count_;
            ++ext_line_count;
          }
          std::cout << "\t\t found "<< ext_line_count << " lines in " << input_file << std::endl;
        }
      // usual read-in of line
      } else { //do not push '/include' statement to lines_
        lines_.push_back(CsvInputLine(line));
        ++line_count_;
      }
    }
  }
  if (line_count_ > 0) {
    std::cout << "\t***INFO: File contains " << line_count_ << " lines total" << std::endl;
  } else {
    std::cerr << "ERROR in io_routines::CsvInput::CsvInput : File " << filename_ << " is empty" << std::endl;
    std::terminate();
  }
}

void CsvInput::Print() const {
  std::cout << "Printing input file: " << filename_ << std::endl;
  for (CsvInputLine line : lines_) {
    line.Print();
    }
  std::cout << "------------------------------------END OF FILE---------------" << std::endl;
}

bool CsvInput::next_field() {
  if (current_field_ + 1 < lines_[current_line_].get_field_count()) {
    ++current_field_;
    return true;
  } else {
    return false;
  }
}

bool CsvInput::next_line() {
  if (current_line_ + 1 < line_count_) {
    ++current_line_;
    current_field_ = 0;
    return true;
  } else {
    return false;
  }
}

std::vector<std::string> CsvInput::GetFieldsUpToEOL() {
  std::vector<std::string> return_vector;
  do {
    return_vector.push_back(get_field());
  } while (next_field());
  return return_vector;
}

} /* namespace io_routines */
