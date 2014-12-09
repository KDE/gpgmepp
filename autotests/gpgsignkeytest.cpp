/* tests/gpgsignkeytest.cpp
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
// gpgsignkeytest <key>
//

#include "test_editinteractor.h"

#include <gpgsignkeyeditinteractor.h>

#include <QByteArray>

#include <memory>
#include <algorithm>
#include <iostream>

using namespace GpgME;
using namespace boost;

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);

    if (argc != 3) {
        return 1;
    }

    const char *const keyid = argv[1];
    QByteArray signing_mode_string = argv[2];
    if (!signing_mode_string.endsWith("sign")) {
        throw std::runtime_error(std::string("Not a valid signing mode: ") + argv[2]);
    }
    signing_mode_string.chop(4);
    int options = 0;
    if (!signing_mode_string.contains('l')) {
        options |= GpgSignKeyEditInteractor::Exportable;
    }
    if (signing_mode_string.contains("nr")) {
        options |= GpgSignKeyEditInteractor::NonRevocable;
    }
    if (signing_mode_string.contains('t')) {
        options |= GpgSignKeyEditInteractor::Trust;
    }
    try {
        GpgSignKeyEditInteractor *const skei = new GpgSignKeyEditInteractor;
        skei->setSigningOptions(options);
        std::auto_ptr<EditInteractor> ei(skei);
        return test_editinteractor(ei, keyid);
    } catch (const std::exception &e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        return 1;
    }
}

#include "moc_test_editinteractor.cpp"
