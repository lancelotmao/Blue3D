//
//  HouyiVector4.h
//  Houyi
//
//  Created by Lance Mao on 26/2/14.
//  Copyright (c) 2014 Lance Mao. All rights reserved.
//

#ifndef Houyi_HouyiVector4_h
#define Houyi_HouyiVector4_h

#include "HouyiDef.h"
#include "HouyiVector3.h"

namespace Houyi
{
    
	/** \addtogroup Core
     *  @{
     */
	/** \addtogroup Math
     *  @{
     */
	/** 4-dimensional homogeneous vector.
     */
    class Vector4
    {
    public:
        Real x, y, z, w;
        
    public:
        inline Vector4()
        {
        }
        
        inline Vector4( const Real fX, const Real fY, const Real fZ, const Real fW )
        : x( fX ), y( fY ), z( fZ ), w( fW)
        {
        }
        
        inline explicit Vector4( const Real afCoordinate[4] )
        : x( afCoordinate[0] ),
        y( afCoordinate[1] ),
        z( afCoordinate[2] ),
        w( afCoordinate[3] )
        {
        }
        
        inline explicit Vector4( const int afCoordinate[4] )
        {
            x = (Real)afCoordinate[0];
            y = (Real)afCoordinate[1];
            z = (Real)afCoordinate[2];
            w = (Real)afCoordinate[3];
        }
        
        inline explicit Vector4( Real* const r )
        : x( r[0] ), y( r[1] ), z( r[2] ), w( r[3] )
        {
        }
        
        inline explicit Vector4( const Real scaler )
        : x( scaler )
        , y( scaler )
        , z( scaler )
        , w( scaler )
        {
        }
        
        inline explicit Vector4(const Vector3& rhs)
        : x(rhs.x), y(rhs.y), z(rhs.z), w(1.0f)
        {
        }
        
		/** Exchange the contents of this vector with another.
         */
		inline void swap(Vector4& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
			std::swap(w, other.w);
		}
        
		inline Real operator [] ( const size_t i ) const
        {
            return *(&x+i);
        }
        
		inline Real& operator [] ( const size_t i )
        {
            return *(&x+i);
        }
        
		/// Pointer accessor for direct copying
		inline Real* ptr()
		{
			return &x;
		}
		/// Pointer accessor for direct copying
		inline const Real* ptr() const
		{
			return &x;
		}
        
        /** Assigns the value of the other vector.
         @param
         rkVector The other vector
         */
        inline Vector4& operator = ( const Vector4& rkVector )
        {
            x = rkVector.x;
            y = rkVector.y;
            z = rkVector.z;
            w = rkVector.w;
            
            return *this;
        }
        
		inline Vector4& operator = ( const Real fScalar)
		{
			x = fScalar;
			y = fScalar;
			z = fScalar;
			w = fScalar;
			return *this;
		}
        
        inline bool operator == ( const Vector4& rkVector ) const
        {
            return ( x == rkVector.x &&
                    y == rkVector.y &&
                    z == rkVector.z &&
                    w == rkVector.w );
        }
        
        inline bool operator != ( const Vector4& rkVector ) const
        {
            return ( x != rkVector.x ||
                    y != rkVector.y ||
                    z != rkVector.z ||
                    w != rkVector.w );
        }
        
        inline Vector4& operator = (const Vector3& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = 1.0f;
            return *this;
        }
        
        // arithmetic operations
        inline Vector4 operator + ( const Vector4& rkVector ) const
        {
            return Vector4(
                           x + rkVector.x,
                           y + rkVector.y,
                           z + rkVector.z,
                           w + rkVector.w);
        }
        
        inline Vector4 operator - ( const Vector4& rkVector ) const
        {
            return Vector4(
                           x - rkVector.x,
                           y - rkVector.y,
                           z - rkVector.z,
                           w - rkVector.w);
        }
        
        inline Vector4 operator * ( const Real fScalar ) const
        {
            return Vector4(
                           x * fScalar,
                           y * fScalar,
                           z * fScalar,
                           w * fScalar);
        }
        
        inline Vector4 operator * ( const Vector4& rhs) const
        {
            return Vector4(
                           rhs.x * x,
                           rhs.y * y,
                           rhs.z * z,
                           rhs.w * w);
        }
        
        inline Vector4 operator / ( const Real fScalar ) const
        {
            Real fInv = 1.0f / fScalar;
            
            return Vector4(
                           x * fInv,
                           y * fInv,
                           z * fInv,
                           w * fInv);
        }
        
        inline Vector4 operator / ( const Vector4& rhs) const
        {
            return Vector4(
                           x / rhs.x,
                           y / rhs.y,
                           z / rhs.z,
                           w / rhs.w);
        }
        
        inline const Vector4& operator + () const
        {
            return *this;
        }
        
        inline Vector4 operator - () const
        {
            return Vector4(-x, -y, -z, -w);
        }
        
        inline friend Vector4 operator * ( const Real fScalar, const Vector4& rkVector )
        {
            return Vector4(
                           fScalar * rkVector.x,
                           fScalar * rkVector.y,
                           fScalar * rkVector.z,
                           fScalar * rkVector.w);
        }
        
        inline friend Vector4 operator / ( const Real fScalar, const Vector4& rkVector )
        {
            return Vector4(
                           fScalar / rkVector.x,
                           fScalar / rkVector.y,
                           fScalar / rkVector.z,
                           fScalar / rkVector.w);
        }
        
        inline friend Vector4 operator + (const Vector4& lhs, const Real rhs)
        {
            return Vector4(
                           lhs.x + rhs,
                           lhs.y + rhs,
                           lhs.z + rhs,
                           lhs.w + rhs);
        }
        
        inline friend Vector4 operator + (const Real lhs, const Vector4& rhs)
        {
            return Vector4(
                           lhs + rhs.x,
                           lhs + rhs.y,
                           lhs + rhs.z,
                           lhs + rhs.w);
        }
        
        inline friend Vector4 operator - (const Vector4& lhs, Real rhs)
        {
            return Vector4(
                           lhs.x - rhs,
                           lhs.y - rhs,
                           lhs.z - rhs,
                           lhs.w - rhs);
        }
        
        inline friend Vector4 operator - (const Real lhs, const Vector4& rhs)
        {
            return Vector4(
                           lhs - rhs.x,
                           lhs - rhs.y,
                           lhs - rhs.z,
                           lhs - rhs.w);
        }
        
        // arithmetic updates
        inline Vector4& operator += ( const Vector4& rkVector )
        {
            x += rkVector.x;
            y += rkVector.y;
            z += rkVector.z;
            w += rkVector.w;
            
            return *this;
        }
        
        inline Vector4& operator -= ( const Vector4& rkVector )
        {
            x -= rkVector.x;
            y -= rkVector.y;
            z -= rkVector.z;
            w -= rkVector.w;
            
            return *this;
        }
        
        inline Vector4& operator *= ( const Real fScalar )
        {
            x *= fScalar;
            y *= fScalar;
            z *= fScalar;
            w *= fScalar;
            return *this;
        }
        
        inline Vector4& operator += ( const Real fScalar )
        {
            x += fScalar;
            y += fScalar;
            z += fScalar;
            w += fScalar;
            return *this;
        }
        
        inline Vector4& operator -= ( const Real fScalar )
        {
            x -= fScalar;
            y -= fScalar;
            z -= fScalar;
            w -= fScalar;
            return *this;
        }
        
        inline Vector4& operator *= ( const Vector4& rkVector )
        {
            x *= rkVector.x;
            y *= rkVector.y;
            z *= rkVector.z;
            w *= rkVector.w;
            
            return *this;
        }
        
        inline Vector4& operator /= ( const Real fScalar )
        {
            Real fInv = 1.0f / fScalar;
            
            x *= fInv;
            y *= fInv;
            z *= fInv;
            w *= fInv;
            
            return *this;
        }
        
        inline Vector4& operator /= ( const Vector4& rkVector )
        {
            x /= rkVector.x;
            y /= rkVector.y;
            z /= rkVector.z;
            w /= rkVector.w;
            
            return *this;
        }
        
        /** Calculates the dot (scalar) product of this vector with another.
         @param
         vec Vector with which to calculate the dot product (together
         with this one).
         @return
         A float representing the dot product value.
         */
        inline Real dotProduct(const Vector4& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
        }
		/// Check whether this vector contains valid values
		inline bool isNaN() const
		{
			return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
		}
        
        inline static void scalar(Vector4* v, float* scalar)
        {
//#if DEBUG
            (*v) = (*v) * (*scalar);
//#else
//            asm volatile
//            (
//             
//             "vldmia %1, {q1}     \n\t"
//             //"vdup.32 %1, {q1}     \n\t"
//             
//             "vmul.f32 q0, q0, d2[0] \n\t"
//             
//             "vstmia %0, {q0}"
//             
//             : // no output
//             : "r" (v), "r" (scalar)
//             : "memory", "q0", "q1"
//             );
//#endif
        }
        
        // matrix is column major but we have row major by default so tranpose is needed
        static void transform(const float* m, const Vector4* vIn, Vector4* vOut);

        // special
        static const Vector4 ZERO;
    };
	/** @} */
	/** @} */
    
}

#endif
