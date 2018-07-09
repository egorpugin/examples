/*
local_settings:
    use_shared_libs: false
    build:
        generator: Visual Studio 15 2017 Win64
c++: 17
dependencies:
    #- pvt.cppan.demo.libxml2: 2
    #-	pvt.cppan.demo.zeux.pugixml: 1
    - pvt.cppan.demo.expat: 2
    #- pvt.cppan.demo.apache.xerces: 3
    - pvt.egorpugin.primitives.filesystem: master
*/

#include <primitives/filesystem.h>

#include <expat.h>

#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;

    auto f = read_file(argv[1], true);

    auto p = XML_ParserCreate(0);
    const auto sz = f.size();
    int n = sz / INT_MAX;
    while (sz / n >= INT_MAX / 2)
        n++;
    for (int i = 0; i <= n; i++)
    {
        auto b = sz / n * i;
        auto len = std::max((size_t)0, std::min(sz - b, sz / n));
        std::cout << "parsing " << sz << " bytes: [step " << i << "] [" << b << "; " << b + len << "]: ret = ";
        auto ret = XML_Parse(p, f.data() + b, len, i == n);
        std::cout << ret << "\n";
        if (ret != XML_STATUS_OK)
        {
            //std::cerr << ret;
        }
    }

    return 0;
}
