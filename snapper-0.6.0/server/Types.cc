/*
 * Copyright (c) [2012-2013] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#include "Types.h"


namespace DBus
{
    const char* TypeInfo<ConfigInfo>::signature = "(ssa{ss})";
    const char* TypeInfo<Snapshot>::signature = "(uquxussa{ss})";
    const char* TypeInfo<File>::signature = "(su)";
    const char* TypeInfo<QuotaData>::signature = "(tt)";


    Hoho&
    operator<<(Hoho& hoho, const ConfigInfo& data)
    {
	hoho.open_struct();
	hoho << data.getConfigName() << data.getSubvolume() << data.getAllValues();
	hoho.close_struct();
	return hoho;
    }


    Hihi&
    operator>>(Hihi& hihi, SnapshotType& data)
    {
	dbus_uint16_t tmp;
	hihi >> tmp;
	data = static_cast<SnapshotType>(tmp);
	return hihi;
    }


    Hoho&
    operator<<(Hoho& hoho, SnapshotType data)
    {
	hoho << static_cast<dbus_uint16_t>(data);
	return hoho;
    }


    Hoho&
    operator<<(Hoho& hoho, const Snapshot& data)
    {
	hoho.open_struct();
	hoho << data.getNum() << data.getType() << data.getPreNum() << data.getDate()
	     << data.getUid() << data.getDescription() << data.getCleanup()
	     << data.getUserdata();
	hoho.close_struct();
	return hoho;
    }


    Hoho&
    operator<<(Hoho& hoho, const Snapshots& data)
    {
	hoho.open_array(TypeInfo<Snapshot>::signature);
	for (Snapshots::const_iterator it = data.begin(); it != data.end(); ++it)
	    hoho << *it;
	hoho.close_array();
	return hoho;
    }


    Hoho&
    operator<<(Hoho& hoho, const File& data)
    {
	hoho.open_struct();
	hoho << data.getName() << data.getPreToPostStatus();
	hoho.close_struct();
	return hoho;
    }


    Hoho&
    operator<<(Hoho& hoho, const QuotaData& data)
    {
	hoho.open_struct();
	hoho << data.size << data.used;
	hoho.close_struct();
	return hoho;
    }


    Hoho&
    operator<<(Hoho& hoho, const Files& data)
    {
	hoho.open_array(TypeInfo<File>::signature);
	for (Files::const_iterator it = data.begin(); it != data.end(); ++it)
	    hoho << *it;
	hoho.close_array();
	return hoho;
    }

}
