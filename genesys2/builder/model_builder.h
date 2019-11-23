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
// model_builder.h
//
// This file is part of the genesys-framework v.2

#ifndef BUILDER_MODEL_BUILDER_H_
#define BUILDER_MODEL_BUILDER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <builder/converter_prototype.h>
#include <builder/link_prototype.h>
#include <builder/prim_energy_prototype.h>
#include <builder/region_prototype.h>
#include <builder/sys_comp_act_prototype.h>
#include <builder/transm_conv_prototype.h>
#include <builder/multi_converter_proto.h>
#include <builder/global_proto.h>
#include <abstract_model/abstract_model.h>
#include <auxiliaries/time_tools.h>
#include <io_routines/csv_input.h>

namespace builder {

class ModelBuilder {
 public:
  ModelBuilder() = default;
  ~ModelBuilder() = default;
  ModelBuilder(const ModelBuilder&) = delete;
  ModelBuilder(ModelBuilder&&) = delete;
  ModelBuilder& operator=(const ModelBuilder&) = delete;
  ModelBuilder& operator=(ModelBuilder&&) = delete;

  am::AbstractModel Create();

 private:
  void FactoryPrimaryEnergy(const std::string& filename);
  void FactoryStorage(const std::string& filename);
  void FactoryConverter(const std::string& filename);
  void FactoryMultiConverter(const std::string& filename);
  void FactoryTransmissionConverter(const std::string& filename);
  void FactoryRegions(const std::string& filename);
  void FactoryLinks(const std::string& filename);
  void FactoryGlobals(const std::string& filename);
  std::vector<std::string> PrepareKeywords(std::vector<std::string>&& expected_keywords_except_code);
  void PrepareVariables(const std::vector<std::string>& expected_variables,
                        const std::vector<std::string>& expected_LT_variables,  //lookup-table variables
                        std::vector<std::string>&& block_delimiter,
                        std::vector<std::string> optional_variables = {});
  std::unordered_map<std::string, double> PrepareMCExtraContent();
  void open_file(const std::string& filename) {file_ = io_routines::CsvInput(filename);}
  bool check_blockwise();
  void skip_comment();
  std::unordered_map<std::string, std::string> ParseKeywords();
  std::vector<std::string> GetData(const std::unordered_map<std::string,
                                   std::string>& var_keys);
  //void ProcessVariablesUpTo(std::vector<std::string>&& stop_keywords);
  void ProcessVariablesUpTo(std::vector<std::string>&& stop_keywords,
                            std::vector<std::string> optional_var = {});
  std::unique_ptr<aux::TimeBasedData> ProcessDVPData(
      const std::unordered_map<std::string, std::string>& var_keys);
  std::unique_ptr<aux::TimeBasedData> ProcessTSData(
      const std::unordered_map<std::string, std::string>& var_keys);
  aux::TBDLookupTable ProcessTBDLookupTableData(const std::string& data_source_path);
  std::unordered_map<std::string, std::unique_ptr<aux::TimeBasedData> >
  ParseComponentInfo(std::string type_string,
                     std::vector<std::string>&& var_name,
                     std::vector<std::string>&& stop_keywords);
  std::unordered_map<std::string, aux::TBDLookupTable>
  ParseComponentInfoLT(std::string type_string,
                     std::vector<std::string>&& var_name,
                     std::vector<std::string>&& stop_keywords);
  void IssueError(std::string calling_function_name,
                  std::string error_message) const;
  void IssueWarning(std::string calling_function_name,
                    std::string error_message) const;

  io_routines::CsvInput file_; /// currently processed input file

  /// variable caches
  std::unordered_map<std::string, std::unique_ptr<aux::TimeBasedData> > TBDvariable_cache_;
  std::unordered_map<std::string, std::unique_ptr<aux::TimeBasedData> > TBD_optional_var_cache_;
  std::unordered_map<std::string, aux::TBDLookupTable> TBDLTvariable_cache_;

  /// Prototype caches
  std::unordered_map<std::string, std::unique_ptr<builder::PrimEnergyPrototype> > primary_energy_prototype_cache_;
  std::unordered_map<std::string, std::unique_ptr<builder::SysCompActPrototype> > storage_prototype_cache_;
  std::unordered_map<std::string, std::unique_ptr<builder::ConverterPrototype> > converter_prototype_cache_;
  std::unordered_map<std::string, std::unique_ptr<builder::MultiConverterProto> > multi_converter_prototype_cache_;
  std::unordered_map<std::string, std::unique_ptr<builder::TransmConvPrototype> > transm_converter_prototype_cache_;
  std::unordered_map<std::string, std::unique_ptr<builder::RegionPrototype> > region_prototype_cache_;
  std::unordered_map<std::string, std::unique_ptr<builder::LinkPrototype> > link_prototype_cache_;
  std::unordered_map<std::string, std::unique_ptr<builder::GlobalProto> > global_proto_;
};

} /* namespace builder */

#endif /* BUILDER_MODEL_BUILDER_H_ */
