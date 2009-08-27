/* $Id$
 * vim: fdm=marker
 *
 * liblangen (LANguageGENerator)
 * RegEx Random Generator, similar to the one described in section 4.3 of
 * [1] "F. Denis, A. Lemay and A. Terlutte - Learning regular languages using RFSAs"
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <liblangen/regex_randomgenerator.h>
#include <liblangen/prng.h>

#include <stdio.h>

namespace liblangen {

using namespace std;
using namespace prng;

regex_randomgenerator::regex_randomgenerator()
{{{
	seed_prng();
}}}

string regex_randomgenerator::normalized_generate(int & alphabet_size, int num_op, float & p_concat, float & p_union, float & p_star)
{{{
	if(num_op <= 0) {
		// just return a terminal, i.e. an element from the alphabet
		string s;
		s += (char)('a'+random_int(alphabet_size));
		return s;
	}

	float x, p_sum;

	// choose random operator
	x = random_float();
	p_sum = 0.;

	p_sum += p_concat;
	if(x < p_sum) {
		string s1,s2;
		s1 = normalized_generate(alphabet_size, num_op/2, p_concat, p_union, p_star);
		s2 = normalized_generate(alphabet_size, num_op/2, p_concat, p_union, p_star);
		return s1 + s2;
	}

	p_sum += p_union;
	if(x < p_sum) {
		string s1,s2;
		s1 = normalized_generate(alphabet_size, num_op/2, p_concat, p_union, p_star);
		s2 = normalized_generate(alphabet_size, num_op/2, p_concat, p_union, p_star);

		return "(" + s1 + ")|(" + s2 + ")";
	}

	// p_star in any case (normed to 1)
	string s;
	s = normalized_generate(alphabet_size, num_op-1, p_concat, p_union, p_star);

	return "(" + s + ")*";
}}}

string regex_randomgenerator::generate(int alphabet_size, int num_op, float p_concat, float p_union, float p_star)
{{{
	float p_concat_norm;
	float p_union_norm;
	float p_star_norm;
	float p_sum;

	if(p_concat < 0 || p_union < 0 || p_star < 0)
		return "";

	p_sum = p_concat + p_union + p_star;

	p_concat_norm = p_concat / p_sum;
	p_union_norm = p_union / p_sum;
	p_star_norm = p_star / p_sum;

	return normalized_generate(alphabet_size, num_op, p_concat_norm, p_union_norm, p_star_norm);
}}}

}; // end of namespace liblangen

