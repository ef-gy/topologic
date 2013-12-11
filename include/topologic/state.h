/**\file
 * \brief Programme context
 *
 * All of the context that a generic Topologic frontend should need to keep
 * track of is encapsulated in the state class template; this files defines
 * this template and a few others which are closely related to this programme
 * state.
 *
 * \copyright
 * Copyright (c) 2012-2013, Topologic Project Members
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/topologic.git
 */

#if !defined(TOPOLOGIC_STATE_H)
#define TOPOLOGIC_STATE_H

#include <ef.gy/euclidian.h>
#include <ef.gy/polar.h>
#include <ef.gy/projection.h>
#include <ef.gy/colour-space-rgb.h>
#include <sstream>
#include <type_traits>

#include <topologic/render.h>

namespace topologic
{
    template<typename Q, unsigned int d> class state;

    /**\brief Output mode
     *
     * Defines enums for the individual renderers supported by Topologic. These
     * enums are used when creating a renderer at runtime to choose the right
     * renderer type - e.g. SVG or OpenGL.
     */
    enum outputMode
    {
        /**\brief SVG renderer label
         *
         * libefgy's SVG renderer is able to render any given model to simple
         * SVG files, annotated with the settings used to create the model.
         */
        outSVG  = 1,

        /**\brief OpenGL renderer label
         *
         * libefgy's OpenGL renderer has gotten a lot of love lately, and it
         * currently renders directly to any OpenGL 3.2 or higher context,
         * which is expected to be prepared by the frontend. Unlike the SVG
         * render, this one also supports 3D lighting and a Fractal Flame
         * colouring mode.
         */
        outGL   = 2
    };

    /**\brief Topologic global programme state object
     *
     * This is the global programme state object, which contains generic data
     * common to all the frontends. Could very well be a singleton, but it's OK
     * to have more than one.
     *
     * The 'Q' parameter should either be topologic::GLFP or topologic::FP, but
     * you do have quite a bit of flexibility here if you intend to use
     * something fancy for your calculations. The 'd' parameter defines a limit
     * on how 'deep' any renders can end up being, in terms of spatial
     * dimensions.
     *
     * For example, if you primarily intend to render to OpenGL and you know
     * you won't need more than 4 spatial dimensions, you'd use a state<GLFP,4>
     * instance.
     *
     * \note This is a recursive template. Doxygen doesn't like this and will
     *       throw a warning about this, but sod them :P.
     *
     * \tparam Q Base data type for calculations
     * \tparam d Maximum render depth
     */
    template<typename Q, unsigned int d>
    class state : public state<Q,d-1>
    {
    public:
        typedef state<Q,2> base;

        state()
            : projection(typename efgy::geometry::euclidian::space<Q,d>::vector(),
                         typename efgy::geometry::euclidian::space<Q,d>::vector(),
                         Q(M_PI_4),
                         false),
              from(projection.from),
              to(projection.to),
#if !defined(NO_OPENGL)
              opengl(transformation, projection, state<Q,d-1>::opengl),
#endif
              svg(transformation, projection, state<Q,d-1>::svg),
              active(d == 3)
            {
                if (d == 3)
                {
                    fromp[0] = 3;
                    fromp[1] = 1;
                    fromp[2] = 1;
                }
                else
                {
                    fromp[0] = 2;
                    for (int i = 1; i < d; i++)
                    {
                        fromp[i] = 1.57;
                    }
                }
            }

        typename efgy::geometry::polar::space<Q,d>::vector fromp;
        typename efgy::geometry::euclidian::space<Q,d>::vector &from, &to;

        typename efgy::geometry::projection<Q,d> projection;
        typename efgy::geometry::transformation::affine<Q,d> transformation;
        typename efgy::render::svg<Q,d> svg;
#if !defined(NO_OPENGL)
        typename efgy::render::opengl<Q,d> opengl;
#endif
        bool active;

        void updateMatrix (void)
        {
            projection.aspect = (d == 3) ? Q(state<Q,d-1>::width) / Q(state<Q,d-1>::height) : Q(1);
            if (state<Q,d-1>::polarCoordinates)
            {
                from = fromp;
            }
            projection.updateMatrix();
            state<Q,d-1>::updateMatrix();
        }

        const std::string metadata(void) const
        {
            std::stringstream rv;
            rv << "<t:camera";
            if (base::polarCoordinates)
            {
                rv << " radius='" << double(fromp[0]) << "'";
                for (unsigned int i = 1; i < d; i++)
                {
                    rv << " theta-" << i << "='" << double(fromp[i]) << "'";
                }
            }
            else
            {
                for (unsigned int i = 0; i < d; i++)
                {
                    if (i < sizeof(cartesianDimensions))
                    {
                        rv << " " << cartesianDimensions[i] << "='" << double(from[i]) << "'";
                    }
                    else
                    {
                        rv << " d-" << i << "='" << double(from[i]) << "'";
                    }
                }
            }
            rv << "/>";
            rv << "<t:transformation";
            if (isIdentity (transformation.transformationMatrix))
            {
                rv << " matrix='identity' depth='" << d << "'";
            }
            else
            {
                for (unsigned int i = 0; i <= d; i++)
                {
                    for (unsigned int j = 0; j <= d; j++)
                    {
                        rv << " e" << i << "-" << j << "='" << double(transformation.transformationMatrix[i][j]) << "'";
                    }
                }
            }
            rv << "/>";
            return rv.str() + state<Q,d-1>::metadata();
        }

        bool scale (const Q &scale)
        {
            if (!active)
            {
                return state<Q,d-1>::scale(scale);
            }

#if !defined(NO_OPENGL)
            if (d > 3)
            {
                opengl.prepared = false;
            }
#endif

            transformation = transformation
                           * efgy::geometry::transformation::scale<Q,d>(scale);

            return true;
        }

        bool magnify (const Q &magnification)
        {
            if (!active)
            {
                return state<Q,d-1>::magnify(magnification);
            }
            
#if !defined(NO_OPENGL)
            if (d > 3)
            {
                opengl.prepared = false;
            }
#endif

            transformation = transformation
                           * efgy::geometry::transformation::scale<Q,d>(Q(1.) + magnification);

            return true;
        }

        bool interpretDrag (const Q &x, const Q &y, const Q &z)
        {
            if (!active)
            {
                return state<Q,d-1>::interpretDrag(x,y,z);
            }

#if !defined(NO_OPENGL)
            if (d > 3)
            {
                opengl.prepared = false;
            }
#endif

            typename efgy::geometry::euclidian::space<Q,d>::vector fn;

            if (base::polarCoordinates)
            {
                fn = fromp;
            }
            else
            {
                fn = from;
            }

            efgy::geometry::lookAt<Q,d> lookAt(fn, to);
            efgy::geometry::transformation::affine<Q,d> reverseLookAt;
            reverseLookAt.transformationMatrix = transpose(lookAt.transformationMatrix);

            transformation = transformation
                           * lookAt
                           * efgy::geometry::transformation::rotation<Q,d> (x / (Q(M_PI) * Q( 50.)), 0, d-1)
                           * efgy::geometry::transformation::rotation<Q,d> (y / (Q(M_PI) * Q(-50.)), 1, d-1)
                           * reverseLookAt;

            magnify (z / Q(50.));

            return true;
        }

        bool setActive (const unsigned int &dim)
        {
            active = (d == dim);

            return state<Q,d-1>::setActive(dim);
        }

        bool realign (void)
        {
            if (base::polarCoordinates)
            {
                from = fromp;
            }

            if (active)
            {
#if !defined(NO_OPENGL)
                if (d > 3)
                {
                    opengl.prepared = false;
                }
#endif
                Q lb = efgy::geometry::euclidian::lengthSquared<Q,d>(from);

                efgy::geometry::transformation::affine<Q,d> mirror;
                /*
                for (int i = 0; i <= d; i++)
                {
                    mirror.transformationMatrix[i][i] = Q(-1);
                }
                */
                mirror.transformationMatrix[2][2] = Q(-1);

                transformation.transformationMatrix
                    = mirror.transformationMatrix
                    * transformation.transformationMatrix
                    * mirror.transformationMatrix;

                to   = transformation * to;
                from = transformation * from;

                transformation = efgy::geometry::transformation::affine<Q,d>();
                
                Q la = efgy::geometry::euclidian::lengthSquared<Q,d>(from);

                from = from * lb / la;
            }

            return state<Q,d-1>::realign();
        }

        /**\brief Set coordinate of currently active dimension
         *
         * This sets the specified coordinate of the currently 'active'
         * dimension's 'from' point in the currently active source coordinate
         * system to the given value.
         *
         * For example, if transformations are currently being applied to the 4D
         * position, and the from point is currently specified in polar
         * coordinates, and you call this function with coord set to 2 and value
         * set to pi/2, then this will set the 4D from point's theta-2 to pi/2.
         *
         * \param[in] coord The index of the coordinate to set. Bad things
         *                  happen if this is greater than or equal to the
         *                  currently active dimension.
         * \param[in] value What to set this coordinate to.
         *
         * \returns True if the from point was updated successfully, false
         *          otherwise.
         */
        bool setActiveFromCoordinate (const unsigned int &coord, const Q &value)
        {
            if (!active)
            {
                return state<Q,d-1>::setActiveFromCoordinate(coord, value);
            }

            if (coord >= d)
            {
                return false;
            }

#if !defined(NO_OPENGL)
            if (d > 3)
            {
                opengl.prepared = false;
            }
#endif

            if (base::polarCoordinates)
            {
                fromp[coord] = value;
            }
            else
            {
                from[coord] = value;
            }

            return true;
        }
        
        const Q getActiveFromCoordinate (const unsigned int &coord) const
        {
            if (!active)
            {
                return state<Q,d-1>::getActiveFromCoordinate(coord);
            }

            if (coord >= d)
            {
                return Q();
            }

            if (base::polarCoordinates)
            {
                return fromp[coord];
            }
            else
            {
                return from[coord];
            }
        }

        /**\brief Translate from points from polar to cartesian coordinates
         *
         * Sets all the 'from' points' cartesian coordinates to the equivalent
         * point currently specified in polar coordinates.
         *
         * \returns True if the conversions were performed successfully, false
         *          otherwise. Should always be 'true'.
         */
        bool translatePolarToCartesian (void)
        {
            from = fromp;
            return state<Q,d-1>::translatePolarToCartesian();
        }

        /**\brief Translate from points from cartesian to polar coordinates
         *
         * Sets all the 'from' points' polar coordinates to the equivalent point
         * currently specified in cartesian coordinates, i.e. the inverse of
         * translatePolarToCartesian().
         *
         * \todo This function is currently not implemented in libefgy.
         *
         * \returns True if the conversions were performed successfully, false
         *          otherwise.
         */
        bool translateCartesianToPolar (void)
        {
            // fromp = from;
            return state<Q,d-1>::translateCartesianToPolar();
        }
    };

    /**\brief Topologic programme state (2D fix point)
     *
     * Part of the global Topologic state class, this is the 2D fix point and
     * as such contains most of the flags and other state that apply to all 
     * dimensions: output sizes, colours, model parameters, etc.
     *
     * \tparam Q Base data type; should be a class that acts like a rational
     *           base arithmetic type.
     */
    template<typename Q>
    class state<Q,2>
    {
    public:
        state()
            : polarCoordinates(true),
              svg(transformation),
#if !defined(NO_OPENGL)
              opengl(transformation),
#endif
              exportMultiplier(Q(2)),
              lighting(Q(1), Q(1), Q(1), Q(1)),
              background(Q(0.45), Q(0.45), Q(0.65), Q(1)),
              wireframe(Q(1), Q(1), Q(1), Q(1)),
              surface(Q(1), Q(1), Q(1), Q(0.1)),
              model(0),
              idPrefix(""),
              fractalFlameColouring(false)
            {
                parameter.polarRadius       = Q(1);
                parameter.polarPrecision    = Q(10);
                parameter.iterations        = 4;
                parameter.functions         = 3;
                parameter.seed              = 0;
                parameter.preRotate         = true;
                parameter.postRotate        = false;
                parameter.flameCoefficients = 3;
            }

        void updateMatrix (void) const {}

        const std::string metadata(void) const
        {
            std::stringstream rv;

            rv << "<t:camera mode='" << (polarCoordinates ? "polar" : "cartesian") << "'/>";
            if (model)
            {
                rv << "<t:model type='" << model->id() << "' depth='" << model->depth() << "D' render-depth='" << model->renderDepth() << "D'/>";
            }
            rv << "<t:options radius='" << double(parameter.polarRadius) << "' id-prefix='" << idPrefix << "'/>"
               << "<t:precision polar='" << double(parameter.polarPrecision) << "' export-multiplier='" << double(exportMultiplier) << "'/>"
               << "<t:ifs iterations='" << parameter.iterations << "' seed='" << parameter.seed << "' functions='" << parameter.functions << "' pre-rotate='" << (parameter.preRotate ? "yes" : "no") << "' post-rotate='" << (parameter.postRotate ? "yes" : "no") << "'/>"
               << "<t:flame coefficients='" << parameter.flameCoefficients << "'/>"
               << "<t:colour-lighting red='" << double(lighting.red) << "' green='" << double(lighting.green) << "' blue='" << double(lighting.blue) << "' alpha='" << double(lighting.alpha) << "'/>"
               << "<t:colour-background red='" << double(background.red) << "' green='" << double(background.green) << "' blue='" << double(background.blue) << "' alpha='" << double(background.alpha) << "'/>"
               << "<t:colour-wireframe red='" << double(wireframe.red) << "' green='" << double(wireframe.green) << "' blue='" << double(wireframe.blue) << "' alpha='" << double(wireframe.alpha) << "'/>"
               << "<t:colour-surface red='" << double(surface.red) << "' green='" << double(surface.green) << "' blue='" << double(surface.blue) << "' alpha='" << double(surface.alpha) << "'/>";

            return rv.str();
        }

        bool scale (const Q &) const { return false; }
        bool magnify (const Q &) const { return false; }
        bool interpretDrag (const Q &, const Q &, const Q &) const { return true; }
        bool setActive (const unsigned int &) const { return true; }
        bool realign (void)
        {
            polarCoordinates = false;
            return true;
        }
        bool setActiveFromCoordinate (const unsigned int &, const Q &) const { return false; }
        const Q getActiveFromCoordinate (const unsigned int &) const { return Q(); }
        bool translatePolarToCartesian (void) const { return true; }
        bool translateCartesianToPolar (void) const { return false; }

        render::base<true> *model;

        typename efgy::geometry::transformation::affine<Q,2> transformation;
        typename efgy::render::svg<Q,2> svg;
#if !defined(NO_OPENGL)
        typename efgy::render::opengl<Q,2> opengl;
#endif

        bool polarCoordinates;
        efgy::geometry::parameters<Q> parameter;
        Q exportMultiplier;
        typename efgy::colour::RGBA<Q>::value lighting;
        typename efgy::colour::RGBA<Q>::value background;
        typename efgy::colour::RGBA<Q>::value wireframe;
        typename efgy::colour::RGBA<Q>::value surface;
        std::string idPrefix;
        std::stringstream output;
        Q width;
        Q height;

        bool fractalFlameColouring;
    };
};

#endif
