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
#ifndef _XML4WRAPPER_INL_
#define _XML4WRAPPER_INL_

#if defined(_XML4WRAPPER_H_)
namespace xmldrv {

template<typename _Ty>
_Ty element::get_value(const _Ty& default_value) const
{
    return nsc::to_numeric<_Ty>(this->get_value(nsc::to_string(default_value)));
}

template<typename _Ty>
_Ty element::get_attribute_value(const char* name, const _Ty& default_value) const
{
    return nsc::to_numeric<_Ty>(this->get_attribute_value(name, nsc::to_string(default_value)));
}

template<typename _Ty>
void element::set_value(const _Ty& value)
{
    this->set_value(nsc::to_string(value));
}

template<typename _Ty>
void element::set_attribute_value(const char* name, const _Ty& value)
{
    if(_Mynode != nullptr) {
        this->set_attribute_value(name, nsc::to_string(value).c_str());
    }
}

template<typename _Handler>
void element::cforeach(_Handler handler) const
{
    auto ptr = *this;
    __xml4wts_algo(ptr, 
        ptr.get_child(), 
        ptr.get_next_sibling(),
        handler(ptr)
        );
}

template<typename _Operation>
void element::cforeach_breakif(_Operation handler) const
{           
    auto ptr = *this;
    __xml4wts_algo_cond(ptr,
        ptr.get_child(),
        ptr.get_next_sibling(),
        handler(ptr),
        break
        );
}

template<typename _Handler>
void element::cforeach(const char* name, _Handler handler) const
{
    auto ptr = *this;
    __xml4wts_algo_cond(ptr, 
        ptr.get_child(),
        ptr.get_next_sibling(),
        name == ((element)ptr).get_name(),
        handler(ptr)
        );
}

template<typename _Handler>
void document::xforeach(const char* xpath, _Handler handler) const
{
    auto result = this->xpath_eval(xpath);

    xml4wNodePtr ptr;
    size_t index = 0;
    while ((ptr = this->xpath_node_of(result, index++)) != nullptr)
        handler(ptr);
    
    this->xpath_free_result(result);
}

};

#endif

#endif /* _XML4WRAPPER_INL_ */
/*
* Copyright (c) 2012-2014 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/
