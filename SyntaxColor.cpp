#include <wx/textctrl.h>
#include <string>
#include <iostream>
using namespace std;

void syntax_color_8025(wxTextCtrl* elem) {
	//elem->Show(false);
	string aux = std::string(elem->GetValue() + '\n');
	elem->SetValue("");
	
	while (!aux.empty()) {
		int pos = aux.find_first_of('\n');
		string line = aux.substr(0, pos);
		aux = aux.substr(pos+1, aux.length());
		
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
				elem->SetDefaultStyle(wxTextAttr(wxColour( 28, 255, 251)));
				break;
			case 'N':
				elem->SetDefaultStyle(wxTextAttr(*wxGREEN));
				break;
			case '(':
				sentence = sentence + line;
				line = "";
				elem->SetDefaultStyle(wxTextAttr(wxColour(68, 117, 71)));
				break;
			case '#':
				sentence = sentence + line;
				line = "";
				elem->SetDefaultStyle(wxTextAttr(wxColour(224, 13, 13)));
				break;
			default:
				elem->SetDefaultStyle(wxTextAttr(*wxYELLOW));
			}
			elem->AppendText(sentence);
		}
		elem->AppendText('\n');
	}
	//elem->Show(true);
	elem->SetFocus();
}

void syntax_color_8035(wxTextCtrl* elem) {
	//elem->Show(false);
	string aux = std::string(elem->GetValue() + '\n');
	elem->SetValue("");
	
	while (!aux.empty()) {
		int pos = aux.find_first_of('\n');
		string line = aux.substr(0, pos);
		aux = aux.substr(pos+1, aux.length());
		
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
				elem->SetDefaultStyle(wxTextAttr(wxColour( 28, 255, 251)));
				break;
			case 'N':
				elem->SetDefaultStyle(wxTextAttr(*wxGREEN));
				break;
			case ';':
				sentence = sentence + line;
				line = "";
				elem->SetDefaultStyle(wxTextAttr(wxColour(68, 117, 71)));
				break;
			case '#':
				sentence = sentence + line;
				line = "";
				elem->SetDefaultStyle(wxTextAttr(wxColour(224, 13, 13)));
				break;
			default:
				elem->SetDefaultStyle(wxTextAttr(*wxYELLOW));
			}
			elem->AppendText(sentence);
		}
		elem->AppendText('\n');
	}
	//elem->Show(true);
	elem->SetFocus();
}
