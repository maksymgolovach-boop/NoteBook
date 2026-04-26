#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "filemanager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QSettings>
#include <QInputDialog>
#include <QWheelEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);

    connect(ui->actionRedo, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::redo);
    connect(ui->actionUndo, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::undo);

    connect(ui->plainTextEdit, &QPlainTextEdit::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(ui->plainTextEdit, &QPlainTextEdit::redoAvailable, ui->actionRedo, &QAction::setEnabled);

    this->setWindowTitle("NoteBook");
    setCentralWidget(ui->plainTextEdit);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &MainWindow::TextChanged);
    renderRecent();

    setShortcuts();

    ui->plainTextEdit->viewport()->installEventFilter(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Open
void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file");
    if(fileName.isEmpty()) return;
    QString format = QFileInfo(fileName).suffix();
    if (!filehandler.isValidFormat(format)) {
        throw std::runtime_error("Unsupported file format: " + format.toStdString());
    }
    try{
        OpenNew(fileName);
    } catch (std::exception& e){
        QMessageBox::critical(this, "Error", e.what());
    }
    updateWindowTitle();
    updateRecent(fileName);
}

void MainWindow::OpenNew(const QString& filepath){
    if(filepath.isEmpty())
        return;
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Open"), tr("Do you want to open %1 in new window ?").arg(QFileInfo(filepath).fileName()),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    try{
        if(reply == QMessageBox::Yes){
            int x = this->pos().x();
            int y = this->pos().y();

            newwindow = new MainWindow(this);
            newwindow->move(x+20, y+20);

            QString content = newwindow->filehandler.openFile(filepath);
            newwindow->ui->plainTextEdit->setPlainText(content);
            newwindow->m_isDirty = false;

            newwindow->show();
            newwindow->updateRecent(filepath);
            newwindow->updateWindowTitle();
        }
        else if(reply == QMessageBox::No) {
            QString content = filehandler.openFile(filepath);

            ui->plainTextEdit->blockSignals(true);
            ui->plainTextEdit->setPlainText(content);
            ui->plainTextEdit->blockSignals(false);
            m_isDirty = false;
            updateRecent(filepath);
            updateWindowTitle();
        }
    }
    catch(std::exception& e){
        QMessageBox::critical(this, "Error", e.what());
    }
}

// Save
void MainWindow::on_actionSave_triggered()
{
    if(filehandler.hasOpenedFile()){
        try{
            filehandler.saveFile(ui->plainTextEdit->toPlainText());
            m_isDirty = false;
        } catch(std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
        }
    }  else {
        on_actionSave_as_triggered();
        return;
    }
    updateRecent(filehandler.getCurrentFilePath());
    updateWindowTitle();
}

// Save as...
void MainWindow::on_actionSave_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as", "C:/");
    if(fileName.isEmpty()) return;

    QString format = QFileInfo(fileName).suffix();
    if(format.isEmpty()) {  // set default format to .txt
        fileName += ".txt";
        format = "txt";
    }
    try{
        filehandler.saveFileAs(fileName, format);
        filehandler.saveFile(ui->plainTextEdit->toPlainText());
        m_isDirty = false;
    } catch (std::exception& e){
        QMessageBox::critical(this, "Error", e.what());
    }
    updateRecent(fileName);
    updateWindowTitle();
}

void MainWindow::updateWindowTitle() {
    QString path = filehandler.getCurrentFilePath();
    QString fileName = path.isEmpty() ? "Untitled" : QFileInfo(path).fileName();

    if(m_isDirty)
        fileName+="*";

    this->setWindowTitle(fileName + " - NoteBook");
}

void MainWindow::TextChanged(){
    if(!m_isDirty){
        m_isDirty = true;
        updateWindowTitle();
    }
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(!m_isDirty){
        event->accept();
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Unsaved changes", "The document has been modified, do you want to save changes ?",
                                  QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if(reply == QMessageBox::Save){
        on_actionSave_triggered();
        event->accept();
    }
    else if(reply == QMessageBox::Discard){
        event->accept();
    }
    else{
        event->ignore();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    if(obj == ui->plainTextEdit->viewport() && event->type() == QEvent::Wheel){
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);

        if(wheelEvent->modifiers() & Qt::ControlModifier){
            if(wheelEvent->angleDelta().y() > 0){
                this->ui->plainTextEdit->zoomIn(1);
            } else {
                this->ui->plainTextEdit->zoomOut(1);
            }
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionNew_triggered()
{
    int x = this->pos().x();
    int y = this->pos().y();
    newwindow = new MainWindow(this);
    newwindow->move(x+20, y+20);
    newwindow->show();
}

void MainWindow::updateRecent(const QString &filepath)
{
    QSettings settings("ThisNotebook","NoteBook");

    QStringList files = settings.value("recentFiles").toStringList();

    files.removeAll(filepath);

    files.prepend(filepath);

    while (files.size() > 5) {
        files.removeLast();
    }

    settings.setValue("recentFiles", files);

    renderRecent();
}

void MainWindow::renderRecent(){
    QSettings settings("ThisNotebook","NoteBook");

    QStringList files = settings.value("recentFiles").toStringList();

    ui->menuRecent->clear();

    for(int i =0 ; i < files.size() ;  ++i){
        QString FileText = tr("&%1 %2").arg(i+1).arg(QFileInfo(files[i]).fileName());

        QAction *action = ui->menuRecent->addAction(FileText);
        action->setData(files[i]);

        connect(action, &QAction::triggered, this, [this, action]() {
            QString path = action->data().toString();

            OpenNew(path);
        });
    }
}


void MainWindow::on_actionFind_triggered()
{
    bool answer;
    QString searchStr = QInputDialog::getText(this, tr("Find"), tr("Text to find: "), QLineEdit::Normal, "", &answer);

    if(!answer || searchStr.isEmpty()){
        return;
    }

    bool found = ui->plainTextEdit->find(searchStr);

    if(!found){
        QTextCursor cursor = ui->plainTextEdit->textCursor();
        cursor.setPosition(0);
        ui->plainTextEdit->setTextCursor(cursor);

        found = ui->plainTextEdit->find(searchStr);
        if(!found){
            QMessageBox::information(this, tr("Find"), tr("Not found a single match!"));
        }
    }
}

void MainWindow::on_actionZoom_In_triggered()
{
    this->ui->plainTextEdit->zoomIn(2);
}


void MainWindow::on_actionZoom_Out_triggered()
{
    this->ui->plainTextEdit->zoomOut(2);
}


void MainWindow::setShortcuts(){
    //shortcuts
    ui->actionNew->setShortcut(QKeySequence::New);          // Ctrl+N
    ui->actionOpen->setShortcut(QKeySequence::Open);        // Ctrl+O
    ui->actionSave->setShortcut(QKeySequence::Save);        // Ctrl+S
    ui->actionSave_as->setShortcut(QKeySequence::SaveAs);   // Ctrl+Shift+S
    ui->actionExit->setShortcut(QKeySequence::Quit);        // Ctrl+Q
    ui->actionPaste->setShortcut(QKeySequence::Paste);      // Ctrl+V
    ui->actionCopy->setShortcut(QKeySequence::Copy);        // Ctrl+C
    ui->actionRedo->setShortcut(QKeySequence::Redo);        // Ctrl+Z
    ui->actionUndo->setShortcut(QKeySequence::Undo);        // Ctrl+Y
    ui->actionFind->setShortcut(QKeySequence::Find);        // Ctrl+F
    ui->actionZoom_In->setShortcut(QKeySequence::ZoomIn);   // Ctrl++ or Ctrl+mouse wheel roll up
    ui->actionZoom_Out->setShortcut(QKeySequence::ZoomOut); // Ctrl+- or Ctrl+mouse wheel roll down
}
