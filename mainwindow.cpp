#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "filemanager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("NoteBook");
    setCentralWidget(ui->plainTextEdit);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &MainWindow::TextChanged);

    ui->actionNew->setShortcut(QKeySequence::New);          // Ctrl+N
    ui->actionOpen->setShortcut(QKeySequence::Open);        // Ctrl+O
    ui->actionSave->setShortcut(QKeySequence::Save);        // Ctrl+S
    ui->actionSave_as->setShortcut(QKeySequence::SaveAs);   // Ctrl+Shift+S
    ui->actionExit->setShortcut(QKeySequence::Quit);        // Ctrl+Q
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
        QString content = filehandler.openFile(fileName);

        ui->plainTextEdit->blockSignals(true);
        ui->plainTextEdit->setPlainText(content);
        ui->plainTextEdit->blockSignals(false);

        m_isDirty = false;
    } catch (std::exception& e){
        QMessageBox::critical(this, "Error", e.what());
    }
    updateWindowTitle();
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
    updateWindowTitle();
}

// Save as...
void MainWindow::on_actionSave_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    if(fileName.isEmpty()) return;
    QString format = QFileInfo(fileName).suffix();
    try{
        filehandler.saveFileAs(fileName, format);
        filehandler.saveFile(ui->plainTextEdit->toPlainText());
        m_isDirty = false;
    } catch (std::exception& e){
        QMessageBox::critical(this, "Error", e.what());
    }
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

void MainWindow::on_actionExit_triggered()
{
    if(!m_isDirty){
        return;
    }

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

