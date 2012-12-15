#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QString"
#include "QUrl"
#include "QDebug"
#include "QLabel"
#include "qxtjson.h"
#include "title.h"
#include "QNetworkProxy"
#include "QtAlgorithms"
#include "QGraphicsScene"
#include "QMessageBox"
#include "QFileDialog"
#include "QCryptographicHash"
#include "md5.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    firstPic = true;
    firstPush = true;
    /*QNetworkProxy proxy;
     proxy.setType(QNetworkProxy::HttpProxy);
     proxy.setHostName("172.16.110.60");
     proxy.setPort(2527);
     proxy.setUser("421-3");
     proxy.setPassword("1234567");
     QNetworkProxy::setApplicationProxy(proxy);*/
    ui->centralWidget->setWindowTitle(QString::fromUtf8("Serial detective"));
    ui->statusBar->showMessage(APP_REVISION);
    ui->statusBar->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    GetSerialsByName();
}

void MainWindow::login(QString login, QString pass)
{
    QString hash = MD5(pass);
    url.setUrl("http://api.myshows.ru/profile/login?login="+login+"&password="+hash);
    request.setUrl(url);
    reply = networkManager.get(request);
    qDebug()<<reply->rawHeaderList();
}

void MainWindow::GetSerialsByName()
{
    if (firstPush)
    {
        connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResult(QNetworkReply*)));
    }
    firstPush=false;
    url.setUrl("http://api.myshows.ru/shows/search/?q="+ui->lineEdit->text());
    request.setUrl(url);
    reply = networkManager.get(request);
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem(QString::fromUtf8("Title")));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem(QString::fromUtf8("Id")));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem(QString::fromUtf8("Year")));
    ui->tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem(QString::fromUtf8("Ended")));
}

void MainWindow::renewTable()
{
    /*if (titles.size()>=1)
    {
        titles.remove(0,titles.size());
    }*/
    ui->tableWidget->setRowCount(0);
    for (int i=0;i<titles.size();i++)
    {
        QLabel *label = new QLabel(this);
        label->setOpenExternalLinks(true);
        label->setTextFormat(Qt::RichText);
        label->setTextInteractionFlags(Qt::TextBrowserInteraction);
        label->setProperty("row",i);
        label->setProperty("column",1);

        QString text(QString::fromUtf8("<a href=\"http://myshows.ru/view/"));
        text.append(QString::number(titles[i].id));
        text.append(QString::fromUtf8("/\">myshows.ru<\\a>"));

        label->setText(text);

        ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(titles[i].uktitle));
        ui->tableWidget->setCellWidget(i,1,label);
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(titles[i].year)));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(titles[i].ended));
    }
}

void MainWindow::onResult(QNetworkReply* reply)
{
    const QByteArray rawdata = reply->readAll();

    QxtJSON parser = QxtJSON();

    QVariantMap map = parser.parse(QString::fromUtf8(rawdata.data())).toMap();
    QMapIterator<QString, QVariant> it(map);
    while (it.hasNext())
    {
        it.next();
        title tit;
        tit.id = it.key().toInt();
        QVariantMap map2 = it.value().toMap();
        tit.country = map2["country"].toString();
        tit.uktitle = map2["title"].toString();
        tit.ended = map2["ended"].toString();
        tit.year = map2["year"].toInt();
        tit.image = map2["image"].toString();
        tit.genres = map2["genres"].toString();
        tit.imdbid = map2["imdbid"].toInt();
        tit.kinopoiskid = map2["kinopoiskid"].toInt();
        tit.rating = map2["rating"].toFloat();
        tit.status = map2["status"].toString();
        this->titles.append(tit);
    }
    renewTable();
}


void MainWindow::onFileResult(QNetworkReply* reply)
{
    if (titles.size()>=1)
    {
        titles.remove(0,titles.size());
    }
    const QByteArray rawdata = reply->readAll();

    QxtJSON parser = QxtJSON();

    QVariantMap map = parser.parse(QString::fromUtf8(rawdata.data())).toMap();
    QVariant match = map["match"];
    if (match.toInt()==85 || match.toInt()==100)
    {
        title tit;
        QVariantMap map2 = map["show"].toMap();
        QVariant country = map2["country"];
        tit.country = country.toString();
        QVariant title = map2["title"];
        tit.id = map2["id"].toInt();
        tit.uktitle = title.toString();
        QVariant year = map2["year"];
        QVariant ended = map2["ended"];
        tit.ended = ended.toString();
        tit.year = year.toInt();
        tit.image = map2["image"].toString();
        this->titles.append(tit);
    }
    renewTable();
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{

    if (titles[row].image!="")
    {
        QUrl imageUrl(titles[row].image);
        m_pImgCtrl = new FileDownloader(imageUrl, this);
    }
    //if (firstPic)
    {
        connect(m_pImgCtrl,SIGNAL(downloaded()),SLOT(loadImage()));
    }
    firstPic=false;
}

void MainWindow::loadImage()
{
    QPixmap poster;
    poster.loadFromData(m_pImgCtrl->downloadedData());
    QGraphicsScene* scene = new QGraphicsScene;
    scene->addPixmap(poster);
    ui->graphicsView->setScene(scene);
}

void MainWindow::GetSerialsByFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "",
                                                     tr("Files (*.*)"));
    if (firstPush)
    {
        connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFileResult(QNetworkReply*)));
    }
    firstPush=false;
    QFileInfo fi(fileName);
    QString base = fi.fileName();
    url.setUrl("http://api.myshows.ru/shows/search/file/?q="+fi.fileName());
    request.setUrl(url);
    reply = networkManager.get(request);
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem(QString::fromUtf8("Title")));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem(QString::fromUtf8("Id")));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem(QString::fromUtf8("Year")));
    ui->tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem(QString::fromUtf8("Ended")));
};

void MainWindow::on_pushButton_2_clicked()
{
    GetSerialsByFile();
}

void MainWindow::on_pushButton_3_clicked()
{
    login(ui->lineEdit_2->text(),ui->lineEdit_3->text());
}
