/**\file
 * \brief Input parsing
 *
 * Provides functionality to read XML files as well as factories to create
 * model renderer instances.
 *
 * \copyright
 * Copyright (c) 2012-2014, Topologic Project Members
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
 * \see Project Source Code: https://github.com/ef-gy/topologic.git
 */

#if !defined(TOPOLOGIC_PARSE_H)
#define TOPOLOGIC_PARSE_H

#include <topologic/state.h>
#include <ef.gy/polytope.h>
#include <ef.gy/parametric.h>
#include <ef.gy/ifs.h>
#include <ef.gy/flame.h>
#include <ef.gy/render-null.h>
#include <ef.gy/factory.h>
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

    /**\brief Model update functor
     *
     * Used with efgy::geometry::with to initialise the model of a
     * topologic::state instance. This class has one additional template
     * parameter in addition to what the efgy::geometry::with template expects,
     * which should be set to one of topologic's render wrappers - e.g.
     * render::svg or render::opengl. You need to "remove" this template
     * parameter with the help of a C++11 type alias.
     *
     * An appropriate type alias is already provided for render::svg and
     * render::opengl, named updateModelWrapper.
     *
     * \tparam Q      Base type for calculations, e.g. double or GLfloat
     * \tparam T      Model template class to use, e.g. efgy::geometry::cube
     * \tparam d      Number of model dimensions, e.g. 4 for a tesseract
     * \tparam e      Number of render dimensions, e.g. >= 4 for a tesseract
     * \tparam C      The model renderer, e.g. render::svg
     * \tparam format The vector format to use.
     */
    template<typename Q, template <class,unsigned int,unsigned int,typename> class T, unsigned int d, unsigned int e,
             template <typename, unsigned int, template <class,unsigned int,unsigned int,typename> class, unsigned int, bool, typename> class C,
             typename format>
    class updateModel
    {
        public:
            /**\brief Argument type
             *
             * The argument type that is to be passed into the functor. Since we
             * want to modify a topologic::state object, that is the type we
             * require as input.
             */
            typedef state<Q,e> &argument;

            /**\brief Output type
             *
             * Return type of our functor - we return a boolean for whether or
             * not the 'model' member of the state object has been initialise,
             * so this is set to 'bool'.
             */
            typedef bool output;

            /**\brief Initialise new model
             *
             * Creates a new model and updates the given state object to use the
             * newly created instance.
             *
             * \param[out] out The state object to modify.
             * \param[in]  tag The vector format tag instance to use.
             *
             * \returns 'true' if the state object has a valid model pointer at
             *          the time the function returns.
             */
            static output apply (argument out, const format &tag)
            {
                out.state<Q,2>::model
                    = std::shared_ptr<render::base<true>>
                        (new C<Q,d,T,e,true,format>(out, tag));

                return out.model != 0;
            }

            /**\brief Return default exit status
             *
             * This is used instead of the apply method whenever
             * efgy::geometry::with was unable to find a matching model/depth
             * combination for the given parameters.
             *
             * \param[out] out The state object in question. Will not be updated
             *                 with a new model.
             *
             * \returns 'true' if the state object has a valid model pointer at
             *          the time the function returns.
             */
            static output pass (const argument out)
            {
                return out.model != 0;
            }
    };

    /**\brief Model update functor for render::wrapper
     *
     * Convenient specialisation of updateModel using render::wrapper as the
     * model renderer.
     *
     * \tparam Q      Base type for calculations, e.g. double or GLfloat.
     * \tparam T      Model template class to use, e.g. efgy::geometry::cube.
     * \tparam d      Number of model dimensions, e.g. 4 for a tesseract.
     * \tparam e      Number of render dimensions, e.g. >= 4 for a tesseract.
     * \tparam format The vector format to use.
     */
    template<typename Q, template <class,unsigned int,unsigned int,typename> class T, unsigned int d, unsigned int e, typename format>
    using updateModelWrapper = updateModel<Q,T,d,e,render::wrapper,format>;

    /**\brief Update transformation matrix of state object instance
     *
     * This is a helper template to update a specific affine transformation
     * matrix cell of a topologic::state instance.
     *
     * \tparam Q Base data type for calculations.
     * \tparam d Maximum number of dimensions supported by the given state
     *           instance
     *
     * \param[out] s  The state object instance to modify.
     * \param[in]  sd Target dimension of the state object to modify.
     * \param[in]  x  First coordinate of the matrix cell to modify.
     * \param[in]  y  Second coordinate of the matrix cell to modify.
     * \param[in]  vv The value to set the (x,y) cell to.
     *
     * \returns 'true' if the cell was updated, 'false' if not.
     */
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

    /**\brief Update transformation matrix of state object instance; 2D fix
     *        point
     *
     * This is a helper template to update a specific affine transformation
     * matrix cell of a topologic::state instance. This 2D fix point doesn't
     * do anything at all, because the 2D transformation matrix of the state
     * object isn't being used right now.
     *
     * Any parameters passed to this function are currently being ignored.
     *
     * \tparam Q Base data type for calculations.
     * \tparam d Maximum number of dimensions supported by the given state
     *           instance
     *
     * \returns 'true' if the cell was updated, 'false' if not. Since this
     *          function doesn't do anything by design it will always fail.
     */
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
            /**\brief Construct with XML data and file name
             *
             * Uses libxml2 to parse the XML file data passed in as the first
             * argument to this function, using the given file name as a basis
             * for relative references.
             *
             * \param[in] data     A proper, well-formed XML document
             * \param[in] filename The source location of the document
             */
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

            /**\brief Copy constructor
             *
             * The copy constructor is explicitly deleted for memory management
             * reasons.
             *
             * \todo Provide a move constructor.
             */
            parser(const parser &) = delete;

            /**\brief Destructor
             *
             * Destroys the XPath and XML document contexts associated with
             * this instance.
             */
            ~parser(void)
            {
                xmlXPathFreeContext(xpathContext);
                xmlFreeDoc(document);
            }

            /**\brief Evaluate XPath expression to string
             *
             * Evaluates the given XPath expression in the current context and
             * returns the string result of an implicit XPath cast.
             *
             * \param[in] expression The XPath expression to evaluate.
             *
             * \returns The string value of the given expression.
             */
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

            /**\brief Evaluate XPath expression to XML fragment
             *
             * This method works much like the evaluate() method, but instead
             * of returning the result of an implicit XPath cast to string,
             * this instead returns the XML fragment that the expression
             * returned.
             *
             * \param[in] expression The XPath expression to evaluate.
             *
             * \returns An XML fragment of the given expression's result.
             */
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

            /**\brief Update XPath evaluation context
             *
             * Evaluates the provided XPath expression and sets the current
             * XPath context to the result of this expression.
             *
             * The expression must return exactly one node, otherwise the
             * context is not updated.
             *
             * \param[in] expression An XPath expression that is evaluated in
             *                       the current context and which must return
             *                       precisely one result node.
             *
             * \returns 'true' if the context was updated successfully, 'false'
             *          otherwise.
             */
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

        protected:
            /**\brief libxml2 document context
             *
             * This is what libxml2 came up with by parsing the XML document
             * provided to the constructor.
             */
            xmlDocPtr document;

            /**\brief XPath evaluation context
             *
             * The current context in which XPath expressions are evaluated.
             * Defaults to the root node of the document, but can be changed
             * with the updateContext() method.
             */
            xmlXPathContextPtr xpathContext;

            /**\brief Evaluate XPath expression
             *
             * This method will evaluate the given XPath expression in the
             * current context and return the result. Errors are signaled by
             * throwing a std::runtime_error.
             *
             * \throws std::runtime_error if the given expression is invalid.
             *
             * \param[in] expression The XPath expression to evaluate
             *
             * \returns The result of the XPath expression after evaluation.
             */
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
    };

    /**\brief Parse XML file contents and update global state object
     *
     * This function uses an xml::parser instance to update a topologic::state
     * instance with the metadata contained in the XML document. The parser is
     * fairly tolerant of the input XML's layout and will accept tags in
     * topologic's namespace almost everywhere. You should probably still use a
     * designated metadata element in your XML files, such as the svg:metadata
     * element.
     *
     * \tparam Q Base data type for calculations.
     * \tparam d Maximum number of dimensions supported by the given state
     *           instance
     *
     * \param[out] s      The global state object to update.
     * \param[out] parser An XML parser instance, hopefully containing
     *                    Topologic metadata.
     *
     * \returns 'true' if the code didn't blow up trying to parse your XML,
     *          'false' if it did. Probably.
     */
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

    /**\brief Parse XML file contents and update global state object; 2D fix
     *        point.
     *
     * This function uses an xml::parser instance to update a topologic::state
     * instance with the metadata contained in the XML document. The parser is
     * fairly tolerant of the input XML's layout and will accept tags in
     * topologic's namespace almost everywhere. You should probably still use a
     * designated metadata element in your XML files, such as the svg:metadata
     * element.
     *
     * This process is applied recursively, and this is the 2D fix point, which
     * makes certain that the compiler won't blow up due to infinite template
     * recursion.
     *
     * \tparam Q Base data type for calculations.
     * \tparam d Maximum number of dimensions supported by the given state
     *           instance
     *
     * \param[out] s      The global state object to update.
     * \param[out] parser An XML parser instance, hopefully containing
     *                    Topologic metadata.
     *
     * \returns 'true' if the code didn't blow up trying to parse your XML,
     *          'false' if it did. Probably.
     */
    template<typename Q, unsigned int d>
    static bool parse (state<Q,2> &s, xml::parser &parser)
    {
        std::string value;
        if ((value = parser.evaluate("//topologic:precision/@polar")) != "")
        {
            s.parameter.precision = Q(stringToDouble(value));
        }
        if ((value = parser.evaluate("//topologic:options/@radius")) != "")
        {
            s.parameter.radius = Q(stringToDouble(value));
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

    /**\brief Parse and update model data
     *
     * Like topologic::parse(), this function uses an XML parser object
     * instance to update a topologic::state instance with the data contained
     * in the XML file. topologic::parse() won't update the model in the state
     * object, however, which is what this function is for.
     *
     * The reason this functionality was split into two functions, is that the
     * other parser step shouldn't have to worry about which renderer template
     * will be used for the model.
     *
     * \tparam Q    Base data type for calculations.
     * \tparam d    Maximum number of dimensions supported by the given state
     *              instance
     * \tparam func State object update functor, e.g.
     *              topologic::updateModelWrapper
     *
     * \param[out] s      The global state object to update.
     * \param[out] parser An XML parser instance, hopefully containing
     *                    Topologic metadata.
     *
     * \returns 'true' if things worked out, 'false' otherwise.
     */
    template<typename Q, unsigned int d,
             template<typename, template <class,unsigned int,unsigned int,typename> class, unsigned int, unsigned int, typename> class func>
    static bool parseModel (state<Q,d> &s, xml::parser &parser)
    {
        std::string format = "cartesian", value;
        if ((value = parser.evaluate("//topologic:coordinates/@format")) != "")
        {
            format = value;
        }

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

            return efgy::geometry::with<Q,func,d,d> (s, format, type, depth, rdepth);
        }

        return false;
    }
#endif
};

#endif
