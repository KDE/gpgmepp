/* tests/editinteractortest.cpp
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

//
// usage:
// editinteractortest [--smime|--openpgp] <key>
// (--openpgp is implied is --smime is not given)
//

#include <qgpgme/eventloopinteractor.h>

#include <editinteractor.h>
#include <context.h>
#include <error.h>
#include <data.h>
#include <key.h>
#include <keylistresult.h>

#include <gpg-error.h>

#include <QtGui>

#include <memory>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace GpgME;

class InteractiveEditInteractor : public QDialog, public EditInteractor
{
    Q_OBJECT
public:
    explicit InteractiveEditInteractor(QWidget *parent = 0);
    ~InteractiveEditInteractor();

    const char *action(Error &err) const;
    unsigned int nextState(unsigned int status, const char *args, Error &err) const;

private Q_SLOTS:
    void slotAckButtonClicked()
    {
        lastAction = 0;
    }

    void slotSendButtonClicked()
    {
        lastAction = qstrdup(lineEdit.text().toUtf8().constData());
    }

private:
    QVBoxLayout vlay;
    mutable QTextEdit textEdit;
    QHBoxLayout hlay;
    mutable QLineEdit lineEdit;
    mutable QPushButton sendButton;
    mutable QPushButton ackButton;
    mutable QEventLoop loop;
    mutable char *lastAction;
};

InteractiveEditInteractor::InteractiveEditInteractor(QWidget *p)
    : QDialog(p),
      vlay(this),
      textEdit(this),
      hlay(),
      lineEdit(this),
      sendButton(tr("&Send"), this),
      ackButton(tr("&ACK"), this),
      loop(),
      lastAction(0)
{
    vlay.addWidget(&textEdit);
    vlay.addLayout(&hlay);
    hlay.addWidget(&lineEdit);
    hlay.addWidget(&sendButton);
    hlay.addWidget(&ackButton);

    setEnabled(false);

    connect(&sendButton, SIGNAL(clicked()), this, SLOT(slotSendButtonClicked()));
    connect(&sendButton, SIGNAL(clicked()), &loop, SLOT(quit()));

    connect(&lineEdit, SIGNAL(returnPressed()), this, SLOT(slotSendButtonClicked()));
    connect(&lineEdit, SIGNAL(returnPressed()), &loop, SLOT(quit()));

    connect(&ackButton, SIGNAL(clicked()), this, SLOT(slotAckButtonClicked()));
    connect(&ackButton, SIGNAL(clicked()), &loop, SLOT(quit()));
}

InteractiveEditInteractor::~InteractiveEditInteractor()
{
    if (lastAction) {
        std::free(lastAction);
    }
}

const char *InteractiveEditInteractor::action(Error &) const
{
    const_cast<InteractiveEditInteractor *>(this)->setEnabled(true);

    lineEdit.setFocus();

    if (lastAction) {
        std::free(lastAction);
    }
    lastAction = 0;

    loop.exec();

    lineEdit.clear();

    const_cast<InteractiveEditInteractor *>(this)->setEnabled(false);
    return lastAction;
}

static const char *status_strings[] = {
    "EOF",
    /* mkstatus processing starts here */
    "ENTER",
    "LEAVE",
    "ABORT",

    "GOODSIG",
    "BADSIG",
    "ERRSIG",

    "BADARMOR",

    "RSA_OR_IDEA",
    "KEYEXPIRED",
    "KEYREVOKED",

    "TRUST_UNDEFINED",
    "TRUST_NEVER",
    "TRUST_MARGINAL",
    "TRUST_FULLY",
    "TRUST_ULTIMATE",

    "SHM_INFO",
    "SHM_GET",
    "SHM_GET_BOOL",
    "SHM_GET_HIDDEN",

    "NEED_PASSPHRASE",
    "VALIDSIG",
    "SIG_ID",
    "ENC_TO",
    "NODATA",
    "BAD_PASSPHRASE",
    "NO_PUBKEY",
    "NO_SECKEY",
    "NEED_PASSPHRASE_SYM",
    "DECRYPTION_FAILED",
    "DECRYPTION_OKAY",
    "MISSING_PASSPHRASE",
    "GOOD_PASSPHRASE",
    "GOODMDC",
    "BADMDC",
    "ERRMDC",
    "IMPORTED",
    "IMPORT_OK",
    "IMPORT_PROBLEM",
    "IMPORT_RES",
    "FILE_START",
    "FILE_DONE",
    "FILE_ERROR",

    "BEGIN_DECRYPTION",
    "END_DECRYPTION",
    "BEGIN_ENCRYPTION",
    "END_ENCRYPTION",

    "DELETE_PROBLEM",
    "GET_BOOL",
    "GET_LINE",
    "GET_HIDDEN",
    "GOT_IT",
    "PROGRESS",
    "SIG_CREATED",
    "SESSION_KEY",
    "NOTATION_NAME",
    "NOTATION_DATA",
    "POLICY_URL",
    "BEGIN_STREAM",
    "END_STREAM",
    "KEY_CREATED",
    "USERID_HINT",
    "UNEXPECTED",
    "INV_RECP",
    "NO_RECP",
    "ALREADY_SIGNED",
    "SIGEXPIRED",
    "EXPSIG",
    "EXPKEYSIG",
    "TRUNCATED",
    "ERROR",
    "NEWSIG",
    "REVKEYSIG",
    "SIG_SUBPACKET",
    "NEED_PASSPHRASE_PIN",
    "SC_OP_FAILURE",
    "SC_OP_SUCCESS",
    "CARDCTRL",
    "BACKUP_KEY_CREATED",
    "PKA_TRUST_BAD",
    "PKA_TRUST_GOOD",

    "PLAINTEXT",
};
static const unsigned int num_status_strings = sizeof status_strings / sizeof * status_strings ;

unsigned int InteractiveEditInteractor::nextState(unsigned int s, const char *args, Error &) const
{
    const char *const state_string = s < num_status_strings ? status_strings[s] : "<unknown state>" ;
    if (needsNoResponse(s)) {
        textEdit.append(tr("%1: %2 (auto-ack'ed)").arg(state_string, QString::fromUtf8(args)));
        return state();
    } else {
        textEdit.append(tr("%1: %2").arg(state_string, QString::fromUtf8(args)));
        return state() + 1; // fake state change
    }
}

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    (void)QGpgME::EventLoopInteractor::instance();

    Protocol proto = OpenPGP;
    const char *keyid = 0;
    if (argc < 1 || argc > 3) {
        return 1;
    }
    if (argc == 2) {
        keyid = argv[1];
    }
    if (argc == 3) {
        if (qstrcmp(argv[1], "--openpgp") == 0) {
            proto = OpenPGP;
        } else if (argc == 2 && qstrcmp(argv[1], "--smime") == 0) {
            proto = CMS;
        } else {
            return 1;
        }
        keyid = argv[2];
    }

    try {
        Key key;
        {
            const std::auto_ptr<Context> kl(Context::createForProtocol(proto));

            if (!kl.get()) {
                return 1;
            }

            if (Error err = kl->startKeyListing(keyid)) {
                throw std::runtime_error(std::string("startKeyListing: ") + err.asString());
            }

            Error err;
            key = kl->nextKey(err);
            if (err) {
                throw std::runtime_error(std::string("nextKey: ") + err.asString());
            }

            (void)kl->endKeyListing();
        }

        const std::auto_ptr<Context> ctx(Context::createForProtocol(proto));

        ctx->setManagedByEventLoopInteractor(true);

        Data data;
        {
            InteractiveEditInteractor *iei = new InteractiveEditInteractor;
            iei->show();
            QObject::connect(QGpgME::EventLoopInteractor::instance(), SIGNAL(operationDoneEventSignal(GpgME::Context*,GpgME::Error)),
                             iei, SLOT(close()));
            std::auto_ptr<EditInteractor> ei(iei);
            ei->setDebugChannel(stderr);
            if (Error err = ctx->startEditing(key, ei, data)) {
                throw std::runtime_error(std::string("startEditing: ") + err.asString());
            }
            // ei released in passing to startEditing
        }
        return app.exec();
    } catch (const std::exception &e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        return 1;
    }
}

#include "editinteractortest.moc"
