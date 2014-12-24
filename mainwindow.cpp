#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_connectiondialog.h"
#include "connectiondialog.h"
#include <QSqlQuery>
#include <iostream>
#include <QVariant>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QImageReader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    if( db.isOpen() ){
        db.close();
    }
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
    }else{
        return;
    }
    if( !createConnection() ){
        ui->textBrowser->append( tr("Open database failed!" ) );
    } else {
        ui->textBrowser->append( tr( "Open database successfully!" ) );
        QStringList tableList;
        tableList = db.tables();
        ui->tableBox->addItems(tableList);
    }

}

bool MainWindow::createConnection()
{
    db = QSqlDatabase::addDatabase( databaseType );
    db.setHostName( hostname );
    db.setDatabaseName( databaseName );
    db.setUserName( userName );
    db.setPassword( password );
    if( !db.open() ){
        return false;
    } else {
        return true;
    }

}

void MainWindow::on_listButton_clicked()
{
    QSqlQuery query;
    QString items = ui->SELECTEdit->text();
    QStringList itemList = items.split("," );
    int nbr_items = itemList.length();
    QString conditions = ui->WHEREEdit->text();

    QString sqlCommand;
    if( conditions.isEmpty()  ){
        sqlCommand = "SELECT " + items +
                " FROM " + ui->tableBox->currentText() + ";";
    }else{
        sqlCommand = "SELECT " + items +
                " FROM " + ui->tableBox->currentText() +
                " WHERE " + conditions + ";";
    }
    if( !query.exec( sqlCommand ) ){
            ui->textBrowser->append( tr( "query operation failed!" ) );
    }else{
            ui->textBrowser->append( tr( "query operation successfully!\n" ) );
            QString head;
            foreach (QString item, itemList) {
                head += item + "\t";
            }
            ui->textBrowser->append( head );
    }
    while( query.next() ){
        QString outputline;
        for( int i = 0; i < nbr_items; i++ ){
            outputline += query.value(i).toString() + "\t";
        }
        ui->textBrowser->append( outputline );
    }

}

void MainWindow::on_imageBrowseButton_clicked()
{
    QString initialPath = ui->imagepathEdit->text();
    if( initialPath.isEmpty() ){
        initialPath = QDir::homePath();
    }
    QString imagepath = QFileDialog::getExistingDirectory( this, "Choose imageSet directory",
                                                           initialPath );
    imagepath = QDir::toNativeSeparators( imagepath);
    if( !imagepath.isEmpty() ){
        ui->imagepathEdit->setText( imagepath );
    }

}


void MainWindow::on_depthBrowseButton_clicked()
{
    QString initialPath = ui->depthpathEdit->text();
    if( initialPath.isEmpty() ){
        initialPath = QDir::homePath();
    }
    QString depthpath = QFileDialog::getExistingDirectory( this, "Choose imageSet directory",
                                                           initialPath );
    depthpath = QDir::toNativeSeparators( depthpath);
    if( !depthpath.isEmpty() ){
        ui->depthpathEdit->setText( depthpath );
    }
}

void MainWindow::on_insertButton_clicked()
{
    QString imagepath = ui->imagepathEdit->text();
    QString depthpath = ui->depthpathEdit->text();
    if( imagepath.isEmpty() || depthpath.isEmpty() ){
        ui->textBrowser->append( tr( "imagepath or depthpath is empty!" ) );
        return;
    }

    QString imagePrefix = "image_";
    QString depthPrefix = "depth_";

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    QString sqlCommand = "INSERT INTO " + ui->tableBox->currentText() +
            " VALUES (:id,:imagepath,:imagename,:depthpath,:depthname );" ;
    for( int i = 1; i <= 1449; i++ ){
        QString imageName = imagePrefix + QString::number(i) + ".png";
        QString depthName = depthPrefix + QString::number(i) + ".yaml";
        query.prepare( sqlCommand );
        query.bindValue( ":id", i );
        query.bindValue( ":imagepath", imagepath );
        query.bindValue( ":imagename", imageName );
        query.bindValue( ":depthpath", depthpath );
        query.bindValue( ":depthname", depthName );
        query.exec();
    }
    if( QSqlDatabase::database().commit() ){
        ui->textBrowser->append( tr("Commit finished" ) );
    }else{
        ui->textBrowser->append( tr("Commit failed" ) );
    }


}


void MainWindow::on_iterativelyInsertButton_clicked()
{
    QString initialPath = ui->iterativelyInsertEdit->text();
    if( initialPath.isEmpty() ){
        initialPath = QDir::homePath();
    }
    QString path = QFileDialog::getExistingDirectory( this, "Choose a directory",
                                                      initialPath );
    if( path.isEmpty()){
        ui->textBrowser->append( tr("The path you choose is empty" ) );
        return;
    }
    path = QDir::toNativeSeparators( path );
    ui->iterativelyInsertEdit->setText( path );
    QStringList pathList;
    QStringList fileList;
    getFileIteratively( path, pathList, fileList );
    
    QStringList::const_iterator i = pathList.constBegin();
    QStringList::const_iterator j = fileList.constBegin();
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    QString sqlCommand = "INSERT INTO " + ui->tableBox->currentText() +
            " VALUES (:id,:imagepath,:imagename );" ;
    for( int k = 1; i != pathList.constEnd() && 
         j != fileList.constEnd(); k++, i++, j++ ){
        query.prepare( sqlCommand );
        query.bindValue( ":id", k );
        query.bindValue( ":imagepath", *i );
        query.bindValue( ":imagename", *j );
        query.exec();
    }
    if( QSqlDatabase::database().commit() ){
        ui->textBrowser->append( tr("Commit finished" ) );
    }else{
        ui->textBrowser->append( tr("Commit failed" ) );
    }
}

void MainWindow::getFileIteratively(QString const &path, QStringList &pathList, QStringList &fileList )
{
    QDir dir( path );
    QStringList filters;
    foreach (QByteArray format, QImageReader::supportedImageFormats() ) {
        filters += "*." + format;
    }

    foreach (QString file, dir.entryList( filters, QDir::Files ) ) {
        pathList.push_back( path );
        fileList.push_back( file );
    }

    foreach( QString subDir, dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot ) ){
        getFileIteratively( path + QDir::separator() + subDir, pathList, fileList );
    }
    
}

