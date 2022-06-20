#ifndef CMDPROC_H
#define CMDPROC_H

#include <QProcess>
#include <QObject>
#include <QSharedPointer>
class CmdProc : public QProcess
{
    Q_OBJECT
  public:
    enum CMD_EXEC_RESULT
    {
        CMD_SUCCESS_START,        // 启动成功
        CMD_ERROR_START,          // 启动失败
        CMD_SUCCESS_EXEC,         // 执行成功
        CMD_ERROR_EXEC,           // 执行失败
        CMD_ERROR_MISSING_BINARY, // 找不到文件
    };
    CmdProc(QObject *parent = 0);

    //执行命令，命令都可以通过这个执行
    void execute(const QString &progPath, const QStringList &args);

    /**
     * @brief 查看所有的编码器
     */
    void checkEncoders(); //查看编码器
    /**
     * @brief 查看所有的解码器
     */
    void checkDecoders();
    /**
     * @brief 查看硬件加速选项
     */
    void checkHwAccelers();

    /**
     * @brief convertVideo 转换视频格式
     * @param file 输入文件路径
     * @param outputFile 输出文件路径
     * @param argsIn 输入视频参数
     * @param argsOut 输出视频参数
     */
    void convertVideo(const QString file, const QString outputFile, const QStringList argsIn, const QStringList argsOut);

    const QString &stdOutput() const;

    void setStdOutput(const QString &newStdOutput);

  private:
    QString m_stdOutput = "";
    QString m_errOutput = "";

    void initSignals(); //在这里统一初始化信号绑定
  signals:
    void cmdProcessResult(CMD_EXEC_RESULT res);
};

#endif // CMDPROC_H
