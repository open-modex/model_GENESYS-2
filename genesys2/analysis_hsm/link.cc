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
// link.cc
//
// This file is part of the genesys-framework v.2/*

#include <analysis_hsm/link.h>

#include <iostream>

#include <analysis_hsm/region.h>
#include <dynamic_model_hsm/link.h>
#include <auxiliaries/functions.h>

namespace analysis_hsm {

Link::Link(const dm_hsm::Link& origin)
    : dm_hsm::Link(origin) {
  if (!tr_converter_ptrs_dm().empty()) {
    for (const auto& it : tr_converter_ptrs_dm()) {
          converter_ptrs_.emplace(it.first, std::shared_ptr<TransmissionConverter>
          (new TransmissionConverter(*it.second))); // static_cast is necessary because of multiple inheritance
    }
  }
}

void Link::writeXmlLink(io_routines::XmlWriter& xmlout, bool dynamic_output) {
  //std::cout << "FUNC-ID: Link::writeXmlLink" << std::endl;
  for (const auto& it : converter_ptrs_) {
    xmlout.addElement("tr-converter");
    xmlout.addAttr("code", it.first);
    it.second->writeXmlVariables(xmlout, dynamic_output);
    xmlout.closeElement();//converter
  }
}

} /* namespace analysis_hsm */
