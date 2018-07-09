#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractItemModel>
#include <QMainWindow>
#include <QTranslator>

class QWidget;
class QMenuBar;
class QMenu;
class QActionGroup;
class QAction;
class QPushButton;
class QPlainTextEdit;
class QHBoxLayout;
class QVBoxLayout;
class QComboBox;
class QGroupBox;
class QLabel;
class QScrollArea;
class QTreeView;

struct TreeItem;
class AnswersWidget;
class QuestionsItemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void changeEvent(QEvent* event);
    void closeEvent(QCloseEvent *event);
signals:
    void enableEditing(bool);
private slots:
    void selectQuestion(const QModelIndex &index);
    void newFile();
    void clear();
    void saveData();
    void changeLanguage(QAction *action);
    void checkQuestions();
private:
    void setupUi();
    void retranslateUi();

    bool openFile(QString filename);
    void saveFile(QString filename);
    void exportToGift(QString filename);

private: /* data */
    QTranslator iexamTranslator;
    QuestionsItemModel *questionsModel;
    QModelIndex currentIndex;
    TreeItem *currentItem;
    QString qbState;
private: /* ui functions */
    void createActions();
    void createLanguageMenu();
    void createMenus();
    void createButtons();
    void createStatusBar();
    void createTestElements();
    void createToolBar();
    void createLayouts();
private: /* ui components */
    QWidget *centralWidget;

    QMenuBar *mainMenu;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *settingsMenu;
    QMenu *languageMenu;
    QMenu *helpMenu;

    QActionGroup *languageActionGroup;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *exportToGiftAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *addSectionAction;
    QAction *removeSectionAction;
    QAction *addQuestionAction;
    QAction *removeQuestionAction;
    QAction *addAnswerAction;
    QAction *removeAnswerAction;

    QHBoxLayout *mainLayout;

    QVBoxLayout *answersLayout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;
    QVBoxLayout *topLayout;
    QVBoxLayout *midLayout;
    QVBoxLayout *botLayout;

    /* different classes */
    QPlainTextEdit *questionEdit;
    QPlainTextEdit *solutionEdit;

    QLabel *questionLabel;
    QLabel *solutionLabel;
    QLabel *treeViewLabel;
    QLabel *questionTypeLabel;
    QLabel *sectionChangeLabel;
    QLabel *questionChangeLabel;
    QLabel *answerChangeLabel;
    QLabel *qbStateLabelLabel;
    QLabel *qbStateLabel;

    QTreeView *treeView;
    QComboBox *questionType;
    QGroupBox *answersGroupBox;
    AnswersWidget *answersWidget;
    QScrollArea *scrollArea;

    bool opened = false;
};

#endif // MAINWINDOW_H
