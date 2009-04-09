
#include <libalf/knowledgebase.h>
#include <amore/vars.h>

using namespace std;
using namespace libalf;

int main()
{
	knowledgebase<bool> base;
	knowledgebase<bool>::iterator ki;
	list<int> w;

	for(ki = base.qbegin(); ki != base.qend(); ki++) {

		ki->get_word();

		base.add_knowledge(w, true);
	}

	list<int>::iterator wi;
	for(wi = w.begin(); wi != w.end(); wi++) {
		int c;
		c = *wi;
	}

	return 0;
}

