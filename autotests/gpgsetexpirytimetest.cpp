/* tests/gpgsetexpirytimetest.cpp
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
// gpgsetexpirytimetest <key> <YYYY-MM-DD>
//

#include "test_editinteractor.h"

#include <gpgsetexpirytimeeditinteractor.h>

#include <iostream>
#include <stdexcept>

using namespace GpgME;

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);

    if (argc != 3) {
        return 1;
    }

    const char *const keyid = argv[1];
    const char *const date = argv[2];

    try {

        std::auto_ptr<EditInteractor> ei(new GpgSetExpiryTimeEditInteractor(date));

        return test_editinteractor(ei, keyid);

    } catch (const std::exception &e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        return 1;
    }
}

#include "moc_test_editinteractor.cpp"
