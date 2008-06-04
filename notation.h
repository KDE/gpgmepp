/*
  notation.h - wraps a gpgme verify result
  Copyright (C) 2004, 2007 Klarälvdalens Datakonsult AB

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

#ifndef __GPGMEPP_NOTATION_H__
#define __GPGMEPP_NOTATION_H__

#include <gpgme++/gpgmefw.h>
#include <gpgme++/verificationresult.h>
#include <gpgme++/gpgme++_export.h>

#include <boost/shared_ptr.hpp>

namespace GpgME {

  class GPGMEPP_EXPORT Notation {
    friend class ::GpgME::Signature;
    Notation( const boost::shared_ptr<VerificationResult::Private> & parent, unsigned int sindex, unsigned int nindex );
  public:
    Notation();
    explicit Notation( gpgme_sig_notation_t nota );

    const Notation & operator=( Notation other ) {
	swap( other );
	return *this;
    }

    void swap( Notation & other ) {
	using std::swap;
	swap( this->d, other.d );
    }

    bool isNull() const;

    const char * name() const;
    const char * value() const;

    enum Flags {
	NoFlags = 0,
	HumanReadable = 1,
	Critical = 2
    };
    Flags flags() const;

    bool isHumanReadable() const;
    bool isCritical() const;

  private:
    class Private;
    boost::shared_ptr<Private> d;
  };

}

GPGMEPP_MAKE_STD_SWAP_SPECIALIZATION( Notation )

#endif // __GPGMEPP_NOTATION_H__
