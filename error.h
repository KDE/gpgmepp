/*
  error.h - wraps a gpgme error
  Copyright (C) 2003, 2007 Klarälvdalens Datakonsult AB

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
#ifndef __GPGMEPP_ERROR_H__
#define __GPGMEPP_ERROR_H__

#include <gpgme++/gpgmefw.h>
#include <gpgme++/gpgme++_export.h>

namespace GpgME {

  class GPGMEPP_EXPORT Error {
  public:
    explicit Error( int e=0 ) : mErr( e ) {}

    const char * source() const;
    const char * asString() const;

    int code() const;
    int sourceID() const;

    bool isCanceled() const;

    operator int() const { return mErr; }
    operator bool() const { return mErr && !isCanceled(); }
  private:
    int mErr;
  };

} // namespace GpgME

#endif /* __GPGMEPP_ERROR_H__ */
