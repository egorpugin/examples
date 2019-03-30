void build(Solution &s)
{
    // example_win32_directx11
    auto &t = s.addTarget<Executable>("project");
    t.CPPVersion = CPPLanguageStandard::CPP17;
    t.ApiName = "IMGUI_IMPL_API";
    t += "main.cpp";
    t += "org.sw.demo.ocornut.imgui"_dep;
    t += "User32.lib"_slib;
    t += "gdi32.lib"_slib;
    t += "d3d11.lib"_slib;
    t += "../imgui_impl_win32.cpp";
    t += "../imgui_impl_dx11.cpp";
}

