/**\file
 * \brief Version information
 *
 * Contains Topologic's version number and assorted pieces of documentation
 * that didn't quite fit in anywhere else.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
 */

#if !defined(TOPOLOGIC_VERSION_H)
#define TOPOLOGIC_VERSION_H

/**\dir include/topologic
 * \brief Topologic template code
 *
 * Like libefgy, Topologic is mostly just a set of C++ template headers; as
 * such, most of the code is contained within the headers themselves, and these
 * headers are located in the include/topologic directory. And just like
 * libefgy, if you intend to create your own frontend then all you need to do
 * is copy or symlink this directory in your own source code repository.
 */

/**\defgroup topologic-frontend Topologic Frontends
 * \brief Programmes that produce actual output
 *
 * At its core, Topologic is more of a library than a single programme. The
 * distribution archives do, however, contain several "frontends" to this
 * library, each with unique capabilities, ranging from very simple command
 * line frontends to fairly complex WebGL clients.
 */

/**\brief Library namespace for the Topologic project
 *
 * This namespace contains all of the classes and functions that make up the
 * actual backend code of Topologic. The frontends then only really need to set
 * up an OpenGL context (if they do render to OpenGL) and handle user I/O.
 */
namespace topologic {
/**\brief Library Version
 *
 * This is the library's version number. Topologic uses a very simple,
 * single-integer versioning scheme: whenever a new version is released,
 * this number is increased by one and used until the next release.
 */
static const unsigned int version = 11;

/**\brief Project web site
 *
 * This is the project's web site, which is displayed and linked to in
 * various frontends. It could change, so there's no reason not to have it
 * as a variable.
 */
static const char website[] = "https://ef.gy/documentation/topologic";

/**\brief Web service base URL
 *
 * To increase availability for users, the project provides a web service
 * that can be used to view anything interesting that you find in any WebGL
 * enabled browser. This is the base URL for this service.
 */
static const char service[] = "https://dee.pe/r";

/**\brief Source code repository
 *
 * All the code is kept in this repository, except for the libefgy templates
 * this code makes use of. Those are linked to from here, however.
 */
static const char repository[] = "https://github.com/ef-gy/topologic";
};

#endif
