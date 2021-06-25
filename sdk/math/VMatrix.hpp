#pragma once

#include <limits>
#include "Vector.hpp"
#include <DirectXMath.h>

class Quaternion
{
public:
	float x, y, z, w;
};

struct cplane_t {
	Vector normal;
	float dist;
	uint8_t type;   // for fast side tests
	uint8_t signbits;  // signx + (signy<<1) + (signz<<1)
	uint8_t pad[2];

};

class matrix3x4_t {

public:

	matrix3x4_t() { } //-V730
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23) {
		m_flMatVal[0][0] = m00; m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10; m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20; m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}
	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin) {
		m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin) {
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	inline void SetOrigin(Vector const & p) {
		m_flMatVal[0][3] = p.x;
		m_flMatVal[1][3] = p.y;
		m_flMatVal[2][3] = p.z;
	}

	inline void Invalidate(void) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				m_flMatVal[i][j] = std::numeric_limits<float>::infinity();;
			}
		}
	}

	template<typename T>
	T ToRadians(T degrees) {
		return (degrees * (static_cast<T>(RadPi) / static_cast<T>(DegPi)));
	}

	void AngleMatrix(const Vector& angles) 
	{
		float sr, sp, sy, cr, cp, cy;
		DirectX::XMScalarSinCos(&sy, &cy, ToRadians(angles[1]));
		DirectX::XMScalarSinCos(&sp, &cp, ToRadians(angles[0]));
		DirectX::XMScalarSinCos(&sr, &cr, ToRadians(angles[2]));

		m_flMatVal[0][0] = cp * cy;
		m_flMatVal[1][0] = cp * sy;
		m_flMatVal[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		m_flMatVal[0][1] = sp * srcy - crsy;
		m_flMatVal[1][1] = sp * srsy + crcy;
		m_flMatVal[2][1] = sr * cp;

		m_flMatVal[0][2] = (sp * crcy + srsy);
		m_flMatVal[1][2] = (sp * crsy - srcy);
		m_flMatVal[2][2] = cr * cp;

		m_flMatVal[0][3] = 0.0f;
		m_flMatVal[1][3] = 0.0f;
		m_flMatVal[2][3] = 0.0f;
	}

	void MatrixSetColumn(const Vector& in, int column) 
	{
		m_flMatVal[0][column] = in.x;
		m_flMatVal[1][column] = in.y;
		m_flMatVal[2][column] = in.z;
	}

	void AngleMatrix(const Vector& angles, const Vector& position)
	{
		AngleMatrix(angles);
		MatrixSetColumn(position, 3);
	}

	void QuaternionMatrix(const Quaternion& q) 
	{
		m_flMatVal[0][0] = 1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z;
		m_flMatVal[1][0] = 2.0 * q.x * q.y + 2.0 * q.w * q.z;
		m_flMatVal[2][0] = 2.0 * q.x * q.z - 2.0 * q.w * q.y;

		m_flMatVal[0][1] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
		m_flMatVal[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
		m_flMatVal[2][1] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;

		m_flMatVal[0][2] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
		m_flMatVal[1][2] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
		m_flMatVal[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

		m_flMatVal[0][3] = 0.0f;
		m_flMatVal[1][3] = 0.0f;
		m_flMatVal[2][3] = 0.0f;
	}

	void QuaternionMatrix(const Quaternion& q, const Vector& pos) 
	{
		QuaternionMatrix(q);

		m_flMatVal[0][3] = pos.x;
		m_flMatVal[1][3] = pos.y;
		m_flMatVal[2][3] = pos.z;
	}

	matrix3x4_t matrix3x4_t::ConcatTransforms(matrix3x4_t in) const 
	{
		matrix3x4_t out;

		out[0][0] = m_flMatVal[0][0] * in[0][0] + m_flMatVal[0][1] * in[1][0] + m_flMatVal[0][2] * in[2][0];
		out[0][1] = m_flMatVal[0][0] * in[0][1] + m_flMatVal[0][1] * in[1][1] + m_flMatVal[0][2] * in[2][1];
		out[0][2] = m_flMatVal[0][0] * in[0][2] + m_flMatVal[0][1] * in[1][2] + m_flMatVal[0][2] * in[2][2];
		out[0][3] = m_flMatVal[0][0] * in[0][3] + m_flMatVal[0][1] * in[1][3] + m_flMatVal[0][2] * in[2][3] + m_flMatVal[0][3];
		out[1][0] = m_flMatVal[1][0] * in[0][0] + m_flMatVal[1][1] * in[1][0] + m_flMatVal[1][2] * in[2][0];
		out[1][1] = m_flMatVal[1][0] * in[0][1] + m_flMatVal[1][1] * in[1][1] + m_flMatVal[1][2] * in[2][1];
		out[1][2] = m_flMatVal[1][0] * in[0][2] + m_flMatVal[1][1] * in[1][2] + m_flMatVal[1][2] * in[2][2];
		out[1][3] = m_flMatVal[1][0] * in[0][3] + m_flMatVal[1][1] * in[1][3] + m_flMatVal[1][2] * in[2][3] + m_flMatVal[1][3];
		out[2][0] = m_flMatVal[2][0] * in[0][0] + m_flMatVal[2][1] * in[1][0] + m_flMatVal[2][2] * in[2][0];
		out[2][1] = m_flMatVal[2][0] * in[0][1] + m_flMatVal[2][1] * in[1][1] + m_flMatVal[2][2] * in[2][1];
		out[2][2] = m_flMatVal[2][0] * in[0][2] + m_flMatVal[2][1] * in[1][2] + m_flMatVal[2][2] * in[2][2];
		out[2][3] = m_flMatVal[2][0] * in[0][3] + m_flMatVal[2][1] * in[1][3] + m_flMatVal[2][2] * in[2][3] + m_flMatVal[2][3];

		return out;
	}

	Vector GetXAxis()  const { return at(0); }
	Vector GetYAxis()  const { return at(1); }
	Vector GetZAxis()  const { return at(2); }
	Vector GetOrigin() const { return at(3); }

	Vector at(int i) const { return Vector{ m_flMatVal[0][i], m_flMatVal[1][i], m_flMatVal[2][i] }; }

	float *operator[](int i) { return m_flMatVal[i]; }
	const float *operator[](int i) const { return m_flMatVal[i]; }
	float *Base() { return &m_flMatVal[0][0]; }
	const float *Base() const { return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};

class VMatrix //-V690
{
public:

	VMatrix();
	VMatrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	);

	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	VMatrix(const Vector& forward, const Vector& left, const Vector& up);

	// Construct from a 3x4 matrix
	VMatrix(const matrix3x4_t& matrix3x4);

	// Set the values in the matrix.
	void  Init(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	);


	// Initialize from a 3x4
	void  Init(const matrix3x4_t& matrix3x4);

	// array access
	inline float* operator[](int i) {
		return m[i];
	}

	inline const float* operator[](int i) const {
		return m[i];
	}

	// Get a pointer to m[0][0]
	inline float *Base() {
		return &m[0][0];
	}

	inline const float *Base() const {
		return &m[0][0];
	}

	void  SetLeft(const Vector &vLeft);
	void  SetUp(const Vector &vUp);
	void  SetForward(const Vector &vForward);

	void  GetBasisVectors(Vector &vForward, Vector &vLeft, Vector &vUp) const;
	void  SetBasisVectors(const Vector &vForward, const Vector &vLeft, const Vector &vUp);

	// Get/Set the translation.
	Vector & GetTranslation(Vector &vTrans) const;
	void  SetTranslation(const Vector &vTrans);

	void  PreTranslate(const Vector &vTrans);
	void  PostTranslate(const Vector &vTrans);

	matrix3x4_t& As3x4();
	const matrix3x4_t& As3x4() const;
	void  CopyFrom3x4(const matrix3x4_t &m3x4);
	void  Set3x4(matrix3x4_t& matrix3x4) const;

	bool  operator==(const VMatrix& src) const;
	bool  operator!=(const VMatrix& src) const { return !(*this == src); }

	// Access the basis vectors.
	Vector  GetLeft() const;
	Vector  GetUp() const;
	Vector  GetForward() const;
	Vector  GetTranslation() const;


	// Matrix->vector operations.
public:
	// Multiply by a 3D vector (same as operator*).
	void  V3Mul(const Vector &vIn, Vector &vOut) const;

	// Multiply by a 4D vector.
	//void  V4Mul( const Vector4D &vIn, Vector4D &vOut ) const;

	// Applies the rotation (ignores translation in the matrix). (This just calls VMul3x3).
	Vector  ApplyRotation(const Vector &vVec) const;

	// Multiply by a vector (divides by w, assumes input w is 1).
	Vector  operator*(const Vector &vVec) const;

	// Multiply by the upper 3x3 part of the matrix (ie: only apply rotation).
	Vector  VMul3x3(const Vector &vVec) const;

	// Apply the inverse (transposed) rotation (only works on pure rotation matrix)
	Vector  VMul3x3Transpose(const Vector &vVec) const;

	// Multiply by the upper 3 rows.
	Vector  VMul4x3(const Vector &vVec) const;

	// Apply the inverse (transposed) transformation (only works on pure rotation/translation)
	Vector  VMul4x3Transpose(const Vector &vVec) const;


	// Matrix->plane operations.
	//public:
	// Transform the plane. The matrix can only contain translation and rotation.
	//void  TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const;

	// Just calls TransformPlane and returns the result.
	//VPlane  operator*(const VPlane &thePlane) const;

	// Matrix->matrix operations.
public:

	VMatrix& operator=(const VMatrix &mOther);

	// Add two matrices.
	const VMatrix& operator+=(const VMatrix &other);

	// Add/Subtract two matrices.
	VMatrix  operator+(const VMatrix &other) const;
	VMatrix  operator-(const VMatrix &other) const;

	// Negation.
	VMatrix  operator-() const;

	// Return inverse matrix. Be careful because the results are undefined 
	// if the matrix doesn't have an inverse (ie: InverseGeneral returns false).
	VMatrix  operator~() const;

	// Matrix operations.
public:
	// Set to identity.
	void  Identity();
	bool  IsIdentity() const;
public:
	// The matrix.
	float  m[4][4];
};

inline void MatrixGetColumn(const matrix3x4_t &src, int nCol, Vector& pColumn) {
	pColumn.x = src[0][nCol];
	pColumn.y = src[1][nCol];
	pColumn.z = src[2][nCol];
}

inline void MatrixPosition(const matrix3x4_t &matrix, Vector &position) {
	MatrixGetColumn(matrix, 3, position);
}