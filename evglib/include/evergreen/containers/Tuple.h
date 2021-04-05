#pragma once

namespace evg
{
	namespace detail
	{
	template <typename T> class TupleElementFirst { public: T first; };
	template <typename T> class TupleElementSecond { public: T second; };
	template <typename T> class TupleElementThird { public: T third; };
	template <typename T> class TupleElementFourth { public: T fourth; };
	template <typename T> class TupleElementFifth { public: T fifth; };
	template <typename T> class TupleElementSixth { public: T sixth; };
	template <typename T> class TupleElementSeventh { public: T seventh; };
	template <typename T> class TupleElementEighth { public: T eighth; };

	// 1-20 placeholder classes
	class Empty1 {}; class Empty2 {}; class Empty3 {}; class Empty4 {}; class Empty5 {}; class Empty6 {}; class Empty7 {}; class Empty8 {}; class Empty9 {}; class Empty10 {}; class Empty11 {}; class Empty12 {}; class Empty13 {}; class Empty14 {}; class Empty15 {}; class Empty16 {}; class Empty17 {}; class Empty18 {}; class Empty19 {}; class Empty20 {};
	}
	
	template<typename FirstT = void, typename SecondT = void, typename ThirdT = void, typename FourthT = void,
		typename FifthT = void, typename SixthT = void, typename SeventhT = void, typename EighthT = void>
		class Tuple :
		public std::conditional<std::is_same<FirstT, void>::value, detail::Empty1, detail::TupleElementFirst<FirstT>>::type,
		public std::conditional<std::is_same<SecondT, void>::value, detail::Empty2, detail::TupleElementSecond<SecondT>>::type,
		public std::conditional<std::is_same<ThirdT, void>::value, detail::Empty3, detail::TupleElementThird<ThirdT>>::type,
		public std::conditional<std::is_same<FourthT, void>::value, detail::Empty4, detail::TupleElementFourth<FourthT>>::type,
		public std::conditional<std::is_same<FifthT, void>::value, detail::Empty5, detail::TupleElementFifth<FifthT>>::type,
		public std::conditional<std::is_same<SixthT, void>::value, detail::Empty6, detail::TupleElementSixth<SixthT>>::type,
		public std::conditional<std::is_same<SeventhT, void>::value, detail::Empty7, detail::TupleElementSeventh<SeventhT>>::type,
		public std::conditional<std::is_same<EighthT, void>::value, detail::Empty8, detail::TupleElementEighth<EighthT>>::type
	{

	};
}