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
// csv_input.h
//
// This file is part of the genesys-framework v.2

#ifndef IO_ROUTINES_CSV_INPUT_H_
#define IO_ROUTINES_CSV_INPUT_H_

#include <string>
#include <vector>

#include <io_routines/csv_input_line.h>

namespace io_routines {

class CsvInput {
public:
  typedef CsvInputLine::index_type index_type;

  /**
   * @brief Default constructor
   */
  CsvInput();

  /**
   * @brief Constructor
   *
   * @param[in] filename Name of the CsvInput-file to read
   */
  CsvInput(const std::string& filename);

  /**
   * @brief Default destructor
   */
  ~CsvInput() = default;

  /**
   * @brief Prints the Csv-File to console
   */
  void Print() const;

  void reset_cursor_start(){
	  current_line_ = 0;
	  current_field_= 0;
  }

  bool next_field();
  bool next_line();
  std::string get_field() const {return (lines_[current_line_].get_field(current_field_));}
  CsvInputLine get_line() const {return lines_[current_line_];}
  std::vector<std::string> GetFieldsUpToEOL();
  std::string filename() const {return filename_;}
  std::string current_line() const {return std::to_string(current_line_);}
  std::string current_field() const {return std::to_string(current_field_);}

private:
  std::string filename_;
  index_type line_count_;
  index_type current_line_;
  index_type current_field_;
  std::vector<CsvInputLine> lines_;
};

} /* namespace io_routines */

#endif /* IO_ROUTINES_CSV_INPUT_H_ */
