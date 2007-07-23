/*
  keylistresult.h - wraps a gpgme keylist result
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

#ifndef __GPGMEPP_KEYLISTRESULT_H__
#define __GPGMEPP_KEYLISTRESULT_H__

#include <gpgme++/gpgmefw.h>
#include <gpgme++/result.h>
#include <gpgme++/gpgme++_export.h>

namespace GpgME {

  class Error;

  class GPGMEPP_EXPORT KeyListResult : public Result {
  public:
    explicit KeyListResult( gpgme_ctx_t ctx=0, int error=0 );
    explicit KeyListResult( const Error & err );
    KeyListResult( const Error & err, const _gpgme_op_keylist_result & res );
    KeyListResult( const KeyListResult & other );
    ~KeyListResult();

    const KeyListResult & operator=( KeyListResult other ) {
	swap( other );
	return *this;
    }
    void swap( KeyListResult & other ) {
	Result::swap( other );
	using std::swap;
	swap( this->d, other.d );
    }

    const KeyListResult & operator+=( const KeyListResult & other ) {
      mergeWith( other );
      return *this;
    }

    void mergeWith( const KeyListResult & other );

    bool isNull() const;

    bool isTruncated() const;

  private:
    void detach();
    class Private;
    Private * d;
  };

}

GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( KeyListResult )

#endif // __GPGMEPP_KEYLISTRESULT_H__
