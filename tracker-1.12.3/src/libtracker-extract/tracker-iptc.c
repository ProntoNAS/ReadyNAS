/*
 * Copyright (C) 2009, Nokia <ivan.frade@nokia.com>
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

#include "tracker-iptc.h"
#include "tracker-utils.h"

#ifdef HAVE_LIBIPTCDATA

#include <libiptcdata/iptc-data.h>
#include <libiptcdata/iptc-dataset.h>

#define IPTC_DATE_FORMAT "%Y %m %d"

/**
 * SECTION:tracker-iptc
 * @title: IPTC
 * @short_description: Information Interchange Model (IIM) /
 * International Press Telecommunications Council (IPTC)
 * @stability: Stable
 * @include: libtracker-extract/tracker-extract.h
 *
 * The Information Interchange Model (IIM) is a file structure and set
 * of metadata attributes that can be applied to text, images and
 * other media types. It was developed in the early 1990s by the
 * International Press Telecommunications Council (IPTC) to expedite
 * the international exchange of news among newspapers and news
 * agencies.
 *
 * The full IIM specification includes a complex data structure and a
 * set of metadata definitions.
 *
 * Although IIM was intended for use with all types of news items —
 * including simple text articles — a subset found broad worldwide
 * acceptance as the standard embedded metadata used by news and
 * commercial photographers. Information such as the name of the
 * photographer, copyright information and the caption or other
 * description can be embedded either manually or automatically.
 *
 * IIM metadata embedded in images are often referred to as "IPTC
 * headers," and can be easily encoded and decoded by most popular
 * photo editing software.
 *
 * The Extensible Metadata Platform (XMP) has largely superseded IIM's
 * file structure, but the IIM image attributes are defined in the
 * IPTC Core schema for XMP and most image manipulation programs keep
 * the XMP and non-XMP IPTC attributes synchronized.
 *
 * This API is provided to remove code duplication between extractors
 * using these standards.
 **/

static const gchar *
fix_iptc_orientation (const gchar *orientation)
{
	if (g_strcmp0 (orientation, "P") == 0) {
		return "nfo:orientation-left";
	}

	return "nfo:orientation-top"; /* We take this as default */
}

static void
foreach_dataset (IptcDataSet *dataset,
                 void        *user_data)
{
	TrackerIptcData *data = user_data;
	gchar mbuffer[1024];

	/* The meaning of dataset->tag DEPENDS on the value of dataset->record.
	 * See iptc-tag.h for the relationship.
	 *
	 * Now, We only want record-2 tags, otherwise we'll end up mixing
	 * for example IPTC_TAG_CITY and IPTC_TAG_CHARACTER_SET, which BOTH
	 * have a value of 90. */
	if (dataset->record != IPTC_RECORD_APP_2)
		return;

	switch (dataset->tag) {
	case IPTC_TAG_KEYWORDS:
		if (!data->keywords) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->keywords = g_strdup (mbuffer);
		} else {
			gchar *tmp = data->keywords;
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->keywords = g_strdup_printf ("%s, %s", data->keywords, mbuffer);
			g_free (tmp);
		}
		break;

	case IPTC_TAG_DATE_CREATED:
		if (!data->date_created) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			/* From: ex; date "2007 04 15"
			 * To : ex. "2007-04-15T00:00:00+0200 where +0200 is offset w.r.t gmt */
			data->date_created = tracker_date_format_to_iso8601 (mbuffer, IPTC_DATE_FORMAT);
		}
		break;

	case IPTC_TAG_BYLINE:
		if (!data->byline) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->byline = g_strdup (mbuffer);
		}
		break;

	case IPTC_TAG_CREDIT:
		if (!data->credit) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->credit = g_strdup (mbuffer);
		}
		break;

	case IPTC_TAG_COPYRIGHT_NOTICE:
		if (!data->copyright_notice) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->copyright_notice = g_strdup (mbuffer);
		}
		break;

	case IPTC_TAG_IMAGE_ORIENTATION:
		if (!data->image_orientation) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->image_orientation = g_strdup (fix_iptc_orientation (mbuffer));
		}
		break;

	case IPTC_TAG_BYLINE_TITLE:
		if (!data->byline_title) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->byline_title = g_strdup (mbuffer);
		}
		break;

	case IPTC_TAG_CITY:
		if (!data->city) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->city = g_strdup (mbuffer);
		}
		break;

	case IPTC_TAG_STATE:
		if (!data->state) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->state = g_strdup (mbuffer);
		}
		break;

	case IPTC_TAG_SUBLOCATION:
		if (!data->sublocation) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->sublocation = g_strdup (mbuffer);
		}
		break;

	case IPTC_TAG_COUNTRY_NAME:
		if (!data->country_name) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->country_name = g_strdup (mbuffer);
		}
		break;

	case IPTC_TAG_CONTACT:
		if (!data->contact) {
			iptc_dataset_get_as_str (dataset, mbuffer, 1024);
			data->contact = g_strdup (mbuffer);
		}
		break;

	default:
		break;
	}
}

#endif /* HAVE_LIBIPTCDATA */

static gboolean
parse_iptc (const unsigned char *buffer,
            size_t               len,
            const gchar         *uri,
            TrackerIptcData     *data)
{
#ifdef HAVE_LIBIPTCDATA
	IptcData *iptc;
#endif /* HAVE_LIBIPTCDATA */

	memset (data, 0, sizeof (TrackerIptcData));

#ifdef HAVE_LIBIPTCDATA

	/* FIXME According to valgrind this is leaking (together with the unref).
	 * Problem in libiptc (I replaced this with the _free equivalent) */

	iptc = iptc_data_new ();

	if (!iptc)
		return FALSE;

	if (iptc_data_load (iptc, buffer, len) < 0) {
		iptc_data_free (iptc);
		return FALSE;
	}

	iptc_data_foreach_dataset (iptc, foreach_dataset, data);
	iptc_data_free (iptc);
#endif /* HAVE_LIBIPTCDATA */

	return TRUE;
}

#ifndef TRACKER_DISABLE_DEPRECATED

// LCOV_EXCL_START

/**
 * tracker_iptc_read:
 * @buffer: a chunk of data with iptc data in it.
 * @len: the size of @buffer.
 * @uri: the URI this is related to.
 * @data: a pointer to a TrackerIptcData struture to populate.
 *
 * This function takes @len bytes of @buffer and runs it through the
 * IPTC library. The result is that @data is populated with the IPTC
 * data found in @uri.
 *
 * Returns: %TRUE if the @data was populated successfully, otherwise
 * %FALSE is returned.
 *
 * Since: 0.8
 *
 * Deprecated: 0.9. Use tracker_iptc_new() instead.
 **/
gboolean
tracker_iptc_read (const unsigned char *buffer,
                   size_t               len,
                   const gchar         *uri,
                   TrackerIptcData     *data)
{
	g_return_val_if_fail (buffer != NULL, FALSE);
	g_return_val_if_fail (len > 0, FALSE);
	g_return_val_if_fail (uri != NULL, FALSE);
	g_return_val_if_fail (data != NULL, FALSE);

	return parse_iptc (buffer, len, uri, data);
}

// LCOV_EXCL_STOP

#endif /* TRACKER_DISABLE_DEPRECATED */

/**
 * tracker_iptc_new:
 * @buffer: a chunk of data with iptc data in it.
 * @len: the size of @buffer.
 * @uri: the URI this is related to.
 *
 * This function takes @len bytes of @buffer and runs it through the
 * IPTC library.
 *
 * Returns: a newly allocated #TrackerIptcData struct if IPTC data was
 * found, %NULL otherwise. Free the returned struct with
 * tracker_iptc_free().
 *
 * Since: 0.10
 **/
TrackerIptcData *
tracker_iptc_new (const guchar *buffer,
                  gsize         len,
                  const gchar  *uri)
{
	TrackerIptcData *data;

	g_return_val_if_fail (buffer != NULL, NULL);
	g_return_val_if_fail (len > 0, NULL);
	g_return_val_if_fail (uri != NULL, NULL);

	data = g_new0 (TrackerIptcData, 1);

	if (!parse_iptc (buffer, len, uri, data)) {
		tracker_iptc_free (data);
		return NULL;
	}

	return data;
}

/**
 * tracker_iptc_free:
 * @data: a #TrackerIptcData
 *
 * Frees @data and all #TrackerIptcData members. %NULL will produce a
 * a warning.
 *
 * Since: 0.10
 **/
void
tracker_iptc_free (TrackerIptcData *data)
{
	g_return_if_fail (data != NULL);

	g_free (data->keywords);
	g_free (data->date_created);
	g_free (data->byline);
	g_free (data->credit);
	g_free (data->copyright_notice);
	g_free (data->image_orientation);
	g_free (data->byline_title);
	g_free (data->city);
	g_free (data->state);
	g_free (data->sublocation);
	g_free (data->country_name);
	g_free (data->contact);

	g_free (data);
}
