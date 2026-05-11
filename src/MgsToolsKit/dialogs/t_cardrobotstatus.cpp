#include "t_cardrobotstatus.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QIcon>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "global/appdefs.h"
#include "global/constant.h"
#include "global/globalmanager.h"
#include "global/signalmanager.h"

T_CardRobotStatus::T_CardRobotStatus(QWidget *parent)
    : ElaDialog{parent}
{
    setFixedSize(500, 500);
    setWindowTitle("设置心跳上报状态");
    setWindowIcon(QIcon(Constant::APP::ICON_PATH));
    setIsFixedSize(true);
    setWindowModality(Qt::ApplicationModal);
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);

    initContent();

    connect(m_confirmButton, &ElaPushButton::clicked, this, [=](bool checked) {
        Q_UNUSED(checked);
        initRobotStatus();
        this->hide();
    });

    initRobotStatus();
}

T_CardRobotStatus::~T_CardRobotStatus() {}

void T_CardRobotStatus::initContent()
{
    ElaText *robotStatusTip1 = new ElaText("卡机状态", this);
    ElaText *robotStatusTip2 = new ElaText("卡机状态", this);
    ElaText *robotStatusTip3 = new ElaText("卡机状态", this);
    ElaText *robotStatusTip4 = new ElaText("卡机状态", this);

    ElaText *bagStatusTip1 = new ElaText("卡夹状态", this);
    ElaText *bagStatusTip2 = new ElaText("卡夹状态", this);
    ElaText *bagStatusTip3 = new ElaText("卡夹状态", this);
    ElaText *bagStatusTip4 = new ElaText("卡夹状态", this);

    ElaText *cardNumTip1 = new ElaText("卡数量", this);
    ElaText *cardNumTip2 = new ElaText("卡数量", this);
    ElaText *cardNumTip3 = new ElaText("卡数量", this);
    ElaText *cardNumTip4 = new ElaText("卡数量", this);

    ElaText *trackStatusTip1 = new ElaText("轨道有卡状态", this);
    ElaText *trackStatusTip2 = new ElaText("轨道有卡状态", this);
    ElaText *trackStatusTip3 = new ElaText("轨道有卡状态", this);
    ElaText *trackStatusTip4 = new ElaText("轨道有卡状态", this);

    QList<ElaText *> texts = {robotStatusTip1, robotStatusTip2, robotStatusTip3, robotStatusTip4, bagStatusTip1, bagStatusTip2,
                              bagStatusTip3,   bagStatusTip4,   cardNumTip1,     cardNumTip2,     cardNumTip3,   cardNumTip4,
                              trackStatusTip1, trackStatusTip2, trackStatusTip3, trackStatusTip4};
    for (auto text : texts) {
        text->setTextPixelSize(15);
        text->setIsWrapAnywhere(false);
    }

    m_robotStatusComboBox1 = new ElaComboBox(this);
    m_robotStatusComboBox1->addItems({"卡机正常", "卡机故障", "卡机坏卡", "卡机离线", "上工位低卡量告警"});
    m_robotStatusComboBox2 = new ElaComboBox(this);
    m_robotStatusComboBox2->addItems({"卡机正常", "卡机故障", "卡机坏卡", "卡机离线"});
    m_robotStatusComboBox3 = new ElaComboBox(this);
    m_robotStatusComboBox3->addItems({"卡机正常", "卡机故障", "卡机坏卡", "卡机离线", "下工位低卡量告警"});
    m_robotStatusComboBox4 = new ElaComboBox(this);
    m_robotStatusComboBox4->addItems({"卡机正常", "卡机故障", "卡机坏卡", "卡机离线"});

    m_bagStatusComboBox1 = new ElaComboBox(this);
    m_bagStatusComboBox2 = new ElaComboBox(this);
    m_bagStatusComboBox3 = new ElaComboBox(this);
    m_bagStatusComboBox4 = new ElaComboBox(this);
    QList<ElaComboBox *> comboBoxs1 = {m_bagStatusComboBox1, m_bagStatusComboBox2, m_bagStatusComboBox3, m_bagStatusComboBox4};
    for (auto comboBox : comboBoxs1) {
        comboBox->addItems({"卡夹已装上", "卡夹已卸下"});
    }

    m_cardNumEdit1 = new ElaLineEdit(this);
    m_cardNumEdit2 = new ElaLineEdit(this);
    m_cardNumEdit3 = new ElaLineEdit(this);
    m_cardNumEdit4 = new ElaLineEdit(this);
    QList<ElaLineEdit *> numEdits = {m_cardNumEdit1, m_cardNumEdit2, m_cardNumEdit3, m_cardNumEdit4};
    for (auto edit : numEdits) {
        edit->setPlaceholderText("请输入数量");
        edit->setText("100");
    }

    m_trackStatusComboBox1 = new ElaComboBox(this);
    m_trackStatusComboBox2 = new ElaComboBox(this);
    m_trackStatusComboBox3 = new ElaComboBox(this);
    m_trackStatusComboBox4 = new ElaComboBox(this);
    QList<ElaComboBox *> comboBoxs2 = {m_trackStatusComboBox1, m_trackStatusComboBox2, m_trackStatusComboBox3, m_trackStatusComboBox4};
    for (auto comBox : comboBoxs2) {
        comBox->addItems({"无卡", "天线有卡", "卡口有卡"});
        comBox->setCurrentIndex(1);
    }

    QGroupBox *box1 = new QGroupBox("卡机1", this);
    QGridLayout *boxGLayout1 = new QGridLayout(box1);
    boxGLayout1->addWidget(robotStatusTip1, 0, 0);
    boxGLayout1->addWidget(m_robotStatusComboBox1, 0, 1);
    boxGLayout1->addWidget(bagStatusTip1, 1, 0);
    boxGLayout1->addWidget(m_bagStatusComboBox1, 1, 1);
    boxGLayout1->addWidget(cardNumTip1, 2, 0);
    boxGLayout1->addWidget(m_cardNumEdit1, 2, 1);
    boxGLayout1->addWidget(trackStatusTip1, 3, 0);
    boxGLayout1->addWidget(m_trackStatusComboBox1, 3, 1);

    QGroupBox *box2 = new QGroupBox("卡机2", this);
    QGridLayout *boxGLayout2 = new QGridLayout(box2);
    boxGLayout2->addWidget(robotStatusTip2, 0, 0);
    boxGLayout2->addWidget(m_robotStatusComboBox2, 0, 1);
    boxGLayout2->addWidget(bagStatusTip2, 1, 0);
    boxGLayout2->addWidget(m_bagStatusComboBox2, 1, 1);
    boxGLayout2->addWidget(cardNumTip2, 2, 0);
    boxGLayout2->addWidget(m_cardNumEdit2, 2, 1);
    boxGLayout2->addWidget(trackStatusTip2, 3, 0);
    boxGLayout2->addWidget(m_trackStatusComboBox2, 3, 1);

    QGroupBox *box3 = new QGroupBox("卡机3", this);
    QGridLayout *boxGLayout3 = new QGridLayout(box3);
    boxGLayout3->addWidget(robotStatusTip3, 0, 0);
    boxGLayout3->addWidget(m_robotStatusComboBox3, 0, 1);
    boxGLayout3->addWidget(bagStatusTip3, 1, 0);
    boxGLayout3->addWidget(m_bagStatusComboBox3, 1, 1);
    boxGLayout3->addWidget(cardNumTip3, 2, 0);
    boxGLayout3->addWidget(m_cardNumEdit3, 2, 1);
    boxGLayout3->addWidget(trackStatusTip3, 3, 0);
    boxGLayout3->addWidget(m_trackStatusComboBox3, 3, 1);

    QGroupBox *box4 = new QGroupBox("卡机4", this);
    QGridLayout *boxGLayout4 = new QGridLayout(box4);
    boxGLayout4->addWidget(robotStatusTip4, 0, 0);
    boxGLayout4->addWidget(m_robotStatusComboBox4, 0, 1);
    boxGLayout4->addWidget(bagStatusTip4, 1, 0);
    boxGLayout4->addWidget(m_bagStatusComboBox4, 1, 1);
    boxGLayout4->addWidget(cardNumTip4, 2, 0);
    boxGLayout4->addWidget(m_cardNumEdit4, 2, 1);
    boxGLayout4->addWidget(trackStatusTip4, 3, 0);
    boxGLayout4->addWidget(m_trackStatusComboBox4, 3, 1);

    m_confirmButton = new ElaPushButton("确认", this);

    QGridLayout *mainGLayout = new QGridLayout(this);
    mainGLayout->setContentsMargins(5, 5, 5, 5);
    mainGLayout->addWidget(box1, 0, 0);
    mainGLayout->addWidget(box2, 0, 1);
    mainGLayout->addWidget(box3, 1, 0);
    mainGLayout->addWidget(box4, 1, 1);
    mainGLayout->addWidget(m_confirmButton, 2, 0, 1, 2, Qt::AlignCenter);
}

void T_CardRobotStatus::initRobotStatus()
{
    ST_CardRobotStatusInfo info1;
    info1.boxStatus = m_robotStatusComboBox1->currentIndex() + 0x30;
    info1.hasCardBag = m_bagStatusComboBox1->currentIndex() + 0x30;
    info1.restCard = static_cast<ushort>(m_cardNumEdit1->text().toInt());
    info1.hasAnnCard = m_trackStatusComboBox1->currentIndex() + 0x30;

    ST_CardRobotStatusInfo info2;
    info2.boxStatus = m_robotStatusComboBox2->currentIndex() + 0x30;
    info2.hasCardBag = m_bagStatusComboBox2->currentIndex() + 0x30;
    info2.restCard = static_cast<ushort>(m_cardNumEdit2->text().toInt());
    info2.hasAnnCard = m_trackStatusComboBox2->currentIndex() + 0x30;

    ST_CardRobotStatusInfo info3;
    info3.boxStatus = m_robotStatusComboBox3->currentIndex() + 0x30;
    info3.hasCardBag = m_bagStatusComboBox3->currentIndex() + 0x30;
    info3.restCard = static_cast<ushort>(m_cardNumEdit3->text().toInt());
    info3.hasAnnCard = m_trackStatusComboBox3->currentIndex() + 0x30;

    ST_CardRobotStatusInfo info4;
    info4.boxStatus = m_robotStatusComboBox4->currentIndex() + 0x30;
    info4.hasCardBag = m_bagStatusComboBox4->currentIndex() + 0x30;
    info4.restCard = static_cast<ushort>(m_cardNumEdit4->text().toInt());
    info4.hasAnnCard = m_trackStatusComboBox4->currentIndex() + 0x30;

    emit GM_INSTANCE->m_signalMan->sigRobotStatusChanged(info1, info2, info3, info4);
}
