REFCPP - Some reflection in C++ for free
========================================

REFCPP is a new way of defining C++ classes that gives up some limited-but-sometimes-enough reflection for free, in both compilation and execution time.

The following example shows an ordinary C++ class first and then its persistent version using REFCPP.

``` cpp

class Person
{
public:

    std::string getName() const { return name; }
    void setName(std::string name_) { name = name_; }

    std::string getSurname() const { return surname; }
    void setSurname(std::string surname_) { surname = surname_; }

    uint32_t getAge() const { return age; }
    void setAge(uint32_t age_) { age = age_; }
    
    std::string getFullName() const
    {
        return getName() + " " + getSurname();
    }

protected:
    std::string name;
    std::string surname;
    uint32_t age;
};

```

REFCPP version:


``` cpp

struct Name    : String {};
struct Surname : String {};
struct Age     : UInt32 {};

struct Person :
    Class< Person,
           Features< Name, Surname, Age >>
{
    std::string getFullName() const
    {
        return get< Name >() + " " + get< Surname >();
    }
};

```

Reflection in execution time is given through a hierarchy of type descriptors, defined in ref/Descriptors.hpp. These descriptors were used to implement a simple JSON serializer available in examples/json.hpp.
