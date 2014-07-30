/* qeventloopinteractor.cpp
   Copyright (C) 2003 Klarälvdalens Datakonsult AB

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

#include <qgpgme/qgpgme_eventloopinteractor.h>

#include <gpgme++/global.h>

#include <QIODevice>
#include <QSignalMapper>

Q_GLOBAL_STATIC(QSignalMapper, readSignalMapper)
Q_GLOBAL_STATIC(QSignalMapper, writeSignalMapper)

static QSignalMapper *setupReadSignalMapper(QObject *o)
{
    QSignalMapper *sm = readSignalMapper();
    o->connect(sm, SIGNAL(mapped(int)), SLOT(slotReadActivity(int)));
    return sm;
}

static QSignalMapper *setupWriteSignalMapper(QObject *o)
{
    QSignalMapper *sm = writeSignalMapper();
    o->connect(sm, SIGNAL(mapped(int)), SLOT(slotWriteActivity(int)));
    return sm;
}

namespace
{
struct IO {
    QIODevice *device;
    QGpgME::EventLoopInteractor::Direction direction;
};
}

void *QGpgME::EventLoopInteractor::registerWatcher(int fd, Direction dir, bool &ok)
{
    QIODevice *const iod = GpgME::getQIODevice(fd);
    if (!iod) {
        ok = false;
        return 0;
    }
    if (dir == Read) {
        static QSignalMapper *rsm = setupReadSignalMapper(this);
        if (!rsm->mapping(fd)) {
            rsm->setMapping(iod, fd);
            connect(iod, SIGNAL(readyRead()), rsm, SLOT(map()));
        } else {
            // if this fd is already registered, gpgme registers an additional
            // callback for the same fd.
            // if there is already something to read when registering the new
            // callback, gpgme expects the new callback to be called, so we
            // trigger it"
            QMetaObject::invokeMethod(this, "slotReadActivity", Qt::QueuedConnection, Q_ARG(int, fd));
        }
    } else {
        static QSignalMapper *wsm = setupWriteSignalMapper(this);
        if (!wsm->mapping(fd)) {
            wsm->setMapping(iod, fd);
            connect(iod, SIGNAL(bytesWritten(qint64)), wsm, SLOT(map()));
        } else {
            // if this fd is already registered, gpgme registers an additional
            // callback for the same fd.
            // if the device is writable when registering the new
            // callback, gpgme expects the new callback to be called, so we
            // trigger it:
            QMetaObject::invokeMethod(this, "slotWriteActivity", Qt::QueuedConnection, Q_ARG(int, fd));
        }
    }

    ok = true;
    IO *const io = new IO;
    io->device = iod;
    io->direction = dir;
    iod->bytesAvailable(); //HACK: tell KDPipeIODevices to start their threads
    iod->bytesToWrite();
    return io;
}

void QGpgME::EventLoopInteractor::unregisterWatcher(void *tag)
{
    if (!tag) {
        return;
    }
    const IO *const io = static_cast<IO *>(tag);
    if (io->direction == Read) {
        // no setupReadSignalMapper here, since registerWatcher,
        // called before us, is guaranteed to have set it up
        static QSignalMapper *rsm = readSignalMapper();
        disconnect(io->device, SIGNAL(readyRead()), rsm, SLOT(map()));
    } else {
        static QSignalMapper *wsm = writeSignalMapper();
        disconnect(io->device, SIGNAL(bytesWritten(qint64)), wsm, SLOT(map()));
    }
    delete io;
}
