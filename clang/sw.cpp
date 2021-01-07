void build(Solution &s)
{
    auto add = [&s](String &&n)
    {
        auto &t = s.addExecutable(n);
        t += cpp20;
        t += path(n += ".cpp");
        t += "org.sw.demo.llvm_project.clang.tools.libclang"_dep;
        t += "pub.egorpugin.primitives.filesystem"_dep;
        //t += "pub.egorpugin.primitives.sw.main"_dep;

        t.CompileOptions.push_back("-wd4624");
        t.CompileOptions.push_back("-wd4291");
    };

    add("main_c");
    add("main_cpp");
}
