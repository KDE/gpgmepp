/* tests/test_editinteractor.h
   Copyright (C) 2007 Klarälvdalens Datakonsult AB

   This file is part of QGPGME's regression test suite.

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

#ifndef __QGPGME_TESTS_TEST_EDITINTERACTOR_H__
#define __QGPGME_TESTS_TEST_EDITINTERACTOR_H__

#include <qgpgme/eventloopinteractor.h>

#include <gpgme++/editinteractor.h>
#include <gpgme++/context.h>
#include <gpgme++/error.h>
#include <gpgme++/data.h>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>

#include <gpg-error.h>

#include <QtCore>

#include <memory>
#include <stdexcept>
#include <cassert>
#include <string>

class KeyResolveJob : QObject
{
    Q_OBJECT
public:
    explicit KeyResolveJob(GpgME::Protocol proto = GpgME::OpenPGP, QObject *p = 0)
        : QObject(p),
          m_ctx(GpgME::Context::createForProtocol(proto)),
          m_done(false),
          m_loop(0)
    {
        assert(m_ctx.get());
        connect(QGpgME::EventLoopInteractor::instance(), SIGNAL(nextKeyEventSignal(GpgME::Context *, GpgME::Key)),
                this, SLOT(slotNextKey(GpgME::Context *, GpgME::Key)));
        connect(QGpgME::EventLoopInteractor::instance(), SIGNAL(operationDoneEventSignal(GpgME::Context *, GpgME::Error)),
                this, SLOT(slotDone(GpgME::Context *, GpgME::Error)));

        m_ctx->setManagedByEventLoopInteractor(true);
    }

    GpgME::Error start(const char *pattern, bool secretOnly = false)
    {
        return m_ctx->startKeyListing(pattern, secretOnly);
    }

    GpgME::Error waitForDone()
    {
        if (m_done) {
            return m_error;
        }
        QEventLoop loop;
        m_loop = &loop;
        loop.exec();
        m_loop = 0;
        return m_error;
    }

    std::vector<GpgME::Key> keys() const
    {
        return m_keys;
    }

private Q_SLOTS:
    void slotNextKey(GpgME::Context *ctx, const GpgME::Key &key)
    {
        if (ctx != m_ctx.get()) {
            return;
        }
        m_keys.push_back(key);
    }
    void slotDone(GpgME::Context *ctx, const GpgME::Error &err)
    {
        if (ctx != m_ctx.get()) {
            return;
        }
        m_error = err;
        m_done = true;
        if (m_loop) {
            m_loop->quit();
        }
    }

private:
    std::auto_ptr<GpgME::Context> m_ctx;
    GpgME::Error m_error;
    bool m_done;
    std::vector<GpgME::Key> m_keys;
    QEventLoop *m_loop;
};

static int test_editinteractor(std::auto_ptr<GpgME::EditInteractor> ei, const char *keyid, GpgME::Protocol proto = GpgME::OpenPGP)
{

    using namespace GpgME;

    KeyResolveJob job(proto);
    if (const GpgME::Error err = job.start(keyid)) {
        throw std::runtime_error(std::string("startKeyListing: ") + err.asString());
    }

    if (const GpgME::Error err = job.waitForDone()) {
        throw std::runtime_error(std::string("nextKey: ") + err.asString());
    }

    const Key key = job.keys().front();

    const std::auto_ptr<Context> ctx(Context::createForProtocol(proto));

    ctx->setManagedByEventLoopInteractor(true);

    Data data;
    ei->setDebugChannel(stderr);

    QObject::connect(QGpgME::EventLoopInteractor::instance(), SIGNAL(operationDoneEventSignal(GpgME::Context *, GpgME::Error)),
                     QCoreApplication::instance(), SLOT(quit()));

    if (Error err = ctx->startEditing(key, ei, data)) {
        throw std::runtime_error(std::string("startEditing: ") + err.asString());
    }
    // ei released in passing to startEditing

    return QCoreApplication::instance()->exec();
}

#endif // __QGPGME_TESTS_TEST_EDITINTERACTOR_H__
