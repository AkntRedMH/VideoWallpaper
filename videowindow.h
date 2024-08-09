#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <windows.h>

#define VolumeRatio 10 // 滚动条和实际设置音量之间的倍数关系。

class VideoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWindow(QWidget *parent = nullptr);
    ~VideoWindow();

    enum State
    {
        StoppedState,
        PlayingState,
        PausedState,
        ErrorState
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
        Fill,
        Fit,
        Stretch
    };

    static HWND hWorkerW;

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
    void SetPlayIndex(int index);
    void SetVideoVolume(int volume);
    void SetPlaybackMode(PlaybackMode mode);
    void SetPlaybackRate(float rate);

private:
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videowidget;

    void SetWallpaper();
};

#endif // VIDEOWINDOW_H
