REFCPP - Some reflection in C++ for free
========================================

REFCPP is a new way of defining C++ classes that gives up some limited-but-sometimes-enough reflection for free, in both compile and run time.

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
            static_cast< const PrimitiveTypeDescriptor * >(typeDesc);
        std::cout << featureDesc->getName() << " = "
            << priTypeDesc->getString(h) << std::endl;
    }
    else
    {
        std::cout << featureDesc->getName() << " = non-printable type!" << std::endl;
    }
}
```

Compile-time is given through a set of typedefs available within any class thus defined.


What can this approach be used for?
-----------------------------------
To avoid having to write anything that involves boilterplate code, such as (de)serializers. 
