/*
 * oauth2.c
 */
#define _GNU_SOURCE

#include "oauth2.h"

#include <string.h>

#include <systemd/sd-bus.h>

#include "xalloc.h"

#include "gettext.h"
#define _(string) gettext(string)

#define OAUTH2_VAULT_SERVICE                 "com.readynas.OAuth2Vault"
#define OAUTH2_VAULT_INTERFACE               "com.readynas.OAuth2Vault.Email"
#define OAUTH2_VAULT_INTERFACE_PATH          "/"

char *oauth2_access_token(const char *pass, char **errstr)
{
	if (!pass || !errstr)
		return NULL;

	sd_bus_error sd_error = SD_BUS_ERROR_NULL;
	sd_bus_message *message = NULL;
	sd_bus *bus = NULL;
	char *access_token = NULL;
	*errstr = NULL;
	int r = sd_bus_open_system(&bus);
	if (r < 0)
	{
		*errstr = xasprintf(_("Failed to connect to system bus: %s"), strerror(-r));
		return NULL;
	}

	r = sd_bus_call_method(bus,
		OAUTH2_VAULT_SERVICE,
		OAUTH2_VAULT_INTERFACE_PATH,
		OAUTH2_VAULT_INTERFACE,
		"GetAccessToken",
		&sd_error, &message, "s", pass);
	if (r < 0)
	{
		*errstr = xasprintf(_("Failed to issue method call: %s"), sd_error.message);
		goto finish;
	}

	char *p = NULL;
	r = sd_bus_message_read(message, "s", &p);
	if (r < 0)
	{
		*errstr = xasprintf(_("Failed to parse respose message: %s"), strerror(-r));
		goto finish;
	}

	if (p)
	{
		access_token = strdup(p);
	}

finish:
	sd_bus_error_free(&sd_error);
	sd_bus_message_unref(message);
	sd_bus_flush_close_unref(bus);

	return access_token;
}
