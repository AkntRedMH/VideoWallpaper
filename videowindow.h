#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>

enum State
{
    StoppedState,
    PlayingState,
    PausedState,
    ErrorState,
};
enum MediaStatus
{
    UnknownMediaStatus,
    NoMedia,
    LoadingMedia,
    LoadedMedia,
    StalledMedia,
    BufferingMedia,
    BufferedMedia,
    EndOfMedia,
    InvalidMedia,
    ErrorMediaStatus
};
enum PlaybackMode
{
    CurrentItemOnce,
    CurrentItemInLoop,
    Sequential,
    Loop,
    Random
};
enum AspectRatioMode
{
    IgnoreAspectRatio,
    KeepAspectRatio,
    KeepAspectRatioByExpanding
};

class VideoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWindow(QWidget *parent = nullptr);
    ~VideoWindow();

    HWND hWorkerW; // 存储第二个WorkerW的句柄

    void VideoAdd(QStringList filePaths);
    void VideoRemove(int index);
    void VideoPlay(int volume = -1);
    void VideoPause();
    void VideoUp();
    void VideoStop();
    void VideoNext();
    void VideoMute();
    void VideoUnmute();
    void VideoVolume();

    State GetVideoState();
    MediaStatus GetMediaState();
    bool GetMuteState();
    int GetPlayIndex();
    void SetPlayIndex(int index);
    void SetVideoVolume(int volume);
    void SetPlaybackMode(PlaybackMode mode);
    void SetPlaybackRate(float rate);
    void SetAspectRatioMode(AspectRatioMode mode);

private:
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videowidget;

    void SetWallpaper();
};

#endif // VIDEOWINDOW_H
