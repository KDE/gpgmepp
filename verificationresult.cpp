/*
  verificationresult.cpp - wraps a gpgme verify result
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

#include <gpgme++/config-gpgme++.h>
#include <gpgme++/verificationresult.h>
#include <gpgme++/notation.h>
#include "shared.h"
#include "result_p.h"
#include "util.h"

#include <gpgme.h>

#include <algorithm>
#include <string>
#include <cstring>
#include <cstdlib>

class GpgME::VerificationResult::Private : public GpgME::Shared {
public:
  Private( const gpgme_verify_result_t r ) : Shared() {
    if ( !r )
      return;
#ifdef HAVE_GPGME_VERIFY_RESULT_T_FILE_NAME
    file_name = r->file_name;
#endif
    // copy recursively, using compiler-generated copy ctor.
    // We just need to handle the pointers in the structs:
    for ( gpgme_signature_t is = r->signatures ; is ; is = is->next ) {
      gpgme_signature_t scopy = new _gpgme_signature( *is );
      if ( is->fpr )
	scopy->fpr = strdup( is->fpr );
      scopy->next = 0;
      sigs.push_back( scopy );
      // copy notations:
      nota.push_back( std::vector<Nota>() );
      purls.push_back( 0 );
      for ( gpgme_sig_notation_t in = is->notations ; in ; in = in->next ) {
	if ( !in->name ) {
	  if ( in->value )
	    purls.back() = strdup( in->value ); // policy url
	  continue;
	}
#ifdef HAVE_GPGME_SIG_NOTATION_FLAGS_T
	Nota n = { 0, 0, in->flags };
#else
        Nota n = { 0, 0 };
#endif
	n.name = strdup( in->name );
	if ( in->value )
	  n.value = strdup( in->value );
	nota.back().push_back( n );
      }
    }
  }
  ~Private() {
    for ( std::vector<gpgme_signature_t>::iterator it = sigs.begin() ; it != sigs.end() ; ++it ) {
      std::free( (*it)->fpr );
      delete *it; *it = 0;
    }
    for ( std::vector< std::vector<Nota> >::iterator it = nota.begin() ; it != nota.end() ; ++it )
      for ( std::vector<Nota>::iterator jt = it->begin() ; jt != it->end() ; ++jt ) {
	std::free( jt->name );  jt->name = 0;
	std::free( jt->value ); jt->value = 0;
      }
    std::for_each( purls.begin(), purls.end(), &std::free );
  }

  struct Nota {
    char * name;
    char * value;
#ifdef HAVE_GPGME_SIG_NOTATION_FLAGS_T
    gpgme_sig_notation_flags_t flags;
#endif
  };

  std::vector<gpgme_signature_t> sigs;
  std::vector< std::vector<Nota> > nota;
  std::vector<char*> purls;
  std::string file_name;
};

GpgME::VerificationResult::VerificationResult( gpgme_ctx_t ctx, int error )
  : GpgME::Result( error ), d( 0 )
{
  if ( error || !ctx )
    return;
  gpgme_verify_result_t res = gpgme_op_verify_result( ctx );
  if ( !res )
    return;
  d = new Private( res );
  d->ref();
}

make_standard_stuff(VerificationResult)

const char * GpgME::VerificationResult::fileName() const {
  return d ? d->file_name.c_str() : 0 ;
}

unsigned int GpgME::VerificationResult::numSignatures() const {
  return d ? d->sigs.size() : 0 ;
}

GpgME::Signature GpgME::VerificationResult::signature( unsigned int idx ) const {
  return Signature( d, idx );
}

std::vector<GpgME::Signature> GpgME::VerificationResult::signatures() const {
  if ( !d )
    return std::vector<Signature>();
  std::vector<Signature> result;
  result.reserve( d->sigs.size() );
  for ( unsigned int i = 0 ; i < d->sigs.size() ; ++i )
    result.push_back( Signature( d, i ) );
  return result;
}






GpgME::Signature::Signature( VerificationResult::Private * parent, unsigned int i )
  : d( parent ), idx( i )
{
  if ( d )
    d->ref();
}

GpgME::Signature::Signature() : d( 0 ), idx( 0 ) {}

GpgME::Signature::Signature( const Signature & other )
  : d( other.d ), idx( other.idx )
{
  if ( d )
    d->ref();
}

GpgME::Signature::~Signature() {
  if ( d )
    d->unref();
}

bool GpgME::Signature::isNull() const {
  return !d || idx >= d->sigs.size() ;
}


GpgME::Signature::Summary GpgME::Signature::summary() const {
  if ( isNull() )
    return None;
  gpgme_sigsum_t sigsum = d->sigs[idx]->summary;
  unsigned int result = 0;
  if ( sigsum & GPGME_SIGSUM_VALID       ) result |= Valid;
  if ( sigsum & GPGME_SIGSUM_GREEN       ) result |= Green;
  if ( sigsum & GPGME_SIGSUM_RED         ) result |= Red;
  if ( sigsum & GPGME_SIGSUM_KEY_REVOKED ) result |= KeyRevoked;
  if ( sigsum & GPGME_SIGSUM_KEY_EXPIRED ) result |= KeyExpired;
  if ( sigsum & GPGME_SIGSUM_SIG_EXPIRED ) result |= SigExpired;
  if ( sigsum & GPGME_SIGSUM_KEY_MISSING ) result |= KeyMissing;
  if ( sigsum & GPGME_SIGSUM_CRL_MISSING ) result |= CrlMissing;
  if ( sigsum & GPGME_SIGSUM_CRL_TOO_OLD ) result |= CrlTooOld;
  if ( sigsum & GPGME_SIGSUM_BAD_POLICY  ) result |= BadPolicy;
  if ( sigsum & GPGME_SIGSUM_SYS_ERROR   ) result |= SysError;
  return static_cast<Summary>( result );
}

const char * GpgME::Signature::fingerprint() const {
  return isNull() ? 0 : d->sigs[idx]->fpr ;
}

GpgME::Error GpgME::Signature::status() const {
  return Error( isNull() ? 0 : d->sigs[idx]->status );
}

time_t GpgME::Signature::creationTime() const {
  return static_cast<time_t>( isNull() ? 0 : d->sigs[idx]->timestamp );
}

time_t GpgME::Signature::expirationTime() const {
  return static_cast<time_t>( isNull() ? 0 : d->sigs[idx]->exp_timestamp );
}

bool GpgME::Signature::neverExpires() const {
  return expirationTime() == (time_t)0;
}

bool GpgME::Signature::isWrongKeyUsage() const {
  return !isNull() && d->sigs[idx]->wrong_key_usage;
}

GpgME::Signature::PKAStatus GpgME::Signature::pkaStatus() const {
#ifdef HAVE_GPGME_SIGNATURE_T_PKA_FIELDS
  if ( !isNull() )
    return static_cast<PKAStatus>( d->sigs[idx]->pka_trust );
#endif
  return UnknownPKAStatus;
}

const char * GpgME::Signature::pkaAddress() const {
#ifdef HAVE_GPGME_SIGNATURE_T_PKA_FIELDS
  if ( !isNull() )
    return d->sigs[idx]->pka_address;
#endif
  return 0;
}

GpgME::Signature::Validity GpgME::Signature::validity() const {
  if ( isNull() )
    return Unknown;
  switch ( d->sigs[idx]->validity ) {
  default:
  case GPGME_VALIDITY_UNKNOWN:   return Unknown;
  case GPGME_VALIDITY_UNDEFINED: return Undefined;
  case GPGME_VALIDITY_NEVER:     return Never;
  case GPGME_VALIDITY_MARGINAL:  return Marginal;
  case GPGME_VALIDITY_FULL:      return Full;
  case GPGME_VALIDITY_ULTIMATE:  return Ultimate;
  }
}


char GpgME::Signature::validityAsString() const {
  if ( isNull() )
    return '?';
  switch ( d->sigs[idx]->validity ) {
  default:
  case GPGME_VALIDITY_UNKNOWN:   return '?';
  case GPGME_VALIDITY_UNDEFINED: return 'q';
  case GPGME_VALIDITY_NEVER:     return 'n';
  case GPGME_VALIDITY_MARGINAL:  return 'm';
  case GPGME_VALIDITY_FULL:      return 'f';
  case GPGME_VALIDITY_ULTIMATE:  return 'u';
  }
}

GpgME::Error GpgME::Signature::nonValidityReason() const {
  return Error( isNull() ? 0 : d->sigs[idx]->validity_reason );
}

unsigned int GpgME::Signature::publicKeyAlgorithm() const {
#ifdef HAVE_GPGME_SIGNATURE_T_ALGORITHM_FIELDS
  if ( !isNull() )
    return d->sigs[idx]->pubkey_algo;
#endif
  return 0;
}

const char * GpgME::Signature::publicKeyAlgorithmAsString() const {
#ifdef HAVE_GPGME_SIGNATURE_T_ALGORITHM_FIELDS
  if ( !isNull() )
    return gpgme_pubkey_algo_name( d->sigs[idx]->pubkey_algo );
#endif
  return 0;
}

unsigned int GpgME::Signature::hashAlgorithm() const {
#ifdef HAVE_GPGME_SIGNATURE_T_ALGORITHM_FIELDS
  if ( !isNull() )
    return d->sigs[idx]->hash_algo;
#endif
  return 0;
}

const char * GpgME::Signature::hashAlgorithmAsString() const {
#ifdef HAVE_GPGME_SIGNATURE_T_ALGORITHM_FIELDS
  if ( !isNull() )
    return gpgme_hash_algo_name( d->sigs[idx]->hash_algo );
#endif
  return 0;
}

const char * GpgME::Signature::policyURL() const {
    return isNull() ? 0 : d->purls[idx] ;
}

GpgME::Notation GpgME::Signature::notation( unsigned int nidx ) const {
  return GpgME::Notation( d, idx, nidx );
}

std::vector<GpgME::Notation> GpgME::Signature::notations() const {
  if ( isNull() )
    return std::vector<GpgME::Notation>();
  std::vector<GpgME::Notation> result;
  result.reserve( d->nota[idx].size() );
  for ( unsigned int i = 0 ; i < d->nota[idx].size() ; ++i )
    result.push_back( GpgME::Notation( d, idx, i ) );
  return result;
}


class GpgME::Notation::Private {
public:
    Private() : d( 0 ), sidx( 0 ), nidx( 0 ), nota( 0 ) {}
    Private( VerificationResult::Private * priv, unsigned int sindex, unsigned int nindex )
	: d( priv ), sidx( sindex ), nidx( nindex ), nota( 0 )
    {
      if ( d )
        d->ref();
    }
    Private( gpgme_sig_notation_t n )
	: d( 0 ), sidx( 0 ), nidx( 0 ), nota( n ? new _gpgme_sig_notation( *n ) : 0 )
    {
      if ( nota && nota->name )
        nota->name = strdup( nota->name );
      if ( nota && nota->value )
        nota->value = strdup( nota->value );
    }
    Private( const Private & other )
	: d( other.d ), sidx( other.sidx ), nidx( other.nidx ), nota( other.nota )
    {
      if ( d )
        d->ref();
      if ( nota ) {
        nota->name = strdup( nota->name );
        nota->value = strdup( nota->value );
      }
    }
    ~Private() {
      if ( d )
        d->unref();
      if ( nota ) {
        std::free( nota->name );  nota->name = 0;
        std::free( nota->value ); nota->value = 0;
      }
    }

    VerificationResult::Private * d;
    unsigned int sidx, nidx;
    gpgme_sig_notation_t nota;
};


GpgME::Notation::Notation( VerificationResult::Private * parent, unsigned int sindex, unsigned int nindex )
  : d( new Private( parent, sindex, nindex ) )
{

}

GpgME::Notation::Notation( gpgme_sig_notation_t nota )
  : d( new Private( nota ) )
{

}

GpgME::Notation::Notation() : d( 0 ) {}

GpgME::Notation::Notation( const Notation & other )
  : d( other.d ? new Private( *other.d ) : 0 )
{

}

GpgME::Notation::~Notation() {
  delete d; d = 0;
}

bool GpgME::Notation::isNull() const {
    if ( !d )
	return true;
    if ( d->d )
	return d->sidx >= d->d->nota.size() || d->nidx >= d->d->nota[d->sidx].size() ;
    return !d->nota;
}


const char * GpgME::Notation::name() const {
    return
	isNull() ? 0 :
	d->d ? d->d->nota[d->sidx][d->nidx].name :
	d->nota ? d->nota->name : 0 ;
}

const char * GpgME::Notation::value() const {
    return
	isNull() ? 0 :
	d->d ? d->d->nota[d->sidx][d->nidx].value :
	d->nota ? d->nota->value : 0 ;
}

GpgME::Notation::Flags GpgME::Notation::flags() const {
    return
      convert_from_gpgme_sig_notation_flags_t(
#ifdef HAVE_GPGME_SIG_NOTATION_FLAGS_T
        isNull() ? 0 : 
        d->d ? d->d->nota[d->sidx][d->nidx].flags :
        d->nota ? d->nota->flags : 0 );
#else
        0 );
#endif
}

bool GpgME::Notation::isHumanReadable() const {
    return flags() & HumanReadable;
}

bool GpgME::Notation::isCritical() const {
    return flags() & Critical;
}
