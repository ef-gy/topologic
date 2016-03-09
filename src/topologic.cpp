/**\ingroup topologic-frontend
 * \defgroup frontend-cli CLI frontend
 * \brief Basic command line Topologic frontend
 *
 * This is the very simplest frontend for Topologic, which doesn't do any OpenGL
 * magic at all but rather only provides a scriptable command line interface
 * that produces SVGs on stdout.
 *
 * \{
 */

/**\file
 * \brief Topologic/CLI frontend
 *
 * This is the simplest possible Topologic frontend: a command line application
 * that renders SVG files to stdout.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
 */

#include <topologic/cli.h>

/**\brief Topologic/CLI main function
 *
 * This is really just a stub that calls the topologic::cli function, which
 * contains the actual logic for the Topologic/CLI frontend.
 *
 * \param[in] argc The number of arguments in the argv array.
 * \param[in] argv The actual command line arguments passed to the programme.
 *
 * \returns 0 on success, nonzero otherwise.
 */
int main(int argc, char *argv[]) { return topologic::cli<double>(argc, argv); }

/** \} */
