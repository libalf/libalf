/* $Id$
 * vim: fdm=marker
 *
 * This file is part of libalf.
 *
 * libalf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libalf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libalf.  If not, see <http://www.gnu.org/licenses/>.
 *
 * (c) 2008,2009 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *           and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_filter_h__
# define __libalf_filter_h__

#include <list>
#include <string>

// forward declaration for knowledgebase
namespace libalf {
	template <class answer> class filter;
};

#include <libalf/knowledgebase.h>

namespace libalf {

using namespace std;

template <class answer>
class filter {
	public:
		enum type {
			FILTER_NONE = 0,

			// logical conjunction filters
			FILTER_AND = 1,
			FILTER_OR = 2,
			FILTER_NOT = 3,
			FILTER_ALL_EQUAL = 4,

			// word operation filter
			FILTER_REVERSE = 100,

			// directly evaluating filter
			FILTER_IDENTITY = 200,
		};

		virtual ~filter() { };

		virtual void free_all_subfilter() = 0;

		virtual enum type get_type()
		{ return filter<answer>::FILTER_NONE; };

		virtual bool evaluate(knowledgebase<answer> & base, list<int> & word, answer & result) = 0;

		// layout for serialized filter:
		// int size (of upcoming, including type)
		// int type
		// data[] (of length size - 2)
		virtual basic_string<int32_t> serialize() = 0;
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit) = 0;
};

//////////////////////////////////////////// logical conjunction filters

template <class answer>
class filter_subfilter_array : public filter<answer> {
	protected:
		list<filter<answer>*> subfilter_array;
	public:
		virtual ~filter_subfilter_array()
		{ };
		virtual void free_all_subfilter()
		{{{
			typename list<filter<answer>*>::iterator li;
			while( subfilter_array.end() != (li = subfilter_array.begin()) ) {
				(*li)->free_all_subfilter();
				delete *li;
				subfilter_array.erase(li);
			}
		}}}
		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;
			typename list<filter<answer>*>::iterator li;
			ret += 0; // size, filled in later.
			ret += htonl(this->get_type());
			ret += htonl(subfilter_array.size());
			for(li = subfilter_array.begin(); li != subfilter_array.end(); li++)
				ret += (*li)->serialize();
			ret[0] = htonl(ret.size() - 1);
			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
			// FIXME
			
			return false;
		}
		virtual void add(filter<answer> *f)
		{ subfilter_array.push_back(f); }
		virtual void remove(filter<answer> *f)
		{ subfilter_array.remove(f); }
};


template <class answer>
class filter_and : public filter_subfilter_array<answer> {
	public:
		virtual ~filter_and()
		{ };
		virtual enum filter<answer>::type get_type()
		{ return filter<answer>::FILTER_AND; };
		virtual bool evaluate(knowledgebase<answer> & base, list<int> & word, answer & result)
		{{{
			typename list<filter<answer>*>::iterator li;
			for(li = this->subfilter_array.begin(); li != this->subfilter_array.end(); li++) {
				answer a;
				if( ! (*li)->evaluate(base, word, a)) {
					// filter does not know
					return false;
				} else {
					if( ! ((bool)a)) {
						result = false;
						return true;
					}
				}
			}
			result = true;
			return true;
		}}};
};

template <class answer>
class filter_or : public filter_subfilter_array<answer> {
	public:
		virtual ~filter_or()
		{ };
		virtual enum filter<answer>::type get_type()
		{ return filter<answer>::FILTER_OR; };
		virtual bool evaluate(knowledgebase<answer> & base, list<int> & word, answer & result)
		{{{
			typename list<filter<answer>*>::iterator li;
			for(li = this->subfilter_array.begin(); li != this->subfilter_array.end(); li++) {
				answer a;
				if( ! (*li)->evaluate(base, word, a)) {
					// filter does not know
					// don't care.
				} else {
					if(((bool)a)) {
						result = true;
						return true;
					}
				}
			}
			result = false;
			return true;
		}}};
};

template <class answer>
class filter_not : public filter<answer> {
	protected:
		filter<answer> * subfilter;
	public:
		virtual ~filter_not()
		{ subfilter = NULL; };
		virtual void free_all_subfilter()
		{{{
			if(subfilter) {
				subfilter->free_all_subfilter();
				delete subfilter;
				subfilter = NULL;
			}
		}}}
		virtual enum filter<answer>::type get_type()
		{ return filter<answer>::FILTER_NOT; };
		virtual bool evaluate(knowledgebase<answer> & base, list<int> & word, answer & result)
		{{{
			if(subfilter) {
				if(subfilter->evaluate(base, word, result)) {
					result = ! result;
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			};
		}}}
		virtual void set_subfilter(filter<answer> * f)
		{{{
			subfilter = f;
		}}}
		virtual void remove_subfilter()
		{{{
			subfilter = NULL;
		}}}
		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;
			ret += 0; // size, filled in later.
			ret += htonl(this->get_type());
			ret += subfilter->serialize();
			ret[0] = htonl(ret.size() - 1);
			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
			// FIXME
			
			return false;
		}
};

template <class answer>
class filter_all_equal : public filter_subfilter_array<answer> {
	public:
		virtual ~filter_all_equal()
		{ };
		virtual enum filter<answer>::type get_type()
		{ return filter<answer>::FILTER_ALL_EQUAL; }
		virtual bool evaluate(knowledgebase<answer> & base, list<int> & word, answer & result)
		{{{
			typename list<filter<answer>*>::iterator li;
			answer a;

			if(this->subfilter_array.empty())
				return false;

			li = this->subfilter_array.begin();
			if( ! (*li)->evaluate(base, word, result))
				return false;

			for(li++; li != this->subfilter_array.end(); li++) {
				if( ! (*li)->evaluate(base, word, a))
					return false;
				if(a != result)
					return false;
			}

			return true;
		}}}
};

//////////////////////////////////////////// word operation filter

template <class answer>
class filter_reverse : public filter<answer> {
	// this will reverse the queried word and send it to its subfilters.
	protected:
		filter<answer> * subfilter;
	public:
		virtual ~filter_reverse()
		{ subfilter = NULL; };
		virtual void free_all_subfilter()
		{{{
			if(subfilter) {
				subfilter->free_all_subfilter();
				delete subfilter;
				subfilter = NULL;
			}
		}}}
		virtual enum filter<answer>::type get_type()
		{ return filter<answer>::FILTER_NOT; };
		virtual bool evaluate(knowledgebase<answer> & base, list<int> & word, answer & result)
		{{{
			if(subfilter) {
				list<int> reversed_word;
				typename list<int>::reverse_iterator li;
				for(li = word.rbegin(); li != word.rend(); li++)
					reversed_word.push_back(*li);

				if(subfilter->evaluate(base, reversed_word, result)) {
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			};
		}}}
		virtual void set_subfilter(filter<answer> * f)
		{{{
			subfilter = f;
		}}}
		virtual void remove_subfilter()
		{{{
			subfilter = NULL;
		}}}
		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;
			ret += 0; // size, filled in later.
			ret += htonl(this->get_type());
			ret += subfilter->serialize();
			ret[0] = htonl(ret.size() - 1);
			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
			// FIXME
			
			return false;
		}
};


//////////////////////////////////////////// directly evaluating filters

template <class answer>
class filter_identity : public filter<answer> {
	// this is a pretty stupid filter that will just try to resolve the given word.
	public:
		virtual ~filter_identity()
		{ };
		virtual void free_all_subfilter()
		{ };
		virtual enum filter<answer>::type get_type()
		{ return filter<answer>::FILTER_IDENTITY; };
		virtual bool evaluate(knowledgebase<answer> & base, list<int> & word, answer & result)
		{{{
			typename knowledgebase<answer>::node * n;
			n = base.get_rootptr();
			n = n->find_child(word.begin(), word.end());
			if(n) {
				if(n->is_answered()) {
					result = n->get_answer();
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			};
		}}}
		virtual basic_string<int32_t> serialize()
		{{{
			basic_string<int32_t> ret;
			ret += 0; // size, filled in later.
			ret += htonl(this->get_type());
			ret[0] = htonl(ret.size() - 1);
			return ret;
		}}}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{{{
			if(it == limit) return false;
			if(*it != 2) return false;
			it++;
			if(it == limit) return false;
			if(*it != filter<answer>::FILTER_IDENTITY) return false;
			return true;
		}}}
};

//////////////////////////////////////////// helper functions

template <class answer>
filter<answer> * get_filter_from_type(typename filter<answer>::type t)
{
	switch (t) {
		case filter<answer>::FILTER_AND:
			return new filter_and<answer>;
		case filter<answer>::FILTER_OR:
			return new filter_or<answer>;
		case filter<answer>::FILTER_NOT:
			return new filter_not<answer>;
		case filter<answer>::FILTER_ALL_EQUAL:
			return new filter_all_equal<answer>;
		case filter<answer>::FILTER_REVERSE:
			return new filter_reverse<answer>;
		case filter<answer>::FILTER_IDENTITY:
			return new filter_identity<answer>;
		default:
			// waaaaa!!!
			return NULL;
	};
};


}; // end of namespace libalf

#endif // __libalf_filter_h__

