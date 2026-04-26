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
    void setShortcuts();

private slots:
    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void TextChanged();

    void on_actionNew_triggered();

    void updateRecent(const QString &filepath);

    void on_actionFind_triggered();

    void OpenNew(const QString &filename);

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

private:
    Ui::MainWindow *ui;
    MainWindow *newwindow;
    bool m_isDirty = false;
    FileManager filehandler;

    QMenu *recentFilesMenu;
    QAction *recentFilesAction[5];
    void renderRecent();

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void Find(const QString &string);
};
#endif // MAINWINDOW_H
