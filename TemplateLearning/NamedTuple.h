// Named tuple for C++
// Example code from http://vitiy.info/
// Written by Victor Laskin (victor.laskin@gmail.com)

// Parts of code were taken from: https://gist.github.com/Manu343726/081512c43814d098fe4b
#include <tuple>
#include <stdint.h>

namespace foonathan {
	namespace string_id {
		namespace detail
		{
			using hash_type = uint64_t;

			constexpr hash_type fnv_basis = 14695981039346656037ull;
			constexpr hash_type fnv_prime = 109951162821ull;

			// FNV-1a 64 bit hash
			constexpr hash_type sid_hash(const char* str, hash_type hash = fnv_basis) noexcept
			{
				return *str ? sid_hash(str + 1, (hash ^ *str) * fnv_prime) : hash;
			}
		}
	}
} // foonathan::string_id::detail

namespace fn_detail {
	/// Named parameter (could be empty!)
	template <typename Hash, typename... Ts>
	struct named_param : public std::tuple<std::decay_t<Ts>...> 
	{
		using hash = Hash;                                                              ///< key
		named_param(Ts&& ... ts)
			: std::tuple<std::decay_t<Ts>...>(std::forward<Ts>(ts)...)
		{ };        ///< constructor

		template <typename P>
		named_param<Hash, P> operator=(P&& p) 
		{ 
			return named_param<Hash, P>(std::forward<P>(p)); 
		};
	};

	template <typename Hash>
	using make_named_param = named_param<Hash>;
	
	/// Named tuple is just tuple of named params
	template <typename... Params>
	struct named_tuple : public std::tuple<Params...>
	{
		template <typename... Args>
		named_tuple(Args&& ... args)
			: std::tuple<Args...>(std::forward<Args>(args)...)
		{ }

		static const std::size_t error = -1;

		template<std::size_t I = 0, typename Hash>
		constexpr typename std::enable_if<I == sizeof...(Params), const std::size_t>::type
			static get_element_index()
		{
			return error;
		}

		template<std::size_t I = 0, typename Hash>
		constexpr typename std::enable_if < I < sizeof...(Params), const std::size_t>::type
			static get_element_index()
		{
			using elementType = typename std::tuple_element<I, std::tuple<Params...>>::type;
			//return (typeid(typename elementType::hash) == typeid(Hash)) ? I : get_element_index<I + 1, Hash>();
			return (std::is_same<typename elementType::hash, Hash>::value) ? I : get_element_index<I + 1, Hash>();
		}

		template<typename Hash>
		const auto& get() const
		{
			constexpr std::size_t index = get_element_index<0, Hash>();
			static_assert((index != error), "Wrong named tuple key");
			auto & param = (std::get< index >(static_cast<const std::tuple<Params...>&>(*this)));
			return std::get<0>(param);
		}

		template<typename NP>
		const auto& operator[](NP && param)
		{
			return get<typename NP::hash>();
		}
	};
}

template <typename... Args>
auto make_named_tuple(Args&& ... args)
{
	return fn_detail::named_tuple<Args...>(std::forward<Args>(args)...);
}

#define param(x) fn_detail::make_named_param< std::integral_constant<foonathan::string_id::detail::hash_type, foonathan::string_id::detail::sid_hash(x)> >{}