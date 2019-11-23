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
// csvfile.h
//
// This file is part of the genesys-framework v.2

#ifndef IO_ROUTINES_CSV_FILE_H_
#define IO_ROUTINES_CSV_FILE_H_

#include <limits>
#include <map>
#include <vector>

#include <io_routines/csv_input.h>
#include <io_routines/csv_output.h>
#include <io_routines/csv_output_line.h>

namespace io_routines {

class CsvFile {
public:
  CsvFile() = delete;
	~CsvFile() = default;
	CsvFile(const std::string& filename)
	    : output_filename_(filename),
	      best_fitness_(std::numeric_limits<int>::max()) {}

	void ReadFromPath(const std::string& path,
	                  const std::string& filename,
	                  int numfiles);

	void ToFile();
	void WriteBestSolution() const;

private:
  void open_file(const std::string& filename) {file_ = CsvInput(filename);}

  std::string output_filename_;
	std::string path_;
  CsvOutput output_file_;
  CsvOutput output_file_best_solution_;
	CsvInput file_; /// currently processed input file
	CsvOutputLine best_solution_;
	int best_fitness_;
};

} /* namespace io_routines */

#endif /* IO_ROUTINES_CSV_FILE_H_ */
