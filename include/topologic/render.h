/**\file
 * \brief Render context
 *
 * The individual renderers provided by libefgy have very similar interfaces,
 * but there are some minor differences that need to be mangled around a bit
 * so that topologic can just substitute one renderer for another. The classes
 * in this file provide that mangling.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
 */

#if !defined(TOPOLOGIC_RENDER_H)
#define TOPOLOGIC_RENDER_H

#include <ef.gy/render-svg.h>
#if !defined(NO_OPENGL)
#include <ef.gy/render-opengl.h>
#endif

namespace topologic {
/**\brief Cartesian dimension shorthands
 *
 * As you'll probably remember from high school, it's customary to label the
 * first three dimensions in a euclidian coordinate space "x", "y" and "z".
 * If you've had a poke at OpenGL or two, you'll also remember that the
 * fourth coordinate is often called "w". But what about the remaining "n"
 * dimensions? Well, we could just use "dimension-N", but that's kind of
 * dull, and since Topologic could, in theory, use a lot more dimensions
 * than 4, this array here defines how these dimensions are labelled - e.g.
 * in XML metadata fragments.
 */
static const char cartesianDimensions[] =
    "xyzwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";

template <typename Q, std::size_t d> class state;

/**\brief Templates related to Topologic's rendering process
 *
 * This namespace encompasses all of the templates related to topologic's
 * actual rendering process, as opposed to state management or setup tasks.
 */
namespace render {
/**\brief Model metadata
 *
 * Holds all the common model metadata that is needed to identify a
 * model type.
 */
class metadata {
public:
  /**\brief Construct with model metadata
   *
   * Sets the basic metadata for a model.
   */
  metadata(std::size_t pDepth = 0, std::size_t pRenderDepth = 0,
           const char *pID = "none", const char *pFormatID = "default")
      : depth(pDepth), renderDepth(pRenderDepth), id(pID), formatID(pFormatID),
        update(true) {}

  /**\brief Query model depth
   *
   * Used to access the model depth; this is typically a template
   * parameter.
   *
   * \returns The model depth; expect values like "2" for a
   *          square, "3" for a cube, etc.
   */
  const std::size_t depth;

  /**\brief Query render depth
   *
   * Used to access the depth that the renderer has been
   * initialised to.
   *
   * \returns The model renderer's depth; expect this value to be
   *          greater than or equal to the model's depth.
   */
  const std::size_t renderDepth;

  /**\brief Query model name
   *
   * Used to obtain a short, descriptive name of a model. This
   * name is also used when instantiating the model with a
   * factory.
   *
   * \returns A C-style, 0-terminated string containing the name
   *          of the model. This should never return a 0-pointer.
   */
  const char *id;

  /**\brief Query extended model name
   *
   * This returns a string of the form "depth()-id()", e.g.
   * "4-cube" for a 4D model with the id "cube".
   *
   * \returns A C++ std::string containing the model's name.
   */
  std::string name(void) const {
    std::stringstream rv;
    rv << metadata::depth << "-" << id;
    return rv.str();
  }

  /**\brief Query vector format ID
   *
   * Used to obtain a string that identifies the vector format
   * currently used by the model.
   *
   * \returns Vector format ID string.
   */
  const char *formatID;

  /**\brief Force internal update
   *
   * This tells a renderer that it should do a full redraw,
   * because you changed some parameters that it may have cached.
   */
  bool update;
};

/**\brief Base class for a model renderer
 *
 * The primary purpose of this class is to force certain parts of a model
 * renderer's interface to be virtual, and to provide an interface that is used
 * topologic to render its output.
 */
class base : public metadata {
public:
  using metadata::metadata;

  /**\brief Virtual destructor
   *
   * Generally necessary for virtual classes; stubbed to be a
   * trivial destructor.
   */
  virtual ~base(void) {}

  /**\brief Render to SVG
   *
   * This is a wrapper for libefgy's SVG renderer, augmented with
   * some code to write out model parameters and use Topologic's
   * state object to handle these parameters.
   *
   * \param[in] output       The stream to write to.
   * \param[in] updateMatrix Whether to update the projection
   *                         matrices.
   *
   * \returns 'true' upon success.
   */
  virtual bool svg(std::ostream &output, bool updateMatrix = false) = 0;

#if !defined(NO_OPENGL)
  /**\brief Render to OpenGL context
   *
   * This is a wrapper for libefgy's OpenGL renderer.
   *
   * \param[in] updateMatrix Whether to update the projection
   *                         matrices.
   *
   * \returns 'true' upon success.
   */
  virtual bool opengl(bool updateMatrix = false) = 0;
#endif
};

/**\brief Renderer base class with default methods
 *
 * This template provides some of the basic functionality shared
 * between distinct model renderers which aren't being provided by the
 * renderers in libefgy, or which only need to be passed along.
 *
 * \tparam Q      Base data type for calculations
 * \tparam d      Model depth; typically has to be <= the render depth
 * \tparam T      Model template; use things like efgy::geometry::cube
 * \tparam format The vector format to use.
 */
template <typename Q, std::size_t d, template <class, std::size_t> class T, typename format>
class wrapper : public base {
public:
  /**\brief Model type
   *
   * Alias of the model type that this class represents with all
   * the template parameters filled in.
   */
  using modelType = T<Q, d>;

  /**\brief Global state type
   *
   * Alias of the global state type that this class uses to
   * gather information to render a model with all the template
   * parameters filled in.
   */
  using stateType = state<Q, modelType::renderDepth>;

  /**\brief Construct with global state and renderer
   *
   * Sets the object up with a global state object and an
   * appropriate renderer instance. The parameters and export
   * multipliers passed to the model are the default ones
   * provided by the global state object.
   *
   * \param[in,out] pState  The global topologic::state instance
   * \param[in]     pFormat The vector format tag to use
   */
  wrapper(stateType &pState, const format &pFormat)
      : gState(pState), object(gState.parameter, pFormat),
        base(d, modelType::renderDepth, modelType::id(),
             modelType::format::id()) {}

  bool svg(std::ostream &output, bool updateMatrix = false) {
    if (metadata::update) {
      metadata::update = false;
    }

    if (updateMatrix) {
      gState.width = 3;
      gState.height = 3;
      gState.updateMatrix();
    }

    gState.svg.frameStart();

    output << "<?xml version='1.0' encoding='utf-8'?>"
              "<svg xmlns='http://www.w3.org/2000/svg'"
              " xmlns:xlink='http://www.w3.org/1999/xlink'"
              " version='1.1' width='100%' height='100%' viewBox='-1.2 -1.2 "
              "2.4 2.4'>"
              "<title>" +
                  metadata::name() +
                  "</title>"
                  "<metadata xmlns:t='http://ef.gy/2012/topologic'>"
           << efgy::xml::tag() << gState;
    output << "</metadata>"
              "<style type='text/css'>svg { background: rgba("
           << double(gState.background.red) * 100. << "%,"
           << double(gState.background.green) * 100. << "%,"
           << double(gState.background.blue) * 100. << "%,"
           << double(gState.background.alpha)
           << "); }"
              " path { stroke-width: 0.002; stroke: rgba("
           << double(gState.wireframe.red) * 100. << "%,"
           << double(gState.wireframe.green) * 100. << "%,"
           << double(gState.wireframe.blue) * 100. << "%,"
           << double(gState.wireframe.alpha) << ");"
                                                " fill: rgba("
           << double(gState.surface.red) * 100. << "%,"
           << double(gState.surface.green) * 100. << "%,"
           << double(gState.surface.blue) * 100. << "%,"
           << double(gState.surface.alpha) << "); }</style>";
    if (gState.surface.alpha > Q(0.)) {
      output << gState.svg << object;
    }
    output << "</svg>\n";

    gState.svg.frameEnd();

    return true;
  }

#if !defined(NO_OPENGL)
  bool opengl(bool updateMatrix = false) {
    if (metadata::update) {
      gState.opengl.context.prepared = false;
      metadata::update = false;
    }

    if (updateMatrix) {
      gState.updateMatrix();
    }

    gState.opengl.context.fractalFlameColouring = gState.fractalFlameColouring;
    gState.opengl.context.width = gState.width;
    gState.opengl.context.height = gState.height;

    if (!gState.fractalFlameColouring) {
      glClearColor(gState.background.red, gState.background.green,
                   gState.background.blue, gState.background.alpha);
    }

    gState.opengl.frameStart();

    gState.opengl.context.wireframeColour = gState.wireframe;
    gState.opengl.context.surfaceColour = gState.surface;

    if (!gState.opengl.context.prepared) {
      std::cerr << gState.opengl << object;
    }

    gState.opengl.frameEnd();

    return true;
  }
#endif

protected:
  /**\brief Global state object
   *
   * A reference to the global state object, which was passed to
   * the constructor as 'pState'. This is necessary to keep track
   * of updated global settings.
   */
  stateType &gState;

  /**\brief Intrinsic object instance
   *
   * Contains the instance of the model that this renderer is
   * trying to create a representation of.
   */
  modelType object;
};
}
}

#endif
