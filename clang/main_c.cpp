#include <clang-c/Index.h>

#include <iostream>
#include <vector>

std::ostream& operator<<(std::ostream& stream, const CXString& str) {
    stream << clang_getCString(str);
    clang_disposeString(str);
    return stream;
}

int main() {
    CXIndex index = clang_createIndex(0, 0);
    std::vector<const char *> args{ "-std=c++20", "-v" };
    CXTranslationUnit unit = clang_parseTranslationUnit(index, "header.hpp", args.data(), args.size(), nullptr, 0, CXTranslationUnit_None);
    if (unit == nullptr)
    {
        std::cerr << "Unable to parse translation unit. Quitting." << std::endl;
        return 1;
    }

    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(
        cursor,
        [](CXCursor c, CXCursor parent, CXClientData client_data) {
            std::cout << "Cursor '" << clang_getCursorSpelling(c) << "' of kind '"
            << clang_getCursorKindSpelling(clang_getCursorKind(c)) << "'\n";
            return CXChildVisit_Recurse;
        },
        nullptr);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return 0;
}
