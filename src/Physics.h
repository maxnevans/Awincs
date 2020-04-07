#pragma once
namespace AWC
{
	namespace Physics
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
		};

		using IntDimensions2D = Dimensions2D<int>;
	}
}
