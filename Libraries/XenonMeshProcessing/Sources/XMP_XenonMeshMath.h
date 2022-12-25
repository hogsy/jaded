// ------------------------------------------------------------------------------------------------
// File   : XMP_XenonMeshMath.h
// Date   : 2005-06-01
// Author : Sebastien Comte
// Descr. : 
//
// UBISOFT Inc.
// ------------------------------------------------------------------------------------------------

#ifndef GUARD_XMP_XENONMESHMATH_H
#define GUARD_XMP_XENONMESHMATH_H

#if defined(ACTIVE_EDITORS)

#include "XMP_XenonMeshUtils.h"

namespace XenonMeshPack
{

// ------------------------------------------------------------------------------------------------
// MACROS
// ------------------------------------------------------------------------------------------------

#define XMP_FOR_ALL_1D(_op) for (ULONG i = 0; i < N; ++i) { _op; }
#define XMP_FOR_ALL_2D(_op) for (ULONG j = 0; j < N; ++j) { for (ULONG i = 0; i < N; ++i) { _op; } }
#define XMP_FOR_ALL_SM(_op) for (ULONG i = 0; i < MATRIX_SIZE; ++i) { _op; }

// ------------------------------------------------------------------------------------------------
// CLASSES
// ------------------------------------------------------------------------------------------------
  template<const ULONG N>
  class Vector
  {
  public:

    XMP_DECLARE_NEW_AND_DELETE();

    inline Vector(void)             { Reset(); }
    inline Vector(const Vector& _v) { *this = _v; }
    inline ~Vector(void)            { }

    inline void Reset(void) { XMP_FOR_ALL_1D(m_Vector[i] = 0.0f); }

    inline Vector& operator=  (const Vector& _v) { XMP_FOR_ALL_1D(m_Vector[i]  = _v.m_Vector[i]); return *this; }
    inline Vector& operator+= (const Vector& _v) { XMP_FOR_ALL_1D(m_Vector[i] += _v.m_Vector[i]); return *this; }
    inline Vector& operator-= (const Vector& _v) { XMP_FOR_ALL_1D(m_Vector[i] -= _v.m_Vector[i]); return *this; }
    inline Vector& operator*= (FLOAT _f)         { XMP_FOR_ALL_1D(m_Vector[i] *= _f);             return *this; }

    inline FLOAT GetLength(void)
    {
      FLOAT length = 0.0f;

      XMP_FOR_ALL_1D(length += (m_Vector[i] * m_Vector[i]));

      return sqrtf(length);
    }

    inline FLOAT DotProduct(const Vector& _v) const
    {
      FLOAT dot = 0.0f;

      XMP_FOR_ALL_1D(dot += m_Vector[i] * _v.m_Vector[i]);

      return dot;
    }

    inline       FLOAT& operator() (ULONG _i)       { return m_Vector[_i]; }
    inline const FLOAT& operator() (ULONG _i) const { return m_Vector[_i]; }

  private:

    FLOAT m_Vector[N];
  };

  template<const ULONG N>
  class Matrix
  {
  public:

    XMP_DECLARE_NEW_AND_DELETE();

    inline Matrix(void)             { Reset(); }
    inline Matrix(const Matrix& _m) { *this = _m; }
    inline ~Matrix(void)            { }

    inline void Reset(void)       { XMP_FOR_ALL_2D(m_Matrix[j][i] = 0.0f); }
    inline void SetIdentity(void) { XMP_FOR_ALL_2D(m_Matrix[j][i] = (i == j) ? 1.0f : 0.0f); }

    inline Matrix& operator=  (const Matrix& _m) { XMP_FOR_ALL_2D(m_Matrix[j][i]  = _m.m_Matrix[j][i]); return *this; }
    inline Matrix& operator+= (const Matrix& _m) { XMP_FOR_ALL_2D(m_Matrix[j][i] += _m.m_Matrix[j][i]); return *this; }
    inline Matrix& operator-= (const Matrix& _m) { XMP_FOR_ALL_2D(m_Matrix[j][i] -= _m.m_Matrix[j][i]); return *this; }
    inline Matrix& operator*= (FLOAT _f)         { XMP_FOR_ALL_2D(m_Matrix[j][i] *= _f);                return *this; }

    inline       FLOAT& operator() (ULONG _j, ULONG _i)       { return m_Matrix[_j][_i]; }
    inline const FLOAT& operator() (ULONG _j, ULONG _i) const { return m_Matrix[_j][_i]; }

  private:

    FLOAT m_Matrix[N][N];
  };

  template<const ULONG N>
  class SymmetricMatrix
  {
    static const ULONG MATRIX_SIZE = (N * (N + 1)) >> 1;

  public:

    XMP_DECLARE_NEW_AND_DELETE();

    inline SymmetricMatrix(void)                      { Reset(); }
    inline SymmetricMatrix(const SymmetricMatrix& _m) { *this = _m; }
    inline ~SymmetricMatrix(void)                     { }

    inline void Reset(void) { XMP_FOR_ALL_SM(m_Matrix[i] = 0.0f); }

    inline void SetIdentity(void)
    {
      ULONG index = 0;
      for (ULONG j = 0; j < N; ++j)
      {
        for (ULONG i = 0; i <= j; ++i)
        {
          if (i == j)
            m_Matrix[index] = 1.0f;
          else
            m_Matrix[index] = 0.0f;
          ++index;
        }
      }
    }

    inline SymmetricMatrix& operator=  (const SymmetricMatrix& _m) { XMP_FOR_ALL_SM(m_Matrix[i]  = _m.m_Matrix[i]); return *this; }
    inline SymmetricMatrix& operator+= (const SymmetricMatrix& _m) { XMP_FOR_ALL_SM(m_Matrix[i] += _m.m_Matrix[i]); return *this; }
    inline SymmetricMatrix& operator-= (const SymmetricMatrix& _m) { XMP_FOR_ALL_SM(m_Matrix[i] -= _m.m_Matrix[i]); return *this; }
    inline SymmetricMatrix& operator*= (FLOAT _f)                  { XMP_FOR_ALL_SM(m_Matrix[i] *= _f);             return *this; }

    inline void MakeMatrix(Matrix<N>& _m) const
    {
      ULONG index = 0;
      for (ULONG j = 0; j < N; ++j)
      {
        for (ULONG i = 0; i <= j; ++i)
        {
          _m(i,j) = m_Matrix[index];
          _m(j,i) = m_Matrix[index];
          ++index;
        }
      }
    }

    inline void SymmetricSubtract(const Vector<N>& _v1, const Vector<N>& _v2)
    {
      ULONG index = 0;
      for (ULONG i = 0; i < N; ++i)
      {
        for (ULONG j = 0; j <= i; ++j)
        {
          m_Matrix[index] -= _v1(i) * _v2(j);
          ++index;
        }
      }
    }

  private:

    FLOAT m_Matrix[MATRIX_SIZE];
  };

// ------------------------------------------------------------------------------------------------
// FUNCTIONS
// ------------------------------------------------------------------------------------------------
  template<const ULONG N>
  inline void XMP_NormalizeVector(Vector<N>* _v)
  {
    FLOAT len = sqrtf(_v->DotProduct(*_v));
    if (len > 0.0f)
    {
      *_v *= 1.0f / len;
    }
  }

  template<const ULONG N>
  inline void XMP_TransformVector(Vector<N>* _res, const Matrix<N>& _m, const Vector<N>& _v)
  {
    for (ULONG j = 0; j < N; ++j)
    {
      (*_res)(j) = 0.0f;

      for (ULONG i = 0; i < N; ++i)
      {
        (*_res)(j) += _m(j, i) * _v(i);
      }
    }
  }

  template<const ULONG N>
  inline void XMP_MatrixSymetricSubtract(Matrix<N>* _m, const Vector<N>& _v1, const Vector<N>& _v2)
  {
    for (ULONG i = 0; i < N; ++i)
    {
      for (ULONG j = 0; j < N; ++j)
      {
        (*_m)(i,j) -= _v1(i) * _v2(j);
      }
    }
  }

  // Gaussian elimination with partial pivoting [A|I] -> elementary line operations -> [I|A^-1]
  // Returns the determinant of _m. If other than 0, the matrix in _res is valid
  // Ref: QSlim MixKit - Micheal Garland & Paul Heckbert
  template<const ULONG N>
  inline FLOAT XMP_MatrixInverse(Matrix<N>* _res, const Matrix<N>& _m)
  {
    #define XMP_SWAP(_val1, _val2) { t = _val1; _val1 = _val2; _val2 = t; }

    Matrix<N> m(_m); // Working copy of _m since we are going to modify it
    ULONG     i, j, k;
    FLOAT     max, t, det, pivot;

    _res->SetIdentity();

    det = 1.0;
    j   = 0;

    // Forward elimination
    // -------------------
    // Eliminate in column i, below diag
    for (i = 0; i < N; ++i)
    {
      max = -1.0f;

      // Find pivot for column i
      for (k = i; k < N; ++k)
      {
        if (fabsf(m(k, i)) > max)
        {
          max = fabs(m(k, i));
          j   = k;
        }
      }

      // if no non-zero pivot we can't invert the matrix
      if (max <= 0.0f)
        return 0.0f;

      // Swap rows i and j
      if (j != i)
      {
        for (k = i; k < N; ++k)
        {
          XMP_SWAP(m(i, k), m(j, k));
        }

        for (k = 0; k < N; ++k)
        {
          XMP_SWAP((*_res)(i, k), (*_res)(j, k));
        }

        det = -det;
      }

      pivot = m(i, i);
      det  *= pivot;

      // Only do elems to right of pivot
      for (k = i + 1; k < N; ++k)
      {
        m(i, k) /= pivot;
      }

      for (k = 0; k < N; ++k)
      {
        (*_res)(i, k) /= pivot;
      }

      // We know that m(i, i) will be set to 1, so don't bother to do it

      // Eliminate in rows below i
      for (j = i + 1; j < N; ++j)
      {
        t = m(j, i);

        // Subtract scaled row i from row j, (ignore k<=i, we know they're 0)
        for (k = i + 1; k < N; ++k)
        {
          m(j, k) -= m(i, k) * t;
        }

        for (k = 0; k < N; ++k)
        {
          (*_res)(j, k) -= (*_res)(i, k) * t;
        }
      }
    }

    // Backward elimination
    // --------------------
    // Eliminate in column i, above diag
    for (i = N - 1; i > 0; --i)
    {
      // Eliminate in rows above i
      for (j = 0; j < i; ++j)
      {
        t = m(j, i);

        // Subtract scaled row i from row j
        for (k=0; k<N; k++)
        {
          (*_res)(j, k) -= (*_res)(i, k) * t;
        }
      }
    }

    return det;

    #undef XMP_SWAP
  }
};

#endif // ACTIVE_EDITORS

#endif // #ifdef GUARD_XMP_XENONMESHMATH_H
