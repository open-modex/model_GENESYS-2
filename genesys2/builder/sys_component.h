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
// sys_component.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_SYS_COMPONENT_H_
#define BUILDER_SYS_COMPONENT_H_

#include <string>

namespace builder {

class SysComponent {
 public:
  SysComponent() = delete;
  virtual ~SysComponent() = default;
  SysComponent& operator=(const SysComponent&) = delete;
  SysComponent& operator=(SysComponent&&) = delete;

  std::string code() const {return code_;}
  std::string name() const {return name_;}

 protected:
  SysComponent(const SysComponent&) = default;
  SysComponent(SysComponent&&) = default;
  SysComponent(const std::string& code,
               const std::string& name)
      : code_(code),
        name_(name) {}

 private:
  const std::string code_;
  const std::string name_;
};

} /* namespace builder */

#endif /* BUILDER_SYS_COMPONENT_H_ */
