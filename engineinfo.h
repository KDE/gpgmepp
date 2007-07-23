/*
  engineinfo.h
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

#ifndef __GPGMEPP_ENGINEINFO_H__
#define __GPGMEPP_ENGINEINFO_H__

#include <gpgme++/gpgmefw.h>
#include <gpgme++/context.h>
#include <gpgme++/gpgme++_export.h>

#include <algorithm>

namespace GpgME {

  class GPGMEPP_EXPORT EngineInfo {
  public:
    EngineInfo();
    explicit EngineInfo( gpgme_engine_info_t engine );
    EngineInfo( const EngineInfo & other );
    ~EngineInfo();

    const EngineInfo & operator=( EngineInfo other ) {
	swap( other );
	return *this;
    }

    void swap( EngineInfo & other ) {
	using std::swap;
	swap( this->d, other.d );
    }

    bool isNull() const;

    Context::Protocol protocol() const;
    const char * fileName() const;
    const char * version() const;
    const char * requiredVersion() const;
  private:
    class Private;
    Private * d;
  };

}

GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( EngineInfo )

#endif // __GPGMEPP_ENGINEINFO_H__
