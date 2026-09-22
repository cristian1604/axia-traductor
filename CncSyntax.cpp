#include "CncSyntax.h"
#include <cctype>

namespace {

inline bool is_digit(char c) { return std::isdigit((unsigned char) c) != 0; }
inline bool is_alpha(char c) { return std::isalpha((unsigned char) c) != 0; }

void fill(std::vector<unsigned char> &st, size_t from, size_t to, unsigned char style) {
	for (size_t k = from; k < to; ++k) st[k] = style;
}

// Clasifica la línea [beg, end) de un programa ya iniciado.
void classify_line(const std::string &text, size_t beg, size_t end, bool paren_comments,
                   std::vector<unsigned char> &st) {
	size_t p = beg;
	while (p < end) {
		char c = text[p];
		if (paren_comments && c == '(') {
			// Comentario entre paréntesis (8025, FANUC); sin cierre llega al fin de línea
			size_t e = text.find(')', p);
			e = (e == std::string::npos || e >= end) ? end : e + 1;
			fill(st, p, e, STYLE_COMMENT);
			p = e;
			continue;
		}
		if (!paren_comments && c == ';') {
			// Comentario de 8035/8037: hasta el fin de línea
			fill(st, p, end, STYLE_COMMENT);
			return;
		}
		if (is_alpha(c)) {
			// Palabra: letra + dígitos (con punto y signo). Solo empieza un token
			// si no viene de otra letra, para no colorear "VITON", "GOTO", "IF".
			bool token_start = (p == beg) || !is_alpha(text[p - 1]);
			size_t e = p + 1;
			bool has_digit = false;
			while (e < end && (is_digit(text[e]) || text[e] == '.' || text[e] == '-' || text[e] == '+')) {
				if (is_digit(text[e])) has_digit = true;
				++e;
			}
			unsigned char style = STYLE_TEXT;
			if (token_start && has_digit) {
				switch (std::toupper((unsigned char) c)) {
				case 'N': style = STYLE_LABEL; break;
				case 'M': style = STYLE_M;     break;
				case 'T':
				case 'D': style = STYLE_TOOL;  break;
				}
			}
			fill(st, p, e, style);
			p = e;
			continue;
		}
		++p;
	}
}

} // namespace

std::vector<unsigned char> classify_cnc(const std::string &text, int standard) {
	std::vector<unsigned char> st(text.size(), STYLE_TEXT);
	const bool paren_comments = !(standard == WAS_8035 || standard == WAS_8037 || standard == TAKI_8037);
	bool started = false;
	size_t i = 0;
	while (i < text.size()) {
		size_t eol = text.find('\n', i);
		if (eol == std::string::npos) eol = text.size();
		size_t b = i;
		while (b < eol && (text[b] == ' ' || text[b] == '\t')) ++b;

		bool header = false;
		if (b < eol) {
			if (text[b] == '%') header = true;
			else if (!started && standard == KIA_FANUC && text[b] == 'O' && b + 1 < eol && is_digit(text[b + 1])) header = true;
		}
		if (header) {
			started = true;
			fill(st, i, eol, STYLE_HEADER);
		} else if (!started) {
			fill(st, i, eol, STYLE_COMMENT);
		} else {
			classify_line(text, i, eol, paren_comments, st);
		}
		i = eol + 1;
	}
	return st;
}
