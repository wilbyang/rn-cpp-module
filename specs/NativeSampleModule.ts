import {TurboModule, TurboModuleRegistry} from 'react-native';
import type {UnsafeObject} from 'react-native/Libraries/Types/CodegenTypes';

export interface Spec extends TurboModule {
  readonly reverseString: (input: string) => string;
  
  // WebP encoding functions
  // Encodes RGBA data to WebP format
  // rgbaData: base64 encoded RGBA pixel data
  // width: image width in pixels
  // height: image height in pixels
  // quality: compression quality (0-100)
  // Returns base64 encoded WebP image data
  readonly encodeRGBAToWebP: (
    rgbaData: string,
    width: number,
    height: number,
    quality: number
  ) => string;
  
  // Encodes RGBA data to WebP format with lossless compression
  readonly encodeRGBAToWebPLossless: (
    rgbaData: string,
    width: number,
    height: number
  ) => string;
}

export default TurboModuleRegistry.getEnforcing<Spec>(
  'NativeSampleModule',
);