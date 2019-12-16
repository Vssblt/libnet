#include "base64.h"
#include <string.h>

size_t base64_encode_core(const u_char *str, size_t in_size, char *out, const char *table);
size_t base64_decode_core(const u_char *str, size_t in_size, char *out, const char *table);
u_char decodeTableMap(const u_char str, const char *table);

size_t
base64_encode(const u_char *str, size_t in_size, char *out)
{
	return base64_encode_core(str, in_size, out, base64.table);
}

size_t
base64_safeEncode(const u_char *str, size_t in_size, char *out)
{
	return base64_encode_core(str, in_size, out, base64.safeTable);
}

size_t
base64_encode_core(const u_char *str, size_t in_size, char *out, const char *table)
{
	size_t size = 0;

	for (int i = 0; i < in_size / 3; i++)
	{
		out[i * 3] = table[str[i * 3] >> 2];
		out[i * 3 + 1] = table[((str[i * 3] << 4) | (str[i * 3 + 1] >> 4)) & 0x3F];
		out[i * 3 + 2] = table[((str[i * 3 + 1] << 2) | (str[i * 3 + 2] >> 6)) & 0x3F];
		out[i * 3 + 3] = table[str[i * 3 + 2] & 0x3F];
	}

	size = in_size * 8 / 6;
	int num = in_size * 8 % 6;

	if (num == 4)
	{
		int i = in_size / 3;
		out[i] = table[str[i * 3] >> 2];
		out[i * 3 + 1] = table[((str[i * 3] << 4) | (str[i * 3 + 1] >> 4)) & 0x3F];
		out[i * 3 + 2] = table[(str[i * 3 + 1] << 2) & 0x3F];
		out[i * 3 + 3] = '=';
		size++;
	} else if (num == 2) {
		int i = in_size / 3;
		out[i] = table[str[i * 3] >> 2];
		out[i * 3 + 1] = table[(str[i * 3] << 4) & 0x3F];
		out[i * 3 + 2] = '=';
		out[i * 3 + 3] = '=';
		size += 2;
	}

	return size; 
}

size_t
base64_decode(const u_char *str, size_t in_size, char *out)
{
	return base64_decode_core(str, in_size, out, base64.table);
}

size_t
base64_safeDecode(const u_char *str, size_t in_size, char *out)
{
	return base64_decode_core(str, in_size, out, base64.safeTable);
}

size_t
base64_decode_core(const u_char *str, size_t in_size, char *out, const char *table)
{
	size_t fillBytes = 4 - in_size % 4;
	if (fillBytes == 3) return 0;
	size_t outSize = in_size / 4 * 3 + (fillBytes == 0 ? 0 : 3);

	for (int i = 0; i < in_size / 4; i++)
	{
		out[i * 3] |= decodeTableMap(str[i * 4], table) << 2;
		out[i * 3] |= decodeTableMap(str[i * 4 + 1], table) >> 4;
		out[i * 3 + 1] |= decodeTableMap(str[i * 4 + 1], table) << 4;
		out[i * 3 + 1] |= decodeTableMap(str[i * 4 + 2], table) >> 2;
		out[i * 3 + 2] |= decodeTableMap(str[i * 4 + 2], table) << 6;
		out[i * 3 + 2] |= decodeTableMap(str[i * 4 + 3], table);
	}

	return outSize;
}

u_char
decodeTableMap(const u_char str, const char *table)
{
	if (str > 0x30 && str < 0x39) {
		return str - 0x30 + 26 * 2;
	}

	if (str > 0x41 && str < 0x5A) {
		return str - 0x41;
	}

	if (str > 0x61 && str < 0x7A) {
		return str - 0x61 + 26;
	}

	if (str == 0x2B) {
		return str - 0x2B + 26 * 2 + 10;
	}

	if (str == 0x2F) {
		return str - 0x2F + 26 * 2 + 10 + 1;
	}
	
	return 0;
}

