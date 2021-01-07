// header.hpp

// 123
    [[clang::annotate("123")]]
class /*aaaaaaaa*/ MyClass // sdfsdf
/// fsdfsd
{
public:
    [[clang::annotate("zzz")]]
  int field;
  virtual void method() const = 0;

  static const int static_field;
    [[clang::annotate("vvvvvvvvv")]]
  static int static_method();
};


struct A{};
