/*
  context_glib.cpp - wraps a gpgme key context, gpgme-glib-specific functions
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

#include <config-gpgme++.h>

#include <gpgme++/global.h>

#ifdef HAVE_GPGME_GET_FDPTR
extern "C" GIOChannel * gpgme_get_fdptr( int );
#endif

GIOChannel * GpgME::getGIOChannel( int fd ) {
#ifdef HAVE_GPGME_GET_FDPTR
    return gpgme_get_fdptr( fd );
#else
    (void)fd;
    return 0;
#endif
}

QIODevice * GpgME::getQIODevice( int fd ) {
    return 0;
}

