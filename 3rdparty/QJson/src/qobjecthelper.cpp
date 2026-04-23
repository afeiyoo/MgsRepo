/* This file is part of qjson
  *
  * Copyright (C) 2009 Till Adam <adam@kde.org>
  * Copyright (C) 2009 Flavio Castelli <flavio@castelli.name>
  * Copyright (C) 2016 Anton Kudryavtsev <a.kudryavtsev@netris.ru>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License version 2.1, as published by the Free Software Foundation.
  *
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */


#include "qobjecthelper.h"

#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QObject>

using namespace QJson;

class QObjectHelper::QObjectHelperPrivate {
};

QObjectHelper::QObjectHelper()
  : d (new QObjectHelperPrivate)
{
}

QObjectHelper::~QObjectHelper()
{
  delete d;
}

QVariantMap QObjectHelper::qobject2qvariant( const QObject* object,
                              const QStringList& ignoredProperties)
{
  //判空
  QVariantMap result;
  if(object == nullptr)
      return result;
  const QMetaObject *metaobject = object->metaObject();
  int count = metaobject->propertyCount();
  for (int i=0; i<count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();

    if (!metaproperty.isReadable() || ignoredProperties.contains(QLatin1String(name)))
      continue;

    QVariant value = object->property(name);
    result[QLatin1String(name)] = value;
 }
  return result;
}

void QObjectHelper::qvariant2qobject(const QVariantMap& variant, QObject* object)
{
  //判空
  if(object == nullptr || variant.isEmpty())
      return;
  const QMetaObject *metaobject = object->metaObject();
  // 第三方库修改 2025-05-18 对象属性名与键名之间忽略大小写进行匹配
  int propCount = metaobject->propertyCount();
  QHash<QString, int> nameToIndex;  // 构建一个 “小写属性名 → 元属性索引” 的映射
  nameToIndex.reserve(propCount);
  for (int i = 0; i < propCount; ++i) {
    QMetaProperty prop = metaobject->property(i);
    // 转为小写以实现不区分大小写
    nameToIndex.insert(QString::fromLatin1(prop.name()).toLower(), i);
  }

  for (QVariantMap::const_iterator iter = variant.constBegin(), end = variant.constEnd(); iter != end; ++iter) {

    QString keyLower = iter.key().toLower();
    // 不存在同名属性（忽略大小写）则跳过
    if (!nameToIndex.contains(keyLower))
        continue;

    int pIdx = nameToIndex.value(keyLower);
    QMetaProperty metaproperty = metaobject->property(pIdx);
    // 跳过不可写属性
    if (!metaproperty.isWritable())
        continue;

    QVariant v = iter.value();
    int targetType = metaproperty.type();

    if (v.canConvert(targetType)) {
        v.convert(targetType);
        metaproperty.write(object, v);
    } else if (QLatin1String(metaproperty.typeName()) == QLatin1String("QVariant")) {
        metaproperty.write(object, v);
    }
  }
}
