#include "NativeSampleModule.h"
#include <webp/encode.h>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <android/log.h>

namespace facebook::react {

// Base64 encoding table
static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

// Base64 encode helper
static std::string base64_encode(const uint8_t* data, size_t length) {
  std::string ret;
  int i = 0;
  int j = 0;
  uint8_t char_array_3[3];
  uint8_t char_array_4[4];

  while (length--) {
    char_array_3[i++] = *(data++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; i < 4; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; j < i + 1; j++)
      ret += base64_chars[char_array_4[j]];

    while(i++ < 3)
      ret += '=';
  }

  return ret;
}

// Base64 decode helper
static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

static std::vector<uint8_t> base64_decode(const std::string& encoded_string) {
  std::vector<uint8_t> ret;
  int in_len = encoded_string.size();
  int i = 0;
  int in_ = 0;
  uint8_t char_array_4[4], char_array_3[3];

  while (in_ < in_len && encoded_string[in_] != '=' && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_++];
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = strchr(base64_chars, char_array_4[i]) - base64_chars;

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; i < 3; i++)
        ret.push_back(char_array_3[i]);
      i = 0;
    }
  }

  if (i) {
    for (int j = 0; j < i; j++)
      char_array_4[j] = strchr(base64_chars, char_array_4[j]) - base64_chars;

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

    for (int j = 0; j < i - 1; j++)
      ret.push_back(char_array_3[j]);
  }

  return ret;
}

NativeSampleModule::NativeSampleModule(std::shared_ptr<CallInvoker> jsInvoker)
    : NativeSampleModuleCxxSpec(std::move(jsInvoker)) {}

std::string NativeSampleModule::reverseString(jsi::Runtime& rt, std::string input) {
  return std::string(input.rbegin(), input.rend());
}

std::string NativeSampleModule::encodeRGBAToWebP(
    jsi::Runtime& rt,
    std::string rgbaData,
    double width,
    double height,
    double quality) {
  
  try {
    // Decode base64 input
    std::vector<uint8_t> rgba = base64_decode(rgbaData);
    
    int w = static_cast<int>(width);
    int h = static_cast<int>(height);
    float q = static_cast<float>(quality);
    
    // Debug output
    __android_log_print(ANDROID_LOG_DEBUG, "WebP", "Base64 input length: %zu", rgbaData.size());
    __android_log_print(ANDROID_LOG_DEBUG, "WebP", "Decoded RGBA length: %zu", rgba.size());
    __android_log_print(ANDROID_LOG_DEBUG, "WebP", "Expected length: %d", w * h * 4);
    
    // Validate input
    if (w <= 0 || h <= 0) {
      throw std::runtime_error("Invalid width or height");
    }
    
    if (q < 0) q = 0;
    if (q > 100) q = 100;
    
    size_t expected_size = w * h * 4;
    if (rgba.size() != expected_size) {
      throw std::runtime_error(
        "RGBA data size mismatch. Expected " + 
        std::to_string(expected_size) + 
        " bytes, got " + 
        std::to_string(rgba.size())
      );
    }
    
    // Encode to WebP
    uint8_t* output = nullptr;
    size_t output_size = WebPEncodeRGBA(
      rgba.data(),
      w,
      h,
      w * 4,  // stride
      q,
      &output
    );
    
    if (output_size == 0 || output == nullptr) {
      throw std::runtime_error("WebP encoding failed");
    }
    
    // Convert to base64
    std::string result = base64_encode(output, output_size);
    
    // Free WebP output buffer
    WebPFree(output);
    
    return result;
    
  } catch (const std::exception& e) {
    throw jsi::JSError(rt, std::string("WebP encoding error: ") + e.what());
  }
}

std::string NativeSampleModule::encodeRGBAToWebPLossless(
    jsi::Runtime& rt,
    std::string rgbaData,
    double width,
    double height) {
  
  try {
    // Decode base64 input
    std::vector<uint8_t> rgba = base64_decode(rgbaData);
    
    int w = static_cast<int>(width);
    int h = static_cast<int>(height);
    
    // Validate input
    if (w <= 0 || h <= 0) {
      throw std::runtime_error("Invalid width or height");
    }
    
    size_t expected_size = w * h * 4;
    if (rgba.size() != expected_size) {
      throw std::runtime_error(
        "RGBA data size mismatch. Expected " + 
        std::to_string(expected_size) + 
        " bytes, got " + 
        std::to_string(rgba.size())
      );
    }
    
    // Encode to WebP lossless
    uint8_t* output = nullptr;
    size_t output_size = WebPEncodeLosslessRGBA(
      rgba.data(),
      w,
      h,
      w * 4,  // stride
      &output
    );
    
    if (output_size == 0 || output == nullptr) {
      throw std::runtime_error("WebP lossless encoding failed");
    }
    
    // Convert to base64
    std::string result = base64_encode(output, output_size);
    
    // Free WebP output buffer
    WebPFree(output);
    
    return result;
    
  } catch (const std::exception& e) {
    throw jsi::JSError(rt, std::string("WebP lossless encoding error: ") + e.what());
  }
}

} // namespace facebook::react