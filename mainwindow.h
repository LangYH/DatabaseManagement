#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();

    void on_listButton_clicked();

    void on_imageBrowseButton_clicked();

    void on_depthBrowseButton_clicked();

    void on_insertButton_clicked();

    void on_iterativelyInsertButton_clicked();
    
private:
    Ui::MainWindow *ui;
    QString databaseType;
    QString hostname;
    QString databaseName;
    QString userName;
    QString password;

    QSqlDatabase db;

private:
    bool createConnection();
    static void getFileIteratively(const QString &path, QStringList &pathList, QStringList &fileList );
};

#endif // MAINWINDOW_H
