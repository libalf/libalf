/* $Id$
 * vim: fdm=marker
 *
 * libalf - Automata Learning Factory
 *
 * (c) by David R. Piegdon, i2 Informatik RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 * see LICENSE file for licensing information.
 */

#ifndef __libalf_teacher_h__
# define __libalf_teacher_h__

#include <list>

#include <libalf/statistics.h>
#include <libalf/structured_query_tree.h>
#include <libalf/knowledgebase.h>

namespace libalf {

template <class answer>
class teacher {
	protected:
		statistics * stats;

	public:
		teacher()
		{{{
			  stats = NULL;
		}}}
		virtual ~teacher() { };

		virtual answer membership_query(list<int> & word) = 0;

		virtual void set_statistics_counter(statistics * stats)
		{{{
			this->stats = stats;
		}}}

		virtual void answer_structured_query(structured_query_tree<answer> & sqt)
		{{{
			typename structured_query_tree<answer>::iterator qi;

			for(qi = sqt.begin(); qi != sqt.end(); qi++) {
				int prefix;
				list<int> word;
				list<answer> results;

				word = qi->get_word();

				for(prefix = qi->get_prefix_count(); prefix >= 0; prefix--) {
					results.push_back(this->membership_query(word));
					if(prefix)
						word.pop_back();
				}
				qi->set_answers(results);
			}
		}}}

		virtual void answer_knowledgebase(knowledgebase<answer> & base)
		{{{
			typename knowledgebase<answer>::iterator qi;
			// resolving a query changes the query iterators...
			// we have to walk around this
			qi = base->qbegin();
			while(qi != base->qend()) {
				list<int> word;
				word = qi->get_word();
				qi->set_answer(this->membership_query(word));
				qi = base->qbegin();
			}
		}}}

};

}; // end namespace libalf

#endif

