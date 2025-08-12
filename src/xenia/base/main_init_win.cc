/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2014 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/base/platform_win.h"

#include <cstdlib>

// Rosetta 2 stuff
#include <intrin.h>
#include <string.h>

// Includes Windows headers, so it goes after platform_win.h.
#include "third_party/xbyak/xbyak/xbyak_util.h"

bool is_rosetta2() {
  int cpuInfo[4];
  char brand[49] = {0};

  __cpuid(cpuInfo, 0x80000002);
  memcpy(brand, cpuInfo, sizeof(cpuInfo));
  __cpuid(cpuInfo, 0x80000003);
  memcpy(brand + 16, cpuInfo, sizeof(cpuInfo));
  __cpuid(cpuInfo, 0x80000004);
  memcpy(brand + 32, cpuInfo, sizeof(cpuInfo));
  brand[48] = '\0';

  for (char* p = brand; *p; ++p) {
    if (_strnicmp(p, "VirtualApple", 12) == 0) {
      return true;
    }
  }
  return false;
}

class StartupCpuFeatureCheck {
 public:
  StartupCpuFeatureCheck() {
    Xbyak::util::Cpu cpu;
    const char* error_message = nullptr;
    if (!cpu_.has(Xbyak::util::Cpu::tAVX) && !is_rosetta2()) {
      error_message =
          "Your CPU does not support AVX, which is required by Xenia. See "
          "the "
          "FAQ for system requirements at https://xenia.jp";
    }
    if (error_message == nullptr) {
      return;
    } else {
      // TODO(gibbed): detect app type and printf instead, if needed?
      MessageBoxA(nullptr, error_message, "Xenia Error",
                  MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
      ExitProcess(static_cast<uint32_t>(-1));
    }
  }
};

// This is a hack to get an instance of StartupAvxCheck
// constructed before any initialization code,
// where the AVX check then happens in the constructor.
//
// https://docs.microsoft.com/en-us/cpp/preprocessor/init-seg
#pragma warning(suppress : 4073)
#pragma init_seg(lib)
static StartupCpuFeatureCheck gStartupAvxCheck;
