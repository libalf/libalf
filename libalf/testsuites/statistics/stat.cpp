
#include <iostream>

#include <libalf/statistics.h>
#include <libalf/serialize.h>
#include <libalf/basic_string.h>

using namespace std;
using namespace libalf;

int main()
{
	generic_integer_statistics stat;

	stat["fnord"] = 23;

	basic_string<int32_t> serial;

	serial = ::serialize(stat);

	stat.print(cout);

	cout << "\n";
	print_basic_string_2hl(serial, cout);
	cout << "\n";




	generic_integer_statistics fnord;

	serial_stretch s(serial.begin(), serial.end());

	if(!::deserialize(fnord, s))
		cout << "deser failed.\n";

	fnord.print(cout);
	cout << "\n";
}

