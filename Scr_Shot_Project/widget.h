#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QHotKey>
#include "screenshotwindows.h"
#include <QFileDialog>

#include <QSettings>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QSystemTrayIcon *SysIcon;
    QAction *min; // 最小化
    QAction *max; // 最大化
    QAction *restor; // 恢复
    QAction *quit; // 退出
    QMenu *menu;
    QHotkey *hotkey_1;//用于截取屏幕
    ScreenshotWindow* ssw;
    int tray_times = 0;//tray icon弹出次数

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void setShortcut_1(const QKeySequence &sequence);
    void on_pushButton_clicked();
    void on_checkBox_toggled(bool checked);

    void on_pushButton_2_clicked();

private:
    QSettings Settings; // 用于存储配置
    void loadAll();
};
#endif // WIDGET_H
