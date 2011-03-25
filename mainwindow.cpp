
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      connector(new Xmms2Connector(this))
{

    ui->setupUi(this);

    // Setting up the connector

    if (!connector->xmms2Connect())
    {
        // Tell the user and quit if failed
        QMessageBox::warning(this, "Error", "Unable to connect to XMMS2.\nThe application will quit.");
        QApplication::quit();
    }

    // Connecting to the signals of the connector

    connect(connector, SIGNAL(currentSongChanged(SongData)), this, SLOT(onSongChanged(SongData)));
    connect(connector, SIGNAL(started()), this, SLOT(onStarted()));
    connect(connector, SIGNAL(paused()), this, SLOT(onPaused()));
    connect(connector, SIGNAL(stopped()), this, SLOT(onStopped()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete connector;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#ifdef Q_OS_SYMBIAN
    showFullScreen();
#elif defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    showMaximized();
#else
    show();
#endif
}

void MainWindow::onStarted()
{
    ui->play_bt->setText("Pause");
}

void MainWindow::onPaused()
{
    ui->play_bt->setText("Play");
}

void MainWindow::onStopped()
{
    ui->play_bt->setText("Play");
}

void MainWindow::on_play_bt_clicked()
{
    if (connector->currentStatus() == Xmms2Connector::Playing)
        connector->pausePlayback();
    else
        connector->startPlayback();
}

void MainWindow::on_stop_bt_clicked()
{
    connector->stopPlayback();
}

void MainWindow::on_next_bt_clicked()
{
    connector->jumpToNextSong();
}

void MainWindow::on_prev_bt_clicked()
{
    connector->jumpToPreviousSong();
}

void MainWindow::onSongChanged(SongData song)
{
    ui->song_info->setText("<b>"+song.artist+" - "+song.title+"</b>  <br> ["+song.album+"]");
}
