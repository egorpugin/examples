/*
local_settings:
    use_shared_libs: false
    build:
        generator: Visual Studio 15 2017 Win64
c++: 17
dependencies:
    #- pvt.cppan.demo.libxml2: 2
    #-	pvt.cppan.demo.zeux.pugixml: 1
    #- pvt.cppan.demo.expat: 2
    - pvt.cppan.demo.apache.xerces: 3.1
    - pvt.egorpugin.primitives.filesystem: master
*/

#include <primitives/filesystem.h>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <iostream>

int main(int argc, char **argv)
{
    using namespace xercesc;

    if (argc != 2)
        return (1);

    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        // Do your failure processing here
        return 1;
    }

    // Do your actual work with Xerces-C++ here.


    //auto f = read_file(argv[1], true);

    XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true);    // optional

    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);

    try {
        parser->parse(argv[1]);
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message is: \n"
            << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        std::cout << "Exception message is: \n"
            << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (...) {
        std::cout << "Unexpected Exception \n";
        return -1;
    }

    delete parser;
    delete errHandler;

    XMLPlatformUtils::Terminate();

    return 0;
}
