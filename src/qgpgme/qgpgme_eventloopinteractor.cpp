/* qeventloopinteractor.cpp
   Copyright (C) 2003, 2007 Klarälvdalens Datakonsult AB

   This file is part of QGPGME.

   QGPGME is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   QGPGME is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with QGPGME; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA. */

// -*- c++ -*-

#include "qgpgme_eventloopinteractor.h"

#include <QCoreApplication>
#include <QSocketNotifier>
#include <QPointer>

using namespace GpgME;

QGpgME::EventLoopInteractor::EventLoopInteractor(QObject *parent)
    : QObject(parent), GpgME::EventLoopInteractor()
{
    setObjectName(QLatin1String("QGpgME::EventLoopInteractor::instance()"));
    if (!parent) {
        if (QCoreApplication *const app = QCoreApplication::instance()) {
            connect(app, SIGNAL(aboutToQuit()), SLOT(deleteLater()));
            connect(app, SIGNAL(aboutToQuit()), SIGNAL(aboutToDestroy()));
        }
    }
    mSelf = this;
}

QGpgME::EventLoopInteractor::~EventLoopInteractor()
{
    emit aboutToDestroy();
    mSelf = 0;
}

QGpgME::EventLoopInteractor *QGpgME::EventLoopInteractor::mSelf = 0;

QGpgME::EventLoopInteractor *QGpgME::EventLoopInteractor::instance()
{
    if (!mSelf) {
#ifndef NDEBUG
        if (!QCoreApplication::instance()) {
            qWarning("QGpgME::EventLoopInteractor: Need a Q(Core)Application object before calling instance()!");
        } else {
#endif
            (void)new EventLoopInteractor;
#ifndef NDEBUG
        }
#endif
    }
    return mSelf;
}

namespace
{

template <typename T_Enableable>
class QDisabler
{
    const QPointer<T_Enableable> o;
    const bool wasEnabled;
public:
    explicit QDisabler(T_Enableable *t) : o(t), wasEnabled(o  &&o->isEnabled())
    {
        if (t) {
            t->setEnabled(false);
        }
    }
    ~QDisabler()
    {
        if (o) {
            o->setEnabled(wasEnabled);
        }
    }
};
}

void QGpgME::EventLoopInteractor::slotWriteActivity(int socket)
{
    // Make sure to disable the notifier while we are processing the event, as
    // it's easy to run into re-entrancy issues, if actOn causes things to return
    // to the event loop in some way (such as showing the passphrase dialog).
    // We use a qpointer as actOn will destroy the notifier, when it's done with the FD
    const QDisabler<QSocketNotifier> disabled(qobject_cast<QSocketNotifier *>(sender()));
    actOn(socket , Write);
}

void QGpgME::EventLoopInteractor::slotReadActivity(int socket)
{
    const QDisabler<QSocketNotifier> disabled(qobject_cast<QSocketNotifier *>(sender()));
    actOn(socket , Read);
}

void QGpgME::EventLoopInteractor::nextTrustItemEvent(GpgME::Context *context, const GpgME::TrustItem &item)
{
    emit nextTrustItemEventSignal(context, item);
}

void QGpgME::EventLoopInteractor::nextKeyEvent(GpgME::Context *context, const GpgME::Key &key)
{
    emit nextKeyEventSignal(context, key);
}

void QGpgME::EventLoopInteractor::operationDoneEvent(GpgME::Context *context, const GpgME::Error &e)
{
    emit operationDoneEventSignal(context, e);
}

void QGpgME::EventLoopInteractor::operationStartEvent(GpgME::Context *context)
{
    emit operationStartEventSignal(context);
}

