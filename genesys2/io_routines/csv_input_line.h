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
// csv_input_line.h
//
// This file is part of the genesys-framework v.2

#ifndef IO_ROUTINES_CSV_INPUT_LINE_H_
#define IO_ROUTINES_CSV_INPUT_LINE_H_

#include <string>
#include <vector>

namespace io_routines {

class CsvInputLine {
public:
  typedef unsigned int index_type;

  /**
   * @brief Default constructor (deleted)
   */
  CsvInputLine() = delete;

  /**
   * @brief Constructor dividing the data stored in a string in separate fields
   *
   * @param[in] line_string String containing the data of a hole line
   */
  CsvInputLine(std::string& line_string);
  ~CsvInputLine() = default;

  /**
   * @brief Prints the content of the data stored in the fields
   */
  void Print() const;

  /**
   * @brief Gets the index number of the actual field
   *
   * @return Index number of the actual field
   */
  index_type get_field_count() const {return field_count_;}
  /**
   * @brief Gets the data of a specific field within a line
   *
   * @param[in] field_index Index of the field inside a line
   * @return Data of the field specified by the field index
   */
  std::string get_field(index_type field_index) const {return fields_[field_index];}

private:
  index_type field_count_;
  std::vector<std::string> fields_;
  void replaceAll(std::string& str, const std::string from, const std::string to, const bool to_contains_from=true);
};

} /* namespace io_routines */

#endif /* IO_ROUTINES_CSV_INPUT_LINE_H_ */
