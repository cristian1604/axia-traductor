#include <string>
#include <wx/textctrl.h>
#include <iostream>
using namespace std;

void translate_8025_to_8035(wxTextCtrl* elem) {
	//elem->Show(false);
	string aux = std::string(elem->GetValue() + '\n');
	elem->SetValue("");
	bool program_initiated = false;
	bool comments_inserted = false;
	wxString comments;
	
	while (!aux.empty()) {
		int pos = aux.find_first_of('\n');
		string line = aux.substr(0, pos);
		aux = aux.substr(pos+1, aux.length());
		if (!program_initiated && line[0] != '%') {
			comments.Append(';' + line + '\n');
			continue;
		} else {
			program_initiated = true;
		}
		
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
				if (!comments_inserted) {
					elem->AppendText(sentence + '\n');
					elem->AppendText(comments);
					comments_inserted = true;
				}
				break;
			case 'A':
				sentence[0] = 'Q';
				break;
			case '(':
				sentence = ';' + sentence + line;
				line = "";
				break;
			case 'N':
				// blank lines (block separator)
				if (line.length() <= 2) {
					sentence = sentence + "  ;";
				}
				break;
			case 'T':
				int pos = sentence.find_first_of('.',0);
				sentence = 'D' + sentence.substr(pos+1,sentence.length());
				break;
			}
			if (comments_inserted && sentence[0] != '%')
				elem->AppendText(sentence);
			if (comments_inserted && sentence[0] == '%')
				elem->AppendText(';');
		}
		elem->AppendText('\n');
	}
	//elem->Show(true);
	elem->SetFocus();
}
