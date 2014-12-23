#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_connectiondialog.h"
#include "connectiondialog.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <iostream>
#include <QVariant>

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

void MainWindow::on_connectButton_clicked()
{
    ConnectionDialog *dialog = new ConnectionDialog(this);
    if( dialog->exec() ){
        databaseType = dialog->ui->databaseTypeBox->currentText();
        hostname = dialog->ui->hostNameEdit->text();
        databaseName = dialog->ui->databaseNameEdit->text();
        userName = dialog->ui->userNameEdit->text();
        password = dialog->ui->passwordEdit->text();
    }
    if( !createConnection() ){
        ui->textBrowser->append( tr("Open database failed!" ) );
    } else {
        ui->textBrowser->append( tr( "Open database successfully!" ) );
    }

}

bool MainWindow::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase( databaseType );
    db.setHostName( hostname );
    db.setDatabaseName( databaseName );
    db.setUserName( userName );
    db.setPassword( password );
    if( !db.open() ){
        std::cout << "Open database failed!" << std::endl;
        return false;
    } else {
        std::cout << "Database connected!" << std::endl;
        return true;
    }

}
