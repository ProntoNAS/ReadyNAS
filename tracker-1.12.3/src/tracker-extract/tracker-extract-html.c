/*
 * Copyright (C) 2007, Jason Kivlighn <jkivlighn@gmail.com>
 * Copyright (C) 2008-2009, Nokia <ivan.frade@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "config.h"

#include <string.h>

#include <glib.h>

#include <libxml/HTMLparser.h>
#include <libtracker-common/tracker-utils.h>
#include <libtracker-extract/tracker-extract.h>

#include "tracker-main.h"

typedef enum {
	READ_TITLE,
	READ_IGNORE
} tag_type;

typedef struct {
	TrackerResource *metadata;
	tag_type current;
	guint in_body : 1;
	guint has_license : 1;
	guint has_description : 1;
	GString *title;
	GString *plain_text;
	guint n_bytes_remaining;
} parser_data;

static gboolean
has_attribute (const gchar **attrs,
               const gchar  *attr,
               const gchar  *val)
{
	gint i;

	if (!attrs || !attr || !val) {
		return FALSE;
	}

	for (i = 0; attrs[i] && attrs[i + 1]; i += 2) {
		if (g_ascii_strcasecmp (attrs[i], attr) == 0) {
			if (g_ascii_strcasecmp (attrs[i + 1], val) == 0) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

static const xmlChar *
lookup_attribute (const gchar **attrs,
                  const gchar  *attr)
{
	gint i;

	if (!attrs || !attr) {
		return NULL;
	}

	for (i = 0; attrs[i] && attrs[i + 1]; i += 2) {
		if (g_ascii_strcasecmp (attrs[i], attr) == 0) {
			return attrs[i + 1];
		}
	}

	return NULL;
}

static void
parser_start_element (void           *data,
                      const xmlChar  *name_,
                      const xmlChar **attrs_)
{
	parser_data *pd = data;
	const gchar *name = (const gchar*) name_;
	const gchar **attrs = (const gchar**) attrs_;

	if (!pd || !name) {
		return;
	}

	/* Look for RDFa triple describing the license */
	if (g_ascii_strcasecmp (name, "a") == 0) {
		/* This tag is a license.  Ignore, however, if it is
		 * referring to another document.
		 */
		if (has_attribute (attrs, "rel", "license") &&
		    has_attribute (attrs, "about", NULL) == FALSE) {
			const xmlChar *href;

			href = lookup_attribute (attrs, "href");

			if (href && !pd->has_license) {
				tracker_resource_add_string (pd->metadata, "nie:license", href);
				pd->has_license = TRUE;
			}
		}
	} else if (g_ascii_strcasecmp (name, "title") == 0) {
		pd->current = READ_TITLE;
	} else if (g_ascii_strcasecmp (name, "meta") == 0) {
		if (has_attribute (attrs, "name", "author")) {
			const xmlChar *author;

			author = lookup_attribute (attrs, "content");

			if (author) {
				TrackerResource *creator = tracker_extract_new_contact (author);

				tracker_resource_add_relation (pd->metadata, "nco:creator", creator);

				g_object_unref (creator);
			}
		}

		if (has_attribute (attrs, "name", "description")) {
			const xmlChar *desc;

			desc = lookup_attribute (attrs,"content");

			if (desc && !pd->has_description) {
				tracker_resource_set_string (pd->metadata, "nie:description", desc);
				pd->has_description = TRUE;
			}
		}

		if (has_attribute (attrs, "name", "keywords")) {
			const xmlChar* content = lookup_attribute (attrs, "content");

			if (content) {
				gchar **keywords;
				gint i;

				keywords = g_strsplit (content, ",", -1);
				if (keywords) {
					for (i = 0; keywords[i] != NULL; i++) {
						if (!keywords[i] || keywords[i] == '\0') {
							continue;
						}

						tracker_resource_add_string (pd->metadata, "nie:keyword", g_strstrip (keywords[i]));
					}

					g_strfreev (keywords);
				}
			}
		}
	} else if (g_ascii_strcasecmp (name, "body") == 0) {
		pd->in_body = TRUE;
	} else if (g_ascii_strcasecmp (name, "script") == 0) {
		/* Ignore javascript and such */
		pd->current = READ_IGNORE;
	}
}

static void
parser_end_element (void           *data,
                    const xmlChar  *name_)
{
	parser_data *pd = data;
	const gchar *name = (const gchar*) name_;

        if (g_ascii_strcasecmp (name, "title") == 0 ||
            g_ascii_strcasecmp (name, "script") == 0) {
                    pd->current = -1;
        }
}

static void
parser_characters (void          *data,
                   const xmlChar *ch,
                   int            len)
{
	parser_data *pd = data;

	switch (pd->current) {
	case READ_TITLE:
		g_string_append (pd->title, ch);
		break;
	case READ_IGNORE:
		break;
	default:
		if (pd->in_body && pd->n_bytes_remaining > 0) {
			gsize text_len;

			text_len = strlen (ch);

			if (tracker_text_validate_utf8 (ch,
			                                (pd->n_bytes_remaining < text_len ?
			                                 pd->n_bytes_remaining :
			                                 text_len),
			                                &pd->plain_text,
			                                NULL)) {
				/* In the case of HTML, each string arriving this
				 * callback is independent to any other previous
				 * string, so need to add an explicit whitespace
				 * separator */
				g_string_append_c (pd->plain_text, ' ');
			}

			if (pd->n_bytes_remaining > text_len) {
				pd->n_bytes_remaining -= text_len;
			} else {
				pd->n_bytes_remaining = 0;
			}
		}
		break;
	}
}

G_MODULE_EXPORT gboolean
tracker_extract_get_metadata (TrackerExtractInfo *info)
{
	TrackerResource *metadata;
	GFile *file;
	TrackerConfig *config;
	htmlDocPtr doc;
	parser_data pd;
	gchar *filename;
	xmlSAXHandler handler = {
		NULL, /* internalSubset */
		NULL, /* isStandalone */
		NULL, /* hasInternalSubset */
		NULL, /* hasExternalSubset */
		NULL, /* resolveEntity */
		NULL, /* getEntity */
		NULL, /* entityDecl */
		NULL, /* notationDecl */
		NULL, /* attributeDecl */
		NULL, /* elementDecl */
		NULL, /* unparsedEntityDecl */
		NULL, /* setDocumentLocator */
		NULL, /* startDocument */
		NULL, /* endDocument */
		parser_start_element, /* startElement */
		parser_end_element, /* endElement */
		NULL, /* reference */
		parser_characters, /* characters */
		NULL, /* ignorableWhitespace */
		NULL, /* processingInstruction */
		NULL, /* comment */
		NULL, /* xmlParserWarning */
		NULL, /* xmlParserError */
		NULL, /* xmlParserError */
		NULL, /* getParameterEntity */
		NULL, /* cdataBlock */
		NULL, /* externalSubset */
		1,    /* initialized */
		NULL, /* private */
		NULL, /* startElementNsSAX2Func */
		NULL, /* endElementNsSAX2Func */
		NULL  /* xmlStructuredErrorFunc */
	};

	file = tracker_extract_info_get_file (info);

	metadata = tracker_resource_new (NULL);
	tracker_resource_add_uri (metadata, "rdf:type", "nfo:HtmlDocument");

	pd.metadata = metadata;
	pd.current = -1;
	pd.in_body = FALSE;
	pd.plain_text = g_string_new (NULL);
	pd.title = g_string_new (NULL);

	config = tracker_main_get_config ();
	pd.n_bytes_remaining = tracker_config_get_max_bytes (config);

	filename = g_file_get_path (file);
	doc = htmlSAXParseFile (filename, NULL, &handler, &pd);
	g_free (filename);

	if (doc) {
		xmlFreeDoc (doc);
	}

	g_strstrip (pd.plain_text->str);
	g_strstrip (pd.title->str);

	if (pd.title->str &&
	    *pd.title->str != '\0') {
		tracker_resource_set_string (metadata, "nie:title", pd.title->str);
	}

	if (pd.plain_text->str &&
	    *pd.plain_text->str != '\0') {
		tracker_resource_set_string (metadata, "nie:plainTextContent", pd.plain_text->str);
	}

	g_string_free (pd.plain_text, TRUE);
	g_string_free (pd.title, TRUE);

	tracker_extract_info_set_resource (info, metadata);
	g_object_unref (metadata);

	return TRUE;
}
