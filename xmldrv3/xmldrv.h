/******************************************************************************
* Module : xmldrv                                                             *
*                                                                             *
* Purpose: Make C++ App load and modify XML-CONFIGURATION more conveniently.  *
*                                                                             *
* Author : halx99 (halx99@163.com)                                            *
*                                                                             *
* Comment:                                                                    *
*     Please compile with defining macro:                                     *
*        _USING_LIBXML2 or _USING_XERCESC or _USING_TINYXML2                  *
*     or _USING_RAPIDXML                                                      *
*                                                                             *
*     Version history:                                                        *
*       3.9.3: element::is_good --> element::is_good                         *
*              add operator[](const char* name) interface.                    *
*       3.9.2: change module name 'xml4wrapper' --> 'xmldrv'                  *
*              remove 3rd header dependency.                                  *
*       3.6.2: rapidxml use fatest mode flags                                 *
*       3.6.1: add suport for rapidxml,change module name xml3c to xmldrv     *
*       3.6.00: For more conveniently, use default value api style, remove    *
*               legacy api styles(empty or output parameter)                  *
*       3.5.70:                                                               *
*               (1)Add support for tinyxml2                                   *
*               (2)modify bug for the function:                               *
*                  element::set_attribute_value will lead heap overflow because*
*                  of recursive call infinity                                 *
*       3.5.62: xml4w_api, Add vs2005 project file.                           *
*       3.5.61: xml4w_api, Add vs2008, 2010 project files.                    *
*       3.5.6: xml4w_api, support for compiler no c++2011 standard            *
*              such as vs2005, vs2008, or g++ no flag -std=c++0x/c++11        *
*       3.5.5: xml4w_api, support c++0x/11                                    *
*       3.5.3: xml4w_api, add two interface for getting children              *
*       3.5: xml4w_api, change naming style, optimize some code               *
*       3.3: xml4w_api, support XPATH based on 3rd library: libxml2 and the   *
*            3.0.0 or later version of xerces-c.                              *
*       3.2: xml4w_api, support XPATH based on 3rd library: libxml2_7 or      *
*            xercesc2_8/3_1                                                   *
*       3.1: xml4w_api, based on 3rd library: libxml2_7 or xercesc3           *
*            support XPATH.                                                   *
*       3.0: xml4w_api, based on 3rd library: libxml2_7 or xercesc            *
*       2.0: xml2c, based on 3rd library: xercesc                             *
*       1.0: xmlxx, based on 3rd library: libxml2_7                           *
*******************************************************************************/
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#ifndef _XML4WRAPPER_H_
#define _XML4WRAPPER_H_

/// version
#ifndef _XML4WRAPPER_VERSION
#define _XML4WRAPPER_VERSION "3.10"
#endif

#undef _XMLDRV_STATIC
#define _XMLDRV_STATIC 1

#undef _USE_IN_COCOS2DX
#define _USE_IN_COCOS2DX 0

#if defined(_XMLDRV_STATIC) && _XMLDRV_STATIC
#define XMLDRV_DLL
#else
#if defined(_USRDLL)
#define XMLDRV_DLL     __declspec(dllexport)
#else         /* use a DLL library */
#define XMLDRV_DLL     __declspec(dllimport)
#endif
#endif

/// default: use rapidxml
#undef _USING_LIBXML2
#undef _USING_XERCESC
#undef _USING_TINYXML2
#undef _USING_RAPIDXML
#undef _USING_PUGIXML
#undef _USING_VTDXML
#define _USING_RAPIDXML 1

#if _USE_IN_COCOS2DX
#include <cocos2d.h>
#endif

/// check xpath supports
#if defined(_USING_LIBXML2) || defined(_USING_XERCESC)
#define _XML4W_XPATH_SUPPORTED 1
#endif

/// includes
#include <cstring>
#include <vector>
#include <string>
#include <functional>

#include "nsconv.h"
#include "unreal_string.h"

#if defined(_USING_RAPIDXML)
typedef purelib::unmanaged_string vstring;
#else
typedef std::string vstring;
#endif

extern vstring vstring_empty;

/// basic types
typedef void xml4wNode;
typedef xml4wNode* xml4wNodePtr;

typedef struct xml4wDoc xml4wDoc;
typedef xml4wDoc* xml4wDocPtr;

typedef void* xml4wXPathResultPtr;

/// traversal macro defination
#define __xml4wts_algo(ptr, first, second, stmt) \
        if(ptr != nullptr) { \
            ptr = first; \
        } \
        while (ptr != nullptr) { \
            stmt; \
            ptr = second; \
        }

#define __xml4wts_algo_cond(ptr, first, second, cond, stmt) \
        if(ptr != nullptr) { \
            ptr = first; \
        } \
        while (ptr != nullptr) { \
            if (cond) { \
                 stmt; \
            } \
            ptr = second; \
        }

using namespace purelib;
// using namespace purelib::gc;

namespace xmldrv {

    class element;
    class document;

    class XMLDRV_DLL element
    {
        friend class document;
    public:

        explicit element(xml4wNodePtr _Ptr = nullptr);
        ~element(void);

        element         clone(void) const;

        vstring         get_name(void) const;

        vstring         get_value(const vstring& default_value) const;

        vstring         get_attribute_value(const vstring& name, const vstring&) const;

        bool            has_attribute(const vstring& name) const;

        element         get_parent(void) const;
        element         get_prev_sibling(void) const;
        element         get_next_sibling(void) const;
        element         get_first_child(void) const;

        element         operator[](int index) const;
        element         operator[](const vstring& name) const;
        element         get_child(int index = 0) const;
        element         get_child(const vstring& name, int index = 0) const;

        void            set_value(const vstring& value);
        void            set_attribute_value(const vstring& name, const vstring& value);

        element         add_child(const element& element) const;
        element         add_child(const vstring& name, const vstring& value = nullptr) const;

        void            remove_child(int index = 0);
        void            remove_child(const vstring& name, int index = 0);
        void            remove_children(void);
        void            remove_children(const vstring& name);
        void            remove_self(void);

        std::string     to_string(bool formatted = false) const;

        /// get_value APIs
        bool            get_value(bool value = false, int radix = 10) const;

        int8_t          get_value(int8_t value = 0, int radix = 10) const;
        int16_t         get_value(int16_t value = 0, int radix = 10) const;
        int32_t         get_value(int32_t value = 0, int radix = 10) const;
        int64_t         get_value(int64_t value = 0, int radix = 10) const;

        uint8_t         get_value(uint8_t value = 0, int radix = 10) const;
        uint16_t        get_value(uint16_t value = 0, int radix = 10) const;
        uint32_t        get_value(uint32_t value = 0, int radix = 10) const;
        uint64_t        get_value(uint64_t value = 0, int radix = 10) const;

        float           get_value(float value = 0) const;
        double          get_value(double value = 0) const;

        /// get_attribute_value APIs
        bool            get_attribute_value(const vstring& name, bool value = false, int radix = 10) const;

        int8_t          get_attribute_value(const vstring& name, int8_t value = 0, int radix = 10) const;
        int16_t         get_attribute_value(const vstring& name, int16_t value = 0, int radix = 10) const;
        int32_t         get_attribute_value(const vstring& name, int32_t value = 0, int radix = 10) const;
        int64_t         get_attribute_value(const vstring& name, int64_t value = 0, int radix = 10) const;

        uint8_t         get_attribute_value(const vstring& name, uint8_t value = 0, int radix = 10) const;
        uint16_t        get_attribute_value(const vstring& name, uint16_t value = 0, int radix = 10) const;
        uint32_t        get_attribute_value(const vstring& name, uint32_t value = 0, int radix = 10) const;
        uint64_t        get_attribute_value(const vstring& name, uint64_t value = 0, int radix = 10) const;

        float           get_attribute_value(const vstring& name, float value = 0) const;
        double          get_attribute_value(const vstring& name, double value = 0) const;

        /// set_value APIs
        void            set_value(const char& value);
        void            set_value(const short& value);
        void            set_value(const int& value);
        void            set_value(const long long& value);

        void            set_value(const unsigned char& value);
        void            set_value(const unsigned short& value);
        void            set_value(const unsigned int& value);
        void            set_value(const unsigned long long& value);

        void            set_value(const float& value);
        void            set_value(const double& value);

        /// set_attribute_value APIs
        void            set_attribute_value(const vstring& name, const char& value);
        void            set_attribute_value(const vstring& name, const short& value);
        void            set_attribute_value(const vstring& name, const int& value);
        void            set_attribute_value(const vstring& name, const long long& value);

        void            set_attribute_value(const vstring& name, const unsigned char& value);
        void            set_attribute_value(const vstring& name, const unsigned short& value);
        void            set_attribute_value(const vstring& name, const unsigned int& value);
        void            set_attribute_value(const vstring& name, const unsigned long long& value);

        void            set_attribute_value(const vstring& name, const float& value);
        void            set_attribute_value(const vstring& name, const double& value);

        template<typename _Handler>
        void            cforeach(const _Handler&) const;

        template<typename _Handler> // op must return bool
        void            cforeach_breakif(const _Handler&) const;

        template<typename _Handler>
        void            cforeach(const vstring& name, const _Handler&) const;

        template<typename _Handler> // op must return bool
        void            cforeach_breakif(const vstring& name, const _Handler&) const;

        template<typename _Handler> // foreach attribute, op protype: (const unmanaged_string& name, const unmanaged_string& value)
        void            pforeach(const _Handler&) const;

        template<typename _Handler> // foreach attribute, op protype: (const unmanaged_string& name, const unmanaged_string& value)
        void            pforeach_breakif(const _Handler&) const;

        void*           first_attribute() const;

        static void*     next_attribute(void* attr);
        static vstring   name_of_attr(void* attr);
        static vstring   value_of_attr(void* attr);

        bool            is_good(void) const { return _Mynode != nullptr; }
        operator xml4wNodePtr(void) { return _Mynode; }
        operator xml4wNodePtr(void) const { return _Mynode; }

#if _USE_IN_COCOS2DX
        void set_attribute_value(const vstring& name, const cocos2d::Color3B& value);
        void set_attribute_value(const vstring& name, const cocos2d::Color4B& value);
        void set_attribute_value(const vstring& name, const cocos2d::Color4F& value);
        void set_attribute_value(const vstring& name, const cocos2d::Rect& value);
        void set_attribute_value(const vstring& name, const cocos2d::Vec2& value);
        void set_attribute_value(const vstring& name, const cocos2d::Vec3& value);
        void set_attribute_value(const vstring& name, const cocos2d::Size& value);

        cocos2d::Color3B get_attribute_value(const vstring& name, const cocos2d::Color3B& default_value = cocos2d::Color3B::WHITE) const;
        cocos2d::Color4B get_attribute_value(const vstring& name, const cocos2d::Color4B& default_value = cocos2d::Color4B::WHITE) const;
        cocos2d::Color4F get_attribute_value(const vstring& name, const cocos2d::Color4F& default_value = cocos2d::Color4F::WHITE) const;
        cocos2d::Rect    get_attribute_value(const vstring& name, const cocos2d::Rect& default_value = cocos2d::Rect::ZERO) const;
        cocos2d::Vec2    get_attribute_value(const vstring& name, const cocos2d::Vec2& default_value = cocos2d::Vec2::ZERO) const;
        cocos2d::Size    get_attribute_value(const vstring& name, const cocos2d::Size& default_value = cocos2d::Size::ZERO) const;
#endif

    private:
        xml4wNodePtr    _Mynode;
    }; /* CLASS element */


    class XMLDRV_DLL document
    {
        friend class element;
        document(const document& right);
        document& operator=(const document& right);
    public:
        document(void);

        /*
        * mode: "#disk", "#buffer", "#memory"
        */
        document(const char* name, const char* mode = "#disk", int namelen = -1);

        /* std::move support */
        document(document&& right) { impl_ = right.impl_; right.impl_ = nullptr; }
        document& operator=(document&& right) { impl_ = right.impl_; right.impl_ = nullptr; return *this; }

        ~document(void);

        bool                open(const char* name, const char* mode = "#disk", int namelen = -1);

        /* @brief  : Open an exist XML document
        ** @params :
        **        filename: specify the XML filename of to open.
        **
        ** @returns: No Explain...
        */
        bool                openf(const char* filename);


        /* @brief  : Create a new XML document, if the document already exist, the old file will be overwritten.
        ** @params :
        **        filename: specify the filename of new XML document
        **        rootname: specify the rootname of new XML document
        **
        ** @returns: No Explain...
        */
        bool                openn(const char* rootname, const char* filename = "");
        bool                openn(); // open a empty
        element             set_root(element root);


        /* @brief  : Open from xml formated string
        ** @params :
        **        xmlstring: xml formated string, such as
        **                   "<peoples><people><name>xxx</name><sex>female</sex></people></peoples>"
        **        length   : the length of xmlstring
        **
        ** @returns: No Explain...
        */
        bool                openb(const char* xmlstring, int length);

        bool                openb(std::string&& xmlstring);


        /* @brief  : Save document to disk with filename when it was opened
        ** @params : None
        **
        ** @returns: None
        */
        void                save(bool formatted = false) const;


        /* @brief  : Save document to disk with new filename
        ** @params :
        **           filename: specify the new filename
        **
        ** @returns: None
        */
        void                save(const char* filename, bool formatted = false) const;


        /* @brief: Close the XML document and release all resource.
        ** @params : None
        **
        ** @returns: None
        */
        void                close(void);


        /* @brief: Is open succeed.
        ** @params : None
        **
        ** @returns: None
        */
        bool                is_open(void) const;


        /* @brief: Get the root element of the document.
        ** @params : None
        **
        ** @returns: the root element of the document
        */
        element             root(void) const;


        /* @brief: Get element by XPATH
        ** @params :
        **          xpath: such as: "/peoples/people/name"
        **          index: the index of the element set, default: the first.
        **
        ** @returns: the element of the document with spacified XPATH
        ** @remark: only libxml2, xercesc support this
        */
        element             select_element(const char* xpath, int index = 0) const;

        //#if !defined(_USING_TINYXML2) && !defined(_USING_RAPIDXML)
        /* @brief: A function template, "foreach" all element which have specified XPATH.
        ** @params :
        **           xpath: specify the xpath to "foreach"
        **           op: foreach operation
        **
        ** @returns: None
        ** @comment(usage):
        **          please see example in the file: xml4w_testapi.cpp
        */
        template<typename _Handler>
        void                xforeach(const char* xpath, const _Handler&) const;

        /*
        ** @brief: From DOM to String
        ** @params : None.
        **
        ** @returns: XML formated string
        */
        std::string         to_string(bool formatted = false) const;

        element             create_element(const char* name, const char* value = "");

    private:
        xml4wXPathResultPtr xpath_eval(const char* xpath) const;
        xml4wNodePtr        xpath_node_of(xml4wXPathResultPtr, size_t index) const;
        void                xpath_free_result(xml4wXPathResultPtr) const;

    private:
        xml4wDocPtr impl_;
    }; /* CLASS document */
};
/* namespace: xmldrv alias */
namespace xmld = xmldrv;
namespace xml4wrapper = xmldrv;
namespace xml4w = xmldrv;
namespace xml3s = xmldrv;

#include "xmldrv.inl"

#endif /* _XMLDRV_H_ */
/*
* Copyright (c) 2012-2016 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/
