/*
  encryptionresult.h - wraps a gpgme sign result
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

#ifndef __GPGMEPP_ENCRYPTIONRESULT_H__
#define __GPGMEPP_ENCRYPTIONRESULT_H__

#include <gpgme++/gpgmefw.h>
#include <gpgme++/result.h>
#include <gpgme++/gpgme++_export.h>

#include <vector>

namespace GpgME {

  class Error;
  class InvalidRecipient;

  class GPGMEPP_EXPORT EncryptionResult : public Result {
  public:
    explicit EncryptionResult( gpgme_ctx_t ctx=0, int error=0 );
    explicit EncryptionResult( gpgme_ctx_t ctx, const Error & error );
    explicit EncryptionResult( const Error & err );
    EncryptionResult( const EncryptionResult & other );
    ~EncryptionResult();

    const EncryptionResult & operator=( EncryptionResult other ) {
	swap( other );
	return *this;
    }

    void swap( EncryptionResult & other ) {
	Result::swap( other );
	using std::swap;
	swap( this->d, other.d );
    }

    bool isNull() const;

    unsigned int numInvalidRecipients() const;

    InvalidRecipient invalidEncryptionKey( unsigned int index ) const;
    std::vector<InvalidRecipient> invalidEncryptionKeys() const;

    class Private;
  private:
    void init( gpgme_ctx_t ctx );
    Private * d;
  };

  class GPGMEPP_EXPORT InvalidRecipient {
    friend class ::GpgME::EncryptionResult;
    InvalidRecipient( EncryptionResult::Private * parent, unsigned int index );
  public:
    InvalidRecipient();
    InvalidRecipient( const InvalidRecipient & other );
    ~InvalidRecipient();

    const InvalidRecipient & operator=( InvalidRecipient other ) {
	swap( other );
	return *this;
    }

    void swap( InvalidRecipient & other ) {
	using std::swap;
	swap( this->d, other.d );
    }

    bool isNull() const;

    const char * fingerprint() const;
    Error reason() const;

  private:
    EncryptionResult::Private * d;
    unsigned int idx;
  };

}

GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( EncryptionResult )
GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( InvalidRecipient )

#endif // __GPGMEPP_ENCRYPTIONRESULT_H__
