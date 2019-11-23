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
// link.h
//
// This file is part of the genesys-framework v.2/*

#ifndef ANALYSIS_HSM_LINK_H_
#define ANALYSIS_HSM_LINK_H_

#include <analysis_hsm/transmission_converter.h>
#include <memory>
#include <string>
#include <unordered_map>


#include <dynamic_model_hsm/link.h>
#include <io_routines/csv_output.h>
#include <io_routines/xml_writer.h>



namespace analysis_hsm {

class Region;

class Link : public dm_hsm::Link {
 public:
  Link() = delete;
  virtual ~Link() override = default;
  Link(const Link& other) = delete; // if this copy c'tor is needed, special implementation is necessary, taking care of correct handling of the weak pointers
  Link(Link&& other) = delete; // if this move c'tor is needed, special implementation is necessary, taking care of correct handling of the weak pointers
  Link& operator=(const Link&) = delete;
  Link& operator=(Link&&) = delete;
  Link(const dm_hsm::Link& origin);

  /** \name Static Output Interface */
  ///@{
  void writeXmlLink(io_routines::XmlWriter& xmlout, bool dynamic_output);
  ///@}
 private:
  std::unordered_map<std::string, std::shared_ptr<TransmissionConverter> > converter_ptrs_;

};

} /* namespace analysis_hsm */

#endif /* ANALYSIS_HSM_LINK_H_ */
