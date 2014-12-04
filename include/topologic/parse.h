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
 * \see Project Source Code: https://github.com/ef-gy/topologic
 */

#if !defined(TOPOLOGIC_PARSE_H)
#define TOPOLOGIC_PARSE_H

#include <topologic/state.h>
#include <ef.gy/polytope.h>
#include <ef.gy/parametric.h>
#include <ef.gy/ifs.h>
#include <ef.gy/flame.h>
#include <ef.gy/factory.h>
#if !defined (NOLIBRARIES)
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#endif
#include <set>
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
     * topologic::state instance.
     *
     * \tparam Q      Base type for calculations, e.g. double or GLfloat
     * \tparam T      Model template class to use, e.g. efgy::geometry::cube
     * \tparam d      Number of model dimensions, e.g. 4 for a tesseract
     * \tparam e      Number of render dimensions, e.g. >= 4 for a tesseract
     * \tparam format The vector format to use.
     */
    template<typename Q, template <class,unsigned int> class T, unsigned int d, unsigned int e,
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

            /**\brief Adapted model type
             *
             * The resolved model type wrapped in a geometry::adapt template so
             * it works in arbirary dimensions.
             *
             * \tparam tQ Base type for calculations, e.g. double or GLfloat
             * \tparam tD Number of model dimensions, e.g. 4 for a tesseract
             * \tparam tF The vector format to use.
             */
            template <class tQ, unsigned int tD>
            using adapted = efgy::geometry::autoAdapt<tQ,e,T<tQ,tD>,format>;

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
                if (out.model)
                {
                    delete out.model;
                    out.model = 0;
                }

                out.model
                    = (render::base<true>*)
                        (new render::wrapper<Q,d,adapted,true,format>(out, tag));

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

    /**\brief Update transformation matrix of state object instance; 1D fix
     *        point
     *
     * This is a helper template to update a specific affine transformation
     * matrix cell of a topologic::state instance. This 1D fix point doesn't
     * do anything at all, because the 1D transformation matrix of the state
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
    static bool setMatrixCell (state<Q,1> &, const unsigned int &, const unsigned int &, const unsigned int &, const Q &)
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
                : valid(false),
                  document (xmlReadMemory (data.data(), int(data.size()), filename.c_str(), 0,
                            XML_PARSE_NOERROR | XML_PARSE_NOWARNING)),
                  xpathContext(0)
            {
                if (document == 0)
                {
                    std::cerr << "failed to parse xml file\n";
                    return;
                }

                xpathContext = xmlXPathNewContext(document);
                if (xpathContext == 0)
                {
                    xmlFreeDoc(document); 
                    document = 0;
                    std::cerr << "failed to create XPath context\n";
                }

                if (xmlXPathRegisterNs
                        (xpathContext,
                         (const xmlChar *)"svg",
                         (const xmlChar *)"http://www.w3.org/2000/svg") != 0)
                {
                    xmlXPathFreeContext(xpathContext);
                    xpathContext = 0;
                    xmlFreeDoc(document); 
                    document = 0;
                    std::cerr << "failed to register namespace: svg\n";
                }

                if (xmlXPathRegisterNs
                        (xpathContext,
                         (const xmlChar *)"topologic",
                         (const xmlChar *)"http://ef.gy/2012/topologic") != 0)
                {
                    xmlXPathFreeContext(xpathContext);
                    xpathContext = 0;
                    xmlFreeDoc(document); 
                    document = 0;
                    std::cerr << "failed to register namespace: topologic\n";
                }

                valid = true;
            }

            /**\brief Copy constructor
             *
             * The copy constructor is explicitly deleted for memory management
             * reasons.
             */
            parser(const parser &) = delete;

            /**\brief Destructor
             *
             * Destroys the XPath and XML document contexts associated with
             * this instance.
             */
            ~parser(void)
            {
                if (xpathContext)
                {
                    xmlXPathFreeContext(xpathContext);
                }

                if (document)
                {
                    xmlFreeDoc(document);
                }
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
                    std::cerr << "failed to create xmlBufferPtr\n";
                    return "";
                }

                xmlNodeSetPtr nodeset = xpathObject->nodesetval;

                if ((nodeset == 0) || (nodeset->nodeNr < 1) || (nodeset->nodeTab == 0))
                {
                    return "";
                }

                if (xmlNodeDump (buffer, document, nodeset->nodeTab[0], 0, 0) == -1)
                {
                    std::cerr << "could not generate XML fragment\n";
                    return "";
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

            /**\brief Has a valid XML file been loaded?
             *
             * Set to 'true' when this parser context has a valid object loaded,
             * 'false' otherwise. Set by the constructor.
             */
            bool valid;

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
                    std::cerr << "failed to evaluate XPath expression\n";
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
        if (!parser.valid)
        {
            return false;
        }

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

    /**\brief Parse XML file contents and update global state object; 1D fix
     *        point.
     *
     * This function uses an xml::parser instance to update a topologic::state
     * instance with the metadata contained in the XML document. The parser is
     * fairly tolerant of the input XML's layout and will accept tags in
     * topologic's namespace almost everywhere. You should probably still use a
     * designated metadata element in your XML files, such as the svg:metadata
     * element.
     *
     * This process is applied recursively, and this is the 1D fix point, which
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
    static bool parse (state<Q,1> &s, xml::parser &parser)
    {
        if (!parser.valid)
        {
            return false;
        }

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
     *              topologic::updateModel
     *
     * \param[out] s      The global state object to update.
     * \param[out] parser An XML parser instance, hopefully containing
     *                    Topologic metadata.
     *
     * \returns 'true' if things worked out, 'false' otherwise.
     */
    template<typename Q, unsigned int d,
             template<typename, template <class,unsigned int> class, unsigned int, unsigned int, typename> class func>
    static bool parseModel (state<Q,d> &s, xml::parser &parser)
    {
        if (!parser.valid)
        {
            return false;
        }

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

            return efgy::geometry::with<Q,func,d> (s, format, type, depth, rdepth);
        }

        return false;
    }
#endif

    /**\brief Parse JSON file contents and update global state object
     *
     * This is analogous to topologic::parse() with XML data; however, this
     * parses a JSON value instead of querying an XML parser.
     *
     * \tparam Q Base data type for calculations.
     * \tparam d Maximum number of dimensions supported by the given state
     *           instance
     *
     * \param[out] s      The global state object to update.
     * \param[out] value  A JSON value, hopefully containing Topologic metadata.
     *
     * \returns 'true' if the code didn't blow up trying to parse your XML,
     *          'false' if it did. Probably.
     */
    template<typename Q, unsigned int d>
    static bool parse (state<Q,d> &s, efgy::json::value<> &value)
    {
        if (value.type != efgy::json::value<>::object)
        {
            return false;
        }

        bool polar = value.getObject()["polar"].getBoolean();

        if (value.getObject()["camera"].type == efgy::json::value<>::array)
        {
            efgy::json::value<> &cameras = value.getObject()["camera"];
            for (efgy::json::value<> &c : cameras.getArray())
            {
                if (   c.type == efgy::json::value<>::array
                    && c.getArray().size() == d)
                {
                    for (unsigned int i = 0; i < d; i++)
                    {
                        if (c.getArray()[i].type == efgy::json::value<>::number)
                        {
                            if (polar)
                            {
                                s.fromp[i] = c.getArray()[i].getNumber();
                            }
                            else
                            {
                                s.from[i] = c.getArray()[i].getNumber();
                            }
                        }
                    }
                }
            }
        }

        if (value.getObject()["transformation"].type == efgy::json::value<>::array)
        {
            efgy::json::value<> &transformations = value.getObject()["transformation"];
            for (efgy::json::value<> &t : transformations.getArray())
            {
                if (   t.type == efgy::json::value<>::array
                    && t.getArray().size() == ((d+1)*(d+1)))
                {
                    for (unsigned int i = 0; i <= d; i++)
                    {
                        for (unsigned int j = 0; j <= d; j++)
                        {
                            if (t.getArray()[(i*(d+1)+j)].type == efgy::json::value<>::number)
                            {
                                s.transformation.transformationMatrix[i][j]
                                    = t.getArray()[(i*(d+1)+j)].getNumber();
                            }
                        }
                    }
                }
            }
        }

        return parse<Q,d-1>(s, value);
    }
    
    /**\brief Parse JSON file contents and update global state object; 1D fix
     *        point.
     *
     * This is analogous to topologic::parse() with XML data; however, this
     * parses a JSON value instead of querying an XML parser.
     *
     * \tparam Q Base data type for calculations.
     * \tparam d Maximum number of dimensions supported by the given state
     *           instance
     *
     * \param[out] s      The global state object to update.
     * \param[out] value  A JSON value, hopefully containing Topologic metadata.
     *
     * \returns 'true' if the code didn't blow up trying to parse your XML,
     *          'false' if it did. Probably.
     */
    template<typename Q, unsigned int d>
    static bool parse (state<Q,1> &s, efgy::json::value<> &value)
    {
        if (value.type != efgy::json::value<>::object)
        {
            return false;
        }

        if (value.getObject()["radius"].type == efgy::json::value<>::number)
        {
            s.parameter.radius = value.getObject()["radius"].getNumber();
        }
        if (value.getObject()["minorRadius"].type == efgy::json::value<>::number)
        {
            s.parameter.radius2 = value.getObject()["minorRadius"].getNumber();
        }
        if (value.getObject()["constant"].type == efgy::json::value<>::number)
        {
            s.parameter.constant = value.getObject()["constant"].getNumber();
        }
        if (value.getObject()["precision"].type == efgy::json::value<>::number)
        {
            s.parameter.precision = value.getObject()["precision"].getNumber();
        }
        if (value.getObject()["iterations"].type == efgy::json::value<>::number)
        {
            s.parameter.iterations = (int)value.getObject()["iterations"].getNumber();
        }
        if (value.getObject()["seed"].type == efgy::json::value<>::number)
        {
            s.parameter.seed = (int)value.getObject()["seed"].getNumber();
        }
        if (value.getObject()["functions"].type == efgy::json::value<>::number)
        {
            s.parameter.functions = (int)value.getObject()["functions"].getNumber();
        }
        if (value.getObject()["flameCoefficients"].type == efgy::json::value<>::number)
        {
            s.parameter.flameCoefficients = (int)value.getObject()["flameCoefficients"].getNumber();
        }

        if (value.getObject()["preRotate"].type != efgy::json::value<>::null)
        {
            s.parameter.preRotate  = value.getObject()["preRotate"].getBoolean();
        }
        if (value.getObject()["postRotate"].type != efgy::json::value<>::null)
        {
            s.parameter.postRotate = value.getObject()["postRotate"].getBoolean();
        }

        if (   value.getObject()["background"].type == efgy::json::value<>::array
            && value.getObject()["background"].getArray().size() >= 5
            && value.getObject()["background"].getArray()[1].type == efgy::json::value<>::number
            && value.getObject()["background"].getArray()[2].type == efgy::json::value<>::number
            && value.getObject()["background"].getArray()[3].type == efgy::json::value<>::number
            && value.getObject()["background"].getArray()[4].type == efgy::json::value<>::number)
        {
            s.background.red   = value.getObject()["background"].getArray()[1].getNumber();
            s.background.green = value.getObject()["background"].getArray()[2].getNumber();
            s.background.blue  = value.getObject()["background"].getArray()[3].getNumber();
            s.background.alpha = value.getObject()["background"].getArray()[4].getNumber();
        }
        if (   value.getObject()["wireframe"].type == efgy::json::value<>::array
            && value.getObject()["wireframe"].getArray().size() >= 5
            && value.getObject()["wireframe"].getArray()[1].type == efgy::json::value<>::number
            && value.getObject()["wireframe"].getArray()[2].type == efgy::json::value<>::number
            && value.getObject()["wireframe"].getArray()[3].type == efgy::json::value<>::number
            && value.getObject()["wireframe"].getArray()[4].type == efgy::json::value<>::number)
        {
            s.wireframe.red   = value.getObject()["wireframe"].getArray()[1].getNumber();
            s.wireframe.green = value.getObject()["wireframe"].getArray()[2].getNumber();
            s.wireframe.blue  = value.getObject()["wireframe"].getArray()[3].getNumber();
            s.wireframe.alpha = value.getObject()["wireframe"].getArray()[4].getNumber();
        }
        if (   value.getObject()["surface"].type == efgy::json::value<>::array
            && value.getObject()["surface"].getArray().size() >= 5
            && value.getObject()["surface"].getArray()[1].type == efgy::json::value<>::number
            && value.getObject()["surface"].getArray()[2].type == efgy::json::value<>::number
            && value.getObject()["surface"].getArray()[3].type == efgy::json::value<>::number
            && value.getObject()["surface"].getArray()[4].type == efgy::json::value<>::number)
        {
            s.surface.red   = value.getObject()["surface"].getArray()[1].getNumber();
            s.surface.green = value.getObject()["surface"].getArray()[2].getNumber();
            s.surface.blue  = value.getObject()["surface"].getArray()[3].getNumber();
            s.surface.alpha = value.getObject()["surface"].getArray()[4].getNumber();
        }

        return true;
    }

    /**\brief Parse and update model data
     *
     * This is analogous to topologic::parseModel() with XML data; however, this
     * parses a JSON value instead of querying an XML parser.
     *
     * \tparam Q    Base data type for calculations.
     * \tparam d    Maximum number of dimensions supported by the given state
     *              instance
     * \tparam func State object update functor, e.g.
     *              topologic::updateModel
     *
     * \param[out] s      The global state object to update.
     * \param[out] value  A JSON value, hopefully containing Topologic metadata.
     *
     * \returns 'true' if things worked out, 'false' otherwise.
     */
    template<typename Q, unsigned int d,
             template<typename, template <class,unsigned int> class, unsigned int, unsigned int, typename> class func>
    static bool parseModel (state<Q,d> &s, efgy::json::value<> &value)
    {
        if (value.type != efgy::json::value<>::object)
        {
            return false;
        }

        std::string type = "cube";
        std::string format = "cartesian";
        int depth = 4;
        int rdepth = 4;

        if (value.getObject()["coordinateFormat"].type == efgy::json::value<>::string)
        {
            format = value.getObject()["coordinateFormat"].getString();
        }

        if (value.getObject()["model"].type == efgy::json::value<>::string)
        {
            type = value.getObject()["model"].getString();
        }
        else
        {
            return false;
        }

        if (value.getObject()["depth"].type == efgy::json::value<>::number)
        {
            depth = (int)value.getObject()["depth"].getNumber();
        }

        if (value.getObject()["renderDepth"].type == efgy::json::value<>::number)
        {
            rdepth = (int)value.getObject()["renderDepth"].getNumber();
        }

        return efgy::geometry::with<Q,func,d> (s, format, type, depth, rdepth);
    }
};

#endif
