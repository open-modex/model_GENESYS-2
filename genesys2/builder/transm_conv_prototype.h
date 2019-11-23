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
// transm_conv_prototype.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_TRANSM_CONV_PROTOTYPE_H_
#define BUILDER_TRANSM_CONV_PROTOTYPE_H_

#include <memory>

#include <auxiliaries/time_based_data.h>
#include <builder/converter_prototype.h>

namespace am {class Converter;}

namespace builder {

class TransmConvPrototype : public ConverterPrototype {
 public:
  friend class am::Converter;

  TransmConvPrototype() = delete;
  virtual ~TransmConvPrototype() override = default;
  TransmConvPrototype(const TransmConvPrototype& other)
      : SysCompActPrototype(other), // virtual inheritance and deleted default c'tor
        ConverterPrototype(other),
        length_dep_loss_(std::move(other.length_dep_loss_->clone())),
        length_dep_cost_(std::move(other.length_dep_cost_->clone())) {}
  TransmConvPrototype(TransmConvPrototype&& other) = default;
  TransmConvPrototype& operator=(const TransmConvPrototype&) = delete;
  TransmConvPrototype& operator=(TransmConvPrototype&&) = delete;
  TransmConvPrototype(const std::string& code,
                      const std::string& name,
                      const std::string& source,
                      const std::string& sink,
                      bool bidirectional,
                      std::unique_ptr<aux::TimeBasedData> efficiency_new,
                      std::unique_ptr<aux::TimeBasedData> cost,
                      std::unique_ptr<aux::TimeBasedData> lifetime,
                      std::unique_ptr<aux::TimeBasedData> internal_rate_of_return,
                      std::unique_ptr<aux::TimeBasedData> OaM_rate,
                      std::unique_ptr<aux::TimeBasedData> length_dep_loss,
                      std::unique_ptr<aux::TimeBasedData> length_dep_cost)
      : SysCompActPrototype(code, name, source, sink, std::move(efficiency_new), std::move(cost),
                            std::move(lifetime), std::move(internal_rate_of_return), std::move(OaM_rate)), // diamond problem and deleted default c'tor
        ConverterPrototype(*this, bidirectional),
        length_dep_loss_(std::move(length_dep_loss)),
        length_dep_cost_(std::move(length_dep_cost)) {}

 protected:
  std::unique_ptr<aux::TimeBasedData> length_dep_loss_TBD() const {return length_dep_loss_->clone();}
  std::unique_ptr<aux::TimeBasedData> length_dep_cost_TBD() const {return length_dep_cost_->clone();}

 private:
  std::unique_ptr<aux::TimeBasedData> length_dep_loss_;
  std::unique_ptr<aux::TimeBasedData> length_dep_cost_;
};

} /* namespace builder */

#endif /* BUILDER_TRANSM_CONV_PROTOTYPE_H_ */
