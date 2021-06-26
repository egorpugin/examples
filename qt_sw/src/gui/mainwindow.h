#pragma once

#include <primitives/filesystem.h>
#include <qmainwindow.h>

class QLabel;
class QTextEdit;
class QListWidget;
class QStatusBar;
class QTranslator;
class QLineEdit;
class QMenu;
class QAction;
class QPushButton;
class QGroupBox;
class QMenuBar;

struct main_window : public QMainWindow {
    Q_OBJECT
public:
    main_window(QWidget *parent = 0);

    // core
private:
    QTranslator *translator;
    QStatusBar *sb;
    QMenuBar *mb;
    QMenu *language_menu;
    QMenu *help_menu;
    QMenu *file_menu;
    QMenu *settings_menu;
    QAction *exit_action;
    QAction *about_action;

    void setup_ui();
    void create_menus();
    void create_language_menu();
    void create_actions();
    void retranslate_ui();
    void change_language(QAction *);
    void changeEvent(QEvent *) override;

    // custom
private:
};
