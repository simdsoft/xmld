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
*       3.9.3: element::is_good --> element::is_valid                         *
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
#define _XML4WRAPPER_VERSION "3.9.3"
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
#include "container_helper.h"

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

        element(xml4wNodePtr _Ptr = nullptr);
        ~element(void);

        element         clone(void) const;

        std::string     get_name(void) const;
        std::string     get_value(const char* default_value = "") const;
        std::string     get_value(const std::string& default_value = "") const;
        std::string     get_attribute_value(const char* name, const char* = "") const;
        std::string     get_attribute_value(const char* name, const std::string& default_value = "") const;
        element         get_parent(void) const;
        element         get_prev_sibling(void) const;
        element         get_next_sibling(void) const;
        element         get_first_child(void) const;

        element         operator[](int index) const;
        element         operator[](const char* name) const;
        element         get_child(int index = 0) const;
        element         get_child(const char* name, int index = 0) const;
        /*void            get_children(std::vector<element>& children) const;
        void            get_children(const char* name, std::vector<element>& children) const;
*/
        void            set_value(const char* value);
        void            set_value(const std::string& value);
        void            set_attribute_value(const char* name, const char* value);

        element         add_child(const element& element) const;
        element         add_child(const char* name, const char* value = nullptr) const;
        element         add_child(const std::string& name, const char* value = nullptr) const;

        void            remove_child(int index = 0);
        void            remove_child(const char* name, int index = 0);
        void            remove_children(void);
        void            remove_children(const char* name);
        void            remove_self(void);

        std::string     to_string(bool formatted = false) const;

        //  Function TEMPLATEs
        template<typename _Ty>
        _Ty             get_value(const _Ty& default_value = _Ty()) const;


        template<typename _Ty>
        _Ty             get_attribute_value(const char* name, const _Ty& default_value = _Ty()) const;

        template<typename _Ty>
        void            set_value(const _Ty& value);

        template<typename _Ty>
        void            set_attribute_value(const char* name, const _Ty& value);

        template<typename _Handler>
        void            cforeach(const _Handler&) const;

        template<typename _Handler> // op must return bool
        void            cforeach_breakif(const _Handler&) const;

        template<typename _Handler>
        void            cforeach(const char* name, const _Handler&) const;

        template<typename _Handler> // op must return bool
        void            cforeach_breakif(const char* name, const _Handler&) const;

        template<typename _Handler> // foreach attribute, op protype: (const unmanaged_string& name, const unmanaged_string& value)
        void            pforeach(const _Handler& );

        void*           first_attribute();

        static void*     next_attribute(void* attr);
        static unmanaged_string name_of_attr(void* attr);
        static unmanaged_string value_of_attr(void* attr);

        bool            is_valid(void) const { return _Mynode != nullptr; }
        operator xml4wNodePtr(void) { return _Mynode; }
        operator xml4wNodePtr(void) const { return _Mynode;}
        
#if _USE_IN_COCOS2DX
        void set_attribute_value(const char* name, const cocos2d::Color3B& value);
        void set_attribute_value(const char* name, const cocos2d::Color4B& value);
        void set_attribute_value(const char* name, const cocos2d::Color4F& value);
        void set_attribute_value(const char* name, const cocos2d::Rect& value);
        void set_attribute_value(const char* name, const cocos2d::Vec2& value);
        void set_attribute_value(const char* name, const cocos2d::Vec3& value);
        void set_attribute_value(const char* name, const cocos2d::Size& value);

        cocos2d::Color3B get_attribute_value(const char* name, const cocos2d::Color3B& default_value = cocos2d::Color3B::WHITE) const;
        cocos2d::Color4B get_attribute_value(const char* name, const cocos2d::Color4B& default_value = cocos2d::Color4B::WHITE) const;
        cocos2d::Color4F get_attribute_value(const char* name, const cocos2d::Color4F& default_value = cocos2d::Color4F::WHITE) const;
        cocos2d::Rect    get_attribute_value(const char* name, const cocos2d::Rect& default_value = cocos2d::Rect::ZERO) const;
        cocos2d::Vec2    get_attribute_value(const char* name, const cocos2d::Vec2& default_value = cocos2d::Vec2::ZERO) const;
        cocos2d::Size    get_attribute_value(const char* name, const cocos2d::Size& default_value = cocos2d::Size::ZERO) const;
        /*template<> cocos2d::Color3B get_attribute_value<cocos2d::Color3B>(const char* name, const cocos2d::Color3B& default_value) const;
        template<> cocos2d::Color4B get_attribute_value<cocos2d::Color4B>(const char* name, const cocos2d::Color4B& default_value) const;
        template<> cocos2d::Color4F get_attribute_value<cocos2d::Color4F>(const char* name, const cocos2d::Color4F& default_value) const;
        template<> cocos2d::Rect    get_attribute_value<cocos2d::Rect>(const char* name, const cocos2d::Rect& default_value) const;*/
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


        /* @brief  : Open from xml formated string
        ** @params :
        **        xmlstring: xml formated string, such as
        **                   "<peoples><people><name>xxx</name><sex>female</sex></people></peoples>"
        **        length   : the length of xmlstring
        **
        ** @returns: No Explain...
        */ 
        bool                openb(const char* xmlstring, int length);


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

    private:
        xml4wDocPtr impl_;
    }; /* CLASS document */

    /*
    ** set xmldrv use AES security mode.
    */
    void set_security(bool security);

    /*
    ** set xmldrv use AES key mode 0~32bytes.
    */
    void set_encrypt_key(const char* key);
    void set_encrypt_key(const void* key, size_t size);

};
/* namespace: xmldrv alias */
namespace xmld = xmldrv;
namespace xml4wrapper = xmldrv;
namespace xml4w = xmldrv;
namespace xml3s = xmldrv;

#include "xmldrv.inl"

#endif /* _XMLDRV_H_ */
/*
* Copyright (c) 2012-2015 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/
