#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "videowindow.h"
#include <QListWidgetItem>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    VideoWindow *videowindow;
    QStringList filepathsbackup;
    QSystemTrayIcon *systemtray;
    QMenu *menu;
    QAction *play;
    QAction *stop;
    QAction *quit;

    void SetConfig();
    void GetConfig();
    void SetSystemTray();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void SetFilePaths(QStringList filepaths);

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
};
#endif // MAINWINDOW_H
