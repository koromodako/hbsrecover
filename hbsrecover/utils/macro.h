/*
 *  Picsou | Keep track of your expenses !
 *  Copyright (C) 2018  koromodako
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef MACRO_H
#define MACRO_H

#include <QDir>
#include <QDebug>
#include <QSharedPointer>

#define BOOL2STR(b)     ((b)?"true":"false")
#define IS_FLAG_SET(field, flag) (((field)&(flag))==(flag))

#define DECL_PICSOU_OBJ_PTR(Class, ClassShPtr, ClassShPtrList) \
    typedef QSharedPointer<Class> ClassShPtr; \
    typedef QList<ClassShPtr> ClassShPtrList

#define LOG(log_func, lvl, color, ...) \
    do { \
        QDebug debug=log_func(); \
        debug.nospace(); \
        debug<<__VA_ARGS__; \
    } while(0);

#ifdef QT_DEBUG
#   define LOG_DEBUG(...)   LOG(qDebug, LOG_LVL_DBG, LOG_CLR_DBG, __VA_ARGS__)
#else /* QT_DEBUG */
#   define LOG_DEBUG(...)
#endif /* QT_DEBUG */

#define LOG_INFO(...)       LOG(qInfo, LOG_LVL_INF, LOG_CLR_INF, __VA_ARGS__)
#define LOG_WARNING(...)    LOG(qWarning, LOG_LVL_WRN, LOG_CLR_WRN, __VA_ARGS__)
#define LOG_CRITICAL(...)   LOG(qCritical, LOG_LVL_ERR, LOG_CLR_ERR, __VA_ARGS__)

#define LOG_IN(...)         LOG_DEBUG("<- ("<<__VA_ARGS__<<")")
#define LOG_IN_VOID(...)    LOG_DEBUG("<- (void)")

#define LOG_BOOL_RETURN(bool_expr) \
    do { \
        bool result=(bool_expr); \
        LOG_DEBUG("-> "<<BOOL2STR(result)) \
        return result; \
    } while(0);

#define LOG_VOID_RETURN() \
    do { \
        LOG_DEBUG("-> (void)") \
        return; \
    } while(0);

#define LOG_CUST_RETURN(value, display) \
    do { \
        LOG_DEBUG("-> "<<display) \
        return value; \
    } while(0);

#endif // MACRO_H


