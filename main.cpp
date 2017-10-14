#include "maindialog.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
#if (QT_VERSION < 0x050000)
	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
#endif

	MainDialog w;
	w.show();

	return a.exec();
}
