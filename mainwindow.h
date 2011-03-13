
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "xmms2connector.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{

    Q_OBJECT
    Ui::MainWindow *ui;
    Xmms2Connector *connector;

public:

    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    void setOrientation(ScreenOrientation orientation);
    void showExpanded();

private slots:
    void onSongChanged(SongData song);
    void onStarted();
    void onPaused();
    void onStopped();
    void on_play_bt_clicked();
    void on_stop_bt_clicked();
    void on_next_bt_clicked();
    void on_prev_bt_clicked();
};

#endif // MAINWINDOW_H
