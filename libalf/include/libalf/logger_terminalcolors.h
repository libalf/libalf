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

