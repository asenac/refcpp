REFCPP - Some reflection in C++ for free
========================================

REFCPP is a new way of defining C++ classes that gives you some limited-but-sometimes-enough reflection for free, in both compile and run time.

The following example shows an ordinary C++ class first and then its version using REFCPP.

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
// Features. Everything is a type! ;)
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

Run time reflection is given through a hierarchy of type descriptors, defined in ref/Descriptors.hpp. These descriptors were used to implement a simple JSON serializer available in examples/json.hpp.

The following example shows how to use the type descriptors API to iterate over the attributes of a class thus defined.

``` cpp
Person person;

// ...

const ClassDescriptor * classDesc = person.getClassDescriptor();
const FeatureDescriptorVector features =
    classDesc->getAllFeatureDescriptors();

for (size_t i = 0; i < features.size(); i++)
{
    const FeatureDescriptor * featureDesc = features[i];

    Holder h = featureDesc->getValue(&person);
    const TypeDescriptor * typeDesc = h.descriptor();

    if (typeDesc->getKind() == TypeDescriptor::kPrimitive)
    {
        const PrimitiveTypeDescriptor * priTypeDesc =
            typeDesc->as<PrimitiveTypeDescriptor>();
        std::cout << featureDesc->getName() << " = "
            << priTypeDesc->getString(h) << std::endl;
    }
    else
    {
        std::cout << featureDesc->getName() << " = non-printable type!" << std::endl;
    }
}
```

Compile-time reflection is given through a set of typedefs available within any class thus defined.

Inheritance
-----------

Inheritance is supported through the third parameter of Class template, although
multiple inheritance is not supported (yet?).

The following example shows how to specify a parent class.

``` cpp
struct Name    : String {};
struct Surname : String {};
struct Age     : UInt32 {};
struct Alive   : Bool {};
struct Emails  : Feature< std::vector< std::string > >{};

// Classes
struct Animal :
    Class< Animal, Features< Alive, Age > >
{
};

struct Person :
    Class< Person,
           Features< Name, Surname, Emails >,
           Animal > // Parent class
{
};
```

What can this approach be used for?
-----------------------------------
To avoid having to write anything that involves boilerplate code, such as (de)serializers.
