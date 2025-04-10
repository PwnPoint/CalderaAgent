/*
	base64.c - by Joe DF (joedf@ahkscript.org)
	Released under the MIT License

	This file provides functions for encoding and decoding data in Base64 format.

	See "base64.h", for more information.

	Thanks to the following resource for inspiration:
	http://www.codeproject.com/Tips/813146/Fast-base-functions-for-encode-decode
*/

#include "base64.h"  // Include the base64 header where function prototypes and necessary includes are declared.
#include <Windows.h> // Windows header for various system and memory functions.

// Base64 character table used internally for encoding Base64
unsigned char b64_chr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Function to convert an ASCII character to its Base64 equivalent.
// This function maps the ASCII characters to Base64 index values.
// For example, 'A' -> 0, 'a' -> 26, '0' -> 52, '+' -> 62, '/' -> 63, '=' -> 64.
unsigned int b64_int(unsigned int ch) {
	if (ch == 43)  // '+' character
		return 62;
	if (ch == 47)  // '/' character
		return 63;
	if (ch == 61)  // '=' character (padding)
		return 64;
	if ((ch > 47) && (ch < 58))  // Numbers '0'-'9'
		return ch + 4;
	if ((ch > 64) && (ch < 91))  // Uppercase 'A'-'Z'
		return ch - 'A';
	if ((ch > 96) && (ch < 123))  // Lowercase 'a'-'z'
		return (ch - 'a') + 26;
	return 0;  // Default case
}

// Function to calculate the size of the output buffer required for Base64 encoding.
// The formula is 4 * (1/3 * input size) rounded up.
unsigned int b64e_size(unsigned int in_size) {
	int i, j = 0;
	for (i = 0; i < in_size; i++) {
		if (i % 3 == 0)  // For every 3 bytes of input, 4 Base64 characters are required.
			j += 1;
	}
	return (4 * j);  // Return the total size of the encoded output.
}

// Function to calculate the size of the output buffer required for Base64 decoding.
// The formula is (3 * input size) / 4, since every 4 Base64 characters represent 3 bytes of data.
unsigned int b64d_size(unsigned int in_size) {
	return ((3 * in_size) / 4);
}

// Function to encode a given input buffer into Base64 format.
// Parameters:
// - `in`: Input buffer (raw data).
// - `in_len`: Length of the input buffer.
// - `out`: Output buffer to store the Base64 encoded string.
// Returns the size of the encoded output.
unsigned int b64_encode(const unsigned char* in, unsigned int in_len, unsigned char* out) {
	unsigned int i = 0, j = 0, k = 0, s[3];  // Buffers for 3 bytes of input data.

	for (i = 0; i < in_len; i++) {
		s[j++] = *(in + i);  // Read input bytes.
		if (j == 3) {  // Process when 3 bytes have been read.
			out[k + 0] = b64_chr[(s[0] & 255) >> 2];  // Encode first 6 bits.
			out[k + 1] = b64_chr[((s[0] & 0x03) << 4) + ((s[1] & 0xF0) >> 4)];  // Encode next 6 bits.
			out[k + 2] = b64_chr[((s[1] & 0x0F) << 2) + ((s[2] & 0xC0) >> 6)];  // Encode next 6 bits.
			out[k + 3] = b64_chr[s[2] & 0x3F];  // Encode the last 6 bits.
			j = 0;  // Reset byte counter.
			k += 4;  // Move to the next set of output positions.
		}
	}

	if (j) {  // Handle remaining bytes if the input size is not a multiple of 3.
		if (j == 1)  // If only 1 byte remains, pad the rest.
			s[1] = 0;
		out[k + 0] = b64_chr[(s[0] & 255) >> 2];  // Encode the first 6 bits.
		out[k + 1] = b64_chr[((s[0] & 0x03) << 4) + ((s[1] & 0xF0) >> 4)];  // Encode the next bits.
		if (j == 2)  // If 2 bytes remain, encode the next part and pad the last one.
			out[k + 2] = b64_chr[((s[1] & 0x0F) << 2)];
		else
			out[k + 2] = '=';  // Add padding '='.
		out[k + 3] = '=';  // Add padding '='.
		k += 4;
	}

	out[k] = '\0';  // Null-terminate the encoded output string.
	return k;  // Return the length of the encoded string.
}

// Function to decode a Base64 encoded string into raw data.
// Parameters:
// - `in`: Input buffer (Base64 encoded string).
// - `in_len`: Length of the Base64 input string.
// - `out`: Output buffer to store the decoded raw data.
// Returns the size of the decoded output.
unsigned int b64_decode(const unsigned char* in, unsigned int in_len, unsigned char* out) {
	unsigned int i = 0, j = 0, k = 0, s[4];  // Buffers for 4 Base64 characters.

	for (i = 0; i < in_len; i++) {
		s[j++] = b64_int(*(in + i));  // Convert Base64 character to its integer equivalent.
		if (j == 4) {  // Process when 4 Base64 characters have been read.
			out[k + 0] = ((s[0] & 255) << 2) + ((s[1] & 0x30) >> 4);  // Decode first byte.
			if (s[2] != 64) {  // Check if it's not padding.
				out[k + 1] = ((s[1] & 0x0F) << 4) + ((s[2] & 0x3C) >> 2);  // Decode second byte.
				if (s[3] != 64) {  // Check if there's a third byte.
					out[k + 2] = ((s[2] & 0x03) << 6) + (s[3]);  // Decode third byte.
					k += 3;  // Advance by 3 bytes.
				}
				else {
					k += 2;  // Only 2 bytes decoded.
				}
			}
			else {
				k += 1;  // Only 1 byte decoded.
			}
			j = 0;  // Reset character counter.
		}
	}

	return k;  // Return the length of the decoded output.
}

// Helper function to encode a string into Base64 format.
// Parameters:
// - `input`: Input string to be encoded.
// - `length`: Length of the input string.
// Returns a pointer to the Base64 encoded string.
char* b64encode(char* input, int length) {
	int b64size = b64e_size(length) + 1;  // Calculate the size of the Base64 encoded string.
	char* encodedstr = (char*)malloc(b64size);  // Allocate memory for the encoded string.
	memset(encodedstr, 0, b64size);  // Initialize memory.
	b64_encode((unsigned char*)input, length, (unsigned char*)encodedstr);  // Encode the input string.
	return encodedstr;  // Return the encoded string.
}

// Helper function to decode a Base64 encoded string.
// Parameters:
// - `input`: Base64 encoded string to be decoded.
// - `length`: Length of the encoded string.
// Returns a pointer to the decoded string.
char* b64decode(char* input, int length) {
	int b64size = b64d_size(length) + 1;  // Calculate the size of the decoded output.
	char* decodedstr = (char*)malloc(b64size);  // Allocate memory for the decoded string.
	memset(decodedstr, 0, b64size);  // Initialize memory.
	b64_decode((unsigned char*)input, length, (unsigned char*)decodedstr);  // Decode the Base64 string.
	return decodedstr;  // Return the decoded string.
}
