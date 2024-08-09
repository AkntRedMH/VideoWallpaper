#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QToolTip>
#include <windows.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    videowindow = new VideoWindow(nullptr);

    GetConfig();

    SetSystemTray();
}

MainWindow::~MainWindow()
{
    delete videowindow;
    delete ui;
}

void MainWindow::SetConfig()
{
    QSettings config("Settings.ini", QSettings::IniFormat);

    config.setValue("mode", ui->CB_mode->currentIndex());
    config.setValue("fit", ui->CB_fit->currentIndex());
    config.setValue("rate", ui->HS_rate->value());

    if(videowindow->GetMuteState()) config.setValue("mute", true);
    else config.setValue("mute", false);

    config.setValue("volume", ui->HS_volume->value());

    // 存储列表中的路径
    config.remove("Paths");
    int index = 0;
    for(const QString &filepath : filepathsbackup)
    {
        config.setValue("Paths/"+QString::number(index), filepath);
        index++;
    }
}

void MainWindow::GetConfig()
{
    QSettings config("Settings.ini", QSettings::IniFormat);
    int temp; // 临时变量

    temp = config.value("mode", VideoWindow::CurrentItemInLoop).toInt();
    videowindow->SetPlaybackMode(static_cast<VideoWindow::PlaybackMode>(temp));
    ui->CB_mode->setCurrentIndex(temp);

    temp = config.value("fit", VideoWindow::KeepAspectRatioByExpanding).toInt();
    videowindow->SetAspectRatioMode(static_cast<VideoWindow::AspectRatioMode>(temp));
    ui->CB_fit->setCurrentIndex(temp);

    temp = config.value("rate", 10).toInt();
    videowindow->SetPlaybackRate(temp);
    ui->HS_rate->setValue(temp);

    if(config.value("mute", true).toBool())
    {
        videowindow->VideoMute();
        ui->PB_mute->setIcon(QIcon(":/icons/mute"));
    }
    else
    {
        videowindow->VideoUnmute();
        ui->PB_mute->setIcon(QIcon(":/icons/unmute"));
    }

    temp = config.value("volume", 0).toInt();
    videowindow->SetVideoVolume(temp);
    ui->HS_volume->setValue(temp);

    // 读取路径到列表
    QStringList filepaths;
    QFileInfo fileinfo;
    QString filepath;

    int index = 0;
    while(config.contains("Paths/" + QString::number(index)))
    {
        filepath = config.value("Paths/" + QString::number(index)).toString();

        fileinfo.setFile(filepath);
        if(fileinfo.exists() && fileinfo.isFile())
        {
            filepaths.append(filepath);
        }

        index++;
    }

    filepathsbackup.clear();
    SetFilePaths(filepaths);
}

void MainWindow::SetSystemTray()
{
    systemtray = new QSystemTrayIcon(this);
    menu = new QMenu(this);

    // 设置系统托盘提示信息、托盘图标
    systemtray->setToolTip("视频壁纸");
    systemtray->setIcon(QIcon(":/icons/app"));

    // 设置系统托盘的上下文菜单
    systemtray->setContextMenu(menu);

    play = new QAction(this);
    connect(play, &QAction::triggered, this, &MainWindow::on_PB_play_clicked);
    menu->addAction(play);
    if(videowindow->GetVideoState()!=VideoWindow::PlayingState) play->setText("播放视频");
    else play->setText("暂停视频");

    stop = new QAction("停止视频", this);
    connect(stop, &QAction::triggered, this, &MainWindow::on_PB_stop_clicked);
    menu->addAction(stop);

    mute = new QAction(this);
    connect(mute, &QAction::triggered, this, &MainWindow::on_PB_mute_clicked);
    menu->addAction(mute);
    if(videowindow->GetMuteState()) mute->setText("开启声音");
    else mute->setText("关闭声音");

    quit = new QAction("退出程序", this);
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    menu->addAction(quit);

    connect(systemtray, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);

    // 显示系统托盘
    systemtray->show();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::DoubleClick:
            if(this->isHidden()) this->show();
            else this->hide();
        break;

        default: break;
    }
}

void MainWindow::SetFilePaths(QStringList filepaths)
{
    QFileInfo fileinfo;

    filepathsbackup.append(filepaths);

    videowindow->VideoAdd(filepaths);

    // 只保留路径中的文件名
    for(QString &filepath : filepaths)
    {
        fileinfo.setFile(filepath);
        filepath = fileinfo.fileName();
    }

    ui->LW_list->addItems(filepaths);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    SetConfig();

    this->hide();
    event->ignore();
}

void MainWindow::on_PB_add_clicked()
{
    QStringList filepaths = QFileDialog::getOpenFileNames(this, tr("打开文件"), ".", tr("全部文件 (*.*)"));

    SetFilePaths(filepaths);
}

void MainWindow::on_PB_remove_clicked()
{
    int index = ui->LW_list->currentRow();

    filepathsbackup.removeAt(index);

    videowindow->VideoRemove(index);

    QListWidgetItem *item = ui->LW_list->takeItem(index);
    delete item;
}

void MainWindow::on_PB_play_clicked()
{
    if(videowindow->GetVideoState()==VideoWindow::PlayingState)
    {
        videowindow->VideoPause();
        ui->PB_play->setIcon(QIcon(":/icons/play"));
        play->setText("播放视频");
    }
    else
    {
        videowindow->VideoPlay(ui->HS_volume->value());
        ui->PB_play->setIcon(QIcon(":/icons/pause"));
        play->setText("暂停视频");
    }
}

void MainWindow::on_PB_up_clicked()
{
    videowindow->VideoUp();
}

void MainWindow::on_PB_stop_clicked()
{
    if(videowindow->GetVideoState()!=VideoWindow::StoppedState)
    {
        videowindow->VideoStop();
        ui->PB_play->setIcon(QIcon(":/icons/play"));
        play->setText("播放视频");
    }
}

void MainWindow::on_PB_next_clicked()
{
    videowindow->VideoNext();
}

void MainWindow::on_PB_mute_clicked()
{
    if(videowindow->GetMuteState())
    {
        videowindow->VideoUnmute();
        ui->PB_mute->setIcon(QIcon(":/icons/unmute"));
        mute->setText("关闭声音");
    }
    else
    {
        videowindow->VideoMute();
        ui->PB_mute->setIcon(QIcon(":/icons/mute"));
        mute->setText("开启声音");
    }
}

void MainWindow::on_HS_volume_valueChanged(int value)
{
    QPoint pos = ui->HS_volume->rect().center();
    pos = ui->HS_volume->mapToGlobal(pos);

    videowindow->SetVideoVolume(value);
    QToolTip::showText(pos, QString::number(value*VolumeRatio) + '%', ui->HS_volume);
}

void MainWindow::on_LW_list_itemDoubleClicked(QListWidgetItem *item)
{
    videowindow->SetPlayIndex(ui->LW_list->row(item));

    if(videowindow->GetVideoState()!=VideoWindow::PlayingState)
    {
        videowindow->VideoPlay(ui->HS_volume->value());
        ui->PB_play->setIcon(QIcon(":/icons/pause"));
        play->setText("暂停视频");
    }
}

void MainWindow::on_CB_mode_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0: videowindow->SetPlaybackMode(VideoWindow::CurrentItemOnce); break;
        case 1: videowindow->SetPlaybackMode(VideoWindow::Loop); break;
        case 2: videowindow->SetPlaybackMode(VideoWindow::CurrentItemInLoop); break;
        case 3: videowindow->SetPlaybackMode(VideoWindow::Random); break;
    }
}

void MainWindow::on_CB_fit_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0: videowindow->SetAspectRatioMode(VideoWindow::IgnoreAspectRatio); break;
        case 1: videowindow->SetAspectRatioMode(VideoWindow::KeepAspectRatio); break;
        case 2: videowindow->SetAspectRatioMode(VideoWindow::KeepAspectRatioByExpanding); break;
    }
}

void MainWindow::on_HS_rate_valueChanged(int value)
{
    QPoint pos = ui->HS_rate->rect().center();
    pos = ui->HS_rate->mapToGlobal(pos);

    videowindow->SetPlaybackRate(value);
    QToolTip::showText(pos, QString::number(value/PlaybackRateRatio) + 'x', ui->HS_rate);
}
