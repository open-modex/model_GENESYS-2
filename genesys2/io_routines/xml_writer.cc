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

#include <io_routines/xml_writer.h>
#include <error.h>

namespace io_routines {

XmlWriter::XmlWriter(const std::string& filename, const char& indent_identifier) :
    filename_(filename+".xml"),
    indent_identifier_(indent_identifier),
    fileIsOpen_(false),
    current_indent_(0),
    openTagCount_(0),
    openElementCount_(0),
    SubElementCount(0),
    curElementOpen_(false),
    openTags_{},
    openElements_{}
    {
      std::cout << "XML-Writer opening - Writing Prolog with version:"<<XmlWriter::versionMajor<<"."<<XmlWriter::versionMinor<<"\n";
      stream_mem_ << "<?xml version='"<<XmlWriter::versionMajor<<"."<<XmlWriter::versionMinor<<"' encoding='UTF-8' ?>\n";
}

XmlWriter::~XmlWriter() {
}

void XmlWriter::close() {
  //  std::cout << "FUNC-ID: XmlWriter::close()" << std::endl;
  if(openTagCount_ >0 || openElementCount_>0) {
    std::cerr << "Error: XmlWriter::close ->need to close open Element or Tag before file can be closed!" << std::endl;
    std::cout << openTags_.back() << " still open!" << std::endl;
    std::terminate();
  }
  writeToDisk();
  // Dialogue Routine (cbu)
  //  bool checker = false;
  //  do {
  //    std::cout << "Write xml file to disk? y|n" << std::endl;
  //    std::string answer;
  //    std::cin >> answer;
  //
  //    if (answer.compare("y") == 0) {
  //      writeToDisk();
  //      checker  = true;
  //    } else if (answer.compare("n") == 0) {
  //    std::cout << "Abort writing!" << std::endl;
  //    return;
  //    } else {
  //      std::cout << "Invalid input - retry!" << std::endl;
  //    }
  //  } while (checker == false);
}

void XmlWriter::addComment(const std::string& comment) {
  std::cout << "\tFUNC-ID: XmlWriter::addComment()" << std::endl;
  stream_mem_ << "\n";
  stream_mem_ << "<!--" << comment << "-->\n";
  stream_mem_ << "\n";
}

void XmlWriter::writeToDisk() {
  std::cout << "\tFUNC-ID: XmlWriter::writeToDisk() into file " << filename_ << std::endl;
  std::ofstream output(filename_);
  output << stream_mem_.str(); //convert to filestream
}

void XmlWriter::printToConsole(){
  std::cout << "FUNC-ID: XmlWriter::printToConsole()" << std::endl;
  std::cout << stream_mem_.str();
}


void XmlWriter::addTag(const std::string& tag) {
  if (curElementOpen_){//write closeElemet Tag
   stream_mem_ << ">\n";
   curElementOpen_ = false;
  }
  for (int i = 0; i < current_indent_; i++) {
    stream_mem_ << indent_identifier_;
  }
  stream_mem_  << "<" << tag << ">\n";
  openTags_.push_back(tag);
  ++current_indent_ ;
  openTagCount_=openTags_.size();
}


void XmlWriter::addData(const std::string& data) {
  //std::cout << "XmlWriter::addData: data-input=" << data << std::endl;
  if(openElementCount_ <1){
      std::cerr << "Error: XmlWriter::addData -> no open Element found to addData! Check syntax" << std::endl;
      std::terminate();
  }
  if (curElementOpen_){
    stream_mem_ << ">";
    curElementOpen_ = false;
  }
  stream_mem_ << data;
}

void XmlWriter::addAttr(const std::string& name, const std::string& value) {
  //std::cout << "FUNC-ID: XmlWriter::addAttr - trying to write attribute " << name << std::endl;
  if(openElementCount_<1) {
      std::cerr << "Error: XmlWriter::addAttr ->no open element detected!" << std::endl;
      std::terminate();
  }
  if (curElementOpen_) {
    stream_mem_  << " " << name << "=\"" << value << "\"";
  } else {
    std::cerr << "ERROR: cannot write XmlWriter::addAttr to closed Element!" << std::endl;
    std::terminate();
  }
}

void XmlWriter::addElement(const std::string elementTag) {
  //  std::cout << "FUNC-ID: XmlWriter::addElement" << std::endl;
  if(openTagCount_< 1){
    std::cerr << "Error: XmlWriter::addElement | need to open Tag before new element can be added!" << std::endl;
    std::terminate();
  }
  if (curElementOpen_){
    stream_mem_ << ">\n";
     curElementOpen_ = false;
     current_indent_++;
  }
  for (int i = 0; i <current_indent_; i++) {
    stream_mem_ << indent_identifier_;
  }
  openElements_.push_back(elementTag);
  openElementCount_++;
  stream_mem_ << "<" << elementTag;
  curElementOpen_ = true;
}

void XmlWriter::closeElement() {
    //std::cout << "\tFUNC-ID: XmlWriter::closeElement()" <<  openElements_.back()<< std::endl;
    if (openElementCount_< 1) {
      std::cerr << "Error: XmlWriter::closeElement -> wrong syntax - no open Element to close!" << std::endl;
      std::terminate();
    }
    if (!curElementOpen_) {
      //Write close-Statement in new line
      stream_mem_ << "\n";
      current_indent_--;
      for (int i = 0; i <current_indent_; i++) {
        stream_mem_ << indent_identifier_;
      }
      stream_mem_ << "</" << openElements_.back() << ">\n";
      openElements_.pop_back();
      openElementCount_ = openElements_.size();
    } else {
      std::cout << "***WARNING current Element still open!" << std::endl;
    }
}

void XmlWriter::addSubElement(const std::string subElement) {
  if(openTagCount_< 1 || openElementCount_ < 1){
      std::cerr << "Error: XmlWriter::addSubElement | need to open Tag and open Element before new SubElement can be added!" << std::endl;
      std::terminate();
  }
  //Add tabs for indentation
  for (int i = 0; i <current_indent_; i++) {
    stream_mem_ << indent_identifier_;
  }
  if (curElementOpen_) {
    stream_mem_ << ">\n";
    curElementOpen_ = false;
    current_indent_++;
    for (int i = 0; i <current_indent_; i++) {
      stream_mem_ << indent_identifier_;
    }
  }
  openElements_.push_back(subElement);
  openElementCount_++;
  stream_mem_ << "<" << subElement;
  curElementOpen_ = true;
}

void XmlWriter::closeSubElement() {
  if (openElementCount_< 1) {
        std::cerr << "Error: XmlWriter::closeElement -> wrong syntax - no open Element to close!" << std::endl;
        std::terminate();
  }
  //Write close-Statement in same line
  stream_mem_  << "</" << openElements_.back() << ">\n";
  openElements_.pop_back();
  openElementCount_ = openElements_.size();
  SubElementCount--;
}

void XmlWriter::closeTag() {
    if (openTagCount_ < 1){
      std::cerr << "Error: XmlWriter::closeTag ->wrong syntax - no open Tag found!" << std::endl;
      std::terminate();
    }
    for (int i = 0; i < current_indent_; i++) {
      stream_mem_ << indent_identifier_;
    }
    stream_mem_ << "</" << openTags_.back() << ">\n";
    openTags_.pop_back();
    openTagCount_ = openTags_.size();
}

void XmlWriter::addTBD(const aux::TimeSeriesConstAddable& dataObj) {
  auto data_str = dataObj.toXml();
  addAttr("start", std::get<0>(data_str));
  addAttr("interval", std::get<1>(data_str));
  addData(std::get<2>(data_str));
}

} /* namespace io_routines */
