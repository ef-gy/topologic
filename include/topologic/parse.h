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

#if !defined(TOPOLOGIC_PARSE_H)
#define TOPOLOGIC_PARSE_H

#include <topologic/state.h>
#include <ef.gy/polytope.h>
#include <ef.gy/parametric.h>
#include <ef.gy/ifs.h>
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
#if !defined (NOLIBRARIES)
    class xml
    {
    public:
        xml()
        {
            LIBXML_TEST_VERSION
        }

        ~xml(void)
        {
            xmlCleanupParser();
        }

        class parser
        {
        public:
            parser(xml &pXml,
                   const std::string &data,
                   const std::string &filename)
                : xml (pXml),
                  document (xmlReadMemory (data.data(), int(data.size()), filename.c_str(), 0,
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

                xml.parsers.insert (this);
            }

            ~parser(void)
            {
                xml.parsers.erase (this);

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
            xml &xml;

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
            return parser(*this, data, filename);
        }

    protected:
        std::set<parser*> parsers;
    };
#endif

    static inline double stringToDouble (const std::string &s)
    {
        std::istringstream in(s);
        double d = 0;
        in >> d;
        return d;
    }

#if !defined (NOLIBRARIES)
    template<typename Q, unsigned int d>
    static bool parse (state<Q,d> &s, xml::parser &parser)
    {
        std::stringstream st;
        std::string value, dims;
        st << d;
        st >> dims;
        st.clear();

        if (parser.updateContext ("//topologic:camera[count(@*) = " + dims + "][1]"))
        {
            do
            {
                for (unsigned int i = 0; i < d; i++)
                {
                    if ((i == 0) && ((value = parser.evaluate("@radius")) != ""))
                    {
                        s.fromp.data[0] = Q(stringToDouble(value));
                        continue;
                    }
                    else
                    {
                        st.str("");
                        st << "@theta-" << i;
                        if ((value = parser.evaluate(st.str())) != "")
                        {
                            s.fromp.data[i] = Q(stringToDouble(value));
                            continue;
                        }
                    }

                    if (i < sizeof(cartesianDimensions))
                    {
                        char r[] = {'@', cartesianDimensions[i], 0};
                        if ((value = parser.evaluate(r)) != "")
                        {
                            s.from.data[i] = Q(stringToDouble(value));
                        }
                    }
                    else
                    {
                        st.str("");
                        st << "@d-" << i;
                        if ((value = parser.evaluate(st.str())) != "")
                        {
                            s.from.data[i] = Q(stringToDouble(value));
                        }
                    }
                }
            }
            while (parser.updateContext ("following-sibling::topologic:camera[count(@*) = " + dims + "][1]"));
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
        return true;
    }
#endif

    template<typename Q, unsigned int d, unsigned int e, template <class,unsigned int,class,unsigned int> class T,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    static bool setModel (const state<Q,d> &s, state<Q,e> &so)
    {
        if (so.state<Q,2>::model)
        {
            delete so.state<Q,2>::model;
            so.state<Q,2>::model = 0;
        }

        so.state<Q,2>::model = new C<Q,d,T,e,true>(so);

        return so.state<Q,2>::model != 0;
    }

    template<typename Q, unsigned int d, unsigned int e, template <class,unsigned int,class,unsigned int> class T,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model
    {
    public: static bool set (const state<Q,d> &s, state<Q,e> &so, const unsigned int &rdims)
        {
            if (e == rdims)
            {
                return setModel<Q,d,e,T,C>(s, so);
            }

            return model<Q,d,e-1,T,C>::set (s, so, rdims);
        }
    };

    template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,d,d,T,C>
    {
    public: static bool set (const state<Q,d> &s, state<Q,d> &so, const unsigned int &rdims)
        {
            if (d == rdims)
            {
                return setModel<Q,d,d,T,C>(s, so);
            }

            return false;
        }
    };

    template<typename Q, unsigned int d,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,d,d,efgy::geometry::sphere,C>
    {
    public: static bool set (const state<Q,d> &s, state<Q,d> &so, const unsigned int &rdims) { return false; }
    };

    template<typename Q, unsigned int d,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,d,d,efgy::geometry::moebiusStrip,C>
    {
    public: static bool set (const state<Q,d> &s, state<Q,d> &so, const unsigned int &rdims) { return false; }
    };

    template<typename Q, unsigned int d,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,d,d,efgy::geometry::kleinBagel,C>
    {
    public: static bool set (const state<Q,d> &s, state<Q,d> &so, const unsigned int &rdims) { return false; }
    };

    template<typename Q, unsigned int d, template <class,unsigned int,class,unsigned int> class T,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,d,2,T,C>
    {
    public: static bool set (const state<Q,d> &s, state<Q,2> &so, const unsigned int &rdims) { return false; }
    };

    template<typename Q,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,2,2,efgy::geometry::sphere,C>
    {
    public: static bool set (const state<Q,2> &s, state<Q,2> &so, const unsigned int &rdims) { return false; }
    };

    template<typename Q,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,2,2,efgy::geometry::moebiusStrip,C>
    {
    public: static bool set (const state<Q,2> &s, state<Q,2> &so, const unsigned int &rdims) { return false; }
    };

    template<typename Q,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,2,2,efgy::geometry::kleinBagel,C>
    {
    public: static bool set (const state<Q,2> &s, state<Q,2> &so, const unsigned int &rdims) { return false; }
    };

    template<typename Q, template <class,unsigned int,class,unsigned int> class T,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    class model<Q,2,2,T,C>
    {
    public: static bool set (const state<Q,2> &s, state<Q,2> &so, const unsigned int &rdims) { return false; }
    };

    template<typename Q, unsigned int d, unsigned int e,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    static bool setModelWithTypeString (const state<Q,d> &s, const std::string &type, state<Q,e> &so, const unsigned int &rdims = e)
    {
             if (type == "axe-graph")
                 return model<Q,d,e,efgy::geometry::axeGraph,C>::set(s, so, rdims);
        else if (type == "simplex")
                 return model<Q,d,e,efgy::geometry::simplex,C>::set(s, so, rdims);
        else if (type == "cube")
                 return model<Q,d,e,efgy::geometry::cube,C>::set(s, so, rdims);
        else if (type == "sphere")
                 return model<Q,d,e,efgy::geometry::sphere,C>::set(s, so, rdims);
        else if (type == "moebius-strip")
                 return model<Q,d,e,efgy::geometry::moebiusStrip,C>::set(s, so, rdims);
        else if (type == "klein-bagel")
                 return model<Q,d,e,efgy::geometry::kleinBagel,C>::set(s, so, rdims);
        else if (type == "sierpinski-gasket")
                 return model<Q,d,e,efgy::geometry::sierpinski::gasket,C>::set(s, so, rdims);
        else if (type == "sierpinski-carpet")
                 return model<Q,d,e,efgy::geometry::sierpinski::carpet,C>::set(s, so, rdims);

        return false;
    }

    template<typename Q, unsigned int d, unsigned int e,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    static bool setModelWithTypeStringParameters (const state<Q,d> &s, state<Q,e> &so, const std::string &type, const unsigned int &dims, const unsigned int &rdims)
    {
        if (dims == d)
        {
            return setModelWithTypeString<Q,d,e,C> (s, type, so, rdims);
        }

        return setModelWithTypeStringParameters <Q,d-1,e,C>(s, so, type, dims, rdims);
    }

    template<typename Q, unsigned int d, unsigned int e,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    static bool setModelWithTypeStringParameters (const state<Q,2> &s, state<Q,e> &so, const std::string &type, const unsigned int &dims, const unsigned int &rdims)
    {
        if (dims == 2)
        {
            return setModelWithTypeString<Q,2,e,C> (s, type, so, rdims);
        }

        return false;
    }

    template<typename Q, unsigned int d,
             template <typename, unsigned int, template <class,unsigned int,class,unsigned int> class, unsigned int, bool> class C>
    static bool parseModelWithTypeStringParameters (state<Q,d> &s, const std::string &type, const unsigned int &dims, const unsigned int &rdims)
    {
        return setModelWithTypeStringParameters<Q,d,d,C> (s, s, type, dims, rdims);
    }

#if !defined (NOLIBRARIES)
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

            return parseModelWithTypeStringParameters<Q,d,C> (s, type, depth, rdepth);
        }

        return false;
    }
#endif

    template<typename Q, unsigned int d>
    static bool setPolar (state<Q,d> &s, const unsigned int &sd, const unsigned int &vn, const Q &vv)
    {
        if (d == sd)
        {
            s.fromp.data[vn] = vv;
            return true;
        }

        return setPolar<Q,d-1>(s, sd, vn, vv);
    }

    template<typename Q, unsigned int d>
    static bool setPolar (state<Q,2> &, const unsigned int &, const unsigned int &, const Q &)
    {
        return false;
    }

    template<typename Q, unsigned int d>
    static bool setCartesian (state<Q,d> &s, const unsigned int &sd, const unsigned int &vn, const Q &vv)
    {
        if (d == sd)
        {
            s.fromp.data[vn] = vv;
            return true;
        }

        return setCartesian<Q,d-1>(s, sd, vn, vv);
    }

    template<typename Q, unsigned int d>
    static bool setCartesian (state<Q,2> &, const unsigned int &, const unsigned int &, const Q &)
    {
        return false;
    }
};

#endif
