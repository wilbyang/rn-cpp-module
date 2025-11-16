import React, { useState } from 'react';
import { View, Text, Button, StyleSheet, Alert } from 'react-native';
import NativeSampleModule from './specs/NativeSampleModule';

export default function WebPExample() {
  const [result, setResult] = useState<string>('');

  const testWebPEncoding = () => {
    try {
      // Create a simple 2x2 red RGBA image
      // RGBA format: 4 bytes per pixel (R, G, B, A)
      const width = 2;
      const height = 2;
      
      // Create RGBA data (2x2 red pixels)
      const rgbaArray = new Uint8Array([
        255, 0, 0, 255,  // Red pixel 1
        255, 0, 0, 255,  // Red pixel 2
        255, 0, 0, 255,  // Red pixel 3
        255, 0, 0, 255,  // Red pixel 4
      ]);

      console.log('Original array length:', rgbaArray.length);
      console.log('Original array:', Array.from(rgbaArray));

      // Convert to base64
      const base64RGBA = arrayBufferToBase64(rgbaArray);
      console.log('Base64 string:', base64RGBA);
      console.log('Base64 length:', base64RGBA.length);

      // Encode to WebP with quality 80
      const webpBase64 = NativeSampleModule.encodeRGBAToWebP(
        base64RGBA,
        width,
        height,
        80
      );

      setResult(`WebP encoded successfully!\nSize: ${webpBase64.length} chars\nFirst 50 chars: ${webpBase64.substring(0, 50)}...`);
      
      Alert.alert('Success', 'WebP encoding completed!');
    } catch (error) {
      const errorMessage = error instanceof Error ? error.message : 'Unknown error';
      console.error('Error:', errorMessage);
      setResult(`Error: ${errorMessage}`);
      Alert.alert('Error', errorMessage);
    }
  };

  const testWebPLossless = () => {
    try {
      // Create a simple 2x2 blue RGBA image
      const width = 2;
      const height = 2;
      
      const rgbaArray = new Uint8Array([
        0, 0, 255, 255,  // Blue pixel 1
        0, 0, 255, 255,  // Blue pixel 2
        0, 0, 255, 255,  // Blue pixel 3
        0, 0, 255, 255,  // Blue pixel 4
      ]);

      const base64RGBA = arrayBufferToBase64(rgbaArray);

      // Encode to WebP lossless
      const webpBase64 = NativeSampleModule.encodeRGBAToWebPLossless(
        base64RGBA,
        width,
        height
      );

      setResult(`WebP lossless encoded!\nSize: ${webpBase64.length} chars\nFirst 50 chars: ${webpBase64.substring(0, 50)}...`);
      
      Alert.alert('Success', 'WebP lossless encoding completed!');
    } catch (error) {
      const errorMessage = error instanceof Error ? error.message : 'Unknown error';
      setResult(`Error: ${errorMessage}`);
      Alert.alert('Error', errorMessage);
    }
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>WebP Encoding Example</Text>
      
      <View style={styles.buttonContainer}>
        <Button
          title="Test WebP Encoding (Lossy)"
          onPress={testWebPEncoding}
        />
      </View>
      
      <View style={styles.buttonContainer}>
        <Button
          title="Test WebP Encoding (Lossless)"
          onPress={testWebPLossless}
        />
      </View>

      {result ? (
        <View style={styles.resultContainer}>
          <Text style={styles.resultLabel}>Result:</Text>
          <Text style={styles.resultText}>{result}</Text>
        </View>
      ) : null}
    </View>
  );
}

// Helper function to convert Uint8Array to base64
function arrayBufferToBase64(buffer: Uint8Array): string {
  const base64Chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
  let result = '';
  const len = buffer.length;
  
  // Process 3 bytes at a time
  for (let i = 0; i < len; i += 3) {
    const byte1 = buffer[i];
    const byte2 = i + 1 < len ? buffer[i + 1] : 0;
    const byte3 = i + 2 < len ? buffer[i + 2] : 0;
    
    const encoded1 = byte1 >> 2;
    const encoded2 = ((byte1 & 3) << 4) | (byte2 >> 4);
    const encoded3 = ((byte2 & 15) << 2) | (byte3 >> 6);
    const encoded4 = byte3 & 63;
    
    result += base64Chars[encoded1];
    result += base64Chars[encoded2];
    result += (i + 1 < len) ? base64Chars[encoded3] : '=';
    result += (i + 2 < len) ? base64Chars[encoded4] : '=';
  }
  
  return result;
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 20,
    justifyContent: 'center',
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    marginBottom: 30,
    textAlign: 'center',
  },
  buttonContainer: {
    marginVertical: 10,
  },
  resultContainer: {
    marginTop: 30,
    padding: 15,
    backgroundColor: '#f0f0f0',
    borderRadius: 8,
  },
  resultLabel: {
    fontSize: 16,
    fontWeight: 'bold',
    marginBottom: 8,
  },
  resultText: {
    fontSize: 14,
    fontFamily: 'monospace',
  },
});
