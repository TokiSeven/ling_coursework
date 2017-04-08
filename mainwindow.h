#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QString>
#include <QList>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "parsing.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

private slots:
    void on_btn_view_clicked();
    QString load_data_from_file();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
