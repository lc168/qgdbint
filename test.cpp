#include "qgdbint.h"
#include "record.h"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

using namespace qgdbint;

int main(int argc, char* argv[]) {
	QCoreApplication app(argc, argv);
	QTimer::singleShot(0, [&]() {
		QGdb* gdb = new QGdb("/usr/bin/gdb", "/usr/bin/gdbserver");
		QObject::connect(gdb, &qgdbint::QGdb::textResponse, [](QString text) {
			qDebug().noquote() << text;
		});
		QObject::connect(gdb, &qgdbint::QGdb::stateChanged, [](bool running, QString reason, QGdb* gdb) {
			if (running == false && reason == "exited-normally") {
				gdb->exit();
			}
		});
		QObject::connect(gdb, &qgdbint::QGdb::positionUpdated, [](QString file, int row) {
			qDebug() << "position changed to" << file << row;
		});
		QObject::connect(gdb, &qgdbint::QGdb::readyStdout, [](QString out) {
			qDebug().nospace() << "@" << out;
		});
		QObject::connect(gdb, &qgdbint::QGdb::exited, &app, &QCoreApplication::quit);
		gdb->start("/home/nekosu/test");
		if (gdb->connect()) {
			gdb->setBreakpoint("main");
			gdb->cont();
			gdb->waitUntilPause();
			gdb->step();
			gdb->cont();
			gdb->waitUntilPause();
		}
	});
	return app.exec();
}

/*
int main() {
	Record rec(R"(stopped,reason="breakpoint-hit",thread-id="1",stopped-threads="1",core="1")");
	rec.result.dump();
}
*/
