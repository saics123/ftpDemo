#include "ftpuploadthread.h"]
#include <QDebug>

FtpUploadThread::FtpUploadThread(FtpManager* pFtpManager)
{
	m_pFtpManager = pFtpManager;
	m_shouldStop = false;
	m_isBusy = false;
	m_currentErr = 0;
	m_currentFile.clear();

	connect(m_pFtpManager, SIGNAL(error(int)),
			this, SLOT(receiveError(int)));
}

FtpUploadThread::~FtpUploadThread()
{
	delete m_pFtpManager;
}

//向队列中添加需要上传的文件
void FtpUploadThread::addFileToUpload(const QString &filePath)
{
	m_UploadQue.append(filePath);
}

void FtpUploadThread::run()
{
	m_totalMount = 0;
	m_finishedMount = 0;
	m_errList.clear();

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

		}else{//休眠20ms
			usleep(20 * 1e3);
		}
	}
}

void FtpUploadThread::updateCurrenProgress(qint64 bytesSent, qint64 bytesTotal)
{

}

void FtpUploadThread::finish()
{
	m_isBusy = false;
	m_finishedMount++;
}

void FtpUploadThread::receiveError(int error)
{
	m_currentErr = error;
	m_errList.insert(m_currentFile, FtpManager::toString(error));
}

void FtpUploadThread::clearError()
{
	m_errList.clear();
}


