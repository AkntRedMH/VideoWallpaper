#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define NTDDI_VERSION 0x06000000
#include <windows.h>
#include <shellapi.h>

#include <QDebug>
#include <QListWidgetItem>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>

#include "videowindow.h"

#define VolumeRatio 10 // 滚动条和实际设置音量之间的倍数关系。
#define PlaybackRateRatio 10.0 // 滚动条和实际设置播放速度之间的倍数关系。

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool timeout_invisible = false; // 是否有其他程序全屏或最大化

private:
    Ui::MainWindow *ui;

    VideoWindow *videowindow;
    QStringList filepathsbackup;
    QSystemTrayIcon *systemtray;
    QMenu *menu;
    QAction *play;
    QAction *stop;
    QAction *mute;
    QAction *quit;
    QTimer *timer;
    bool timeout_playstate = false;

    void SetConfig();
    void GetConfig();
    void SetSystemTray();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void SetFilePaths(QStringList filepaths);
    void onTimeOut();

protected:
    void closeEvent(QCloseEvent*event);

private slots:
    void on_PB_add_clicked();
    void on_PB_remove_clicked();
    void on_PB_play_clicked();
    void on_PB_up_clicked();
    void on_PB_stop_clicked();
    void on_PB_next_clicked();
    void on_PB_mute_clicked();
    void on_HS_volume_valueChanged(int value);
    void on_LW_list_itemDoubleClicked(QListWidgetItem *item);
    void on_CB_mode_currentIndexChanged(int index);
    void on_CB_fit_currentIndexChanged(int index);
    void on_HS_rate_valueChanged(int value);
    void on_PB_startup_toggled(bool checked);
    void on_PB_timer_toggled(bool checked);
    void on_PB_displayoffstop_toggled(bool checked);
    void on_PB_alwaysondisplay_toggled(bool checked);
    void on_PB_github_clicked();
};
#endif // MAINWINDOW_H
