/*
  gpgsetexpirytime.cpp - Edit Interactor to change the expiry time of an OpenPGP key
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

#include "gpgsetexpirytime.h"
#include "error.h"

#include <gpgme.h>

using namespace GpgME;

GpgSetExpiryTime::GpgSetExpiryTime( const std::string & t )
    : EditInteractor(),
      m_strtime( t )
{

}

GpgSetExpiryTime::~GpgSetExpiryTime() {}

enum {
    START = EditInteractor::StartState,
    COMMAND,
    DATE,
    QUIT,
    SAVE,

    ERROR = EditInteractor::ErrorState
};

const char * GpgSetExpiryTime::action() const {
    switch ( state() ) {
    case COMMAND:
        return "expire";
    case DATE:
        return m_strtime.c_str();
    case QUIT:
        return "quit";
    case SAVE:
        return "Y";
    case START:
    case ERROR:
        return 0;
    default:
        throw gpg_error( GPG_ERR_GENERAL );
    }
}

unsigned int GpgSetExpiryTime::nextState( unsigned int status, const char * args ) const {

    static const Error GENERAL_ERROR(  gpg_error( GPG_ERR_GENERAL  ) );
    static const Error INV_TIME_ERROR( gpg_error( GPG_ERR_INV_TIME ) );

    if ( needsNoResponse( status ) )
        return state();

    switch ( state() ) {
    case START:
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "keyedit.prompt" ) == 0 )
            return COMMAND;
        else
            throw GENERAL_ERROR;
    case COMMAND:
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "keygen.valid" ) == 0 )
            return DATE;
        else
            throw GENERAL_ERROR;
    case DATE:
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "keyedit.prompt" ) == 0 )
            return QUIT;
        else if ( status == GPGME_STATUS_GET_LINE &&
                  strcmp( args, "keygen.valid" ) )
            throw INV_TIME_ERROR;
        else
            throw GENERAL_ERROR;
    case QUIT:
        if ( status == GPGME_STATUS_GET_BOOL &&
             strcmp( args, "keyedit.save.okay" ) == 0 )
            return SAVE;
        else
            throw GENERAL_ERROR;
    case ERROR:
        if ( status == GPGME_STATUS_GET_LINE &&
             strcmp( args, "keyedit.prompt" ) == 0 )
            return QUIT;
        else
            throw lastError();
    default:
        throw GENERAL_ERROR;
    };
}
