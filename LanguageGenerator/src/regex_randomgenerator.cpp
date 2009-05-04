/* $Id$
 * vim: fdm=marker
 *
 * LanguageGenerator
 * RegEx Random Generator, similar to the one described in section 4.3 of
 * "F. Denis, A. Lemay and A. Terlutte - Learning regular languages using RFSAs"
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#include <LanguageGenerator/regex_randomgenerator.h>

namespace LanguageGenerator {

using namespace std;

string regex_randomgenerator::normalized_generate(int num_op, int & alphabet_size, float p_sigma[], float & p_epsilon, float & p_concat, float & p_union, float & p_star)
{
	if(num_op <= 0)
		return "";

	//if(
}

string regex_randomgenerator::generate(int num_op, int alphabet_size, float p_sigma[], float p_epsilon, float p_concat, float p_union, float p_star)
{{{
	float p_sigma_norm[alphabet_size];
	float p_epsilon_norm;
	float p_concat_norm;
	float p_union_norm;
	float p_star_norm;
	float p_sum;

	if(p_epsilon < 0 || p_concat < 0 || p_union < 0 || p_star < 0)
		return "";

	p_sum = p_epsilon + p_concat + p_union + p_star;

	for(int i = 0; i < alphabet_size; i++) {
		if(p_sigma < 0)
			return "";
		p_sum += p_sigma[i];
	}
	p_epsilon_norm = p_epsilon / p_sum;
	p_concat_norm = p_concat / p_sum;
	p_union_norm = p_union / p_sum;
	p_star_norm = p_star / p_sum;

	return normalized_generate(num_op, alphabet_size, p_sigma_norm, p_epsilon_norm, p_concat_norm, p_union_norm, p_star_norm);
}}}

}; // end of namespace LanguageGenerator

