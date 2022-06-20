#include "OptionDlg.h"
#include "ui_OptionDlg.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>
#pragma execution_character_set("utf-8")
OptionDlg::OptionDlg(QWidget *parent) : QDialog(parent), ui(new Ui::OptionDlg)
{
    ui->setupUi(this);
    proc = new CmdProc();
}

OptionDlg::~OptionDlg()
{
    delete ui;
    proc->deleteLater();
}

void OptionDlg::on_btnCheckEncoders_clicked()
{
    proc->checkEncoders();
    //解析出编码器列表
    QString cmdOutput = proc->stdOutput();
    //开始正则
    QString regStr = "[VAS.][F.][S.]..[D.]\\s(\\w+)\\s*([^\\r^\\n]+)";
    QRegExp reg(regStr);
    int pos = 0;
    //循环遍历
    while ((pos = reg.indexIn(cmdOutput, pos)) != -1)
    {
        QString key     = reg.cap(1).trimmed();
        QString value   = reg.cap(2).trimmed();
        encoderMap[key] = value;
        pos += reg.matchedLength();
    }
    //添加到界面
    QMapIterator<QString, QString> i(encoderMap);
    while (i.hasNext())
    {
        ui->comboEncoders->addItem(i.next().key());
    }
}

void OptionDlg::on_comboEncoders_currentIndexChanged(int index)
{
    m_options.encoder = ui->comboEncoders->currentText();
}

void OptionDlg::on_comboDecoders_currentIndexChanged(int index)
{
    m_options.decoder = ui->comboDecoders->currentText();
}

void OptionDlg::on_btnCheckDecoders_clicked()
{
    proc->checkEncoders();
    //解析出编码器列表
    QString cmdOutput = proc->stdOutput();
    //开始正则
    QString regStr = "[VAS.][F.][S.]..[D.]\\s(\\w+)\\s*([^\\r^\\n]+)";
    QRegExp reg(regStr);
    int pos = 0;
    //循环遍历
    while ((pos = reg.indexIn(cmdOutput, pos)) != -1)
    {
        QString key     = reg.cap(1).trimmed();
        QString value   = reg.cap(2).trimmed();
        encoderMap[key] = value;
        pos += reg.matchedLength();
    }
    //添加到界面
    QMapIterator<QString, QString> i(encoderMap);
    while (i.hasNext())
    {
        ui->comboDecoders->addItem(i.next().key());
    }
}

/**
 * @brief 添加一个输出路径
 */
void OptionDlg::on_btnOutputDir_clicked()
{
    //获取整个路径
    m_options.outputDir = QFileDialog::getExistingDirectory(this, "选择一个文件夹", QDir::currentPath());
    if (m_options.outputDir.isEmpty())
    {
        QMessageBox::information(this, "消息", QString("路径错误,%1").arg(m_options.outputDir));
        return;
    }
    //设置界面显示
    ui->lineEditOutputDir->setText(m_options.outputDir);
}

void OptionDlg::on_btnSetResolution_clicked()
{
    m_options.width  = ui->lineEditWidth->text().toInt();
    m_options.height = ui->lineEditHeight->text().toInt();
}

void OptionDlg::on_btnCheckHwAccelers_clicked()
{
    //清空数据
    hwAccelerList.clear();
    ui->comboHwAccelers->clear(); //清空界面
    proc->checkHwAccelers();      //检测
    QString cmdOutput = proc->stdOutput();
    //开始正则
    QString regStr = "(\\w*)\\r\\n";
    QRegExp reg(regStr);
    int pos = 0;
    //循环遍历
    while ((pos = reg.indexIn(cmdOutput, pos)) != -1)
    {
        QString key = reg.cap(1).trimmed();
        pos += reg.matchedLength();
        hwAccelerList.append(key);
    }
    //添加到界面
    foreach (QString key, hwAccelerList)
    {
        ui->comboHwAccelers->addItem(key);
    }
}

void OptionDlg::on_comboHwAccelers_currentIndexChanged(int index)
{
    m_options.hwacceler = ui->comboHwAccelers->currentText();
}

void OptionDlg::on_btnConvertQuality_clicked()
{
    int level = ui->lineEditQuality->text().toInt();
    if (level < 0)
    {
        m_options.crf = 0;
    }
    else if (level <= 58 && level >= 0)
    {
        m_options.crf = level;
    }
    else
    {
        m_options.crf = 58;
    }
}

void OptionDlg::on_btnBitrate_clicked()
{
    m_options.bitrate = ui->lineEditBitrate->text().toInt();
}

void OptionDlg::on_buttonBox_accepted()
{
    //同步到信息存储结构体里
    m_options.height       = ui->lineEditHeight->text().toInt();
    m_options.width        = ui->lineEditWidth->text().toInt();
    m_options.bitrate      = ui->lineEditBitrate->text();
    m_options.crf          = ui->lineEditQuality->text().toInt();
    m_options.decoder      = ui->comboDecoders->currentText();
    m_options.encoder      = ui->comboEncoders->currentText();
    m_options.hwacceler    = ui->comboHwAccelers->currentText();
    m_options.outputDir    = ui->lineEditOutputDir->text();
    m_options.preset_level = ui->comboPreset->currentData().toString();

    //同步设置到文件
    write2Json();
}

void OptionDlg::write2Json()
{
    //打开一个文件
    QFile aFile("./setting.json");
    if (!aFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        QMessageBox::critical(this, "致命", "打开设置文件失败!");
        exit(-1);
    }

    //构建json结构体
    QJsonDocument doc;
    QJsonObject srcObject;
    srcObject.insert("height", m_options.height);
    srcObject.insert("width", m_options.width);
    srcObject.insert("bitrate", m_options.bitrate);
    srcObject.insert("crf", m_options.crf);
    srcObject.insert("decoder", m_options.decoder);
    srcObject.insert("encoder", m_options.encoder);
    srcObject.insert("hwacceler", m_options.hwacceler);
    srcObject.insert("outputDir", m_options.outputDir);
    srcObject.insert("preset_level", m_options.preset_level);
    doc.setObject(srcObject); //添加到文档

    //写入到文件
    aFile.write(doc.toJson());
    aFile.close();
}

void OptionDlg::initConvertSpeedCombo()
{
    //为压缩速度，设置数据
    for (int i = 0; i < ui->comboPreset->count(); i++)
    {
        QString text = ui->comboPreset->itemText(i);
        if ("快" == text)
        {
        }
        else if ("中等" == text)
        {
        }
        else if ("慢" == text)
        {
        }
        else
        {
        }
    }
}
