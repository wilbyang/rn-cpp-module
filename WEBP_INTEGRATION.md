# WebP Integration Guide

## Overview

This project now includes libwebp integration, allowing you to encode RGBA image data to WebP format directly from JavaScript using native C++ code via React Native JSI (JavaScript Interface).

## Features Implemented

### 1. **encodeRGBAToWebP** (Lossy Compression)
Encodes RGBA pixel data to WebP format with adjustable quality.

**Parameters:**
- `rgbaData`: Base64-encoded RGBA pixel data (4 bytes per pixel: R, G, B, A)
- `width`: Image width in pixels
- `height`: Image height in pixels
- `quality`: Compression quality (0-100, where 100 is highest quality)

**Returns:** Base64-encoded WebP image data

### 2. **encodeRGBAToWebPLossless** (Lossless Compression)
Encodes RGBA pixel data to WebP format with lossless compression.

**Parameters:**
- `rgbaData`: Base64-encoded RGBA pixel data
- `width`: Image width in pixels
- `height`: Image height in pixels

**Returns:** Base64-encoded WebP image data

## Architecture

### File Structure

```
rnit3/
├── specs/
│   └── NativeSampleModule.ts          # TypeScript interface definitions
├── shared/
│   ├── NativeSampleModule.h           # C++ header with WebP methods
│   └── NativeSampleModule.cpp         # C++ implementation with libwebp
├── android/
│   └── app/src/main/jni/
│       ├── CMakeLists.txt             # Build configuration with libwebp
│       └── OnLoad.cpp                 # Native module registration
├── WebPExample.tsx                     # Example usage component
└── App.tsx                            # Main app entry
```

### Implementation Details

#### 1. TypeScript Spec (`specs/NativeSampleModule.ts`)
Defines the interface for the native module using React Native Codegen. This generates the necessary glue code for JSI.

#### 2. C++ Header (`shared/NativeSampleModule.h`)
Declares the native methods that will be callable from JavaScript.

#### 3. C++ Implementation (`shared/NativeSampleModule.cpp`)
- Implements base64 encoding/decoding utilities
- Uses libwebp's `WebPEncodeRGBA()` for lossy compression
- Uses libwebp's `WebPEncodeLosslessRGBA()` for lossless compression
- Handles error validation and memory management

#### 4. CMake Configuration (`android/app/src/main/jni/CMakeLists.txt`)
- Uses CMake's `FetchContent` to download libwebp v1.3.2 from GitHub
- Configures build to only include encoding functionality (excludes CLI tools)
- Links libwebp to the native module

## Usage Example

```typescript
import NativeSampleModule from './specs/NativeSampleModule';

// Create RGBA data (e.g., 2x2 red image)
const width = 2;
const height = 2;
const rgbaArray = new Uint8Array([
  255, 0, 0, 255,  // Red pixel
  255, 0, 0, 255,
  255, 0, 0, 255,
  255, 0, 0, 255,
]);

// Convert to base64
const base64RGBA = arrayBufferToBase64(rgbaArray);

// Encode to WebP (lossy)
const webpBase64 = NativeSampleModule.encodeRGBAToWebP(
  base64RGBA,
  width,
  height,
  80  // quality
);

// Encode to WebP (lossless)
const webpLossless = NativeSampleModule.encodeRGBAToWebPLossless(
  base64RGBA,
  width,
  height
);
```

## Building and Running

### Android

1. **Clean and rebuild** (recommended after changes):
   ```bash
   cd android
   ./gradlew clean
   cd ..
   ```

2. **Install dependencies**:
   ```bash
   npm install
   # or
   yarn install
   ```

3. **Run on Android**:
   ```bash
   npm run android
   # or
   yarn android
   ```

### Important Notes

- On first build, CMake will download libwebp from GitHub (~5-10 seconds)
- Subsequent builds will use the cached version
- The build includes only the encoding library, not the full libwebp suite

## Technical Details

### Base64 Encoding/Decoding
Since React Native doesn't support passing binary data directly through JSI, we use base64 encoding:
- JavaScript → C++: Base64-encoded RGBA data is decoded to binary
- C++ → JavaScript: WebP binary data is base64-encoded for return

### Memory Management
- libwebp allocates output buffers with `WebPEncodeRGBA()` and `WebPEncodeLosslessRGBA()`
- These buffers are freed using `WebPFree()` after base64 encoding
- Input data is managed by `std::vector<uint8_t>` for automatic cleanup

### Error Handling
- Validates input dimensions and data size
- Clamps quality values to 0-100 range
- Throws `jsi::JSError` for errors, which propagate to JavaScript as exceptions

## Performance Considerations

1. **Base64 Overhead**: ~33% size increase for data transfer between JS and C++
2. **WebP Encoding**: Very fast for small images; performance scales with image size
3. **Quality vs Size**: Lower quality = smaller files but more artifacts
4. **Lossless vs Lossy**: Lossless produces larger files but perfect reproduction

## Future Enhancements

Potential additions to the WebP module:

1. **WebPEncodeBGRA()** - Support BGR pixel order
2. **WebPConfig** - Advanced encoding options (method, segments, preprocessing, etc.)
3. **WebPDecode***() - Decoding WebP to RGBA
4. **WebPAnimEncoder** - Animated WebP support
5. **Batch encoding** - Encode multiple images in one call
6. **Progress callbacks** - For large images

## Troubleshooting

### Build Issues

**Problem**: CMake fails to download libwebp
- **Solution**: Check internet connection; CMake needs to fetch from GitHub

**Problem**: Linker errors with libwebp symbols
- **Solution**: Clean build (`./gradlew clean`) and rebuild

### Runtime Issues

**Problem**: "RGBA data size mismatch" error
- **Solution**: Ensure your RGBA array is exactly `width × height × 4` bytes

**Problem**: "WebP encoding failed" error
- **Solution**: Check that width/height are positive; validate input data is not corrupted

## References

- [libwebp GitHub](https://github.com/webmproject/libwebp)
- [libwebp API Documentation](https://developers.google.com/speed/webp/docs/api)
- [React Native New Architecture](https://reactnative.dev/docs/the-new-architecture/landing-page)
- [React Native JSI](https://reactnative.dev/architecture/glossary#javascript-interfaces-jsi)
