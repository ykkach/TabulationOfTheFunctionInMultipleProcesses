
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
    std::string createCommLineArguments(int numOfProcesses, double A, double B, int steps);
    void on_kill_clicked();
    void on_suspend_clicked();
    void on_run_clicked();

    void on_Set_clicked();

private:
    PROCESS_INFORMATION PI[8];
    int numberOfDivisions = 0;
    Ui::MainWindow *ui;
    std::string path = "C:\\Users\\Yaroslav\\Documents\\MultyThredingProgram\\executable.exe";
};
#endif // MAINWINDOW_H
