/*
 * Copyright (c) 1983, 1990, 1993
 *    The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
 * Portions Copyright (c) 1996-1999 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#include <sys/types.h>
#include <sys/param.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <ctype.h>

#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

/*
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 */
int
private__inet_aton(const char *cp, struct in_addr *addr)
{
	static const in_addr_t max[4] = { 0xffffffff, 0xffffff, 0xffff, 0xff };
	in_addr_t val;
	char c;
	union iaddr {
	  uint8_t bytes[4];
	  uint32_t word;
	} res;
	uint8_t *pp = res.bytes;
	int digit;

	int saved_errno = errno;
	__set_errno (0);

	res.word = 0;

	c = *cp;
	for (;;) {
		/*
		 * Collect number up to ``.''.
		 * Values are specified as for C:
		 * 0x=hex, 0=octal, isdigit=decimal.
		 */
		if (!isdigit(c))
			goto ret_0;
		{
			char *endp;
			unsigned long ul = strtoul (cp, (char **) &endp, 0);
			if (ul == ULONG_MAX && errno == ERANGE)
				goto ret_0;
			if (ul > 0xfffffffful)
				goto ret_0;
			val = ul;
			digit = cp != endp;
			cp = endp;
		}
		c = *cp;
		if (c == '.') {
			/*
			 * Internet format:
			 *	a.b.c.d
			 *	a.b.c	(with c treated as 16 bits)
			 *	a.b	(with b treated as 24 bits)
			 */
			if (pp > res.bytes + 2 || val > 0xff)
				goto ret_0;
			*pp++ = val;
			c = *++cp;
		} else
			break;
	}
	/*
	 * Check for trailing characters.
	 */
	if (c != '\0' && (!isascii(c) || !isspace(c)))
		goto ret_0;
	/*
	 * Did we get a valid digit?
	 */
	if (!digit)
		goto ret_0;

	/* Check whether the last part is in its limits depending on
	   the number of parts in total.  */
	if (val > max[pp - res.bytes])
	  goto ret_0;

	if (addr != NULL)
		addr->s_addr = res.word | htonl (val);

	__set_errno (saved_errno);
	return (1);

ret_0:
	__set_errno (saved_errno);
	return (0);
}
weak_alias (private__inet_aton, inet_aton)
libc_hidden_def (private__inet_aton)
libc_hidden_weak (inet_aton)

/* Copyright (C) 1997-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by H.J. Lu <hjl@gnu.ai.mit.edu>, 1997.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>
#include <resolv.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "nsswitch.h"

#ifdef USE_NSCD
# define inet_aton private__inet_aton
# include <nscd/nscd_proto.h>
#endif

int
patched___nss_hostname_digits_dots (const char *name, struct hostent *resbuf,
			    char **buffer, size_t *buffer_size,
			    size_t buflen, struct hostent **result,
			    enum nss_status *status, int af, int *h_errnop)
{
  int save;

  /* We have to test for the use of IPv6 which can only be done by
     examining `_res'.  */
  if (__res_maybe_init (&_res, 0) == -1)
    {
      if (h_errnop)
	*h_errnop = NETDB_INTERNAL;
      *result = NULL;
      return -1;
    }

  /*
   * disallow names consisting only of digits/dots, unless
   * they end in a dot.
   */
  if (isdigit (name[0]) || isxdigit (name[0]) || name[0] == ':')
    {
      const char *cp;
      char *hostname;
      typedef unsigned char host_addr_t[16];
      host_addr_t *host_addr;
      typedef char *host_addr_list_t[2];
      host_addr_list_t *h_addr_ptrs;
      char **h_alias_ptr;
      size_t size_needed;
      int addr_size;

      switch (af)
	{
	case AF_INET:
	  addr_size = INADDRSZ;
	  break;

	case AF_INET6:
	  addr_size = IN6ADDRSZ;
	  break;

	default:
	  af = (_res.options & RES_USE_INET6) ? AF_INET6 : AF_INET;
	  addr_size = af == AF_INET6 ? IN6ADDRSZ : INADDRSZ;
	  break;
	}

      size_needed = (sizeof (*host_addr)
		     + sizeof (*h_addr_ptrs) + strlen (name) + 1);

      if (buffer_size == NULL)
        {
	  if (buflen < size_needed)
	    {
	      if (h_errnop != NULL)
		*h_errnop = TRY_AGAIN;
	      __set_errno (ERANGE);
	      goto done;
	    }
	}
      else if (buffer_size != NULL && *buffer_size < size_needed)
	{
	  char *new_buf;
	  *buffer_size = size_needed;
	  new_buf = (char *) realloc (*buffer, *buffer_size);

	  if (new_buf == NULL)
	    {
	      save = errno;
	      free (*buffer);
	      *buffer = NULL;
	      *buffer_size = 0;
	      __set_errno (save);
	      if (h_errnop != NULL)
		*h_errnop = TRY_AGAIN;
	      *result = NULL;
	      goto done;
	    }
	  *buffer = new_buf;
	}

      memset (*buffer, '\0', size_needed);

      host_addr = (host_addr_t *) *buffer;
      h_addr_ptrs = (host_addr_list_t *)
	((char *) host_addr + sizeof (*host_addr));
      h_alias_ptr = (char **) ((char *) h_addr_ptrs + sizeof (*h_addr_ptrs));
      hostname = (char *) h_alias_ptr + sizeof (*h_alias_ptr);

      if (isdigit (name[0]))
	{
	  for (cp = name;; ++cp)
	    {
	      if (*cp == '\0')
		{
		  int ok;

		  if (*--cp == '.')
		    break;

		  /* All-numeric, no dot at the end. Fake up a hostent as if
		     we'd actually done a lookup.  What if someone types
		     255.255.255.255?  The test below will succeed
		     spuriously... ???  */
		  if (af == AF_INET)
		    ok = private__inet_aton (name, (struct in_addr *) host_addr);
		  else
		    {
		      assert (af == AF_INET6);
		      ok = inet_pton (af, name, host_addr) > 0;
		    }
		  if (! ok)
		    {
		      *h_errnop = HOST_NOT_FOUND;
		      if (buffer_size)
			*result = NULL;
		      goto done;
		    }

		  resbuf->h_name = strcpy (hostname, name);
		  h_alias_ptr[0] = NULL;
		  resbuf->h_aliases = h_alias_ptr;
		  (*h_addr_ptrs)[0] = (char *) host_addr;
		  (*h_addr_ptrs)[1] = NULL;
		  resbuf->h_addr_list = *h_addr_ptrs;
		  if (af == AF_INET && (_res.options & RES_USE_INET6))
		    {
		      /* We need to change the IP v4 address into the
			 IP v6 address.  */
		      char tmp[INADDRSZ];
		      char *p = (char *) host_addr;
		      int i;

		      /* Save a copy of the IP v4 address. */
		      memcpy (tmp, host_addr, INADDRSZ);
		      /* Mark this ipv6 addr as a mapped ipv4. */
		      for (i = 0; i < 10; i++)
			*p++ = 0x00;
		      *p++ = 0xff;
		      *p++ = 0xff;
		      /* Copy the IP v4 address. */
		      memcpy (p, tmp, INADDRSZ);
		      resbuf->h_addrtype = AF_INET6;
		      resbuf->h_length = IN6ADDRSZ;
		    }
		  else
		    {
		      resbuf->h_addrtype = af;
		      resbuf->h_length = addr_size;
		    }
		  if (h_errnop != NULL)
		    *h_errnop = NETDB_SUCCESS;
		  if (buffer_size == NULL)
		    *status = NSS_STATUS_SUCCESS;
		  else
		   *result = resbuf;
		  goto done;
		}

	      if (!isdigit (*cp) && *cp != '.')
		break;
	    }
	}

      if ((isxdigit (name[0]) && strchr (name, ':') != NULL) || name[0] == ':')
	{
	  const char *cp;
	  char *hostname;
	  typedef unsigned char host_addr_t[16];
	  host_addr_t *host_addr;
	  typedef char *host_addr_list_t[2];
	  host_addr_list_t *h_addr_ptrs;
	  size_t size_needed;
	  int addr_size;

	  switch (af)
	    {
	    default:
	      af = (_res.options & RES_USE_INET6) ? AF_INET6 : AF_INET;
	      if (af == AF_INET6)
		{
		  addr_size = IN6ADDRSZ;
		  break;
		}
	      /* FALLTHROUGH */

	    case AF_INET:
	      /* This is not possible.  We cannot represent an IPv6 address
		 in an `struct in_addr' variable.  */
	      *h_errnop = HOST_NOT_FOUND;
	      *result = NULL;
	      goto done;

	    case AF_INET6:
	      addr_size = IN6ADDRSZ;
	      break;
	    }

	  size_needed = (sizeof (*host_addr)
			 + sizeof (*h_addr_ptrs) + strlen (name) + 1);

	  if (buffer_size == NULL && buflen < size_needed)
	    {
	      if (h_errnop != NULL)
		*h_errnop = TRY_AGAIN;
	      __set_errno (ERANGE);
	      goto done;
	    }
	  else if (buffer_size != NULL && *buffer_size < size_needed)
	    {
	      char *new_buf;
	      *buffer_size = size_needed;
	      new_buf = realloc (*buffer, *buffer_size);

	      if (new_buf == NULL)
		{
		  save = errno;
		  free (*buffer);
		  __set_errno (save);
		  *buffer = NULL;
		  *buffer_size = 0;
		  *result = NULL;
		  goto done;
		}
	      *buffer = new_buf;
	    }

	  memset (*buffer, '\0', size_needed);

	  host_addr = (host_addr_t *) *buffer;
	  h_addr_ptrs = (host_addr_list_t *)
	    ((char *) host_addr + sizeof (*host_addr));
	  hostname = (char *) h_addr_ptrs + sizeof (*h_addr_ptrs);

	  for (cp = name;; ++cp)
	    {
	      if (!*cp)
		{
		  if (*--cp == '.')
		    break;

		  /* All-IPv6-legal, no dot at the end. Fake up a
		     hostent as if we'd actually done a lookup.  */
		  if (inet_pton (AF_INET6, name, host_addr) <= 0)
		    {
		      *h_errnop = HOST_NOT_FOUND;
		      if (buffer_size)
			*result = NULL;
		      goto done;
		    }

		  resbuf->h_name = strcpy (hostname, name);
		  h_alias_ptr[0] = NULL;
		  resbuf->h_aliases = h_alias_ptr;
		  (*h_addr_ptrs)[0] = (char *) host_addr;
		  (*h_addr_ptrs)[1] = (char *) 0;
		  resbuf->h_addr_list = *h_addr_ptrs;
		  resbuf->h_addrtype = AF_INET6;
		  resbuf->h_length = addr_size;
		  *h_errnop = NETDB_SUCCESS;
		  if (buffer_size == NULL)
		    *status = NSS_STATUS_SUCCESS;
		  else
		    *result = resbuf;
		  goto done;
		}

	      if (!isxdigit (*cp) && *cp != ':' && *cp != '.')
		break;
	    }
	}
    }

  return 0;

done:
  return 1;
}
libc_hidden_def (__nss_hostname_digits_dots)


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "trampoline.h"

void patch_constructor(void)
{
}

void patch_destructor(void)
{
}
