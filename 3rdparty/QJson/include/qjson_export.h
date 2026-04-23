/*  This file is part of the KDE project
    Copyright (C) 2009 Pino Toscano <pino@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License version 2.1, as published by the Free Software Foundation.


    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#pragma once

#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(QJSON_DYNAMIC)
        #define QJSON_EXPORT Q_DECL_EXPORT
    #elif defined(QJSON_STATIC)
        #define QJSON_EXPORT
    #else
        #define QJSON_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define QJSON_EXPORT
#endif
