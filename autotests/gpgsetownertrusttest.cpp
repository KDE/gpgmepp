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
// gpgsetownertrusttest <key> {undefined|unknown|never|marginal|full|ultimate}
//

#include "test_editinteractor.h"

#include <gpgsetownertrusteditinteractor.h>

#include <boost/bind.hpp>
#include <boost/range.hpp>

#include <memory>
#include <algorithm>
#include <iostream>

using namespace GpgME;
using namespace boost;

static const struct _Values {
    const char *name;
    Key::OwnerTrust value;
} values[] = {
    { "unknown",   Key::Unknown   },
    { "undefined", Key::Undefined },
    { "never",     Key::Never     },
    { "marginal",  Key::Marginal  },
    { "full",      Key::Full      },
    { "ultimate",  Key::Ultimate  },
};

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);

    if (argc != 3) {
        return 1;
    }

    const char *const keyid = argv[1];
    const std::string ownertrust_string = argv[2];

    try {
        const _Values *const it = std::find_if(begin(values), end(values), bind(&_Values::name, _1) == ownertrust_string);
        if (it == end(values)) {
            throw std::runtime_error("Not a valid ownertrust value: \"" + ownertrust_string + "\"");
        }
        const Key::OwnerTrust ownertrust = it->value;

        std::auto_ptr<EditInteractor> ei(new GpgSetOwnerTrustEditInteractor(ownertrust));

        return test_editinteractor(ei, keyid);
    } catch (const std::exception &e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        return 1;
    }
}

#include "moc_test_editinteractor.cpp"
