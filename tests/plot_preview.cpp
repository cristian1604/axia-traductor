// Vista previa del graficador (herramienta de desarrollo, no se distribuye).
// Uso: plot_preview <programa> [8025|8035|fanuc]
// Flechas arriba/abajo (y RePág/AvPág) recorren las líneas del programa como
// lo hará el cursor del editor; Fin quita el seguimiento; R muestra u oculta
// los rápidos; P muestra u oculta lo posterior a la línea actual.
#include <wx/wx.h>
#include <wx/ffile.h>
#include "../wxPlotPanel.h"
#include "../CncPath.h"
#include <string>
#include <vector>

class PreviewFrame : public wxFrame {
	wxPlotPanel *m_plot;
	std::vector<wxString> m_lines;
	int m_line = -1;

	void step(int delta) {
		int n = (int) m_lines.size();
		m_line = std::max(-1, std::min(n - 1, m_line + delta));
		m_plot->SetCurrentLine(m_line);
		if (m_line < 0) SetStatusText(wxT("Programa completo (flechas para recorrerlo)"), 1);
		else SetStatusText(wxString::Format(wxT("Línea %d: %s"), m_line + 1, m_lines[m_line]), 1);
	}

	void OnKey(wxKeyEvent &e) {
		switch (e.GetKeyCode()) {
		case WXK_DOWN:     step(1); break;
		case WXK_UP:       step(-1); break;
		case WXK_PAGEDOWN: step(10); break;
		case WXK_PAGEUP:   step(-10); break;
		case WXK_END:      m_line = 0; step(-1); break;
		case 'R': m_plot->SetShowRapids(!m_plot->GetShowRapids()); break;
		case 'P': m_plot->SetShowFuture(!m_plot->GetShowFuture()); break;
		default: e.Skip();
		}
	}

public:
	PreviewFrame(const wxString &file, int standard)
		: wxFrame(NULL, wxID_ANY, wxT("Vista previa del graficador - ") + file, wxDefaultPosition, wxSize(1000, 700)) {
		std::string text;
		wxFFile f(file, wxT("rb"));
		if (f.IsOpened()) {
			size_t n = (size_t) f.Length();
			text.resize(n);
			if (n) f.Read(&text[0], n);
		}
		size_t i = 0;
		while (i <= text.size()) {
			size_t e = text.find('\n', i);
			if (e == std::string::npos) e = text.size();
			std::string line = text.substr(i, e - i);
			if (!line.empty() && line.back() == '\r') line.pop_back();
			m_lines.push_back(wxString::From8BitData(line.c_str(), line.size()));
			if (e >= text.size()) break;
			i = e + 1;
		}

		CncPath path = interpret_cnc(text, standard);
		m_plot = new wxPlotPanel(this);
		m_plot->SetPath(path);

		CreateStatusBar(2);
		int widths[] = { 260, -1 };
		GetStatusBar()->SetStatusWidths(2, widths);
		wxString first;
		for (size_t k = 0; k < path.messages.size(); ++k) {
			if (first.empty() || (path.messages[k].error && !first.StartsWith(wxT("ERROR")))) {
				first = wxString::Format(wxT("%s línea %d: %s"), path.messages[k].error ? wxT("ERROR") : wxT("aviso"),
				                         path.messages[k].line + 1, wxString::FromUTF8(path.messages[k].text.c_str()));
			}
		}
		SetStatusText(wxString::Format(wxT("%d tramos, %d avisos"), (int) path.segments.size(), (int) path.messages.size()), 0);
		SetStatusText(first.empty() ? wxT("Programa completo (flechas para recorrerlo)") : first, 1);

		Bind(wxEVT_CHAR_HOOK, &PreviewFrame::OnKey, this);
	}
};

class PreviewApp : public wxApp {
public:
	bool OnInit() override {
		if (argc < 2) {
			wxMessageBox(wxT("Uso: plot_preview <programa> [8025|8035|fanuc]"), wxT("Vista previa del graficador"));
			return false;
		}
		int standard = FAGOR_8025;
		if (argc > 2) {
			wxString s = argv[2];
			if (s == wxT("8035")) standard = WAS_8035;
			else if (s == wxT("fanuc")) standard = KIA_FANUC;
		}
		PreviewFrame *frame = new PreviewFrame(argv[1], standard);
		frame->Show();
		return true;
	}
};

wxIMPLEMENT_APP(PreviewApp);
