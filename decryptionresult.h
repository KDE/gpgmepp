/*
  decryptionresult.h - wraps a gpgme keygen result
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

#ifndef __GPGMEPP_DECRYPTIONRESULT_H__
#define __GPGMEPP_DECRYPTIONRESULT_H__

#include <gpgme++/gpgmefw.h>
#include <gpgme++/result.h>
#include <gpgme++/gpgme++_export.h>

#include <vector>

namespace GpgME {

  class Error;

  class GPGMEPP_EXPORT DecryptionResult : public Result {
  public:
    explicit DecryptionResult( gpgme_ctx_t ctx=0, int error=0 );
    explicit DecryptionResult( const Error & err );
    DecryptionResult( const DecryptionResult & other );
    ~DecryptionResult();

    const DecryptionResult & operator=( DecryptionResult other ) {
	swap( other );
	return *this;
    }

    void swap( DecryptionResult & other ) {
	Result::swap( other );
	using std::swap;
	swap( this->d, other.d );
    }

    bool isNull() const;

    GPGMEPP_DEPRECATED const char * unsupportedAlgortihm() const { return unsupportedAlgorithm(); }
    const char * unsupportedAlgorithm() const;

    GPGMEPP_DEPRECATED bool wrongKeyUsage() const { return isWrongKeyUsage(); }
    bool isWrongKeyUsage() const;

    const char * fileName() const;

    class Recipient;

    unsigned int numRecipients() const;
    Recipient recipient( unsigned int idx ) const;
    std::vector<Recipient> recipients() const;

  private:
    class Private;
    Private * d;
  };

  class DecryptionResult::Recipient {
  public:
      explicit Recipient( gpgme_recipient_t reci=0 );
      Recipient( const Recipient & other );
      ~Recipient();

      const Recipient & operator=( Recipient other ) {
	  swap( other );
	  return *this;
      }

      void swap( Recipient & other ) {
	  using std::swap;
	  swap( this->d, other.d );
      }

      bool isNull() const;

      const char * keyID() const;
      const char * shortKeyID() const;

      unsigned int publicKeyAlgorithm() const;
      const char * publicKeyAlgorithmAsString() const;

      Error status() const;

  private:
    class Private;
    Private * d;
  };

}

GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( DecryptionResult )

#endif // __GPGMEPP_DECRYPTIONRESULT_H__
