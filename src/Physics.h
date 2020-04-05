#pragma once
#include <utility>

namespace AWC
{
	namespace Physics
	{
		template<typename T>
		using Point2D = std::pair<T, T>;
		using IntPoint2D = Point2D<int>;
		static const int X = 0;
		static const int Y = 1;

		template<typename T>
		T getX(Point2D<T> point)
		{
			return std::get<X>(point);
		}

		template<typename T>
		T getY(Point2D<T> point)
		{
			return std::get<Y>(point);
		}


		template<typename T>
		using Dimensions2D = std::pair<T, T>;
		using IntDimensions2D = Dimensions2D<int>;
		static const int WIDTH = 0;
		static const int HEIGHT = 1;

		template<typename T>
		T getWidth(Dimensions2D<T> dimension)
		{
			return std::get<WIDTH>(dimension);
		}

		template<typename T>
		T getHeight(Dimensions2D<T> dimension)
		{
			return std::get<HEIGHT>(dimension);
		}
	}
}
