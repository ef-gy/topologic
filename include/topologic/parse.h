/**\file
 * \brief Input parsing
 *
 * Provides functionality to read XML files as well as factories to create
 * model renderer instances.
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

#if !defined(TOPOLOGIC_PARSE_H)
#define TOPOLOGIC_PARSE_H

#include <topologic/state.h>
#include <ef.gy/polytope.h>
#include <ef.gy/parametric.h>
#include <ef.gy/ifs.h>
#include <ef.gy/flame.h>
#include <ef.gy/render-null.h>
#if !defined (NOLIBRARIES)
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#endif
#include <set>
#include <stdexcept>
#include <sstream>

namespace topologic
{
    /**\brief Helper function to turn strings into doubles
     *
     * Fairly self-explanatory: use a std::stringstream to parse the value of a
     * std::string and return the double it represents. Used by the XML
     * interpreter.
     *
     * \param[in] s The string to parse.
     *
     * \returns The double represented by the string. 0 if the string could not
     *          be parsed.
     */
    static inline double stringToDouble (const std::string &s)
    {
        std::istringstream in(s);
        double d = 0;
        in >> d;
        return d;
    }

    /**\brief Model factory helper
     *
     * Used to update the model of a topologic::state object with parameters
     * are largely determined at run time. This approach allows the compiler to
     * create optimised code instances of each model/renderer combination. In
     * theory, anyway.
     *
     * \tparam Q Base data type for calculations
     * \tparam d Model depth, e.g. 4 for a tesseract
     * \tparam e Model render depth, e.g. >= 4 when rendering a tesseract
     * \tparam T Model template, e.g. efgy::geometry::cube
     * \tparam C Renderer template, e.g. topologic::render::opengl
     */
    template<typename Q, unsigned int d, unsigned int e, template <class,unsigned int,class,unsigned int> class T,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model
    {
    public:
        /**\brief Set model with current parameters
         *
         * This method will delete the current model and replace it with one
         * created by using the template parameters of this class.
         *
         * \param[out] so The topologic::state object to modify.
         *
         * \returns 'false' if the topologic::state object that was passed in
         *          does not contain a model after this function returns.
         *          'true' if things went smoothly instead.
         */
        static bool set (state<Q,e> &so)
        {
            if (so.state<Q,2>::model)
            {
                delete so.state<Q,2>::model;
                so.state<Q,2>::model = 0;
            }

            so.state<Q,2>::model = new C<Q,d,T,e,true>(so);

            return so.state<Q,2>::model != 0;
        }

        /**\brief Set model with current parameters
         *
         * Like the set() method with just one parameter, this method will try
         * to update the model of the state object passed in as the 'so'
         * parameter; unlike this other overload of the method, this one is
         * used to determine the proper template parameters to use. It will
         * call itself recursively in order to do so.
         *
         * \param[out] so    The topologic::state object to modify.
         * \param[in]  dims  Target model depth (e.g. 4 for a tesseract).
         * \param[in]  rdims Target render depth (e.g. >= 4 for a tesseract).
         *
         * \returns 'true' if the model was updated successfully, 'false' when
         *          either the parameters didn't make sense or the new model's
         *          constructor failed to create a new model in the other set()
         *          overload.
         */
        static bool set (state<Q,e> &so, const unsigned int &dims, const unsigned int &rdims)
        {
            if (d < T<Q,d,efgy::render::null<Q,e>,e>::modelDimensionMinimum)
            {
                return false;
            }

            if (   (T<Q,d,efgy::render::null<Q,e>,e>::modelDimensionMaximum > 0)
                && (d > T<Q,d,efgy::render::null<Q,e>,e>::modelDimensionMaximum))
            {
                return model<Q,d-1,e,T,C>::set (so, dims, rdims);
            }

            if (e < T<Q,d,efgy::render::null<Q,e>,e>::renderDimensionMinimum)
            {
                return false;
            }

            if (   (T<Q,d,efgy::render::null<Q,e>,e>::renderDimensionMaximum > 0)
                && (e > T<Q,d,efgy::render::null<Q,e>,e>::renderDimensionMaximum))
            {
                return model<Q,d,e-1,T,C>::set (so, dims, rdims);
            }

            if (e == rdims)
            {
                if (d == dims)
                {
                    return set(so);
                }
                else if (d < dims)
                {
                    return false;
                }
                else
                {
                    return model<Q,d-1,e,T,C>::set (so, dims, rdims);
                }
            }
            else if (e < rdims)
            {
                return false;
            }
            else
            {
                return model<Q,d,e-1,T,C>::set (so, dims, rdims);
            }
        }
    };

    /**\brief Model factory helper; e=2 fix point
     *
     * The model factory helper, topologic::model, works by calling itself
     * recursively with different template parameters. This is one of these
     * parameters' fix points, which prevents an infinite template recursion.
     *
     * \tparam Q Base data type for calculations
     * \tparam d Model depth, e.g. 4 for a tesseract
     * \tparam T Model template, e.g. efgy::geometry::cube
     * \tparam C Renderer template, e.g. topologic::render::opengl
     */
    template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,d,2,T,C>
    {
    public: static bool set (state<Q,2> &, const unsigned int &, const unsigned int &) { return false; }
    };

    /**\brief Model factory helper; d=1 fix point
     *
     * The model factory helper, topologic::model, works by calling itself
     * recursively with different template parameters. This is one of these
     * parameters' fix points, which prevents an infinite template recursion.
     *
     * \tparam Q Base data type for calculations
     * \tparam e Model render depth, e.g. >= 4 when rendering a tesseract
     * \tparam T Model template, e.g. efgy::geometry::cube
     * \tparam C Renderer template, e.g. topologic::render::opengl
     */
    template<typename Q, unsigned int e, template <class,unsigned int,class,unsigned int> class T,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,1,e,T,C>
    {
    public: static bool set (state<Q,e> &, const unsigned int &, const unsigned int &) { return false; }
    };

    /**\brief Set model in global state object
     *
     * Updates the model of a topologic::state instance. This seems easy
     * enough in principle, but all of the model and renderer parameters are
     * template parameters, and thus need to be known at compile time. If you
     * don't know the parameters to use at compile time then you can use this
     * function to determine them at run time.
     *
     * \tparam Q Base data type for calculations
     * \tparam d Model depth, e.g. 4 for a tesseract
     * \tparam e Model render depth, e.g. >= 4 when rendering a tesseract
     * \tparam C Renderer template, e.g. topologic::render::opengl
     *
     * \param[in]  type  Target model id (e.g. "cube")
     * \param[out] so    The topologic::state object to modify.
     * \param[in]  dims  Target model depth (e.g. 4 for a tesseract).
     * \param[in]  rdims Target render depth (e.g. >= 4 for a tesseract).
     *
     * \returns 'true' if the model has been updated successfully, 'false' if
     *          either the parameters didn't make sense or the new model could
     *          not be created for some other reason.
     */
    template<typename Q, unsigned int d, unsigned int e,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    static bool setModelWithTypeString (const std::string &type, state<Q,e> &so, const unsigned int &dims = d, const unsigned int &rdims = e)
    {
             if (type == "simplex")
                return model<Q,d,e,efgy::geometry::simplex,C>::set(so, dims, rdims);
        else if (type == "plane")
                return model<Q,d,e,efgy::geometry::plane,C>::set(so, dims, rdims);
        else if (type == "cube")
                 return model<Q,d,e,efgy::geometry::cube,C>::set(so, dims, rdims);
        else if (type == "sphere")
                 return model<Q,d,e,efgy::geometry::sphere,C>::set(so, dims, rdims);
        else if (type == "moebius-strip")
                 return model<Q,d,e,efgy::geometry::moebiusStrip,C>::set(so, dims, rdims);
        else if (type == "klein-bagel")
                 return model<Q,d,e,efgy::geometry::kleinBagel,C>::set(so, dims, rdims);
        else if (type == "sierpinski-gasket")
                 return model<Q,d,e,efgy::geometry::sierpinski::gasket,C>::set(so, dims, rdims);
        else if (type == "sierpinski-carpet")
                 return model<Q,d,e,efgy::geometry::sierpinski::carpet,C>::set(so, dims, rdims);
        else if (type == "random-affine-ifs")
                 return model<Q,d,e,efgy::geometry::randomAffineIFS,C>::set(so, dims, rdims);
        else if (type == "random-flame")
                 return model<Q,d,e,efgy::geometry::flame::random,C>::set(so, dims, rdims);

        return false;
    }

    template<typename Q, unsigned int d>
    static bool setMatrixCell (state<Q,d> &s, const unsigned int &sd, const unsigned int &x, const unsigned int &y, const Q &vv)
    {
        if (d == sd)
        {
            s.transformation.transformationMatrix[x][y] = vv;
            return true;
        }
        
        return setMatrixCell<Q,d-1>(s, sd, x, y, vv);
    }
    
    template<typename Q, unsigned int d>
    static bool setMatrixCell (state<Q,2> &, const unsigned int &, const unsigned int &, const unsigned int &, const Q &)
    {
        return false;
    }

#if !defined (NOLIBRARIES)
    /**\brief XML parser wrapper
     *
     * This class is a wrapper for the libxml2 XML parser. It is used when
     * reading the model parameters saved to XML files, e.g. to SVGs by the SVG
     * renderer.
     */
    class xml
    {
    public:
        /**\brief Default constructor
         *
         * The sole purpose of this destructor is to do whatever the
         * LIBXML_TEST_VERSION macro does, in order to initialise libxml2.
         */
        xml(void)
        {
            LIBXML_TEST_VERSION
        }

        /**\brief Destructor
         *
         * Calls xmlCleanupParser() to deinitialize libxml2.
         */
        ~xml(void)
        {
            xmlCleanupParser();
        }

        /**\brief XML parser instance
         *
         * Objects of this class are generated by the xml class to provide
         * access to the parsed content of an XML file.
         */
        class parser
        {
        public:
            parser(const std::string &data,
                   const std::string &filename)
                : document (xmlReadMemory (data.data(), int(data.size()), filename.c_str(), 0,
                            XML_PARSE_NOERROR | XML_PARSE_NOWARNING))
            {
                if (document == 0)
                {
                    throw std::runtime_error("failed to parse xml file");
                }

                xpathContext = xmlXPathNewContext(document);
                if (xpathContext == 0)
                {
                    xmlFreeDoc(document); 
                    throw std::runtime_error("failed to create XPath context");
                }

                if (xmlXPathRegisterNs
                        (xpathContext,
                         (const xmlChar *)"svg",
                         (const xmlChar *)"http://www.w3.org/2000/svg") != 0)
                {
                    xmlFreeDoc(document); 
                    xmlXPathFreeContext(xpathContext);
                    throw std::runtime_error("failed to register namespace: svg");
                }

                if (xmlXPathRegisterNs
                        (xpathContext,
                         (const xmlChar *)"topologic",
                         (const xmlChar *)"http://ef.gy/2012/topologic") != 0)
                {
                    xmlFreeDoc(document);
                    xmlXPathFreeContext(xpathContext);
                    throw std::runtime_error("failed to register namespace: topologic");
                }
            }

            ~parser(void)
            {
                xmlXPathFreeContext(xpathContext);
                xmlFreeDoc(document);
            }

            const std::string evaluate
                (const std::string &expression)
            {
                xmlXPathObjectPtr xpathObject = lookup (expression);

                const xmlChar *resultc = xmlXPathCastNodeSetToString(xpathObject->nodesetval);
                const std::string result((const char *)resultc);
                free((void*)resultc);

                xmlXPathFreeObject(xpathObject);

                return result;
            }

            const std::string evaluateToFragment
                (const std::string &expression)
            {
                xmlXPathObjectPtr xpathObject = lookup (expression);

                xmlBufferPtr buffer = xmlBufferCreate();
                if (buffer == 0)
                {
                    throw std::runtime_error("failed to create xmlBufferPtr");
                }

                xmlNodeSetPtr nodeset = xpathObject->nodesetval;

                if ((nodeset == 0) || (nodeset->nodeNr < 1) || (nodeset->nodeTab == 0))
                {
                    return "";
                }

                if (xmlNodeDump (buffer, document, nodeset->nodeTab[0], 0, 0) == -1)
                {
                    throw std::runtime_error("could not generate XML fragment");
                }

                const std::string rv((const char*)buffer->content, buffer->size);

                xmlBufferFree (buffer);

                return rv;
            }

            bool updateContext
                (const std::string &expression)
            {
                xmlXPathObjectPtr xpathObject = lookup (expression);
                xmlNodeSetPtr nodeset = xpathObject->nodesetval;

                if ((nodeset == 0) || (nodeset->nodeNr < 1) || (nodeset->nodeTab == 0))
                {
                    return false;
                }

                if (nodeset->nodeNr != 1)
                {
                    return false;
                }

                xmlNodePtr node = nodeset->nodeTab[0];

                if (node == 0)
                {
                    return false;
                }

                xpathContext->node = node;

                xmlXPathFreeObject (xpathObject);

                return true;
            }

            xmlDocPtr document;
            xmlXPathContextPtr xpathContext;

        protected:
            xmlXPathObjectPtr lookup (const std::string &expression)
            {
                xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression
                    ((const xmlChar *)expression.c_str(),
                     xpathContext);
                if (xpathObject == 0)
                {
                    throw std::runtime_error("failed to evaluate XPath expression");
                }

                return xpathObject;
            }
        };

        parser parse
            (const std::string &data,
             const std::string &filename)
        {
            return parser(data, filename);
        }
    };

    template<typename Q, unsigned int d>
    static bool parse (state<Q,d> &s, xml::parser &parser)
    {
        std::stringstream st;
        std::string value, dims, dimssq;
        st << d;
        st >> dims;
        st.clear();
        st.str("");
        st << ((d+1) * (d+1));
        st >> dimssq;
        st.clear();

        if (parser.updateContext ("//topologic:camera[count(@*) = " + dims + "][1]"))
        {
            do
            {
                for (unsigned int i = 0; i < d; i++)
                {
                    if ((i == 0) && ((value = parser.evaluate("@radius")) != ""))
                    {
                        s.fromp[0] = Q(stringToDouble(value));
                        continue;
                    }
                    else
                    {
                        st.str("");
                        st << "@theta-" << i;
                        if ((value = parser.evaluate(st.str())) != "")
                        {
                            s.fromp[i] = Q(stringToDouble(value));
                            continue;
                        }
                    }

                    if (i < sizeof(cartesianDimensions))
                    {
                        char r[] = {'@', cartesianDimensions[i], 0};
                        if ((value = parser.evaluate(r)) != "")
                        {
                            s.from[i] = Q(stringToDouble(value));
                        }
                    }
                    else
                    {
                        st.str("");
                        st << "@d-" << i;
                        if ((value = parser.evaluate(st.str())) != "")
                        {
                            s.from[i] = Q(stringToDouble(value));
                        }
                    }
                }
            }
            while (parser.updateContext ("following-sibling::topologic:camera[count(@*) = " + dims + "][1]"));
        }

        if (parser.updateContext ("//topologic:transformation[@depth = " + dims + "][1]"))
        {
            do
            {
                if ((value = parser.evaluate("@matrix")) == "identity")
                {
                    s.transformation = efgy::geometry::transformation::affine<Q,d>();
                }
            }
            while (parser.updateContext ("following-sibling::topologic:transformation[@depth = " + dims + "][1]"));
        }

        if (parser.updateContext ("//topologic:transformation[count(@*) = " + dimssq + "][1]"))
        {
            do
            {
                for (unsigned int i = 0; i <= d; i++)
                {
                    for (unsigned int j = 0; j <= d; j++)
                    {
                        st.str("");
                        st << "@e" << i << "-" << j;
                        if ((value = parser.evaluate(st.str())) != "")
                        {
                            s.transformation.transformationMatrix[i][j] = Q(stringToDouble(value));
                        }
                    }
                }
            }
            while (parser.updateContext ("following-sibling::topologic:transformation[count(@*) = " + dimssq + "][1]"));
        }

        return parse<Q,d-1>(s, parser);
    }

    template<typename Q, unsigned int d>
    static bool parse (state<Q,2> &s, xml::parser &parser)
    {
        std::string value;
        if ((value = parser.evaluate("//topologic:precision/@polar")) != "")
        {
            s.parameter.polarPrecision = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:precision/@export-multiplier")) != "")
        {
            s.exportMultiplier = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:options/@radius")) != "")
        {
            s.parameter.polarRadius = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:camera/@mode[1]")) != "")
        {
            s.polarCoordinates = (value == "polar");
        }
        if ((value = parser.evaluate("//topologic:colour-background/@red")) != "")
        {
            s.background.red = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-background/@green")) != "")
        {
            s.background.green = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-background/@blue")) != "")
        {
            s.background.blue = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-background/@alpha")) != "")
        {
            s.background.alpha = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-wireframe/@red")) != "")
        {
            s.wireframe.red = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-wireframe/@green")) != "")
        {
            s.wireframe.green = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-wireframe/@blue")) != "")
        {
            s.wireframe.blue = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-wireframe/@alpha")) != "")
        {
            s.wireframe.alpha = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-surface/@red")) != "")
        {
            s.surface.red = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-surface/@green")) != "")
        {
            s.surface.green = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-surface/@blue")) != "")
        {
            s.surface.blue = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:colour-surface/@alpha")) != "")
        {
            s.surface.alpha = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:ifs/@iterations")) != "")
        {
            s.parameter.iterations = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:ifs/@seed")) != "")
        {
            s.parameter.seed = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:ifs/@functions")) != "")
        {
            s.parameter.functions = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:ifs/@pre-rotate")) != "")
        {
            s.parameter.preRotate = (value == "yes");
        }
        if ((value = parser.evaluate("//topologic:ifs/@post-rotate")) != "")
        {
            s.parameter.postRotate = (value == "yes");
        }
        if ((value = parser.evaluate("//topologic:flame/@coefficients")) != "")
        {
            s.parameter.flameCoefficients = Q(stringToDouble(value));
        }
        return true;
    }

    template<typename Q, unsigned int d,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    static bool parseModel (state<Q,d> &s, xml::parser &parser)
    {
        if (parser.updateContext("//topologic:model[@depth][@type][1]"))
        {
            int depth  = int(stringToDouble(parser.evaluate("@depth")));
            int rdepth = depth;
            std::string type = parser.evaluate("@type");
            std::string value = parser.evaluate("@render-depth");
            if (value != "")
            {
                rdepth = int(stringToDouble(value));
            }

            if (rdepth == 0)
            {
                rdepth = depth;
                if (   (type == "sphere")
                    || (type == "moebius-strip")
                    || (type == "klein-bagle")) rdepth++;
            }

            return setModelWithTypeString<Q,d,d,C> (type, s, depth, rdepth);
        }

        return false;
    }
#endif
};

#endif
