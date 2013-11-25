/**\file
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
#include <ef.gy/render-svg.h>
#if !defined(NO_OPENGL)
#include <ef.gy/render-opengl.h>
#endif
#include <sstream>

namespace topologic
{
    static const char cartesianDimensions[] = "xyzwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";

    template<typename Q, unsigned int d> class state;

    enum outputMode
    {
        outSVG  = 1,
        outGL   = 2
    };

    class renderer
    {
    public:
        virtual ~renderer(void) {}
        virtual std::stringstream &render (bool updateMatrix = false) = 0;
        virtual unsigned int depth (void) const = 0;
        virtual unsigned int renderDepth (void) const = 0;
        virtual const char *id (void) const = 0;
        virtual std::string name (void) const = 0;
        virtual void update (void) = 0;
    };

    class empty
    {
    };

    template<bool B, typename T, typename F> class conditional { public: typedef T type; };
    template<typename T, typename F> class conditional<false, T, F> { public: typedef F type; };

    template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T, unsigned int rd = d, bool isVirtual = false>
    class renderSVG : public conditional<isVirtual, renderer, empty>::type
    {
    public:
        typedef T<Q,d,efgy::render::svg<Q,rd>,rd > P;
        typedef state<Q,rd> S;
        typedef state<Q,2> S2;

        renderSVG(S &pState)
            : gState(pState),
              object(gState.S::svg,
                     gState.S2::parameter,
                     gState.S2::exportMultiplier)
            {}

        renderSVG(S &pState, const efgy::geometry::parameters<Q> &pParameter)
            : gState(pState),
              object(gState.S::svg,
                     pParameter,
                     gState.S2::exportMultiplier)
            {}

        renderSVG(S &pState, const efgy::geometry::parameters<Q> &pParameter, const Q &pMultiplier)
            : gState(pState),
              object(gState.S::svg,
                     pParameter,
                     pMultiplier)
            {}

        std::stringstream &render (bool updateMatrix = false)
        {
            if (updateMatrix)
            {
                gState.S2::width  = 3;
                gState.S2::height = 3;
                gState.S::updateMatrix();
            }

            gState.S::svg.frameStart();

            gState.S2::svg.reset();

            gState.S2::svg.output
              << "<?xml version='1.0' encoding='utf-8'?>"
                 "<svg xmlns='http://www.w3.org/2000/svg'"
                 " xmlns:xlink='http://www.w3.org/1999/xlink'"
                 " version='1.1' width='100%' height='100%' viewBox='-1.2 -1.2 2.4 2.4'>"
                 "<title>" + name() + "</title>"
                 "<metadata xmlns:t='http://ef.gy/2012/topologic'>"
              << gState.metadata()
              << "</metadata>"
                 "<style type='text/css'>svg { background: rgba(" << double(gState.S2::background.red)*100. << "%," <<double(gState.S2::background.green)*100. << "%," << double(gState.S2::background.blue)*100. << "%," << double(gState.S2::background.alpha) << "); }"
                 " path { stroke-width: 0.002; stroke: rgba(" << double(gState.S2::wireframe.red)*100. << "%," << double(gState.S2::wireframe.green)*100. << "%," << double(gState.S2::wireframe.blue)*100. << "%," << double(gState.S2::wireframe.alpha) << ");"
                 " fill: rgba(" << double(gState.S2::surface.red)*100. << "%," << double(gState.S2::surface.green)*100. << "%," << double(gState.S2::surface.blue)*100. << "%," << double(gState.S2::surface.alpha) << "); }</style>";
            if (gState.S2::surfacesEnabled && (gState.S2::surface.alpha > Q(0.)))
            {
                object.renderSolid();
            }
            gState.S2::svg.output << "</svg>\n";

            gState.S::svg.frameEnd();

            return gState.S2::svg.output;
        }

        unsigned int depth (void) const { return P::depth(); };
        unsigned int renderDepth (void) const { return P::renderDepth(); };
        const char *id (void) const { return P::id(); };
        std::string name (void) const
        {
            std::stringstream rv;
            rv << depth() << "-" << id();
            return rv.str();
        }
        void update (void) { object.calculateObject(); }

    protected:
        S &gState;
        P object;
    };

#if !defined (NO_OPENGL)
    template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T, unsigned int rd = d, bool isVirtual = false>
    class renderGL : public conditional<isVirtual, renderer, empty>::type
    {
    public:
        typedef T<Q,d,efgy::render::opengl<Q,rd>,rd > P;
        typedef state<Q,rd> S;
        typedef state<Q,2> S2;

        renderGL(S &pState)
            : gState(pState),
              object(gState.S::opengl,
                     gState.S2::parameter,
                     gState.S2::exportMultiplier)
            {
                gState.S::opengl.reset();
            }

        renderGL(S &pState, const efgy::geometry::parameters<Q> &pParameter)
            : gState(pState),
              object(gState.S::opengl,
                     pParameter,
                     Q(1))
            {
                gState.S::opengl.reset();
            }

        renderGL(S &pState, const efgy::geometry::parameters<Q> &pParameter, const Q &pMultiplier)
            : gState(pState),
              object(gState.S::opengl,
                     pParameter,
                     pMultiplier)
            {
                gState.S::opengl.reset();
            }

        std::stringstream &render (bool updateMatrix = false)
        {
            if (updateMatrix)
            {
                gState.S::updateMatrix();
            }

            gState.S::opengl.fractalFlameColouring = gState.fractalFlameColouring;
            gState.S::opengl.width  = gState.width;
            gState.S::opengl.height = gState.height;

            if (!gState.fractalFlameColouring)
            {
                glClearColor
                    (gState.S2::background.red, gState.S2::background.green,
                     gState.S2::background.blue, gState.S2::background.alpha);
            }
            
            gState.S::opengl.frameStart();
            
            gState.S2::output.str("");

            if (gState.fractalFlameColouring)
            {
                gState.opengl.setColour(0,0,0,0.5,true);
                gState.opengl.setColour(0,0,0,0.8,false);
            }
            else
            {
                gState.opengl.setColour
                    (gState.S2::wireframe.red, gState.S2::wireframe.green,
                     gState.S2::wireframe.blue, gState.S2::wireframe.alpha,
                     true);
                gState.opengl.setColour
                    (gState.S2::surface.red, gState.S2::surface.green,
                     gState.S2::surface.blue, gState.S2::surface.alpha,
                     false);
            }

            if (!gState.S::opengl.prepared)
            {
                object.renderSolid();
            }

            gState.S::opengl.frameEnd();

            return gState.S2::output;
        }

        unsigned int depth (void) const { return P::depth(); };
        unsigned int renderDepth (void) const { return P::renderDepth(); };
        const char *id (void) const { return P::id(); };
        std::string name (void) const
        {
            std::stringstream rv;
            rv << depth() << "-" << id();
            return rv.str();
        }
        void update (void)
        {
            gState.S::opengl.reset();
            object.calculateObject();
        }

    protected:
        S &gState;
        P object;
    };
#endif

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

        template<unsigned int md, template <class,unsigned int,class,unsigned int> class T, unsigned int rd = md>
        renderer * getModel (void)
        {
            return new renderSVG<Q,md,T,rd,true>(*this);
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
                opengl.reset();
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
                opengl.reset();
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
                opengl.reset();
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
                    opengl.reset();
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
                opengl.reset();
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

        bool translatePolarToCartesian (void)
        {
            from = fromp;
            return state<Q,d-1>::translatePolarToCartesian();
        }

        bool translateCartesianToPolar (void)
        {
            // TODO: implement this transformation
            // fromp = from;
            return state<Q,d-1>::translateCartesianToPolar();
        }
    };

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
              lightingEnabled(true),
              surfacesEnabled(true),
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
        bool translateCartesianToPolar (void) const { return true; }

        renderer *model;

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
        bool lightingEnabled;
        bool surfacesEnabled;

        bool fractalFlameColouring;
    };
};

#endif
