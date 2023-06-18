#pragma once

#include <algorithm>

template<size_t N>
struct CompileTimeStringLiteral_t
//< Allows us to template by a string constant
{
    constexpr CompileTimeStringLiteral_t( const char ( &str )[N])
	{
        std::copy_n(str, N, m_value);
    }
    
    char m_value[N];
};


template<class ValueType_t, CompileTimeStringLiteral_t filenameTag, int lineNumberTag>
struct OptionalArg_t
{
	ValueType_t m_value{};
};

template<class ValueType_t, auto defaultValue, CompileTimeStringLiteral_t filenameTag, int lineNumberTag>
struct OptionalArgWithDefault_t
{
	ValueType_t m_value = defaultValue;
};

/// These macros define option types, using the filename + line number to uniquify the type generated.

#define DECLARE_OPTION( optionName, ValueType_t )						\
	using optionName = OptionalArg_t<ValueType_t, __FILE__, __LINE__>;

#define DECLARE_OPTION_DEFAULT( optionName, ValueType_t, defaultValue )		\
	using optionName = OptionalArgWithDefault_t<ValueType_t, defaultValue, __FILE__, __LINE__>;


//: Search functions for parameter packs:
template<class T>
inline constexpr auto GetOptionValue()
{
	T tmp;
	return tmp.m_value;
}

template<typename T, typename FirstArgType_t, typename ... restTypes>
inline constexpr auto GetOptionValue( FirstArgType_t arg, restTypes... restArgs )
{
	if constexpr( std::is_same_v<FirstArgType_t, T> )
	{
		return arg.m_value;								// for options that wrap the value in a trivial struct
	}
	else
	{
		return GetOptionValue<T>( restArgs... );
	}
}

template<class T, class FirstArgType_t>
inline constexpr auto GetOptionValue( FirstArgType_t arg )
{
	if constexpr( std::is_same_v<FirstArgType_t, T> )
	{
		return arg.m_value;
	}
	else
	{
		T tmp;
		return tmp.m_value;
	}
}

//: Gets w/ provided default value for when the option isn't present:
template<class T>
inline constexpr auto GetOptionValueWithDefault( auto defaultValue )
{
	return defaultValue;
}

template<typename T, typename FirstArgType_t, typename ... restTypes, class ValueType_t>
inline constexpr ValueType_t GetOptionValueWithDefault( ValueType_t defaultValue, FirstArgType_t arg, restTypes... restArgs )
{
	if constexpr( std::is_same_v<FirstArgType_t, T> )
	{
		if constexpr ( std::is_convertible_v<FirstArgType_t, ValueType_t> ) // for scalar options like enums
		{
			return arg;
		}
		else
		{
			return arg.m_value;								// for options that wrap the value in a trivial struct
		}
	}
	else
	{
		return GetOptionValueWithDefault<T>( defaultValue, restArgs... );
	}
}

template<class T, class FirstArgType_t, class ValueType_t>
inline constexpr ValueType_t GetOptionValueWithDefault( ValueType_t defaultValue, FirstArgType_t arg )
{
	if constexpr( std::is_same_v<FirstArgType_t, T> )
	{
		return arg.m_value;
	}
	else
	{
		return defaultValue;
	}
}

#ifdef OPTIONALARGS_SAMPLE_TEST_CODE
// Sample usage and compilation test

namespace opt
//< Use a namespace for option types so as not to pollute the clobal space with short names
{
	DECLARE_OPTION_DEFAULT( ItemCount, int, 256 );
	DECLARE_OPTION_DEFAULT( VerboseLogs, bool, false );
};


//: Example of a function taking named options:

template< typename ...optionTypes_t>
void TestFunction( char const *pString, optionTypes_t... options )
{
	Log( "TestFunction ", pString );
	int nItemCount = GetOptionValue<opt::ItemCount>( options...);
	if ( GetOptionValue<opt::VerboseLogs>( options...) )
	{
		Log( "Item count=%d", nItemCount );
	}
}

//: Example of a class with a constructor taking named options:

class TestClass
{
public:
	template< typename ...optionTypes_t>
	TestClass( char const *pMsg, optionTypes_t... options )
	{
		// We will call the SetOption method with each optional arg instead of using GetOptionValue. This gets you compile-time type checking for unsupported args
		Log( "TestClass", pMsg );
		( SetOption( options), ...);						// fold expression. Calls our SetItem with each option
		m_pData.reset( new int[m_nNumItems] );
		if ( m_bVerboseLogs )
		{
			Log( "item count=%d", m_nNumItems );
		}
	}

	void SetOption( opt::ItemCount x ) { m_nNumItems = x.m_value; }
	void SetOption( opt::VerboseLogs x ) { m_bVerboseLogs = x.m_value; }
	int m_nNumItems = 101;
	std::unique_ptr<int[]> m_pData;
	bool m_bVerboseLogs = false;
};

//: Example of a class with named template arguments:

template<auto... options>
class TestTemplate
{
public:
	int m_nData[GetOptionValue<opt::ItemCount>( options...)];
	bool m_bVerboseLogs = GetOptionValue<opt::VerboseLogs>( options... );

	TestTemplate()
	{
		Log( "TestTemplate" );
		if constexpr( GetOptionValue<opt::VerboseLogs>( options... ) )
		{
			Log( "data size=%?", sizeof( m_nData ) );
		}
	}
};

inline void TestFunctionality()
{
	TestFunction( "no args" );
	TestFunction( "itemcount=50", opt::ItemCount( 50 ) );
	TestFunction( "both args", opt::VerboseLogs(), opt::ItemCount( 100 ) );

	TestTemplate x;
	TestTemplate<opt::VerboseLogs( true )> x1;
	TestTemplate<opt::ItemCount( 50 ), opt::VerboseLogs( true)> x2;

	TestClass yy( "no args" );;
	TestClass y( "vblogs arg", opt::VerboseLogs( true) );
	TestClass y2( "both args", opt::VerboseLogs( true), opt::ItemCount( 1024 ) );
}

#endif

	
	
