#include "maindialog.h"
#include "ui_maindialog.h"


MainDialog::MainDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MainDialog)
{
	ui->setupUi(this);
	m_pFtpManager= new FtpManager("192.168.1.145", 21,
								  "Administrator", "za123.", 0);
	m_pFtpThread = new FtpUploadThread(m_pFtpManager);

//	m_pFtpManager = new FtpManager("192.168.1.123", 21,
//								   "wq", "123", 0);

	connect(m_pFtpThread, SIGNAL(notifyError(QString)),
			this, SLOT(getError(QString)));
	connect(m_pFtpThread, SIGNAL(notifyUploadFinish()), this, SLOT(finish()));
	connect(m_pFtpThread, SIGNAL(updateCurrenProgress(qint64,qint64)),
			this, SLOT(updateFileProgress(qint64,qint64)));
	connect(m_pFtpThread, SIGNAL(updateTotalProgress(qint64,qint64)),
			this, SLOT(updateTotalProgress(qint64,qint64)));
	connect(m_pFtpThread, SIGNAL(notifyFinish()), this, SLOT(finish()));
}

MainDialog::~MainDialog()
{
	delete ui;
	delete m_pFtpManager;
	delete m_pFtpThread;
}

void MainDialog::updateFileProgress(qint64 bytesSent, qint64 bytesTotal)
{
	//更新进度条
	//如果传输失败，会发来bytesTotal = 0；此时进度条会不断左右移动。
	//在这种情况下不应该更新进度条
	if(bytesTotal != 0){
		//显示当前文件路径
		ui->lbCurrentFile->setText("当前文件：" + m_pFtpThread->currentFile());
		//显示进度
		ui->pbCurrentFile->setMaximum(bytesTotal);
		ui->pbCurrentFile->setValue(bytesSent);
	}
}

void MainDialog::updateTotalProgress(qint64 bytesSent, qint64 bytesTotal)
{
	//更新进度条
	//如果传输失败，会发来bytesTotal = 0；此时进度条会不断左右移动。
	//在这种情况下不应该更新进度条
	if(bytesTotal != 0){
		ui->pbTotal->setMaximum(bytesTotal);
		ui->pbTotal->setValue(bytesSent);
	}
}

void MainDialog::finish()
{
	ui->btnDownload->setEnabled(true);
	ui->btnUpload->setEnabled(true);
	ui->teMsg->append("传输结束");
}

void MainDialog::getError(const QString& error)
{
	ui->teMsg->append(QString("错误：" + error));
}

void MainDialog::on_btnDownload_clicked()
{
	ui->btnUpload->setEnabled(false);
	ui->btnDownload->setEnabled(false);
	disconnect(m_pFtpManager, SIGNAL(notifyDownProgress(qint64,qint64)),
			this, SLOT(updateProgress(qint64,qint64)));
	disconnect(m_pFtpManager, SIGNAL(notifyUpProgress(qint64,qint64)),
			this, SLOT(updateProgress(qint64,qint64)));
	connect(m_pFtpManager, SIGNAL(notifyDownProgress(qint64,qint64)),
			this, SLOT(updateProgress(qint64,qint64)));

	//获取待收文件的名称，并设置为目标文件名
	QString srcPath = ui->leDownload->text();
	const QString desPath = "./"
			+ srcPath.right(srcPath.count() - srcPath.lastIndexOf('/') - 1);
	m_pFtpManager->asynchronousDown(srcPath, desPath);
}

void MainDialog::on_btnUpload_clicked()
{
	//连接信号
	ui->btnUpload->setEnabled(false);
	ui->btnDownload->setEnabled(false);
	disconnect(m_pFtpManager, SIGNAL(notifyDownProgress(qint64,qint64)),
			this, SLOT(updateProgress(qint64,qint64)));
	disconnect(m_pFtpManager, SIGNAL(notifyUpProgress(qint64,qint64)),
			this, SLOT(updateProgress(qint64,qint64)));
	connect(m_pFtpManager, SIGNAL(notifyUpProgress(qint64,qint64)),
			this, SLOT(updateProgress(qint64,qint64)));

	//获取待发文件的名称，并设置为目标文件名
	QString srcPath = ui->leUpload->text();
	const QString desPath = "/iristamplate/"
			+ srcPath.right(srcPath.count() - srcPath.lastIndexOf('/') - 1);
	m_pFtpManager->asynchronousUp(srcPath, desPath);
}

void MainDialog::on_btnUpdate_clicked()
{

}

void MainDialog::on_btnDirUp_clicked()
{
	ui->btnDirUp->setEnabled(false);
	ui->btnUpload->setEnabled(false);
	ui->btnDownload->setEnabled(false);

	m_pFtpThread->addFileToUpload("/home/wq/Downloads/gs.0000020170911.log");
	m_pFtpThread->addFileToUpload("/home/wq/Downloads/gs.0000020170912.log");
	m_pFtpThread->addFileToUpload("/home/wq/Downloads/gs.0000020170913.log");
	m_pFtpThread->addFileToUpload("/home/wq/Downloads/gs.0000020170914.log");

	emit m_pFtpThread->start();
}
