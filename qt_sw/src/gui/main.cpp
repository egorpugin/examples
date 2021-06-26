#define SW_EXECUTABLE

#include "mainwindow.h"

#include <qapplication.h>
#include <qglobal.h>
#include <qmessagebox.h>
#include <qthread.h>

#include <time.h>

#ifdef WIN32
#include <Windows.h>
#endif

#include <primitives/log.h>
DECLARE_STATIC_LOGGER(logger, "labview.address.gui");

static std::string get_time_string() {
    time_t now;
    time(&now);
    char buf[50];
    strftime(buf, sizeof buf, "%Y_%m_%d_%H_%M_%S", gmtime(&now));
    return buf;
}

static auto get_time_string_constant() {
    static auto s = get_time_string();
    return s;
}

void setup_logger(const std::string &log_level, bool simple) {
    LoggerSettings log_settings;
    log_settings.log_level = log_level;
    log_settings.log_file = "log/"s + PACKAGE_NAME_CLEAN + "_" + get_time_string_constant();
    log_settings.simple_logger = simple;
    log_settings.print_trace = true;
    initLogger(log_settings);

    // first trace message
    LOG_TRACE(logger, "----------------------------------------");
    LOG_TRACE(logger, "Starting " PACKAGE_NAME_CLEAN "...");
}

void win32_hacks();

int main(int argc, char *argv[]) {
    win32_hacks();
    qsrand(time(0));

    setup_logger("INFO", false);
    LOG_INFO(logger, "starting");

    QThread t(0);
    QApplication a(argc, argv);
    a.setApplicationName(PACKAGE_NAME);
    a.setOrganizationName("pvt.egorpugin");
    a.setOrganizationDomain("egorpugin.me");
    try {
        main_window w;
        w.show();
        return a.exec();
    } catch (std::exception &e) {
        QMessageBox::critical(0, "Error", e.what());
    } catch (...) {
        QMessageBox::critical(0, "Error", "unknown error");
    }
}

void win32_hacks() {
#ifdef WIN32
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif
}
