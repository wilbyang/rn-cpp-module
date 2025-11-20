#pragma once

#include <AppNativeSpecsJSI.h>

#include <memory>
#include <string>

namespace facebook::react {

class NativeSampleModule : public NativeSampleModuleCxxSpec<NativeSampleModule> {
public:
  NativeSampleModule(std::shared_ptr<CallInvoker> jsInvoker);

  std::string reverseString(jsi::Runtime& rt, std::string input);
  
  // WebP encoding functions
  std::string encodeRGBAToWebP(
    jsi::Runtime& rt,
    std::string rgbaData,
    double width,
    double height,
    double quality
  );
  
  std::string encodeRGBAToWebPLossless(
    jsi::Runtime& rt,
    std::string rgbaData,
    double width,
    double height
  );
};

} // namespace facebook::react
