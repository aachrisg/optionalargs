# optionalargs
Single header file for implementing position parameters for functions and classes in c++


The header can be used to provide rich interfaces for class and function definitions that would like to accept a wide variety of arguments, with defaults.
Using c++'s optional/default argument functionality rapidly produces unreadable/unusable code, which this technique fixes.

For an article on the technique and why you would use this header, see my blog post at https://chrisgreendevelopmentblog.wordpress.com/2021/11/21/c-non-positional-parameters-for-rich-interfaces/

Before:
```
   MyContainer<float, 256, 1024, 64> numbers;
   float flRoot = Solve( []( float x ){ return cos( x ); }, .1, true, false, {}, 1024 );
```
After:
```
  MyContainer<float, opt::InitialAllocation( 256 ), opt::MaxAllocation( 1024 ), opt::GrowSize( 64 ) > numbers;
  float flRoot = Solve( []( float x ){ return cos( x ); }, opt::Tolerance( .1 ), opt::MaxIters( 1024 ) );
```
  This file has no dependencies other than a c++20 compiler. Tested with GCC and MSVC.
  

  
