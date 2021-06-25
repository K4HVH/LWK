// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "math.hpp"
#define M_RADPI 57.295779513082f

namespace math
{
	void normalize_angles(Vector& angles)
	{
		while (angles.x > 89.0f)
			angles.x -= 180.0f;

		while (angles.x < -89.0f)
			angles.x += 180.0f;

		while (angles.y < -180.0f)
			angles.y += 360.0f;

		while (angles.y > 180.0f)
			angles.y -= 360.0f;

		angles.z = 0.0f;
	}

	float angle_distance(float firstangle, float secondangle)
	{
		if (firstangle == secondangle) //-V550
			return 0.f;

		bool oppositeSides = false;

		if (firstangle > 0 && secondangle < 0)
			oppositeSides = true;
		else if (firstangle < 0 && secondangle > 0)
			oppositeSides = true;

		if (!oppositeSides)
			return fabs(firstangle - secondangle);

		bool past90 = false;

		if (firstangle > 90 && secondangle < -90)
		{
			firstangle -= (firstangle - 90);
			secondangle += (secondangle + 90);
		}
		else if (firstangle < -90 && secondangle > 90)
		{
			firstangle += (firstangle + 90);
			secondangle -= (secondangle - 90);
		}

		float oneTwo;

		oneTwo = fabs(firstangle - secondangle);

		return oneTwo;
	}

	void normalize_angle(QAngle& angles) {
		for (int i = 0; i < 3; i++) {
			while (angles[i] < -180.0f) angles[i] += 360.0f;
			while (angles[i] > 180.0f) angles[i] -= 360.0f;
		}
	}

	void normalize_vector(Vector & vector) {
		for (int i = 0; i < 3; i++) {
			while (vector[i] < -180.0f) vector[i] += 360.0f;
			while (vector[i] > 180.0f) vector[i] -= 360.0f;
		}

		vector[2] = 0.f;
	}

	//--------------------------------------------------------------------------------
	float normalize_yaw(float f)
	{
		while (f < -180.0f)
			f += 360.0f;

		while (f > 180.0f)
			f -= 360.0f;

		return f;
	}

	float normalize_pitch(float pitch)
	{
		while (pitch > 89.0f)
			pitch -= 180.0f;

		while (pitch < -89.0f)
			pitch += 180.0f;

		return pitch;
	}
	//--------------------------------------------------------------------------------
	float vector_normalize(Vector& v) {
		float l = v.Length();

		if (l != 0.0f) //-V550
			v /= l;
		else
		{
			v.x = v.y = 0.0f; 
			v.z = 1.0f;
		}

		return l;
	}
	//--------------------------------------------------------------------------------
	void clamp_angles(Vector& angles)
	{
		if (angles.x > 89.0f)
			angles.x = 89.0f;
		else if (angles.x < -89.0f)
			angles.x = -89.0f;

		if (angles.y > 180.0f)
			angles.y = 180.0f;
		else if (angles.y < -180.0f)
			angles.y = -180.0f;

		angles.z = 0;
	}
	//--------------------------------------------------------------------------------
	void vector_transform(const Vector& in1, const matrix3x4_t& in2, Vector& out) 
	{
		out = Vector(in1.Dot(Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3], in1.Dot(Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3], in1.Dot(Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3]);
	}
	//--------------------------------------------------------------------------------
	Vector calculate_angle(const Vector& src, const Vector& dst) {
		Vector angles;

		Vector delta = src - dst;
		float hyp = delta.Length2D();

		angles.y = std::atanf(delta.y / delta.x) * M_RADPI;
		angles.x = std::atanf(-delta.z / hyp) * -M_RADPI;
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return angles;
	}
	//--------------------------------------------------------------------------------
	float get_fov(const Vector& view_angle, const Vector& aim_angle)
	{
		Vector delta = aim_angle - view_angle;
		normalize_angles(delta);

		return min(sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f)), 180.0f);
	}
	//--------------------------------------------------------------------------------
	void angle_vectors(const Vector& angles, Vector& forward)
	{
		float sp, sy, cp, cy;

		sy = sin(DEG2RAD(angles[1]));
		cy = cos(DEG2RAD(angles[1]));

		sp = sin(DEG2RAD(angles[0]));
		cp = cos(DEG2RAD(angles[0]));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}
	//--------------------------------------------------------------------------------
	void angle_vectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
	{
		auto sin_cos = [](float radian, float* sin, float* cos)
		{
			*sin = std::sin(radian);
			*cos = std::cos(radian);
		};

		float sp, sy, sr, cp, cy, cr;

		sin_cos(M_PI / 180.0f * angles.x, &sp, &cp);
		sin_cos(M_PI / 180.0f * angles.y, &sy, &cy);
		sin_cos(M_PI / 180.0f * angles.z, &sr, &cr);

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = -1.0f * sr * sp * cy + -1.0f * cr * -sy;
			right->y = -1.0f * sr * sp * sy + -1.0f * cr * cy;
			right->z = -1.0f * sr * cp;
		}

		if (up)
		{
			up->x = cr * sp * cy + -sr * -sy;
			up->y = cr * sp * sy + -sr * cy;
			up->z = cr * cp;
		}
	}
	//--------------------------------------------------------------------------------
	void vector_angles(const Vector& forward, Vector& angles)
	{
		Vector view;

		if (!forward[0] && !forward[1])
		{
			view[0] = 0.0f;
			view[1] = 0.0f;
		}
		else
		{
			view[1] = atan2(forward[1], forward[0]) * 180.0f / M_PI;

			if (view[1] < 0.0f)
				view[1] += 360.0f;

			view[2] = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			view[0] = atan2(forward[2], view[2]) * 180.0f / M_PI;
		}

		angles[0] = -view[0];
		angles[1] = view[1];
		angles[2] = 0.f;
	}
	//--------------------------------------------------------------------------------
	void vector_angles(const Vector& forward, Vector& up, Vector& angles)
	{
		auto left = cross_product(up, forward);
		left.NormalizeInPlace();

		auto forwardDist = forward.Length2D();

		if (forwardDist > 0.001f)
		{
			angles.x = atan2(-forward.z, forwardDist) * 180.0f / M_PI;
			angles.y = atan2(forward.y, forward.x) * 180.0f / M_PI;

			auto upZ = (left.y * forward.x) - (left.x * forward.y);
			angles.z = atan2(left.z, upZ) * 180.0f / M_PI;
		}
		else 
		{
			angles.x = atan2(-forward.z, forwardDist) * 180.0f / M_PI;
			angles.y = atan2(-left.x, left.y) * 180.0f / M_PI;
			angles.z = 0.0f;
		}
	}
	//--------------------------------------------------------------------------------
	void VectorMAInline(const float* start, float scale, const float* direction, float* dest)
	{
		dest[0] = start[0] + direction[0] * scale;
		dest[1] = start[1] + direction[1] * scale;
		dest[2] = start[2] + direction[2] * scale;
	}

	void VectorMAInline(const Vector& start, float scale, const Vector& direction, Vector& dest)
	{
		dest.x = start.x + direction.x*scale;
		dest.y = start.y + direction.y*scale;
		dest.z = start.z + direction.z*scale;
	}

	void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest)
	{
		VectorMAInline(start, scale, direction, dest);
	}

	void VectorMA(const float * start, float scale, const float *direction, float *dest)
	{
		VectorMAInline(start, scale, direction, dest);
	}
	//--------------------------------------------------------------------------------
	Vector cross_product(const Vector & a, const Vector & b) 
	{
		return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}
	//--------------------------------------------------------------------------------
	float dot_product(const Vector & a, const Vector & b)
	{
		return (a.x * b.x
			+ a.y * b.y
			+ a.z * b.z);
	}
	//--------------------------------------------------------------------------------
	bool world_to_screen(const Vector &origin, Vector &screen)
	{
		return !m_debugoverlay()->ScreenPosition(origin, screen);
	}
	//--------------------------------------------------------------------------------
	void SmoothAngle(Vector& From, Vector& To, float Percent)
	{
		Vector VecDelta = From - To;
		math::normalize_angles(VecDelta);
		VecDelta.x *= Percent;
		VecDelta.y *= Percent;
		To = From - VecDelta;
	}
	//--------------------------------------------------------------------------------
	void rotate_triangle(std::array<Vector2D, 3>& points, float rotation)
	{
		const auto pointsCenter = (points.at(0) + points.at(1) + points.at(2)) / 3;
		for (auto& point : points)
		{
			point -= pointsCenter;

			const auto tempX = point.x;
			const auto tempY = point.y;

			const auto theta = DEG2RAD(rotation);
			const auto c = cos(theta);
			const auto s = sin(theta);

			point.x = tempX * c - tempY * s;
			point.y = tempX * s + tempY * c;

			point += pointsCenter;
		}
	}
	//--------------------------------------------------------------------------------
	float random_float(float min, float max) {
		typedef float(*RandomFloat_t)(float, float);
		static RandomFloat_t m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle(crypt_str("vstdlib.dll")), crypt_str("RandomFloat"));
		return m_RandomFloat(min, max);
	}
	//--------------------------------------------------------------------------------
	int random_int(int min, int max) {
		typedef int(*RandomInt_t)(int, int);
		static RandomInt_t m_RandomInt = (RandomInt_t)GetProcAddress(GetModuleHandle(crypt_str("vstdlib.dll")), crypt_str("RandomInt"));
		return m_RandomInt(min, max);
	}
	//--------------------------------------------------------------------------------
	void random_seed(int seed) {
		typedef void(*RandomSeed_t)(int);
		static RandomSeed_t m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandle(crypt_str("vstdlib.dll")), crypt_str("RandomSeed"));
		return m_RandomSeed(seed);
	}
	//--------------------------------------------------------------------------------
	float segment_to_segment(const Vector& s1, const Vector& s2, const Vector& k1, const Vector& k2)
	{
		static auto constexpr epsilon = 0.00000001f;

		auto u = s2 - s1;
		auto v = k2 - k1;
		auto w = s1 - k1;

		auto a = u.Dot(u); //-V525
		auto b = u.Dot(v);
		auto c = v.Dot(v);
		auto d = u.Dot(w);
		auto e = v.Dot(w);
		auto D = a * c - b * b;

		auto sn = 0.0f, sd = D;
		auto tn = 0.0f, td = D;

		if (D < epsilon)
		{
			sn = 0.0f;
			sd = 1.0f;
			tn = e;
			td = c;
		}
		else 
		{
			sn = b * e - c * d;
			tn = a * e - b * d;

			if (sn < 0.0f) 
			{
				sn = 0.0f;
				tn = e;
				td = c;
			}
			else if (sn > sd)
			{
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		if (tn < 0.0f) 
		{
			tn = 0.0f;

			if (-d < 0.0f)
				sn = 0.0f;
			else if (-d > a)
				sn = sd;
			else 
			{
				sn = -d;
				sd = a;
			}
		}
		else if (tn > td) 
		{
			tn = td;

			if (-d + b < 0.0f)
				sn = 0.0f;
			else if (-d + b > a)
				sn = sd;
			else 
			{
				sn = -d + b;
				sd = a;
			}
		}

		auto sc = fabs(sn) < epsilon ? 0.0f : sn / sd;
		auto tc = fabs(tn) < epsilon ? 0.0f : tn / td;

		auto dp = w + u * sc - v * tc;
		return dp.Length();
	}
	//--------------------------------------------------------------------------------
	bool intersect_line_with_bb(Vector& start, Vector& end, Vector& min, Vector& max)
	{
		char quadrant[3];
		int i;

		Vector candidatePlane;
		for (i = 0; i < 3; i++) {
			if (start[i] < min[i]) {
				quadrant[i] = 1;
				candidatePlane[i] = min[i];
			}
			else if (start[i] > max[i]) {
				quadrant[i] = 0;
				candidatePlane[i] = max[i];
			}
			else {
				quadrant[i] = 2;
			}
		}

		// Calculate T distances to candidate planes
		Vector maxT;
		for (i = 0; i < 3; i++) {
			if (quadrant[i] != 2 && end[i] != 0.f) //-V550
				maxT[i] = (candidatePlane[i] - start[i]) / end[i];
			else
				maxT[i] = -1.f;
		}

		// Get largest of the maxT's for final choice of intersection
		int whichPlane = 0;
		for (i = 1; i < 3; i++) {
			if (maxT[whichPlane] < maxT[i])
				whichPlane = i;
		}

		if (maxT[whichPlane] < 0.0f)
			return false;

		for (i = 0; i < 3; i++) {
			if (whichPlane != i) {
				float temp = start[i] + maxT[whichPlane] * end[i];
				if (temp < min[i] || temp > max[i]) {
					return false;
				}
			}
		}

		return true;
	}
	//--------------------------------------------------------------------------------
	Vector vector_rotate(const Vector& in1, const matrix3x4_t& in2)
	{
		return Vector(in1.Dot(in2[0]), in1.Dot(in2[1]), in1.Dot(in2[2]));
	}
	//--------------------------------------------------------------------------------
	Vector vector_rotate(const Vector& in1, const Vector& in2)
	{
		const auto matrix = angle_matrix(in2);
		return vector_rotate(in1, matrix);
	}
	//--------------------------------------------------------------------------------
	matrix3x4_t angle_matrix(const Vector& angles)
	{
		matrix3x4_t result;

		m128 angle, sin, cos;
		angle.f[0] = DEG2RAD(angles.x);
		angle.f[1] = DEG2RAD(angles.y);
		angle.f[2] = DEG2RAD(angles.z);
		sincos_ps(angle.v, &sin.v, &cos.v);

		result[0][0] = cos.f[0] * cos.f[1];
		result[1][0] = cos.f[0] * sin.f[1];
		result[2][0] = -sin.f[0];

		const auto crcy = cos.f[2] * cos.f[1];
		const auto crsy = cos.f[2] * sin.f[1];
		const auto srcy = sin.f[2] * cos.f[1];
		const auto srsy = sin.f[2] * sin.f[1];

		result[0][1] = sin.f[0] * srcy - crsy;
		result[1][1] = sin.f[0] * srsy + crcy;
		result[2][1] = sin.f[2] * cos.f[0];

		result[0][2] = sin.f[0] * crcy + srsy;
		result[1][2] = sin.f[0] * crsy - srcy;
		result[2][2] = cos.f[2] * cos.f[0];

		return result;
	}
	//
	void angle_matrix(const Vector& ang, const Vector& pos, matrix3x4_t& out) 
	{
		out = angle_matrix(ang);
		out.SetOrigin(pos);
	}
	//--------------------------------------------------------------------------------
	void vector_rotate(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
		out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
		out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
	}
	//--------------------------------------------------------------------------------
	Vector matrix_get_origin(const matrix3x4_t& src)
	{
		return Vector(src[0][3], src[1][3], src[2][3]);
	}
	//--------------------------------------------------------------------------------
	void matrix_set_origin(const Vector& pos, matrix3x4_t& matrix)
	{
		for (auto i = 0; i < 3; i++) 
			matrix[i][3] = pos[i];
	}
	//--------------------------------------------------------------------------------
	void fast_rsqrt(float a, float* out)
	{
		const auto xx = _mm_load_ss(&a);
		auto xr = _mm_rsqrt_ss(xx);
		auto xt = _mm_mul_ss(xr, xr);
		xt = _mm_mul_ss(xt, xx);
		xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
		xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
		xr = _mm_mul_ss(xr, xt);
		_mm_store_ss(out, xr);
	}
	//--------------------------------------------------------------------------------
	float fast_vec_normalize(Vector& vec)
	{
		const auto sqrlen = vec.LengthSqr() + 1.0e-10f;
		float invlen;
		fast_rsqrt(sqrlen, &invlen);
		vec.x *= invlen;
		vec.y *= invlen;
		vec.z *= invlen;
		return sqrlen * invlen;
	}
	//--------------------------------------------------------------------------------
	void math::matrix_copy(const matrix3x4_t& in, matrix3x4_t& out) 
	{
		memcpy(out.Base(), in.Base(), sizeof(matrix3x4_t));
	}
	//--------------------------------------------------------------------------------
	void math::concat_transforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out) 
	{
		if (&in1 == &out) 
		{
			matrix3x4_t in1b;
			matrix_copy(in1, in1b);
			concat_transforms(in1b, in2, out);
			return;
		}

		if (&in2 == &out) 
		{
			matrix3x4_t in2b;
			matrix_copy(in2, in2b);
			concat_transforms(in1, in2b, out);
			return;
		}

		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];

		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];

		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
	}
	//--------------------------------------------------------------------------------
	double fast_sin(double x)
	{
		int k;
		double y;
		double z;

		z = x;
		z *= 0.3183098861837907;
		z += 6755399441055744.0;
		k = *((int*)&z);
		z = k;
		z *= 3.1415926535897932;
		x -= z;
		y = x;
		y *= x;
		z = 0.0073524681968701;
		z *= y;
		z -= 0.1652891139701474;
		z *= y;
		z += 0.9996919862959676;
		x *= z;
		k &= 1;
		k += k;
		z = k;
		z *= x;
		x -= z;

		return x;
	}
	//--------------------------------------------------------------------------------
	float simple_spline(float value)
	{
		auto value_squared = value * value;
		return 3.0f * value_squared - 2.0f * value_squared * value;
	}
	//--------------------------------------------------------------------------------
	float simple_spline_remap_val_clamped(float value, float a, float b, float c, float d)
	{
		if (a == b) //-V550
			return value >= b ? d : c;

		auto c_value = math::clamp((value - a) / (b - a), 0.0f, 1.0f);
		return c + (d - c) * simple_spline(c_value);
	}
	//--------------------------------------------------------------------------------
	float direction_difference(const Vector& first, const Vector& second)
	{
		auto a = math::normalize_yaw(RAD2DEG(atan2(first.y, first.x)));
		auto b = math::normalize_yaw(RAD2DEG(atan2(second.y, second.x)));

		auto c = fmodf(a - b, 360.0f);

		if (a > b)
		{
			if (c >= 180.f)
				c -= 360.f;
		}
		else
		{
			if (c <= -180.f)
				c += 360.f;
		}

		return c;
	}
}