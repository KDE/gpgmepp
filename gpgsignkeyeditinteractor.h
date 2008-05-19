/*
  gpgsignkeyeditinteractor.h - Edit Interactor to change the owner trust of an OpenPGP key
  Copyright (C) 2008 Klarälvdalens Datakonsult AB

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

#ifndef __GPGMEPP_GPGSIGNKEYEDITINTERACTOR_H__
#define __GPGMEPP_GPGSIGNKEYEDITINTERACTOR_H__

#include <gpgme++/editinteractor.h>

#include <string>
#include <vector>

namespace GpgME {

    class Key;
    class UserID;

    class GPGMEPP_EXPORT GpgSignKeyEditInteractor : public EditInteractor {
    public:
        enum SigningOption {
            LocalSignature,
            ExportableSignature
        };

        GpgSignKeyEditInteractor( const std::vector<UserID> & userIDsToSign,
                                  const Key & secretKey,
                                  unsigned int checkLevel,
                                  SigningOption option );
        ~GpgSignKeyEditInteractor();

    private:
        /* reimp */ const char * action( Error & err ) const;
        /* reimp */ unsigned int nextState( const unsigned int statusCode, const char * args, Error & err ) const;

    private:
        class Private;
        Private * const d;
    };

} // namespace GpgME

#endif // __GPGMEPP_GPGSIGNKEYEDITINTERACTOR_H__
