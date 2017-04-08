#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::load_data_from_file()
{
    QFile file("document.html");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);

    QString data = "";
    while(!in.atEnd()) {
        data += in.readLine();
    }

    file.close();
    data = data.replace("\t", "");
    return data;
}

void MainWindow::on_btn_view_clicked()
{
    QString raw_data = this->load_data_from_file();
    Parsing parsing(raw_data);
    QList<QListWidgetItem*> data = parsing.parse();
    this->ui->listWidget->clear();
    this->ui->listWidget->addItem("Title first: " + parsing.getTitleFirst());
    this->ui->listWidget->addItem("Title last: " + parsing.getTitleLast());
    for(int i = 0; i < data.count(); i++)
        this->ui->listWidget->addItem(data[i]);
}
