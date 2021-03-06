/*
 * Copyright (C) 2011 Andreas Steffen, HSR Hochschule fuer Technik Rapperswil
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

#include "imcv.h"
#include "ietf/ietf_attr.h"
#include "ita/ita_attr.h"

#include <utils/debug.h>
#include <utils/utils.h>
#include <pen/pen.h>

#include <syslog.h>

#define IMCV_DEBUG_LEVEL			1
#define IMCV_DEFAULT_POLICY_SCRIPT	"ipsec _imv_policy"


/**
 * PA-TNC attribute manager
 */
pa_tnc_attr_manager_t *imcv_pa_tnc_attributes;

/**
 * Global IMV database
 */
imv_database_t *imcv_db;

/**
 * Reference count for libimcv
 */
static refcount_t libimcv_ref = 0;

/**
 * Reference count for libstrongswan
 */
static refcount_t libstrongswan_ref = 0;

/**
 * Global configuration of imcv dbg function
 */
static int  imcv_debug_level;
static bool imcv_stderr_quiet;

/**
 * imvc dbg function
 */
static void imcv_dbg(debug_t group, level_t level, char *fmt, ...)
{
	int priority = LOG_INFO;
	char buffer[8192];
	char *current = buffer, *next;
	va_list args;

	if (level <= imcv_debug_level)
	{
		if (!imcv_stderr_quiet)
		{
			va_start(args, fmt);
			fprintf(stderr, "[HSR] ");
			vfprintf(stderr, fmt, args);
			fprintf(stderr, "\n");
			va_end(args);
		}

		/* write in memory buffer first */
		va_start(args, fmt);
		vsnprintf(buffer, sizeof(buffer), fmt, args);
		va_end(args);

		/* do a syslog with every line */
		while (current)
		{
			next = strchr(current, '\n');
			if (next)
			{
				*(next++) = '\0';
			}
			syslog(priority, "[HSR] %s\n", current);
			current = next;
		}
	}
}

/**
 * Described in header.
 */
bool libimcv_init(bool is_imv)
{
	/* initialize libstrongswan library only once */
	if (lib)
	{
		/* did main program initialize libstrongswan? */
		if (libstrongswan_ref == 0)
		{
			ref_get(&libstrongswan_ref);
		}
	}
	else
	{
		/* we are the first to initialize libstrongswan */
		if (!library_init(NULL, "libimcv"))
		{
			return FALSE;
		}

		/* set the debug level and stderr output */
		imcv_debug_level =  lib->settings->get_int(lib->settings,
									"libimcv.debug_level", IMCV_DEBUG_LEVEL);
		imcv_stderr_quiet = lib->settings->get_int(lib->settings,
									"libimcv.stderr_quiet", FALSE);

		/* activate the imcv debugging hook */
		dbg = imcv_dbg;
		openlog("imcv", 0, LOG_DAEMON);

		if (!lib->plugins->load(lib->plugins,
				lib->settings->get_str(lib->settings, "libimcv.load",
					"random nonce gmp pubkey x509")))
		{
			library_deinit();
			return FALSE;
		}
	}
	ref_get(&libstrongswan_ref);

	lib->settings->add_fallback(lib->settings, "%s.imcv", "libimcv", lib->ns);
	lib->settings->add_fallback(lib->settings, "%s.plugins", "libimcv.plugins",
								lib->ns);

	if (libimcv_ref == 0)
	{
		char *uri, *script;

		/* initialize the PA-TNC attribute manager */
	 	imcv_pa_tnc_attributes = pa_tnc_attr_manager_create();
		imcv_pa_tnc_attributes->add_vendor(imcv_pa_tnc_attributes, PEN_IETF,
							ietf_attr_create_from_data, ietf_attr_names);
		imcv_pa_tnc_attributes->add_vendor(imcv_pa_tnc_attributes, PEN_ITA,
							ita_attr_create_from_data, ita_attr_names);

		/* attach global IMV database */
		if (is_imv)
		{
			uri = lib->settings->get_str(lib->settings,
						"%s.imcv.database", NULL, lib->ns);
			script = lib->settings->get_str(lib->settings,
						"%s.imcv.policy_script", IMCV_DEFAULT_POLICY_SCRIPT,
						lib->ns);
			if (uri)
			{
				imcv_db = imv_database_create(uri, script);
			}
		}
		DBG1(DBG_LIB, "libimcv initialized");
	}
	ref_get(&libimcv_ref);

	return TRUE;
}

/**
 * Described in header.
 */
void libimcv_deinit(void)
{
	if (ref_put(&libimcv_ref))
	{
		imcv_pa_tnc_attributes->remove_vendor(imcv_pa_tnc_attributes, PEN_IETF);
		imcv_pa_tnc_attributes->remove_vendor(imcv_pa_tnc_attributes, PEN_ITA);
		DESTROY_IF(imcv_pa_tnc_attributes);
		imcv_pa_tnc_attributes = NULL;
		DESTROY_IF(imcv_db);
		DBG1(DBG_LIB, "libimcv terminated");
	}
	if (ref_put(&libstrongswan_ref))
	{
		library_deinit();
	}
}

