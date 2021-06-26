#include "mainwindow.h"

#include <primitives/exceptions.h>
#include <QtWidgets>

#include <algorithm>
#include <fstream>
#include <ranges>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent) {
    // core
    translator = new QTranslator;
    setup_ui();
    retranslate_ui();

    //
    setWindowTitle("gui");
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

void main_window::setup_ui() {
    create_actions();
    create_menus();

    auto mainLayout = new QHBoxLayout;

    auto centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void main_window::create_language_menu() {
    language_menu = new QMenu(this);
    QAction *defaultApplicationLanguage = language_menu->addAction("English");
    defaultApplicationLanguage->setCheckable(true);
    defaultApplicationLanguage->setData(QString());
    defaultApplicationLanguage->setChecked(true);
    auto languageActionGroup = new QActionGroup(this);
    languageActionGroup->addAction(defaultApplicationLanguage);
    connect(languageActionGroup, &QActionGroup::triggered, this, &main_window::change_language);

    QSettings s;
    auto lf = s.value("language.file").toString();

    bool defaultFound = false;
    QString tsDirName = ":/ts/";
    QDir dir(tsDirName);
    QStringList filenames = dir.entryList();
    for (int i = 0; i < filenames.size(); ++i) {
        QString filename = tsDirName + filenames[i];

        QTranslator tr;
        tr.load(filename);

        QString language = tr.translate("language", "English");
        if (language == "English")
            continue;
        QString defaultLanguage = tr.translate("language", "Default application language",
            "Set this variable to \"1\" to default choose current language");

        QAction *action = new QAction(language, this);
        action->setCheckable(true);
        action->setData(filename);
        language_menu->addAction(action);
        languageActionGroup->addAction(action);

        if (!defaultFound && defaultLanguage != QString{} || lf == filename) {
            action->setChecked(true);
            defaultFound = true;

            QApplication::installTranslator(translator);
            translator->load(action->data().toString());
            s.setValue("language.file", action->data().toString());
        }
    }
}

void main_window::change_language(QAction *action) {
    QApplication::removeTranslator(translator);
    if (action->data() != QString{}) {
        QApplication::installTranslator(translator);
        translator->load(action->data().toString());
    }
    QSettings s;
    s.setValue("language.file", action->data().toString());
}

void main_window::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange)
        retranslate_ui();
    QMainWindow::changeEvent(event);
}

void main_window::retranslate_ui() {
    // core
    about_action->setText(tr("About"));
    help_menu->setTitle(tr("Help"));

    exit_action->setText(tr("Exit"));
    file_menu->setTitle(tr("File"));

    language_menu->setTitle(tr("Language"));
    settings_menu->setTitle(tr("Settings"));

    //
}

void main_window::create_menus() {
    create_language_menu();

    file_menu = new QMenu(this);
    file_menu->addSeparator();
    file_menu->addAction(exit_action);

    settings_menu = new QMenu(this);
    settings_menu->addMenu(language_menu);

    help_menu = new QMenu(this);
    help_menu->addAction(about_action);

    mb = new QMenuBar(this);
    mb->addMenu(file_menu);
    mb->addMenu(settings_menu);
    mb->addMenu(help_menu);

    setMenuBar(mb);
}

void main_window::create_actions() {
    // core
    exit_action = new QAction(this);
    connect(exit_action, &QAction::triggered, [this]() {
        close();
    });

    about_action = new QAction(this);
    connect(about_action, &QAction::triggered, [this] {
        QMessageBox::information(this, 0, tr("Version") + QString(": %1").arg(PACKAGE_VERSION));
    });

    //
}
