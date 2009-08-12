#if !defined(INCLUDED_VECTOR_H)
#define INCLUDED_VECTOR_H

#include <cstddef>
#include <math.h>

template<typename Element>
class BasicVector2
{
		Element m_elements[2];
	public:
		BasicVector2 ()
		{
		}
		BasicVector2 (const Element& x_, const Element& y_)
		{
			x() = x_;
			y() = y_;
		}

		Element& x ()
		{
			return m_elements[0];
		}
		const Element& x () const
		{
			return m_elements[0];
		}
		Element& y ()
		{
			return m_elements[1];
		}
		const Element& y () const
		{
			return m_elements[1];
		}

		const Element& operator[] (std::size_t i) const
		{
			return m_elements[i];
		}
		Element& operator[] (std::size_t i)
		{
			return m_elements[i];
		}

		Element* data ()
		{
			return m_elements;
		}
		const Element* data () const
		{
			return m_elements;
		}
};

/// \brief A 3-element vector.
template<typename Element>
class BasicVector3
{
		Element m_elements[3];
	public:

		BasicVector3 ()
		{
		}
		template<typename OtherElement>
		BasicVector3 (const BasicVector3<OtherElement>& other)
		{
			x() = static_cast<Element> (other.x());
			y() = static_cast<Element> (other.y());
			z() = static_cast<Element> (other.z());
		}
		BasicVector3 (const Element& x_, const Element& y_, const Element& z_)
		{
			x() = x_;
			y() = y_;
			z() = z_;
		}

		/** Construct a BasicVector3 from a 3-element array. The array must be
		 * valid as no bounds checking is done.
		 */
		BasicVector3 (const Element* array)
		{
			for (int i = 0; i < 3; ++i)
				m_elements[i] = array[i];
		}

		Element& x ()
		{
			return m_elements[0];
		}
		const Element& x () const
		{
			return m_elements[0];
		}
		Element& y ()
		{
			return m_elements[1];
		}
		const Element& y () const
		{
			return m_elements[1];
		}
		Element& z ()
		{
			return m_elements[2];
		}
		const Element& z () const
		{
			return m_elements[2];
		}

		const Element& operator[] (std::size_t i) const
		{
			return m_elements[i];
		}
		Element& operator[] (std::size_t i)
		{
			return m_elements[i];
		}

		Element* data ()
		{
			return m_elements;
		}
		const Element* data () const
		{
			return m_elements;
		}

		/**
		 * Return the length of this vector.
		 *
		 * @return The Pythagorean length of this vector.
		 */
		double getLength () const
		{
			const double lenSquared = getLengthSquared();
			return sqrt(lenSquared);
		}

		/**
		 * Return the squared length of this vector.
		 */
		double getLengthSquared () const
		{
			const double lenSquared = double(m_elements[0]) * double(m_elements[0]) + double(m_elements[1])
					* double(m_elements[1]) + double(m_elements[2]) * double(m_elements[2]);
			return lenSquared;
		}

		/**
		 * Normalise this vector in-place by scaling by the inverse of its size.
		 */
		void normalise ()
		{
			const double inverseLength = 1 / getLength();
			m_elements[0] *= inverseLength;
			m_elements[1] *= inverseLength;
			m_elements[2] *= inverseLength;
		}

		// Return a new BasicVector3 equivalent to this BasicVector3
		// scaled by a constant amount
		BasicVector3<Element> getScaledBy (double scale) const
		{
			return BasicVector3<Element> (x() * scale, y() * scale, z() * scale);
		}

		// Return a new BasicVector3 equivalent to the normalised
		// version of this BasicVector3 (scaled by the inverse of its size)
		BasicVector3<Element> getNormalised () const
		{
			return getScaledBy(1.0 / getLength());
		}

		/* Cross-product this vector with another Vector3, returning the result
		 * in a new Vector3.
		 *
		 * @param other
		 * The Vector3 to cross-product with this Vector3.
		 *
		 * @returns
		 * The cross-product of the two vectors.
		 */
		template<typename OtherT>
		BasicVector3<Element> crossProduct (const BasicVector3<OtherT>& other)
		{
			return BasicVector3<Element> (y() * other.z() - z() * other.y(), z() * other.x() - x() * other.z(), x()
					* other.y() - y() * other.x());
		}
};

/// \brief A 4-element vector.
template<typename Element>
class BasicVector4
{
		Element m_elements[4];
	public:

		BasicVector4 ()
		{
		}
		BasicVector4 (Element x_, Element y_, Element z_, Element w_)
		{
			x() = x_;
			y() = y_;
			z() = z_;
			w() = w_;
		}
		BasicVector4 (const BasicVector3<Element>& self, Element w_)
		{
			x() = self.x();
			y() = self.y();
			z() = self.z();
			w() = w_;
		}

		Element& x ()
		{
			return m_elements[0];
		}
		const Element& x () const
		{
			return m_elements[0];
		}
		Element& y ()
		{
			return m_elements[1];
		}
		const Element& y () const
		{
			return m_elements[1];
		}
		Element& z ()
		{
			return m_elements[2];
		}
		const Element& z () const
		{
			return m_elements[2];
		}
		Element& w ()
		{
			return m_elements[3];
		}
		const Element& w () const
		{
			return m_elements[3];
		}

		Element index (std::size_t i) const
		{
			return m_elements[i];
		}
		Element& index (std::size_t i)
		{
			return m_elements[i];
		}
		Element operator[] (std::size_t i) const
		{
			return m_elements[i];
		}
		Element& operator[] (std::size_t i)
		{
			return m_elements[i];
		}

		Element* data ()
		{
			return m_elements;
		}
		const Element* data () const
		{
			return m_elements;
		}
};

template<typename Element>
inline BasicVector3<Element> vector3_from_array (const Element* array)
{
	return BasicVector3<Element> (array[0], array[1], array[2]);
}

template<typename Element>
inline Element* vector3_to_array (BasicVector3<Element>& self)
{
	return self.data();
}
template<typename Element>
inline const Element* vector3_to_array (const BasicVector3<Element>& self)
{
	return self.data();
}

template<typename Element>
inline Element* vector4_to_array (BasicVector4<Element>& self)
{
	return self.data();
}
template<typename Element>
inline const Element* vector4_to_array (const BasicVector4<Element>& self)
{
	return self.data();
}

template<typename Element>
inline BasicVector3<Element>& vector4_to_vector3 (BasicVector4<Element>& self)
{
	return *reinterpret_cast<BasicVector3<Element>*> (vector4_to_array(self));
}
template<typename Element>
inline const BasicVector3<Element>& vector4_to_vector3 (const BasicVector4<Element>& self)
{
	return *reinterpret_cast<const BasicVector3<Element>*> (vector4_to_array(self));
}

/// \brief A 2-element vector stored in single-precision floating-point.
typedef BasicVector2<float> Vector2;

/// \brief A 3-element vector stored in single-precision floating-point.
typedef BasicVector3<float> Vector3;

/// \brief A 4-element vector stored in single-precision floating-point.
typedef BasicVector4<float> Vector4;

#endif
