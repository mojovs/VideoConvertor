#include "CmdProc.h"
#include <QDebug>
#include <QRegExp>
#include <QDir>
#define CMD ""
#define POWERSHELL ("powershell")
#define DISM ("dism")
#define ADB ("adb-tools/adb.exe")
#define FFMPEG ("ffmpeg/ffmpeg.exe")
/*------ 构造 ------*/
CmdProc::CmdProc(QObject *parent) : QProcess(parent)
{
    initSignals();
}

/*------ 执行命令------*/
void CmdProc::execute(const QString &progPath, const QStringList &args)
{
    QStringList adbArgs;
    adbArgs << args;
    qDebug() << adbArgs.join(" ");
    start(progPath, adbArgs);
}

/*------ 查看编码器 ------*/
void CmdProc::checkEncoders()
{
    QStringList args;
    args << "-encoders";
    execute(FFMPEG, args);
    waitForFinished();
}

void CmdProc::checkDecoders()
{
    QStringList args;
    args << "-decoders";
    execute(FFMPEG, args);
    waitForFinished();
}

void CmdProc::checkHwAccelers()
{
    QStringList args;
    args << "-hwaccels";
    execute(FFMPEG, args);
    waitForFinished();
}

void CmdProc::convertVideo(const QString file, const QString outputFile, const QStringList argsIn, const QStringList argsOut)
{
    QStringList args;
    args << argsIn << file << argsOut << outputFile;
    execute(FFMPEG, args);
    waitForFinished();
}

const QString &CmdProc::stdOutput() const
{
    return m_stdOutput;
}

void CmdProc::setStdOutput(const QString &newStdOutput)
{
    m_stdOutput = newStdOutput;
}

/*------ 绑定所有信号------*/
void CmdProc::initSignals()
{
    //执行错误
    connect(this, &QProcess::errorOccurred, this,
            [this](QProcess::ProcessError error)
            {
                if (QProcess::FailedToStart == error)
                {
                    emit cmdProcessResult(CMD_ERROR_MISSING_BINARY);
                }
                else
                {
                    emit cmdProcessResult(CMD_ERROR_START);
                }
                qDebug() << error;
            });
    //执行完成
    connect(this, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
            [this](int exitCode, QProcess::ExitStatus exitStatus)
            {
                if (QProcess::NormalExit == exitStatus && 0 == exitCode)
                {
                    emit cmdProcessResult(CMD_SUCCESS_EXEC);
                }
                else
                {
                    emit cmdProcessResult(CMD_ERROR_EXEC);
                }
                qDebug() << exitCode << exitStatus;
            });

    //读取错误
    connect(this, &QProcess::readyReadStandardError, this,
            [this]()
            {
                m_errOutput = QString::fromLocal8Bit(readAllStandardError()).trimmed();
                qDebug() << m_errOutput;
            });
    //读取输出
    connect(this, &QProcess::readyReadStandardOutput, this,
            [this]()
            {
                m_stdOutput = QString::fromLocal8Bit(readAllStandardOutput()).trimmed();
                qDebug() << m_stdOutput;
            });
    //线程开始
    connect(this, &QProcess::started, this, [this]() { emit cmdProcessResult(CMD_SUCCESS_START); });
}
