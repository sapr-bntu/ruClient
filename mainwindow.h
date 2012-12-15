#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtNetwork/QNetworkReply"
#include "QScriptEngine"
#include "QScriptValue"
#include "QScriptValueIterator"
#include <title.h>
#include "filedownloader.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public slots:
    void onResult(QNetworkReply*);
    void onFileResult(QNetworkReply*);
public:
    explicit MainWindow(QWidget *parent = 0);
    QNetworkAccessManager networkManager;
    QVector<title> titles;
    void renewTable();
    QUrl url;
    QNetworkRequest request;
    QNetworkReply *reply;
    ~MainWindow();
private:
    FileDownloader *m_pImgCtrl;
    bool firstPic;
    void GetSerialsByName();
    void GetSerialsByFile();
    bool firstPush;
    void login(QString login, QString pass);
private slots:
    void on_pushButton_clicked();

    void loadImage();

    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
