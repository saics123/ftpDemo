#ifndef FTPMANAGER_H
#define FTPMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QThread>

class FtpManager : public QObject
{
	Q_OBJECT
public:
	explicit FtpManager(QObject *parent = 0);
	explicit FtpManager(const QString &hostIp,
						const int &hostPort,
						const QString &userName,
						const QString &userPwd,
						QObject *parent = 0);
	~FtpManager();

	int synchronizeRec(const QString &filePath);//同步发送
	int synchronizeSend(const QString &filePath);//同步发送

	enum ErrorCode{
		FileNotExist = 1001,
	};

	static QString toString(int errCode);//将错误码翻译成中文的错误信息

signals:
	//将上传/下载进度通知给界面
	void notifyUpProgress(const qint64 &bytesSent, const qint64 &bytesTotal);
	void notifyDownProgress(const qint64 &bytesSent, const qint64 &bytesTotal);

	//外部调用，通知manager进行上传/下载
	void pleaseDownload(QString, QString);
	void pleaseUpload(QString, QString);

	//将错误信息通知给界面
	void error(int);

	//通知界面传输完成
	void notifyUploadFinish();
	void notifyDownloadFinish();
public slots:
	void asynchronousUp(const QString &src, const QString &des);//异步上传
	void asynchronousDown(const QString &src, const QString &des);//异步发送

private slots:
	void uploadFinish();
	void downloadFinish();
	void downloadToFile(qint64, qint64);

	//收集各类枚举错误码，转化成统一的int型发送给界面
	void replyErrorHandler(QNetworkReply::NetworkError errCode);

private:
	QString m_hostIp;
	int m_hostPort;
	QString m_userName;
	QString m_userPwd;
	QUrl m_url;
	QFile m_file;
	QNetworkAccessManager m_netManager;
	static QThread st_opThread;//用来执行上传和下载操作的线程
};

#endif // FTPMANAGER_H
