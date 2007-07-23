/*
  keygenerationresult.h - wraps a gpgme keygen result
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

#ifndef __GPGMEPP_KEYGENERATIONRESULT_H__
#define __GPGMEPP_KEYGENERATIONRESULT_H__

#include <gpgme++/gpgmefw.h>
#include <gpgme++/result.h>
#include <gpgme++/gpgme++_export.h>

namespace GpgME {

  class Error;

  class GPGMEPP_EXPORT KeyGenerationResult : public Result {
  public:
    explicit KeyGenerationResult( gpgme_ctx_t ctx=0, int error=0 );
    explicit KeyGenerationResult( const Error & err );
    KeyGenerationResult( const KeyGenerationResult & other );
    ~KeyGenerationResult();

    const KeyGenerationResult & operator=( KeyGenerationResult other ) {
	swap( other );
	return *this;
    }

    void swap( KeyGenerationResult & other ) {
	Result::swap( other );
	using std::swap;
	swap( this->d, other.d );
    }

    bool isNull() const;

    GPGMEPP_DEPRECATED bool primaryKeyGenerated() const { return isPrimaryKeyGenerated(); }
    GPGMEPP_DEPRECATED bool subkeyGenerated() const { return isSubkeyGenerated(); }
    bool isPrimaryKeyGenerated() const;
    bool isSubkeyGenerated() const;
    const char * fingerprint() const;

  private:
    class Private;
    Private * d;
  };

}

GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( KeyGenerationResult )

#endif // __GPGMEPP_KEYGENERATIONRESULT_H__
