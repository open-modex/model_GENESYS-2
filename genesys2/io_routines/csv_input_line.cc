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
// csv_input_line.cc
//
// This file is part of the genesys-framework v.2

#include <io_routines/csv_input_line.h>

#include <exception>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace io_routines {

CsvInputLine::CsvInputLine(std::string& line_string)
                 : field_count_(0),
                   fields_() {
  //remove duplicates from editing in e.g. libre office
  std::string remove = ";;";
  std::string replace = ";";
  bool b_move_curser_if_found = false;
  replaceAll(line_string, remove, replace, b_move_curser_if_found);
  //cut stream into fields defined by single ;
  std::istringstream linestream(line_string);
  std::string field;
  while (std::getline(linestream, field, ';')) {
     fields_.push_back(field);
     ++field_count_;
  }
}

void CsvInputLine::Print() const {
  for (const auto& field : fields_) {
    std::cout << field << ";";
  }
  std::cout << std::endl;
}

void CsvInputLine::replaceAll(std::string& str_to_be_modified, const std::string from, const std::string to, const bool to_contains_from) {
  //std::cout<< "FUNC-ID = CsvInputLine::replaceAll FROM "<< __FILE__ << " LINE: " << __LINE__ << std::endl;
  try {
    if(from.empty())
      return;
    size_t start_pos = 0;
    while((start_pos = str_to_be_modified.find(from, start_pos)) != std::string::npos) {
      str_to_be_modified.replace(start_pos, from.length(), to);
      if(to_contains_from) // In case 'to' contains 'from', like replacing 'x' with 'yx' curser needs to be moved forward
        start_pos += to.length();
    }
  } catch (const std::exception& e) {
    std::cout<< "FUNC-ID = CsvInputLine::replaceAll FROM "<< __FILE__ << " LINE: " << __LINE__ << std::endl;
    std::cout << str_to_be_modified << std::endl;
    std::cerr << "ERROR in function CsvInputLine::replaceAll " <<  e.what();
    std::terminate();
  }

}
//    std::cout << "CsvInputLine::replaceAll" << std::endl;
//    if(from.empty())
//        return;
//    size_t start_pos = 0;
//    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
//        str.replace(start_pos, from.length(), to);
//        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
//    }
//    return;
//}

} /* namespace io_routines */
