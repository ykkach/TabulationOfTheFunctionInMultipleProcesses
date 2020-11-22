#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <QApplication>
#include <QElapsedTimer>
#include <windows.h>
#include <basetsd.h>
#include <winnt.h>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    for(int i = 0; i < ui->num_of_processes->currentText().toInt(); ++i){
        TerminateProcess(PI->hProcess,0);
        CloseHandle(PI[i].hThread);
    }

    delete ui;
}

void MainWindow::on_start_all_clicked()
{
    numOfProcesses = ui->num_of_processes->currentText().toInt();
    ui->tableWidget->setRowCount(numOfProcesses);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("PID"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Priority"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("State"));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("Time"));
    int steps_q = ui->textEdit_steps->toPlainText().toInt()/numOfProcesses;
    double A = ui->textEdit_A->toPlainText().toDouble(), B = ui->textEdit_B->toPlainText().toDouble();
    double div = (B-A)/numOfProcesses;

    for(int i = 0; i < numOfProcesses; i++)
    {
        STARTUPINFO info;
        PROCESS_INFORMATION processInfo;
        ZeroMemory(&info, sizeof(STARTUPINFO));
        ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));
        B = A + div;

        std::string strCommandLineArgs = path + " " + std::to_string(A) + " " + std::to_string(B) + " " + std::to_string(steps_q) + + " tabulation" + std::to_string(i) + ".txt";
        A += div;
        std::wstring wCommand(strCommandLineArgs.begin(), strCommandLineArgs.end());
        LPWSTR lpwCommand = &(wCommand[0]);

        if(!CreateProcess(L"C:\\Users\\Yaroslav\\Documents\\MultyThredingProgram\\executable.exe",
                      lpwCommand,
                      nullptr,
                      nullptr,
                      FALSE,
                      CREATE_NO_WINDOW | CREATE_SUSPENDED,
                      nullptr,
                      nullptr,
                      /*(LPSTARTUPINFOW)*/&info,
                      &processInfo))
        {
            std::cout << "Create process failed " << GetLastError() << std::endl;
            exit(EXIT_FAILURE);
        }

        PI[i] = processInfo;
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(processInfo.dwProcessId)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(priority(GetPriorityClass(PI[i].hProcess)))));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem("Suspended"));

    }
}

std::string MainWindow::priority(DWORD code){
    switch (code)
    {
        case 32768: return "Above normal";
        case 16384: return "Below normal";
        case 32: return "Normal";
        case 64: return "Idle";
        case 128: return "Hight";
    default: return "";
    }
}

void MainWindow::on_Set_clicked()
{
    HANDLE threadHandles[numOfProcesses];

    QElapsedTimer timer;
    timer.start();

    for(int i = 0; i < numOfProcesses; ++i){
        ui->tableWidget->item(i, 2)->setText("Running");
        ResumeThread(PI[i].hThread);
    }
    WaitForMultipleObjects(numOfProcesses, threadHandles, TRUE, INFINITE);
    timer.nsecsElapsed();

    __int64 kernel, user, creation, exit, result = 0;

    for(int i = 0; i < numOfProcesses; ++i){
         result = 0;
         kernel = 0;
         user = 0;
         GetProcessTimes(PI[i].hProcess,(FILETIME*)&creation,(FILETIME*)&exit,(FILETIME*)&kernel,(FILETIME*)&user);
         result += kernel;
         result += user;

         ui->tableWidget->item(i, 2)->setText("Finished");
         ui->tableWidget->setItem(i,3,new QTableWidgetItem(QString::number(result*0.000001), 'g'));
    }
}

void MainWindow::on_run_clicked()
{
    ResumeThread(PI[ui->comboBox_2->currentIndex()].hThread);
    ui->tableWidget->item(ui->comboBox_2->currentIndex(), 2)->setText("Running");
    WaitForSingleObject(PI[ui->comboBox_2->currentIndex()].hProcess, INFINITE);
    ui->tableWidget->item(ui->comboBox_2->currentIndex(), 2)->setText("Finished");
    ui->tableWidget->item(ui->comboBox_2->currentIndex(),1)->setText(QString::fromStdString((priority(GetPriorityClass(PI[ui->comboBox_2->currentIndex()].hProcess)))));
}

void MainWindow::on_kill_clicked()
{
      TerminateProcess(PI[ui->comboBox_2->currentIndex()].hProcess,0);
      ui->tableWidget->item(ui->comboBox_2->currentIndex(), 2)->setText("Terminated");
      ui->run->setEnabled(0);
      ui->suspend->setEnabled(0);
      ui->kill->setEnabled(0);
      ui->priority->setEnabled(0);
}

void MainWindow::on_suspend_clicked()
{
    SuspendThread(PI[ui->comboBox_2->currentIndex()].hThread);
    ui->tableWidget->item(ui->comboBox_2->currentIndex(), 2)->setText("Suspended");
}


void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    if(ui->tableWidget->item(index,2)->text() == "Terminated"){
        ui->run->setEnabled(0);
        ui->suspend->setEnabled(0);
        ui->kill->setEnabled(0);
        ui->priority->setEnabled(0);
    } else
    {
        ui->run->setEnabled(1);
        ui->suspend->setEnabled(1);
        ui->kill->setEnabled(1);
        ui->priority->setEnabled(1);
    }
    switch((GetPriorityClass(PI[index].hProcess))){
        case 64: ui->priority->setCurrentIndex(0); break;
        case 16384: ui->priority->setCurrentIndex(1); break;
        case 32: ui->priority->setCurrentIndex(2); break;
        case 32768: ui->priority->setCurrentIndex(3); break;
        case 128: ui->priority->setCurrentIndex(4); break;
    }

}

void MainWindow::on_priority_currentIndexChanged(int index)
{
    switch(index){
    case 0: {SetPriorityClass(PI[ui->comboBox_2->currentIndex()].hProcess, IDLE_PRIORITY_CLASS);
            ui->tableWidget->item(ui->comboBox_2->currentIndex(), 1)->setText("Idle");
            break;}
    case 1: {SetPriorityClass(PI[ui->comboBox_2->currentIndex()].hProcess, BELOW_NORMAL_PRIORITY_CLASS);
         ui->tableWidget->item(ui->comboBox_2->currentIndex(), 1)->setText("Below normal");
            break;}
    case 2: {SetPriorityClass(PI[ui->comboBox_2->currentIndex()].hProcess, NORMAL_PRIORITY_CLASS);
         ui->tableWidget->item(ui->comboBox_2->currentIndex(), 1)->setText("Normal");
            break;}
    case 3: {SetPriorityClass(PI[ui->comboBox_2->currentIndex()].hProcess, ABOVE_NORMAL_PRIORITY_CLASS);
        ui->tableWidget->item(ui->comboBox_2->currentIndex(), 1)->setText("Above normal");
            break;}
    case 4: {SetPriorityClass(PI[ui->comboBox_2->currentIndex()].hProcess, HIGH_PRIORITY_CLASS);
         ui->tableWidget->item(ui->comboBox_2->currentIndex(), 1)->setText("High");
            break;}
    case 5: {
        SetPriorityClass(PI[ui->comboBox_2->currentIndex()].hProcess, REALTIME_PRIORITY_CLASS);
        ui->tableWidget->item(ui->comboBox_2->currentIndex(), 1)->setText("Real time");
            break;}
    }
}
