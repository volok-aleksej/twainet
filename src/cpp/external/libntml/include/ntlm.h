/* ntlm.h --- Header file for libntlm.                                -*- c -*-
 *
 * This file is part of libntlm.
 *
 * Libntlm is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libntlm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libntlm; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#ifndef NTLM_H
# define NTLM_H

# ifdef __cplusplus
extern "C"
{
# endif

#ifdef WIN32
#	define LIBNTLM_EXPORT(func, retType) typedef retType (*g_##func)
#else
#	define LIBNTLM_EXPORT(func, retType) extern retType *func
#endif/*WIN32*/
  /* Get FILE. */
#include <stdio.h>

  typedef unsigned short uint16;
  typedef unsigned int uint32;
  typedef unsigned char uint8;

#define NTLM_VERSION "1.4"

/*
 * These structures are byte-order dependant, and should not
 * be manipulated except by the use of the routines provided
 */

  typedef struct
  {
    uint16 len;
    uint16 maxlen;
    uint32 offset;
  } tSmbStrHeader;

  typedef struct
  {
    char ident[8];
    uint32 msgType;
    uint32 flags;
    tSmbStrHeader user;
    tSmbStrHeader domain;
    uint8 buffer[1024];
    uint32 bufIndex;
  } tSmbNtlmAuthRequest;

  typedef struct
  {
    char ident[8];
    uint32 msgType;
    tSmbStrHeader uDomain;
    uint32 flags;
    uint8 challengeData[8];
    uint8 reserved[8];
    tSmbStrHeader emptyString;
    uint8 buffer[1024];
    uint32 bufIndex;
  } tSmbNtlmAuthChallenge;


  typedef struct
  {
    char ident[8];
    uint32 msgType;
    tSmbStrHeader lmResponse;
    tSmbStrHeader ntResponse;
    tSmbStrHeader uDomain;
    tSmbStrHeader uUser;
    tSmbStrHeader uWks;
    tSmbStrHeader sessionKey;
    uint32 flags;
    uint8 buffer[1024];
    uint32 bufIndex;
  } tSmbNtlmAuthResponse;

/* public: */

#define SmbLength(ptr) (((ptr)->buffer - (uint8*)(ptr)) + (ptr)->bufIndex)

  LIBNTLM_EXPORT(dumpSmbNtlmAuthRequest, void)(FILE * fp, tSmbNtlmAuthRequest * request);
  LIBNTLM_EXPORT(dumpSmbNtlmAuthChallenge, void)(FILE * fp, tSmbNtlmAuthChallenge * challenge);
  LIBNTLM_EXPORT(dumpSmbNtlmAuthResponse, void)(FILE * fp, tSmbNtlmAuthResponse * response);

  LIBNTLM_EXPORT(buildSmbNtlmAuthRequest, void)(tSmbNtlmAuthRequest * request,
			     const char *user, const char *domain);

  /* Same as buildSmbNtlmAuthRequest, but won't treat @ in USER as a
     DOMAIN. */
  LIBNTLM_EXPORT(buildSmbNtlmAuthRequest_noatsplit, void)(tSmbNtlmAuthRequest * request,
				       const char *user, const char *domain);

  LIBNTLM_EXPORT(buildSmbNtlmAuthResponse, void)(tSmbNtlmAuthChallenge * challenge,
			      tSmbNtlmAuthResponse * response,
			      const char *user, const char *password);

  /* Same as buildSmbNtlmAuthResponse, but won't treat @ in USER as a
     REALM. */
  LIBNTLM_EXPORT(buildSmbNtlmAuthResponse_noatsplit, void)(tSmbNtlmAuthChallenge * challenge,
					tSmbNtlmAuthResponse * response,
					const char *user,
					const char *password);

  /* smbencrypt.c */
  LIBNTLM_EXPORT(ntlm_smb_encrypt, void)(const char *passwd,
		    const uint8 * challenge,
		    uint8 * answer);
  LIBNTLM_EXPORT(ntlm_smb_nt_encrypt, void)(const char *passwd,
		       const uint8 * challenge,
		       uint8 * answer);

  LIBNTLM_EXPORT(ntlm_check_version, const char*)(const char *req_version);

# ifdef __cplusplus
}
# endif

#endif				/* NTLM_H */
