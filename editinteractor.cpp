/*
  editinteractor.cpp - Interface for edit interactors
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

#include "editinteractor.h"
#include "callbacks.h"
#include "error.h"

#include <gpg-error.h>

#ifdef _WIN32
# include <io.h>
# define write _write
#else
# include <unistd.h>
#endif

#include <cstring>

using namespace GpgME;

static int edit_interactor_callback_impl( void * opaque, gpgme_status_code_t status, const char * args, int fd );

class EditInteractor::Private {
    friend class ::GpgME::EditInteractor;
    EditInteractor * const q;
public:
    explicit Private( EditInteractor * qq );
    ~Private();

    friend int ::edit_interactor_callback_impl( void * opaque, gpgme_status_code_t status, const char * args, int fd ) {
        Private * ei = (Private*)opaque;

        try {
            // advance to next state based on input:
            const unsigned int oldState = ei->state;
            ei->state = ei->q->nextState( status, args );
            if ( ei->debug )
                std::fprintf( ei->debug, "EditInteractor: %u -> nextState( %u, %s ) -> %u\n",
                              oldState, (unsigned int)status, args ? args : "<null>", ei->state );
            
            if ( ei->state != oldState &&
                 // if there was an error from before, we stop here (### this looks weird, can this happen at all?)
                 gpg_err_code( ei->error.code() ) == GPG_ERR_NO_ERROR ) {

                // successful state change -> call action
                if ( const char * const result = ei->q->action() ) {
                    if ( ei->debug )
                        std::fprintf( ei->debug, "EditInteractor: action result \"%s\"\n", result );
                    // if there's a result, write it:
                    if ( *result )
                        write( fd, result, std::strlen( result ) );
                    write( fd, "\n", 1 );
                } else {
                    if ( ei->debug )
                        std::fprintf( ei->debug, "EditInteractor: no action result\n" );
                }
            } else {
                if ( ei->debug )
                    std::fprintf( ei->debug, "EditInteractor: no action executed\n" );
            }

        } catch ( const Error & err ) {
            ei->error = err;
            ei->state = EditInteractor::ErrorState;
        }

        if ( ei->debug )
            std::fprintf( ei->debug, "EditInteractor: error now %u (%s)\n",
                          ei->error.encodedError(), gpg_strerror( ei->error.encodedError() ) );

        return ei->error.encodedError();
    }

private:
    unsigned int state;
    Error error;
    std::FILE * debug;
};

static gpgme_error_t edit_interactor_callback( void * opaque, gpgme_status_code_t status, const char * args, int fd )
{
    return edit_interactor_callback_impl( opaque, status, args, fd );
}

gpgme_edit_cb_t GpgME::edit_interactor_callback = ::edit_interactor_callback;

EditInteractor::Private::Private( EditInteractor * qq )
    : q( qq ),
      state( StartState ),
      error()
{

}

EditInteractor::Private::~Private() {}

EditInteractor::EditInteractor()
    : d( new Private( this ) )
{

}

EditInteractor::~EditInteractor() {
    delete d; d = 0;
}

unsigned int EditInteractor::state() const {
    return d->state;
}

Error EditInteractor::lastError() const {
    return d->error;
}

bool EditInteractor::needsNoResponse( unsigned int status ) const {
    switch ( status ) {
    case GPGME_STATUS_EOF:
    case GPGME_STATUS_GOT_IT:
    case GPGME_STATUS_NEED_PASSPHRASE:
    case GPGME_STATUS_NEED_PASSPHRASE_SYM:
    case GPGME_STATUS_GOOD_PASSPHRASE:
    case GPGME_STATUS_BAD_PASSPHRASE:
    case GPGME_STATUS_USERID_HINT:
    case GPGME_STATUS_SIGEXPIRED:
    case GPGME_STATUS_KEYEXPIRED:
        return true;
    default:
        return false;
    }
}

void EditInteractor::setDebugChannel( std::FILE * debug ) {
    d->debug = debug;
}
