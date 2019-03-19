#include <string>
#include <wx/textctrl.h>
#include <iostream>
#include <algorithm>
using namespace std;

void translate_8025_to_8035(wxTextCtrl* elem) {
	//elem->Show(false);
	string translated;
	string aux = std::string(elem->GetValue() + '\n');
	elem->SetValue("");
	bool program_initiated = false;
	bool comments_inserted = false;
	wxString comments;
	wxString sAux, prevSentence;
	int x;
	double db;
	
	
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
				translated += ' ';
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
					translated.append(sentence + '\n');
					translated.append(comments);
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
				// Convert form Taa.bb to Dbb
				x = sentence.find_first_of('.',0);
				sentence = 'D' + sentence.substr(x+1,sentence.length());
				break;
			case 'K':
				// Convert from seconds to 1/100 seconds ONLY if the prev sentence was G04 (delay)
				if (prevSentence != "G04") {
					break;
				}
				x = min(sentence.find_first_of(' ',0), sentence.find_first_of('\n',0));
				sAux = sentence.substr(1,x);
				sAux.ToDouble(&db);
				sAux.Clear();
				sAux<<db * 100;
				sentence = 'K' + sAux;
				break;
			}
			
			// analyze assign commands on remaining line before insert
			int x = line.find("P1=Z",0);
			if (x >= 0) {
				line = line.replace(x,4,"(P100=PPOSZ)");
				sentence.append(line);
				line = "";
			}
			
			if (comments_inserted && sentence[0] != '%') {
				translated.append(sentence);
				prevSentence = sentence;
			}
			if (comments_inserted && sentence[0] == '%')
				translated += ';';
			
		}
		translated += '\n';
		
	}
	//elem->Show(true);
	elem->SetValue(translated);
	elem->SetFocus();
}
