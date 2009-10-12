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
 * (c) 2008,2009 by David R. Piegdon, Chair of Computer Science 2 and 7, RWTH-Aachen
 *        <david-i2@piegdon.de>
 *
 */

#ifndef __libalf_logger_terminalcolors_h__
# define __libalf_logger_terminalcolors_h__

# define COLOR(x)      "\x1b[" x "m"
//Type
# define CT_NORMAL  ";0"
# define CT_BOLD    ";1"
// ";2" : unknown
# define CT_REVERSE ";3"
# define CT_ULINE   ";4"
# define CT_BLINK   ";5"
// ";6" : unknown
# define CT_INVERT  ";7"
//Foreground colors
# define CFG_BLACK        ";30"
# define CFG_RED          ";31"
# define CFG_GREEN        ";32"
# define CFG_YELLOW       ";33"
# define CFG_BLUE         ";34"
# define CFG_MAGENTA      ";35"
# define CFG_CYAN         ";36"
# define CFG_WHITE        ";37"
//Background colors
# define CBG_BLACK        ";40"
# define CBG_RED          ";41"
# define CBG_GREEN        ";42"
# define CBG_YELLOW       ";43"
# define CBG_BLUE         ";44"
# define CBG_MAGENTA      ";45"
# define CBG_CYAN         ";46"
# define CBG_WHITE        ";47"
//Reset colors
# define C_RESET     "\x1b[m"
# define C_RESET_NL     C_RESET "\n"

# define C_WARN COLOR(CT_BOLD CFG_YELLOW)
# define C_ERR  COLOR(CT_BOLD CFG_RED)
# define C_ATT  COLOR(CT_BOLD CFG_BLUE)
# define C_OK   COLOR(CT_BOLD CFG_GREEN)
# define C_BOLD COLOR(CT_BOLD)

#endif // __libalf_logger_terminalcolors_h__

