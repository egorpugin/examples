#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#include <primitives/filesystem.h>

class FindNamedClassVisitor
    : public clang::RecursiveASTVisitor<FindNamedClassVisitor> {
public:
    explicit FindNamedClassVisitor(clang::ASTContext *Context)
        : Context(Context) {}

    bool VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration) {
        Declaration->dump();

        if (Declaration->getQualifiedNameAsString() == "n::m::C") {
            clang::FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
            if (FullLocation.isValid())
                llvm::outs() << "Found declaration at "
                << FullLocation.getSpellingLineNumber() << ":"
                << FullLocation.getSpellingColumnNumber() << "\n";
        }

        auto attrs = Context->getDeclAttrs(Declaration);

        auto& ctx = Declaration->getASTContext();
        auto& sm = ctx.getSourceManager();

        auto rc = Declaration->getASTContext().getRawCommentForDeclNoCache(Declaration);
        if (rc)
        {
            //Found comment!
            auto range = rc->getSourceRange();

            auto startPos = sm.getPresumedLoc(range.getBegin());
            auto endPos = sm.getPresumedLoc(range.getEnd());

            auto raw = rc->getRawText(sm);
            auto brief = rc->getBriefText(ctx);

            // ... Do something with positions or comments
        }

        return true;
    }

    bool VisitAttribute(clang::Attr *a) {
        return true;
    }

private:
    clang::ASTContext *Context;
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
    explicit FindNamedClassConsumer(clang::ASTContext *Context)
        : Visitor(Context) {}

    void HandleTranslationUnit(clang::ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    FindNamedClassVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(
            new FindNamedClassConsumer(&Compiler.getASTContext()));
    }
};

int main(int argc, char **argv) {
    auto f = read_file("header.hpp");
    clang::tooling::runToolOnCode(std::make_unique<FindNamedClassAction>(), f.c_str());
    return 0;
}
