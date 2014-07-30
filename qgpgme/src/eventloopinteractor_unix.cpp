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

#include <qgpgme_eventloopinteractor.h>

#include <QSocketNotifier>

void *QGpgME::EventLoopInteractor::registerWatcher(int fd, Direction dir, bool &ok)
{
    QSocketNotifier *const sn = new QSocketNotifier(fd, dir == Read ? QSocketNotifier::Read : QSocketNotifier::Write);
    if (dir == Read) {
        connect(sn, SIGNAL(activated(int)), this, SLOT(slotReadActivity(int)));
    } else {
        connect(sn, SIGNAL(activated(int)), this, SLOT(slotWriteActivity(int)));
    }
    ok = true; // Can above operations fails?
    return sn;
}

void QGpgME::EventLoopInteractor::unregisterWatcher(void *tag)
{
    static_cast<QSocketNotifier *>(tag)->deleteLater();
}
