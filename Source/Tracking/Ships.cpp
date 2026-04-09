/*
	Copyright(c) 2021-2026 jvde.github@gmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <vector>
#include <fstream>

#include "AIS-catcher.h"
#include "Ships.h"
#include "StringBuilder.h"

void Ship::reset()
{

	path_ptr = -1;

	mmsi = count = msg_type = shiptype = group_mask = 0;
	flags.reset();

	heading = HEADING_UNDEFINED;
	status = STATUS_UNDEFINED;
	to_port = to_bow = to_starboard = to_stern = DIMENSION_UNDEFINED;
	IMO = IMO_UNDEFINED;
	angle = ANGLE_UNDEFINED;
	month = ETA_MONTH_UNDEFINED;
	day = ETA_DAY_UNDEFINED;
	hour = ETA_HOUR_UNDEFINED;
	minute = ETA_MINUTE_UNDEFINED;
	lat = LAT_UNDEFINED;
	lon = LON_UNDEFINED;
	ppm = PPM_UNDEFINED;
	level = LEVEL_UNDEFINED;
	altitude = ALT_UNDEFINED;
	received_stations = RECEIVED_STATIONS_UNDEFINED;

	distance = DISTANCE_UNDEFINED;
	angle = ANGLE_UNDEFINED;
	draught = DRAUGHT_UNDEFINED;
	speed = SPEED_UNDEFINED;

	cog = COG_UNDEFINED;
	last_signal = last_direct_signal = last_static_signal = {};
	shipclass = CLASS_UNKNOWN;
	mmsi_type = MMSI_OTHER;

	memset(shipname, 0, sizeof(shipname));
	memset(destination, 0, sizeof(destination));
	memset(callsign, 0, sizeof(callsign));
	memset(country_code, 0, sizeof(country_code));
	last_group = GROUP_OUT_UNDEFINED;

	msg.clear();
}

std::string getSprite(const Ship *ship)
{
	std::string shipofs = (ship->speed != SPEED_UNDEFINED && ship->speed > 0.5) ? "<y>88</y><w>20</w><h>20</h>" : "<y>68</y><w>20</w><h>20</h>";
	std::string stationofs = "<y>50</y><w>20</w><h>20</h>";

	switch (ship->shipclass)
	{
	case CLASS_OTHER:
		return "<x>120</x>" + shipofs;
	case CLASS_UNKNOWN:
		return "<x>120</x>" + shipofs;
	case CLASS_CARGO:
		return "<x>0</x>" + shipofs;
	case CLASS_TANKER:
		return "<x>80</x>" + shipofs;
	case CLASS_PASSENGER:
		return "<x>40</x>" + shipofs;
	case CLASS_HIGHSPEED:
		return "<x>100</x>" + shipofs;
	case CLASS_SPECIAL:
		return "<x>60</x>" + shipofs;
	case CLASS_FISHING:
		return "<x>140</x>" + shipofs;
	case CLASS_B:
		return "<x>140</x>" + shipofs;
	case CLASS_ATON:
		return "<x>0</x>" + stationofs;
	case CLASS_STATION:
		return "<x>20</x>" + stationofs;
	case CLASS_SARTEPIRB:
		return "<x>40</x>" + stationofs;
	case CLASS_HELICOPTER:
		return "<w>25</w><h>25</h>";
	case CLASS_PLANE:
		return "<y>25</y><w>25</w><h>25</h>";
	}
	return "";
}

bool Ship::getKML(std::string &kmlString) const
{
	if (lat == LAT_UNDEFINED || lon == LON_UNDEFINED || (lat == 0 && lon == 0))
		return false;

	std::string shipNameStr(shipname);

	const std::string name = !shipNameStr.empty() ? shipNameStr : std::to_string(mmsi);
	const std::string styleId = "style" + std::to_string(mmsi);
	const std::string coordinates = std::to_string(lon) + "," + std::to_string(lat) + ",0";

	kmlString += "<Style id=\"" + styleId + "\"><IconStyle><scale>1</scale><heading>" +
				 std::to_string(cog) + "</heading><Icon><href>/icons.png</href>" +
				 getSprite(this) + "</Icon></IconStyle></Style><Placemark><name>" +
				 name + "</name><description>Description of your placemark</description><styleUrl>#" +
				 styleId + "</styleUrl><Point><coordinates>" +
				 coordinates + "</coordinates></Point></Placemark>";
	return true;
}

bool Ship::getGeoJSON(JSON::Writer &w) const
{
	w.beginObject();
	w.kv("type", "Feature");
	w.key("properties");
	w.beginObject();

	w.kv("mmsi", mmsi);

	w.kv("distance", distance)
	 .kv("bearing", angle)
	 .kv_unless("level", level, LEVEL_UNDEFINED)
	 .kv("count", count)
	 .kv_unless("ppm", ppm, PPM_UNDEFINED)
	 .kv("group_mask", group_mask)
	 .kv("approx", (bool)getApproximate())
	 .kv_unless("heading", heading, HEADING_UNDEFINED)
	 .kv_unless("cog", cog, COG_UNDEFINED)
	 .kv_unless("speed", speed, SPEED_UNDEFINED)
	 .kv_unless("to_bow", to_bow, DIMENSION_UNDEFINED)
	 .kv_unless("to_stern", to_stern, DIMENSION_UNDEFINED)
	 .kv_unless("to_starboard", to_starboard, DIMENSION_UNDEFINED)
	 .kv_unless("to_port", to_port, DIMENSION_UNDEFINED)
	 .kv("shiptype", shiptype)
	 .kv("mmsi_type", mmsi_type)
	 .kv("shipclass", shipclass)
	 .kv("validated", getValidated())
	 .kv("msg_type", msg_type)
	 .kv("channels", getChannels())
	 .kv("country", country_code)
	 .kv("status", status)
	 .kv("draught", draught)
	 .kv_unless("eta_month", (int)month, ETA_MONTH_UNDEFINED)
	 .kv_unless("eta_day", (int)day, ETA_DAY_UNDEFINED)
	 .kv_unless("eta_hour", (int)hour, ETA_HOUR_UNDEFINED)
	 .kv_unless("eta_minute", (int)minute, ETA_MINUTE_UNDEFINED)
	 .kv_unless("imo", IMO, IMO_UNDEFINED)
	 .kv("callsign", callsign);

	if (getVirtualAid())
		w.kv("shipname", shipname, " [V]", 4);
	else
		w.kv("shipname", shipname);

	w.kv("destination", destination)
	 .kv("last_signal", last_signal)
	 .endObject() // properties
	 .key("geometry")
	 .beginObject()
	 .kv("type", "Point")
	 .key("coordinates")
	 .beginArray()
	 .val(lon)
	 .val(lat)
	 .endArray()
	 .endObject()  // geometry
	 .endObject(); // feature

	return true;
}

int Ship::getMMSItype()
{
	if ((mmsi > 111000000 && mmsi < 111999999) || (mmsi > 11100000 && mmsi < 11199999))
	{
		return MMSI_SAR;
	}
	if (mmsi >= 970000000 && mmsi <= 980000000)
	{
		return MMSI_SARTEPIRB;
	}
	if (msg_type & ATON_MASK || (mmsi >= 990000000 && mmsi <= 999999999))
	{
		return MMSI_ATON;
	}
	if (msg_type & CLASS_A_MASK)
	{
		return MMSI_CLASS_A;
	}
	if (msg_type & CLASS_B_MASK)
	{
		return MMSI_CLASS_B;
	}
	if (msg_type & BASESTATION_MASK || (mmsi < 9000000))
	{
		return MMSI_BASESTATION;
	}
	if (msg_type & SAR_MASK)
	{
		return MMSI_SAR;
	}
	if (msg_type & CLASS_A_STATIC_MASK)
	{
		return MMSI_CLASS_A;
	}
	if (msg_type & CLASS_B_STATIC_MASK)
	{
		return MMSI_CLASS_B;
	}
	return MMSI_OTHER;
}

int Ship::getShipTypeClassEri()
{
	switch (shiptype)
	{
	// Cargo cases
	case 8030:
	case 8010:
	case 8070:
	case 8210:
	case 8220:
	case 8230:
	case 8240:
	case 8250:
	case 8260:
	case 8270:
	case 8280:
	case 8290:
	case 8310:
	case 8320:
	case 8330:
	case 8340:
	case 8350:
	case 8360:
	case 8370:
	case 8380:
	case 8390:
	case 8130:
	case 8140:
	case 8150:
	case 8170:
	case 8410:
		return CLASS_CARGO;
	// Tanker cases
	case 8020:
	case 8021:
	case 8022:
	case 8023:
	case 8040:
	case 8060:
	case 8160:
	case 8161:
	case 8162:
	case 8163:
	case 8180:
	case 8490:
	case 8500:
	case 1530:
	case 1540:
		return CLASS_TANKER;
	// Special cases
	case 8050:
	case 8080:
	case 8090:
	case 8100:
	case 8110:
	case 8120:
	case 8400:
	case 8420:
	case 8430:
	case 8450:
	case 8460:
	case 8470:
	case 8510:
		return CLASS_SPECIAL;
	// Passenger cases
	case 8440:
	case 8441:
	case 8442:
	case 8443:
	case 8444:
		return CLASS_PASSENGER;
	// Other cases
	case 8480:
		return CLASS_FISHING;
	case 1850:
		return CLASS_B;
	case 1900:
	case 1910:
	case 1920:
		return CLASS_HIGHSPEED;
	default:
		return CLASS_OTHER;
	}
}

int Ship::getShipTypeClass()
{
	int c = CLASS_UNKNOWN;

	switch (mmsi_type)
	{
	case MMSI_CLASS_A:
	case MMSI_CLASS_B:
		c = (mmsi_type == MMSI_CLASS_B) ? CLASS_B : CLASS_UNKNOWN;

		// Overwrite default if there's a better mapping
		if (shiptype >= 80 && shiptype < 90)
			c = CLASS_TANKER;
		else if (shiptype >= 70 && shiptype < 80)
			c = CLASS_CARGO;
		else if (shiptype >= 60 && shiptype < 70)
			c = CLASS_PASSENGER;
		else if (shiptype >= 40 && shiptype < 50)
			c = CLASS_HIGHSPEED;
		else if (shiptype >= 50 && shiptype < 60)
			c = CLASS_SPECIAL;
		else if (shiptype == 30)
			c = CLASS_FISHING;
		else if ((shiptype >= 1500 && shiptype <= 1920) || (shiptype >= 8000 && shiptype <= 8510))
		{
			c = getShipTypeClassEri();
		}
		break;
	case MMSI_BASESTATION:
		c = CLASS_STATION;
		break;
	case MMSI_SAR:
		c = CLASS_HELICOPTER;
		if ((mmsi > 111000000 && mmsi < 111999999 && (mmsi / 100) % 10 == 1) || (mmsi > 11100000 && mmsi < 11199999 && (mmsi / 10) % 10 == 1))
			c = CLASS_PLANE;
		break;
	case MMSI_SARTEPIRB:
		c = CLASS_SARTEPIRB;
		break;
	case MMSI_ATON:
		c = CLASS_ATON;
		break;
	default:
		break;
	}

	return c;
}

void Ship::setType()
{
	mmsi_type = getMMSItype();
	shipclass = getShipTypeClass();
}

bool Ship::Save(std::ofstream &file) const
{
	// Write magic number and version first
	int magic = _SHIP_MAGIC;
	int version = _SHIP_VERSION;

	if (!file.write((const char *)&magic, sizeof(magic)))
		return false;
	if (!file.write((const char *)&version, sizeof(version)))
		return false;

	// Write all ship data except the msg vector
	if (!file.write((const char *)&mmsi, sizeof(mmsi)))
		return false;
	if (!file.write((const char *)&count, sizeof(count)))
		return false;
	if (!file.write((const char *)&msg_type, sizeof(msg_type)))
		return false;
	if (!file.write((const char *)&shiptype, sizeof(shiptype)))
		return false;
	if (!file.write((const char *)&group_mask, sizeof(group_mask)))
		return false;
	if (!file.write((const char *)&flags, sizeof(flags)))
		return false;
	if (!file.write((const char *)&heading, sizeof(heading)))
		return false;
	if (!file.write((const char *)&status, sizeof(status)))
		return false;
	if (!file.write((const char *)&to_port, sizeof(to_port)))
		return false;
	if (!file.write((const char *)&to_bow, sizeof(to_bow)))
		return false;
	if (!file.write((const char *)&to_starboard, sizeof(to_starboard)))
		return false;
	if (!file.write((const char *)&to_stern, sizeof(to_stern)))
		return false;
	if (!file.write((const char *)&IMO, sizeof(IMO)))
		return false;
	if (!file.write((const char *)&angle, sizeof(angle)))
		return false;
	if (!file.write((const char *)&month, sizeof(month)))
		return false;
	if (!file.write((const char *)&day, sizeof(day)))
		return false;
	if (!file.write((const char *)&hour, sizeof(hour)))
		return false;
	if (!file.write((const char *)&minute, sizeof(minute)))
		return false;
	if (!file.write((const char *)&lat, sizeof(lat)))
		return false;
	if (!file.write((const char *)&lon, sizeof(lon)))
		return false;
	if (!file.write((const char *)&ppm, sizeof(ppm)))
		return false;
	if (!file.write((const char *)&level, sizeof(level)))
		return false;
	if (!file.write((const char *)&altitude, sizeof(altitude)))
		return false;
	if (!file.write((const char *)&received_stations, sizeof(received_stations)))
		return false;
	if (!file.write((const char *)&distance, sizeof(distance)))
		return false;
	if (!file.write((const char *)&draught, sizeof(draught)))
		return false;
	if (!file.write((const char *)&speed, sizeof(speed)))
		return false;
	if (!file.write((const char *)&cog, sizeof(cog)))
		return false;
	if (!file.write((const char *)&last_signal, sizeof(last_signal)))
		return false;
	if (!file.write((const char *)&last_direct_signal, sizeof(last_direct_signal)))
		return false;
	if (!file.write((const char *)&shipclass, sizeof(shipclass)))
		return false;
	if (!file.write((const char *)&mmsi_type, sizeof(mmsi_type)))
		return false;
	if (!file.write((const char *)shipname, sizeof(shipname)))
		return false;
	if (!file.write((const char *)destination, sizeof(destination)))
		return false;
	if (!file.write((const char *)callsign, sizeof(callsign)))
		return false;
	if (!file.write((const char *)country_code, sizeof(country_code)))
		return false;
	if (!file.write((const char *)&last_group, sizeof(last_group)))
		return false;
	if (!file.write((const char *)&incoming.next, sizeof(incoming.next)))
		return false;
	if (!file.write((const char *)&incoming.prev, sizeof(incoming.prev)))
		return false;
	if (!file.write((const char *)&path_ptr, sizeof(path_ptr)))
		return false;

	return true;
}

bool Ship::Load(std::ifstream &file)
{
	// Read magic number and version first
	int magic = 0, version = 0;

	if (!file.read((char *)&magic, sizeof(magic)))
		return false;
	if (!file.read((char *)&version, sizeof(version)))
		return false;

	if (magic != _SHIP_MAGIC || version != _SHIP_VERSION)
		return false;

	// Read all ship data except the msg vector (which is skipped)
	if (!file.read((char *)&mmsi, sizeof(mmsi)))
		return false;
	if (!file.read((char *)&count, sizeof(count)))
		return false;
	if (!file.read((char *)&msg_type, sizeof(msg_type)))
		return false;
	if (!file.read((char *)&shiptype, sizeof(shiptype)))
		return false;
	if (!file.read((char *)&group_mask, sizeof(group_mask)))
		return false;
	if (!file.read((char *)&flags, sizeof(flags)))
		return false;
	if (!file.read((char *)&heading, sizeof(heading)))
		return false;
	if (!file.read((char *)&status, sizeof(status)))
		return false;
	if (!file.read((char *)&to_port, sizeof(to_port)))
		return false;
	if (!file.read((char *)&to_bow, sizeof(to_bow)))
		return false;
	if (!file.read((char *)&to_starboard, sizeof(to_starboard)))
		return false;
	if (!file.read((char *)&to_stern, sizeof(to_stern)))
		return false;
	if (!file.read((char *)&IMO, sizeof(IMO)))
		return false;
	if (!file.read((char *)&angle, sizeof(angle)))
		return false;
	if (!file.read((char *)&month, sizeof(month)))
		return false;
	if (!file.read((char *)&day, sizeof(day)))
		return false;
	if (!file.read((char *)&hour, sizeof(hour)))
		return false;
	if (!file.read((char *)&minute, sizeof(minute)))
		return false;
	if (!file.read((char *)&lat, sizeof(lat)))
		return false;
	if (!file.read((char *)&lon, sizeof(lon)))
		return false;
	if (!file.read((char *)&ppm, sizeof(ppm)))
		return false;
	if (!file.read((char *)&level, sizeof(level)))
		return false;
	if (!file.read((char *)&altitude, sizeof(altitude)))
		return false;
	if (!file.read((char *)&received_stations, sizeof(received_stations)))
		return false;
	if (!file.read((char *)&distance, sizeof(distance)))
		return false;
	if (!file.read((char *)&draught, sizeof(draught)))
		return false;
	if (!file.read((char *)&speed, sizeof(speed)))
		return false;
	if (!file.read((char *)&cog, sizeof(cog)))
		return false;
	if (!file.read((char *)&last_signal, sizeof(last_signal)))
		return false;
	if (!file.read((char *)&last_direct_signal, sizeof(last_direct_signal)))
		return false;
	if (!file.read((char *)&shipclass, sizeof(shipclass)))
		return false;
	if (!file.read((char *)&mmsi_type, sizeof(mmsi_type)))
		return false;
	if (!file.read((char *)shipname, sizeof(shipname)))
		return false;
	if (!file.read((char *)destination, sizeof(destination)))
		return false;
	if (!file.read((char *)callsign, sizeof(callsign)))
		return false;
	if (!file.read((char *)country_code, sizeof(country_code)))
		return false;
	if (!file.read((char *)&last_group, sizeof(last_group)))
		return false;
	if (!file.read((char *)&incoming.next, sizeof(incoming.next)))
		return false;
	if (!file.read((char *)&incoming.prev, sizeof(incoming.prev)))
		return false;
	if (!file.read((char *)&path_ptr, sizeof(path_ptr)))
		return false;

	// Clear the msg vector as it's not persisted
	msg.clear();

	return true;
}
