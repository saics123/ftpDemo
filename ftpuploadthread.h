#ifndef FTPUPLOADTHREAD_H
#define FTPUPLOADTHREAD_H

#include <QThread>
#include <QQueue>
#include <QStringList>

#include "ftpmanager.h"

class FtpUploadThread : public QObject
{
	Q_OBJECT
public:
	FtpUploadThread(FtpManager *pFtpManager, QObject* parent = 0);
	~FtpUploadThread();
	void addFileToUpload(const QString &filePath);
	void clearError();//清除错误表
	const QString currentFile();

private:
	bool m_shouldStop;
	bool m_isBusy;
	FtpManager* m_pFtpManager;
	QQueue<QString> m_UploadQue;//待上传的文件队列
	QMap<QString, QString> m_errList;//历史错误信息表
	QString m_currentFile;//当前上传的文件
	int m_currentErr;//当前错误信息
	qint64 m_totalMount;//本次需要传输的文件的总数量
	qint64 m_finishedMount;//本次已经传输的文件的数量
	QThread *m_pThread;//执行该类槽函数的操作线程

private slots:
	void finish();//完成传输
	void receiveError(int error);//错误信息添加到历史错误信息表
	void doRun();

signals:
	//更新所有文件上传的总进度，以文件为单位
	void updateTotalProgress(qint64, qint64);
	void updateCurrenProgress(qint64, qint64);
	void notifyUploadFinish();
	void start();
	void notifyError(QString);
	void notifyFinish();
};

#endif // FTPUPLOADTHREAD_H
