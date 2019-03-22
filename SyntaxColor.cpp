#include <wx/textctrl.h>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include "wxOptions.h"
using namespace std;

void syntax_highlight(wxTextCtrl* elem, int standard, s_Settings &s) {
	elem->SetStyle(0, (elem->GetValue()).length(), wxTextAttr(*wxYELLOW,s.colour_textCtrl));
	map<char, wxTextAttr> Style;
	char comment_symbol;
	
	Style['%'] = wxTextAttr(wxColour( 28, 255, 251));
	Style['M'] = wxTextAttr(s.colour_command_m);
	Style['N'] = wxTextAttr(s.colour_line_number);
	if (standard == 8025) {
		Style['T'] = wxTextAttr(s.colour_command_tool);
		comment_symbol = '(';
	}
	if (standard == 8035) {
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


void syntax_color_8035_OLD(wxTextCtrl* elem) {
	//elem->Show(false);
	string aux = std::string(elem->GetValue() + '\n');
	elem->SetValue("");
	
	while (!aux.empty()) {
		int pos = aux.find_first_of('\n');
		string line = aux.substr(0, pos);
		aux = aux.substr(pos+1, aux.length());
		wxColour bg = wxColour( 0, 30, 60);
		
		while (!line.empty()) {
			if (line[0] == ' ') {
				elem->AppendText(' ');
				line = line.substr(1, line.length());
				continue;
			}
			pos = line.find_first_of(' ');
			if (pos < 0)
				pos = line.length();
			string sentence = line.substr(0, pos);
			line = line.substr(pos, line.length());
			switch (sentence[0]) {
			case '%':
				elem->SetDefaultStyle(wxTextAttr(wxColour( 28, 255, 251), bg));
				break;
			case 'D':
				elem->SetDefaultStyle(wxTextAttr(*wxRED, bg));
				break;
			case 'M':
				if (sentence == "M04" || sentence == "M03") {
					elem->SetDefaultStyle(wxTextAttr(*wxRED, bg));
				} else {
					elem->SetDefaultStyle(wxTextAttr(*wxYELLOW, bg));
				}
				break;
			case 'N':
				elem->SetDefaultStyle(wxTextAttr(*wxGREEN, bg));
				break;
			case ';':
				sentence = sentence + line;
				line = "";
				elem->SetDefaultStyle(wxTextAttr(wxColour(68, 117, 71), bg));
				break;
			case '#':
				sentence = sentence + line;
				line = "";
				elem->SetDefaultStyle(wxTextAttr(wxColour(224, 13, 13), bg));
				break;
			default:
				elem->SetDefaultStyle(wxTextAttr(*wxYELLOW, bg));
			}
			elem->AppendText(sentence);
		}
		elem->AppendText('\n');
	}
	//elem->Show(true);
	elem->SetFocus();
}
