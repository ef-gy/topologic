/**\file
 * \brief Render context
 *
 * The individual renderers provided by libefgy have very similar interfaces,
 * but there are some minor differences that need to be mangled around a bit
 * so that topologic can just substitute one renderer for another. The classes
 * in this file provide that mangling.
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

#if !defined(TOPOLOGIC_RENDER_H)
#define TOPOLOGIC_RENDER_H

#include <topologic/state.h>
#include <ef.gy/render-svg.h>
#if !defined(NO_OPENGL)
#include <ef.gy/render-opengl.h>
#endif

namespace topologic
{
    /**\brief Cartesian dimension shorthands
     *
     * As you'll probably remember form high school, it's customary to label
     * first three dimensions in a euclidian coordinate space "x", "y" and "z".
     * If you've had a poke at OpenGL or two, you'll also remember that the
     * fourth coordinate is often called "w". But what about the remaining "n"
     * dimensions? Well, we could just use "dimension-N", but that's kind of
     * dull, and since Topologic could, in theory, use a lot more dimensions
     * than 4, this array here defines how these dimensions are labelled - e.g.
     * in XML metadata fragments.
     */
    static const char cartesianDimensions[] = "xyzwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";

    template<typename Q, unsigned int d> class state;

    namespace render
    {
        /**\brief Base class for a model renderer
         *
         * The primary purpose of this class is to force certain parts of a
         * model renderer's interface to be virtual.
         *
         * \tparam isVirtual Whether the derived class should contain virtual
         *                   methods.
         */
        template<bool isVirtual = false>
        class base
        {
            public:
                /**\brief Virtual destructor
                 *
                 * Generally necessary for virtual classes; stubbed to be a
                 * trivial destructor.
                 */
                virtual ~base(void) {}

                /**\brief Render model
                 *
                 * Processes the model with the renderer. Depending on the
                 * output, this may or may not produce output directly on
                 * screen or fill a provided stringstream.
                 *
                 * \param[in] updateMatrix Whether to update the projection
                 *                         matrices.
                 *
                 * \returns A stringstream which will either be empty or
                 *          contain the generated data.
                 */
                virtual std::stringstream &render (bool updateMatrix = false) = 0;

                /**\brief Query model depth
                 *
                 * Used to access the model depth; this is typically a template
                 * parameter.
                 *
                 * \returns The model depth; expect values like "2" for a
                 *          square, "3" for a cube, etc.
                 */
                virtual unsigned int depth (void) const = 0;

                /**\brief Query render depth
                 *
                 * Used to access the depth that the renderer has been
                 * initialised to.
                 *
                 * \returns The model renderer's depth; expect this value to be
                 *          greater than or equal to the model's depth.
                 */
                virtual unsigned int renderDepth (void) const = 0;

                /**\brief Query model name
                 *
                 * Used to obtain a short, descriptive name of a model. This
                 * name is also used when instantiating the model with a
                 * factory.
                 *
                 * \returns A C-style, 0-terminated string containing the name
                 *          of the model. This should never return a 0-pointer.
                 */
                virtual const char *id (void) const = 0;

                /**\brief Query extended model name
                 *
                 * This returns a string of the form "depth()-id()", e.g.
                 * "4-cube" for a 4D model with the id "cube".
                 *
                 * \returns A C++ std::string containing the model's name.
                 */
                virtual std::string name (void) const = 0;

                /**\brief Force internal update
                 *
                 * This tells a renderer that it should do a full redraw,
                 * because you changed some parameters that it may have cached.
                 */
                virtual void update (void) = 0;
        };

        /**\brief Non-virtual model renderer base class
         *
         * This is simply an empty class, which allows a model renderer to be
         * non-virtual, which in turn would probably be handy in certain
         * situations where only a very select few models will be used and it
         * would be a good idea to highly optimise the renderers for these
         * models; by cutting down on the number of virtual functions, the
         * compiler should be able to provide slightly better code.
         */
        template<>
        class base<false> {};

        /**\brief Renderer base class with default methods
         *
         * This template provides some of the basic functionality shared
         * between distinct model renderers which aren't being provided by the
         * renderers in libefgy, or which only need to be passed along.
         *
         * \tparam Q  Base data type for calculations
         * \tparam d  Model depth; typically has to be <= the render depth
         * \tparam T  Model template; use things like efgy::geometry::cube
         * \tparam R  Model renderer template; use things like efgy::render::svg
         * \tparam rd Model render depth; this is the maximum depth for your
         *            models and also specifies the maximum depth of any
         *            transformations you can apply.
         * \tparam isVirtual Whether the class should contain the virtual
         *                   functions defined in renderer<true>. It'll contain
         *                   the actual functions in there either way, this
         *                   just determines if they should make the class a
         *                   virtual class.
         */
        template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T, template <class,unsigned int> class R, unsigned int rd, bool isVirtual>
        class render : public base<isVirtual>
        {
            public:
                typedef T<Q,d,R<Q,rd>,rd > modelType;
                typedef R<Q,rd> renderType;

                unsigned int depth (void) const
                {
                    return modelType::depth();
                };

                unsigned int renderDepth (void) const
                {
                    return modelType::renderDepth();
                };

                const char *id (void) const
                {
                    return modelType::id();
                };

                std::string name (void) const
                {
                    std::stringstream rv;
                    rv << depth() << "-" << id();
                    return rv.str();
                }
        };

        /**\brief SVG model renderer
         *
         * This is a wrapper for libefgy's SVG renderer, augmented with some
         * code to write out model parameters and use Topologic's state object
         * to handle these parameters.
         *
         * \tparam Q  Base data type for calculations
         * \tparam d  Model depth; typically has to be <= the render depth
         * \tparam T  Model template; use things like efgy::geometry::cube
         * \tparam rd Model render depth; this is the maximum depth for your
         *            models and also specifies the maximum depth of any
         *            transformations you can apply.
         * \tparam isVirtual Whether the class should contain the virtual
         *                   functions defined in renderer<true>. It'll contain
         *                   the actual functions in there either way, this
         *                   just determines if they should make the class a
         *                   virtual class.
         */
        template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T, unsigned int rd = d, bool isVirtual = false>
        class renderSVG : public render<Q,d,T,efgy::render::svg,rd,isVirtual>
        {
            public:
                typedef render<Q,d,T,efgy::render::svg,rd,isVirtual> parent;

                using parent::name;

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

                void update (void)
                {
                    object.calculateObject();
                }

            protected:
                S &gState;
                typename parent::modelType object;
        };

#if !defined (NO_OPENGL)
        /**\brief OpenGL model renderer
         *
         * This is a wrapper for libefgy's OpenGL renderer, augmented with some
         * code to write out model parameters and use Topologic's state object
         * to handle these parameters.
         *
         * \tparam Q  Base data type for calculations
         * \tparam d  Model depth; typically has to be <= the render depth
         * \tparam T  Model template; use things like efgy::geometry::cube
         * \tparam rd Model render depth; this is the maximum depth for your
         *            models and also specifies the maximum depth of any
         *            transformations you can apply.
         * \tparam isVirtual Whether the class should contain the virtual
         *                   functions defined in renderer<true>. It'll contain
         *                   the actual functions in there either way, this
         *                   just determines if they should make the class a
         *                   virtual class.
         */
        template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T, unsigned int rd = d, bool isVirtual = false>
        class renderGL : public render<Q,d,T,efgy::render::opengl,rd,isVirtual>
        {
            public:
                typedef render<Q,d,T,efgy::render::opengl,rd,isVirtual> parent;

                typedef state<Q,rd> S;
                typedef state<Q,2> S2;

                renderGL(S &pState)
                    : gState(pState),
                      object(gState.S::opengl,
                             gState.S2::parameter,
                             gState.S2::exportMultiplier)
                    {
                        gState.opengl.prepared = false;
                    }

                renderGL(S &pState, const efgy::geometry::parameters<Q> &pParameter)
                    : gState(pState),
                      object(gState.S::opengl,
                             pParameter,
                             Q(1))
                    {
                        gState.opengl.prepared = false;
                    }

                renderGL(S &pState, const efgy::geometry::parameters<Q> &pParameter, const Q &pMultiplier)
                    : gState(pState),
                      object(gState.S::opengl,
                             pParameter,
                             pMultiplier)
                    {
                        gState.opengl.prepared = false;
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

                void update (void)
                {
                    gState.opengl.prepared = false;
                    object.calculateObject();
                }

            protected:
                S &gState;
                typename parent::modelType object;
        };
#endif
    };

    template<bool isVirtual>
    using renderer = render::base<isVirtual>;

    template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T, unsigned int rd, bool isVirtual>
    using renderSVG = render::renderSVG<Q,d,T,rd,isVirtual>;

#if !defined (NO_OPENGL)
    template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T, unsigned int rd, bool isVirtual>
    using renderGL = render::renderGL<Q,d,T,rd,isVirtual>;
#endif
};

#endif
