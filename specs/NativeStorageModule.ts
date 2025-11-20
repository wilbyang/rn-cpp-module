import {TurboModule, TurboModuleRegistry} from 'react-native';
import type {UnsafeObject} from 'react-native/Libraries/Types/CodegenTypes';

export interface Spec extends TurboModule {
  setItem(value: string, key: string): void;
  getItem(key: string): string | null;
  removeItem(key: string): void;
  clear(): void;
}

export default TurboModuleRegistry.getEnforcing<Spec>(
  'NativeStorageModule',
);