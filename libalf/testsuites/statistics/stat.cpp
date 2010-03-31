
#include <iostream>

#include <libalf/statistics.h>
#include <libalf/serialize.h>
#include <libalf/basic_string.h>

using namespace std;
using namespace libalf;

int main()
{
	generic_statistics stat;

	stat["fnord"] = 23;
	stat["pi"] = 4 * atan(1);
	stat["x"] = false;
	stat["y"] = true;
	stat["z"] = "bla";
	stat["Asdf"];

	stat.print(cout);




	basic_string<int32_t> serial;
	serial = ::serialize(stat);
	cout << "\n\n";
	print_basic_string_2hl(serial, cout);
	cout << "\n\n";




	generic_statistics fnord;

	serial_stretch ser(serial.begin(), serial.end());

	if(!::deserialize(fnord, ser))
		cout << "deser failed.\n";

	fnord.print(cout);
	cout << "\n";

	string foo;

	try {
		foo = (string)fnord["z"];
		cout << "\ngot \"" << foo << "\" for >z<.\n";
	}
	catch (exception & e) {
		cerr << "\nexception caught: " << e.what() << "\n";
		return -1;
	}
}

