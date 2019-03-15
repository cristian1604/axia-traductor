#ifndef PARAMETERS_H
#define PARAMETERS_H
#include <string>
using namespace std;

class Parameters {
private:
	bool erase_m08;
	string replace_src;
	//string replace_src;
public:
	Parameters();

	void setEraseM08(bool state);
	void replace(string from, string by);
};

#endif

