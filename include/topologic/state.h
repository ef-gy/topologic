/**\file
 * \brief Programme context
 *
 * All of the context that a generic Topologic frontend should need to keep
 * track of is encapsulated in the state class template; this files defines
 * this template and a few others which are closely related to this programme
 * state.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic
 * \see Project Source Code: https://github.com/ef-gy/topologic
 * \see Licence Terms: https://github.com/ef-gy/topologic/blob/master/COPYING
 */

#if !defined(TOPOLOGIC_STATE_H)
#define TOPOLOGIC_STATE_H

#include <ef.gy/euclidian.h>
#include <ef.gy/polar.h>
#include <ef.gy/projection.h>
#include <ef.gy/colour-space-rgb.h>
#include <ef.gy/maybe.h>
#include <ef.gy/render-xml.h>
#include <ef.gy/render-svg.h>
#include <ef.gy/render-json.h>
#include <ef.gy/render-css.h>
#include <sstream>
#include <type_traits>

#include <topologic/render.h>

namespace topologic {
template <typename Q, unsigned int d> class state;

/**\brief Output mode
 *
 * Defines enums for the individual renderers supported by Topologic. These
 * enums are used when creating a renderer at runtime to choose the right
 * renderer type - e.g. SVG or OpenGL.
 */
enum outputMode {
  /**\brief No output
   *
   * Do not produce any output. This is the default for the CLI programme.
   */
  outNone = -1,

  /**\brief SVG renderer label
   *
   * libefgy's SVG renderer is able to render any given model to simple
   * SVG files, annotated with the settings used to create the model.
   */
  outSVG = 1,

  /**\brief OpenGL renderer label
   *
   * libefgy's OpenGL renderer has gotten a lot of love lately, and it
   * currently renders directly to any OpenGL 3.2 or higher context,
   * which is expected to be prepared by the frontend. Unlike the SVG
   * render, this one also supports 3D lighting and a Fractal Flame
   * colouring mode.
   */
  outGL = 2,

  /**\brief JSON renderer label
   *
   * The JSON output method only produces metadata and not proper image
   * renditions.
   */
  outJSON = 4,

  /**\brief Text metadata label
   *
   * Output is supposed to be a set of arguments, which could be passed to the
   * command line topologic binary.
   */
  outArguments = 5
};

/**\brief Topologic global programme state object
 *
 * This is the global programme state object, which contains generic data
 * common to all the frontends. Could very well be a singleton, but it's OK
 * to have more than one.
 *
 * The 'Q' parameter should either be GLfloat or double, but you do have
 * quite a bit of flexibility here if you intend to use something fancy for
 * your calculations. The 'd' parameter defines a limit on how 'deep' any
 * renders can end up being, in terms of spatial dimensions.
 *
 * For example, if you primarily intend to render to OpenGL and you know
 * you won't need more than 4 spatial dimensions, you'd use a
 * state<GLfloat,4> instance.
 *
 * \note This is a recursive template. Doxygen doesn't like this and will
 *       throw a warning about this, but sod them :P.
 *
 * \tparam Q Base data type for calculations
 * \tparam d Maximum render depth
 */
template <typename Q, unsigned int d> class state : public state<Q, d - 1> {
public:
  /**\brief Parent class type
   *
   * A convenient type alias for this class's parent class. This class is
   * doing a lot of evil recursive template magic, so aliasing the parent
   * class tends to come in handy.
   */
  typedef state<Q, d - 1> parent;

  /**\brief Base (1D) class type
   *
   * Some data is only contained in the 1D version of this class, which
   * this higher-dimensional variant is based on thanks to the power of
   * recursive templates.
   */
  typedef state<Q, 1> base;

  /**\brief Default constructor
   *
   * Sets up default projection and transformation matrices, as well as
   * an instance of libefgy's SVG renderer. If the NO_OPENGL macro has
   * not been defined then this constructor will also set up an instance
   * of libefgy's OpenGL renderer.
   */
  state(void)
      : projection(efgy::math::vector<Q, d>(), efgy::math::vector<Q, d>(),
                   Q(M_PI_4), false),
        from(projection.from), to(projection.to),
#if !defined(NO_OPENGL)
        opengl(transformation, projection, state<Q, d - 1>::opengl),
#endif
        svg(transformation, projection, state<Q, d - 1>::svg), active(d == 3) {
    if (d == 3) {
      fromp[0] = 3;
      fromp[1] = 1;
      fromp[2] = 1;
    } else {
      fromp[0] = 2;
      for (int i = 1; i < d; i++) {
        fromp[i] = 1.57;
      }
    }
  
    from = fromp;
  }

  /**\brief Polar 'from' point
   *
   * Contains the eye coordinates of the camera that is used when
   * creating the projection matrix; This is the set of coordinates that
   * is used when 'polarCoordinates' is set to 'true'.
   */
  efgy::math::vector<Q, d, efgy::math::format::polar> fromp;

  /**\brief Cartesian 'from' point
   *
   * Contains the eye coordinates of the camera that is used when
   * creating the projection matrix; This is the set of coordinates that
   * is used when 'polarCoordinates' is set to 'false'.
   */
  efgy::math::vector<Q, d> &from;

  /**\brief Cartesian 'to' point
   *
   * This is the point that the camera in this dimension is looking at.
   * Right now this should always be the origin, i.e. (0, 0, ...).
   */
  efgy::math::vector<Q, d> &to;

  /**\brief Projective transformation
   *
   * The projective transformation to be applied to any vectors before
   * they're rendered.
   */
  typename efgy::geometry::projection<Q, d> projection;

  /**\brief Viewport transformation matrix
   *
   * An affine transformation which is applied to any vectors being drawn
   * right before applying the projection transformation.
   */
  typename efgy::geometry::transformation::affine<Q, d> transformation;

  /**\brief libefgy SVG renderer instance
   *
   * This is an instance of libefgy's SVG renderer for this template's
   * render depth.
   */
  typename efgy::render::svg<Q, d> svg;

#if !defined(NO_OPENGL)
  /**\brief libefgy OpenGL renderer instance
   *
   * This is an instance of libefgy's OpenGL renderer for this template's
   * render depth.
   */
  typename efgy::render::opengl<Q, d> opengl;
#endif

  /**\brief Update projection matrices
   *
   * Resets the projection matrix's parameters and forces it to be updated
   * with the new parameters, then keeps doing so recursively for all its
   * parent classes.
   *
   * \returns 'true' when matrices have been updated successfully.
   */
  bool updateMatrix(void) {
    projection.aspect =
        (d == 3) ? Q(base::width) / Q(base::height) : Q(1);
    if (base::polarCoordinates) {
      from = fromp;
    }
    projection.updateMatrix();
    return state<Q, d - 1>::updateMatrix();
  }

  bool invalidateCache(void) {
#if !defined(NO_OPENGL)
#if defined(TRANSFORM_4D_IN_PIXEL_SHADER)
    if (d > 4)
#else
    if (d > 3)
#endif
    {
      opengl.context.prepared = false;
    }
#endif

    return true;
  }

  /**\brief Apply scale
   *
   * Applies a scale to the affine transformation matrix of the currently
   * active dimension. If the scale is '1' then the transformation matrix
   * will remain unmodified, if it's greater than '1' then the render
   * output will appear larger and if it's less than '1' the output will
   * appear smaller.
   *
   * \param[in] scale The scale to apply.
   *
   * \returns 'true' if the active matrix was modified, 'false' if there
   *          wasn't any active dimension or if you called the 1D version
   *          of this method.
   */
  bool scale(const Q &scale) {
    if (!active) {
      return state<Q, d - 1>::scale(scale);
    }

    invalidateCache();

    transformation =
        transformation * efgy::geometry::transformation::scale<Q, d>(scale);

    return true;
  }

  /**\brief Apply magnification
   *
   * Applies a scale to the affine transformation matrix of the currently
   * active dimension, very much like the scale() method. Unlike the
   * scale method, this one won't do anything if you pass in '0' instead,
   * things get bigger if they magnification is positive and they get
   * smaller if the magnification is negative.
   *
   * \param[in] magnification The magnification factor to apply.
   *
   * \returns 'true' if the active matrix was modified, 'false' if there
   *          wasn't any active dimension or if you called the 1D version
   *          of this method.
   */
  bool magnify(const Q &magnification) {
    return scale(Q(1.) + magnification);
  }

  efgy::math::vector<Q, d> getFrom(void) const {
    if (base::polarCoordinates) {
      return fromp;
    }

    return from;
  }

  /**\brief Interpret trackball/mouse drag and scroll operations
   *
   * This is a helper function that interprets pointer drag events and
   * modifies the affine transformation matrix accordingly. The 'x' and
   * 'y' parameters correspond to drag events on the 2 pointing device
   * axes, and the 'z' parameter corresponds to scroll events.
   *
   * Dragging to the left and right applies a rotation to the model,
   * whereas scrolling will zoom in and out.
   *
   * \param[in] x Horizontal movement, e.g. dragging from left to right
   * \param[in] y Vertical movement, e.g. dragging from top to bottom
   * \param[in] z Scroll wheel movement
   *
   * \brief 'true' if everything went as expected, 'false' otherwise.
   */
  bool interpretDrag(const Q &x, const Q &y, const Q &z) {
    if (!active) {
      return state<Q, d - 1>::interpretDrag(x, y, z);
    }

    invalidateCache();

    efgy::geometry::lookAt<Q, d> lookAt(getFrom(), to);
    efgy::geometry::transformation::affine<Q, d> reverseLookAt;
    reverseLookAt.transformationMatrix = transpose(lookAt.transformationMatrix);

    transformation = transformation * lookAt *
                     efgy::geometry::transformation::rotation<Q, d>(
                         x / (Q(M_PI) * Q(50.)), 0, d - 1) *
                     efgy::geometry::transformation::rotation<Q, d>(
                         y / (Q(M_PI) * Q(-50.)), 1, d - 1) * reverseLookAt;

    magnify(z / Q(50.));

    return true;
  }

  /**\brief Set active dimension
   *
   * Some of the functions that modify the global state rely on a
   * currently 'active' dimension; this method is used to set this
   * dimension.
   *
   * This method is applied recursively to all of the lower-dimensional
   * state objects, updating each of the instances' 'active' flag.
   *
   * \returns 'true' if updating all of the 'active' flags of the global
   *          state object succeeded. Since there is no way for this
   *          method to fail it will always return 'true'.
   *
   * \note The 1D state object does not have an 'active' flag, so that
   *       particular instance of the method never does anything.
   */
  bool setActive(const unsigned int &dim) {
    active = (d == dim);

    return state<Q, d - 1>::setActive(dim);
  }

  /**\brief Set 'from' coordinate
   *
   * This sets the specified coordinate of the specified dimension's
   * 'from' point in the currently active source coordinate system to the
   * given value.
   *
   * For example, if transformations are currently being applied to the 4D
   * position, and the from point is currently specified in polar
   * coordinates, and you call this function with coord set to 2 and value
   * set to pi/2, then this will set the 4D from point's theta-2 to pi/2.
   *
   * \param[in] coord     The index of the coordinate to set. Bad things
   *                      happen if this is greater than or equal to the
   *                      target dimension.
   * \param[in] value     What to set this coordinate to.
   * \param[in] dimension Target render dimension; uses the active
   *                      dimension if the maybe contains 'nothing'.
   *
   * \returns True if the from point was updated successfully, false
   *          otherwise.
   */
  bool setFromCoordinate(const unsigned int &coord, const Q &value,
                         const efgy::maybe<unsigned int> &dimension =
                             efgy::maybe<unsigned int>()) {
    if (!active && (!dimension || ((unsigned int) dimension != d))) {
      return state<Q, d - 1>::setFromCoordinate(coord, value, dimension);
    }

    if (coord >= d) {
      return false;
    }

    invalidateCache();

    if (base::polarCoordinates) {
      fromp[coord] = value;
    } else {
      from[coord] = value;
    }

    return true;
  }

  /**\brief Query 'from' coordinate
   *
   * Queries the specified dimension's 'from' coordinate with the index
   * specified in 'coord'.
   *
   * \param[in] coord     The index of the coordinate to get. Bad things
   *                      happen if this is greater than or equal to the
   *                      target dimension.
   * \param[in] dimension Target render dimension; uses the active
   *                      dimension if the maybe contains 'nothing'.
   *
   * \returns The specified coordinate; bad things happen if you query a
   *          value that is out of range.
   */
  const Q getFromCoordinate(const unsigned int &coord,
                            const efgy::maybe<unsigned int> &dimension =
                                efgy::maybe<unsigned int>()) const {
    if (!active && (!dimension || ((unsigned int) dimension != d))) {
      return state<Q, d - 1>::getFromCoordinate(coord);
    }

    if (coord >= d) {
      return Q();
    }

    if (base::polarCoordinates) {
      return fromp[coord];
    } else {
      return from[coord];
    }
  }

  /**\brief Translate 'from' points from polar to cartesian coordinates
   *
   * Sets all the 'from' points' cartesian coordinates to the equivalent
   * point currently specified in polar coordinates.
   *
   * \returns True if the conversions were performed successfully, false
   *          otherwise. Should always be 'true'.
   */
  bool translatePolarToCartesian(void) {
    from = fromp;
    return state<Q, d - 1>::translatePolarToCartesian();
  }

  /**\brief Translate 'from' points from cartesian to polar coordinates
   *
   * Sets all the 'from' points' polar coordinates to the equivalent point
   * currently specified in cartesian coordinates, i.e. the inverse of
   * translatePolarToCartesian().
   *
   * \returns True if the conversions were performed successfully, false
   *          otherwise.
   */
  bool translateCartesianToPolar(void) {
    fromp = from;
    return state<Q, d - 1>::translateCartesianToPolar();
  }

  /**\brief Get JSON value
   *
   * Modifies the passed-in value so that it contains the metadata that
   * would be needed to reconstruct this state object. Then returns that
   * value.
   *
   * \param[out] value The JSON value object to modify.
   *
   * \returns The value that was passed in, after it has been modified.
   */
  efgy::json::value<Q> &json(efgy::json::value<Q> &value) const {
    state<Q, d - 1>::json(value);

    if (!state<Q, 1>::model || (d > state<Q, 1>::model->renderDepth)) {
      return value;
    }

    {
      efgy::json::value<Q> v;
      v.toArray();

      for (std::size_t i = 0; i < d; i++) {
        v.push(base::polarCoordinates ? fromp[i] : from[i]);
      }
      value("camera").push(v);
    }

    {
      efgy::json::value<Q> v;
      v.toArray();

      for (std::size_t i = 0; i <= d; i++) {
        for (std::size_t j = 0; j <= d; j++) {
          v.push(transformation.transformationMatrix[i][j]);
        }
      }
      value("transformation").push(v);
    }

    return value;
  }

  /**\brief Get CLI arguments
   *
   * Modifies the passed-in value so that it contains the metadata that
   * would be needed to reconstruct this state object. Then returns that
   * value.
   *
   * \param[out] value The vecotr of CLI arguments to modify.
   *
   * \returns The value that was passed in, after it has been modified.
   */
  std::vector<std::string> &args(std::vector<std::string> &value) const {
    state<Q, d - 1>::args(value);

    if (!state<Q, 1>::model || (d > state<Q, 1>::model->renderDepth)) {
      return value;
    }

    std::stringstream s("");
    bool printFrom = !base::polarCoordinates;

    if (!printFrom) {
      for (std::size_t i = 0; i < d; i++) {
        if (d == 3) {
          printFrom = printFrom || ((i == 0) ? fromp[i] != 3 : fromp[i] != 1);
        } else {
          printFrom =
              printFrom ||
              ((i == 0) ? fromp[i] != 2 : std::abs(fromp[i] - 1.57) > 0.01);
        }
      }
    }

    if (printFrom) {
      s << "f";

      for (std::size_t i = 0; i < d; i++) {
        s << ":" << (base::polarCoordinates ? fromp[i] : from[i]);
      }

      if (base::polarCoordinates) {
        s << ":polar";
      }

      value.push_back(s.str());
      s.str("");
    }

    if (!efgy::math::isIdentity(transformation.transformationMatrix)) {
      s << "t";

      for (std::size_t i = 0; i <= d; i++) {
        for (std::size_t j = 0; j <= d; j++) {
          s << ":" << transformation.transformationMatrix[i][j];
        }
      }

      value.push_back(s.str());
      s.str("");
    }

    return value;
  }

protected:
  /**\brief Is this the currently active dimension?
   *
   * Certain state-modifying methods need to know which dimension is
   * currently 'active', i.e. which dimensions' parameters should be
   * modified. For that reason each instance of a state object has a
   * separate 'active' flag for each of the dimensions.
   *
   * You should only set this flag with the setActive() method.
   */
  bool active;
};

/**\brief Topologic programme state (1D fix point)
 *
 * Part of the global Topologic state class, this is the 1D fix point and
 * as such contains most of the flags and other state that apply to all
 * dimensions: output sizes, colours, model parameters, etc.
 *
 * Additionally, this class provides fix points for some of the recursive
 * functions in the higher level variants of the programme state object.
 *
 * \tparam Q Base data type; should be a class that acts like a rational
 *           base arithmetic type.
 */
template <typename Q> class state<Q, 1> {
public:
  /**\brief Default constructor
   *
   * Constructs an instance of a 1D state object with more-or-less sane
   * defaults.
   */
  state(void)
      : polarCoordinates(true), svg(),
#if !defined(NO_OPENGL)
        opengl(),
#endif
        background(Q(1), Q(1), Q(1), Q(1)), wireframe(Q(0), Q(0), Q(0), Q(0.8)),
        surface(Q(0), Q(0), Q(0), Q(0.2)), fractalFlameColouring(false),
        model(0), autoScaleParameters(false), autoscaleTargetTime(20) {
    parameter.radius = Q(1);
    parameter.precision = Q(10);
    parameter.iterations = 4;
    parameter.functions = 3;
    parameter.seed = 0;
    parameter.preRotate = true;
    parameter.postRotate = false;
    parameter.flameCoefficients = 3;
  }

  /**\brief Destructor
   *
   * Deletes the model instance, if it exists.
   */
  ~state(void) {
    if (model) {
      delete model;
      model = 0;
    }
  }

  /**\brief Update projection matrices; 1D fix point
   *
   * This is the 1D fix point of the state::updateMatrix() method. Since
   * there's not much point in projecting from a 2-space -- which would
   * result in an image in 1-space -- this method is a stub that doesn't
   * do anything.
   *
   * \returns 'true' because there's no 1D matrices to update so that can't
   *          fail.
   */
  constexpr bool updateMatrix(void) const { return true; }

  /**\brief Apply scale; 1D fix point
   *
   * Applies a scale to the affine transformation matrix; since the 1D
   * fix point's transformation matrix is currently being ignored, this
   * particular instance of the method will not do anything at all.
   *
   * \returns 'true' if things went as expected. This 1D fix point should
   *          only be called as a fallback of the higher-level magnify()
   *          methods, so it is not actually expected to ever be called,
   *          which means it'll always return 'false'.
   */
  constexpr bool scale(const Q &) const { return false; }

  /**\brief Apply mouse drag; 1D fix point
   *
   * Applies mouse drag events to the affine transformation matrix; this
   * is the 1D fix point, which doesn't do anything at all as the 1D
   * transformation matrix is currently being ignored.
   *
   * \returns 'true' if things went as expected. Since this function
   *          doesn't do anything that could fail it will always return
   *          'true'.
   */
  constexpr bool interpretDrag(const Q &, const Q &, const Q &) const { return true; }

  /**\brief Set active dimension
   *
   * Some of the functions that modify the global state rely on a
   * currently 'active' dimension; this method is used to set this
   * dimension.
   *
   * This method is applied recursively to all of the lower-dimensional
   * state objects, updating each of the instances' 'active' flag.
   *
   * \returns 'true' if updating all of the 'active' flags of the global
   *          state object succeeded. Since there is no way for this
   *          method to fail it will always return 'true'.
   */
  constexpr bool setActive(const unsigned int &) const { return true; }

  /**\brief Set 'from' coordinate
   *
   * This is the 1D fix point of the setFromCoordinate() method - this
   * instance in particular cannot set anything, as there is no 'from'
   * point in 1D. The arguments are ignored.
   *
   * \returns 'false' because the 1D fix point does not have a 'from'
   *          point, so the function cannot succeed.
   */
  constexpr bool setFromCoordinate(const unsigned int &, const Q &,
                                   const efgy::maybe<unsigned int> & =
                                       efgy::maybe<unsigned int>()) const {
    return false;
  }

  /**\brief Query 'from' coordinate
   *
   * This is the 1D fix point of the getFromCoordinate() method - this
   * instance in particular cannot query anything, as there is no 'from'
   * point in 1D. The arguments are ignored.
   *
   * \returns The default-constructed value of Q, as the 1D fix point does
   *          not have a 'from' point to query.
   */
  constexpr const Q getFromCoordinate(const unsigned int &,
                                      const efgy::maybe<unsigned int> & =
                                          efgy::maybe<unsigned int>()) const {
    return Q();
  }

  bool autoscaleExpectModelChange(void) {
    bool didChangeParameter = false;

    if (autoScaleParameters) {
      parameter.precision = 8;
      didChangeParameter = true;
    }
    
    if (didChangeParameter && model) {
      model->update = true;
    }

    return true;
  }

  bool autoscale(void) {
    bool scaleUp = false;
    bool scaleDown = false;
    bool didChangeParameter = false;

    if (autoScaleParameters) {
#if defined(DEBUG_AUTOSCALE)
      std::cerr << "autoscale factors: "
                << model->renderTime.count() << " "
                << model->initialTime.count() << " "
                << model->prepareTime.count() << " | "
                << autoscaleTargetTime.count() << "\n";
#endif

      auto effectiveTime = model->initialTime + model->renderTime;

      scaleUp = effectiveTime < autoscaleTargetTime * 0.75;
      scaleDown = effectiveTime > autoscaleTargetTime * 2;

#if defined(DEBUG_AUTOSCALE)
      std::cerr << "scaling decision: "
                << scaleDown << " "
                << scaleUp << "\n";
#endif
    }

    if (scaleUp) {
      /*
      if (parameter.iterations < 11) {
        parameter.iterations += 1;
        didChangeParameter = true;
      }
       */

      if (parameter.precision < 100) {
        parameter.precision += 0.5;
        didChangeParameter = true;
      }
    }

    if (scaleDown) {
      /*
      if (parameter.iterations >= 3) {
        parameter.iterations -= 1;
        didChangeParameter = true;
      }
       */

      if (parameter.precision >= 4) {
        parameter.precision -= 1;
        didChangeParameter = true;
      }
    }

    if (didChangeParameter) {
      model->update = true;
    }

    return true;
  }

  /**\brief Translate 'from' points from polar to cartesian coordinates
   *
   * Sets all the 'from' points' cartesian coordinates to the equivalent
   * point currently specified in polar coordinates.
   *
   * \returns True if the conversions were performed successfully, false
   *          otherwise. Should always be 'true'.
   *
   * \note This is the 1D fix point; as there are no 1D 'from' points,
   *       this method does nothing.
   */
  constexpr bool translatePolarToCartesian(void) const { return true; }

  /**\brief Translate 'from' points from cartesian to polar coordinates
   *
   * Sets all the 'from' points' polar coordinates to the equivalent point
   * currently specified in cartesian coordinates, i.e. the inverse of
   * translatePolarToCartesian().
   *
   * \returns True if the conversions were performed successfully, false
   *          otherwise.
   *
   * \note This is the 1D fix point; as there are no 1D 'from' points,
   *       this method does nothing.
   */
  constexpr bool translateCartesianToPolar(void) const { return true; }

  /**\brief Get JSON value (1D fix point)
   *
   * Modifies the passed-in value so that it contains the metadata that
   * would be needed to reconstruct this state object. Then returns that
   * value.
   *
   * \param[out] value The JSON value object to modify.
   *
   * \returns The value that was passed in, after it has been modified.
   */
  efgy::json::value<Q> &json(efgy::json::value<Q> &value) const {
    value.toObject();

    value("polar") = polarCoordinates;
    value("camera").toArray();
    value("transformation").toArray();
    if (model) {
      value("model") = model->id;
      value("depth") = Q(model->depth);
      value("renderDepth") = Q(model->renderDepth);
      value("coordinateFormat") = model->formatID;
    }
    value("radius") = parameter.radius;
    value("minorRadius") = parameter.radius2;
    value("constant") = parameter.constant;
    value("precision") = parameter.precision;
    value("iterations") = Q(parameter.iterations);
    value("seed") = Q(parameter.seed);
    value("functions") = Q(parameter.functions);
    value("preRotate") = parameter.preRotate;
    value("postRotate") = parameter.postRotate;
    value("flameCoefficients") = Q(parameter.flameCoefficients);

    value("background").toArray();
    value("wireframe").toArray();
    value("surface").toArray();

    value("background").push("rgb");
    value("background").push(background.red);
    value("background").push(background.green);
    value("background").push(background.blue);
    value("background").push(background.alpha);
    value("wireframe").push("rgb");
    value("wireframe").push(wireframe.red);
    value("wireframe").push(wireframe.green);
    value("wireframe").push(wireframe.blue);
    value("wireframe").push(wireframe.alpha);
    value("surface").push("rgb");
    value("surface").push(surface.red);
    value("surface").push(surface.green);
    value("surface").push(surface.blue);
    value("surface").push(surface.alpha);

    return value;
  }

  /**\brief Get CLI arguments (1D fix point)
   *
   * Modifies the passed-in value so that it contains the metadata that
   * would be needed to reconstruct this state object. Then returns that
   * value.
   *
   * \param[out] value The vector of arguments to modify.
   *
   * \returns The value that was passed in, after it has been modified.
   */
  std::vector<std::string> &args(std::vector<std::string> &value) const {
    std::stringstream s("");
    if (model) {
      s << "m:" << model->depth << "-" << model->id;
      if (model->renderDepth != 4) {
        s << "@" << model->renderDepth;
      }
      if (std::string(model->formatID) != "cartesian") {
        s << ":" << model->formatID;
      }
      value.push_back(s.str());
      s.str("");
    }

    if ((parameter.radius != 1) || (parameter.radius2 != 0.5)) {
      s << "R:" << parameter.radius;
      if (parameter.radius2 != 0.5) {
        s << ":" << parameter.radius2;
      }
      value.push_back(s.str());
      s.str("");
    }

    if (std::abs(parameter.constant - 0.9) > 0.01) {
      s << "c:" << parameter.constant;
      value.push_back(s.str());
      s.str("");
    }

    if (parameter.precision != 10) {
      s << "p:" << parameter.precision;
      value.push_back(s.str());
      s.str("");
    }

    if (parameter.iterations != 4) {
      s << "i:" << parameter.iterations;
      value.push_back(s.str());
      s.str("");
    }

    if ((parameter.seed != 0) || (parameter.functions != 3) ||
        (parameter.flameCoefficients != 3) || !parameter.preRotate ||
        parameter.postRotate) {
      s << "r:" << parameter.seed << ":" << parameter.functions << ":"
        << parameter.flameCoefficients << (parameter.preRotate ? ":pre" : "")
        << (parameter.postRotate ? ":post" : "");
      value.push_back(s.str());
      s.str("");
    }

    if (fractalFlameColouring) {
      value.push_back("colour:fractal-flame");
    } else {
      s << "colour";
      if ((background.red != 1) || (background.green != 1) ||
          (background.blue != 1) || (background.alpha != 1)) {
        s << ":b:" << background.red << ":" << background.green << ":"
          << background.blue << ":" << background.alpha;
      }
      if ((wireframe.red != 0) || (wireframe.green != 0) ||
          (wireframe.blue != 0) || (std::abs(wireframe.alpha - 0.8) > 0.01)) {
        s << ":w:" << wireframe.red << ":" << wireframe.green << ":"
          << wireframe.blue << ":" << wireframe.alpha;
      }
      if ((surface.red != 0) || (surface.green != 0) || (surface.blue != 0) ||
          (std::abs(surface.alpha - 0.2) > 0.01)) {
        s << ":s:" << surface.red << ":" << surface.green << ":" << surface.blue
          << ":" << surface.alpha;
      }
      if (s.str() != "colour") {
        value.push_back(s.str());
      }
      s.str("");
    }

    return value;
  }

  /**\brief Model renderer instance
   *
   * Points to an instance of a model renderer, e.g.
   * topologic::render::svg with all parameters - including the model -
   * applied and properly initialised.
   */
  render::base *model;

  /**\brief libefgy SVG renderer instance; 1D fix point
   *
   * This is an instance of the 1D fix point of libefgy's SVG renderer.
   */
  typename efgy::render::svg<Q, 1> svg;

#if !defined(NO_OPENGL)
  /**\brief libefgy OpenGL renderer instance; 1D fix point
   *
   * This is an instance of the 1D fix point of libefgy's OpenGL
   * renderer.
   */
  typename efgy::render::opengl<Q, 1> opengl;
#endif

  /**\brief Use polar coordinates?
   *
   * Set to 'true' if the cameras are supposed to use the 'fromp' member
   * as opposed to the 'from' member when calculating a look-at matrix.
   */
  bool polarCoordinates;

  /**\brief Model parameters
   *
   * An instance of the model parameter class that liebfgy uses to keep
   * track of all the parameters used by libefgy primitives.
   */
  efgy::geometry::parameters<Q> parameter;

  /**\brief Background colour
   *
   * The colour that is used as a rendered image's background.
   */
  efgy::math::vector<Q, 4, efgy::math::format::RGB> background;

  /**\brief Wireframe colour
   *
   * The colour to use when rendering the wireframe of a model.
   */
  efgy::math::vector<Q, 4, efgy::math::format::RGB> wireframe;

  /**\brief Surface colour
   *
   * The colour to use when rendering surfaces, i.e. the parts of an
   * image that aren't part of the wirefrome of a model.
   */
  efgy::math::vector<Q, 4, efgy::math::format::RGB> surface;

  /**\brief Viewport width
   *
   * The width, most likely in pixels, of the output viewport that models
   * should be rendered to. Also used to determine the applicable aspect
   * ratio of the output image.
   *
   * \note The SVG renderer ignores this setting and simply sets the
   *       target SVG to have a size of (100%,100%).
   */
  Q width;

  /**\brief Viewport height
   *
   * The height, most likely in pixels, of the output viewport that
   * models should be rendered to. Also used to determine the applicable
   * aspect ratio of the output image.
   *
   * \note The SVG renderer ignores this setting and simply sets the
   *       target SVG to have a size of (100%,100%).
   */
  Q height;

  /**\brief Use fractal frame colouring?
   *
   * 'true' if renderers should render images using the fractal flame
   * colouring algorithm.
   *
   * \note Only applies to the OpenGL renderer.
   *
   * \see http://flam3.com/flame_draves.pdf for the original paper
   *      describing this colouring algorithm.
   */
  bool fractalFlameColouring;

  bool autoScaleParameters;

  std::chrono::milliseconds autoscaleTargetTime;
};

/**\brief Gather model metadata
 *
 * Creates an XML fragment containing all of the settings in this instance
 * of the global state object. Will call its parent class's metadata method
 * as well, which will do the same recursively until finally the 1D fix
 * point method is called.
 *
 * \param[out] stream The XML stream to write to.
 * \param[in]  pState The state to serialise.
 *
 * \returns A new copy of the input stream.
 *
 * \tparam C Character type for the basic_ostream reference.
 * \tparam Q Base data type; should be a class that acts like a rational
 *           base arithmetic type.
 * \tparam d Maximum render depth
 */
template <typename C, typename Q, unsigned int d>
    static inline efgy::xml::ostream<C> operator<<(efgy::xml::ostream<C> stream,
                                                   const state<Q, d> &pState) {
  stream.stream << "<t:camera";
  if (pState.polarCoordinates) {
    stream.stream << " radius='" << double(pState.fromp[0]) << "'";
    for (unsigned int i = 1; i < d; i++) {
      stream.stream << " theta-" << i << "='" << double(pState.fromp[i]) << "'";
    }
  } else {
    for (unsigned int i = 0; i < d; i++) {
      if (i < sizeof(cartesianDimensions)) {
        stream.stream << " " << cartesianDimensions[i] << "='"
                      << double(pState.from[i]) << "'";
      } else {
        stream.stream << " d-" << i << "='" << double(pState.from[i]) << "'";
      }
    }
  }
  stream.stream << "/>";
  stream.stream << "<t:transformation";
  if (isIdentity(pState.transformation.transformationMatrix)) {
    stream.stream << " matrix='identity' depth='" << d << "'";
  } else {
    for (unsigned int i = 0; i <= d; i++) {
      for (unsigned int j = 0; j <= d; j++) {
        stream.stream
            << " e" << i << "-" << j << "='"
            << double(pState.transformation.transformationMatrix[i][j]) << "'";
      }
    }
  }
  stream.stream << "/>";

  return operator<< <C, Q, d - 1>(stream, pState);
}

/**\brief Gather model metadata (1D fix point)
 *
 * Creates an XML fragment containing all of the settings in this instance
 * of the global state object. This particular method will not recurse, much
 * unlike the higher level equivalents.
 *
 * \param[out] stream The XML stream to write to.
 * \param[in]  pState The state to serialise.
 *
 * \returns A new copy of the input stream.
 *
 * \tparam C Character type for the basic_ostream reference.
 * \tparam Q Base data type; should be a class that acts like a rational
 *           base arithmetic type.
 * \tparam d Maximum render depth
 */
template <typename C, typename Q, unsigned int d>
    static inline efgy::xml::ostream<C> operator<<(efgy::xml::ostream<C> stream,
                                                   const state<Q, 1> &pState) {
  stream.stream << "<t:camera mode='"
                << (pState.polarCoordinates ? "polar" : "cartesian") << "'/>";
  if (pState.model) {
    stream.stream << "<t:model type='" << pState.model->id << "' depth='"
                  << pState.model->depth << "D' render-depth='"
                  << pState.model->renderDepth << "D'/>"
                                                  "<t:coordinates format='"
                  << pState.model->formatID << "'/>";
  }
  stream.stream
      << "<t:options radius='" << double(pState.parameter.radius) << "'/>"
      << "<t:precision polar='" << double(pState.parameter.precision) << "'/>"
      << "<t:ifs iterations='" << pState.parameter.iterations << "' seed='"
      << pState.parameter.seed << "' functions='" << pState.parameter.functions
      << "' pre-rotate='" << (pState.parameter.preRotate ? "yes" : "no")
      << "' post-rotate='" << (pState.parameter.postRotate ? "yes" : "no")
      << "'/>"
      << "<t:flame coefficients='" << pState.parameter.flameCoefficients
      << "'/>"
      << "<t:colour-background red='" << double(pState.background.red)
      << "' green='" << double(pState.background.green) << "' blue='"
      << double(pState.background.blue) << "' alpha='"
      << double(pState.background.alpha) << "'/>"
      << "<t:colour-wireframe red='" << double(pState.wireframe.red)
      << "' green='" << double(pState.wireframe.green) << "' blue='"
      << double(pState.wireframe.blue) << "' alpha='"
      << double(pState.wireframe.alpha) << "'/>"
      << "<t:colour-surface red='" << double(pState.surface.red) << "' green='"
      << double(pState.surface.green) << "' blue='"
      << double(pState.surface.blue) << "' alpha='"
      << double(pState.surface.alpha) << "'/>";

  return stream;
}

/**\brief Gather model metadata (JSON)
 *
 * Creates a JSON fragment containing all of the settings in this instance
 * of the global state object. Will call its parent class's metadata method
 * as well, which will do the same recursively until finally the 1D fix
 * point method is called.
 *
 * \param[out] stream The JSON stream to write to.
 * \param[in]  pState The state to serialise.
 *
 * \returns A new copy of the input stream.
 *
 * \tparam C Character type for the basic_ostream reference.
 * \tparam Q Base data type; should be a class that acts like a rational
 *           base arithmetic type.
 * \tparam d Maximum render depth
 */
template <typename C, typename Q, unsigned int d>
    static inline efgy::json::ostream<C> operator<<(efgy::json::ostream<C>
                                                        stream,
                                                    const state<Q, d> &pState) {
  efgy::json::value<Q> v;
  return stream << pState.json(v);
}

/**\brief Render model (SVG)
 *
 * Creates an SVG fragment containing a render of the model associated with the
 * given state object.
 *
 * \param[out] stream The SVG stream to write to.
 * \param[in]  pState The state to serialise.
 *
 * \returns A new copy of the input stream.
 *
 * \tparam C Character type for the basic_ostream reference.
 * \tparam Q Base data type; should be a class that acts like a rational
 *           base arithmetic type.
 * \tparam d Maximum render depth
 */
template <typename C, typename Q, unsigned int d>
    static inline efgy::svg::ostream<C> operator<<(efgy::svg::ostream<C> stream,
                                                   const state<Q, d> &pState) {
  pState.model->svg(stream.stream, true);
  return stream;
}
}

#endif
