#pragma sw require header pub.egorpugin.primitives.tools.embedder-master
#pragma sw require header org.sw.demo.qtproject.qt.base.tools.moc-=5.15.0.0

#define QT_VERSION "-=5.15.0.0"

void build(Solution &s)
{
    auto &gui = s.addTarget<Executable>("gui");
    {
        auto &t = gui;
        t.PackageDefinitions = true;
        t += cpp20;
        t += "src/gui/.*"_rr;

        t +=
            "pub.egorpugin.primitives.filesystem"_dep,
            "pub.egorpugin.primitives.log"_dep,
            "pub.egorpugin.primitives.templates"_dep
            ;
        t += "org.sw.demo.qtproject.qt.base.widgets" QT_VERSION ""_dep;
        t += "org.sw.demo.qtproject.qt.base.winmain" QT_VERSION ""_dep;
        t += "org.sw.demo.qtproject.qt.base.plugins.platforms.windows" QT_VERSION ""_dep;
        t += "org.sw.demo.qtproject.qt.base.plugins.styles.windowsvista" QT_VERSION ""_dep;

        qt_moc_rcc_uic("org.sw.demo.qtproject.qt" QT_VERSION ""_dep, t);
        qt_tr("org.sw.demo.qtproject.qt" QT_VERSION ""_dep, t);

        if (auto L = t.getSelectedTool()->as<VisualStudioLinker*>(); L)
            L->Subsystem = vs::Subsystem::Windows;
    }
}
