/**\file
 * \brief Basic CLI frontend
 *
 * Contains the basic Topologic CLI frontend, which is limited to processing
 * SVG files. It does demonstrate rather succinctly how to use the library,
 * however, and creating SVGs from the command line is always a neat feature.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
 */

#if !defined(TOPOLOGIC_CLI_H)
#define TOPOLOGIC_CLI_H

/**!\brief Disable OpenGL renderer
 *
 * OpenGL requires the created code to be linked to the system's OpenGL
 * libraries; this macro disables OpenGL support, and in doing so it allows the
 * resulting programme to be used in environments that do not support OpenGL.
 */
#define NO_OPENGL

#include <topologic/arguments.h>

#if !defined(MAXDEPTH)
/**\brief Maximum render depth
 *
 * This macro is used by some of the frontends to determine the maximum render
 * depth supported by a frontend. The default value is '7', which is plenty for
 * most applications - increasing this value will increase the size of the
 * generated code, so it may be desirable to decrease this value in
 * environments with tighter constraints.
 */
#define MAXDEPTH 7
#endif

namespace topologic {
/**\brief Default CLI frontend main function
 *
 * Main function for a typical CLI-/SVG-only frontend. This is part of the
 * library code so that it's easy to reuse where applicable.
 *
 * \tparam FP Floating point data type to use; something like double
 *
 * \param[in] argc The number of arguments that are being passed in argv.
 * \param[in] argv The actual argument vector. The first element must be
 *                 the name the programme was called as, the remainder are
 *                 command line flags.
 *
 * \returns 0 if the function ran correctly, nonzero otherwise.
 */
template <typename FP> int cli(int argc, char *argv[]) {
  state<FP, MAXDEPTH> topologicState;
  std::vector<std::string> args;

  for (std::size_t i = 0; i < argc; i++) {
    args.push_back(argv[i]);
  }

  enum outputMode out = parse(topologicState, args);

  if (!topologicState.model) {
    std::cerr << "error: no model to render\n";
  } else if (out == outSVG) {
    std::cout << efgy::svg::tag() << topologicState;
  } else if (out == outJSON) {
    std::cout << efgy::json::tag() << topologicState;
  } else if (out == outArguments) {
    std::vector<std::string> v;
    std::cout << "topologic";
    for (const auto &arg : topologicState.args(v)) {
      std::cout << " " << arg;
    }
    std::cout << "\n";
  }

  return 0;
}
}

#endif
