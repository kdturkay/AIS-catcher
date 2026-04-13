/*
	Copyright(c) 2021-2026 jvde.github@gmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

// Software-Within-A-Register byte scanning primitives. Word-sized (size_t)
// today; same interface extends to SSE/AVX later.

namespace SWAR
{
	static constexpr size_t ONES = ~(size_t)0 / 255;
	static constexpr size_t HIGHS = ONES * 128;

	static constexpr size_t mask(char target)
	{
		return ONES * (unsigned char)target;
	}

	// Non-zero mask iff any byte in word equals the targeted byte encoded in m.
	// Use with mask(c). The returned value has the high bit set in each matching
	// byte — feed it to first_byte_pos() to locate the first match.
	static inline size_t has_byte(size_t word, size_t m)
	{
		size_t v = word ^ m;
		return (v - ONES) & ~v & HIGHS;
	}

	// Non-zero mask iff any byte in word is < n. Caller's responsibility:
	// 1 <= n <= 128. Useful for catching control characters (n=0x20).
	static inline size_t has_byte_lt(size_t word, size_t n)
	{
		return (word - ONES * n) & ~word & HIGHS;
	}

	static inline int first_byte_pos(size_t m)
	{
#if defined(__GNUC__) || defined(__clang__)
		if (sizeof(size_t) == 8)
			return __builtin_ctzll((unsigned long long)m) >> 3;
		else
			return __builtin_ctz((unsigned int)m) >> 3;
#else
		int n = 0;
		while (!(m & 0xFF))
		{
			m >>= 8;
			n++;
		}
		return n;
#endif
	}
}

// Scan (buf + i) .. (buf + limit) by size_t chunks. `expr` must evaluate to a
// size_t mask (use bitwise | to combine SWAR::has_byte / has_byte_lt results).
// On a hit, i is advanced to the exact matching byte. On no hit, i is left
// at the start of the tail (< sizeof(size_t) bytes remaining) and the caller
// must handle those byte-by-byte.
#define SWAR_SKIP_AT(buf, i, limit, expr)             \
	while ((i) + (int)sizeof(size_t) <= (limit))      \
	{                                                 \
		size_t word;                                  \
		std::memcpy(&word, (buf) + (i), sizeof(word));\
		size_t _swar_hit = (expr);                    \
		if (_swar_hit)                                \
		{                                             \
			(i) += SWAR::first_byte_pos(_swar_hit);   \
			break;                                    \
		}                                             \
		(i) += sizeof(size_t);                        \
	}

// Pointer variant of SWAR_SKIP_AT. Scans p .. pend by size_t chunks.
// On a hit, p is advanced to the exact matching byte. On no hit, p is left
// at the start of the tail (< sizeof(size_t) bytes remaining).
#define SWAR_SKIP_PTR(p, pend, expr)                  \
	while ((p) + sizeof(size_t) <= (pend))            \
	{                                                 \
		size_t word;                                  \
		std::memcpy(&word, (p), sizeof(word));        \
		size_t _swar_hit = (expr);                    \
		if (_swar_hit)                                \
		{                                             \
			(p) += SWAR::first_byte_pos(_swar_hit);   \
			break;                                    \
		}                                             \
		(p) += sizeof(size_t);                        \
	}
