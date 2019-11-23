/*
 * global.h
 *
 *  Created on: May 21, 2017
 *      Author: cbu_loc
 */

#ifndef ABSTRACT_MODEL_GLOBAL_H_
#define ABSTRACT_MODEL_GLOBAL_H_


#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <abstract_model/primary_energy.h>
#include <abstract_model/sys_component_active.h>
#include <builder/prim_energy_prototype.h>

#include <builder/global_proto.h>

namespace am {

class Global: public builder::GlobalProto {
public:
  Global() = delete;
  virtual ~Global() override = default;
  Global(const Global& other);
  Global(Global&& other) = default;
  Global& operator=(const Global&) = delete;
  Global& operator=(Global&&) = delete;
  Global(const builder::GlobalProto& origin,
         const std::unordered_map<std::string, std::unique_ptr<builder::PrimEnergyPrototype> >& primary_energy = {},
         const std::unordered_map<std::string, std::unique_ptr<builder::SysCompActPrototype> >& storage = {});

protected:
 const std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> >& primary_energy_ptrs() {
   return primary_energy_ptrs_;
 }
 const std::unordered_map<std::string, std::shared_ptr<SysComponentActive> >& storage_ptrs() const {
   return storage_ptrs_;
 }

private:
 std::unordered_map<std::string, std::shared_ptr<PrimaryEnergy> > primary_energy_ptrs_;
 std::unordered_map<std::string, std::shared_ptr<SysComponentActive> > storage_ptrs_;
};

} /* namespace am */

#endif /* ABSTRACT_MODEL_GLOBAL_H_ */
