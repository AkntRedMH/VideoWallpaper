#include "videowindow.h"
#include "mainwindow.h"
#include <QDebug>
#include <QVBoxLayout>
#include <windows.h>

HWND VideoWindow::hWorkerW; // 存储第二个WorkerW的句柄

VideoWindow::VideoWindow(QWidget *parent) : QWidget(parent)
{
    // 放到一开始，避免屏幕闪烁。
    SetWallpaper();

    this->setWindowFlag(Qt::FramelessWindowHint);
    this->showFullScreen();

    videowidget = new QVideoWidget(this);
    videowidget->setAspectRatioMode(Qt::IgnoreAspectRatio);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(videowidget);
    layout->setMargin(0); // 移除布局的边距
    setLayout(layout);

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playlist);
    player->setVideoOutput(videowidget);

    player->setMuted(true);
}

VideoWindow::~VideoWindow()
{
    // 结束应用时隐藏WorkerW，因为WorkerW的壁纸已经无法显示。
    ShowWindow(hWorkerW, SW_HIDE);
}

void VideoWindow::VideoAdd(QStringList filePaths)
{
    for(const QString &item : filePaths)
    {
        playlist->addMedia(QUrl::fromLocalFile(item));
    }
}

void VideoWindow::VideoRemove(int index)
{
    playlist->removeMedia(index);
}

void VideoWindow::VideoPlay(int volume = -1)
{
    ShowWindow(hWorkerW, SW_SHOW);
    if(volume!=-1) player->setVolume(volume);
    player->play();
}

void VideoWindow::VideoPause()
{
    player->pause();
}

void VideoWindow::VideoUp()
{
    playlist->previous();
}

void VideoWindow::VideoStop()
{
    // 利用ShowWindow(hWorkerW, SW_HIDE);控制壁纸显隐
    // this->hide()作用效果有问题，隐藏后this->show()无效。
    ShowWindow(hWorkerW, SW_HIDE);
    player->stop();
}

void VideoWindow::VideoNext()
{
    playlist->next();
}

void VideoWindow::VideoMute()
{
    player->setMuted(true);
}

void VideoWindow::VideoUnmute()
{
    player->setMuted(false);
}

VideoWindow::State VideoWindow::GetVideoState()
{
    QMediaPlayer::State state = player->state();

    switch(state)
    {
        case QMediaPlayer::StoppedState: return StoppedState;
        case QMediaPlayer::PlayingState: return PlayingState;
        case QMediaPlayer::PausedState: return PausedState;
    }

    return ErrorState;
}

VideoWindow::MediaStatus VideoWindow::GetMediaState()
{
    QMediaPlayer::MediaStatus mediastate = player->mediaStatus();

    switch(mediastate)
    {
        case QMediaPlayer::UnknownMediaStatus: return UnknownMediaStatus;
        case QMediaPlayer::NoMedia: return NoMedia;
        case QMediaPlayer::LoadingMedia: return LoadingMedia;
        case QMediaPlayer::LoadedMedia: return LoadedMedia;
        case QMediaPlayer::StalledMedia: return StalledMedia;
        case QMediaPlayer::BufferingMedia: return BufferingMedia;
        case QMediaPlayer::BufferedMedia: return BufferedMedia;
        case QMediaPlayer::EndOfMedia: return EndOfMedia;
        case QMediaPlayer::InvalidMedia: return InvalidMedia;
    }

    return ErrorMediaStatus;
}

bool VideoWindow::GetMuteState()
{
    bool state = player->isMuted();
    return state;
}

void VideoWindow::SetPlayIndex(int index)
{
    playlist->setCurrentIndex(index);
}

void VideoWindow::SetVideoVolume(int volume)
{
    player->setVolume(volume);
}

void VideoWindow::SetPlaybackMode(PlaybackMode mode)
{
    switch(mode)
    {
        case CurrentItemOnce: playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce); break;
        case CurrentItemInLoop: playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop); break;
        case Sequential: playlist->setPlaybackMode(QMediaPlaylist::Sequential); break;
        case Loop: playlist->setPlaybackMode(QMediaPlaylist::Loop); break;
        case Random: playlist->setPlaybackMode(QMediaPlaylist::Random); break;
    }
}

void VideoWindow::SetPlaybackRate(float rate)
{
    // 在设置前后暂停，减少闪烁。
    player->pause();
    player->setPlaybackRate(rate);
    player->play();
}

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM Lparam)
{
    HWND hDefView = FindWindowEx(hwnd, 0, L"SHELLDLL_DefView", 0);

    if(hDefView != 0)
    {
        // 找它的下一个窗口，类名为WorkerW，隐藏它
        VideoWindow::hWorkerW = FindWindowEx(0, hwnd, L"WorkerW", 0);
        ShowWindow(VideoWindow::hWorkerW, SW_HIDE);

        return FALSE;
    }

    // 抑制“该变量未使用”警告
    // https://blog.csdn.net/weixin_39445116/article/details/115229042
    Q_UNUSED(Lparam);

    return TRUE;
}

// 实现动态壁纸的简单DEMO
// https://space.bilibili.com/39665558
void VideoWindow::SetWallpaper()
{
    HWND hProgman = FindWindow(L"Progman", 0);              // 找到PM窗口
//    SendMessageTimeout(hProgman, 0x52C, 0, 0, 0, 100, 0);	// 给它发特殊消息
    SendMessage(hProgman, 0x052C, 0x000D, 0x0001);          // 修改消息，参考壁纸引擎
//    SetParent((HWND)this->winId(), hProgman);               // 将视频窗口设置为PM的子窗口
    EnumWindows(EnumWindowsProc, 0);                        // 找到第二个WorkerW窗口并隐藏它
    SetParent((HWND)this->winId(), hWorkerW);               // 将视频窗口设置为第二个WorkerW的子窗口
}
