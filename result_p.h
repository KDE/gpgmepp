/*
  result.h - base class for results
  Copyright (C) 2004 Klarälvdalens Datakonsult AB

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

#ifndef __GPGMEPP_RESULT_P_H__
#define __GPGMEPP_RESULT_P_H__

#define make_copy_ctor(x) \
GpgME::x::x( const x & other ) \
  : GpgME::Result( other ), d( other.d ) \
{ \
  if ( d ) \
    d->ref(); \
}

#define make_error_ctor(x) \
GpgME::x::x( const Error & error ) \
  : GpgME::Result( error ), d( 0 ) \
{ \
 \
}

#define make_dtor(x) \
GpgME::x::~x() { \
  if ( d ) \
    d->unref(); \
  d = 0; \
}

#define make_isNull(x) bool GpgME::x::isNull() const { return !d && !error(); }

#define make_standard_stuff(x) \
make_copy_ctor(x) \
make_error_ctor(x) \
make_isNull(x) \
make_dtor(x)


#endif // __GPGMEPP_RESULT_P_H__
