/*
 * This file is part of the Topologic project.
*/

/*
 * Copyright (c) 2012-2013, ef.gy Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#if !defined(TOPOLOGIC_STATE_H)
#define TOPOLOGIC_STATE_H

#include <ef.gy/euclidian.h>
#include <ef.gy/polar.h>
#include <ef.gy/projection.h>
#include <ef.gy/colour-space-rgb.h>
#if !defined(NO_OPENGL)
#include <ef.gy/render-opengl.h>
#endif
#include <ef.gy/render-svg.h>
#include <type_traits>
#include <sstream>

namespace topologic
{
    static const char cartesianDimensions[] = "xyzwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";

    template<typename Q, unsigned int d> class state;

    class renderer
    {
    public:
        virtual ~renderer(void) {}
        virtual std::stringstream &operator () (bool updateMatrix = false) = 0;
        virtual unsigned int depth (void) const = 0;
        virtual unsigned int renderDepth (void) const = 0;
        virtual const char *id (void) const = 0;
        virtual std::string name (void) const = 0;
    };

    class empty
    {
    };

    template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T, unsigned int rd = d, bool isVirtual = false>
    class renderSVG : public std::conditional<isVirtual, renderer, empty>::type
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

        std::stringstream &operator () (bool updateMatrix = false)
        {
            if (updateMatrix)
            {
                gState.S::updateMatrix();
            }

            gState.S2::svg.output.str("");

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
                 " path#" << gState.S2::idPrefix << "wireframe { stroke-width: 0.002; fill: none; stroke: rgba(" << double(gState.S2::wireframe.red)*100. << "%," << double(gState.S2::wireframe.green)*100. << "%," << double(gState.S2::wireframe.blue)*100. << "%," << double(gState.S2::wireframe.alpha) << "); }"
                 " path { stroke: none; fill: rgba(" << double(gState.S2::surface.red)*100. << "%," << double(gState.S2::surface.green)*100. << "%," << double(gState.S2::surface.blue)*100. << "%," << double(gState.S2::surface.alpha) << "); }</style>";
            object.renderSolid();
            gState.S2::svg.output << "<path id='" << gState.S2::idPrefix << "wireframe' d='";
            object.renderWireframe();
            gState.S2::svg.output << "'/></svg>\n";
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

    protected:
        S &gState;
        P object;
    };

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
              svg(transformation, projection, state<Q,d-1>::svg)
            {
                fromp.data[0] = 2;
                for (int i = 1; i < d; i++)
                {
                    fromp.data[i] = 1.57;
                }
            }

        typename efgy::geometry::polar::space<Q,d>::vector fromp;
        typename efgy::geometry::euclidian::space<Q,d>::vector &from, &to;

        typename efgy::geometry::perspectiveProjection<Q,d> projection;
        typename efgy::geometry::transformation<Q,d> transformation;
#if !defined(NO_OPENGL)
        typename efgy::render::opengl<Q,d> opengl;
#endif
        typename efgy::render::svg<Q,d> svg;

        void updateMatrix (void)
        {
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
                rv << " radius='" << double(fromp.data[0]) << "'";
                for (int i = 1; i < d; i++)
                {
                    rv << " theta-" << i << "='" << double(fromp.data[i]) << "'";
                }
            }
            else
            {
                for (int i = 0; i < d; i++)
                {
                    if (i < sizeof(cartesianDimensions))
                    {
                        rv << " " << cartesianDimensions[i] << "='" << double(from.data[i]) << "'";
                    }
                    else
                    {
                        rv << " d-" << i << "='" << double(from.data[i]) << "'";
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
    };

    template<typename Q>
    class state<Q,3> : public state<Q,2>
    {
    public:
        typedef state<Q,2> base;

        state()
            : projection(typename efgy::geometry::euclidian::space<Q,3>::vector(),
                         typename efgy::geometry::euclidian::space<Q,3>::vector(),
                         Q(M_PI_4),
                         false),
              from(projection.from),
              to(projection.to),
#if !defined(NO_OPENGL)
              opengl(transformation),
#endif
              svg(transformation, projection, base::svg)
            {
                fromp.data[0] = 3;
                fromp.data[1] = 1;
                fromp.data[2] = 1;
            }

        typename efgy::geometry::polar::space<Q,3>::vector fromp;
        typename efgy::geometry::euclidian::space<Q,3>::vector &from, &to;

        typename efgy::geometry::perspectiveProjection<Q,3> projection;
        typename efgy::geometry::transformation<Q,3> transformation;
#if !defined(NO_OPENGL)
        typename efgy::render::opengl<Q,3> opengl;
#endif
        typename efgy::render::svg<Q,3> svg;

        void updateMatrix (void)
        {
            if (base::polarCoordinates)
            {
                from = fromp;
            }
            projection.updateMatrix();
        }

        const std::string metadata(void) const
        {
            std::stringstream rv;
            rv << "<t:camera";
            if (base::polarCoordinates)
            {
                rv << " radius='" << double(fromp.data[0]) << "'";
                for (int i = 1; i < 3; i++)
                {
                    rv << " theta-" << i << "='" << double(fromp.data[i]) << "'";
                }
            }
            else
            {
                for (int i = 0; i < 3; i++)
                {
                    rv << " " << cartesianDimensions[i] << "='" << double(from.data[i]) << "'";
                }
            }
            rv << "/>";
            return rv.str() + base::metadata();
        }

        template<unsigned int md, template <class,unsigned int,class,unsigned int> class T, unsigned int rd = md>
        renderer * getModel (void)
        {
            return new renderSVG<Q,md,T,rd,true>(*this);
        }
    };

    template<typename Q>
    class state<Q,2>
    {
    public:
        state()
            : polarCoordinates(true),
              svg(transformation),
              exportMultiplier(Q(2)),
              background(Q(0.45), Q(0.45), Q(0.65), Q(1)),
              wireframe(Q(1), Q(1), Q(1), Q(1)),
              surface(Q(1), Q(1), Q(1), Q(0.1)),
              model(0),
              idPrefix("")
            {
                parameter.polarRadius    = Q(1);
                parameter.polarPrecision = Q(10);
            }

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
               << "<t:colour-background red='" << double(background.red) << "' green='" << double(background.green) << "' blue='" << double(background.blue) << "' alpha='" << double(background.alpha) << "'/>"
               << "<t:colour-wireframe red='" << double(wireframe.red) << "' green='" << double(wireframe.green) << "' blue='" << double(wireframe.blue) << "' alpha='" << double(wireframe.alpha) << "'/>"
               << "<t:colour-surface red='" << double(surface.red) << "' green='" << double(surface.green) << "' blue='" << double(surface.blue) << "' alpha='" << double(surface.alpha) << "'/>";

            return rv.str();
        }

        renderer *model;

        typename efgy::geometry::transformation<Q,2> transformation;
        typename efgy::render::svg<Q,2> svg;

        bool polarCoordinates;
        efgy::geometry::parameters<Q> parameter;
        Q exportMultiplier;
        typename efgy::colour::RGBA<Q>::value background;
        typename efgy::colour::RGBA<Q>::value wireframe;
        typename efgy::colour::RGBA<Q>::value surface;
        std::string idPrefix;
    };
};

#endif
