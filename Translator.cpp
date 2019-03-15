#include <string>
#include <wx/textctrl.h>
using namespace std;

void translate_8025_to_8035(wxTextCtrl* elem) {
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
				break;
			case 'A':
				sentence[0] = 'Q';
				break;
			case '(':
				sentence = ';' + sentence + line;
				break;
			case '#':
				sentence = sentence + line;
				line = "";
				break;
			}
			elem->AppendText(sentence);
		}
		elem->AppendText('\n');
	}
	//elem->Show(true);
	elem->SetFocus();
}
