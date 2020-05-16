/** ***************************************************************************
	\file confined.enh.h

	\brief The file to declare class for confined counts

	Created 16 May 2020

	This file is part of Enhance.

	Copyright 2020 Harith Manoj <harithpub@gmail.com>

	Enhance is free software : you can redistribute it and /or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Enhance is distributed base the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Enhance. If not, see < https://www.gnu.org/licenses/>.

******************************************************************************/

#ifndef CONFINED_ENH_H

#define CONFINED_ENH_H

#include <functional>
#include <stdexcept>


namespace enh
{
	/**
		\brief Define an integral type that is confined to an interval.

		The interval is specified by two functions set during construction.
		The interval check is done by other function set during construction.

		<h3>template</h3>
		<code>integral</code> : The integral type of the value.
	*/
	template<class integral>
	class confined_base
	{
	public:

		/**
			\brief An alias of the integral type.
		*/
		using value_type = integral;

		/**
			\brief The function type of limit checker.
		*/
		using pred_t = std::function<bool(long long)>;

		/**
			\brief The function type of the limit getter.
		*/
		using limit_t = std::function<value_type()>;
	private:

		/**
			\brief The data.
		*/
		value_type value;

		/**
			\brief The upper limit checker.
		*/
		pred_t uLimit_pred;

		/**
			\brief The lower limit checker.
		*/
		pred_t lLimit_pred;

		/**
			\brief The upper limit.
		*/
		limit_t uLimit;
		
		/**
			\brief The lower limit.
		*/
		limit_t lLimit;
	public:
		
		/**
			\brief Construct the object.

			<h3>Exceptions</h3>
			Throws <code>std::invalid_argument</h3> if value passed fails 
			upper and lower predicate.
			Throws <code>std::invalid_argument</h3> if ulimit is less than llimit.
		*/
		constexpr inline confined_base(
			pred_t upper_p /**< : <i>in</i> : The upper bounds 
								  checker.*/,
			pred_t lower_p /**< : <i>in</i> : The lower bounds
								  checker.*/,
			limit_t upper_l /**< : <i>in</i> : Get the upper bounds.*/,
			limit_t lower_l /**< : <i>in</i> : Get the lower bounds.*/,
			value_type val /**< : <i>in</i> : The value to be set.*/
		) :uLimit_pred(upper_p), lLimit_pred(lower_p), value(val),
			uLimit(upper_l), lLimit(lower_l)
		{
			if (uLimit() < lLimit())
				throw std::invalid_argument("upper limit should be greater than lower");
			if (!uLimit_pred(val) || !lLimit_pred(val))
			{
				value = lLimit() + (uLimit() - lLimit()) / 2;
				throw std::invalid_argument("value not within limits");
			}
		}

		/**
			\brief Construct the object, value as set as lower limit.

			<h3>Exceptions</h3>
			Throws <code>std::invalid_argument</h3> if ulimit is less than llimit.
		*/
		constexpr inline confined_base(
			pred_t upper_p /**< : <i>in</i> : The upper bounds
								  checker.*/,
			pred_t lower_p /**< : <i>in</i> : The lower bounds
								  checker.*/,
			limit_t upper_l /**< : <i>in</i> : Get the upper bounds.*/,
			limit_t lower_l /**< : <i>in</i> : Get the lower bounds.*/
		) :uLimit_pred(upper_p), lLimit_pred(lower_p),
			uLimit(upper_l), lLimit(lower_l), value(0)
		{
			if (uLimit() < lLimit())
				throw std::invalid_argument("upper limit should be greater than lower");
			value = lLimit();
		}

		/**
			\brief Sets the value.

			<h3>Exception</h3>
			Throws <code>std::invalid_argument</code> if value is not within
			limits.
		*/
		constexpr inline void set(
			const value_type &val /**< : <i>in</i> : The value to be set.*/
		)
		{
			if (!uLimit_pred(val) || !lLimit_pred(val))
			{
				throw std::invalid_argument("value not within limits");
			}
			value = val;
		}

		/**
			\brief Returns the value held.
		*/
		constexpr inline value_type get() const noexcept { return value; }

		/**
			\brief Adds single unit to the value held.

			<h3>Return</h3>
			Returns 1 if value goes above upper limit and value is set to 
			lower limit.
		*/
		constexpr inline unsigned add() noexcept
		{
			++value;
			if (!uLimit_pred(value))
			{
				value = lLimit();
				return 1;
			}
			return 0;
		}

		/**
			\brief Adds to the value held.

			<h3>Return</h3>
			Returns the number of times interval was exceded if value goes 
			above upper limit.

			for confining between 2 and 9 (inclusive),
			from  value 6, add(20) will return 3 and sets value to 5.
		*/
		constexpr inline unsigned add(
			unsigned long long additional /**< : <i>in</i> : The number of units to
								  add.*/
		) noexcept
		{
			unsigned long long rem = additional % (uLimit() - lLimit());
			unsigned ret = additional / (uLimit() - lLimit());

			if (!uLimit_pred((unsigned long long)(value) + rem))
			{
				value = value + rem + lLimit() - uLimit();
				++ret;
			}
			else
				value += rem;
			return ret;
		}

		/**
			\brief Adds single unit to the value held.

			<h3>Return</h3>
			Reference to current object.
		*/
		constexpr inline confined_base<value_type> &operator ++() noexcept
		{
			add();
			return *this;
		}

		/**
			\brief Adds single unit to the value held.

			<h3>Return</h3>
			Previous state of object.
		*/
		constexpr inline confined_base<value_type> operator ++(int) noexcept
		{
			confined_base<value_type> temp = *this;
			add();
			return temp;
		}
		
		/**
			\brief Adds to current object and returns reference to the 
			current object.
		*/
		constexpr inline confined_base<value_type> &operator += (
			unsigned long long val /**< : <i>in</i> : The number of units to
								  add.*/
		)
		{
			add(val);
			return *this;
		}

		/**
			\brief Subtracts one unit from the value held.

			<h3>Return</h3>
			Returns 1 if value goes below lower limit and value is set to
			upper limit.
		*/
		constexpr inline unsigned sub() noexcept
		{
			--value;
			if (!lLimit_pred(value))
			{
				value = uLimit();
				return 1;
			}
			return 0;
		}

		/**
			\brief Subtracts from the value held.

			<h3>Return</h3>
			Returns the number of times interval was removed from if value goes
			above upper limit.

			for confining between 2 and 9 (inclusive),
			from  value 6, sub(20) will return 3 and set value to 7.
		*/
		constexpr inline unsigned sub(
			unsigned long long difference /**< : <i>in</i> : The number of units to
								 subtract.*/
		) noexcept
		{
			unsigned long long rem = difference % (uLimit() - lLimit());
			unsigned ret = difference / (uLimit() - lLimit());

			rem = value - rem;
			if (!lLimit_pred(rem))
			{
				value = rem + uLimit() - lLimit();
				++ret;
			}
			else
				value = rem;
			return ret;
		}

		/**
			\brief Subtracts single unit from the value held.

			<h3>Return</h3>
			Reference to current object.
		*/
		constexpr inline confined_base<value_type> &operator --() noexcept
		{
			sub();
			return *this;
		}

		/**
			\brief Subtracts single unit from the value held.

			<h3>Return</h3>
			Previous state of object.
		*/
		constexpr inline confined_base<value_type> operator --(int) noexcept
		{
			confined_base<value_type> temp = *this;
			sub();
			return temp;
		}

		/**
			\brief Subtracts from the current object and returns reference 
			to the current object.
		*/
		constexpr inline confined_base<value_type> &operator -= (
			unsigned long long val /**< : <i>in</i> : The number of units to
								  subtract.*/
		)
		{
			sub(val);
			return *this;
		}

		/**
			\brief Re-evaluates value according to predicate if anything 
			changed.

			Sets value to ulimit if exceeding upper, llimit if lower then lower limit.

			This is used if the predicate changes according to other values,

			Like if the predicate depends on a global variable or if it is a 
			lambda that captures by reference.

			<h3>Return</h3>
			Returns true if any change happend.
		*/
		constexpr inline bool re_eval() noexcept
		{
			if (!uLimit_pred(value))
			{
				value = uLimit();
				return true;
			}
			if (!lLimit_pred(value))
			{
				value = lLimit();
				return true;
			}
			return false;
		}

	};

	/**
		\brief Adds rhs to a copy of lhs then returns the sum. 
	*/
	template<class integral>
	constexpr inline confined_base<integral> operator +(
		const confined_base<integral> &lhs /**< : <i>in</i> : LHS argument 
										   of operator.*/,
		const unsigned long long &rhs /**< : <i>in</i> : RHS argument of operator.*/
	) noexcept
	{
		return confined_base<integral>(lhs).add(rhs);
	}

	/**
		\brief Adds lhs to a copy of rhs then returns the sum.
	*/
	template<class integral>
	constexpr inline confined_base<integral> operator +(
		const unsigned long long &lhs  /**< : <i>in</i> : LHS argument of operator.*/,
		const confined_base<integral> &rhs /**< : <i>in</i> : RHS argument
										   of operator.*/
	) noexcept
	{
		return confined_base<integral>(rhs).add(lhs);
	}

	/**
		\brief Subtracts rhs to a copy of lhs then returns the difference.
	*/
	template<class integral>
	constexpr inline confined_base<integral> operator -(
		const confined_base<integral> &lhs /**< : <i>in</i> : LHS argument
										   of operator.*/,
		const unsigned long long &rhs /**< : <i>in</i> : RHS argument of operator.*/
		) noexcept
	{
		return confined_base<integral>(lhs).sub(rhs);
	}

	/**
		\brief Subtracts lhs to a copy of rhs then returns the difference.
	*/
	template<class integral>
	constexpr inline confined_base<integral> operator -(
		const unsigned long long &lhs  /**< : <i>in</i> : LHS argument of operator.*/,
		const confined_base<integral> &rhs /**< : <i>in</i> : RHS argument
										   of operator.*/
		) noexcept
	{
		return confined_base<integral>(rhs).sub(lhs);
	}
}



#endif