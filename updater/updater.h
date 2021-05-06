#ifndef UPDATER_H
#define UPDATER_H

#include <QMainWindow>
#include <QString>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>
#include <QSslError>

QT_BEGIN_NAMESPACE
namespace Ui { class updater; }
QT_END_NAMESPACE

class updater : public QMainWindow
{
    Q_OBJECT

public:
    updater(QWidget *parent = nullptr);
    ~updater();

    bool downloadFile(QString url, QString dest);
    bool exstractFile(QString l, const QString &dest);

private slots:
    void downloadUpdate();

private:

    void sslErrors(QNetworkReply *, const QList<QSslError> &errors);
    QNetworkAccessManager *manager;
    QNetworkRequest request;


    QNetworkReply *reply;

    Ui::updater *ui;
};
#endif // UPDATER_H
