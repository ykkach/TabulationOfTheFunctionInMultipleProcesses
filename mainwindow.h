
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <windows.h>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_start_all_clicked();
    std::string priority(DWORD);
    void on_comboBox_2_currentIndexChanged(int index);
    void on_priority_currentIndexChanged(int index);
    void on_kill_clicked();
    void on_suspend_clicked();
    void on_run_clicked();

    void on_Set_clicked();

private:
    PROCESS_INFORMATION PI[8];
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
