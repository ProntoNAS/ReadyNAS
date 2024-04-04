/*
 * Copyright (C) 2008, Nokia <ivan.frade@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

using Tracker;
using Tracker.Sparql;

const int max_signals = 1000;
// const int max_signals = 10000;
const string title_data = "title";

// Always start this test AFTER DOING tracker-control -r. The test IS NOT
// deleting existing resources, so you CAN'T RUN IT TWICE unless you clear
// the database before starting it the second time.

// Testreport of Aug 25, 2010 by Philip
// ------------------------------------
// On Aug 25 the difference between using tracker-store on master and the
// tracker-store of class-signal, and then letting this wait until all 10000
// (in case of max_signals = 10000) insert queries' signals arrived (you'll
// have in total 20002 events in the signals in both tracker-store versions)
// was: 20s for class-signals (new class signal) and 23s for master (old class
// signals). Measured using this performance test.
//
// Memory usage of class-signal (new class signal)'s tracker-store:
// Low: VmRSS: 8860 Kb -- Max: VmRSS: 14116 kB
//
// Memory usage of master (old class signal)'s tracker-store:
// Low: VmRSS: 8868 Kb -- Max: VmRSS: 14060 kB


public struct Event {
	int graph_id;
	int subject_id;
	int pred_id;
	int object_id;
}

[DBus (name = "org.freedesktop.Tracker1.Resources")]
private interface Resources : DBusProxy {
	[DBus (name = "GraphUpdated")]
	public signal void graph_updated (string class_name, Event[] deletes, Event[] inserts);

	[DBus (name = "SparqlUpdate")]
	public abstract async void sparql_update_async (string query) throws Sparql.Error, DBusError;
}

[DBus (name = "org.freedesktop.Tracker1.Resources.Class")]
private interface ResourcesClass : DBusProxy {
	[DBus (name = "SubjectsAdded")]
	public signal void subjects_added (string [] subjects);
	[DBus (name = "SubjectsChanged")]
	public signal void subjects_changed (string [] subjects, string [] preds);
}

public class TestApp {
	static Resources resources_object;
	static ResourcesClass class_object;
	MainLoop loop;
	bool initialized = false;
	Sparql.Connection con;
	int count = 0;
	GLib.Timer t;

	public TestApp ()
	requires (!initialized) {
		try {
			con = Tracker.Sparql.Connection.get();

			resources_object = GLib.Bus.get_proxy_sync (BusType.SESSION,
			                                            "org.freedesktop.Tracker1",
			                                            "/org/freedesktop/Tracker1/Resources",
			                                            DBusProxyFlags.DO_NOT_LOAD_PROPERTIES | DBusProxyFlags.DO_NOT_CONNECT_SIGNALS);

			class_object = GLib.Bus.get_proxy_sync (BusType.SESSION,
			                                        "org.freedesktop.Tracker1",
			                                        "/org/freedesktop/Tracker1/Resources/Classes/nmm/MusicPiece",
			                                        DBusProxyFlags.DO_NOT_LOAD_PROPERTIES | DBusProxyFlags.DO_NOT_CONNECT_SIGNALS);

			class_object.subjects_added.connect (on_subjects_added);
			class_object.subjects_changed.connect (on_subjects_changed);

			resources_object.graph_updated.connect (on_graph_updated_received);
			t = new GLib.Timer ();
			
		} catch (GLib.Error e) {
			warning ("Could not connect to D-Bus service: %s", e.message);
			initialized = false;
			return;
		}
		initialized = true;
	}

	private void on_subjects_changed (string [] subjects, string [] preds) {
		foreach (string s in subjects)
			count++;

		//if (count == 20002)
			print ("Old class signal count=%d time=%lf\n", count, t.elapsed ());
	}

	private void on_subjects_added (string [] subjects) {
		foreach (string s in subjects)
			count++;

		//if (count == 20002)
			print ("Old class signal count=%d time=%lf\n", count, t.elapsed ());
	}

	private void on_graph_updated_received (string class_name, Event[] deletes, Event[] inserts) {
		foreach (Event insert in inserts)
			count++;
		//if (count == 20002)
			print ("New class signal count=%d time=%lf\n", count, t.elapsed ());
	}

	private void insert_data () {
		int i;

		t.start();
		for (i = 0; i <= max_signals; i++) {
			string upqry = "INSERT { <%d> a nmm:MusicPiece ; nie:title '%s %d' }".printf(i, title_data, i);
			resources_object.sparql_update_async (upqry);
		}
	}

	private bool in_mainloop () {
		insert_data ();
		return false;
	}

	public int run () {
		loop = new MainLoop (null, false);
		Idle.add (in_mainloop);
		loop.run ();
		return 0;
	}
}

int main (string[] args) {
	TestApp app = new TestApp ();

	return app.run ();
}
