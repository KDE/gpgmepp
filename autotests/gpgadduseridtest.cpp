/* tests/gpgadduseridtest.cpp
   Copyright (C) 2008 Klarälvdalens Datakonsult AB

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
// gpgadduseridtest <key> <name> <email> [<comment>]
//

#include "test_editinteractor.h"

#include <gpgadduserideditinteractor.h>

#include <memory>
#include <iostream>

using namespace GpgME;

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);

    if (argc != 4 && argc != 5) {
        return 1;
    }

    const char *const keyid = argv[1];
    const std::string name =    QString::fromLocal8Bit(argv[2]).toUtf8().constData();
    const std::string email =   QString::fromLocal8Bit(argv[3]).toUtf8().constData();
    const std::string comment = argc > 4 ? QString::fromLocal8Bit(argv[4]).toUtf8().constData() : "" ;

    try {

        std::auto_ptr<GpgAddUserIDEditInteractor> ei(new GpgAddUserIDEditInteractor);

        ei->setNameUtf8(name);
        ei->setEmailUtf8(email);
        ei->setCommentUtf8(comment);

        std::auto_ptr<EditInteractor> eii(ei);
        return test_editinteractor(eii, keyid);

    } catch (const std::exception &e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        return 1;
    }
}

#include "moc_test_editinteractor.cpp"
