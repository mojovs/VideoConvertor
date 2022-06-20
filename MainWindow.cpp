#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMapIterator>
#include <QComboBox>
#include <QCoreApplication>
#include <QMenu>
#include <QFileInfo>
#include "OptionDlg.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    proc = new CmdProc;

    ui->setupUi(this);
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

MainWindow::~MainWindow()
{
    delete proc;
    delete ui;
}
void MainWindow::on_btnFolder_clicked()
{
    //获取整个路径
    curPath = QFileDialog::getExistingDirectory(this, "选择一个文件夹", QDir::currentPath());
    if (curPath.isEmpty())
    {
        QMessageBox::information(this, "消息", QString("路径错误,%1").arg(curPath));
        return;
    }
    //获取该路径下所有视频文件
    getFilesR(curPath, fileList);
    //添加到文件显示区
    ui->listWidget->clear(); //清空
    ui->listWidget->addItems(fileList);
}

void MainWindow::on_btnFile_clicked()
{
    //添加文件到列表
    fileList << QFileDialog::getOpenFileNames(this, "选择文件", QCoreApplication::applicationDirPath(), "mp4文件(*.mp4);;");
    //添加到文件显示区
    ui->listWidget->clear(); //清空
    ui->listWidget->addItems(fileList);
}

void MainWindow::on_btnOutputDir_clicked()
{
    outputDir = QFileDialog::getExistingDirectory(this, "选择一个文件夹", QDir::currentPath());
}

void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    menu->addAction(ui->actionDelete);
    menu->addAction(ui->actionClear);

    //执行菜单
    menu->exec(QCursor::pos());
    menu->deleteLater();
}

void MainWindow::on_btnStart_clicked()
{
    QStringList argsIn, argsOut;
    argsIn << "-y"
           << "-vsync"
           << "passthrough"
           << "-hwaccel"
           << "cuda"
           << "-hwaccel_output_format"
           << "cuda"
           << "-i";
    //获取编码器
    QString height     = ui->lineEditHeight->text();
    QString width      = ui->lineEditWidth->text();
    QString resolution = "scale_cuda=" + width + ":" + height;
    QString biterate   = ui->lineEditBiterate->text();
    argsOut << "-vf" << resolution << "-c:a"
            << "copy"
            << "-c:v"
            << "hevnc_evenc"
            << "-b:v" << biterate << "-crf" << QString::number(crf) << "-preset" << preset_level;
    //循环打包文件
    foreach (QString fileName, fileList)
    {
        proc->convertVideo(fileName, "test.mp4", argsIn, argsOut);
    }
}

void MainWindow::getFilesR(const QString dir, QStringList &list)
{
    QDir aDir(dir);
    if (!aDir.exists())
    {
        QMessageBox::information(this, "消息", QString("路径错误,%1").arg(dir));
        return;
    }
    QFileInfoList infoList = aDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach (QFileInfo fileInfo, infoList)
    {
        if (fileInfo.isDir()) //如果文件夹
        {
            //再次调用
            getFilesR(fileInfo.absoluteFilePath(), list);
        }
        if (fileInfo.isFile())
        {
            //如果是mp4文件
            if ("mp4" == fileInfo.suffix())
            {
                list.append(fileInfo.filePath());
            }
        }
    }
}

void MainWindow::uiChangeBySet(const OptionDlg::Options options)
{
    ui->txtCrf->setText(QString::number(options.crf));
    ui->lineEditBiterate->setText(options.bitrate);
}

void MainWindow::on_actionClear_triggered()
{
    //清空所有文件
    ui->listWidget->clear();
    fileList.clear();
}

void MainWindow::on_actionOptions_triggered()
{
    //运行设置菜单
    OptionDlg *optionDlg = new OptionDlg(this);

    //执行
    int ret = optionDlg->exec();
    if (ret == QDialog::Accepted)
    {
    }
}
