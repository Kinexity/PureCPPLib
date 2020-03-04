#pragma once
#ifndef adv_m
#define adv_m
#include <vector>
#include <variant>
#include <cmath>
#include <string>

namespace PCL {
	namespace math {
		enum class side {
			left, right
		};

		template <typename Type, side bounded_side, bool belongs>
		class interval_bound {
			template <typename Type, bool left_belongs, bool right_belongs>
			friend class interval;
		private:
			interval_bound(Type bound_number) noexcept;
			const Type
				bound;
		public:
			~interval_bound() = default;
			bool
				operator==(Type number_to_check) noexcept;
			template <typename Type_comp, bool belongs_comp>
			bool
				operator==(interval_bound<Type_comp, bounded_side, belongs_comp> number_to_check);
		};

		template <typename Type, side bounded_side, bool belongs>
		inline interval_bound<Type, bounded_side, belongs>::interval_bound(Type bound_number) noexcept :
			bound(bound_number) {}

		template<typename Type, side bounded_side, bool belongs>
		inline bool interval_bound<Type, bounded_side, belongs>::operator==(Type number_to_check) noexcept {
			if constexpr (bounded_side == side::left) {
				if constexpr (belongs) {
					return number_to_check >= bound;
				}
				else {
					return number_to_check > bound;
				}
			}
			else {
				if constexpr (belongs) {
					return number_to_check <= bound;
				}
				else {
					return number_to_check < bound;
				}
			}
		}

		template <typename Type, bool left_belongs, bool right_belongs>
		class interval {
			template <typename Type>
			friend class real_set;
		private:
			interval_bound<Type, side::left, left_belongs>
				left;
			interval_bound<Type, side::right, right_belongs>
				right;
		public:
			interval(Type left_bound, Type right_bound);
			bool
				operator==(Type number_to_check);
			template <typename Type_comp, bool left_belongs_comp, bool right_belongs_comp>
			bool
				operator==(interval<Type_comp, left_belongs_comp, right_belongs_comp> interval_to_compare);
			operator std::string() {
				return std::string(left_belongs ? "<" : "(") + std::to_string(left.bound) + ";" + std::to_string(right.bound) + (right_belongs ? ">" : ")");
			};
		};

		template<typename Type, bool left_belongs, bool right_belongs>
		inline interval<Type, left_belongs, right_belongs>::interval(Type left_bound, Type right_bound) :
			left(left_bound),
			right(right_bound) {}

		template<typename Type, bool left_belongs, bool right_belongs>
		inline bool interval<Type, left_belongs, right_belongs>::operator==(Type number_to_check) {
			return left == number_to_check && right == number_to_check;
		}

		template <typename Type>
		class real_set {
		private:
			std::vector<
				std::variant<
				interval<Type, true, true>,
				interval<Type, true, false>,
				interval<Type, false, true>,
				interval<Type, false, false>,
				interval_bound<Type, side::left, true>,
				interval_bound<Type, side::left, false>,
				interval_bound<Type, side::right, true>,
				interval_bound<Type, side::right, false>,
				Type>
			>
				set_elements;
		public:
		};

		template<typename Type, bool left_belongs, bool right_belongs>
		template<typename Type_comp, bool left_belongs_comp, bool right_belongs_comp>
		inline bool interval<Type, left_belongs, right_belongs>::operator==(interval<Type_comp, left_belongs_comp, right_belongs_comp> interval_to_compare) {
			return left_belongs == left_belongs_comp && right_belongs == right_belongs_comp && left == interval_to_compare.left && right == interval_to_compare.right;
		}

		template<typename Type, side bounded_side, bool belongs>
		template<typename Type_comp, bool belongs_comp>
		inline bool interval_bound<Type, bounded_side, belongs>::operator==(interval_bound<Type_comp, bounded_side, belongs_comp> interval_bound_to_compare) {
			return belongs == belongs_comp && interval_bound_to_compare.bound == bound;
		}
	}
}

#endif // !adv_m