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

#include <string>

#include "StringBuilder.h"

namespace JSON {

	void stringify(const char *s, size_t len, std::string& json, bool esc) {
		if (esc) json += '\"';
		size_t start = 0;

		for (size_t i = 0; i < len; i++) {
			unsigned char c = (unsigned char)s[i];
			if (c >= 0x20 && c != '"' && c != '\\')
				continue;

			if (i > start) json.append(s + start, i - start);

			switch (c) {
			case '"':  json.append("\\\"", 2); break;
			case '\\': json.append("\\\\", 2); break;
			case '\b': json.append("\\b", 2);  break;
			case '\f': json.append("\\f", 2);  break;
			case '\n': json.append("\\n", 2);  break;
			case '\r': json.append("\\r", 2);  break;
			case '\t': json.append("\\t", 2);  break;
			default: {
				static const char hex[] = "0123456789abcdef";
				char esc_buf[6] = { '\\', 'u', '0', '0', hex[(c >> 4) & 0xF], hex[c & 0xF] };
				json.append(esc_buf, 6);
				break;
			}
			}
			start = i + 1;
		}
		if (len > start) json.append(s + start, len - start);
		if (esc) json += '\"';
	}

}
