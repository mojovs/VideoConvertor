#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cmd/CmdProc.h>

#include <QMap>
#include "OptionDlg.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:

    void on_btnFolder_clicked();

    void on_btnFile_clicked();

    void on_btnOutputDir_clicked();

    void on_listWidget_customContextMenuRequested(const QPoint &pos);

    void on_btnStart_clicked();

    void on_actionClear_triggered();

    void on_actionOptions_triggered();

  private:
    CmdProc *proc;                     //线程
    QMap<QString, QString> encoderMap; //编码器列表
    QMap<QString, QString> decoderMap; //解码器列表
    QStringList fileList;              //文件列表
    QString curPath;                   //当前路径
    QString outputDir;                 //输出路径
    int crf              = 28;         //压缩级别
    QString preset_level = "fast";     //压缩速度

    /**
     * @brief 递归
     * @param dir 路径
     * @param list[out] 输出的文件路径
     */
    void getFilesR(const QString dir, QStringList &list);
    void uiChangeBySet(const OptionDlg::Options options);
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
