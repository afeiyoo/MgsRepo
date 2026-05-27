#include "weightinfodelegate.h"

#include "components/weightinfopanel.h"
#include "global/uiconst.h"
#include <QPainter>
#include <QPainterPath>

WeightInfoDelegate::WeightInfoDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}

WeightInfoDelegate::~WeightInfoDelegate() {}

void WeightInfoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QRect rect = option.rect;

    QColor bgColor(Color::WEIGHTINFO_ITEM_BG);
    QPainterPath bgPath;
    bgPath.addRect(rect.adjusted(1, 1, -8, -1));
    painter->fillPath(bgPath, bgColor);

    // 背景（纯透明）
    painter->setPen(Qt::NoPen);
    painter->drawPath(bgPath);

    // 获取数据
    QString plate = index.data(WeightInfoModel::PlateRole).toString();
    uint axisType = index.data(WeightInfoModel::AxisTypeRole).toUInt();
    uint axisNum = index.data(WeightInfoModel::AxisNumRole).toUInt();
    qreal weight = index.data(WeightInfoModel::WeightRole).toReal();
    uint status = index.data(WeightInfoModel::StatusRole).toUInt();
    QPixmap pixmap = getAxisTypePixmap(axisType, status);

    QString axisInfo = QString("%1(%2轴)").arg(axisType).arg(axisNum);
    QString weightInfo = QString("%1吨").arg(weight);

    // 布局参数
    int margin = 5;
    int spacing = 8;

    // 图片区域（右侧）
    int iconWidth = 105;
    int iconHeight = 60;
    QRect iconRect(rect.right() - margin - 7 - iconWidth, rect.center().y() - iconHeight / 2, iconWidth, iconHeight);

    // 绘制图片
    QPainterPath imgPath;
    imgPath.addRect(iconRect);
    painter->drawPixmap(iconRect, pixmap);

    // 文本区域（左侧）
    int textWidth = iconRect.left() - rect.left() - margin;
    int lineHeight = 15;
    int textX = rect.left() + margin;
    int textY = rect.top() + (rect.height() - (lineHeight * 3 + spacing * 2)) / 2;

    QFont font = option.font;
    font.setPixelSize(14);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(QRect(textX, textY, textWidth, lineHeight), Qt::AlignLeft | Qt::AlignVCenter, plate);
    painter->drawText(QRect(textX, textY + lineHeight + spacing, textWidth, lineHeight), Qt::AlignLeft | Qt::AlignVCenter, axisInfo);
    painter->drawText(QRect(textX, textY + (lineHeight + spacing) * 2, textWidth, lineHeight), Qt::AlignLeft | Qt::AlignVCenter, weightInfo);

    painter->restore();
}

QSize WeightInfoDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(195 + 8, 75); // 8px是item之间的水平间距
}

QPixmap WeightInfoDelegate::getAxisTypePixmap(uint axisType, uint status) const
{
    QString strStatus;
    if (status == 0) {
        strStatus = "waiting";
    } else if (status == 1) {
        strStatus = "pass";
    } else {
        strStatus = "overload";
    }

    QString str = QString(":/static/images/axistype_%1_%2.png").arg(axisType).arg(strStatus);

    QPixmap axisTypePixmap = QPixmap(str);
    if (axisTypePixmap.isNull()) {
        if (status == 0) {
            return QPixmap(":/static/images/axistype_unknown_waiting.png");
        } else if (status == 1) {
            return QPixmap(":/static/images/axistype_unknown_pass.png");
        } else {
            return QPixmap(":/static/images/axistype_unknown_overload.png");
        }
    }

    return axisTypePixmap;
}
