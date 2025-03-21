#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    ,hotkey_1(new QHotkey(this))
    ,ssw(new ScreenshotWindow())
    ,Settings("NoCompany", "WOF_Screen")
{
    ui->setupUi(this);
    loadAll();//载入设置

    // 定义托盘图标
    menu = new QMenu(this);
    QIcon icon(":/asserts/window-icon.png");
    SysIcon = new QSystemTrayIcon(this);
    SysIcon->setIcon(icon);
    SysIcon->setToolTip("Qt托盘");

    // 设置菜单的action
    min = new QAction("窗口最小化", this);
    connect(min,&QAction::triggered,this,&Widget::hide);
    max = new QAction("窗口最大化", this);
    connect(max,&QAction::triggered,this,&Widget::showMaximized);
    restor = new QAction("恢复原来的样子", this);
    connect(restor, &QAction::triggered, this, &Widget::showNormal);
    quit = new QAction("退出", this);
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);

    // 将action添加到菜单中
    menu->addAction(min);
    menu->addAction(max);
    menu->addAction(restor);
    menu->addSeparator();
    menu->addAction(quit);

    SysIcon->setContextMenu(menu);
    SysIcon->show();



    // 连接并注册截屏快捷键
    connect(this->ui->hotkeyCheckbox_1, &QCheckBox::toggled,
            this->hotkey_1, &QHotkey::setRegistered);
    // 操作图标
    connect(SysIcon, &QSystemTrayIcon::activated, this, &Widget::on_activatedSysTrayIcon);
    // 修改快捷键
    connect(this->ui->keySequenceEdit1, &QKeySequenceEdit::keySequenceChanged,
            this, &Widget::setShortcut_1);
    // 使用快捷键截屏
    connect(hotkey_1, &QHotkey::activated,ssw,&ScreenshotWindow::takeScreenshot);

    close();
}

Widget::~Widget()
{
    delete ui;
}

// 重写关闭事件
void Widget::closeEvent(QCloseEvent *event)
{
    if(SysIcon->isVisible()){
        hide();
        if(tray_times % 10 == 0){
            SysIcon->showMessage("Qt托盘", "程序已最小化到托盘");
        }
        tray_times ++;
        event->ignore();
    }
    else{
        event->accept();
    }

}

// 隐藏图标操作 槽函数
void Widget::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::DoubleClick:
        show();
        break;
    default:
        break;
    }
}

// 设置快捷键 槽函数
void Widget::setShortcut_1(const QKeySequence &sequence)
{
    this->hotkey_1->setShortcut(sequence, false);

    Settings.setValue("Hotkey/Sequence", sequence.toString());
}

//选择文件夹 槽函数
void Widget::on_pushButton_clicked()
{
    ssw->folderPath = nullptr;
    ssw->folderPath = QFileDialog::getExistingDirectory(this, "选择文件夹", QDir::homePath());
    if (!ssw->folderPath.isEmpty()) {
        qDebug() << "选择的文件夹路径:" << ssw->folderPath; // 打印路径到控制台

        Settings.setValue("File/folderPath",ssw->folderPath);
    }
    else{
        qDebug() << "folderPath is empty";


    }
}

// 用于创建快捷方式在start中实现开机自启动
void Widget::on_checkBox_toggled(bool checked)
{
    //将快捷方式添加到注册表
    const QString exePath = QCoreApplication::applicationFilePath();
    // 此处MyApp_Test.lnk需要修改
    const QString startupPath = QDir::homePath() + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/WOF_Screen.lnk";
    if(checked){
        QFile::link(exePath,startupPath);
        // 需要错误响应*********
    }
    else{
        //取消开机自启动，需要删除名称为本项目的lnk文件
        QFile::remove(startupPath);
    }
}

void Widget::loadAll()
{
    QString sequenceStr = Settings.value("Hotkey/Sequence").toString();
    if (!sequenceStr.isEmpty()) {
        QKeySequence sequence(sequenceStr);
        ui->keySequenceEdit1->setKeySequence(sequence); // 设置到控件中
        hotkey_1->setShortcut(sequence, true); // 注册热键
    }
    QString floder = Settings.value("File/folderPath").toString();
    if (!floder.isEmpty()) {
        ssw->folderPath = floder;
        this->ui->label_floder->setText("默认保存文件夹：\n"+floder);
    }
}


void Widget::on_pushButton_2_clicked()
{
    Settings.clear(); // 清空所有配置
    this->ui->label_floder->setText(""); // 清空选择文件夹的lebal
    hotkey_1->setRegistered(false);// 取消注册热键
    ui->keySequenceEdit1->clear(); // 清空快捷键输入框
}

