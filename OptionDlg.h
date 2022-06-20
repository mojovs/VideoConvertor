#ifndef OPTIONDLG_H
#define OPTIONDLG_H

#include <QDialog>
#include "cmd/CmdProc.h"
#include <QMap>

namespace Ui
{
class OptionDlg;
}

class OptionDlg : public QDialog
{
    Q_OBJECT

  public:
    //包含设置信息
    typedef struct _Options
    {
        QString encoder;
        QString decoder;
        QString hwacceler;
        QString bitrate;
        int width;
        int height;
        QString preset_level;
        int crf;
        QString outputDir;
    } Options;

    Options m_options; //设置选项

    explicit OptionDlg(QWidget *parent = nullptr);
    ~OptionDlg();

  private slots:
    void on_btnCheckEncoders_clicked();

    void on_comboEncoders_currentIndexChanged(int index);

    void on_comboDecoders_currentIndexChanged(int index);

    void on_btnCheckDecoders_clicked();

    void on_btnOutputDir_clicked();

    void on_btnSetResolution_clicked();

    void on_btnCheckHwAccelers_clicked();

    void on_comboHwAccelers_currentIndexChanged(int index);

    void on_btnConvertQuality_clicked();

    void on_btnBitrate_clicked();

    void on_buttonBox_accepted();

  private:
    Ui::OptionDlg *ui;
    CmdProc *proc; // cmd进程
    QMap<QString, QString> encoderMap;
    QMap<QString, QString> decoderMap;
    QStringList hwAccelerList; //硬件加速选项图
    /**
     * @brief 将界面的数据写入到文件
     */
    void write2Json();
    /**
     * @brief 初始化转换速度ui组合框数据
     */
    void initConvertSpeedCombo();
};

#endif // OPTIONDLG_H
