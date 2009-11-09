/*
  context.cpp - wraps a gpgme key context
  Copyright (C) 2003, 2007 Klarälvdalens Datakonsult AB

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

#include <config-gpgme++.h>

#include <gpgme++/context.h>
#include <gpgme++/eventloopinteractor.h>
#include <gpgme++/trustitem.h>
#include <gpgme++/assuanresult.h>
#include <gpgme++/keylistresult.h>
#include <gpgme++/keygenerationresult.h>
#include <gpgme++/importresult.h>
#include <gpgme++/decryptionresult.h>
#include <gpgme++/verificationresult.h>
#include <gpgme++/signingresult.h>
#include <gpgme++/encryptionresult.h>
#include <gpgme++/engineinfo.h>
#include <gpgme++/editinteractor.h>
#include <gpgme++/vfsmountresult.h>

#include <gpgme++/interfaces/assuantransaction.h>
#include <gpgme++/defaultassuantransaction.h>

#include "callbacks.h"
#include "data_p.h"
#include "context_p.h"
#include "util.h"

#include <gpgme.h>

#include <boost/scoped_array.hpp>

#include <istream>
#ifndef NDEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

#include <cassert>

namespace GpgME {

  static inline gpgme_error_t makeError( gpg_err_code_t code ) {
    return gpg_err_make( (gpg_err_source_t)22, code );
  }

  static inline unsigned int xtoi_1( const char * str ) {
      const unsigned int ch = *str;
      const unsigned int result =
          ch <= '9' ? ch - '0' :
          ch <= 'F' ? ch - 'A' + 10 :
          /* else */  ch - 'a' + 10 ;
      return result < 16 ? result : 0 ;
  }
  static inline int xtoi_2( const char * str ) {
      return xtoi_1( str ) * 16U + xtoi_1( str+1 );
  }

  static void percent_unescape( std::string & s, bool plus2space ) {
      std::string::iterator src = s.begin(), dest = s.begin(), end = s.end();
      while ( src != end )
          if ( *src == '%' && end - src > 2 ) {
              *dest++ = xtoi_2( &*++src );
              src += 2;
          } else if ( *src == '+' && plus2space ) {
              *dest++ = ' ';
              ++src;
          } else {
              *dest++ = *src++;
          }
      s.erase( dest, end );
  }

  void initializeLibrary() {
      gpgme_check_version( 0 );
  }

  Error initializeLibrary( int ) {
      if ( gpgme_check_version( GPGME_VERSION ) )
          return Error();
      else
          return Error( gpg_error( GPG_ERR_USER_1 ) );
  }

  static void format_error( gpgme_error_t err, std::string & str ) {
    char buffer[ 1024 ];
    gpgme_strerror_r( err, buffer, sizeof buffer );
    buffer[ sizeof buffer - 1 ] = '\0';
    str = buffer;
  }

  const char * Error::source() const {
    return gpgme_strsource( (gpgme_error_t)mErr );
  }

  const char * Error::asString() const {
    if ( mMessage.empty() )
      format_error( static_cast<gpgme_error_t>( mErr ), mMessage );
    return mMessage.c_str();
  }

  int Error::code() const {
    return gpgme_err_code( mErr );
  }

  int Error::sourceID() const {
    return gpgme_err_source( mErr );
  }

  bool Error::isCanceled() const {
    return code() == GPG_ERR_CANCELED;
  }

  std::ostream & operator<<( std::ostream & os, const Error & err ) {
      return os << "GpgME::Error(" << err.encodedError() << " (" << err.asString() << "))";
  }

  Context::Context( gpgme_ctx_t ctx ) {
    d = new Private( ctx );
  }

  Context::~Context() {
    delete d; d = 0;
  }

  Context * Context::createForProtocol( Protocol proto ) {
    gpgme_ctx_t ctx = 0;
    if ( gpgme_new ( &ctx ) != 0 )
      return 0;

    switch ( proto ) {
    case OpenPGP:
      if ( gpgme_set_protocol( ctx, GPGME_PROTOCOL_OpenPGP ) != 0 ) {
	gpgme_release( ctx );
	return 0;
      }
      break;
    case CMS:
      if ( gpgme_set_protocol( ctx, GPGME_PROTOCOL_CMS ) != 0 ) {
	gpgme_release( ctx );
	return 0;
      }
      break;
#ifdef HAVE_GPGME_G13_VFS
    case G13:
      if ( gpgme_set_protocol( ctx, GPGME_PROTOCOL_G13 ) != 0 ) {
        gpgme_release( ctx );
        return 0;
      }
      break;
#endif
    default:
      return 0;
    }

    return new Context( ctx );
  }

  std::auto_ptr<Context> Context::createForEngine( Engine eng, Error * error ) {
    gpgme_ctx_t ctx = 0;
    if ( const gpgme_error_t err = gpgme_new( &ctx ) ) {
        if ( error )
            *error = Error( err );
        return std::auto_ptr<Context>();
    }

    switch ( eng ) {
    case AssuanEngine:
#ifdef HAVE_GPGME_ASSUAN_ENGINE
      if ( const gpgme_error_t err = gpgme_set_protocol( ctx, GPGME_PROTOCOL_ASSUAN ) ) {
        gpgme_release( ctx );
        if ( error )
          *error = Error( err );
        return std::auto_ptr<Context>();
      }
      break;
#else
      if ( error )
        *error = Error( gpg_error( GPG_ERR_NOT_SUPPORTED ) );
      return std::auto_ptr<Context>();
#endif
    default:
      if ( error )
        *error = Error( gpg_error( GPG_ERR_INV_ARG ) );
      return std::auto_ptr<Context>();
    }

    if ( error )
        *error = Error();

    return std::auto_ptr<Context>( new Context( ctx ) );
  }

  //
  //
  // Context::Private
  //
  //

  Context::Private::Private( gpgme_ctx_t c )
      : ctx( c ),
        iocbs( 0 ),
        lastop( None ),
        lasterr( GPG_ERR_NO_ERROR ),
        lastAssuanInquireData( Data::null ),
        lastAssuanTransaction(),
        lastEditInteractor(),
        lastCardEditInteractor()
  {

  }

  Context::Private::~Private() {
      if ( ctx )
          gpgme_release( ctx );
      ctx = 0;
      delete iocbs;
  }

  //
  //
  // Context attributes:
  //
  //

  Protocol Context::protocol() const {
    gpgme_protocol_t p = gpgme_get_protocol( d->ctx );
    switch ( p ) {
    case GPGME_PROTOCOL_OpenPGP: return OpenPGP;
    case GPGME_PROTOCOL_CMS:     return CMS;
    default:                     return UnknownProtocol;
    }
  }


  void Context::setArmor( bool useArmor ) {
    gpgme_set_armor( d->ctx, int( useArmor ) );
  }
  bool Context::armor() const {
    return gpgme_get_armor( d->ctx );
  }

  void Context::setTextMode( bool useTextMode ) {
    gpgme_set_textmode( d->ctx, int( useTextMode ) );
  }
  bool Context::textMode() const {
    return gpgme_get_textmode( d->ctx );
  }

  void Context::setIncludeCertificates( int which ) {
    if ( which == DefaultCertificates ) {
#ifdef HAVE_GPGME_INCLUDE_CERTS_DEFAULT
      which = GPGME_INCLUDE_CERTS_DEFAULT;
#else
      which = 1;
#endif
    }
    gpgme_set_include_certs( d->ctx, which );
  }

  int Context::includeCertificates() const {
    return gpgme_get_include_certs( d->ctx );
  }

  void Context::setKeyListMode( unsigned int mode ) {
    gpgme_set_keylist_mode( d->ctx, add_to_gpgme_keylist_mode_t( 0, mode ) );
  }

  void Context::addKeyListMode( unsigned int mode ) {
    const unsigned int cur = gpgme_get_keylist_mode( d->ctx );
    gpgme_set_keylist_mode( d->ctx, add_to_gpgme_keylist_mode_t( cur, mode ) );
  }


  unsigned int Context::keyListMode() const {
    return convert_from_gpgme_keylist_mode_t( gpgme_get_keylist_mode( d->ctx ) );
  }

  void Context::setProgressProvider( ProgressProvider * provider ) {
    gpgme_set_progress_cb( d->ctx, provider ? &progress_callback : 0, provider );
  }
  ProgressProvider * Context::progressProvider() const {
    void * pp = 0;
    gpgme_progress_cb_t pcb = &progress_callback;
    gpgme_get_progress_cb( d->ctx, &pcb, &pp );
    return static_cast<ProgressProvider*>( pp );
  }

  void Context::setPassphraseProvider( PassphraseProvider * provider ) {
    gpgme_set_passphrase_cb( d->ctx, provider ? &passphrase_callback : 0, provider );
  }

  PassphraseProvider * Context::passphraseProvider() const {
    void * pp = 0;
    gpgme_passphrase_cb_t pcb = &passphrase_callback;
    gpgme_get_passphrase_cb( d->ctx, &pcb, &pp );
    return static_cast<PassphraseProvider*>( pp );
  }

  void Context::setManagedByEventLoopInteractor( bool manage ) {
    if ( !EventLoopInteractor::instance() ) {
#ifndef NDEBUG
      cerr << "Context::setManagedByEventLoopInteractor(): "
	      "You must create an instance of EventLoopInteractor "
	      "before using anything that needs one." << endl;
#endif
      return;
    }
    if ( manage )
      EventLoopInteractor::instance()->manage( this );
    else
      EventLoopInteractor::instance()->unmanage( this );
  }
  bool Context::managedByEventLoopInteractor() const {
    return d->iocbs != 0;
  }


  void Context::installIOCallbacks( gpgme_io_cbs * iocbs ) {
    if ( !iocbs ) {
      uninstallIOCallbacks();
      return;
    }
    gpgme_set_io_cbs( d->ctx, iocbs );
    delete d->iocbs; d->iocbs = iocbs;
  }

  void Context::uninstallIOCallbacks() {
    static gpgme_io_cbs noiocbs = { 0, 0, 0, 0, 0 };
    // io.add == 0 means disable io callbacks:
    gpgme_set_io_cbs( d->ctx, &noiocbs );
    delete d->iocbs; d->iocbs = 0;
  }

  Error Context::setLocale( int cat, const char * val ) {
    return Error( d->lasterr = gpgme_set_locale( d->ctx, cat, val ) );
  }

  EngineInfo Context::engineInfo() const {
#ifdef HAVE_GPGME_CTX_GETSET_ENGINE_INFO
      return EngineInfo( gpgme_ctx_get_engine_info( d->ctx ) );
#else
      return EngineInfo();
#endif
  }

  Error Context::setEngineFileName( const char * filename ) {
#ifdef HAVE_GPGME_CTX_GETSET_ENGINE_INFO
      const char * const home_dir = engineInfo().homeDirectory();
      return Error( gpgme_ctx_set_engine_info( d->ctx, gpgme_get_protocol( d->ctx ), filename, home_dir ) );
#else
      return Error( makeError( GPG_ERR_NOT_IMPLEMENTED ) );
#endif
  }

  Error Context::setEngineHomeDirectory( const char * home_dir ) {
#ifdef HAVE_GPGME_CTX_GETSET_ENGINE_INFO
      const char * const filename = engineInfo().fileName();
      return Error( gpgme_ctx_set_engine_info( d->ctx, gpgme_get_protocol( d->ctx ), filename, home_dir ) );
#else
      return Error( makeError( GPG_ERR_NOT_IMPLEMENTED ) );
#endif
  }

  //
  //
  // Key Management
  //
  //

  Error Context::startKeyListing( const char * pattern, bool secretOnly ) {
    d->lastop = Private::KeyList;
    return Error( d->lasterr = gpgme_op_keylist_start( d->ctx, pattern, int( secretOnly ) ) );
  }

  Error Context::startKeyListing( const char * patterns[], bool secretOnly ) {
    d->lastop = Private::KeyList;
#ifndef HAVE_GPGME_EXT_KEYLIST_MODE_EXTERNAL_NONBROKEN
    if ( !patterns || !patterns[0] || !patterns[1] )
      // max. one pattern -> use the non-ext version
      return startKeyListing( patterns ? patterns[0] : 0, secretOnly );
#endif
    return Error( d->lasterr = gpgme_op_keylist_ext_start( d->ctx, patterns, int( secretOnly ), 0 ) );
  }

  Key Context::nextKey( GpgME::Error & e ) {
    d->lastop = Private::KeyList;
    gpgme_key_t key;
    e = Error( d->lasterr = gpgme_op_keylist_next( d->ctx, &key ) );
    return Key( key, false );
  }

  KeyListResult Context::endKeyListing() {
    d->lasterr = gpgme_op_keylist_end( d->ctx );
    return keyListResult();
  }

  KeyListResult Context::keyListResult() const {
    return KeyListResult( d->ctx, Error(d->lasterr) );
  }

  Key Context::key( const char * fingerprint, GpgME::Error & e , bool secret /*, bool forceUpdate*/ ) {
    d->lastop = Private::KeyList;
    gpgme_key_t key;
    e = Error( d->lasterr = gpgme_get_key( d->ctx, fingerprint, &key, int( secret )/*, int( forceUpdate )*/ ) );
    return Key( key, false );
  }

  KeyGenerationResult Context::generateKey( const char * parameters, Data & pubKey ) {
    d->lastop = Private::KeyGen;
    Data::Private * const dp = pubKey.impl();
    d->lasterr = gpgme_op_genkey( d->ctx, parameters, dp ? dp->data : 0, 0 );
    return KeyGenerationResult( d->ctx, Error(d->lasterr) );
  }

  Error Context::startKeyGeneration( const char * parameters, Data & pubKey ) {
    d->lastop = Private::KeyGen;
    Data::Private * const dp = pubKey.impl();
    return Error( d->lasterr = gpgme_op_genkey_start( d->ctx, parameters, dp ? dp->data : 0, 0 ) );
  }

  KeyGenerationResult Context::keyGenerationResult() const {
    if ( d->lastop & Private::KeyGen )
      return KeyGenerationResult( d->ctx, Error(d->lasterr) );
    else
      return KeyGenerationResult();
  }

  Error Context::exportPublicKeys( const char * pattern, Data & keyData ) {
    d->lastop = Private::Export;
    Data::Private * const dp = keyData.impl();
    return Error( d->lasterr = gpgme_op_export( d->ctx, pattern, 0, dp ? dp->data : 0 ) );
  }

  Error Context::exportPublicKeys( const char * patterns[], Data & keyData ) {
    d->lastop = Private::Export;
#ifndef HAVE_GPGME_EXT_KEYLIST_MODE_EXTERNAL_NONBROKEN
    if ( !patterns || !patterns[0] || !patterns[1] )
      // max. one pattern -> use the non-ext version
      return exportPublicKeys( patterns ? patterns[0] : 0, keyData );
#endif
    Data::Private * const dp = keyData.impl();
    return Error( d->lasterr = gpgme_op_export_ext( d->ctx, patterns, 0, dp ? dp->data : 0 ) );
  }

  Error Context::startPublicKeyExport( const char * pattern, Data & keyData ) {
    d->lastop = Private::Export;
    Data::Private * const dp = keyData.impl();
    return Error( d->lasterr = gpgme_op_export_start( d->ctx, pattern, 0, dp ? dp->data : 0 ) );
  }

  Error Context::startPublicKeyExport( const char * patterns[], Data & keyData ) {
    d->lastop = Private::Export;
#ifndef HAVE_GPGME_EXT_KEYLIST_MODE_EXTERNAL_NONBROKEN
    if ( !patterns || !patterns[0] || !patterns[1] )
      // max. one pattern -> use the non-ext version
      return startPublicKeyExport( patterns ? patterns[0] : 0, keyData );
#endif
    Data::Private * const dp = keyData.impl();
    return Error( d->lasterr = gpgme_op_export_ext_start( d->ctx, patterns, 0, dp ? dp->data : 0 ) );
  }


  ImportResult Context::importKeys( const Data & data ) {
    d->lastop = Private::Import;
    const Data::Private * const dp = data.impl();
    d->lasterr = gpgme_op_import( d->ctx, dp ? dp->data : 0 );
    return ImportResult( d->ctx, Error(d->lasterr) );
  }

  ImportResult Context::importKeys( const std::vector<Key> & kk ) {
    d->lastop = Private::Import;
    d->lasterr = gpg_error( GPG_ERR_NOT_IMPLEMENTED );
    bool shouldHaveResult = false;
#ifdef HAVE_GPGME_OP_IMPORT_KEYS
    const boost::scoped_array<gpgme_key_t> keys( new gpgme_key_t[ kk.size() + 1 ] );
    gpgme_key_t * keys_it = &keys[0];
    for ( std::vector<Key>::const_iterator it = kk.begin(), end = kk.end() ; it != end ; ++it )
      if ( it->impl() )
        *keys_it++ = it->impl();
    *keys_it++ = 0;
    d->lasterr = gpgme_op_import_keys( d->ctx, keys.get() );
    shouldHaveResult = true;
#endif
    if ( ( gpgme_err_code( d->lasterr ) == GPG_ERR_NOT_IMPLEMENTED ||
           gpgme_err_code( d->lasterr ) == GPG_ERR_NOT_SUPPORTED ) &&
         protocol() == CMS )
    {
        // ok, try the workaround (export+import):
        std::vector<const char*> fprs;
        for ( std::vector<Key>::const_iterator it = kk.begin(), end = kk.end() ; it != end ; ++it ) {
            if ( const char * fpr = it->primaryFingerprint() ) {
                if ( *fpr )
                    fprs.push_back( fpr );
            } else if ( const char * keyid = it->keyID() ) {
                if ( *keyid )
                    fprs.push_back( keyid );
            }
        }
        fprs.push_back( 0 );
        Data data;
        Data::Private * const dp = data.impl();
        const gpgme_keylist_mode_t oldMode = gpgme_get_keylist_mode( d->ctx );
        gpgme_set_keylist_mode( d->ctx, GPGME_KEYLIST_MODE_EXTERN );
        d->lasterr = gpgme_op_export_ext( d->ctx, &fprs[0], 0, dp ? dp->data : 0 );
        gpgme_set_keylist_mode( d->ctx, oldMode );
        if ( !d->lasterr ) {
            data.seek( 0, SEEK_SET );
            d->lasterr = gpgme_op_import( d->ctx, dp ? dp->data : 0 );
            shouldHaveResult = true;
        }
    }
    if ( shouldHaveResult )
        return ImportResult( d->ctx, Error(d->lasterr) );
    else
        return ImportResult( Error( d->lasterr ) );
  }

  Error Context::startKeyImport( const Data & data ) {
    d->lastop = Private::Import;
    const Data::Private * const dp = data.impl();
    return Error( d->lasterr = gpgme_op_import_start( d->ctx, dp ? dp->data : 0 ) );
  }

  Error Context::startKeyImport( const std::vector<Key> & kk ) {
    d->lastop = Private::Import;
#ifdef HAVE_GPGME_OP_IMPORT_KEYS
    const boost::scoped_array<gpgme_key_t> keys( new gpgme_key_t[ kk.size() + 1 ] );
    gpgme_key_t * keys_it = &keys[0];
    for ( std::vector<Key>::const_iterator it = kk.begin(), end = kk.end() ; it != end ; ++it )
      if ( it->impl() )
        *keys_it++ = it->impl();
    *keys_it++ = 0;
    return Error( d->lasterr = gpgme_op_import_keys_start( d->ctx, keys.get() ) );
#else
    (void)kk;
    return Error( d->lasterr = gpg_error( GPG_ERR_NOT_IMPLEMENTED ) );
#endif
  }

  ImportResult Context::importResult() const {
    if ( d->lastop & Private::Import )
      return ImportResult( d->ctx, Error(d->lasterr) );
    else
      return ImportResult();
  }

  Error Context::deleteKey( const Key & key, bool allowSecretKeyDeletion ) {
    d->lastop = Private::Delete;
    return Error( d->lasterr = gpgme_op_delete( d->ctx, key.impl(), int( allowSecretKeyDeletion ) ) );
  }

  Error Context::startKeyDeletion( const Key & key, bool allowSecretKeyDeletion ) {
    d->lastop = Private::Delete;
    return Error( d->lasterr = gpgme_op_delete_start( d->ctx, key.impl(), int( allowSecretKeyDeletion ) ) );
  }

  Error Context::edit( const Key & key, std::auto_ptr<EditInteractor> func, Data & data ) {
      d->lastop = Private::Edit;
      d->lastEditInteractor = func;
      Data::Private * const dp = data.impl();
      return Error( d->lasterr = gpgme_op_edit( d->ctx, key.impl(),
                                                d->lastEditInteractor.get() ? edit_interactor_callback : 0,
                                                d->lastEditInteractor.get() ? d->lastEditInteractor->d : 0,
                                                dp ? dp->data : 0 ) );
  }

  Error Context::startEditing( const Key & key, std::auto_ptr<EditInteractor> func, Data & data ) {
      d->lastop = Private::Edit;
      d->lastEditInteractor = func;
      Data::Private * const dp = data.impl();
      return Error( d->lasterr = gpgme_op_edit_start( d->ctx, key.impl(),
                                                      d->lastEditInteractor.get() ? edit_interactor_callback : 0,
                                                      d->lastEditInteractor.get() ? d->lastEditInteractor->d : 0,
                                                      dp ? dp->data : 0 ) );
  }

  EditInteractor * Context::lastEditInteractor() const {
      return d->lastEditInteractor.get();
  }

  std::auto_ptr<EditInteractor> Context::takeLastEditInteractor() {
      return d->lastEditInteractor;
  }

  Error Context::cardEdit( const Key & key, std::auto_ptr<EditInteractor> func, Data & data ) {
      d->lastop = Private::CardEdit;
      d->lastCardEditInteractor = func;
      Data::Private * const dp = data.impl();
      return Error( d->lasterr = gpgme_op_card_edit( d->ctx, key.impl(),
                                                     d->lastCardEditInteractor.get() ? edit_interactor_callback : 0,
                                                     d->lastCardEditInteractor.get() ? d->lastCardEditInteractor->d : 0,
                                                     dp ? dp->data : 0 ) );
  }

  Error Context::startCardEditing( const Key & key, std::auto_ptr<EditInteractor> func, Data & data ) {
      d->lastop = Private::CardEdit;
      d->lastCardEditInteractor = func;
      Data::Private * const dp = data.impl();
      return Error( d->lasterr = gpgme_op_card_edit_start( d->ctx, key.impl(),
                                                           d->lastCardEditInteractor.get() ? edit_interactor_callback : 0,
                                                           d->lastCardEditInteractor.get() ? d->lastCardEditInteractor->d : 0,
                                                           dp ? dp->data : 0 ) );
  }

  EditInteractor * Context::lastCardEditInteractor() const {
      return d->lastCardEditInteractor.get();
  }

  std::auto_ptr<EditInteractor> Context::takeLastCardEditInteractor() {
      return d->lastCardEditInteractor;
  }

  Error Context::startTrustItemListing( const char * pattern, int maxLevel ) {
    d->lastop = Private::TrustList;
    return Error( d->lasterr = gpgme_op_trustlist_start( d->ctx, pattern, maxLevel ) );
  }

  TrustItem Context::nextTrustItem( Error & e ) {
    gpgme_trust_item_t ti = 0;
    e = Error( d->lasterr = gpgme_op_trustlist_next( d->ctx, &ti ) );
    return TrustItem( ti );
  }

  Error Context::endTrustItemListing() {
    return Error( d->lasterr = gpgme_op_trustlist_end( d->ctx ) );
  }

#ifdef HAVE_GPGME_ASSUAN_ENGINE
  static gpgme_error_t assuan_transaction_data_callback( void * opaque, const void * data, size_t datalen ) {
    assert( opaque );
    AssuanTransaction * t = static_cast<AssuanTransaction*>( opaque );
    return t->data( static_cast<const char*>( data ), datalen ).encodedError();
  }

  static gpgme_error_t assuan_transaction_inquire_callback( void * opaque, const char * name, const char * args, gpgme_data_t * r_data ) {
    assert( opaque );
    Context::Private * p = static_cast<Context::Private*>( opaque );
    AssuanTransaction * t = p->lastAssuanTransaction.get();
    assert( t );
    Error err;
    if ( name )
      p->lastAssuanInquireData = t->inquire( name, args, err );
    else
      p->lastAssuanInquireData = Data::null;
    if ( !p->lastAssuanInquireData.isNull() )
      *r_data = p->lastAssuanInquireData.impl()->data;
    return err.encodedError();
  }

  static gpgme_error_t assuan_transaction_status_callback( void * opaque, const char * status, const char * args ) {
    assert( opaque );
    AssuanTransaction * t = static_cast<AssuanTransaction*>( opaque );
    std::string a = args;
    percent_unescape( a, true ); // ### why doesn't gpgme do this??
    return t->status( status, a.c_str() ).encodedError();
  }
#endif

  AssuanResult Context::assuanTransact( const char * command ) {
    return assuanTransact( command, std::auto_ptr<AssuanTransaction>( new DefaultAssuanTransaction ) );
  }

  AssuanResult Context::assuanTransact( const char * command, std::auto_ptr<AssuanTransaction> transaction ) {
    d->lastop = Private::AssuanTransact;
    d->lastAssuanTransaction = transaction;
    if ( !d->lastAssuanTransaction.get() )
        return AssuanResult( Error( d->lasterr = gpg_error( GPG_ERR_INV_ARG ) ) );
#ifdef HAVE_GPGME_ASSUAN_ENGINE
    d->lasterr = gpgme_op_assuan_transact( d->ctx, command,
                                           assuan_transaction_data_callback,
                                           d->lastAssuanTransaction.get(),
                                           assuan_transaction_inquire_callback,
                                           d, // sic!
                                           assuan_transaction_status_callback,
                                           d->lastAssuanTransaction.get() );
#else
    (void)command;
    d->lasterr = gpg_error( GPG_ERR_NOT_SUPPORTED );
#endif
    return AssuanResult( d->ctx, d->lasterr );
  }

  Error Context::startAssuanTransaction( const char * command ) {
    return startAssuanTransaction( command, std::auto_ptr<AssuanTransaction>( new DefaultAssuanTransaction ) );
  }

  Error Context::startAssuanTransaction( const char * command, std::auto_ptr<AssuanTransaction> transaction ) {
    d->lastop = Private::AssuanTransact;
    d->lastAssuanTransaction = transaction;
    if ( !d->lastAssuanTransaction.get() )
        return Error( d->lasterr = gpg_error( GPG_ERR_INV_ARG ) );
#ifdef HAVE_GPGME_ASSUAN_ENGINE
    return Error( d->lasterr = gpgme_op_assuan_transact_start( d->ctx, command,
                                                               assuan_transaction_data_callback,
                                                               d->lastAssuanTransaction.get(),
                                                               assuan_transaction_inquire_callback,
                                                               d, // sic!
                                                               assuan_transaction_status_callback,
                                                               d->lastAssuanTransaction.get() ) );
#else
    (void)command;
    return Error( d->lasterr = gpg_error( GPG_ERR_NOT_SUPPORTED ) );
#endif
  }

  AssuanResult Context::assuanResult() const {
    if ( d->lastop & Private::AssuanTransact )
      return AssuanResult( d->ctx, d->lasterr );
    else
      return AssuanResult();
  }

  AssuanTransaction * Context::lastAssuanTransaction() const {
    return d->lastAssuanTransaction.get();
  }

  std::auto_ptr<AssuanTransaction> Context::takeLastAssuanTransaction() {
    return d->lastAssuanTransaction;
  }

  DecryptionResult Context::decrypt( const Data & cipherText, Data & plainText ) {
    d->lastop = Private::Decrypt;
    const Data::Private * const cdp = cipherText.impl();
    Data::Private * const pdp = plainText.impl();
    d->lasterr = gpgme_op_decrypt( d->ctx, cdp ? cdp->data : 0, pdp ? pdp->data : 0 );
    return DecryptionResult( d->ctx, Error(d->lasterr) );
  }

  Error Context::startDecryption( const Data & cipherText, Data & plainText ) {
    d->lastop = Private::Decrypt;
    const Data::Private * const cdp = cipherText.impl();
    Data::Private * const pdp = plainText.impl();
    return Error( d->lasterr = gpgme_op_decrypt_start( d->ctx, cdp ? cdp->data : 0, pdp ? pdp->data : 0 ) );
  }

  DecryptionResult Context::decryptionResult() const {
    if ( d->lastop & Private::Decrypt )
      return DecryptionResult( d->ctx, Error(d->lasterr) );
    else
      return DecryptionResult();
  }



  VerificationResult Context::verifyDetachedSignature( const Data & signature, const Data & signedText ) {
    d->lastop = Private::Verify;
    const Data::Private * const sdp = signature.impl();
    const Data::Private * const tdp = signedText.impl();
    d->lasterr = gpgme_op_verify( d->ctx, sdp ? sdp->data : 0, tdp ? tdp->data : 0, 0 );
    return VerificationResult( d->ctx, Error(d->lasterr) );
  }

  VerificationResult Context::verifyOpaqueSignature( const Data & signedData, Data & plainText ) {
    d->lastop = Private::Verify;
    const Data::Private * const sdp = signedData.impl();
    Data::Private * const pdp = plainText.impl();
    d->lasterr = gpgme_op_verify( d->ctx, sdp ? sdp->data : 0, 0, pdp ? pdp->data : 0 );
    return VerificationResult( d->ctx, Error(d->lasterr) );
  }

  Error Context::startDetachedSignatureVerification( const Data & signature, const Data & signedText ) {
    d->lastop = Private::Verify;
    const Data::Private * const sdp = signature.impl();
    const Data::Private * const tdp = signedText.impl();
    return Error( d->lasterr = gpgme_op_verify_start( d->ctx, sdp ? sdp->data : 0, tdp ? tdp->data : 0, 0 ) );
  }

  Error Context::startOpaqueSignatureVerification( const Data & signedData, Data & plainText ) {
    d->lastop = Private::Verify;
    const Data::Private * const sdp = signedData.impl();
    Data::Private * const pdp = plainText.impl();
    return Error( d->lasterr = gpgme_op_verify_start( d->ctx, sdp ? sdp->data : 0, 0, pdp ? pdp->data : 0 ) );
  }

  VerificationResult Context::verificationResult() const {
    if ( d->lastop & Private::Verify )
      return VerificationResult( d->ctx, Error(d->lasterr) );
    else
      return VerificationResult();
  }


  std::pair<DecryptionResult,VerificationResult> Context::decryptAndVerify( const Data & cipherText, Data & plainText ) {
    d->lastop = Private::DecryptAndVerify;
    const Data::Private * const cdp = cipherText.impl();
    Data::Private * const pdp = plainText.impl();
    d->lasterr = gpgme_op_decrypt_verify( d->ctx, cdp ? cdp->data : 0, pdp ? pdp->data : 0 );
    return std::make_pair( DecryptionResult( d->ctx, Error(d->lasterr) ),
                           VerificationResult( d->ctx, Error(d->lasterr) ) );
  }

  Error Context::startCombinedDecryptionAndVerification( const Data & cipherText, Data & plainText ) {
    d->lastop = Private::DecryptAndVerify;
    const Data::Private * const cdp = cipherText.impl();
    Data::Private * const pdp = plainText.impl();
    return Error( d->lasterr = gpgme_op_decrypt_verify_start( d->ctx, cdp ? cdp->data : 0, pdp ? pdp->data : 0 ) );
  }

#ifdef HAVE_GPGME_OP_GETAUDITLOG
  unsigned int to_auditlog_flags( unsigned int flags ) {
      unsigned int result = 0;
      if ( flags & Context::HtmlAuditLog )
          result |= GPGME_AUDITLOG_HTML;
      if ( flags & Context::AuditLogWithHelp )
          result |= GPGME_AUDITLOG_WITH_HELP;
      return result;
  }
#endif // HAVE_GPGME_OP_GETAUDITLOG


  Error Context::startGetAuditLog( Data & output, unsigned int flags ) {
    d->lastop = Private::GetAuditLog;
#ifdef HAVE_GPGME_OP_GETAUDITLOG
    Data::Private * const odp = output.impl();
    return Error( d->lasterr = gpgme_op_getauditlog_start( d->ctx, odp ? odp->data : 0, to_auditlog_flags( flags ) ) );
#else
    (void)output; (void)flags;
    return Error( d->lasterr = makeError( GPG_ERR_NOT_IMPLEMENTED ) );
#endif
  }

  Error Context::getAuditLog( Data & output, unsigned int flags ) {
    d->lastop = Private::GetAuditLog;
#ifdef HAVE_GPGME_OP_GETAUDITLOG
    Data::Private * const odp = output.impl();
    return Error( d->lasterr = gpgme_op_getauditlog( d->ctx, odp ? odp->data : 0, to_auditlog_flags( flags ) ) );
#else
    (void)output; (void)flags;
    return Error( d->lasterr = makeError( GPG_ERR_NOT_IMPLEMENTED ) );
#endif
  }

  void Context::clearSigningKeys() {
    gpgme_signers_clear( d->ctx );
  }

  Error Context::addSigningKey( const Key & key ) {
    return Error( d->lasterr = gpgme_signers_add( d->ctx, key.impl() ) );
  }

  Key Context::signingKey( unsigned int idx ) const {
    gpgme_key_t key = gpgme_signers_enum( d->ctx, idx );
    return Key( key, false );
  }

  std::vector<Key> Context::signingKeys() const {
    std::vector<Key> result;
    gpgme_key_t key;
    for ( unsigned int i = 0 ; ( key = gpgme_signers_enum( d->ctx, i ) ) ; ++i )
      result.push_back( Key( key, false ) );
    return result;
  }

  void Context::clearSignatureNotations() {
#ifdef HAVE_GPGME_SIG_NOTATION_CLEARADDGET
    gpgme_sig_notation_clear( d->ctx );
#endif
  }

  GpgME::Error Context::addSignatureNotation( const char * name, const char * value, unsigned int flags ) {
#ifdef HAVE_GPGME_SIG_NOTATION_CLEARADDGET
    return Error( gpgme_sig_notation_add( d->ctx, name, value, add_to_gpgme_sig_notation_flags_t( 0, flags ) ) );
#else
    (void)name; (void)value; (void)flags;
    return Error( makeError( GPG_ERR_NOT_IMPLEMENTED ) );
#endif
  }

  GpgME::Error Context::addSignaturePolicyURL( const char * url, bool critical ) {
#ifdef HAVE_GPGME_SIG_NOTATION_CLEARADDGET
    return Error( gpgme_sig_notation_add( d->ctx, 0, url, critical ? GPGME_SIG_NOTATION_CRITICAL : 0 ) );
#else
    (void)url; (void)critical;
    return Error( makeError( GPG_ERR_NOT_IMPLEMENTED ) );
#endif
  }

  const char * Context::signaturePolicyURL() const {
#ifdef HAVE_GPGME_SIG_NOTATION_CLEARADDGET
    for ( gpgme_sig_notation_t n = gpgme_sig_notation_get( d->ctx ) ; n ; n = n->next )
      if ( !n->name )
        return n->value;
#endif
    return 0;
  }

  Notation Context::signatureNotation( unsigned int idx ) const {
#ifdef HAVE_GPGME_SIG_NOTATION_CLEARADDGET
    for ( gpgme_sig_notation_t n = gpgme_sig_notation_get( d->ctx ) ; n ; n = n->next )
      if ( n->name )
        if ( idx-- == 0 )
          return Notation( n );
#endif
    return Notation();
  }

  std::vector<Notation> Context::signatureNotations() const {
    std::vector<Notation> result;
#ifdef HAVE_GPGME_SIG_NOTATION_CLEARADDGET
    for ( gpgme_sig_notation_t n = gpgme_sig_notation_get( d->ctx ) ; n ; n = n->next )
      if ( n->name )
        result.push_back( Notation( n ) );
#endif
    return result;
  }

  static gpgme_sig_mode_t sigmode2sigmode( SignatureMode mode ) {
    switch ( mode ) {
    default:
    case NormalSignatureMode: return GPGME_SIG_MODE_NORMAL;
    case Detached:            return GPGME_SIG_MODE_DETACH;
    case Clearsigned:         return GPGME_SIG_MODE_CLEAR;
    }
  }

  SigningResult Context::sign( const Data & plainText, Data & signature, SignatureMode mode ) {
    d->lastop = Private::Sign;
    const Data::Private * const pdp = plainText.impl();
    Data::Private * const sdp = signature.impl();
    d->lasterr = gpgme_op_sign( d->ctx, pdp ? pdp->data : 0, sdp ? sdp->data : 0, sigmode2sigmode( mode ) );
    return SigningResult( d->ctx, Error(d->lasterr) );
  }


  Error Context::startSigning( const Data & plainText, Data & signature, SignatureMode mode ) {
    d->lastop = Private::Sign;
    const Data::Private * const pdp = plainText.impl();
    Data::Private * const sdp = signature.impl();
    return Error( d->lasterr = gpgme_op_sign_start( d->ctx, pdp ? pdp->data : 0, sdp ? sdp->data : 0, sigmode2sigmode( mode ) ) );
  }

  SigningResult Context::signingResult() const {
    if ( d->lastop & Private::Sign )
      return SigningResult( d->ctx, Error(d->lasterr) );
    else
      return SigningResult();
  }

  static gpgme_encrypt_flags_t encryptflags2encryptflags( Context::EncryptionFlags flags ) {
    unsigned int result = 0;
    if ( flags & Context::AlwaysTrust )
      result |= GPGME_ENCRYPT_ALWAYS_TRUST;
#ifdef HAVE_GPGME_ENCRYPT_NO_ENCRYPT_TO
    if ( flags & Context::NoEncryptTo )
      result |= GPGME_ENCRYPT_NO_ENCRYPT_TO;
#endif
    return static_cast<gpgme_encrypt_flags_t>( result );
  }

  EncryptionResult Context::encrypt( const std::vector<Key> & recipients, const Data & plainText, Data & cipherText, EncryptionFlags flags ) {
    d->lastop = Private::Encrypt;
#ifndef HAVE_GPGME_ENCRYPT_NO_ENCRYPT_TO
    if ( flags & NoEncryptTo )
        return EncryptionResult( Error( d->lasterr = gpg_error( GPG_ERR_NOT_IMPLEMENTED ) ) );
#endif
    const Data::Private * const pdp = plainText.impl();
    Data::Private * const cdp = cipherText.impl();
    gpgme_key_t * const keys = new gpgme_key_t[ recipients.size() + 1 ];
    gpgme_key_t * keys_it = keys;
    for ( std::vector<Key>::const_iterator it = recipients.begin() ; it != recipients.end() ; ++it )
      if ( it->impl() )
	*keys_it++ = it->impl();
    *keys_it++ = 0;
    d->lasterr = gpgme_op_encrypt( d->ctx, keys, encryptflags2encryptflags( flags ),
				   pdp ? pdp->data : 0, cdp ? cdp->data : 0 );
    delete[] keys;
    return EncryptionResult( d->ctx, Error(d->lasterr) );
  }

  Error Context::encryptSymmetrically( const Data & plainText, Data & cipherText ) {
    d->lastop = Private::Encrypt;
    const Data::Private * const pdp = plainText.impl();
    Data::Private * const cdp = cipherText.impl();
    return Error( d->lasterr = gpgme_op_encrypt( d->ctx, 0, (gpgme_encrypt_flags_t)0,
                                                 pdp ? pdp->data : 0, cdp ? cdp->data : 0 ) );
  }

  Error Context::startEncryption( const std::vector<Key> & recipients, const Data & plainText, Data & cipherText, EncryptionFlags flags ) {
    d->lastop = Private::Encrypt;
#ifndef HAVE_GPGME_ENCRYPT_NO_ENCRYPT_TO
    if ( flags & NoEncryptTo )
        return Error( d->lasterr = gpg_error( GPG_ERR_NOT_IMPLEMENTED ) );
#endif
    const Data::Private * const pdp = plainText.impl();
    Data::Private * const cdp = cipherText.impl();
    gpgme_key_t * const keys = new gpgme_key_t[ recipients.size() + 1 ];
    gpgme_key_t * keys_it = keys;
    for ( std::vector<Key>::const_iterator it = recipients.begin() ; it != recipients.end() ; ++it )
      if ( it->impl() )
	*keys_it++ = it->impl();
    *keys_it++ = 0;
    d->lasterr = gpgme_op_encrypt_start( d->ctx, keys, encryptflags2encryptflags( flags ),
					 pdp ? pdp->data : 0, cdp ? cdp->data : 0 );
    delete[] keys;
    return Error( d->lasterr );
  }

  EncryptionResult Context::encryptionResult() const {
    if ( d->lastop & Private::Encrypt )
      return EncryptionResult( d->ctx, Error(d->lasterr) );
    else
      return EncryptionResult();
  }

  std::pair<SigningResult,EncryptionResult> Context::signAndEncrypt( const std::vector<Key> & recipients, const Data & plainText, Data & cipherText, EncryptionFlags flags ) {
    d->lastop = Private::SignAndEncrypt;
    const Data::Private * const pdp = plainText.impl();
    Data::Private * const cdp = cipherText.impl();
    gpgme_key_t * const keys = new gpgme_key_t[ recipients.size() + 1 ];
    gpgme_key_t * keys_it = keys;
    for ( std::vector<Key>::const_iterator it = recipients.begin() ; it != recipients.end() ; ++it )
      if ( it->impl() )
	*keys_it++ = it->impl();
    *keys_it++ = 0;
    d->lasterr = gpgme_op_encrypt_sign( d->ctx, keys, encryptflags2encryptflags( flags ),
					pdp ? pdp->data : 0, cdp ? cdp->data : 0 );
    delete[] keys;
    return std::make_pair( SigningResult( d->ctx, Error(d->lasterr) ),
                           EncryptionResult( d->ctx, Error(d->lasterr) ) );
  }

  Error Context::startCombinedSigningAndEncryption( const std::vector<Key> & recipients, const Data & plainText, Data & cipherText, EncryptionFlags flags ) {
    d->lastop = Private::SignAndEncrypt;
    const Data::Private * const pdp = plainText.impl();
    Data::Private * const cdp = cipherText.impl();
    gpgme_key_t * const keys = new gpgme_key_t[ recipients.size() + 1 ];
    gpgme_key_t * keys_it = keys;
    for ( std::vector<Key>::const_iterator it = recipients.begin() ; it != recipients.end() ; ++it )
      if ( it->impl() )
	*keys_it++ = it->impl();
    *keys_it++ = 0;
    d->lasterr = gpgme_op_encrypt_sign_start( d->ctx, keys, encryptflags2encryptflags( flags ),
					      pdp ? pdp->data : 0, cdp ? cdp->data : 0 );
    delete[] keys;
    return Error( d->lasterr );
  }

  Error Context::createVFS(const char* containerFile, const std::vector< Key >& recipients) {
    d->lastop = Private::CreateVFS;
#ifdef HAVE_GPGME_G13_VFS
    gpgme_key_t * const keys = new gpgme_key_t[ recipients.size() + 1 ];
    gpgme_key_t * keys_it = keys;
    for ( std::vector<Key>::const_iterator it = recipients.begin() ; it != recipients.end() ; ++it )
      if ( it->impl() )
        *keys_it++ = it->impl();
    *keys_it++ = 0;

    gpgme_error_t op_err;
    d->lasterr = gpgme_op_vfs_create( d->ctx, keys, containerFile, 0, &op_err );
    delete[] keys;
    Error error( d->lasterr );
    if ( error )
      return error;
    return Error( d->lasterr = op_err );
#else
    return Error( d->lasterr = gpg_error( GPG_ERR_NOT_SUPPORTED ) );
#endif
  }

  VfsMountResult Context::mountVFS(const char* containerFile, const char* mountDir) {
    d->lastop = Private::MountVFS;
#ifdef HAVE_GPGME_G13_VFS
    gpgme_error_t op_err;
    d->lasterr = gpgme_op_vfs_mount( d->ctx, containerFile, mountDir, 0, &op_err );
    return VfsMountResult( d->ctx, Error( d->lasterr ), Error( op_err ) );
#else
    return VfsMountResult( d->ctx, Error( d->lasterr = gpg_error( GPG_ERR_NOT_SUPPORTED ) ), Error() );
#endif
  }

  Error Context::cancelPendingOperation() {
#ifdef HAVE_GPGME_CANCEL_ASYNC
    return Error( gpgme_cancel_async( d->ctx ) );
#else
    return Error( gpgme_cancel( d->ctx ) );
#endif
  }

  bool Context::poll() {
    gpgme_error_t e = GPG_ERR_NO_ERROR;
    const bool finished = gpgme_wait( d->ctx, &e, 0 );
    if ( finished )
      d->lasterr = e;
    return finished;
  }

  Error Context::wait() {
    gpgme_error_t e = GPG_ERR_NO_ERROR;
    gpgme_wait( d->ctx, &e, 1 );
    return Error( d->lasterr = e );
  }

  Error Context::lastError() const {
    return Error( d->lasterr );
  }

  std::ostream & operator<<( std::ostream & os, Protocol proto ) {
      os << "GpgME::Protocol(";
      switch ( proto ) {
      case OpenPGP:
          os << "OpenPGP";
          break;
      case CMS:
          os << "CMS";
          break;
      default:
      case UnknownProtocol:
          os << "UnknownProtocol";
          break;
      }
      return os << ')';
  }

  std::ostream & operator<<( std::ostream & os, Engine eng ) {
      os << "GpgME::Engine(";
      switch ( eng ) {
      case GpgEngine:
          os << "GpgEngine";
          break;
      case GpgSMEngine:
          os << "GpgSMEngine";
          break;
      case GpgConfEngine:
          os << "GpgConfEngine";
          break;
      case AssuanEngine:
          os << "AssuanEngine";
      default:
      case UnknownEngine:
          os << "UnknownEngine";
          break;
      }
      return os << ')';
  }

  std::ostream & operator<<( std::ostream & os, Context::CertificateInclusion incl ) {
      os << "GpgME::Context::CertificateInclusion(" << static_cast<int>( incl );
      switch ( incl ) {
      case Context::DefaultCertificates:
          os << "(DefaultCertificates)";
          break;
      case Context::AllCertificatesExceptRoot:
          os << "(AllCertificatesExceptRoot)";
          break;
      case Context::AllCertificates:
          os << "(AllCertificates)";
          break;
      case Context::NoCertificates:
          os << "(NoCertificates)";
          break;
      case Context::OnlySenderCertificate:
          os << "(OnlySenderCertificate)";
          break;
      }
      return os << ')';
  }

  std::ostream & operator<<( std::ostream & os, KeyListMode mode ) {
      os << "GpgME::KeyListMode(";
#define CHECK( x ) if ( !(mode & (x)) ) {} else do { os << #x " "; } while (0)
      CHECK( Local );
      CHECK( Extern );
      CHECK( Signatures );
      CHECK( Validate );
      CHECK( Ephemeral );
#undef CHECK
      return os << ')';
  }

  std::ostream & operator<<( std::ostream & os, SignatureMode mode ) {
      os << "GpgME::SignatureMode(";
      switch ( mode ) {
#define CHECK( x ) case x: os << #x; break
          CHECK( NormalSignatureMode );
          CHECK( Detached );
          CHECK( Clearsigned );
#undef CHECK
      default:
          os << "???" "(" << static_cast<int>( mode ) << ')';
          break;
      }
      return os << ')';
  }

  std::ostream & operator<<( std::ostream & os, Context::EncryptionFlags flags ) {
      os << "GpgME::Context::EncryptionFlags(";
#define CHECK( x ) if ( !(flags & (Context::x)) ) {} else do { os << #x " "; } while (0)
     CHECK( AlwaysTrust );
#undef CHECK
      return os << ')';
  }

  std::ostream & operator<<( std::ostream & os, Context::AuditLogFlags flags ) {
      os << "GpgME::Context::AuditLogFlags(";
#define CHECK( x ) if ( !(flags & (Context::x)) ) {} else do { os << #x " "; } while (0)
      CHECK( HtmlAuditLog );
      CHECK( AuditLogWithHelp );
#undef CHECK
      return os << ')';
  }



} // namespace GpgME

GpgME::Error GpgME::setDefaultLocale( int cat, const char * val ) {
  return Error( gpgme_set_locale( 0, cat, val ) );
}

GpgME::EngineInfo GpgME::engineInfo( GpgME::Protocol proto ) {
  gpgme_engine_info_t ei = 0;
  if ( gpgme_get_engine_info( &ei ) )
    return EngineInfo();

  const gpgme_protocol_t p = proto == CMS ? GPGME_PROTOCOL_CMS : GPGME_PROTOCOL_OpenPGP ;

  for ( gpgme_engine_info_t i = ei ; i ; i = i->next )
    if ( i->protocol == p )
      return EngineInfo( i );

  return EngineInfo();
}

GpgME::Error GpgME::checkEngine( GpgME::Protocol proto ) {
  const gpgme_protocol_t p = proto == CMS ? GPGME_PROTOCOL_CMS : GPGME_PROTOCOL_OpenPGP ;

  return Error( gpgme_engine_check_version( p ) );
}

static gpgme_protocol_t UNKNOWN_PROTOCOL = static_cast<gpgme_protocol_t>( 255 );

static gpgme_protocol_t engine2protocol( const GpgME::Engine engine ) {
    switch ( engine ) {
    case GpgME::GpgEngine:   return GPGME_PROTOCOL_OpenPGP;
    case GpgME::GpgSMEngine: return GPGME_PROTOCOL_CMS;
    case GpgME::GpgConfEngine:
#ifdef HAVE_GPGME_PROTOCOL_GPGCONF
        return GPGME_PROTOCOL_GPGCONF;
#else
        break;
#endif
    case GpgME::AssuanEngine:
#ifdef HAVE_GPGME_ASSUAN_ENGINE
        return GPGME_PROTOCOL_ASSUAN;
#else
        break;
#endif
    case GpgME::UnknownEngine:
        ;
    }
    return UNKNOWN_PROTOCOL;
}

GpgME::EngineInfo GpgME::engineInfo( GpgME::Engine engine ) {
  gpgme_engine_info_t ei = 0;
  if ( gpgme_get_engine_info( &ei ) )
    return EngineInfo();

  const gpgme_protocol_t p = engine2protocol( engine );

  for ( gpgme_engine_info_t i = ei ; i ; i = i->next )
    if ( i->protocol == p )
      return EngineInfo( i );

  return EngineInfo();
}

GpgME::Error GpgME::checkEngine( GpgME::Engine engine ) {
  const gpgme_protocol_t p = engine2protocol( engine );

  return Error( gpgme_engine_check_version( p ) );
}

static const unsigned long supported_features = 0
    | GpgME::ValidatingKeylistModeFeature
    | GpgME::CancelOperationFeature
    | GpgME::WrongKeyUsageFeature
#ifdef HAVE_GPGME_INCLUDE_CERTS_DEFAULT
    | GpgME::DefaultCertificateInclusionFeature
#endif
#ifdef HAVE_GPGME_CTX_GETSET_ENGINE_INFO
    | GpgME::GetSetEngineInfoFeature
#endif
#ifdef HAVE_GPGME_SIG_NOTATION_CLEARADDGET
    | GpgME::ClearAddGetSignatureNotationsFeature
#endif
#ifdef HAVE_GPGME_DATA_SET_FILE_NAME
    | GpgME::SetDataFileNameFeeature
#endif
#ifdef HAVE_GPGME_KEYLIST_MODE_SIG_NOTATIONS
    | GpgME::SignatureNotationsKeylistModeFeature
#endif
#ifdef HAVE_GPGME_KEY_SIG_NOTATIONS
    | GpgME::KeySignatureNotationsFeature
#endif
#ifdef HAVE_GPGME_KEY_T_IS_QUALIFIED
    | GpgME::KeyIsQualifiedFeature
#endif
#ifdef HAVE_GPGME_SIG_NOTATION_CRITICAL
    | GpgME::SignatureNotationsCriticalFlagFeature
#endif
#ifdef HAVE_GPGME_SIG_NOTATION_FLAGS_T
    | GpgME::SignatureNotationsFlagsFeature
#endif
#ifdef HAVE_GPGME_SIG_NOTATION_HUMAN_READABLE
    | GpgME::SignatureNotationsHumanReadableFlagFeature
#endif
#ifdef HAVE_GPGME_SUBKEY_T_IS_QUALIFIED
    | GpgME::SubkeyIsQualifiedFeature
#endif
#ifdef HAVE_GPGME_ENGINE_INFO_T_HOME_DIR
    | GpgME::EngineInfoHomeDirFeature
#endif
#ifdef HAVE_GPGME_DECRYPT_RESULT_T_FILE_NAME
    | GpgME::DecryptionResultFileNameFeature
#endif
#ifdef HAVE_GPGME_DECRYPT_RESULT_T_RECIPIENTS
    | GpgME::DecryptionResultRecipientsFeature
#endif
#ifdef HAVE_GPGME_VERIFY_RESULT_T_FILE_NAME
    | GpgME::VerificationResultFileNameFeature
#endif
#ifdef HAVE_GPGME_SIGNATURE_T_PKA_FIELDS
    | GpgME::SignaturePkaFieldsFeature
#endif
#ifdef HAVE_GPGME_SIGNATURE_T_ALGORITHM_FIELDS
    | GpgME::SignatureAlgorithmFieldsFeature
#endif
#ifdef HAVE_GPGME_GET_FDPTR
    | GpgME::FdPointerFeature
#endif
#ifdef HAVE_GPGME_OP_GETAUDITLOG
    | GpgME::AuditLogFeature
#endif
#ifdef HAVE_GPGME_PROTOCOL_GPGCONF
    | GpgME::GpgConfEngineFeature
#endif
#ifdef HAVE_GPGME_CANCEL_ASYNC
    | GpgME::CancelOperationAsyncFeature
#endif
#ifdef HAVE_GPGME_ENCRYPT_NO_ENCRYPT_TO
    | GpgME::NoEncryptToEncryptionFlagFeature
#endif
#ifdef HAVE_GPGME_SUBKEY_T_IS_CARDKEY
    | GpgME::CardKeyFeature
#endif
#ifdef HAVE_GPGME_ASSUAN_ENGINE
    | GpgME::AssuanEngineFeature
#endif
#ifdef HAVE_GPGME_KEYLIST_MODE_EPHEMERAL
    | GpgME::EphemeralKeylistModeFeature
#endif
#ifdef HAVE_GPGME_OP_IMPORT_KEYS
    | GpgME::ImportFromKeyserverFeature
#endif
#ifdef HAVE_GPGME_G13_VFS
    | GpgME::G13VFSFeature
#endif
    ;

bool GpgME::hasFeature( unsigned long features ) {
    return features == ( features & supported_features );
}
