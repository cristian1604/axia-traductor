#include <wx/textctrl.h>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include "wxOptions.h"
using namespace std;

#define FAGOR_8025 8025
#define FAGOR_8035 8035

void syntax_highlight(wxTextCtrl* elem, int standard, s_Settings &s) {
	elem->Enable(false);
	elem->SetStyle(0, (elem->GetValue()).length(), wxTextAttr(*wxYELLOW,s.colour_textCtrl));
	map<char, wxTextAttr> Style;
	char comment_symbol;
	
	Style['%'] = wxTextAttr(wxColour( 28, 255, 251));
	Style['M'] = wxTextAttr(s.colour_command_m);
	Style['N'] = wxTextAttr(s.colour_line_number);
	if (standard == FAGOR_8025) {
		Style['T'] = wxTextAttr(s.colour_command_tool);
		comment_symbol = '(';
	}
	if (standard == FAGOR_8035) {
		Style['D'] = wxTextAttr(s.colour_command_tool);
		comment_symbol = ';';
	}
	map<char, wxTextAttr>::iterator itStyle = Style.begin();
	
	elem->Enable(false);
	string aux = std::string(elem->GetValue() + '\n');
	elem->SetStyle(0, aux.length(), wxTextAttr(s.colour_text));
	string::iterator itStr = aux.begin();
	long cur_pos = 0;
	long cur_pos_end = 1;
	wxTextAttr attr;
	while (itStyle != Style.end()) {
		while (cur_pos <= aux.length()) {
			cur_pos = aux.find_first_of(itStyle->first, cur_pos);
			if (cur_pos < 0) break;
			cur_pos_end = min(aux.find_first_of(' ', cur_pos), aux.find_first_of('\n', cur_pos));
			elem->SetStyle(cur_pos, cur_pos_end, itStyle->second);
			cur_pos = cur_pos_end;
		}
		cur_pos = 0;
		cur_pos_end = 1;
		itStr = aux.begin();
		++itStyle;
	}
	
	// Highlight comments
	cur_pos = 0;
	while (cur_pos <= aux.length()) {
		cur_pos = aux.find_first_of(comment_symbol, cur_pos);
		if (cur_pos < 0) break;
		cur_pos_end = aux.find_first_of('\n', cur_pos);
		elem->SetStyle(cur_pos, cur_pos_end, wxTextAttr(s.colour_comments));
		cur_pos = cur_pos_end;
	}
	elem->Enable(true);
}
