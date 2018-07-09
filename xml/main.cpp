/*
local_settings:
    use_shared_libs: false
    build:
        generator: Visual Studio 15 2017 Win64
c++: 17
dependencies:
    - pvt.cppan.demo.zeux.pugixml: 1
    #- pvt.egorpugin.primitives.filesystem: master
    #- pvt.cppan.demo.libxml2: 2
    #- pvt.cppan.demo.expat: 2
    #- pvt.cppan.demo.apache.xerces: 3
*/

#include <pugixml.hpp>

#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(argv[1]);

    std::cout << "Load result: " << result.description() << std::endl;

    return 0;
}
