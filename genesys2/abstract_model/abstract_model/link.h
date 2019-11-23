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
// This file is part of the genesys-framework v.2

#ifndef ABSTRACT_MODEL_LINK_H_
#define ABSTRACT_MODEL_LINK_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <abstract_model/converter.h>
#include <abstract_model/region.h>
#include <builder/link_prototype.h>
#include <builder/transm_conv_prototype.h>
#include <builder/model_enums.h>

namespace am {

class Link : public builder::LinkPrototype {
 public:
	Link() = delete;
	virtual ~Link() override = default;
	Link(Link&& other) = default;
	Link& operator=(const Link&) = delete;
	Link& operator=(Link&&) = delete;
	Link(const LinkPrototype& origin,
	     const std::unordered_map<std::string, std::unique_ptr<builder::TransmConvPrototype> >& converter,
		 const std::unordered_map<std::string, std::shared_ptr<Region> >& region);
  Link(const Link& other,
       const std::unordered_map<std::string, std::shared_ptr<Region> >& region);

 protected:
  Link(const Link& other);

  const std::unordered_map<std::string, std::shared_ptr<Converter> >& converter_ptrs() const {return converter_ptrs_;}
	/* currently not needed
	std::shared_ptr<Region> region_A_ptr() {return region_A_ptr_;};
  std::shared_ptr<Region> region_B_ptr() {return region_B_ptr_;};
*/

 private:
  void RegisterRegions(const std::unordered_map<std::string, std::shared_ptr<Region> >& region);

	std::unordered_map<std::string, std::shared_ptr<Converter> > converter_ptrs_;
	std::shared_ptr<Region> region_A_ptr_;
	std::shared_ptr<Region> region_B_ptr_;
};

} /* namespace am */

#endif /* ABSTRACT_MODEL_LINK_H_ */
