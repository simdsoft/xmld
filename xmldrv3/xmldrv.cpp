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
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "super_cast.h"
#include "xmldrv.h"

#if _USE_IN_COCOS2DX
#include <cocos2d.h>

#include "object_pool.h"
#include "xxfsutility.h"
#endif

#if !defined(_STD)
#define _STD ::std::
#endif

namespace xmldrv {
    xmld::string empty_string;
}

using namespace xmldrv;

#if !_USE_IN_COCOS2DX
// xmldrv file read & write support
namespace {
    long get_file_size(const char *path)
    {
        long filesize = -1;
        struct stat statbuff;
        if (stat(path, &statbuff) < 0) {
            return filesize;
        }
        else {
            filesize = statbuff.st_size;
        }
        return filesize;
    }

    long get_file_size(FILE* fp)
    {
        fseek(fp, 0, SEEK_END);
        long length = ftell(fp);
        if (length != 0)
        {
            fseek(fp, 0, SEEK_SET);
        }
        return length;
    }

    _STD string read_file_data(const char* filename)
    {
        FILE* fp = fopen(filename, "rb");
        if (fp == nullptr)
            return (const char*)"";

        size_t size = get_file_size(fp);
        if (size == 0)
            return "";

        _STD string storage(size, '\0');

        size_t bytes_readed = fread(&storage.front(), 1, size, fp);

        fclose(fp);
        if (bytes_readed < size)
            storage.resize(bytes_readed);
        return storage;
    }

    bool write_file_data(const char* filename, const char* data, size_t size)
    {
        FILE* fp = fopen(filename, "wb+");
        if (fp == nullptr)
            return false;
        fwrite(data, size, 1, fp);
        fclose(fp);
        return true;
    }

    bool  write_file_data(const char* filename, const _STD string& content)
    {
        return write_file_data(filename, content.c_str(), content.size());
    }
};
#endif

/*************************common impl******************/
#ifndef _IMPL_COMM
#define _IMPL_COMM 1
#endif
#ifdef _IMPL_COMM
element::element(xml4wNodePtr _Ptr) : _Mynode(_Ptr)
{
}

element::~element(void)
{
}

element::element(const element& rhs)
{
    _Mynode = rhs._Mynode;
}

element& element::operator=(const element& rhs)
{
    _Mynode = rhs._Mynode;
    return *this;
}

element::element(element&& rhs)
{
    _Mynode = rhs._Mynode;
    rhs._Mynode = nullptr;
}

element& element::operator=(element&& rhs)
{
    _Mynode = rhs._Mynode;
    rhs._Mynode = nullptr;
    return *this;
}
element element::operator[](int index) const
{
    return this->get_child(index);
}

element element::operator[](const xmld::string& name) const
{
    auto child = this->get_child(name);
    if (child.is_good())
    {
        return child;
    }
    return this->add_child(name);
}

element element::get_child(const xmld::string& name, int index) const
{
    auto ptr = *this;
    __xml4wts_algo_cond(ptr,
        ptr.get_first_child(),
        ptr.get_next_sibling(),
        name == ptr.get_name() && (0 == index--),
        break
    );
    return ptr;
}

element element::get_child(int index) const
{
    auto ptr = *this;
    __xml4wts_algo_cond(ptr,
        ptr.get_first_child(),
        ptr.get_next_sibling(),
        (0 == index--),
        break
    );
    return ptr;
}

void element::remove_child(int index)
{
    this->get_child(index).remove_self();
}

void element::remove_child(const xmld::string& name, int index)
{
    this->get_child(name, index).remove_self();
}

//void element::set_value(const _STD string& value)
//{
//    this->set_value(value.c_str());
//}

document::document(void)
    : impl_(nullptr)
{
}

document::document(const char* name, const char* mode, int namelen)
    : impl_(nullptr)
{
    this->open(name, mode, namelen);
}

document::~document(void)
{
    this->close();
}

bool document::is_open(void) const
{
    return impl_ != nullptr;
}

#endif /* end of common impl */

/*************************rapidxml wrapper impl*****************************/
#ifdef _USING_RAPIDXML 
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_print.hpp"

#define ensure_persisted(vstr, palloc) vstr.persisted() ? vstr.c_str() : palloc->allocate_string(vstr.c_str(), vstr.size())

inline rapidxml::xml_node<>* _detail(void* raw)
{
    return (rapidxml::xml_node<>*)raw;
}
inline rapidxml::xml_node<>* _detail(const element& elem)
{
    return _detail(static_cast<void*>(elem));
}
inline bool is_element(xml4wNodePtr ptr)
{
    return _detail(ptr)->type() == rapidxml::node_element;
}

void attribute::set_value(const xmld::string& value)
{
    if (is_good()) {
        auto internal = (rapidxml::xml_attribute<char>*)_Ptr;

        auto palloc = internal->parent()->get_pool();
        internal->value(ensure_persisted(value, palloc));
    }
}

xmld::string attribute::get_value() const
{
    if (is_good()) {
        auto internal = (rapidxml::xml_attribute<char>*)_Ptr;
        return xmld::string(internal->value(), internal->value_size());
    }

    return "";
}

xmld::string attribute::get_name() const
{
    if (is_good()) {
        auto internal = (rapidxml::xml_attribute<char>*)_Ptr;
        return xmld::string(internal->name(), internal->name_size());
    }

    return "";
}

void attribute::set_name(const xmld::string& name)
{
    if (is_good()) {
        auto internal = (rapidxml::xml_attribute<char>*)_Ptr;
        internal->name(name.c_str(), name.size());
    }
}

attribute attribute::next()
{
    return ++*this;
}

attribute& attribute::operator++() {
    if (is_good()) {
        auto internal = (rapidxml::xml_attribute<char>*)_Ptr;
        *this = reinterpret_cast<xml4wAttribPtr>(internal->next_attribute());
    }
    return *this;
}

bool attribute::is_good() const
{
    return _Ptr != nullptr;
}
element element::add_child(const element& e, bool force_clone) const
{
    if (is_good() && e.is_good()) {
        auto internal = _detail(*this);
        auto wpool = internal->get_pool();
        auto require_clone = wpool != _detail(e)->get_pool();
        if (!require_clone && !force_clone) {
            internal->append_node(_detail(e));
            return e;
        }
        else {
            rapidxml::xml_node<>* cloned = nullptr;
            cloned = wpool->clone_node(_detail(e));
            internal->append_node(cloned);
            return xmld::element(cloned);
        }
    }
    return element(nullptr);
}

element element::clone(void) const
{
    if (is_good())
    {
        auto palloc = _detail(_Mynode)->get_pool();
        if (palloc != nullptr)
            return element(palloc->clone_node(_detail(_Mynode)));
    }
    return element(nullptr);
}

element element::get_parent(void) const
{
    if (is_good())
        return element(_detail(_Mynode)->parent());
    return element(nullptr);
}

element element::get_first_child(void) const
{
    auto ptr = _detail(_Mynode);
    __xml4wts_algo_cond(ptr,
        ptr->first_node(),
        ptr->next_sibling(),
        is_element(ptr),
        break
    );
    return (element)ptr;
}

element element::get_prev_sibling(void) const
{
    auto ptr = _detail(_Mynode);
    __xml4wts_algo_cond(ptr,
        ptr->previous_sibling(),
        ptr->previous_sibling(),
        is_element(ptr),
        break
    );
    return (element)ptr;
}

element element::get_next_sibling(void) const
{
    auto ptr = _detail(_Mynode);
    __xml4wts_algo_cond(ptr,
        ptr->next_sibling(),
        ptr->next_sibling(),
        is_element(ptr),
        break
    );
    return (element)ptr;
}

bool element::has_attribute(const xmld::string& name) const
{
    return is_good() && _detail(_Mynode)->first_attribute(name.c_str(), name.size()) != nullptr;
}

xmld::string element::get_name(void) const
{
    if (is_good()) {
        return xmld::string(_detail(_Mynode)->name(), _detail(_Mynode)->name_size());
    }
    return "null";
}

void element::set_name(const xmld::string& s) 
{
    if (is_good()) {
      auto internal = _detail(_Mynode);
      internal->name(ensure_persisted(s, internal->get_pool()), s.size());
    }
}

xmld::string element::get_value(const xmld::string& default_value) const
{
    if (is_good()) {
        return xmld::string(_detail(_Mynode)->value(), _detail(_Mynode)->value_size());
    }

    _STD cerr << "xml4w::element::get_value failed, _detail(element:" << this->get_name() << "), use the default value["
        << default_value << "] insteaded!\n";

    return default_value;
}

void element::remove_attribute(const xmld::string& name)
{
    if (is_good())
    {
        auto where = _detail(_Mynode)->first_attribute(name.c_str(), name.size());
        if (where != nullptr)
            _detail(_Mynode)->remove_attribute(where);
    }
}

void  element::remove_all_attributes()
{
    if (is_good())
    {
        _detail(_Mynode)->remove_all_attributes();
    }
}
xmld::string element::get_attribute_value(const xmld::string& name, const xmld::string& default_value) const
{
    if (is_good())
    {
        auto attr = _detail(_Mynode)->first_attribute(name.c_str(), name.size());

        if (attr != nullptr) {
            return xmld::string(attr->value(), attr->value_size());
        }
    }

    _STD cerr << "xml4w::element::get_attribute_value failed, _detail(element:" << this->get_name() << ",property:" << name << "), use the default value["
        << default_value << "] insteaded!\n";

    return default_value;
}

attribute  element::first_attribute() const
{
    if (is_good())
    {
        auto attr = _detail(_Mynode)->first_attribute();

        return reinterpret_cast<xml4wAttribPtr>(attr);
    }
    return nullptr;
}

void element::set_value(const xmld::string& value)
{
    if (is_good())
    {
        auto parent = _detail(_Mynode)->parent();
        auto palloc = _detail(_Mynode)->get_pool();
        _detail(_Mynode)->value(ensure_persisted(value, palloc), value.size());
    }
}

#if _USE_IN_COCOS2DX

void element::set_attribute_value(const xmld::string& name, const cocos2d::Color3B& value)
{
    char svalue[128] = { 0 };
    auto n = sprintf(svalue, "%u,%u,%u", (unsigned int)value.r, (unsigned int)value.g, (unsigned int)value.b);
    this->set_attribute_value(name, xmld::string(svalue, n));
}

void element::set_attribute_value(const xmld::string& name, const cocos2d::Color4B& value)
{
    char svalue[128] = { 0 };
    auto n = sprintf(svalue, "%u,%u,%u,%u", (unsigned int)value.r, (unsigned int)value.g, (unsigned int)value.b, (unsigned int)value.a);
    set_attribute_value(name, xmld::string(svalue, n));
}

void  element::set_attribute_value(const xmld::string& name, const cocos2d::Color4F& value)
{
    char svalue[128] = { 0 };
    auto n = sprintf(svalue, "%.3f,%.3f,%.3f,%.3f", value.r, value.g, value.b, value.a);
    set_attribute_value(name, xmld::string(svalue, n));
}


void  element::set_attribute_value(const xmld::string& name, const cocos2d::Rect& value)
{
    char svalue[128] = { 0 };
    auto n = sprintf(svalue, "%.3f,%.3f,%.3f,%.3f", value.origin.x, value.origin.y, value.size.width, value.size.height);
    set_attribute_value(name, xmld::string(svalue, n));
}

void element::set_attribute_value(const xmld::string& name, const cocos2d::Vec2& value)
{
    char svalue[128] = { 0 };
    auto n = sprintf(svalue, "%.3f,%.3f", value.x, value.y);
    set_attribute_value(name, xmld::string(svalue, n));
}
void element::set_attribute_value(const xmld::string& name, const cocos2d::Vec3& value)
{
    char svalue[128] = { 0 };
    auto n = sprintf(svalue, "%.3f,%.3f, %.3f", value.x, value.y, value.z);
    set_attribute_value(name, xmld::string(svalue, n));
}
void element::set_attribute_value(const xmld::string& name, const cocos2d::Size& value)
{
    char svalue[128] = { 0 };
    auto n = sprintf(svalue, "%.3f,%.3f", value.width, value.height);
    set_attribute_value(name, xmld::string(svalue, n));
}

cocos2d::Color3B element::get_attribute_value(const xmld::string& name, const cocos2d::Color3B& default_value) const
{
    if (is_good())
    {
        auto attr = _detail(_Mynode)->first_attribute(name.c_str(), name.size());
        if (attr != nullptr) {
            auto value = nsc::parse3i(_STD string(attr->value(), attr->value_size()), ',');
            return cocos2d::Color3B(_STD  get<0>(value), _STD get<1>(value), _STD get<2>(value));
        }
    }

    return default_value;
}
cocos2d::Color4B element::get_attribute_value(const xmld::string& name, const cocos2d::Color4B& default_value) const
{
    if (is_good())
    {
        auto attr = _detail(_Mynode)->first_attribute(name.c_str(), name.size());
        if (attr != nullptr) {
            auto value = nsc::parse4i(_STD string(attr->value(), attr->value_size()), ',');
            return cocos2d::Color4B(_STD get<0>(value), _STD get<1>(value), _STD get<2>(value), _STD get<3>(value));
        }
    }

    return default_value;
}
cocos2d::Color4F element::get_attribute_value(const xmld::string& name, const cocos2d::Color4F& default_value) const
{
    if (is_good())
    {
        auto attr = _detail(_Mynode)->first_attribute(name.c_str(), name.size());
        if (attr != nullptr) {
            auto value = nsc::parse4f(_STD string(attr->value(), attr->value_size()), ',');
            return cocos2d::Color4F(_STD get<0>(value), _STD get<1>(value), _STD get<2>(value), _STD get<3>(value));
        }
    }

    return default_value;
}
cocos2d::Rect    element::get_attribute_value(const xmld::string& name, const cocos2d::Rect& default_value) const
{
    if (is_good())
    {
        auto attr = _detail(_Mynode)->first_attribute(name.c_str(), name.size());
        if (attr != nullptr) {
            auto value = nsc::parse4f(_STD string(attr->value(), attr->value_size()), ',');
            return cocos2d::Rect(_STD get<0>(value), _STD get<1>(value), _STD get<2>(value), _STD get<3>(value));
        }
    }

    return default_value;
}
cocos2d::Vec2    element::get_attribute_value(const xmld::string& name, const cocos2d::Vec2& default_value) const
{
    if (is_good())
    {
        auto attr = _detail(_Mynode)->first_attribute(name.c_str(), name.size());
        if (attr != nullptr) {
            auto value = nsc::parse2f(_STD string(attr->value(), attr->value_size()), ',');
            return cocos2d::Vec2(_STD get<0>(value), _STD get<1>(value));
        }
    }

    return default_value;
}
cocos2d::Size    element::get_attribute_value(const xmld::string& name, const cocos2d::Size& default_value) const
{
    if (is_good())
    {
        auto attr = _detail(_Mynode)->first_attribute(name.c_str(), name.size());
        if (attr != nullptr) {
            auto value = nsc::parse2f(_STD string(attr->value(), attr->value_size()), ',');
            return cocos2d::Size(_STD get<0>(value), _STD get<1>(value));
        }
    }


    return default_value;
}

#endif

void element::set_attribute_value(const xmld::string& name, const xmld::string& value)
{ // pitfall: string-literal
    if (is_good()) {
        auto where = _detail(_Mynode)->first_attribute(name.c_str(), name.size());
        auto palloc = _detail(_Mynode)->get_pool();
        if (where) {
            where->value(ensure_persisted(value, palloc), value.size());
        }
        else {
            _detail(_Mynode)->insert_attribute(where,
                palloc->allocate_attribute(ensure_persisted(name, palloc),
                    ensure_persisted(value, palloc),
                    name.size(),
                    value.size()));
        }
    }
}

element element::add_child(const xmld::string& name, const xmld::string& value /* = nullptr */) const
{
    if (is_good()) {
        auto palloc = _detail(_Mynode)->get_pool();
        auto newnode = palloc->allocate_node(rapidxml::node_type::node_element,
            ensure_persisted(name, palloc),
            value.empty() ? nullptr : ensure_persisted(value, palloc),
            name.size(), value.size());
        _detail(_Mynode)->append_node(newnode);
        return (element)newnode;
    }
    return (element)nullptr;
}

void element::remove_children(void)
{
    if (is_good()) {
        _detail(_Mynode)->remove_all_nodes();
    }
}

void element::remove_children(const xmld::string& name)
{
    if (is_good())
    {
        auto first = _detail(_Mynode)->first_node();
        decltype(first) next = nullptr;
        for (decltype(first)* curr = &first; *curr;)
        {
            decltype(first) entry = *curr;
            if (0 == memcmp(entry->name(), name.c_str(), (_STD min)(entry->name_size(), name.size())))
            {
                *curr = entry->next_sibling();
                _detail(_Mynode)->remove_node(entry);
            }
            else {
                next = entry->next_sibling();
                curr = &next;
            }
        }
    }
}

void element::remove_self(void)
{
    if (is_good()) {
        _detail(_Mynode)->parent()->remove_node(_detail(_Mynode));
        _Mynode = nullptr;
    }
}

_STD string element::to_string(bool formatted) const
{
    if (is_good()) {
        _STD string text;
        rapidxml::print(_STD back_inserter(text), *_detail(_Mynode), !formatted ? rapidxml::print_no_indenting : 0);
        return text;
    }
    return "";
}

/// get_value APIs
#define _IMPL_GETVAL(type,s2i) \
type   element::get_value(type value, int radix) const \
{ \
    if (is_good()) { \
        return s2i(get_value(xmld::string()).c_str(), nullptr, radix); \
    } \
    return value; \
}

_IMPL_GETVAL(int8_t, strtol)
_IMPL_GETVAL(int16_t, strtol)
_IMPL_GETVAL(int32_t, strtol)
_IMPL_GETVAL(int64_t, strtoll)

_IMPL_GETVAL(uint8_t, strtoul)
_IMPL_GETVAL(uint16_t, strtoul)
_IMPL_GETVAL(uint32_t, strtoul)
_IMPL_GETVAL(uint64_t, strtoull)

float         element::get_value(float value) const
{
    if (is_good())
        return strtof(get_value(xmld::string()).c_str(), nullptr);
    return value;
}
double        element::get_value(double value) const
{
    if (is_good())
        return strtod(get_value(xmld::string()).c_str(), nullptr);
    return value;
}

/// get_attribute_value APIs
#define _IMPL_GET_ATTRIVAL(type,s2i) \
type  element::get_attribute_value(const xmld::string& name, type value, int radix) const \
{ \
    if (has_attribute(name)) { \
        return s2i(get_attribute_value(name, xmld::string()).c_str(), nullptr, radix); \
    } \
    return value; \
}

_IMPL_GET_ATTRIVAL(int8_t, strtol)
_IMPL_GET_ATTRIVAL(int16_t, strtol)
_IMPL_GET_ATTRIVAL(int32_t, strtol)
_IMPL_GET_ATTRIVAL(int64_t, strtoll)

_IMPL_GET_ATTRIVAL(uint8_t, strtoul)
_IMPL_GET_ATTRIVAL(uint16_t, strtoul)
_IMPL_GET_ATTRIVAL(uint32_t, strtoul)
_IMPL_GET_ATTRIVAL(uint64_t, strtoull)

float         element::get_attribute_value(const xmld::string& name, float value) const
{
    if (has_attribute(name)) {
        return strtof(get_attribute_value(name, xmld::string()).c_str(), nullptr);
    }
    return value;
}
double        element::get_attribute_value(const xmld::string& name, double value) const
{
    if (has_attribute(name)) {
        return strtod(get_attribute_value(name, xmld::string()).c_str(), nullptr);
    }
    return value;
}

#define _IMPL_SETVAL(type, fmt, capacity) \
void xmldrv::element::set_value(const type & value) \
{ \
    char svalue[capacity]; \
    auto n = sprintf(svalue, fmt, value); \
    set_value(xmld::string(svalue, n)); \
}

_IMPL_SETVAL(char, "%d", 8)
_IMPL_SETVAL(short, "%d", 8)
_IMPL_SETVAL(int, "%d", 16)
_IMPL_SETVAL(long long, "%lld", 64)
_IMPL_SETVAL(unsigned char, "%u", 8)
_IMPL_SETVAL(unsigned short, "%u", 8)
_IMPL_SETVAL(unsigned int, "%u", 16)
_IMPL_SETVAL(unsigned long long, "%llu", 64)

void xmldrv::element::set_value(const float & value)
{
    char svalue[64];
    int n = sprintf(svalue, "%.*g", 16, value);
    set_value(xmld::string(svalue, n));
}

void xmldrv::element::set_value(const double & value)
{
    char svalue[64];
    int n = sprintf(svalue, "%.*g", 16, value);
    set_value(xmld::string(svalue, n));
}

#define _IMPL_SET_ATTRI_VAL(type, fmt, capacity) \
void xmldrv::element::set_attribute_value(const xmld::string & name, const type & value) \
{ \
    char svalue[capacity]; \
    int n = sprintf(svalue, fmt, value); \
    set_attribute_value(name, xmld::string(svalue, n)); \
}

_IMPL_SET_ATTRI_VAL(char, "%d", 8)
_IMPL_SET_ATTRI_VAL(short, "%d", 8)
_IMPL_SET_ATTRI_VAL(int, "%d", 16)
_IMPL_SET_ATTRI_VAL(long long, "%lld", 64)
_IMPL_SET_ATTRI_VAL(unsigned char, "%u", 8)
_IMPL_SET_ATTRI_VAL(unsigned short, "%u", 8)
_IMPL_SET_ATTRI_VAL(unsigned int, "%u", 16)
_IMPL_SET_ATTRI_VAL(unsigned long long, "%llu", 64)

void xmldrv::element::set_attribute_value(const xmld::string & name, const float& value)
{
    char svalue[64];
    int n = sprintf(svalue, "%.*g", 16, value);
    set_attribute_value(name, xmld::string(svalue, n));
}

void xmldrv::element::set_attribute_value(const xmld::string & name, const double & value)
{
    char svalue[64];
    int n = sprintf(svalue, "%.*g", 16, value);
    set_attribute_value(name, xmld::string(svalue, n));
}


/*----------------------------- friendly division line ----------------------------------------*/
namespace rapidxml {
    const int parse_normal = (/*rapidxml::parse_no_string_terminators | */rapidxml::parse_no_data_nodes);
};

struct xml4wDoc
{
    _STD string              filename;
    _STD string              originalFileName;
    _STD string              buf;
    rapidxml::xml_document<> doc;
};

namespace xmldrv {
    _STD string& internalGetDocPath(const document* doc)
    {
        xml4wDoc* impl = (xml4wDoc*)(*super_cast::force_cast<uintptr_t*>(doc));
        return impl->filename;
    }

    _STD string& internalGetOriginalDocPath(const document* doc)
    {
        xml4wDoc* impl = (xml4wDoc*)(*super_cast::force_cast<uintptr_t*>(doc));
        return impl->originalFileName;
    }
}

bool document::open(const char* name, const char* mode, int namelen)
{
    if (0 == _stricmp(mode, "#memory")) {
        return openn(name);
    }
    else if (0 == _stricmp(mode, "#buffer")) {
        return openb(name, namelen);
    }
    else if (0 == _stricmp(mode, "#disk") || 0 == _stricmp(mode, "#file"))
    {
        return openf(name);
    }
    else
        return false;
}

bool document::readfile(const char* filename)
{
    if (!is_open()) {

#if _USE_IN_COCOS2DX
        _STD string tempData = cocos2d::FileUtils::getInstance()->getStringFromFile(filename);
#else
        _STD string tempData = read_file_data(filename);
#endif
        if (tempData.empty())
            return false;

        try {
            impl_ = new xml4wDoc();
            impl_->filename = filename;
            impl_->originalFileName = filename;
            impl_->buf = _STD move(tempData);
        }
        catch (...)
        {
            if (impl_ != nullptr) {
                delete impl_;
                impl_ = nullptr;
            }
        }

        return is_open();
    }

    return false;
}

bool document::openf(const char* filename)
{
    if (readfile(filename)) {
        return parse_default();
    }

    return false;
}

bool document::openn(const char* rootname, const char* filename)
{
    if (!is_open()) {
        impl_ = new(_STD nothrow) xml4wDoc();
        if (impl_ != nullptr)
        {
            impl_->doc.append_node(impl_->doc.allocate_node(rapidxml::node_type::node_element, rootname));
            impl_->filename = filename;
            impl_->originalFileName = filename;
        }
    }
    return is_open();
}

bool document::openn()
{
    if (!is_open()) {
        impl_ = new(_STD nothrow) xml4wDoc();
    }
    return is_open();
}

element document::document_element(element newroot)
{
    if (is_open()) {
        impl_ = new(_STD nothrow) xml4wDoc();
        if (impl_ != nullptr)
        {
            impl_->doc.remove_all_nodes();
            impl_->doc.append_node(_detail(newroot));
        }
    }

    return this->document_element();
}

bool document::openb(const char* xmlstring, int length)
{
    if (setbuf(xmlstring, length))
        return parse_default();

    return false;
}

bool document::openb(_STD string&& xmlstring)
{
    if (setbuf(_STD move(xmlstring)))
        return parse_default();
    
    return false;
}

bool document::setbuf(const char* buf, int length)
{
    if (!is_open()) {
        if (length == -1)
            length = strlen(buf);
        if (length == 0)
            return false;

        impl_ = new xml4wDoc();
        impl_->buf.assign(buf, length);
        return true;
    }
    return false;
}

bool document::setbuf(std::string&& xmlstring)
{
    if (!is_open() && !xmlstring.empty()) {
        impl_ = new xml4wDoc();
        impl_->buf = _STD move(xmlstring);
        return true;
    }
    return false;
}

std::string& document::getbuf()
{
    assert(is_open());

    return impl_->buf;
}

template<int flags> inline
bool do_parse_internal(xmld::document* d) {
    try {
        auto& buf = d->getbuf();
        d->internal_object()->doc.parse<flags>((char*)&buf.front(), (int)buf.size());
    }
    catch (rapidxml::parse_error& /*e*/)
    {
        d->close();
    }
    return d->is_open();
}

bool document::parse_default()
{
    return do_parse_internal<rapidxml::parse_normal>(this);
}

bool document::parse_with_pi()
{
    return do_parse_internal<rapidxml::parse_normal | rapidxml::parse_declaration_node | rapidxml::parse_comment_nodes>(this);
}

bool document::parse_with_comment()
{
    return do_parse_internal<rapidxml::parse_normal | rapidxml::parse_comment_nodes>(this);
}

bool document::parse_full()
{
    return do_parse_internal<rapidxml::parse_full>(this);
}

void document::save(bool formatted) const
{
    if (is_open())
        this->save(impl_->filename.c_str(), formatted);
}

void document::save(const char* filename, bool formatted) const
{
    if (is_open()) {
        _STD string stream = this->to_string(formatted);

#if _USE_IN_COCOS2DX
        fsutil::write_file_data(filename, stream.data(), stream.size());
#else
        write_file_data(filename, stream);
#endif
        if (this->impl_->filename.empty()) {
            this->impl_->filename = filename;
        }
    }
}

element document::document_element(void) const
{
    if (impl_ != nullptr) {
        for (auto ptr = impl_->doc.first_node(); ptr != nullptr; ptr = ptr->next_sibling())
            if (ptr->type() == rapidxml::node_element)
                return element(ptr);
    }

    return element(nullptr);
}

element document::document_declaration() const
{
    if (impl_ != nullptr) {
        for (auto ptr = impl_->doc.first_node(); ptr != nullptr; ptr = ptr->next_sibling())
            if (ptr->type() == rapidxml::node_declaration)
                return element(ptr);
    }

    return element(nullptr);
}

element document::create_element(const char* name, const char* value)
{
    auto newe = impl_->doc.allocate_node(rapidxml::node_type::node_element, impl_->doc.allocate_string(name), impl_->doc.allocate_string(value));
    return (element)newe;
}

element document::select_element(const char*, int) const
{
    throw _STD logic_error("It's just supported by xerces-c which has 3.0.0 or more version for xml4w using xpath to operate xml!");
    return (element)nullptr;
}

_STD string document::to_string(bool formatted) const
{
    if (is_open())
    {
        return ((element)&impl_->doc).to_string(formatted);
    }
    return "";
}

#elif defined(_USING_TINYXML2)
/*************************tinyxml wrapper impl*****************************/
#include "tinyxml/tinyxml2.h"

inline tinyxml2::XMLNode* _detail(void* raw)
{
    return (tinyxml2::XMLNode*)raw;
}

inline tinyxml2::XMLNode* _detail(const element& elem)
{
    return _detail(static_cast<void*>(elem));
}

element element::clone(void) const
{
    if (is_good())
    {
        return _detail(_Mynode)->ShallowClone(_detail(_Mynode)->GetDocument());
    }
    return nullptr;
}

element element::get_parent(void) const
{
    if (is_good())
    {
        return _detail(_Mynode)->Parent();
    }
    return nullptr;
}

element element::get_first_child(void) const
{
    if (is_good())
        return _detail(_Mynode)->FirstChildElement();
    return nullptr;
}

element element::get_prev_sibling(void) const
{
    if (is_good())
        return _detail(_Mynode)->PreviousSiblingElement();
    return nullptr;
}

element element::get_next_sibling(void) const
{
    if (is_good())
        return _detail(_Mynode)->NextSiblingElement();
    return nullptr;
}

_STD string element::get_name(void) const
{
    if (is_good()) {
        return dynamic_cast<tinyxml2::XMLElement*>(_detail(_Mynode))->Name();
    }
    return "null";
}

_STD string element::get_value(const char* default_value) const
{
    if (is_good()) {

        const char* text = _detail(_Mynode)->ToElement()->GetText();
        if (text != nullptr) {
            return text;
        }
    }

    _STD cerr << "xml4w::element::get_value failed, _detail(element:" << this->get_name() << "), use the default value["
        << default_value << "] insteaded!\n";

    return default_value;
}

_STD string element::get_attribute_value(const char* name, const char* default_value) const
{
    if (is_good())
    {
        const char* attr_value = dynamic_cast<tinyxml2::XMLElement*>(_detail(_Mynode))->Attribute(name);
        if (attr_value != nullptr) {
            return dynamic_cast<tinyxml2::XMLElement*>(_detail(_Mynode))->Attribute(name);
        }
    }

    _STD cerr << "xml4w::element::get_attribute_value failed, _detail(element:" << this->get_name() << ",property:" << name << "), use the default value["
        << default_value << "] insteaded!\n";

    return default_value;
}

void element::set_value(const char* value)
{
    if (is_good())
    {
        do { if (_detail(_Mynode)->FirstChild() != nullptr) { _detail(_Mynode)->FirstChild()->SetValue(value); } else { _detail(_Mynode)->InsertFirstChild(_detail(_Mynode)->GetDocument()->NewText(value)); } } while (false);
    }
}

void element::set_attribute_value(const char* name, const char* value)
{
    if (is_good()) {
        _detail(_Mynode)->ToElement()->SetAttribute(name, value);
    }
}

void element::remove_children(void)
{
    if (is_good()) {
        _detail(_Mynode)->DeleteChildren();
    }
}

void element::remove_children(const char* name)
{
    /*_STD vector<element> children;
    this->get_children(name, children);
    _STD for_each(
    children.begin(),
    children.end(),
    _STD mem_fun_ref(&element::remove_self)
    );*/
}

void element::remove_self(void)
{
    if (is_good()) {
        _detail(_Mynode)->GetDocument()->DeleteNode(_detail(_Mynode));
        _Mynode = nullptr;
    }
}

_STD string element::to_string(bool formatted) const
{
    if (is_good()) {
        _detail(_Mynode)->ToElement()->ToText()->Value();
        return "";
    }
    return "";
}

element element::add_child(const char* name, const char* value /* = nullptr */) const
{
    if (is_good())
    {
        auto doc = _detail(_Mynode)->GetDocument();
        if (doc) {
            auto node = doc->NewElement(name);
            if (node) {
                node->SetValue(value);
                _detail(_Mynode)->InsertEndChild(node);
            }
            return node;
        }
    }
    return nullptr;
}


/*----------------------------- friendly division line ----------------------------------------*/

struct xml4wDoc : public tinyxml2::XMLDocument
{
    _STD string filename;
};

bool document::open(const char* filename)
{
    if (!is_open())
    {
        impl_ = new(_STD nothrow) xml4wDoc();
        if (impl_ && tinyxml2::XML_NO_ERROR == impl_->LoadFile(filename))
        {
            impl_->filename = filename;
        }
        else {
            close();
        }
    }
    return is_open();
}

bool document::open(const char* filename, const char* rootname)
{
    if (!is_open()) {
        impl_ = new(_STD nothrow) xml4wDoc();
        impl_->LinkEndChild(impl_->NewDeclaration("1.0"));
        impl_->LinkEndChild((impl_->NewElement(rootname)));
        impl_->filename = /*cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + */filename;
        this->save();
        return true;
    }
    return is_open();
}

bool document::open(const char* xmlstring, int length)
{
    if (!is_open()) {

        impl_ = new(_STD nothrow) xml4wDoc();
        if (tinyxml2::XML_NO_ERROR == impl_->Parse(xmlstring, length))
        {
            impl_->filename = "xml4w.memory.xml";
        }
        else {
            close();
        }
    }
    return is_open();
}

void document::save(bool formatted) const
{
    if (is_open())
        this->save(impl_->filename.c_str());
}

void document::save(const char* filename, bool formatted) const
{
    if (is_open()) {
        impl_->SaveFile(filename, formatted);
    }
}

element document::root(void)
{
    if (is_open())
        return impl_->RootElement();
    return nullptr;
}

_STD string document::to_string(bool formatted) const
{
    if (is_open())
    {
        tinyxml2::XMLPrinter writer(0, !formatted);
        impl_->Print(&writer);
        return writer.CStr();//((element)impl_).to_string(formatted);
    }
    return "";
}

/* end of tinyxml impl */

#elif defined(_USING_PUGIXML)
/*************************pugixml wrapper impl*****************************/
#include "pugixml/pugixml.hpp"

inline pugi::xml_node _detail(void* raw)
{
    return static_cast<pugi::xml_node>((pugi::xml_node_struct*)raw);
}
inline pugi::xml_node _detail(const element& elem)
{
    return _detail(static_cast<void*>(elem));
}
inline xml4wNodePtr simplify(const pugi::xml_node& _detail)
{
    auto temp = _detail;

    auto internal = *reinterpret_cast<void**>(&temp); // internal struct pointer

    return internal;
}
inline bool is_element(xml4wNodePtr ptr)
{
    return _detail(ptr).type() == pugi::node_element;
}

element element::clone(void) const
{ // pugixml does not implement
    return nullptr;
}

element element::get_parent(void) const
{
    if (is_good())
        return simplify(_detail(_Mynode).parent());
    return nullptr;
}

element element::get_first_child(void) const
{
    auto ptr = _Mynode;
    __xml4wts_algo_cond(ptr,
        simplify(_detail(ptr).first_child()),
        simplify(_detail(ptr).next_sibling()),
        is_element(ptr),
        break
    );
    return ptr;
}

element element::get_prev_sibling(void) const
{
    auto ptr = _Mynode;
    __xml4wts_algo_cond(ptr,
        simplify(_detail(ptr).previous_sibling()),
        simplify(_detail(ptr).previous_sibling()),
        is_element(ptr),
        break
    );
    return ptr;
}

element element::get_next_sibling(void) const
{
    auto ptr = _Mynode;
    __xml4wts_algo_cond(ptr,
        simplify(_detail(ptr).next_sibling()),
        simplify(_detail(ptr).next_sibling()),
        is_element(ptr),
        break
    );
    return ptr;
}

_STD string element::get_name(void) const
{
    if (is_good()) {
        return _detail(_Mynode).name();
    }
    return "null";
}

_STD string element::get_value(const char* default_value) const
{
    if (is_good())
    {
        return _detail(_Mynode).text().as_string();
    }

    _STD cerr << "xml4w::element::get_value failed, _detail(element:" << this->get_name() << "), use the default value["
        << default_value << "] insteaded!\n";
    return default_value;
}


_STD string element::get_attribute_value(const char* name, const char* default_value) const
{
    if (is_good())
    {
        // real _detail(_Mynode) pointer is internal struct pointer

        auto attr = _detail(_Mynode).attribute(name);
        if (!attr.empty())
        {
            return attr.value();
        }
    }

    _STD cerr << "xml4w::element::get_attribute_value failed, _detail(element:" << this->get_name() << ",property:" << name << "), use the default value["
        << default_value << "] insteaded!\n";

    return default_value;
}

void element::set_value(const char* value)
{
    if (is_good())
        _detail(_Mynode).set_value(value);
}

void element::set_attribute_value(const char* name, const char* value)
{
    if (is_good()) {
        auto attrib = _detail(_Mynode).attribute(name);
        if (!attrib.empty())
            attrib.set_value(value);
        else {
            _detail(_Mynode).append_attribute(name).set_value(value);
        }
    }
}

void element::remove_children(void)
{
}

void element::remove_children(const char* name)
{
    if (is_good())
        _detail(_Mynode).remove_child(name);
}

void element::remove_self(void)
{
    if (is_good()) {
        auto self = _detail(_Mynode);
        auto parent = self.parent();
        if (!parent.empty())
            parent.remove_child(self);
        _Mynode = nullptr;
    }
}

_STD string element::to_string(bool formatted) const
{
    if (is_good()) {
        _STD ostringstream oss;
        _detail(_Mynode).print(oss, "  ", formatted ? pugi::format_indent : pugi::format_raw);
        return oss.str();
    }
    return "";
}

element element::add_child(const char* name, const char* value) const
{
    if (is_good()) {
        auto newe = _detail(_Mynode).append_child(name);
        newe.set_value(value);
        return simplify(newe);
    }
    return nullptr;
}


/*----------------------------- friendly division line ----------------------------------------*/

struct xml4wDoc
{
    _STD string        filename;
    pugi::xml_document doc;
};

bool document::open(const char* filename)
{
    if (!is_open()) {
        this->impl_ = new (_STD nothrow) xml4wDoc();
        if (this->impl_)
        {
            auto stats = this->impl_->doc.load_file(filename);
            bool succeed = stats;
            if (!succeed)
            {
                const char* msg = stats.description();
                close();
            }
        }

    }
    return is_open();
}

bool document::open(const char* filename, const char* rootname)
{
    if (!is_open()) {
        // TODO: impl pugi
    }
    return is_open();
}

bool document::open(const char* xmlstring, int length)
{
    if (!is_open()) {
        this->impl_ = new (_STD nothrow) xml4wDoc();
        if (this->impl_)
            if (!this->impl_->doc.load_buffer(xmlstring, length))
                close();
    }
    return is_open();
}

element document::root(void)
{
    if (is_open())
        return simplify(impl_->doc.root().first_child());
    return nullptr;
}

void document::save(bool formatted) const
{
    if (is_open())
        this->save(this->impl_->filename.c_str(), formatted);
}

void document::save(const char* filename, bool formatted) const
{
    if (is_open()) {
        _STD ofstream fout(filename, _STD ios_base::binary);
        if (fout.is_open())
            this->impl_->doc.save(fout, "  ", (formatted ? pugi::format_indent : pugi::format_raw) | pugi::format_save_file_text);
    }
}

_STD string document::to_string(bool formatted) const
{
    if (is_open())
    {
        _STD ostringstream ss;
        this->impl_->doc.save(ss, "  ", formatted ? pugi::format_indent : pugi::format_raw);
        return ss.str();
    }
    return "";
}

xml4wXPathResultPtr document::xpath_eval(const char* xpath) const
{
    if (is_open())
        return new pugi::xpath_node_set(impl_->doc.select_nodes(xpath));
    return nullptr;
}

xml4wNodePtr        document::xpath_node_of(xml4wXPathResultPtr ptr, size_t index) const
{
    auto _detail = (pugi::xpath_node_set*)ptr;
    if (index < _detail->size())
        return (*_detail)[index];
    return nullptr;
}

void                document::xpath_free_result(xml4wXPathResultPtr ptr) const
{
    auto _detail = (pugi::xpath_node_set*)ptr;
    if (_detail)
        delete _detail;
}

/* end of pugixml impl */


/*************************rapidxml wrapper impl*****************************/
#elif defined(_USING_LIBXML2)
#include <libxml/xpath.h>

inline xmlNodePtr _detail(void* raw)
{
    return (xmlNodePtr)raw;
}
inline xmlNodePtr _detail(const element& elem)
{
    return _detail(static_cast<void*>(elem));
}
inline bool is_element(xml4wNodePtr ptr)
{
    return _detail(ptr)->type == XML_ELEMENT_NODE;
}

element element::clone(void) const
{
    if (is_good())
    {
        return xmlCopyNode(_detail(_Mynode), 1);
    }
    return nullptr;
}

element element::get_parent(void) const
{
    if (is_good())
        return _detail(_Mynode)->parent;
    return nullptr;
}

element element::get_prev_sibling(void) const
{
    auto ptr = _detail(_Mynode);
    __xml4wts_algo_cond(ptr,
        ptr->prev,
        ptr->prev,
        is_element(ptr),
        break
    );
    return ptr;
}

element element::get_next_sibling(void) const
{
    auto ptr = _detail(_Mynode);
    __xml4wts_algo_cond(ptr,
        ptr->next,
        ptr->next,
        is_element(ptr),
        break
    );
    return ptr;
}

element element::get_first_child(void) const
{
    auto ptr = _detail(_Mynode);
    __xml4wts_algo_cond(ptr,
        ptr->children,
        ptr->next,
        is_element(ptr),
        break
    );
    return ptr;
}

_STD string element::get_name(void) const
{
    if (is_good()) {
        return (const char*)_detail(_Mynode)->name;
    }
    return "null";
}

_STD string element::get_value(const char* default_value) const
{
    if (is_good()) {
        pod_ptr<char>::type value((char*)xmlNodeGetContent(_detail(_Mynode)));
        return value.get();
    }

    _STD cerr << "xml4w::element::get_value failed, _detail(element:" << this->get_name() << "), use the default value["
        << default_value << "] insteaded!\n";

    return default_value;
}

_STD string element::get_attribute_value(const char* name, const char* default_value) const
{
    if (is_good())
    {
        pod_ptr<xmlChar>::type value(xmlGetProp(_detail(_Mynode), BAD_CAST name));
        if (value != nullptr)
        {
            return (const char*)value.get()/*, n*/;
        }
    }

    _STD cerr << "xml4w::element::get_attribute_value failed, _detail(element:" << this->get_name() << ",property:" << name << "), use the default value["
        << default_value << "] insteaded!\n";

    return default_value;
}

void element::set_value(const char* value)
{
    if (is_good())
    {
        xmlNodeSetContent(_detail(_Mynode), BAD_CAST value);
    }
}

void element::set_attribute_value(const char* name, const char* value)
{
    if (is_good()) {
        xmlSetProp(_detail(_Mynode), BAD_CAST name, BAD_CAST value);
    }
}

//element element::add_child(const element& e) const
//{
//    if (is_good() && e.is_good()) {
//        xmlAddChild(_detail(_Mynode), _detail(e));
//        return e;
//    }
//    return nullptr;
//}

void element::remove_children(void)
{
    if (is_good()) {
        // xmlDOMWrapRemoveNode
        // _detail(_Mynode)->remove_all_nodes();
    }
}

void element::remove_children(const char* name)
{
    if (is_good())
    {
        /*auto first = _detail(_Mynode)->first_node();
        decltype(first) next = nullptr;
        for (decltype(first)* curr = &first; *curr;)
        {
        decltype(first) entry = *curr;
        if (0 == memcmp(entry->name(), name, (_STD min)(entry->name_size(), strlen(name))))
        {
        *curr = entry->next_sibling();
        _detail(_Mynode)->remove_node(entry);
        }
        else {
        next = entry->next_sibling();
        curr = &next;
        }
        }*/
    }
}

void element::remove_self(void)
{
    if (is_good()) {
        xmlDOMWrapRemoveNode(nullptr, _detail(_Mynode)->doc, _detail(_Mynode), 0);
        _Mynode = nullptr;
    }
}

_STD string element::to_string(bool formatted) const
{
    if (is_good()) {
        simple_ptr<xmlBuffer, xmlBufferFree> buf(xmlBufferCreate());
        xmlNodeDump(buf, _detail(_Mynode)->doc, _detail(_Mynode), 1, formatted ? 1 : 0);
        return _STD string((const char*)buf->content);
    }
    return "";
}

element element::add_child(const char* name, const char* value /* = nullptr */) const
{
    if (is_good()) {
        auto newnode = xmlNewNode(nullptr, BAD_CAST name);
        xmlNodeSetContent(newnode, BAD_CAST value);
        xmlAddChild(_detail(_Mynode), newnode);
        return newnode;
    }
    return nullptr;
}

/*----------------------------- friendly division line ----------------------------------------*/

struct xml4wDoc
{
    xml4wDoc(void) : doc(nullptr) {};
    ~xml4wDoc(void) { if (doc) xmlFreeDoc(doc); }
    xmlDocPtr                doc;
    _STD string              filename;
};

bool document::open(const char* filename)
{
    if (!is_open()) {
        impl_ = new(_STD nothrow) xml4wDoc();
        if (impl_ != nullptr)
        {
            xmlKeepBlanksDefault(0);
            impl_->doc = xmlReadFile(filename, "UTF-8", XML_PARSE_RECOVER | XML_PARSE_NOBLANKS | XML_PARSE_NODICT);
            // _Myroot = xmlDocGetRootElement(impl_->doc);
        }
    }
    return is_open();
}

bool document::open(const char* filename, const char* rootname)
{
    if (!is_open()) {
        impl_ = new(_STD nothrow) xml4wDoc();
        if (impl_ != nullptr)
        {
            impl_->doc = xmlNewDoc(BAD_CAST "1.0");
            auto root = xmlNewNode(NULL, BAD_CAST rootname);
            xmlDocSetRootElement(impl_->doc, root);

            impl_->filename.assign(filename);
        }
    }
    return is_open();
}

bool document::open(const char* xmlstring, int length)
{
    if (!is_open()) {
        impl_ = new(_STD nothrow) xml4wDoc();
        if (impl_ != nullptr)
        {
            xmlKeepBlanksDefault(0);
            impl_->doc = xmlReadMemory(xmlstring, length, "memory.libxml4w.xml", "UTF-8", 1);
            impl_->filename = "xml4w_memory.xml";
        }
    }
    return is_open();
}

void document::save(bool formatted) const
{
    if (is_open())
        this->save(impl_->filename.c_str(), formatted);
}

void document::save(const char* filename, bool formatted) const
{
    if (is_open()) {
        xmlSaveFormatFile(filename, impl_->doc, formatted ? 1 : 0);
    }
}

element document::root(void)
{
    if (is_open())
        return xmlDocGetRootElement(impl_->doc);
    return nullptr;
}

element document::select_element(const char*, int) const
{
    throw _STD logic_error("It's just supported by xerces-c which has 3.0.0 or more version for xml4w using xpath to operate xml!");
    return nullptr;
}

_STD string document::to_string(bool formatted) const
{
    if (is_open())
    {
        pod_ptr<xmlChar>::type buf;
        int size = 0;
        xmlDocDumpFormatMemory(impl_->doc, &buf, &size, formatted ? 1 : 0);
        return _STD string((const char*)buf.get());
    }
    return "";
}

xml4wXPathResultPtr  document::xpath_eval(const char* xpath) const
{
    if (is_open())
    {
        simple_ptr<xmlXPathContext, xmlXPathFreeContext> ctx(xmlXPathNewContext(impl_->doc));
        return xmlXPathEvalExpression(BAD_CAST xpath, ctx);
    }
    return nullptr;
}

xml4wNodePtr  document::xpath_node_of(xml4wXPathResultPtr ptr, size_t index) const
{
    auto _detail = (xmlXPathObjectPtr)ptr;
    if (index < _detail->nodesetval->nodeNr)
    {
        return _detail->nodesetval->nodeTab[index];
    }
    return nullptr;
}

void  document::xpath_free_result(xml4wXPathResultPtr result)  const
{
    if (result != nullptr)
        xmlXPathFreeObject((xmlXPathObject*)result);
}

namespace {
    struct XMLPlatformUtilsInitializer
    {
        XMLPlatformUtilsInitializer(void)
        {
            xmlInitParser();
        }
        ~XMLPlatformUtilsInitializer(void)
        {
            xmlCleanupParser();
        }
    };
    XMLPlatformUtilsInitializer __init_utils;
};

#elif defined(_USING_XERCESC)
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#    if XERCES_VERSION_MAJOR == 2
#        pragma comment(lib, "xerces-c_2D.lib")
#    elif XERCES_VERSION_MAJOR == 3
#        pragma comment(lib, "xerces-c_3_1D.lib")
#    endif
#ifdef _WIN32
#define XMLWRAP_URI_PREFIX "file:///"
#else
#define XMLWRAP_URI_PREFIX "file://"
#endif
using namespace xercesc;
inline xercesc::DOMNode* _detail(void* raw)
{
    return (xercesc::DOMNode*)raw;
}

inline xercesc::DOMNode* _detail(const element& elem)
{
    return _detail(static_cast<void*>(elem));
}

inline bool is_element(xercesc::DOMNode* ptr)
{
    return xercesc::DOMNode::ELEMENT_NODE == ptr->getNodeType();
}

_STD string _xml4w_transcode(const XMLCh* source)
{
    char* value = XMLString::transcode(source);

    if (value != nullptr)
    {
        _STD string sRet(value);
        XMLString::release(&value);
        return sRet;
    }
    return "";
}

_STD basic_string<XMLCh> _xml4w_transcode(const char* source)
{
    XMLCh* value = XMLString::transcode(source);
    if (value != nullptr)
    {
        _STD basic_string<XMLCh> sRet(value);
        XMLString::release(&value);
        return sRet;
    }
    return _STD basic_string<XMLCh>();
}

void _xml4w_serialize(xercesc::DOMNode* _TheNode, XMLFormatTarget& _Target, bool _Formated = true)
{
    static const XMLCh tempLS[] = { chLatin_L, chLatin_S, chNull };

#if XERCES_VERSION_MAJOR >= 3
    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempLS);
    DOMLSSerializer*   writer = ((DOMImplementationLS*)impl)->createLSSerializer();
    DOMLSOutput*       output = ((DOMImplementationLS*)impl)->createLSOutput();

    output->setByteStream(&_Target);

    writer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, _Formated);

    writer->write(_TheNode, output);

    output->release();
    writer->release();
#else
    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempLS);
    DOMWriter* writer = ((DOMImplementationLS*)impl)->createDOMWriter();

    writer->setFeature((XMLCh*)L"format-pretty-print", _Formated);

    writer->writeNode(&_Target, *_TheNode);

    writer->release();
#endif
}

element element::clone(void) const
{
    if (is_good())
    {
        return _detail(_Mynode)->cloneNode(true);
    }
    return nullptr;
}

element element::get_parent(void) const
{
    if (is_good())
    {
        return _detail(_Mynode)->getParentNode();
    }
    return nullptr;
}

element element::get_first_child(void) const
{
    if (is_good()) {
        auto ptr = dynamic_cast<DOMElement*>(_detail(_Mynode));
        if (ptr != nullptr)
            return ptr->getFirstElementChild();
    }
    return nullptr;
}

element element::get_prev_sibling(void) const
{
    if (is_good()) {
        auto ptr = dynamic_cast<DOMElement*>(_detail(_Mynode));
        if (ptr != nullptr)
            return ptr->getPreviousElementSibling();
    }
    return nullptr;
}

element element::get_next_sibling(void) const
{
    if (is_good()) {
        auto ptr = dynamic_cast<DOMElement*>(_detail(_Mynode));
        if (ptr != nullptr)
            return ptr->getNextElementSibling();
    }
    return nullptr;
}

_STD string element::get_name(void) const
{
    if (is_good()) {
        return _xml4w_transcode(_detail(_Mynode)->getNodeName());
    }
    return "null";
}

_STD string element::get_value(const char* default_value) const
{
    if (is_good()) {

        return _xml4w_transcode(_detail(_Mynode)->getTextContent());
    }

    _STD cerr << "xml4w::element::get_value failed, _detail(element:" << this->get_name() << "), use the default value["
        << default_value << "] insteaded!\n";

    return default_value;
}

_STD string element::get_attribute_value(const char* name, const char* default_value) const
{
    if (is_good())
    {
        auto elem = dynamic_cast<xercesc::DOMElement*>(_detail(_Mynode));
        if (elem->hasAttribute(_xml4w_transcode(name).c_str()))
        {
            return _xml4w_transcode(elem->getAttribute(_xml4w_transcode(name).c_str()));
        }
    }

    _STD cerr << "xml4w::element::get_attribute_value failed, _detail(element:" << this->get_name() << ",property:" << name << "), use the default value["
        << default_value << "] insteaded!\n";

    return default_value;
}

void element::set_value(const char* value)
{
    if (is_good())
    {
        auto theval = XMLString::transcode(value);
        if (theval != nullptr) {
            _detail(_Mynode)->setTextContent(theval);
            XMLString::release(&theval);
        }
    }
}

void element::set_attribute_value(const char* name, const char* value)
{
    if (is_good()) {

        auto n = XMLString::transcode(name);
        if (_IsNull(n))
            return;

        auto v = XMLString::transcode(value);
        if (_IsNull(v)) {
            XMLString::release(&n);
            return;
        }

        ((xercesc::DOMElement*)_Mynode)->setAttribute(n, v);

        XMLString::release(&n);
        XMLString::release(&v);
    }
}

void element::remove_children(void)
{
    if (is_good()) {
        auto current = dynamic_cast<xercesc::DOMElement*>(_detail(_Mynode));
        auto ptr = current->getFirstElementChild();
        xercesc::DOMNode* removing = nullptr;
        while (ptr != nullptr)
        {
            removing = ptr;
            ptr = ptr->getNextElementSibling();
            current->removeChild(removing);
        }
    }
}

void element::remove_children(const char* name)
{
    if (is_good()) {
        auto current = dynamic_cast<xercesc::DOMElement*>(_detail(_Mynode));
        auto ptr = current->getFirstElementChild();
        xercesc::DOMNode* removing = nullptr;
        while (ptr != nullptr)
        {
            removing = ptr;
            ptr = ptr->getNextElementSibling();
            if (_xml4w_transcode(name) == ptr->getNodeName())
                current->removeChild(removing);
        }
    }
}

void element::remove_self(void)
{
    if (is_good()) {
        _detail(_Mynode)->getParentNode()->removeChild(_detail(_Mynode));
        _Mynode = nullptr;
    }
}

_STD string element::to_string(bool formatted) const
{
    if (is_good()) {
        MemBufFormatTarget target;
        _xml4w_serialize(_detail(_Mynode), target, formatted);
        return _STD string((const char*)target.getRawBuffer());
    }
    return "";
}

element element::add_child(const char* name, const char* value /* = nullptr */) const
{
    if (is_good())
    {
        auto doc = _detail(_Mynode)->getOwnerDocument();
        if (doc) {
            auto node = doc->createElement(_xml4w_transcode(name).c_str());
            if (node) {
                node->setTextContent(_xml4w_transcode(value).c_str());
                _detail(_Mynode)->appendChild(node);
            }
            return node;
        }
    }
    return nullptr;
}


/*----------------------------- friendly division line ----------------------------------------*/

struct xml4wDoc
{
    xml4wDoc(bool bForParse = true) : doc(nullptr)
    {
        if (bForParse)
            parser = new xercesc::XercesDOMParser();
    }
    ~xml4wDoc(void)
    {
        if (parser != nullptr)
            delete parser; // doc will be released by parser
        else {
            doc->release(); // release doc self.
        }
    }
    _STD string               filename;
    xercesc::DOMDocument*     doc;
    xercesc::XercesDOMParser* parser;
};

bool document::open(const char* filename)
{
    if (!is_open())
    {
        impl_ = new(_STD nothrow) xml4wDoc();
        if (impl_)
        {
            try {
                impl_->parser->setValidationScheme(XercesDOMParser::Val_Always);
                if (impl_->parser != nullptr) {
                    impl_->parser->parse(filename);
                    impl_->doc = impl_->parser->getDocument();
                    impl_->filename = filename;
                }
            }
            catch (...)
            {
                close();
            }
        }
        else {
            close();
        }
    }
    return is_open();
}

bool document::open(const char* filename, const char* rootname)
{
    if (!is_open()) {
        impl_ = new(_STD nothrow) xml4wDoc(false);
        if (impl_ != nullptr)
        {
            impl_->doc = DOMImplementationRegistry::getDOMImplementation(nullptr)->createDocument();
            impl_->doc->createElement(_xml4w_transcode(rootname).c_str());
            impl_->doc->setDocumentURI(_xml4w_transcode(filename).c_str());
            impl_->filename = /*cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + */filename;
        }
        return true;
    }
    return is_open();
}

bool document::open(const char* xmlstring, int length)
{
    if (!is_open()) {

        impl_ = new(_STD nothrow) xml4wDoc();
        if (impl_ != nullptr)
        {
            MemBufInputSource* memBufIS = nullptr;
            impl_->parser->setValidationScheme(XercesDOMParser::Val_Auto);
            try {
                memBufIS = new MemBufInputSource(
                    (const XMLByte*)xmlstring
                    , (XMLSize_t)length
                    , (const char *const)nullptr);

                impl_->parser->parse(*memBufIS);

                impl_->doc = impl_->parser->getDocument();

                impl_->filename = "xml4w.memory.xml";
            }
            catch (...)
            {
                close();
            }

            if (memBufIS != nullptr)
                delete memBufIS;
        }
        else {
            close();
        }
    }
    return is_open();
}

void document::save(bool formatted) const
{
    if (is_open())
    {
        _STD string uri = _xml4w_transcode(impl_->doc->getDocumentURI());
        if (strstr(uri.c_str(), XMLWRAP_URI_PREFIX) != nullptr)
        {
            this->save(uri.c_str() + sizeof(XMLWRAP_URI_PREFIX) - 1);
        }
        else {
            this->save(uri.c_str());
        }
    }
}

void document::save(const char* filename, bool formatted) const
{
    if (is_open()) {
        LocalFileFormatTarget target(filename);
        _xml4w_serialize(impl_->doc, target, formatted);
    }
}

element document::root(void)
{
    if (is_open())
        return impl_->doc->getDocumentElement();
    return nullptr;
}

_STD string document::to_string(bool formatted) const
{
    if (is_open())
    {
        return static_cast<element>(impl_->doc->getDocumentElement()).to_string(formatted);
    }
    return "";
}

xml4wXPathResultPtr  document::xpath_eval(const char* xpath) const
{
    if (is_open())
    {
        return impl_->doc->evaluate(_xml4w_transcode(xpath).c_str(),
            impl_->doc->getDocumentElement(),
            nullptr,
            DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
            nullptr
        );
    }
    return nullptr;
}

xml4wNodePtr  document::xpath_node_of(xml4wXPathResultPtr ptr, size_t index) const
{
    auto _detail = (DOMXPathResult*)ptr;
    if (_detail->snapshotItem(index))
        return _detail->getNodeValue();
    return nullptr;
}

void  document::xpath_free_result(xml4wXPathResultPtr result)  const
{
    if (result != nullptr)
        ((DOMXPathResult*)result)->release();
}

namespace {
    struct XMLPlatformUtilsInitializer
    {
        XMLPlatformUtilsInitializer(void)
        {
            try {
                XMLPlatformUtils::Initialize();
            }
            catch (const XMLException& toCatch) {
                char *pMessage = XMLString::transcode(toCatch.getMessage());
                fprintf(stderr, "xml4wrapper3_6_2: Error during XMLPlatformUtils::Initialize(). \n"
                    "  Message is: %s\n", pMessage);
                XMLString::release(&pMessage);
                return;
            }
        }
        ~XMLPlatformUtilsInitializer(void)
        {
            XMLPlatformUtils::Terminate();
        }
    };
    XMLPlatformUtilsInitializer __init_utils;
};

/* end of xerces-c impl

/*************************vtd-xml wrapper impl*****************************/
#elif defined(_USING_VTDXML)
#include "vtd-xml/vtdGen.h"
#include "vtd-xml/vtdNav.h"
#include "vtd-xml/autoPilot.h"
#include "vtd-xml/bookMark.h"
#include "vtd-xml/XMLModifier.h"
#include "vtd-xml/nodeRecorder.h"
#include "vtd-xml/textIter.h"

using namespace com_ximpleware;

struct xml4wDoc : public VTDGen
{
    VTDNav* nav;
};

struct xml4wNode
{
    xml4wNode(xml4wDoc* arg1) : vg(arg1)
    {
    }
    xml4wDoc* vg;
};

static purelib::gc::object_pool<xml4wNode> s_node_pool;

element::element(xml4wNode* ptr) : _detail(_Mynode)(ptr)
{

}

_STD string element::get_attribute_value(const char* name, const char* default_value) const
{
    if (_IsNotNull(_detail(_Mynode)))
    {
        //_detail(_Mynode)->nav-
        //this->_detail(_Mynode)->
    }

    return default_value;
}

element::~element(void)
{
    if (_detail(_Mynode) != nullptr)
        s_node_pool.release(_detail(_Mynode));
}

document::document(void) : impl(nullptr)
{
}

document::~document(void)
{
    if (_IsNotNull(impl))
    {
        delete impl->getXML();
        delete impl;
    }
}

bool document::is_open(void) const
{
    return this->impl != nullptr;
}

bool document::open(const char* filename)
{
    if (!is_open())
    {
        try {
            this->impl = new xml4wDoc();
            this->impl->parseFile(false, filename);
            //auto nav = this->impl->getNav();
            //// nav->toElement(FIRST_CHILD);
            //auto depth = nav->getCurrentDepth();
            //auto index = nav->getCurrentIndex();
            //int len = nav->getRawStringLength(index);
            //auto value = nav->toRawString(index);
            //
            //
            //delete nav;
            return true;
        }
        catch (ParseException &e) {
            //vg.printLineNumber();
            printf(" error ===> %s \n", e.getMessage());
        }
        catch (...) {
            printf(" unknown error ===> \n");
        }

        if (impl)
            delete impl, impl = nullptr;

        return false;
    }

    return true;
}

element document::root(void)
{
    return new(s_node_pool.get()) xml4wNode(this->impl);
}

#endif

#ifdef _IMPL_COMM
void document::close(void)
{
    if (is_open())
    {
        delete impl_;
        impl_ = nullptr;
    }
}

/// xpath support impl
#if defined(_XML4W_XPATH_SUPPORTED)
element document::select_element(const char* xpath, int index) const
{
    auto result = this->xpath_eval(xpath);

    xml4wNodePtr ptr = this->xpath_node_of(result, index);

    this->xpath_free_result(result);

    return ptr;
}
#endif

#endif
