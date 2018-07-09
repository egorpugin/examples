#include "mainwindow.h"

#include <QtWidgets>

#include <fstream>
#include <iomanip>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    clear();

    if (!openFile(QDir::tempPath() + QDir::separator() + "iexam.json"))
        newFile();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    createActions();
    createLanguageMenu();
    createMenus();
    createButtons();
    createTestElements();
    createStatusBar();
    createToolBar();
    createLayouts();

    //retranslateUi();

    resize(minimumSizeHint());
}

void MainWindow::createActions()
{
    newAction = new QAction(this);
    connect(newAction, SIGNAL(triggered()), SLOT(clear()));
    connect(newAction, SIGNAL(triggered()), SLOT(newFile()));

    openAction = new QAction(this);
    connect(openAction, &QAction::triggered, [=]{
        openFile(QFileDialog::getOpenFileName(this, tr("Open file"), QString(), tr("File with questions") + " (*.json)"));
    });

    saveAction = new QAction(this);
    connect(saveAction, &QAction::triggered, [this]{
        saveFile(QFileDialog::getSaveFileName(this, tr("Save file"), QString(), tr("File with questions") + " (*.json)"));
    });

    exportToGiftAction = new QAction(this);
    connect(exportToGiftAction, &QAction::triggered, [this] {
        exportToGift(QFileDialog::getSaveFileName(this, tr("Export to GIFT"), QString(), tr("GIFT file") + " (*.txt)"));
    });

    exitAction = new QAction(this);
    connect(exitAction, &QAction::triggered, [=]
    {
        saveFile(QDir::tempPath() + QDir::separator() + "iexam.json");
        close();
    });

    aboutAction = new QAction(this);
    connect(aboutAction, &QAction::triggered, [=]
    {
    });

    addSectionAction = new QAction(QIcon(":/icons/plus.png"), 0, this);
    connect(addSectionAction, SIGNAL(triggered()), SLOT(saveData()));
    connect(addSectionAction, &QAction::triggered, [=]
    {
    });

    removeSectionAction = new QAction(QIcon(":/icons/minus.png"), 0, this);
    connect(removeSectionAction, SIGNAL(triggered()), SLOT(saveData()));
    connect(removeSectionAction, &QAction::triggered, [=]
    {
        if (QMessageBox::question(this, tr("Edit questions"), tr("Are you sure to remove current section?")) != QMessageBox::Yes)
            return;

        QModelIndex i = treeView->currentIndex();
        if (!i.isValid())
            return;
        if (i.parent().isValid())
            i = i.parent();
        QModelIndex a = treeView->indexAbove(i);
        QModelIndex b = treeView->indexBelow(i);
        while (b.parent().isValid())
            b = treeView->indexBelow(b);
        if (b.isValid())
        {
            if (b.parent().isValid())
                b = treeView->indexBelow(b.parent());
            else
                b = treeView->indexBelow(b);
            treeView->clicked(b);
            treeView->setCurrentIndex(b);
        }
        else if (a.isValid())
        {
            if (a.parent().isValid())
                a = treeView->indexBelow(a.parent());
            else
                a = treeView->indexBelow(a);
            treeView->clicked(a);
            treeView->setCurrentIndex(a);
        }

        i = treeView->currentIndex();
        if (!i.isValid())
            treeView->clicked(i);
    });

    addQuestionAction = new QAction(QIcon(":/icons/plus.png"), 0, this);
    connect(addQuestionAction, SIGNAL(triggered()), SLOT(saveData()));
    connect(addQuestionAction, &QAction::triggered, [=]
    {
    });

    removeQuestionAction = new QAction(QIcon(":/icons/minus.png"), 0, this);
    removeQuestionAction->setEnabled(false);
    connect(this, SIGNAL(enableEditing(bool)), removeQuestionAction, SLOT(setEnabled(bool)));
    connect(removeQuestionAction, SIGNAL(triggered()), SLOT(saveData()));
    connect(removeQuestionAction, &QAction::triggered, [=]
    {
    });

}

void MainWindow::createLanguageMenu()
{
    languageMenu = new QMenu(this);
    QAction *defaultApplicationLanguage = languageMenu->addAction("English");
    defaultApplicationLanguage->setCheckable(true);
    defaultApplicationLanguage->setData(QString());
    defaultApplicationLanguage->setChecked(true);
    languageActionGroup = new QActionGroup(this);
    languageActionGroup->addAction(defaultApplicationLanguage);
    connect(languageActionGroup, SIGNAL(triggered(QAction *)), SLOT(changeLanguage(QAction *)));

    bool defaultFound = false;
    QString tsDirName = ":/ts/";
    QDir dir(tsDirName);
    QStringList filenames = dir.entryList();
    for (int i = 0; i < filenames.size(); ++i)
    {
        QString filename = tsDirName + filenames[i];

        QTranslator translator;
        translator.load(filename);

        QString language = translator.translate("MainWindow", "English");
        if (language == "English")
            continue;
        QString defaultLanguage = translator.translate("MainWindow", "Default application language",
            "Set this variable to \"1\" to default choose current language");

        QAction *action = new QAction(language, this);
        action->setCheckable(true);
        action->setData(filename);
        languageMenu->addAction(action);
        languageActionGroup->addAction(action);

        if (!defaultFound && defaultLanguage != QString())
        {
            action->setChecked(true);
            defaultFound = true;

            QApplication::installTranslator(&iexamTranslator);
            iexamTranslator.load(action->data().toString());
        }
    }
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(this);
    fileMenu->addAction(newAction);
    fileMenu->addSeparator();
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exportToGiftAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = new QMenu(this);

    settingsMenu = new QMenu(this);
    settingsMenu->addMenu(languageMenu);

    helpMenu = new QMenu(this);
    helpMenu->addAction(aboutAction);

    mainMenu = new QMenuBar(this);
    mainMenu->addMenu(fileMenu);
    //mainMenu->addMenu(editMenu);
    mainMenu->addMenu(settingsMenu);
    mainMenu->addMenu(helpMenu);

    setMenuBar(mainMenu);
}

void MainWindow::createButtons()
{
}

void MainWindow::createStatusBar()
{
    QStatusBar *statusBar = new QStatusBar;
    statusBar->addWidget(qbStateLabelLabel);
    qbStateLabel->setAlignment(Qt::AlignRight);
    statusBar->addWidget(qbStateLabel, 1);
    setStatusBar(statusBar);
}

void MainWindow::createToolBar()
{
    QToolBar *toolBar = new QToolBar(this);
    //toolBar->addAction(newAction);
    //toolBar->addSeparator();
    toolBar->addWidget(sectionChangeLabel);
    toolBar->addAction(addSectionAction);
    toolBar->addAction(removeSectionAction);
    toolBar->addSeparator();
    toolBar->addWidget(questionChangeLabel);
    toolBar->addAction(addQuestionAction);
    toolBar->addAction(removeQuestionAction);
    toolBar->addSeparator();
    toolBar->addWidget(answerChangeLabel);
    //toolBar->addAction(addAnswerAction);
    //toolBar->addAction(removeAnswerAction);
    toolBar->addSeparator();
    //toolBar->addWidget(questionTypeLabel);
    //toolBar->addWidget(questionType);
    toolBar->addSeparator();
    //toolBar->addWidget(new QPushButton("Проверить"));
    addToolBar(toolBar);
    //addToolBarBreak();
}

void MainWindow::createTestElements()
{
    questionEdit = new QPlainTextEdit(this);
    questionEdit->setMinimumHeight(70);
    connect(this, SIGNAL(enableEditing(bool)), questionEdit, SLOT(setEnabled(bool)));

    solutionEdit = new QPlainTextEdit(this);
    solutionEdit->setMinimumHeight(50);
    connect(this, SIGNAL(enableEditing(bool)), solutionEdit, SLOT(setEnabled(bool)));

    treeViewLabel = new QLabel(this);
    questionLabel = new QLabel(this);
    solutionLabel = new QLabel(this);
    questionTypeLabel = new QLabel(this);
    sectionChangeLabel = new QLabel(this);
    questionChangeLabel = new QLabel(this);
    answerChangeLabel = new QLabel(this);
    qbStateLabelLabel = new QLabel(this);
    qbStateLabel = new QLabel(this);
    connect(this, SIGNAL(enableEditing(bool)), answerChangeLabel, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(enableEditing(bool)), questionLabel, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(enableEditing(bool)), questionTypeLabel, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(enableEditing(bool)), solutionLabel, SLOT(setEnabled(bool)));

    treeView = new QTreeView(this);
    treeView->setHeaderHidden(true);
    treeView->header()->setStretchLastSection(false);
    treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(treeView, SIGNAL(clicked(const QModelIndex &)), SLOT(saveData()));
    connect(treeView, SIGNAL(clicked(const QModelIndex &)), SLOT(selectQuestion(const QModelIndex &)));

    connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged, [this](const auto &curr, const auto &prev)
    {
        if (opened)
            treeView->clicked(curr);
    });

    answersGroupBox = new QGroupBox(this);


    //treeView->setTabOrder(treeView, questionType);
}

void MainWindow::createLayouts()
{
    leftLayout = new QVBoxLayout;
    leftLayout->addWidget(treeViewLabel);
    leftLayout->addWidget(treeView);

    answersLayout = new QVBoxLayout;
    //answersLayout->addWidget(scrollArea);
    answersGroupBox->setLayout(answersLayout);

    topLayout = new QVBoxLayout;
    topLayout->addWidget(questionLabel);
    topLayout->addWidget(questionEdit);
    midLayout = new QVBoxLayout;
    midLayout->addWidget(answersGroupBox);
    botLayout = new QVBoxLayout;
    botLayout->addWidget(solutionLabel);
    botLayout->addWidget(solutionEdit);

    rightLayout = new QVBoxLayout;
    rightLayout->addLayout(topLayout, 1);
    rightLayout->addLayout(midLayout, 4);
    rightLayout->addLayout(botLayout, 1);

    mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addLayout(rightLayout, 3);

    centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void MainWindow::retranslateUi()
{
    fileMenu->setTitle(tr("File"));
    newAction->setText(tr("New..."));
    openAction->setText(tr("Open..."));
    saveAction->setText(tr("Save..."));
    exportToGiftAction->setText(tr("Export to GIFT (IOP)"));
    exitAction->setText(tr("Exit"));
    addSectionAction->setText(tr("Add"));
    removeSectionAction->setText(tr("Remove"));
    addQuestionAction->setText(tr("Add"));
    removeQuestionAction->setText(tr("Remove"));
    addAnswerAction->setText(tr("Add"));
    removeAnswerAction->setText(tr("Remove"));
    editMenu->setTitle(tr("Edit"));
    settingsMenu->setTitle(tr("Settings"));
    languageMenu->setTitle(tr("Language"));
    helpMenu->setTitle(tr("Help"));
    aboutAction->setText(tr("About"));

    solutionLabel->setText(tr("Solution"));
    questionTypeLabel->setText(tr("Question type") + ": ");
    treeViewLabel->setText(tr("List of sections and questions"));
    questionLabel->setText(tr("Question") + " " + tr("#") + "1");
    sectionChangeLabel->setText(tr("Section") + ":");
    questionChangeLabel->setText(tr("Question") + ":");
    answerChangeLabel->setText(tr("Answer") + ":");
    qbStateLabelLabel->setText(tr("Question base state") + ":");
    checkQuestions();

    questionType->setItemText(0, tr("With single answer"));
    questionType->setItemText(1, tr("With multiple answers"));
    questionType->setItemText(2, tr("Accordance question"));

    setWindowTitle(tr("Edit questions"));
}

void MainWindow::changeLanguage(QAction *action)
{
    QApplication::removeTranslator(&iexamTranslator);
    if (action->data() != QString())
    {
        QApplication::installTranslator(&iexamTranslator);
        iexamTranslator.load(action->data().toString());
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateUi();
    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //event->ignore();
    saveFile(QDir::tempPath() + QDir::separator() + "iexam.json");
}

void MainWindow::clear()
{
    currentItem = 0;
    questionEdit->clear();
    solutionEdit->clear();
}

void MainWindow::newFile()
{
    clear();

    QJsonObject question;
    question["text"] = QString("");
    QJsonArray array;
    array.append(question);
    QJsonObject questions;
    questions["questions"] = array;
    QJsonArray des;
    des.append(questions);
    QJsonObject root;
    root["des"] = des;
}

bool MainWindow::openFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QByteArray saveData = file.readAll();
    QJsonObject root = QJsonDocument::fromJson(saveData).object();
    if (root.size() == 0)
        return false;
    clear();
    return true;
}

void MainWindow::saveFile(QString filename)
{
    saveData();

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return;

}

void MainWindow::exportToGift(QString filename)
{
}

void MainWindow::saveData()
{
}

void MainWindow::checkQuestions()
{
}

void MainWindow::selectQuestion(const QModelIndex &index)
{
}
