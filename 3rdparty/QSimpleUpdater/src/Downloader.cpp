/*
 * Copyright (c) 2014-2021 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QAuthenticator>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <math.h>

#include "AuthenticateDialog.h"
#include "Downloader.h"
#include "UiHelper.h"

static const QString PARTIAL_DOWN(".part");

Downloader::Downloader(QWidget *parent)
    : QWidget(parent)
{
    m_ui = new Ui::Downloader;
    m_ui->setupUi(this);

    /* Initialize private members */
    m_manager = new QNetworkAccessManager();

    /* Initialize internal values */
    m_url = "";
    m_fileName = "";
    m_startTime = 0;
    m_useCustomProcedures = false;
    m_mandatoryUpdate = false;

    /* Set download directory */
    m_downloadDir.setPath(QDir::homePath() + "/Downloads/");

    /* Make the window look like a modal dialog */
    setWindowIcon(QIcon());
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    /* Configure the appearance and behavior of the buttons */
    // 2025-09-21 第三方库修改 删除下载对话框的打开，停止按钮
    m_ui->tipLabel->setText(tr("按【取消】键将取消下载..."));

    connect(m_manager, &QNetworkAccessManager::authenticationRequired, this, &Downloader::authenticate);

    /* Resize to fit */
    setFixedSize(minimumSizeHint());
}

Downloader::~Downloader()
{
    delete m_ui;
    delete m_reply;
    delete m_manager;
}

/**
 * Returns \c true if the updater shall not intervene when the download has
 * finished (you can use the \c QSimpleUpdater signals to know when the
 * download is completed).
 */
bool Downloader::useCustomInstallProcedures() const
{
    return m_useCustomProcedures;
}

/**
 * Changes the URL, which is used to indentify the downloader dialog
 * with an \c Updater instance
 *
 * \note the \a url parameter is not the download URL, it is the URL of
 *       the AppCast file
 */
void Downloader::setUrlId(const QString &url)
{
    m_url = url;
}

/**
 * Begins downloading the file at the given \a url
 */
void Downloader::startDownload(const QUrl &url)
{
    /* Reset UI */
    m_ui->progressBar->setValue(0);
    m_ui->stopButton->setText(tr("停止"));
    m_ui->downloadLabel->setText(tr("正在下载更新"));
    m_ui->timeLabel->setText(tr("剩余时间") + ": " + tr("未知"));

    /* Configure the network request */
    QNetworkRequest request(url);

    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    /* 10s timeout */
    request.setTransferTimeout(10000);
#endif

    if (!m_userAgentString.isEmpty())
        request.setRawHeader("User-Agent", m_userAgentString.toUtf8());

    /* Start download */
    m_reply = m_manager->get(request);
    m_startTime = QDateTime::currentDateTime().toSecsSinceEpoch();

    /* Ensure that downloads directory exists */
    if (!m_downloadDir.exists())
        m_downloadDir.mkpath(".");

    /* Remove old downloads */
    QFile::remove(m_downloadDir.filePath(m_fileName));
    QFile::remove(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN));

    /* Update UI when download progress changes or download finishes */
    connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(finished()));

    showNormal();
}

/**
 * Changes the name of the downloaded file
 */
void Downloader::setFileName(const QString &file)
{
    m_fileName = file;

    if (m_fileName.isEmpty())
        m_fileName = "QSU_Update.bin";
}

/**
 * Changes the user-agent string used to communicate with the remote HTTP server
 */
void Downloader::setUserAgentString(const QString &agent)
{
    m_userAgentString = agent;
}

void Downloader::finished()
{
    if (m_reply->error() != QNetworkReply::NoError) {
        QFile::remove(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN));
        return;
    }

    /* Rename file */
    QFile::rename(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN), m_downloadDir.filePath(m_fileName));

    /* Notify application */
    emit downloadFinished(m_url, m_downloadDir.filePath(m_fileName));

    /* Install the update */
    m_reply->close();
    // 2025-09-21 第三方库修改 弹窗提示前关闭下载对话框
    setVisible(false);
    installUpdate();
}

/**
 * Opens the downloaded file.
 * \note If the downloaded file is not found, then the function will alert the
 *       user about the error.
 */
void Downloader::openDownload()
{
    if (!m_fileName.isEmpty()) {
        // 2025-09-21 第三方库修改 不同平台下的更新包安装流程
        QString installerPath = m_downloadDir.filePath(m_fileName);
#ifdef Q_OS_WIN
        QProcess::startDetached(installerPath);
#elif defined(Q_OS_LINUX)
        QFileInfo fi(installerPath);
        if (!fi.isExecutable()) {
            QFile::setPermissions(installerPath, fi.permissions() | QFileDevice::ExeUser);
        }
        QProcess::startDetached(installerPath);
#endif
        QApplication::quit();

        // QDesktopServices::openUrl(QUrl::fromLocalFile(m_downloadDir.filePath(m_fileName)));
    }

    else {
        QMessageBox::critical(this, tr("错误"), tr("找不到已下载的更新文件！"), QMessageBox::Close);
    }
}

/**
 * Instructs the OS to open the downloaded file.
 *
 * \note If \c useCustomInstallProcedures() returns \c true, the function will
 *       not instruct the OS to open the downloaded file. You can use the
 *       signals fired by the \c QSimpleUpdater to install the update with your
 *       own implementations/code.
 */
void Downloader::installUpdate()
{
    if (useCustomInstallProcedures())
        return;

    // 2025-08-23 第三方库修改 对话框样式规范化
    /* Update labels */
    m_ui->stopButton->setText(tr("关闭"));
    m_ui->downloadLabel->setText(tr("下载完成!"));
    m_ui->timeLabel->setText(tr("安装程序将独立打开") + "...");
    // 2025-09-21 第三方库修改 弹窗提示前关闭下载对话框

    /* Ask the user to install the download */
    QString title = tr("为了安装更新，您可能需要退出应用程序！");
    QString text = tr("是否开始安装更新包？");
    if (m_mandatoryUpdate)
        text = tr("是否开始安装更新包？<br/><br/><strong>本次为强制更新，若取消将无法上班！</strong>");

    QMessageBox::StandardButton clickBtn = UiHelper::showMessageBoxQuestion(title,
                                                                            text,
                                                                            QMessageBox::Ok | QMessageBox::Cancel);

    /* User wants to install the download */
    if (clickBtn == QMessageBox::Ok) {
        if (!useCustomInstallProcedures())
            openDownload();
    }
    /* Wait */
    else {
        if (m_mandatoryUpdate) {
            // 2025-09-21 第三方库修改 强制更新时不退出程序
            // QApplication::quit();
        }
    }
}

/**
 * Prompts the user if he/she wants to cancel the download and cancels the
 * download if the user agrees to do that.
 */
void Downloader::cancelDownload()
{
    if (!m_reply->isFinished()) {
        // 2025-09-22 第三方库修改 取消下载时不提示
        hide();
        m_reply->abort();
        if (m_mandatoryUpdate) {
            // 2025-09-21 第三方库修改 强制更新时不退出程序
            // QApplication::quit();
        }
    } else {
        if (m_mandatoryUpdate) {
            // 2025-09-21 第三方库修改 强制更新时不退出程序
            // QApplication::quit();
        }

        hide();
    }
}

/**
 * Writes the downloaded data to the disk
 */
void Downloader::saveFile(qint64 received, qint64 total)
{
    Q_UNUSED(received);
    Q_UNUSED(total);

    /* Check if we need to redirect */
    QUrl url = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!url.isEmpty()) {
        startDownload(url);
        return;
    }

    /* Save downloaded data to disk */
    QFile file(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN));
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        file.write(m_reply->readAll());
        file.close();
    }
}

/**
 * Calculates the appropiate size units (bytes, KB or MB) for the received
 * data and the total download size. Then, this function proceeds to update the
 * dialog controls/UI.
 */
void Downloader::calculateSizes(qint64 received, qint64 total)
{
    QString totalSize;
    QString receivedSize;

    if (total < 1024)
        totalSize = tr("%1 bytes").arg(total);

    else if (total < 1048576)
        totalSize = tr("%1 KB").arg(round(total / 1024));

    else
        totalSize = tr("%1 MB").arg(round(total / 1048576));

    if (received < 1024)
        receivedSize = tr("%1 bytes").arg(received);

    else if (received < 1048576)
        receivedSize = tr("%1 KB").arg(received / 1024);

    else
        receivedSize = tr("%1 MB").arg(received / 1048576);

    m_ui->downloadLabel->setText(tr("正在下载更新") + " (" + receivedSize + " " + tr("/") + " " + totalSize + ")");
}

/**
 * Get response filename.
 */
void Downloader::metaDataChanged()
{
    QString filename = "";
    QVariant variant = m_reply->header(QNetworkRequest::ContentDispositionHeader);
    if (variant.isValid()) {
        QString contentDisposition = QByteArray::fromPercentEncoding(variant.toByteArray()).constData();
        QRegularExpression regExp(R"(filename=(\S+))");
        QRegularExpressionMatch match = regExp.match(contentDisposition);
        if (match.hasMatch()) {
            filename = match.captured(1);
        }
        setFileName(filename);
    }
}

/**
 * Uses the \a received and \a total parameters to get the download progress
 * and update the progressbar value on the dialog.
 */
void Downloader::updateProgress(qint64 received, qint64 total)
{
    if (total > 0) {
        m_ui->progressBar->setMinimum(0);
        m_ui->progressBar->setMaximum(100);
        m_ui->progressBar->setValue((received * 100) / total);

        calculateSizes(received, total);
        calculateTimeRemaining(received, total);
        saveFile(received, total);
    }

    else {
        m_ui->progressBar->setMinimum(0);
        m_ui->progressBar->setMaximum(0);
        m_ui->progressBar->setValue(-1);
        m_ui->downloadLabel->setText(tr("正在下载更新") + "...");
        m_ui->timeLabel->setText(QString("%1: %2").arg(tr("剩余时间")).arg(tr("未知")));
    }
}

/**
 * Uses two time samples (from the current time and a previous sample) to
 * calculate how many bytes have been downloaded.
 *
 * Then, this function proceeds to calculate the appropiate units of time
 * (hours, minutes or seconds) and constructs a user-friendly string, which
 * is displayed in the dialog.
 */
void Downloader::calculateTimeRemaining(qint64 received, qint64 total)
{
    uint difference = QDateTime::currentDateTime().toSecsSinceEpoch() - m_startTime;

    if (difference > 0) {
        QString timeString;
        qreal timeRemaining = (total - received) / (received / difference);

        if (timeRemaining > 7200) {
            timeRemaining /= 3600;
            int hours = int(timeRemaining + 0.5);

            if (hours > 1)
                timeString = tr("还需约 %1 小时").arg(hours);
            else
                timeString = tr("还需约 1 小时");
        }

        else if (timeRemaining > 60) {
            timeRemaining /= 60;
            int minutes = int(timeRemaining + 0.5);

            if (minutes > 1)
                timeString = tr("%1 分钟").arg(minutes);
            else
                timeString = tr("1 分钟");
        }

        else if (timeRemaining <= 60) {
            int seconds = int(timeRemaining + 0.5);

            if (seconds > 1)
                timeString = tr("%1 秒").arg(seconds);
            else
                timeString = tr("1 秒");
        }

        m_ui->timeLabel->setText(tr("剩余时间") + ": " + timeString);
    }
}

void Downloader::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply);
    AuthenticateDialog dlg(this);
    dlg.setUserName(authenticator->user());
    dlg.setPassword(authenticator->password());
    if (dlg.exec()) {
        authenticator->setUser(dlg.userName());
        authenticator->setPassword(dlg.password());
    }
}

/**
 * Rounds the given \a input to two decimal places
 */
qreal Downloader::round(const qreal &input)
{
    return static_cast<qreal>(roundf(static_cast<float>(input) * 100) / 100);
}

QString Downloader::downloadDir() const
{
    return m_downloadDir.absolutePath();
}

void Downloader::setDownloadDir(const QString &downloadDir)
{
    if (m_downloadDir.absolutePath() != downloadDir)
        m_downloadDir.setPath(downloadDir);
}

/**
 * If the \a mandatory_update is set to \c true, the \c Downloader has to download and install the
 * update. If the user cancels or exits, the application will close
 */
void Downloader::setMandatoryUpdate(const bool mandatory_update)
{
    m_mandatoryUpdate = mandatory_update;
}

void Downloader::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_X) {
        // 取消下载
        cancelDownload();
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

/**
 * If the \a custom parameter is set to \c true, then the \c Downloader will not
 * attempt to open the downloaded file.
 *
 * Use the signals fired by the \c QSimpleUpdater to implement your own install
 * procedures.
 */
void Downloader::setUseCustomInstallProcedures(const bool custom)
{
    m_useCustomProcedures = custom;
}

#if QSU_INCLUDE_MOC
    #include "moc_Downloader.cpp"
#endif
