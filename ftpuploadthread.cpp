#include "ftpuploadthread.h"
#include <unistd.h>
#include <QDebug>

FtpUploadThread::FtpUploadThread(FtpManager *pFtpManager, QObject *parent)
	:QObject(parent)
{
	m_pFtpManager = pFtpManager;
	m_shouldStop = false;
	m_isBusy = false;
	m_currentErr = 0;
	m_currentFile.clear();
	m_pThread = new QThread(0);
	this->moveToThread(m_pThread);
	//m_pFtpManager->moveToThread(m_pThread);

	connect(m_pFtpManager, SIGNAL(notifyDownProgress(qint64,qint64)),
			this, SIGNAL(updateCurrenProgress(qint64,qint64)));
	connect(m_pFtpManager, SIGNAL(notifyUpProgress(qint64,qint64)),
			this, SIGNAL(updateCurrenProgress(qint64,qint64)));
	connect(m_pFtpManager, SIGNAL(notifyUploadFinish()), this, SLOT(finish()));
	connect(m_pFtpManager, SIGNAL(notifyDownloadFinish()), this, SLOT(finish()));
	connect(m_pFtpManager, SIGNAL(error(int)), this, SLOT(receiveError(int)));
	connect(this, SIGNAL(start()), this, SLOT(doRun()));

	m_pThread->start();
}

FtpUploadThread::~FtpUploadThread()
{
	m_shouldStop = true;
	m_pThread->exit(0);
	while(m_pThread->isRunning()){
		usleep(20*1e3);
	}
	delete m_pThread;
}

//向队列中添加需要上传的文件
void FtpUploadThread::addFileToUpload(const QString &filePath)
{
	m_UploadQue.append(filePath);
}

void FtpUploadThread::doRun()
{
	m_totalMount = m_UploadQue.count();
	m_finishedMount = 0;
	m_errList.clear();
	m_currentErr = 0;

	while(!m_shouldStop && !m_UploadQue.isEmpty()){
		if(!m_isBusy){
			//1.确认上传的文件是否存在
			QString srcPath = m_UploadQue.dequeue();
			QFile file(srcPath);
			if(!file.exists()){
				qDebug()<<"file not exist when upload "<<srcPath;
				continue;
			}

			//2.生成目标文件路径
			const QString desPath = "/iristamplate/"
					+ srcPath.right(srcPath.count() - srcPath.lastIndexOf('/') - 1);
			m_currentFile = srcPath;
			m_isBusy = true;
			m_currentErr = 0;
			emit m_pFtpManager->pleaseUpload(srcPath, desPath);
			//m_pFtpManager->asynchronousUp(srcPath, desPath);
		}else{//休眠20ms
			usleep(20 * 1e3);
		}
	}
}

void FtpUploadThread::finish()
{
	m_isBusy = false;
	m_finishedMount++;
	emit updateTotalProgress(m_finishedMount, m_totalMount);
}

void FtpUploadThread::receiveError(int error)
{
	m_currentErr = error;
	m_errList.insert(m_currentFile, FtpManager::toString(error));
	emit notifyError(m_currentFile + ":" +FtpManager::toString(error));
}

void FtpUploadThread::clearError()
{
	m_errList.clear();
}



