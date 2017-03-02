/*
 * Copyright 2017, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "GlobalAllocPass.h"

#include "RSAllocationUtils.h"

#include "llvm/IR/Attributes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "rs2spirv-global-alloc"

using namespace llvm;

namespace rs2spirv {

namespace {
//
// This pass would enumerate used global rs_allocations (TBD) and
// lowers calls to accessors of the following type:
//
//    rsGetAllocationDimX(g)
//
// to
//
//    __rsov_rsGetAllocationDimX(some uninque constant identifying g) */
//
// Note the __rsov_* variant is used as a marker for another SPIRIT
// transformations (see GlobalAllocSPIRITPass.cpp) to expand them into
// SPIR-V instructions that loads the metadata.
//
class GlobalAllocPass : public ModulePass {
public:
  static char ID;
  GlobalAllocPass() : ModulePass(ID) {}
  const char *getPassName() const override { return "GlobalAllocPass"; }

  bool runOnModule(Module &M) override {
    DEBUG(dbgs() << "RS2SPIRVGlobalAllocPass\n");
    DEBUG(M.dump());

    SmallVector<GlobalVariable *, 8> GlobalAllocs;
    const bool CollectRes = collectGlobalAllocs(M, GlobalAllocs);
    if (!CollectRes)
      return false; // Module not modified.

    SmallVector<RSAllocationInfo, 8> Allocs;
    SmallVector<RSAllocationCallInfo, 8> Calls;
    getRSAllocationInfo(M, Allocs);
    getRSAllocAccesses(Allocs, Calls);

    // Lower the found accessors
    for (auto &C : Calls) {
      assert(C.Kind == RSAllocAccessKind::DIMX &&
             "Unsupported type of accessor call types");
      solidifyRSAllocAccess(M, C);
    }
    // Return true, as the pass modifies module.
    DEBUG(dbgs() << "RS2SPIRVGlobalAllocPass end\n");
    return true;
  }

private:
  bool collectGlobalAllocs(Module &M,
                           SmallVectorImpl<GlobalVariable *> &GlobalAllocs) {
    for (auto &GV : M.globals()) {
      if (!isRSAllocation(GV))
        continue;

      DEBUG(GV.dump());
      GlobalAllocs.push_back(&GV);
    }

    return !GlobalAllocs.empty();
  }
};
} // namespace

char GlobalAllocPass::ID = 0;

ModulePass *createGlobalAllocPass() { return new GlobalAllocPass(); }

} // namespace rs2spirv