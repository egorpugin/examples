#pragma sw require header org.sw.demo.google.grpc.grpc_cpp_plugin
#pragma sw require pub.egorpugin.primitives.filesystem-master

void build(Solution &s)
{
    auto &p = s.addStaticLibrary("proto");
    gen_grpc("org.sw.demo.google.protobuf"_dep, "org.sw.demo.google.grpc.grpc_cpp_plugin"_dep,
        p, p.SourceDir / "helloworld.proto", true);
    p.Public += "org.sw.demo.google.grpc.grpcpp"_dep;
    p.Public += "pub.egorpugin.primitives.filesystem-master"_dep;

    auto &srv = s.addExecutable("server");
    srv.CPPVersion = CPPLanguageStandard::CPP17;
    srv += "greeter_server.cc";
    srv += p;

    auto &c = s.addExecutable("client");
    c.CPPVersion = CPPLanguageStandard::CPP17;
    c += "greeter_client.cc";
    c += p;
}
