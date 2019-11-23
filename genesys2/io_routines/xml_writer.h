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
// xml_writer.h
//
// This file is part of the genesys-framework v.2

#ifndef IO_ROUTINES_XML_WRITER_H_
#define IO_ROUTINES_XML_WRITER_H_

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <auxiliaries/time_series_const_addable.h>


namespace io_routines {

class XmlWriter {
 public:
  XmlWriter() = delete;
  XmlWriter(const std::string& filename, const char& indent_identifier = '\t');

  virtual ~XmlWriter();
  void close();

  void addTag(const std::string& tag);
  void closeTag();

  void addElement(const std::string);
  void closeElement();
  void addSubElement(const std::string);
  void closeSubElement();
  void addAttr(const std::string& name, const std::string& value);
  void addData(const std::string& data);//needs to be templated
  void addTBD(const aux::TimeSeriesConstAddable& dataObj);

  void addComment(const std::string& comment);

  void printToConsole();
  // XML version constants
  enum {versionMajor = 1, versionMinor = 0};

 private:
  void writeToDisk();
  std::ostringstream stream_mem_;
  std::string filename_;
  char indent_identifier_;
  bool fileIsOpen_;
  int current_indent_;
  unsigned int openTagCount_;
  unsigned int openElementCount_;
  unsigned int SubElementCount;
  bool curElementOpen_;
  std::vector<std::string> openTags_;
  std::vector<std::string> openElements_;
};

} /* namespace io_routines */

#endif /* IO_ROUTINES_XML_WRITER_H_ */
