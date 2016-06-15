#include "../xmldrv3/xmldrv.h"
#pragma comment(lib, "../Debug/xmldrv3.lib")

void main()
{
    unmanaged_string str("23");

    str.assign("fdsfsdjfkfkdsf");

    xmldrv::document doc("test.xml", "#memory"); // mode: "#disk", "#buffer", "#memory";

    auto people = doc.create_element("people");

    people.set_attribute_value("name", "guoxiaodong");
    people.set_attribute_value("sex", "male");
    people.set_attribute_value("age", "27");
    people.set_attribute_value("age", 33);

    people.add_child("address", "Guandong Shenzhen Bao'an Xi'xiang Streets.");

    auto number = people.add_child("number");
    number.set_value("value");
    number.set_value(54000);
    char ch = 0xff;
    number.set_value(ch);

    doc.root().add_child(people);
    
    doc.save("est.xml", true);
    
    doc.close();
}

