/* context.h - wraps a gpgme key context
   Copyright (C) 2003 Klarälvdalens Datakonsult AB

   This file is part of GPGME++.
 
   GPGME++ is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
 
   GPGME++ is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GPGME++; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307 USA.  */

// -*- c++ -*-
#ifndef __GPGMEPP_CONTEXT_H__
#define __GPGMEPP_CONTEXT_H__

#include <gpgmepp/key.h>
#include <gpgmepp/gpgmefw.h>

#include <vector>

#include <sys/types.h> // for time_t

namespace GpgME {

  class Key;
  class Data;
  class TrustItem;
  class ProgressProvider;
  class PassphraseProvider;
  class EventLoopInteractor;

  class KeyListResult;
  class KeyGenerationResult;
  class ImportResult;
  class DecryptionResult;
  class VerificationResult;
  class SigningResult;
  class EncryptionResult;

  class Error {
  public:
    Error( int e ) : mErr( e ) {}

    const char * source() const;
    const char * asString() const;

    operator int() const { return mErr; }
    operator bool() const { return mErr; }
  private:
    int mErr;
  };

  class Context {
    Context( gpgme_ctx_t );
  public:
    enum Protocol { OpenPGP, CMS, Unknown };

    //
    // Creation and destruction:
    //

    static Context * createForProtocol( Protocol proto );
    virtual ~Context();

    //
    // Context Attributes
    //

    Protocol protocol() const;

    void setArmor( bool useArmor );
    bool armor() const;

    void setTextMode( bool useTextMode );
    bool textMode() const;

    enum CertificateInclusion {
      AllCertificatesExceptRoot = -2,
      AllCertificates = -1,
      NoCertificates = 0,
      OnlySenderCertificate = 1
    };
    void setIncludeCertificates( int which );
    int includeCertificates() const;

    enum KeyListMode { Local, Extern };
    void setKeyListMode( KeyListMode keyListMode );
    KeyListMode keyListMode() const;

    void setPassphraseProvider( PassphraseProvider * provider );
    PassphraseProvider * passphraseProvider() const;

    void setProgressProvider( ProgressProvider * provider );
    ProgressProvider * progressProvider() const;

    void setManagedByEventLoopInteractor( bool managed );
    bool managedByEventLoopInteractor() const;

  private:
    friend class EventLoopInteractor;
    void installIOCallbacks( gpgme_io_cbs * iocbs );
    void uninstallIOCallbacks();

  public:
    //
    //
    // Key Management
    //
    //

    //
    // Key Listing
    //

    GpgME::Error startKeyListing( const char * pattern=0, bool secretOnly=false );
    GpgME::Error startKeyListing( const char * patterns[], bool secretOnly=false );

    Key nextKey( GpgME::Error & e );
    
    KeyListResult endKeyListing();

    Key key( const char * fingerprint, GpgME::Error & e, bool secret=false );

    //
    // Key Generation
    //

    KeyGenerationResult generateKey( const char * parameters, Data & pubKey );
    GpgME::Error startKeyGeneration( const char * parameters, Data & pubkey );
    KeyGenerationResult keyGenerationResult() const;

    //
    // Key Export
    //

    GpgME::Error exportPublicKeys( const char * pattern, Data & keyData );
    GpgME::Error exportPublicKeys( const char * pattern[], Data & keyData );
    GpgME::Error startPublicKeyExport( const char * pattern, Data & keyData );
    GpgME::Error startPublicKeyExport( const char * pattern[], Data & keyData );

    //
    // Key Import
    //

    ImportResult importKeys( const Data & data );
    GpgME::Error startKeyImport( const Data & data );
    ImportResult importResult() const;

    //
    // Key Deletion
    //

    GpgME::Error deleteKey( const Key & key, bool allowSecretKeyDeletion=false );
    GpgME::Error startKeyDeletion( const Key & key, bool allowSecretKeyDeletion=false );

    //
    // Trust Item Management
    //    

    GpgME::Error startTrustItemListing( const char * pattern, int maxLevel );
    TrustItem nextTrustItem( GpgME::Error & e );
    GpgME::Error endTrustItemListing();

    //
    //
    // Crypto Operations
    //
    //

    //
    // Decryption
    //

    DecryptionResult decrypt( const Data & cipherText, Data & plainText );
    GpgME::Error startDecryption( const Data & cipherText, Data & plainText );
    DecryptionResult decryptionResult() const;

    //
    // Signature Verification
    //

    VerificationResult verifyDetachedSignature( const Data & signature, const Data & signedText );
    VerificationResult verifyOpaqueSignature( const Data & signedData, Data & plainText );
    GpgME::Error startDetachedSignatureVerification( const Data & signature, const Data & signedText );
    GpgME::Error startOpaqueSignatureVerification( const Data & signedData, Data & plainText );
    VerificationResult verificationResult() const;

    //
    // Combined Decryption and Signature Verification
    //

    Data decryptAndVerify( const Data & cipherText, /*SignatureStatus & status,*/ GpgME::Error & e );
    GpgME::Error startVerifyingDecryption( const Data & cipherText, Data & plainText );

    //
    // Signing
    //

    void clearSigningKeys();
    GpgME::Error addSigningKey( const Key & signer );
    Key signingKey( unsigned int index ) const;

    enum SignatureMode { Normal, Detached, Clearsigned };
    SigningResult sign( const Data & plainText, Data & signature, SignatureMode mode );
    GpgME::Error startSigning( const Data & plainText, Data & signature, SignatureMode mode );
    SigningResult signingResult() const;

    //
    // Encryption
    //

    enum EncryptionFlags { None=0, AlwaysTrust };
    EncryptionResult encrypt( const std::vector<Key> & recipients, const Data & plainText, Data & cipherText, EncryptionFlags flags );
    GpgME::Error encryptSymmetrically( const Data & plainText, Data & cipherText );
    GpgME::Error startEncryption( const std::vector<Key> & recipients, const Data & plainText, Data & cipherText, EncryptionFlags flags );
    EncryptionResult encryptionResult() const;

    //
    // Combined Signing and Encryption
    //

#if 0
    Data encryptAndSign( const std::vector<Key> & recipients, const Data & plainText, GpgME::Error & e );
    GpgME::Error startEncryptingSigning( const std::vector<Key> & recipients, const Data & plainText, Data & cipherText );
#endif

    //
    //
    // Run Control
    //
    //

    bool poll();
    GpgME::Error wait();
    GpgME::Error lastError() const;
    void cancelPendingOperation();

    class Private;
    Private * impl() const { return d; }
  private:
    Private * d;

  private: // disable...
    Context( const Context & );
    const Context & operator=( const Context & );
  };

  //
  //
  // Globals
  //
  //

  Context * wait( GpgME::Error & e, bool hang=true );
  typedef void (*IdleFunction)(void);
  IdleFunction registerIdleFunction( IdleFunction idleFunction );

  typedef void (*IOCallback)( void * data, int fd );
  

} // namespace GpgME

#endif // __GPGMEPP_CONTEXT_H__
