/* tests/dataprovidertest.cpp
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

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <qgpgme/dataprovider.h>
#include <data.h>
#include <data_p.h>
#include <engineinfo.h>
#include <gpgme.h>
#include <iostream>

#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

using namespace GpgME;

void searchEngine(gpgme_engine_info_t &ei, gpgme_protocol_t p)
{
    EngineInfo result;
    bool foundEngine = false;
    int nbEngine = 0;
    printf("\n");
    for (gpgme_engine_info_t i = ei ; i ; i = i->next, ++nbEngine) {
        printf("Engine (%d) \n", nbEngine);
        if (i->protocol == p) {
            printf("engine info found for %s\n", (p == GPGME_PROTOCOL_OpenPGP) ? "OpenPGP" : "CMS");
            result = EngineInfo(i);
            foundEngine = true;
            printf("isNull ? %s\n", result.isNull() ? "true" : "false");
            printf("home_dir %s\n", result.homeDirectory());
            printf("version %s\n", result.version());
            printf("requiredVersion %s\n", result.requiredVersion());
            printf("fileName %s\n\n", result.fileName());
        }
    }
    if (!foundEngine) {
        printf("engine info not found for %s\n", (p == GPGME_PROTOCOL_OpenPGP) ? "OpenPGP" : "CMS");
    }
}

int main(int, char **)
{

    printf("GPGME_PROTOCOL_CMS: %s\n",
           gpgme_strerror(gpgme_engine_check_version(GPGME_PROTOCOL_CMS)));
    printf("GPGME_PROTOCOL_OpenPGP: %s\n",
           gpgme_strerror(gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP)));
    gpgme_engine_info_t ei = 0;
    if (gpgme_get_engine_info(&ei)) {
        printf("error not engine info found \n");
    } else {
        searchEngine(ei, GPGME_PROTOCOL_OpenPGP);
        searchEngine(ei, GPGME_PROTOCOL_CMS);
    }
    return 0;
}
