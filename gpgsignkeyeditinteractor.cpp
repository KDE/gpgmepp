/*
  gpgsignkeyeditinteractor.cpp - Edit Interactor to change the expiry time of an OpenPGP key
  Copyright (C) 2007 Klarälvdalens Datakonsult AB

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

#include "gpgsignkeyeditinteractor.h"
#include "error.h"
#include "key.h"

#include <gpgme.h>

#include <string>

#include <cstring>

using std::strcmp;

// avoid conflict (msvc)
#ifdef ERROR
# undef ERROR
#endif

using namespace GpgME;

class GpgSignKeyEditInteractor::Private {
public:
    Private( GpgSignKeyEditInteractor::SigningOption opt ) : option( opt ), checkLevel( "0" ) {}
    const GpgSignKeyEditInteractor::SigningOption option;
    const std::string checkLevel;
};
GpgSignKeyEditInteractor::GpgSignKeyEditInteractor( SigningOption opt )
    : EditInteractor(), d( new Private( opt ) )
{

}

GpgSignKeyEditInteractor::~GpgSignKeyEditInteractor() {
    delete d;
}

// work around --enable-final
namespace GpgSignKeyEditInteractor_Private {
enum {
    START = EditInteractor::StartState,
    COMMAND,
    UIDS,
    SET_EXPIRE,
    SET_CHECK_LEVEL,
    CONFIRM,
    QUIT,
    SAVE,

    ERROR = EditInteractor::ErrorState
};
}

const char * GpgSignKeyEditInteractor::action( Error & err ) const {

    using namespace GpgSignKeyEditInteractor_Private;

    switch ( state() ) {
    case COMMAND:
        return d->option == LocalSignature ? "lsign" : "sign";
    case UIDS:
        return "Y";
    case SET_EXPIRE:
        return "Y";
    case SET_CHECK_LEVEL:
        return d->checkLevel.c_str();
    case CONFIRM:
        return "Y";
    case QUIT:
        return "quit";
    case SAVE:
        return "Y";
    case ERROR:
    default:
        err = Error( gpg_error( GPG_ERR_GENERAL ) );
        return 0;
    }
}

unsigned int GpgSignKeyEditInteractor::nextState( unsigned int status, const char * args, Error & err ) const {

    static const Error GENERAL_ERROR(  gpg_error( GPG_ERR_GENERAL  ) );
    //static const Error INV_TIME_ERROR( gpg_error( GPG_ERR_INV_TIME ) );

    if ( needsNoResponse( status ) )
        return state();

    using namespace GpgSignKeyEditInteractor_Private;

    switch ( state() ) {
    case START:
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "keyedit.prompt" ) == 0 )
            return COMMAND;
        err = GENERAL_ERROR;
        return ERROR;
    case COMMAND:
        if ( status == GPGME_STATUS_GET_BOOL &&
             strcmp( args, "keyedit.sign_all.okay" ) == 0 )
            return UIDS;
        //no break
    case UIDS:
        if ( status == GPGME_STATUS_GET_BOOL &&
             strcmp( args, "sign_uid.okay" ) == 0 )
            return CONFIRM;
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "sign_uid.expire" ) == 0 )
            return SET_EXPIRE;
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "sign_uid.class" ) == 0 )
            return SET_CHECK_LEVEL;
        if ( status == GPGME_STATUS_ALREADY_SIGNED ) {
            err = Error( GPG_ERR_CONFLICT );
            return ERROR;
        }
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "keyedit.prompt" ) == 0 ) {
            err = Error( GPG_ERR_UNUSABLE_PUBKEY );
            return ERROR;
        }
        err = GENERAL_ERROR;
        return ERROR;
    case SET_EXPIRE:
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "sign_uid.class" ) == 0 )
            return SET_CHECK_LEVEL;
        err = GENERAL_ERROR;
        return ERROR;
    case SET_CHECK_LEVEL:
        if ( status == GPGME_STATUS_GET_BOOL &&
             strcmp( args, "sign_uid.okay" ) == 0 )
            return CONFIRM;
        err = GENERAL_ERROR;
         return ERROR;
    case QUIT:
        if ( status == GPGME_STATUS_GET_BOOL &&
             strcmp( args, "keyedit.save.okay" ) == 0 )
            return SAVE;
        err = GENERAL_ERROR;
        return ERROR;
    case CONFIRM:
    case ERROR:
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "keyedit.prompt" ) == 0 )
            return QUIT;
        err = lastError();
        return ERROR;
    default:
        err = GENERAL_ERROR;
        return ERROR;
    }
}
