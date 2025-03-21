#ifndef SCREENSHOTWINDOW_H
#define SCREENSHOTWINDOW_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QHotKey>
#include <QApplication>
class ScreenshotWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenshotWindow(QWidget *parent = nullptr);

public slots:
    void takeScreenshot();
    void fastTakeScreenshot();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void confirmScreenshot();
    void cancelScreenshot();
private:
    void startScreenshot();
    void endScreenshot();

private:
    QPixmap fullScreenshot; // 全屏截图
    QRect selectionRect;     // 用户选择的矩形区域
    QPoint startPoint;       // 鼠标按下时的起点
    bool isSelecting = false;        // 是否正在选择区域
    QPushButton *confirmButton; // 确认按钮
    QPushButton *cancelButton;  // 取消按钮

    bool isDragging = false;         // 是否正在拖拽矩形
    QPoint dragStartPos;     // 拖拽起始点
    QPoint rectStartPos;     // 矩形初始位置

    QHotkey *hotkey_2;//用于快速确认
public:
    QString folderPath;//
};

#endif // SCREENSHOTWINDOW_H
