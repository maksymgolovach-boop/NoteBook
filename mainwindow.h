#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateWindowTitle();

private slots:
    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void TextChanged();

    void on_actionNew_triggered();

private:
    Ui::MainWindow *ui;
    MainWindow *newwindow;
    bool m_isDirty = false;
    FileManager filehandler;

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
