#include "../xmldrv3/xmldrv.h"
#include "../xmldrv3/vtd-xml/everything.h"
#include "../xmldrv3/rapidxml/rapidxml_sax3.hpp"
#include "tinyxml2.h"
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#if defined(_DEBUG)
#pragma comment(lib, "../Debug/xmldrv3.lib")
#else
#pragma comment(lib, "../Release/xmldrv3.lib")
#endif
// using namespace com_ximpleware;
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

    std::string read_file_data(const char* filename)
    {
        FILE* fp = fopen(filename, "rb");
        if (fp == nullptr)
            return (const char*)"";

        size_t size = get_file_size(fp);
        if (size == 0)
            return "";

        std::string storage(size, '\0');

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

    bool  write_file_data(const char* filename, const std::string& content)
    {
        return write_file_data(filename, content.c_str(), content.size());
    }
};

class xml_sax2_handler_impl : public rapidxml::xml_sax2_handler
{
    /**
    * @js NA
    * @lua NA
    */
    virtual void xmlSAX2StartElement(const char *name, size_t len, const char **atts, size_t attslen)
    {
        //std::cout << "xmlSAX2StartElement -->";
        //std::cout.write(name, len);
        //std::cout << "\n\tattribs:\n";
        //for (auto i = 0; i < attslen; i += 2)
        //{
        //    std::cout << "\t\t" << atts[i] << ":" << atts[i + 1] << "\n";
        //}
    }

    /**
    * @js NA
    * @lua NA
    */
    virtual void xmlSAX2EndElement(const char *name, size_t len)
    {
        //std::cout << "xmlSAX2EndElement -->";
        //std::cout.write(name, len);
        //std::cout << "\n\n";
    }
    /**
    * @js NA
    * @lua NA
    */
    virtual void xmlSAX2Text(const char *s, size_t len)
    {
        //std::cout << "xmlSAX2Text -->";
        //std::cout.write(s, len);
        //std::cout << "\n\n";
    }
};

#define _SAX3_TEST_LOG 0

class xml_sax3_handler_impl : public rapidxml::xml_sax3_handler
{
public:
#if _SAX3_TEST_LOG
	std::ofstream fout;
	int deep = 0;
#endif
public:
	xml_sax3_handler_impl()
	{
#if _SAX3_TEST_LOG
		fout.open("D:\\xml_sax3_handler_impl_rapidxml.log");
#endif
	}
	virtual ~xml_sax3_handler_impl() {}

	virtual void xmlSAX3StartElement(char *name, size_t size)
	{
#if _SAX3_TEST_LOG
		fout << "xmlSAX3StartElement-->";
		fout.write(name, size);
		fout << "\n";
#endif
	};

	virtual void xmlSAX3Attr(const char* name, size_t nl,
		const char* value, size_t vl)
	{
#if _SAX3_TEST_LOG
		fout << "xmlSAX3Attr-->";
		fout.write(name, nl);
		fout << "=";
		fout.write(value, vl);
		fout << "\n";
#endif
	};

	virtual void xmlSAX3EndAttr()
	{
	};

	virtual void xmlSAX3EndElement(const char *name, size_t n)
	{
#if _SAX3_TEST_LOG
		fout << "xmlSAX3EndElement==>";
		fout.write(name, n);
		fout << "\n";
#endif
	};

	virtual void xmlSAX3Text(const char *text, size_t len) {
#if _SAX3_TEST_LOG
		fout << "xmlSAX3Text-->\n";
		fout.write(text, len);
		fout << "\n";
#endif
	};
};


class xml_sax3_handler_impl_pugi : public pugi::xml_sax3_handler
{
public:
#if _SAX3_TEST_LOG
	std::ofstream fout;
	int deep = 0;
#endif
public:
	xml_sax3_handler_impl_pugi()
	{
#if _SAX3_TEST_LOG
		fout.open("D:\\xml_sax3_handler_impl_rapidxml.log");
#endif
	}
	virtual ~xml_sax3_handler_impl_pugi() {}

	virtual void xmlSAX3StartElement(char *name, size_t size)
	{
#if _SAX3_TEST_LOG
		fout << "xmlSAX3StartElement-->";
		fout.write(name, size);
		fout << "\n";
#endif
	};

	virtual void xmlSAX3Attr(const char* name, size_t nl,
		const char* value, size_t vl)
	{
#if _SAX3_TEST_LOG
		fout << "xmlSAX3Attr-->";
		fout.write(name, nl);
		fout << "=";
		fout.write(value, vl);
		fout << "\n";
#endif
	};

	virtual void xmlSAX3EndAttr()
	{
	};

	virtual void xmlSAX3EndElement(const char *name, size_t n)
	{
#if _SAX3_TEST_LOG
		fout << "xmlSAX3EndElement==>";
		fout.write(name, n);
		fout << "\n";
#endif
	};

	virtual void xmlSAX3Text(const char *text, size_t len) {
#if _SAX3_TEST_LOG
		fout << "xmlSAX3Text-->\n";
		fout.write(text, len);
		fout << "\n";
#endif
	};
};

void main()
{
#if 0
    xmldrv::document doc("test.xml", "#memory"); // mode: "#disk", "#buffer", "#memory";

    auto people = doc.create_element("people");

    people.set_attribute_value("name", "guoxiaodong");
    people.set_attribute_value("sex", "male");
    people.set_attribute_value("age", "27");
    people.set_attribute_value("age", 33);

    int value = people.get_attribute_value("age", (unsigned int)33);

    auto vv = people.get_attribute_value("age", "");

    people.add_child("address", "Guandong Shenzhen Bao'an Xi'xiang Streets.");

    auto number = people.add_child("number");
    number.set_value("value");
    number.set_value(54000);

    doc.root().add_child(people);

    doc.save("est.xml", true);

    doc.close();
#endif
	for (int i = 0; i < 5; ++i) {

		xmld::document d;

		std::string data = read_file_data("address.xml");
		printf("test file size:%lfMB\n\n", data.size() / (double)SZ(1, M));

		auto start = clock();
		strlen(data.c_str());
		printf("strlen: %lf seconds used!\n", (clock() - start) / (double)CLOCKS_PER_SEC);

		data = read_file_data("address.xml");
		start = clock();
		xml_sax3_handler_impl handler;
		rapidxml::xml_sax3_parser<> parser(&handler);
		parser.parse<>(&data.front(), data.size());
		printf("rapidxml SAX parse: %lf seconds used!\n", (clock() - start) / (double)CLOCKS_PER_SEC);

		data = read_file_data("address.xml");
		start = clock();
		xml_sax3_handler_impl_pugi handler2;
		pugi::xml_document::perform_sax3_parse(&handler2, &data.front(), data.size());
		// parser.parse<>(&data.front(), data.size());
		printf("pugixml SAX parse: %lf seconds used!\n", (clock() - start) / (double)CLOCKS_PER_SEC);

		data = read_file_data("address.xml");
		start = clock();
		d.openb(std::move(data));
		printf("rapidxml: %lf seconds used!\n", (clock() - start) / (double)CLOCKS_PER_SEC);

		/// test pugixml
		/// tinyxml2 performance test
		data = read_file_data("address.xml");
		start = clock();
		pugi::xml_document pugiDoc;
		pugiDoc.load_buffer_inplace(&data.front(), data.length(), pugi::parse_minimal);
		printf("pugixml: %lf seconds used!\n", (clock() - start) / (double)CLOCKS_PER_SEC);

		/// tinyxml2 performance test
		data = read_file_data("address.xml");

		start = clock();
		tinyxml2::XMLDocument tinyDoc;
		tinyDoc.Parse(data.c_str(), data.length());
		printf("tinyxml2: %lf seconds used!\n", (clock() - start) / (double)CLOCKS_PER_SEC);

		/// VTD xml performacne test
		data = read_file_data("address.xml");

		start = clock();
		com_ximpleware::VTDNav *vn = NULL;
		com_ximpleware::VTDGen vg;
		/*if (vg.parseFile(true, "")) {
			vn = vg.getNav();
			if (vn->toElementNS(FIRST_CHILD, L"someURL", L"b")) {
				int i = vn->getText();
				if (i != -1) {
					UCSChar *string = vn->toString(i);
					wprintf(L"the text node value is %d ==> %s \n", i, string);
					delete(string);
				}
			}
			delete vn->getXML();
		}
		else
			delete (vg.getXML());*/
		vg.setDoc(&data.front(), data.size());
		//try {
		vg.parse(false);
		//}
		//catch (com_ximpleware::ParseException& e) {
		//	vg.clear();
		//	printf("%s\n", e.what());
			//printf("%s\n",e.sub_msg);
			//throwException2(out_of_mem,"error occurred in parseFile");
		//}

		printf("vtd-xml: %lf seconds used!\n", (clock() - start) / (double)CLOCKS_PER_SEC);
	}

    system("pause");
}
