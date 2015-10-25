/**\file
 * \brief Command line argument parsing
 *
 * The different frontends tend to receive command line arguments and this
 * header provides the topologic::parse function to parse said command line
 * arguments.
 *
 * \copyright
 * Copyright (c) 2012-2015, Topologic Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 */

#if !defined(TOPOLOGIC_ARGUMENTS_H)
#define TOPOLOGIC_ARGUMENTS_H

#include <iostream>
#include <fstream>
#include <cmath>

#include <ef.gy/cli.h>
#include <ef.gy/version.h>
#include <ef.gy/parametric.h>

#include <topologic/parse.h>
#include <topologic/version.h>

namespace topologic {
/**\brief Parse command line arguments
 *
 * A function template to parse C-style command line arguments, apply the
 * settings in those arguments to a topologic::state instance. This
 * function will also parse XML files that have been passed in as command
 * line arguments and set the model in the state object to match that
 * specified as command line arguments or in XML files.
 *
 * As usual, later options override earlier ones - that also applies to
 * settings in XML files. If the NOLIBRARIES macro is set then XML files
 * will not be processed.
 *
 * \tparam Q   Base data type as used in the topologic::state instance
 * \tparam dim Maximum render depth of the topologic::state instance
 *
 * \param[out] topologicState The topologic::state instance to populate
 * \param[in]  args           Command line argument vector.
 * \param[in]  readFiles      Try to treat unrecognised options as files.
 *
 * \returns The output mode set in the argument vector. Defaults to outNone.
 */
template <typename Q, unsigned int dim>
enum outputMode parse(state<Q, dim> &topologicState,
                      const std::vector<std::string> &args,
                      bool readFiles = true) {
  enum outputMode out = outNone;

#if !defined(NOLIBRARIES)
  topologic::xml XML;
#endif
  unsigned int depth = 4, rdepth = 4;
  std::string model = "cube";
  std::string format = "cartesian";

  efgy::cli::option oversion("-{0,2}version", [](std::smatch &)->bool {
    std::cout << "Topologic/V" << version << "\n"
                                             "libefgy/V" << efgy::version
              << "\n"
                 "Maximum render depth of this binary is " << dim
              << " dimensions.\n"
                 "Supported models:";
    std::set<const char *> models;
    for (const char *m :
         efgy::geometry::with<Q, efgy::geometry::functor::models, dim>(
             models, "*", 0, 0)) {
      std::cout << " " << m;
    }
    std::cout << "\n"
                 "Supported vector coordinate formats:";
    std::set<const char *> formats;
    for (const char *f :
         efgy::geometry::with<Q, efgy::geometry::functor::formats, dim>(
             formats, "*", "*", 0, 0)) {
      std::cout << " " << f;
    }
    std::cout << "\n";
    return true;
  },
                             "Print version information.");

  efgy::cli::option omodel(
      "-{0,2}m(odel)?:([0-9]+)-([a-z-]+)(@([0-9]+))?(:([a-z]+))?",
      [&depth, &rdepth, &model, &format](std::smatch & m)->bool {
    depth = std::stoi(m[2]);
    model = m[3];
    if (m[5] != "") {
      rdepth = std::stoi(m[5]);
    }
    if (m[7] != "") {
      format = m[7];
    }
    return true;
  },
      "Sets all the model type parameters. The form is: D-MODEL[@R][:FORMAT], "
      "e.g. 3-cube@4:polar. The default is 4-cube@4:cartesian.");

  efgy::cli::option oformat("-{0,2}(none|json|svg|arguments)",
                            [&out](std::smatch & m)->bool {
    if (m[1] == "json") {
      out = topologic::outJSON;
    } else if (m[1] == "svg") {
      out = topologic::outSVG;
    } else if (m[1] == "arguments") {
      out = topologic::outArguments;
    } else {
      out = topologic::outNone;
    }
    return true;
  },
                            "Select an output format.");

  efgy::cli::option oifs(
      "-{0,2}r(andom)?:([0-9]+)(:([0-9]+))?(:([0-9]+))?(:pre)?(:post)?",
      [&topologicState](std::smatch & m)->bool {
    topologicState.state<Q, 2>::parameter.seed = Q(std::stold(m[2]));
    if (m[4] != "") {
      topologicState.state<Q, 2>::parameter.functions = Q(std::stold(m[4]));
    }
    if (m[6] != "") {
      topologicState.state<Q, 2>::parameter.flameCoefficients =
          Q(std::stold(m[6]));
    }
    topologicState.state<Q, 2>::parameter.preRotate = (m[7] == ":pre");
    topologicState.state<Q, 2>::parameter.postRotate = (m[8] == ":post");
    return true;
  },
      "Set parameters for randomised models. The order of the arguments is: "
      "seed[:functions][:variants][:pre][:post]. Only the seed is required to "
      "be set.");

  efgy::cli::option ocolour("-{0,2}colour(:fractal-flame|"
                            "(:b:([0-9.]+):([0-9.]+):([0-9.]+):([0-9.]+))?"
                            "(:w:([0-9.]+):([0-9.]+):([0-9.]+):([0-9.]+))?"
                            "(:s:([0-9.]+):([0-9.]+):([0-9.]+):([0-9.]+))?)",
                            [&topologicState](std::smatch & m)->bool {
    topologicState.state<Q, 2>::fractalFlameColouring =
        (m[1] == ":fractal-flame");
    if (m[2] != "") {
      topologicState.state<Q, 2>::background.red = Q(std::stold(m[3]));
      topologicState.state<Q, 2>::background.green = Q(std::stold(m[4]));
      topologicState.state<Q, 2>::background.blue = Q(std::stold(m[5]));
      topologicState.state<Q, 2>::background.alpha = Q(std::stold(m[6]));
    }
    if (m[7] != "") {
      topologicState.state<Q, 2>::wireframe.red = Q(std::stold(m[8]));
      topologicState.state<Q, 2>::wireframe.green = Q(std::stold(m[9]));
      topologicState.state<Q, 2>::wireframe.blue = Q(std::stold(m[10]));
      topologicState.state<Q, 2>::wireframe.alpha = Q(std::stold(m[11]));
    }
    if (m[12] != "") {
      topologicState.state<Q, 2>::surface.red = Q(std::stold(m[13]));
      topologicState.state<Q, 2>::surface.green = Q(std::stold(m[14]));
      topologicState.state<Q, 2>::surface.blue = Q(std::stold(m[15]));
      topologicState.state<Q, 2>::surface.alpha = Q(std::stold(m[16]));
    }
    return true;
  },
                            "Set the colour scheme to use.");

  efgy::cli::option oradius("-{0,2}(R|radius):([0-9.]+)(:([0-9.]+))?",
                            [&topologicState](std::smatch & m)->bool {
    topologicState.state<Q, 2>::parameter.radius = Q(std::stold(m[2]));
    if (m[4] != "") {
      topologicState.state<Q, 2>::parameter.radius2 = Q(std::stold(m[4]));
    }
    return true;
  },
                            "Set the radii used in some formulas.");

  efgy::cli::option oparam(
      "-{0,2}(p|precision|c|constant):([0-9.]+)",
      [&topologicState](std::smatch & m)->bool {
    if ((m[1] == "precision") || (m[1] == "p")) {
      topologicState.state<Q, 2>::parameter.precision = Q(std::stold(m[2]));
    } else if ((m[1] == "constant") || (m[1] == "c")) {
      topologicState.state<Q, 2>::parameter.constant = Q(std::stold(m[2]));
    }
    return true;
  },
      "Set the precision, or the constant factor for some formulae.");

  efgy::cli::option oiterations(
      "-{0,2}(i|iterations):([0-9]+)",
      [&topologicState](std::smatch & m)->bool {
    topologicState.state<Q, 2>::parameter.iterations = Q(std::stoll(m[2]));
    return true;
  },
      "Set the number of iterations for iterative formulae.");

  efgy::cli::option ofrom(
      "-{0,2}f(rom)?((:[0-9.]+){2,})(:polar)?",
      [&topologicState](std::smatch & m)->bool {
    topologicState.state<Q, 2>::polarCoordinates = (m[4] == ":polar");
    std::istringstream s(m[2]);
    std::string coord;
    std::vector<Q> v;

    while (std::getline(s, coord, ':')) {
      if (coord != "") {
        v.push_back(Q(std::stold(coord)));
      }
    }

    for (std::size_t i = 0; i < v.size(); i++) {
      topologicState.setFromCoordinate(i, v[i], v.size());
    }

    return true;
  },
      "Set a from point of the transformation. Which of the from points is set "
      "depends on the number of coordinates given. The polar suffix treats the "
      "input as polar coordinates.");

  efgy::cli::option otransform(
      "-{0,2}t(ransform)?((:[0-9.]+){2,})",
      [&topologicState](std::smatch & m)->bool {
    std::istringstream s(m[2]);
    std::string coord;
    std::vector<Q> v;

    while (std::getline(s, coord, ':')) {
      if (coord != "") {
        v.push_back(Q(std::stold(coord)));
      }
    }

    const std::size_t d = std::sqrt(v.size());
    if (v.size() != (d * d)) {
      return false;
    }

    for (std::size_t i = 0, x = 0; x < d; x++) {
      for (std::size_t y = 0; y < d; y++) {
        setMatrixCell(topologicState, d - 1, x, y, v[i]);
        i++;
      }
    }

    return true;
  },
      "Set a tranformation matrix. Which of the matrices is set depends on the "
      "number of coordinates given.");

  efgy::cli::options<>::common().apply(args);

  if (readFiles) {
    for (const auto &f : efgy::cli::options<>::common().remainder) {
      std::ifstream in(f);
      std::istreambuf_iterator<char> eos;
      std::string s(std::istreambuf_iterator<char>(in), eos);

#if !defined(NOLIBRARIES)
      xml::parser p(s, f);
      if (p.valid) {
        parse(topologicState, p);
        parseModel<Q, dim, updateModel>(topologicState, p);
      } else
#endif
          {
        efgy::json::value<> v;
        s >> v;
        parse(topologicState, v);
        parseModel<Q, dim, updateModel>(topologicState, v);
      }

      if (topologicState.model) {
        format = topologicState.model->formatID;
        model = topologicState.model->id;
        depth = topologicState.model->depth;
        rdepth = topologicState.model->renderDepth;
      }
    }
  }

  if (!topologicState.model ||
      !((format == topologicState.model->formatID) &&
        (model == topologicState.model->id) &&
        (depth == topologicState.model->depth) &&
        (rdepth == topologicState.model->renderDepth))) {
    efgy::geometry::with<Q, updateModel, dim>(topologicState, format, model,
                                              depth, rdepth);
  }

  return out;
}
}

#endif
