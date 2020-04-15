#pragma once
#include "base/DebugUntils.h"

namespace Awincs
{
	namespace Geometry
	{
		template<typename T>
		struct Dimensions2D
		{
			T width;
			T height;

			inline bool operator==(const Dimensions2D& p) const
			{
				return (p.width == width) && (p.height == height);
			}

			inline bool operator!=(const Dimensions2D& p) const
			{
				return (p.width != width) || (p.height != height);
			}

			inline Dimensions2D& operator-=(const Dimensions2D& p)
			{
				width -= p.width;
				height -= p.height;

				expect(width >= 0);
				expect(height >= 0);

				return *this;
			}

			inline Dimensions2D& operator+=(const Dimensions2D& p)
			{
				width += p.width;
				height += p.height;

				expect(width >= 0);
				expect(height >= 0);

				return *this;
			}

			template<typename Number>
			inline Dimensions2D& operator+=(Number n)
			{

				width += n;
				height += n;

				expect(width >= 0);
				expect(height >= 0);

				return *this;
			}

			template<typename Number>
			inline Dimensions2D& operator-=(Number n)
			{
				width -= n;
				height -= n;

				expect(width >= 0);
				expect(height >= 0);

				return *this;
			}

			template<typename Number>
			inline Dimensions2D& operator*=(Number n)
			{
				expect(n >= 0);

				width *= n;
				height *= n;

				return *this;
			}

			template<typename Number>
			inline Dimensions2D& operator/=(Number n)
			{
				expect(n > 0);

				width /= n;
				height /= n;

				return *this;
			}

			template<typename Number>
			inline Dimensions2D operator+(Number n) const
			{
				Dimensions2D<T> d = { width + n, height + n };

				expect(d.width >= 0);
				expect(d.height >= 0);

				return d;
			}

			template<typename Number>
			inline Dimensions2D operator-(Number n) const
			{
				Dimensions2D d = { width - n, height - n };

				expect(d.width >= 0);
				expect(d.height >= 0);

				return d;
			}

			template<typename Number>
			inline Dimensions2D operator*(Number n) const
			{
				expect(n >= 0);
				return { width * n, height * n };
			}

			template<typename Number>
			inline Dimensions2D operator/(Number n) const
			{
				expect(n > 0);
				return { width / n, height / n };
			}
		};

		using IntDimensions2D = Dimensions2D<int>;

		template<typename T>
		struct Point2D
		{
			T x;
			T y;

			inline bool operator==(const Point2D& p) const
			{
				return (p.x == x) && (p.y == y);
			}

			inline bool operator!=(const Point2D& p) const
			{
				return !((p.x == x) && (p.y == y));
			}

			inline Point2D& operator+=(const Point2D& p)
			{
				x += p.x;
				y += p.y;
				return *this;
			}

			inline Point2D& operator-=(const Point2D& p)
			{
				x -= p.x;
				y -= p.y;
				return *this;
			}

			inline Point2D operator+(const Point2D& p) const
			{
				return { x + p.x, y + p.y };
			}

			inline Point2D operator-(const Point2D& p) const
			{
				return { x - p.x, y - p.y };
			}

			template<typename Number>
			inline Point2D operator+(Number n) const
			{
				return { x + n, y + n};
			}

			template<typename Number>
			inline Point2D operator-(Number n) const
			{
				return { x - n, y - n };
			}

			template<typename Number>
			inline Point2D operator*(Number n) const
			{
				return { x * n, y * n };
			}

			template<typename Number>
			inline Point2D operator/(Number n) const
			{
				expect(n != 0);
				return { x / n, y / n };
			}

			inline Point2D operator+(const Dimensions2D<T>& d) const
			{
				return { x + d.width, y + d.height };
			}
		};

		using IntPoint2D = Point2D<int>;

		template<typename T>
		class GeometryShape
		{
		public:
			virtual bool checkAffiliationIgnoreChildren(const Point2D<T>& pt) const = 0;
		};
		
		template<typename T>
		class Rectangle
			:
			public GeometryShape<T>
		{
		public:
			Rectangle() = default;

			Rectangle(const Point2D<T>& p1, const Point2D<T>& p2)
			{
				expect(p1.x <= p2.x);
				expect(p1.y <= p2.y);

				anchorPoint = p1;
				auto pDiff = p2 - p1;
				dimensions = Dimensions2D<T>{pDiff.x, pDiff.y};
			}

			Rectangle(const Point2D<T>& anchorPoint, const Dimensions2D<T>& dimensions)
				:
				anchorPoint(anchorPoint), 
				dimensions(dimensions)
			{
				expect(dimensions.width >= 0);
				expect(dimensions.height >= 0);
			}

			static bool checkAffiliationIgnoreChildren(const Point2D<T>& anchorPoint, const Dimensions2D<T>& dimensions, const Point2D<T>& pt)
			{
				auto [x, y] = anchorPoint;
				auto [width, height] = dimensions;

				return (pt.x >= x) && (pt.x < x + width)
					&& (pt.y >= y) && (pt.y < y + height);
			}

			virtual bool checkAffiliationIgnoreChildren(const Point2D<T>& pt) const
			{
				return Rectangle::checkAffiliationIgnoreChildren(anchorPoint, dimensions, pt);
			}
			
			inline const Point2D<T>& getAnchorPoint() const
			{
				return anchorPoint;
			}

			inline const Dimensions2D<T>& getDimensions() const
			{
				return dimensions;
			}

			inline void setAnchorPoint(const Point2D<T>& pt)
			{
				anchorPoint = pt;
			}

			inline void setDimensions(const Dimensions2D<T>& dims)
			{
				dimensions = dims;
			}

			inline bool operator==(const Rectangle& r) const
			{
				return (r.anchorPoint == anchorPoint) && (r.dimensions == dimensions);
			}

			inline bool operator!=(const Rectangle& r) const
			{
				return (r.anchorPoint != anchorPoint) || (r.dimensions != dimensions);
			}

		private:
			Point2D<T> anchorPoint		= {};
			Dimensions2D<T> dimensions	= {};
		};

		using IntRectangle = Rectangle<int>;
	}
}