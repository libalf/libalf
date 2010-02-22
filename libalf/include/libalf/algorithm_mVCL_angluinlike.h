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
 * (c) 2010 Lehrstuhl Softwaremodellierung und Verifikation (I2), RWTH Aachen University
 *      and Lehrstuhl Logik und Theorie diskreter Systeme (I7), RWTH Aachen University
 * Author: David R. Piegdon <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_algorithm_mvcl_angluinlike_h__
# define __libalf_algorithm_mvcl_angluinlike_h__

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <ostream>
#include <sstream>

#include <stdio.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include <libalf/alphabet.h>
#include <libalf/logger.h>
#include <libalf/learning_algorithm.h>

namespace libalf {

using namespace std;

template <class answer>
class mVCL_angluinlike : public learning_algorithm<answer> {
	public: // types

	protected: // data

	public: // methods
		virtual void increase_alphabet_size(int new_asize)
		{
			
		}
		virtual memory_statistics get_memory_statistics()
		{
			
		}

		virtual bool sync_to_knowledgebase()
		{{{
			(*this->my_logger)(LOGGER_ERROR, "mVCL_angluinlike does not support sync-operation.\n");
			return false;
		}}}
		virtual bool supports_sync()
		{ return false; };

		virtual basic_string<int32_t> serialize()
		{
			
		}
		virtual bool deserialize(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit)
		{
			
		}
		bool deserialize_magic(basic_string<int32_t>::iterator &it, basic_string<int32_t>::iterator limit, basic_string<int32_t> & result)
		{

		}
		virtual void print(ostream &os)
		{
			
		}
		virtual string tostring()
		{
			
		}
		virtual bool conjecture_ready()
		{
			
		}
		virtual bool add_counterexample(list<int>)
		{
			
		}
	protected: // methods
		virtual bool complete()
		{
			
		}
		virtual conjecture * derive_conjecture()
		{
			
		}
}


}; // end of namespace libalf

#endif

