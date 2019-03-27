#include <string>
#include <wx/textctrl.h>
#include <iostream>
#include <algorithm>
#include "wxOptions.h"
#include "FileManager.h"
using namespace std;

string block_conversion(string beg, string end, string &code, string &replacement);
void apply_settings(string &code);

void translate_8025_to_8035(wxTextCtrl* elem) {
	string translated;
	string aux = std::string(elem->GetValue() + '\n');
	elem->SetValue("");
	bool program_initiated = false;
	bool comments_inserted = false;
	wxString comments;
	wxString sAux, sAux2, prevSentence;
	int x;
	double db;
	
	// Search CONSTANTS blocks, called prologue and epilogue
	// Remember: the char ` before the line prevent conversion of the sentence
	// Prologue:
	string beg = "P2 = K";
	string end = "G53\n";
	string rep = "G05\n`(P102 = 9.40)\nG40\n`(ORGX 54=0 , ORGZ 54=P100)\nG54\n";
	aux = block_conversion(beg, end, aux, rep);
	// End prologue
	
	// Epilogue
	beg = "P1 = P1 F2 P2";
	end = "P1 = F11 P10";
	aux = block_conversion(beg, end, aux, beg);

	beg = "M05";
	end = "M30";
	block_conversion(beg, end, aux, beg);
	// End convert epilogue
	
	
	while (!aux.empty()) {
		// search on line. Here we got a complete line to be separated by words
		int pos = aux.find('\n');
		string line = aux.substr(0, pos);
		aux = aux.substr(pos+1, aux.length());
		if (!program_initiated && line[0] != '%') {
			comments.Append(';' + line + '\n');
			continue;
		} else {
			program_initiated = true;
		}
		
		if (line.empty()) translated += "  ;";
		
		while (!line.empty()) {
			// iterate by word
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
				// Convert form Taa.bb to Taa Dbb
				x = sentence.find('.');
				sAux2 = "";
				for (int i=0; i < sentence.length()-x; i++) {
					sAux = sentence[i];
					if (sAux.IsNumber()) {
						sAux2<<sentence[i];
					}
				}
				if (sAux2 != "0" && sAux2 != "00") {
					sentence = 'T' + sAux2 + " D" + sentence.substr(x+1,sentence.length());
				} else {
					sentence = 'D' + sentence.substr(x+1,sentence.length());
				}
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
			case '`':
				// Internal symbol of preserve line (non standard, only on this editor)
				sentence = sentence.substr(1,sentence.length());
				break;
			}
			
			/** FIXED CONVERSIONS **/
			// analyze FIXED assign commands on remaining line before insert
			string str = "P1=Z";
			int x = line.find(str,0);
			if (x >= 0) {
				line = line.replace(x,str.length(),"(P100=PPOSZ)");
				sentence.append(line);
				line = "";
			}
			
			// Sentence "GoTo"
			str = "G29";
			x = line.find(str,0);
			if (x >= 0) {
				line = line.replace(x,str.length(),"(GOTO");
				sentence.append(line + ")");
				line = "";
			}
			
			// Delete Line
			str = "P1 = P1 F2 P2";
			x = line.find(str,0);
			if (x >= 0) {
				line = line.replace(x,str.length(),"(P100 = P100 - P102)");
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
	apply_settings(translated);
	elem->SetValue(translated);
	elem->SetFocus();
}

/**
	This function converts a block of code. The block begins on the start of &code
	Arguments:
	beg = First command on block start
	end = Last command to identify end of block (INCLUSIVE)
	code = Source code
	replacement = Block of code to raplce
**/
string block_conversion(string beg, string end, string &code, string &replacement) {
	int l_beg = code.find(beg);
	if (l_beg < 0)
		return code;
	int l_end = code.find(end, l_beg);
	if (l_end < 0 || l_end < l_beg) {
		return code;
	}
	l_end += end.length();
	return code.replace(l_beg, l_end-l_beg, replacement);
}

/**  APPLY SETTINGS RULES  **/
/** Application of conversion rules on settings file 
	For example: Remove M08 instruction, or replace spining direction (M03 or M04)
**/
void apply_settings(string &code) {
	s_Settings s;
	FileManager F;
	if (F.loadSettings(s)) {
		wxString w_code(code);
		if (s.replace_from && s.replace_to) {
			w_code.Replace(s.replace_from, s.replace_to, true);
		}
		if (s.remove_m08) {
			w_code.Replace("M08", "", true);
		}
		code = w_code;
	}
}
