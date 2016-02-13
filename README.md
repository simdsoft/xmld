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