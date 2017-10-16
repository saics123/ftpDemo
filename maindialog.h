#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include "ftpmanager.h"
#include "ftpuploadthread.h"

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MainDialog(QWidget *parent = 0);
	~MainDialog();

private:
	Ui::MainDialog *ui;
	FtpManager* m_pFtpManager;
	FtpUploadThread* m_pFtpThread;

private slots:
	void updateProgress(qint64 bytesSent, qint64 bytesTotal);// 更新进度
	void finish();//完成传输
	void getError(int error);//处理错误

	void on_btnDownload_clicked();
	void on_btnUpload_clicked();

	void on_btnUpdate_clicked();
	void on_btnDirUp_clicked();
};

#endif // MAINDIALOG_H
