/*
  verificationresult.h - wraps a gpgme verify result
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

#ifndef __GPGMEPP_VERIFICATIONRESULT_H__
#define __GPGMEPP_VERIFICATIONRESULT_H__

#include <gpgme++/gpgmefw.h>
#include <gpgme++/result.h>
#include <gpgme++/gpgme++_export.h>

#include <time.h>

#include <vector>

namespace GpgME {

  class Error;
  class Signature;

  class GPGMEPP_EXPORT VerificationResult : public Result {
  public:
    explicit VerificationResult( gpgme_ctx_t ctx=0, int error=0 );
    explicit VerificationResult( const Error & err );
    VerificationResult( const VerificationResult & other );
    ~VerificationResult();

    const VerificationResult & operator=( VerificationResult other ) {
	swap( other );
	return *this;
    }

    bool isNull() const;

    Signature signature( unsigned int index ) const;
    std::vector<Signature> signatures() const;

    void swap( VerificationResult & other ) {
	Result::swap( other );
	using std::swap;
	swap( this->d, other.d );
    }

    class Private;
  private:
    Private * d;
  };

  class GPGMEPP_EXPORT Signature {
    friend class VerificationResult;
    Signature( VerificationResult::Private * parent, unsigned int index );
  public:
    class Notation;

    Signature();
    Signature( const Signature & other );
    ~Signature();

    const Signature & operator=( Signature other ) {
	swap( other );
	return *this;
    }

    void swap( Signature & other ) {
	using std::swap;
	swap( this->d, other.d );
	swap( this->idx, other.idx );
    }

    bool isNull() const;


    enum Summary {
      None       = 0x000,
      Valid      = 0x001,
      Green      = 0x002,
      Red        = 0x004,
      KeyRevoked = 0x008,
      KeyExpired = 0x010,
      SigExpired = 0x020,
      KeyMissing = 0x040,
      CrlMissing = 0x080,
      CrlTooOld  = 0x100,
      BadPolicy  = 0x200,
      SysError   = 0x400
    };
    Summary summary() const;

    const char * fingerprint() const;

    Error status() const;

    time_t creationTime() const;
    time_t expirationTime() const;
    bool neverExpires() const;

    bool wrongKeyUsage() const;

    enum Validity {
      Unknown, Undefined, Never, Marginal, Full, Ultimate
    };
    Validity validity() const;
    char validityAsString() const;
    Error nonValidityReason() const;

    Notation notation( unsigned int index ) const;
    std::vector<Notation> notations() const;

  private:
    VerificationResult::Private * d;
    unsigned int idx;
  };

  class GPGMEPP_EXPORT Signature::Notation {
    friend class Signature;
    Notation( VerificationResult::Private * parent, unsigned int sindex, unsigned int nindex );
  public:
    Notation();
    Notation( const Notation & other );
    ~Notation();

    const Notation & operator=( Notation other ) {
	swap( other );
	return *this;
    }

    void swap( Notation & other ) {
	using std::swap;
	swap( this->d, other.d );
	swap( this->sidx, other.sidx );
	swap( this->nidx, other.nidx );
    }

    bool isNull() const;

    const char * name() const;
    const char * value() const;

  private:
    VerificationResult::Private * d;
    unsigned int sidx;
    unsigned int nidx;
  };

}

GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( VerificationResult )
GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( Signature )
GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( Signature::Notation )

#endif // __GPGMEPP_VERIFICATIONRESULT_H__
