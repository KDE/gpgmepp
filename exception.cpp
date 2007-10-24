/*
  exception.cpp - exception wrapping a gpgme error
  Copyright (C) 2007 Klarälvdalens Datakonsult AB

  This file is part of GPGME++.

  GPGME++ is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  GPGME++ is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with GPGME++; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

// -*- c++ -*-
#include "exception.h"

#include <gpg-error.h>

#include <cstdio>

using namespace GpgME;
using namespace std; // only safe b/c it's so small a file!

Exception::~Exception() throw() {}

// static
string Exception::make_message( const Error & err, const string & msg ) {
    char buf[128];
    buf[0] = '\0';
    gpg_strerror_r( err, buf, sizeof buf );
    buf[sizeof buf - 1] = '\0';
    char result_buf[256];
    int numChars = 0;
    if ( msg.empty() )
        numChars = snprintf( result_buf, sizeof result_buf, "%lu %s: %s",
                             static_cast<unsigned long>(err), gpg_strsource( err ), buf  );
    else
        numChars = snprintf( result_buf, sizeof result_buf, "%lu %s: %s - %s",
                             static_cast<unsigned long>(err), gpg_strsource( err ), buf, msg.c_str() );
    result_buf[sizeof result_buf - 1] = '\0';
    if ( numChars < 0 )
        numChars = 0; // error - truncate string
    if ( numChars > static_cast<int>(sizeof result_buf) )
        numChars = sizeof result_buf;
    return string( result_buf, numChars );
}
