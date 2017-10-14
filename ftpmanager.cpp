#include "ftpmanager.h"
#include <QFileInfo>
#include <QDebug>

QThread FtpManager::st_opThread;

FtpManager::FtpManager(QObject *parent) : QObject(parent)
{
	//设置协议
	m_url.setScheme("ftp");

	//设置FTP服务器数据
	m_hostIp = "127.0.0.1";
	m_hostPort = 21;
	m_userName = "root";
	m_userPwd = "123";
}

FtpManager::FtpManager(const QString &hostIp, const int &hostPort,
					   const QString &userName, const QString &userPwd,
					   QObject *parent): QObject(parent)
{
	//设置协议
	m_url.setScheme("ftp");
	m_hostIp = hostIp;
	m_hostPort = hostPort;
	m_userName = userName;
	m_userPwd = userPwd;
	m_url.setHost(hostIp);
	m_url.setPort(hostPort);
	m_url.setUserName(userName);
	m_url.setPassword(userPwd);

	//	this->moveToThread(&st_opThread);
	//	st_opThread.start();
	connect(this, SIGNAL(pleaseDownload(QString,QString)), this, SLOT(asynchronousDown(QString,QString)));
	connect(this, SIGNAL(pleaseUpload(QString,QString)), this, SLOT(asynchronousUp(QString,QString)));
}

FtpManager::~FtpManager()
{
	st_opThread.quit();
}

int FtpManager::synchronizeRec(const QString &filePath)
{
	Q_UNUSED(filePath);
	return 0;
}

int FtpManager::synchronizeSend(const QString &filePath)
{
	Q_UNUSED(filePath);
	return 0;
}

/**
 * @brief 错误码翻译成中文的错误信息
 * 包括自定义的错误码和QNetworkReply::NetworkError中的错误码
 * @return
 */
QString FtpManager::toString(int errCode)
{
	QString errMsg("");
	switch (errCode) {
	case 1:
		errMsg = "FTP服务器地址或端口不正确";
		break;
	case 201:
		errMsg = "没有操作权限";
		break;
	case 203:
		errMsg = "没有找到待下载的文件";
	default:
		break;
	}
	return errMsg;
}

void FtpManager::asynchronousUp(const QString &src, const QString &des)
{
	//准备文件
	m_file.setFileName(src);
	if(!m_file.exists()){
		emit error(FileNotExist);
		return;
	}
	m_file.open(QIODevice::ReadOnly);

	//发送
	m_url.setPath(des);
	qDebug()<<m_url.toString();
	QNetworkRequest request;
	request.setUrl(m_url);
	QNetworkReply* pReply = m_netManager.put(request, &m_file);

	connect(pReply, SIGNAL(finished()), this, SLOT(uploadFinish()));
	connect(pReply, SIGNAL(uploadProgress(qint64,qint64)),
			this, SIGNAL(notifyUpProgress(qint64,qint64)));
	connect(pReply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(replyErrorHandler(QNetworkReply::NetworkError)));
}

void FtpManager::asynchronousDown(const QString &src, const QString &des)
{
	//准备文件，如果文件已经存在，就删除
	m_file.setFileName(des);
	if(m_file.exists()){
		QString cmd = "rm -f ";
		cmd.append(des);
		system(cmd.toUtf8().constData());
	}

	m_file.open(QIODevice::WriteOnly | QIODevice::Append);

	//下载
	m_url.setPath(src);
	qDebug()<<m_url.toString();
	QNetworkRequest request;
	request.setUrl(m_url);
	QNetworkReply* pReply = m_netManager.get(request);

	connect(pReply, SIGNAL(finished()), this, SLOT(uploadFinish()));
	connect(pReply, SIGNAL(downloadProgress(qint64,qint64)),
			this, SIGNAL(notifyDownProgress(qint64,qint64)));
	connect(pReply, SIGNAL(downloadProgress(qint64,qint64)),
			this, SLOT(downloadToFile(qint64,qint64)));
	connect(pReply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(replyErrorHandler(QNetworkReply::NetworkError)));
}

void FtpManager::uploadFinish()
{
	//获取QNetworkReply，以停止接收FTP传输的消息
	QObject* sender = QObject::sender();
	if(0 == sender){
		qDebug()<<"uploadFinish sender == 0";
	}else{
		QNetworkReply* pReply = qobject_cast<QNetworkReply*>(sender);
		pReply->disconnect();
		m_file.close();
	}
	//发送上传完成的消息
	emit notifyUploadFinish();
}

void FtpManager::downloadFinish()
{
	//获取QNetworkReply，以停止接收FTP传输的消息
	QObject* sender = QObject::sender();
	if(0 == sender){
		qDebug()<<"downloadFinish sender == 0";
	}else{
		QNetworkReply* pReply = qobject_cast<QNetworkReply*>(sender);
		pReply->disconnect();
		m_file.write(pReply->readAll());
		m_file.flush();
		m_file.close();
	}

	//发送上传完成的消息
	emit notifyDownloadFinish();
}

void FtpManager::downloadToFile(qint64,qint64)
{
	//获取QNetworkReply，以获取下载的内容
	QObject* sender = QObject::sender();

	if(0 == sender){
		qDebug()<<"downloadToFile sender == 0";
	}else{
		QNetworkReply* pReply = qobject_cast<QNetworkReply*>(sender);
		//读取下载的内容，写入文件中
		m_file.write(pReply->readAll());
		m_file.flush();
	}
}

void FtpManager::replyErrorHandler(QNetworkReply::NetworkError errCode)
{
	emit error((int)errCode);
}


