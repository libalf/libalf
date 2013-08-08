#include <libalf/algorithm_kearns_vazirani.h>
#include <libalf/knowledgebase.h>
#include <libalf/alphabet.h>
#include <list>

using namespace libalf;

int main(int argc, char**argv)
{
	knowledgebase<bool> base;
	std::list<int> w;
	
	w = word(2,0,1);
	base.add_knowledge(w, true);
	w = word(3,0,1,1);
	base.add_knowledge(w, false);
	w = word(2,1,1);
	base.add_knowledge(w, false);
	w = word(4,0,1,1,1);
	base.add_knowledge(w, true);
	w = word(4,0,1,0,1);
	base.add_knowledge(w, false);

	kearns_vazirani<bool> alg1(&base, NULL, 2, true);
	kearns_vazirani<bool> alg2(NULL, NULL, 0, false);


	alg1.print(std::cout);

	std::basic_string<int32_t> serial = alg1.serialize();
	//serial_stretch ss(serial);

/*	if(alg2.deserialize(ss))
		std::cout << "done.";
	else
		std::cout << "failed.";
*/
	return 0;
}

