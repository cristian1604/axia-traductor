#ifndef WXAPPLICATION_H
#define WXAPPLICATION_H

#include <wx/app.h>

class wxApplication : public wxApp {
public:
	virtual bool OnInit();
};

#endif
