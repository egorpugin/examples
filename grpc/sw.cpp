#pragma sw require header org.sw.demo.google.grpc.cpp.plugin
#pragma sw require pub.egorpugin.primitives.filesystem

void build(Solution &s)
{
    auto &p = s.addStaticLibrary("proto");
    p += cpp20;
    ProtobufData d;
    d.public_protobuf = true;
    gen_grpc_cpp("org.sw.demo.google.protobuf"_dep, "org.sw.demo.google.grpc.cpp.plugin"_dep,
        p, p.SourceDir / "helloworld.proto", d);
    p.Public += "org.sw.demo.google.grpc.cpp"_dep;
    p.Public += "pub.egorpugin.primitives.filesystem"_dep;

    auto &srv = s.addExecutable("server");
    srv += cpp20;
    srv += "greeter_server.cc";
    srv += p;

    auto &c = s.addExecutable("client");
    c += cpp20;
    c += "greeter_client.cc";
    c += p;
}
