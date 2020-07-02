/* C code produced by gperf version 3.0.3 */
/* Command-line: /usr/bin/gperf -m 10 -C -G -D -t  */
/* Computed positions: -k'1-2,6,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


/*
 * Copyright (C) 2005 Andreas Steffen
 * Hochschule fuer Technik Rapperswil, Switzerland
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <string.h>

#include "keywords.h"

struct kw_entry {
    char *name;
    kw_token_t token;
};

#define TOTAL_KEYWORDS 187
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 17
#define MIN_HASH_VALUE 18
#define MAX_HASH_VALUE 392
/* maximum key range = 375, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned short asso_values[] =
    {
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393,  57,
      191, 393,  77, 393,   9, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393,   5, 393,  64, 393,  79,
       97,   3, 113,  50,  82,   2, 393, 152,   3, 104,
       41,  40,  30, 393,   7,  19,  10, 186,   6,   9,
       12,  23,   2, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393, 393, 393, 393, 393,
      393, 393, 393, 393, 393, 393
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
      case 4:
      case 3:
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static const struct kw_entry wordlist[] =
  {
    {"lifetime",          KW_KEYLIFE},
    {"left",              KW_LEFT},
    {"rightimei",         KW_RIGHTIMEI},
    {"leftfirewall",      KW_LEFTFIREWALL},
    {"right",             KW_RIGHT},
    {"leftcert",          KW_LEFTCERT},
    {"leftsendcert",      KW_LEFTSENDCERT},
    {"rightikeport",      KW_RIGHTIKEPORT},
    {"rightintsubnet",    KW_RIGHTINTSUBNET},
    {"leftprotoport",     KW_LEFTPROTOPORT},
    {"rightikeportnatt",  KW_RIGHTIKEPORTNATT},
    {"rekey",             KW_REKEY},
    {"type",              KW_TYPE},
    {"leftsigkey",        KW_LEFTSIGKEY},
    {"leftgroups",        KW_LEFTGROUPS},
    {"leftallowany",      KW_LEFTALLOWANY},
    {"leftcertpolicy",    KW_LEFTCERTPOLICY},
    {"rightsubnet",       KW_RIGHTSUBNET},
    {"leftnexthop",       KW_LEFT_DEPRECATED},
    {"rightsendcert",     KW_RIGHTSENDCERT},
    {"server_nocert",     KW_SERVER_NOCERT},
    {"rightrsasigkey",    KW_RIGHTSIGKEY},
    {"esp",               KW_ESP},
    {"lifebytes",         KW_LIFEBYTES},
    {"is_emergency",     KW_IS_EMERGENCY},
    {"leftrsasigkey",     KW_LEFTSIGKEY},
    {"rightsigkey",       KW_RIGHTSIGKEY},
    {"rightprotoport",    KW_RIGHTPROTOPORT},
    {"rightidr_apn",      KW_RIGHTIDR_APN},
    {"leftforcetsifull",  KW_LEFTFORCETSIFULL},
    {"retrans_base",      KW_RETRANS_BASE},
    {"leftintsubnet",     KW_LEFTINTSUBNET},
    {"rightsourceip",     KW_RIGHTSOURCEIP},
    {"plutostart",        KW_SETUP_DEPRECATED},
    {"leftdns",           KW_LEFTDNS},
    {"strictcrlpolicy",   KW_STRICTCRLPOLICY},
    {"inactivity",        KW_INACTIVITY},
    {"lefthostaccess",    KW_LEFTHOSTACCESS},
    {"installpolicy",     KW_INSTALLPOLICY},
    {"retrans_tries",     KW_RETRANS_TRIES},
    {"encrkeydisplay",    KW_ENCRKEYDISPLAY},
    {"rightsubnetwithin", KW_RIGHTSUBNET},
    {"leftupdown",        KW_LEFTUPDOWN},
    {"leftsourceip",      KW_LEFTSOURCEIP},
    {"rightgroups",       KW_RIGHTGROUPS},
    {"virtual_private",   KW_SETUP_DEPRECATED},
    {"rightnexthop",      KW_RIGHT_DEPRECATED},
    {"crluri",            KW_CRLURI},
    {"lifepackets",       KW_LIFEPACKETS},
    {"eap",               KW_CONN_DEPRECATED},
    {"retrans_to",        KW_RETRANS_TO},
    {"no_initct",         KW_NO_INIT_CONTACT},
    {"certuribase",       KW_CERTURIBASE},
    {"certreq_critical",    KW_CERTREQ_CRITICAL},
    {"rightcustcpimei",   KW_RIGHTCUSTCPIMEI},
    {"rightcert",         KW_RIGHTCERT},
    {"crlcheckinterval",  KW_SETUP_DEPRECATED},
    {"rightallowany",     KW_RIGHTALLOWANY},
    {"also",              KW_ALSO},
    {"reqid",             KW_REQID},
    {"rightcustcppcscf6", KW_RIGHTCUSTCPPCSCF6},
    {"rightid",           KW_RIGHTID},
    {"plutostderrlog",    KW_SETUP_DEPRECATED},
    {"xauth_identity",    KW_XAUTH_IDENTITY},
    {"leftimei",          KW_LEFTIMEI},
    {"postpluto",         KW_SETUP_DEPRECATED},
    {"righthostaccess",   KW_RIGHTHOSTACCESS},
    {"rightcertpolicy",   KW_RIGHTCERTPOLICY},
    {"packetdefault",     KW_SETUP_DEPRECATED},
    {"nat_traversal",     KW_SETUP_DEPRECATED},
    {"pcscf_restore",      KW_PCSCF_RESTORE},
    {"rightdns",          KW_RIGHTDNS},
    {"rekeyfuzz",         KW_REKEYFUZZ},
    {"ocspuri",           KW_OCSPURI},
    {"leftforcetsi64",    KW_LEFTFORCETSI64},
    {"rightfirewall",     KW_RIGHTFIREWALL},
    {"leftca",            KW_LEFTCA},
    {"device_identity",    KW_DEVICE_IDENTITY},
    {"rightforcetsifull", KW_RIGHTFORCETSIFULL},
    {"no_eaponly",        KW_NO_EAP_ONLY},
    {"aggressive",        KW_AGGRESSIVE},
    {"compress",          KW_COMPRESS},
    {"hidetos",           KW_SETUP_DEPRECATED},
    {"mediated_by",       KW_MEDIATED_BY},
    {"crluri1",           KW_CRLURI},
    {"ocsp",              KW_OCSP},
    {"rightsourceif",     KW_RIGHTSOURCEIF},
    {"leftidr_apn",       KW_LEFTIDR_APN},
    {"mobike",	           KW_MOBIKE},
    {"ldaphost",          KW_CA_DEPRECATED},
    {"rightca",           KW_RIGHTCA},
    {"ike",               KW_IKE},
    {"dpddelay",          KW_DPDDELAY},
    {"rightpcscf",        KW_RIGHTPCSCF},
    {"xauth",             KW_XAUTH},
    {"rightauth",         KW_RIGHTAUTH},
    {"pfs",               KW_PFS_DEPRECATED},
    {"rekeymargin",       KW_REKEYMARGIN},
    {"mediation",         KW_MEDIATION},
    {"interface",         KW_INTERFACE},
    {"cacert",            KW_CACERT},
    {"leftsourceif",      KW_LEFTSOURCEIF},
    {"liveness_check",     KW_LIVENESS_CHECK},
    {"ldapbase",          KW_CA_DEPRECATED},
    {"force_keepalive",   KW_SETUP_DEPRECATED},
    {"fragmentation",     KW_FRAGMENTATION},
    {"rightintnetmask",   KW_RIGHTINTNETMASK},
    {"leftikeport",       KW_LEFTIKEPORT},
    {"reauth",            KW_REAUTH},
    {"rightcustcppcscf4", KW_RIGHTCUSTCPPCSCF4},
    {"leftikeportnatt",   KW_LEFTIKEPORTNATT},
    {"interfaces",        KW_SETUP_DEPRECATED},
    {"forceencaps",       KW_FORCEENCAPS},
    {"dpdaction",         KW_DPDACTION},
    {"nocrsend",          KW_SETUP_DEPRECATED},
    {"plutodebug",        KW_SETUP_DEPRECATED},
    {"ocspuri1",          KW_OCSPURI},
    {"keepalivedelay",    KW_KEEPALIVEDELAY},
    {"closeaction",       KW_CLOSEACTION},
    {"eap_identity",      KW_EAP_IDENTITY},
    {"hashandurl",        KW_HASHANDURL},
    {"dpd_noreply",       KW_DPD_NOREPLY},
    {"oostimeout",        KW_OOSTIMEOUT},
    {"tfc",               KW_TFC},
    {"leftid",            KW_LEFTID},
    {"leftpcscf",         KW_LEFTPCSCF},
    {"leftcustcpimei",    KW_LEFTCUSTCPIMEI},
    {"leftcert2",         KW_LEFTCERT2},
    {"rightid2",          KW_RIGHTID2},
    {"leftsubnet",        KW_LEFTSUBNET},
    {"leftintnetmask",    KW_LEFTINTNETMASK},
    {"rightforcetsi64",   KW_RIGHTFORCETSI64},
    {"leftgroups2",       KW_LEFTGROUPS2},
    {"me_peerid",         KW_ME_PEERID},
    {"leftcustcppcscf6",  KW_LEFTCUSTCPPCSCF6},
    {"mark_in",           KW_MARK_IN},
    {"pfsgroup",          KW_PFS_DEPRECATED},
    {"margintime",        KW_REKEYMARGIN},
    {"charonstart",       KW_SETUP_DEPRECATED},
    {"wsharkfiledump",    KW_WSHARKFILEDUMP},
    {"mark_out",          KW_MARK_OUT},
    {"ah",                KW_AH},
    {"keep_alive",        KW_SETUP_DEPRECATED},
    {"keyingtries",       KW_KEYINGTRIES},
    {"fragicmp",          KW_SETUP_DEPRECATED},
    {"marginbytes",       KW_MARGINBYTES},
    {"marginpackets",     KW_MARGINPACKETS},
    {"modeconfig",        KW_MODECONFIG},
    {"overridemtu",       KW_SETUP_DEPRECATED},
    {"rightupdown",       KW_RIGHTUPDOWN},
    {"keyexchange",       KW_KEYEXCHANGE},
    {"leftsubnetwithin",  KW_LEFTSUBNET},
    {"cachecrls",         KW_CACHECRLS},
    {"dpdtimeout",        KW_DPDTIMEOUT},
    {"pkcs11module",      KW_PKCS11_DEPRECATED},
    {"pkcs11keepstate",   KW_PKCS11_DEPRECATED},
    {"uniqueids",         KW_UNIQUEIDS},
    {"aaa_identity",      KW_AAA_IDENTITY},
    {"rightgroups2",      KW_RIGHTGROUPS2},
    {"charondebug",       KW_CHARONDEBUG},
    {"leftca2",           KW_LEFTCA2},
    {"ikedscp",           KW_IKEDSCP,},
    {"prepluto",          KW_SETUP_DEPRECATED},
    {"pkcs11initargs",    KW_PKCS11_DEPRECATED},
    {"pkcs11proxy",       KW_PKCS11_DEPRECATED},
    {"rightauth2",        KW_RIGHTAUTH2},
    {"wdrv_keepalive",    KW_WDRV_KEEPALIVE},
    {"skipcheckcert",     KW_SKIPCHECKCERT},
    {"fast_reauth",       KW_FAST_REAUTH},
    {"keylife",           KW_KEYLIFE},
    {"ikelifetime",       KW_IKELIFETIME},
    {"leftauth",          KW_LEFTAUTH},
    {"leftcustcppcscf4",  KW_LEFTCUSTCPPCSCF4},
    {"crluri2",           KW_CRLURI2},
    {"rightca2",          KW_RIGHTCA2},
    {"rightcert2",        KW_RIGHTCERT2},
    {"auto",              KW_AUTO},
    {"dumpdir",           KW_SETUP_DEPRECATED},
    {"leftid2",           KW_LEFTID2},
    {"authby",            KW_AUTHBY},
    {"klipsdebug",        KW_SETUP_DEPRECATED},
    {"mark",              KW_MARK},
    {"ocspuri2",          KW_OCSPURI2},
    {"addrchg_reauth",    KW_ADDRCHG_REAUTH},
    {"use_cfg_vip",       KW_USE_CFG_VIP},
    {"skipcheckid",       KW_SKIPCHECKID},
    {"leftauth2",         KW_LEFTAUTH2}
  };

static const short lookup[] =
  {
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,  -1,
      1,  -1,   2,   3,   4,  -1,  -1,   5,  -1,  -1,
     -1,   6,  -1,   7,  -1,   8,   9,  10,  11,  -1,
     12,  13,  14,  -1,  15,  -1,  16,  -1,  -1,  17,
     18,  19,  20,  21,  -1,  22,  23,  -1,  -1,  24,
     -1,  25,  26,  27,  28,  29,  30,  -1,  -1,  -1,
     31,  32,  33,  34,  -1,  -1,  -1,  35,  36,  37,
     -1,  -1,  38,  39,  40,  -1,  41,  42,  43,  44,
     45,  -1,  46,  -1,  -1,  -1,  47,  -1,  -1,  48,
     49,  50,  51,  52,  53,  54,  -1,  55,  56,  57,
     -1,  58,  59,  -1,  60,  61,  62,  -1,  63,  -1,
     64,  -1,  65,  -1,  -1,  66,  67,  68,  69,  -1,
     70,  -1,  -1,  71,  72,  73,  -1,  74,  75,  -1,
     76,  77,  78,  -1,  79,  -1,  80,  -1,  -1,  81,
     82,  83,  84,  85,  86,  87,  88,  -1,  89,  90,
     91,  92,  93,  94,  95,  96,  97,  98,  99, 100,
     -1, 101,  -1,  -1, 102, 103, 104, 105, 106, 107,
    108,  -1, 109, 110,  -1, 111, 112, 113,  -1, 114,
    115, 116,  -1,  -1,  -1, 117,  -1,  -1, 118, 119,
    120, 121,  -1,  -1, 122, 123, 124, 125, 126, 127,
    128,  -1, 129, 130, 131, 132, 133, 134, 135,  -1,
     -1, 136, 137, 138, 139,  -1, 140,  -1,  -1,  -1,
    141,  -1, 142,  -1,  -1, 143,  -1, 144,  -1, 145,
     -1, 146,  -1,  -1, 147, 148,  -1, 149, 150, 151,
    152, 153,  -1,  -1, 154,  -1,  -1, 155, 156,  -1,
    157,  -1, 158, 159,  -1,  -1,  -1,  -1, 160,  -1,
    161, 162, 163, 164, 165, 166, 167, 168, 169,  -1,
     -1, 170, 171,  -1,  -1, 172, 173, 174,  -1, 175,
     -1,  -1,  -1,  -1, 176,  -1,  -1,  -1,  -1, 177,
     -1, 178, 179,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 180,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1, 181, 182,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 183,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 184,
     -1, 185,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
     -1,  -1, 186
  };

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct kw_entry *
in_word_set (str, len)
     register const char *str;
     register unsigned int len;
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
        }
    }
  return 0;
}
