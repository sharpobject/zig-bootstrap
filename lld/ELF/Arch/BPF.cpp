//===- BPF.cpp ------------------------------------------------------------===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "InputFiles.h"
#include "Symbols.h"
#include "Target.h"
#include "lld/Common/ErrorHandler.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;

namespace lld {
namespace elf {

namespace {
class BPF final : public TargetInfo {
public:
  BPF();
  RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
  RelType getDynRel(RelType type) const override;
  void relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const override;
};
} // namespace

BPF::BPF() {
  noneRel = R_BPF_NONE;
  relativeRel = R_BPF_64_RELATIVE;
  symbolicRel = R_BPF_64_64;
}

RelExpr BPF::getRelExpr(RelType type, const Symbol &s,
                        const uint8_t *loc) const {
  switch (type) {
    case R_BPF_64_32:
      return R_PC;
    case R_BPF_64_64:
      return R_ABS;
    default:
      error(getErrorLocation(loc) + "unrecognized reloc " + toString(type));
  }
  return R_NONE;
}

RelType BPF::getDynRel(RelType type) const {
  return type;
}

void BPF::relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const {
  switch (rel.type) {
    case R_BPF_64_32: {
      // Relocation of a symbol
      write32le(loc + 4, ((val - 8) / 8) & 0xFFFFFFFF);
      break;
    }
    case R_BPF_64_64: {
      // Relocation of a lddw instruction
      // 64 bit address is divided into the imm of this and the following
      // instructions, lower 32 first.
      write32le(loc + 4, val & 0xFFFFFFFF);
      write32le(loc + 8 + 4, val >> 32);
      break;
    }
    default:
      error(getErrorLocation(loc) + "unrecognized reloc " + toString(rel.type));
  }
}

TargetInfo *getBPFTargetInfo() {
  static BPF target;
  return &target;
}

} // namespace elf
} // namespace lld
