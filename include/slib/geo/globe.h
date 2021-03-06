/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CHECKHEADER_SLIB_GEO_GLOBE
#define CHECKHEADER_SLIB_GEO_GLOBE

#include "definition.h"

#include "geo_location.h"

#include "../math/vector3.h"

namespace slib
{
	
	// ellipsoid globe structure
	class SLIB_EXPORT Globe
	{
	public:
		double radiusEquatorial;
		double radiusPolar;
	
		double inverseFlattening;
		double eccentricitySquared;
	
	public:
		Globe();

		Globe(const Globe& other);

		Globe(double radiusEquatorial, double radiusPolar);
	
		Globe(double radiusEquatorial, double radiusPolar, double inverseFlattening, double eccentricitySquared);

	public:
		Globe& operator=(const Globe& other);
	
	public:
		double getEquatorialRadius() const;

		void setEquatorialRadius(double radius);
	
		double getPolarRadius() const;
	
		void setPolarRadius(double radius);
	
		double getInverseFlattening() const;
	
		double getEccentricitySquared() const;
	

		Vector3lf getSurfaceNormal(double latitude, double longitude) const;

		Vector3lf getSurfaceNormal(const LatLon& latlon) const;

		Vector3lf getSurfaceNormal(const GeoLocation& location) const;
	
		Vector3lf getNorthPointingTangent(double latitude, double longitude) const;

		Vector3lf getNorthPointingTangent(const LatLon& latlon) const;

		Vector3lf getNorthPointingTangent(const GeoLocation& location) const;

		Vector3lf getSurfaceNormalAtCartesianPosition(double x, double y, double z) const;

		Vector3lf getSurfaceNormalAtCartesianPosition(const Vector3lf& position) const;

		/*
			unit: m
			(0, 0, 0): center
			(0, 1, 0): direction to north pole
			(1, 0, 0): direction to LatLon(0, 90)
			(0, 0, 1): direction to LatLon(0, 180)
		*/
		Vector3lf getCartesianPosition(double latitude, double longitude, double altitude) const;

		Vector3lf getCartesianPosition(const LatLon& latlon) const;

		Vector3lf getCartesianPosition(const GeoLocation& location) const;
	
		GeoLocation getGeoLocation(double x, double y, double z) const;

		GeoLocation getGeoLocation(const Vector3lf& position) const;

	protected:
		void _initializeParameters();

	};
	
	// spherical globe structure
	class SLIB_EXPORT SphericalGlobe
	{
	public:
		double radius;
	
	public:
		SphericalGlobe();

		SphericalGlobe(const SphericalGlobe& other);
	
		SphericalGlobe(double radius);

	public:
		SphericalGlobe& operator=(const SphericalGlobe& other);
	
	public:
		double getRadius() const;
	
		void setRadius(double radius);
	
		Vector3lf getSurfaceNormal(double latitude, double longitude) const;

		Vector3lf getSurfaceNormal(const LatLon& latlon) const;

		Vector3lf getSurfaceNormal(const GeoLocation& location) const;
	
		Vector3lf getNorthPointingTangent(double latitude, double longitude) const;

		Vector3lf getNorthPointingTangent(const LatLon& latlon) const;

		Vector3lf getNorthPointingTangent(const GeoLocation& location) const;
	
		/*
			unit: m
			(0, 0, 0): center
			(0, 1, 0): direction to north pole
			(1, 0, 0): direction to LatLon(0, 90)
			(0, 0, 1): direction to LatLon(0, 180)
		*/
		Vector3lf getCartesianPosition(double latitude, double longitude, double altitude) const;

		Vector3lf getCartesianPosition(const LatLon& latlon) const;

		Vector3lf getCartesianPosition(const GeoLocation& location) const;
	
		GeoLocation getGeoLocation(double x, double y, double z) const;

		GeoLocation getGeoLocation(const Vector3lf& position) const;

	};

}

#endif
