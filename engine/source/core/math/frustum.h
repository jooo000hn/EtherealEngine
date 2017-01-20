#pragma once

#include "math_types.h"
#include "plane.h"
#include "transform.h"
#include "bbox.h"
#include "bbox_extruded.h"

namespace math
{
	using namespace glm;

	//-----------------------------------------------------------------------------
	// Main class declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : frustum (Class)
	/// <summary>
	/// Storage for frustum planes / values and wraps up common functionality
	/// </summary>
	//-----------------------------------------------------------------------------
	class frustum
	{
	public:
		//-------------------------------------------------------------------------
		// Constructors & Destructors
		//-------------------------------------------------------------------------
		frustum();
		frustum(const transform_t & view, const transform_t & proj, bool _oglNDC);
		frustum(const bbox & sourceBounds);

		//-------------------------------------------------------------------------
		// Public Methods
		//-------------------------------------------------------------------------
		void update(const transform_t & view, const transform_t & proj, bool _oglNDC);
		void set_planes(const plane newPlanes[]);
		void recompute_points();
		VolumeQuery::E classify_aabb(const bbox & bounds) const;
		VolumeQuery::E classify_aabb(const bbox & bounds, unsigned int & frustumBits, int & lastOutside) const;
		VolumeQuery::E classify_sphere(const vec3 & center, float radius) const;
		VolumeQuery::E classify_plane(const plane & plane) const;
		bool test_point(const vec3 & point) const;
		bool test_aabb(const bbox & bounds) const;

		bool test_extruded_aabb(const bbox_extruded & box) const;
		bool test_sphere(const vec3 & center, float radius) const;
		bool test_swept_sphere(const vec3 & center, float radius, const vec3 & sweepDirection) const;
		bool test_frustum(const frustum & frustum) const;
		bool test_line(const vec3 & v1, const vec3 & v2) const;
		frustum& mul(const transform_t & t);
		//-------------------------------------------------------------------------
		// Public Static Functions
		//-------------------------------------------------------------------------
		static frustum mul(frustum f, const transform_t & t);
		static bool test_obb(frustum f, const bbox & bounds, const transform_t & t);
		static bool test_extruded_obb(frustum f, const bbox_extruded & bounds, const transform_t & t);
		static VolumeQuery::E classify_obb(frustum f, const bbox & bounds, const transform_t & t);
		static VolumeQuery::E classify_obb(frustum f, const bbox & bounds, const transform_t & t, unsigned int & frustumBits, int & lastOutside);
		//-------------------------------------------------------------------------
		// Public Operators
		//-------------------------------------------------------------------------
		bool operator == (const frustum & f) const;

		//-------------------------------------------------------------------------
		// Public Variables
		//-------------------------------------------------------------------------
		plane planes[6];      // The 6 planes of the frustum.
		vec3 points[8];      // The 8 corner points of the frustum.
		vec3 position;       // The originating position of the frustum.

	private:
		//-------------------------------------------------------------------------
		// Private Static Functions
		//-------------------------------------------------------------------------
		static bool swept_sphere_intersect_plane(float & t0, float & t1, const plane & plane, const vec3 & center, float radius, const vec3 & sweepDirection);
	};

}