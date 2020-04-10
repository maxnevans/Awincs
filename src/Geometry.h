#pragma once
#include "base/DebugUntils.h"

namespace Awincs
{
	namespace Geometry
	{
		template<typename T>
		struct Point2D
		{
			T x;
			T y;
		};

		using IntPoint2D = Point2D<int>;


		template<typename T>
		struct Dimensions2D
		{
			T width;
			T height;

			Dimensions2D<T>& operator-=(const Dimensions2D<T>& p)
			{
				width -= p.width;
				height -= p.height;

				expect(width >= 0);
				expect(height >= 0);

				return *this;
			}

			Dimensions2D<T>& operator+=(const Dimensions2D<T>& p)
			{
				width += p.width;
				height += p.height;

				expect(width >= 0);
				expect(height >= 0);

				return *this;
			}

			template<typename Number>
			Dimensions2D<T>& operator+=(Number n)
			{

				width += n;
				height += n;

				expect(width >= 0);
				expect(height >= 0);

				return *this;
			}

			template<typename Number>
			Dimensions2D<T>& operator-=(Number n)
			{
				width -= n;
				height -= n;

				expect(width >= 0);
				expect(height >= 0);

				return *this;
			}

			template<typename Number>
			Dimensions2D<T>& operator*=(Number n)
			{
				expect(n >= 0);

				width *= n;
				height *= n;

				return *this;
			}

			template<typename Number>
			Dimensions2D<T>& operator/=(Number n)
			{
				expect(n > 0);

				width /= n;
				height /= n;

				return *this;
			}

			template<typename Number>
			Dimensions2D<T> operator+(Number n) const
			{
				Dimensions2D<T> d = {width + n, height + n};

				expect(d.width >= 0);
				expect(d.height >= 0);

				return d;
			}

			template<typename Number>
			Dimensions2D<T> operator-(Number n) const
			{
				Dimensions2D<T> d = {width - n, height - n};

				expect(d.width >= 0);
				expect(d.height >= 0);

				return d;
			}

			template<typename Number>
			Dimensions2D<T> operator*(Number n) const
			{
				expect(n >= 0);
				return { width * n, height * n };
			}

			template<typename Number>
			Dimensions2D<T> operator/(Number n) const
			{
				expect(n > 0);
				return { width / n, height / n };
			}
		};

		using IntDimensions2D = Dimensions2D<int>;

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
			Rectangle(const Point2D<T>& anchorPoint, const Dimensions2D<T> dimensions)
				:
				anchorPoint(anchorPoint), 
				dimensions(dimensions)
			{
				expect(dimensions.width >= 0);
				expect(dimensions.height >= 0);
			}

			static bool checkAffiliationIgnoreChildren(const Point2D<T>& anchorPoint, const Dimensions2D<T> dimensions, const Point2D<T>& pt)
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

			inline void setDimensions(const Dimensions2D<T> dims)
			{
				dimensions = dims;
			}

		private:
			Point2D<T> anchorPoint		= {};
			Dimensions2D<T> dimensions	= {};
		};

		using IntRectangle = Rectangle<int>;
	}
}