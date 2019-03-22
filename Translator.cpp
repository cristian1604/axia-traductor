#include <string>
#include <wx/textctrl.h>
#include <iostream>
#include <algorithm>
using namespace std;

string block_conversion(string end, string &code, string &replacement);

void translate_8025_to_8035(wxTextCtrl* elem) {
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
		int pos = aux.find('\n');
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
			pos = line.find(' ');
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
				x = sentence.find('.');
				sentence = 'D' + sentence.substr(x+1,sentence.length());
				break;
			case 'K':
				// Convert from seconds to 1/100 seconds ONLY if the prev sentence was G04 (delay)
				if (prevSentence != "G04") {
					break;
				}
				x = min(sentence.find(' ',0), sentence.find('\n',0));
				sAux = sentence.substr(1,x);
				sAux.ToDouble(&db);
				sAux.Clear();
				sAux<<db * 100;
				sentence = 'K' + sAux;
				break;
			}
			
			cout<<line<<endl;
			
			/** FIXED CONVERSIONS **/
			// analyze FIXED assign commands on remaining line before insert
			string str = "P1=Z";
			int x = line.find(str,0);
			if (x >= 0) {
				line = line.replace(x,str.length(),"(P100=PPOSZ)");
				sentence.append(line);
				line = "";
			}
			
			// 'Epilogue' detector. Convert a block of code.
//			str = "P1 = P1 F2 P2";
//			x = line.find(str,0);
//			if (x >= 0) {
//				string rep = "(P100 = P100 - P102)\nM00 M05\nG29 JUMP\nM30";
//				aux = block_conversion("M30", aux, rep);
//				line = "";
//			}
			
			// Sentence "GoTo"
			str = "G29";
			x = line.find(str,0);
			if (x >= 0) {
				line = line.replace(x,str.length(),"(GOTO");
				sentence.append(line + ")");
				line = "";
			}
			
			// Delete Line
			str = "P1 = F11 P10";
			x = line.find(str,0);
			if (x >= 0) {
				line = line.replace(x,str.length(),";");
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
	elem->SetValue(translated);
	elem->SetFocus();
}

/**
	This function converts a block of code. The block begins on the start of &code
	Arguments:
	end = Command to identify end of block (INCLUSIVE)
	code = Source code
	replacement = Block of code to raplce
**/
string block_conversion(string end, string &code, string &replacement) {
	int l_end = code.find(end, 0);
	l_end += end.length();
	if (l_end < 0) {
		return "";
	}
	
	return code.replace(0, l_end, replacement);
}
