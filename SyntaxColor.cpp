#include <wx/textctrl.h>
#include <string>
#include <map>
#include <algorithm>
#include "wxOptions.h"
#include "CncStandard.h"
#include "SyntaxColor.h"
using namespace std;

void syntax_highlight(wxTextCtrl* elem, int standard, s_Settings &s) {
	map<char, wxTextAttr> Style;
	char comment_symbol = '(';

	Style['%'] = wxTextAttr(wxColour( 28, 255, 251));
	Style['M'] = wxTextAttr(s.colour_command_m);
	Style['N'] = wxTextAttr(s.colour_line_number);
	switch (standard) {
	case WAS_8035:
	case WAS_8037:
	case TAKI_8037:
		// Fagor 8035/8037: el corrector va en D y los comentarios empiezan con ;
		Style['D'] = wxTextAttr(s.colour_command_tool);
		comment_symbol = ';';
		break;
	case FAGOR_8025:
	case KIA_FANUC:
	default:
		Style['T'] = wxTextAttr(s.colour_command_tool);
		comment_symbol = '(';
		break;
	}

	elem->Enable(false);
	string aux = elem->GetValue().ToStdString() + '\n';
	elem->SetStyle(0, aux.length(), wxTextAttr(s.colour_text, s.colour_textCtrl));

	for (map<char, wxTextAttr>::iterator itStyle = Style.begin(); itStyle != Style.end(); ++itStyle) {
		size_t cur_pos = 0;
		while (cur_pos < aux.length()) {
			cur_pos = aux.find(itStyle->first, cur_pos);
			if (cur_pos == string::npos) break;
			size_t cur_pos_end = min(aux.find(' ', cur_pos), aux.find('\n', cur_pos));
			if (cur_pos_end == string::npos) cur_pos_end = aux.length();
			elem->SetStyle(cur_pos, cur_pos_end, itStyle->second);
			cur_pos = cur_pos_end;
		}
	}

	// Comentarios: desde el símbolo hasta el fin de línea
	size_t cur_pos = 0;
	while (cur_pos < aux.length()) {
		cur_pos = aux.find(comment_symbol, cur_pos);
		if (cur_pos == string::npos) break;
		size_t cur_pos_end = aux.find('\n', cur_pos);
		if (cur_pos_end == string::npos) cur_pos_end = aux.length();
		elem->SetStyle(cur_pos, cur_pos_end, wxTextAttr(s.colour_comments));
		cur_pos = cur_pos_end;
	}
	elem->Enable(true);
}
