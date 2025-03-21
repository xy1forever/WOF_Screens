#include "screenshotwindows.h"
#include <QScreen>
#include <QGuiApplication>
#include <QFileDialog>
#include <QMessageBox>

ScreenshotWindow::ScreenshotWindow(QWidget *parent)
    : QWidget(parent), isSelecting(false),hotkey_2(new QHotkey(QKeySequence("Enter"), false,this))
{
    // 设置窗口为全屏
    setWindowState(Qt::WindowFullScreen);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // 初始化确认和取消按钮
    confirmButton = new QPushButton("√", this);
    cancelButton = new QPushButton("×", this);

    confirmButton->setStyleSheet("QPushButton { background-color: green; color: white; font-size: 20px; border-radius: 10px; }");
    cancelButton->setStyleSheet("QPushButton { background-color: red; color: white; font-size: 20px; border-radius: 10px; }");

    confirmButton->setFixedSize(40, 40);
    cancelButton->setFixedSize(40, 40);

    confirmButton->hide();
    cancelButton->hide();

    connect(confirmButton, &QPushButton::clicked, this, &ScreenshotWindow::confirmScreenshot);
    connect(cancelButton, &QPushButton::clicked, this, &ScreenshotWindow::cancelScreenshot);

    setMouseTracking(true); // 启用鼠标跟踪 解决设置鼠标样式无效的bug

    // 快速确认
    connect(hotkey_2, &QHotkey::activated,this,&ScreenshotWindow::fastTakeScreenshot);
}

void ScreenshotWindow::takeScreenshot()
{
    // 截取全屏
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        fullScreenshot = screen->grabWindow(0);
    }
    selectionRect = QRect(); // 将 selectionRect 置为空矩形
    show();
    startScreenshot();//注册快速确认快捷键
}

void ScreenshotWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    // 绘制全屏截图
    painter.drawPixmap(0, 0, fullScreenshot);

    if (isSelecting || isDragging || !selectionRect.isNull()) {
        // 绘制半透明遮罩（排除选中的矩形区域）
        painter.setBrush(QColor(0, 0, 0, 100)); // 半透明黑色
        painter.setPen(Qt::NoPen);

        // 绘制四个矩形区域（选中的矩形区域之外）
        QRegion fullRegion(rect()); // 整个窗口区域
        QRegion selectedRegion(selectionRect); // 选中的矩形区域
        QRegion maskedRegion = fullRegion.subtracted(selectedRegion); // 排除选中的区域

        painter.setClipRegion(maskedRegion);
        painter.drawRect(rect());

        // 绘制选中的矩形边框
        painter.setClipRegion(fullRegion); // 重置裁剪区域
        painter.setPen(QPen(Qt::blue, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(selectionRect);

        // 显示确认和取消按钮
        confirmButton->move(selectionRect.bottomRight() - QPoint(50, 50));
        cancelButton->move(selectionRect.bottomRight() - QPoint(100, 50));
        confirmButton->show();
        cancelButton->show();
    }
}

void ScreenshotWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (selectionRect.contains(event->pos())) {
            // 如果鼠标在矩形内，开始拖拽
            dragStartPos = event->pos();
            rectStartPos = selectionRect.topLeft();
            isDragging = true;
        } else {
            // 否则开始绘制新矩形
            startPoint = event->pos();
            isSelecting = true;
        }
    }
}

void ScreenshotWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (selectionRect.contains(event->pos(),false)) {
        // 鼠标在矩形内，设置光标为移动标志
        this->setCursor(Qt::SizeAllCursor);
    }
    else {
        // 否则恢复默认光标
        this->setCursor(Qt::ArrowCursor);
    }

    if (isSelecting) {
        selectionRect = QRect(startPoint, event->pos()).normalized();
        update();
    }
    else if (isDragging) {
        // 拖拽矩形阶段
        QPoint delta = event->pos() - dragStartPos;
        selectionRect.moveTo(rectStartPos + delta);
        update();
    }
}

void ScreenshotWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (isSelecting) {
            // 结束绘制矩形
            isSelecting = false;
        } else if (isDragging) {
            // 结束拖拽
            isDragging = false;
        }
    }
}


void ScreenshotWindow::confirmScreenshot()
{
    if (selectionRect.isEmpty()) {
        //未选矩形，而点保存
        return ;
    }
    if(1){
        // 截取选区内容
        QPixmap cropped = fullScreenshot.copy(selectionRect);

        // 检查 folderPath 是否有效
        if (folderPath.isEmpty()) {
            QMessageBox::warning(this, "Error", "Save folder path is not set!");
            return;
        }

        // 使用时间戳生成文件名
        // QString timestamp = QDateTime::currentDateTime().toString("yyyy年_MM月_dd日_HH时_mm分_ss秒");
        // 切换即可
        QString timestamp = QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss");
        QString fileName = folderPath + "/screenshot_" + timestamp + ".png"; // 完整文件路径

        // 如果文件夹不存在，则创建
        QDir dir(folderPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        cropped.save(fileName, "PNG");
    }
    else{
        // 截取选区内容
        QPixmap cropped = fullScreenshot.copy(selectionRect);

        // 弹出文件对话框保存截图
        QString fileName = QFileDialog::getSaveFileName(this, "Save Screenshot", "", "PNG Files (*.png);;JPEG Files (*.jpg);;All Files (*)");
        if (!fileName.isEmpty()) {
            if (cropped.save(fileName)) {
                QMessageBox::information(this, "Success", "Screenshot saved successfully!");
            } else {
                QMessageBox::warning(this, "Error", "Failed to save screenshot!");
            }
        }
    }

    confirmButton->hide();
    cancelButton->hide();
    this->hide();
    endScreenshot();
}

void ScreenshotWindow::cancelScreenshot()
{
    endScreenshot();
    confirmButton->hide();
    cancelButton->hide();
    this->hide();
}

void ScreenshotWindow::fastTakeScreenshot()
{
    if(isSelecting || isDragging || !selectionRect.isNull()){
        confirmScreenshot();
    }
}

void ScreenshotWindow::startScreenshot() {
    // 注册快捷键
    hotkey_2->setRegistered(true); // 启用快捷键
}

void ScreenshotWindow::endScreenshot() {
    // 注销快捷键
    hotkey_2->setRegistered(false); // 禁用快捷键
}
