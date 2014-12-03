#include <iostream>
#include <ref/Class.hpp>
#include <ref/DescriptorsImpl.ipp>
#include <ref/utils/JsonSerializer.hpp>

using namespace ref;

struct Animal;

// Structural features (attributes)
struct Name    : String {};
struct Surname : String {};
struct Age     : UInt32 {};
struct Alive   : Bool {};
struct Emails  : Feature< std::vector< std::string > >{};
struct Pets    : Feature< std::vector< Animal > >{};

// Classes
struct Animal :
    Class< Animal, Features< Alive, Age > >
{
};

struct Person :
    Class< Person,
           Features< Name, Surname, Emails, Pets >,
           Animal >
{
};

int main(int argc, char **argv)
{
    Person me;
    Animal dog;

    // Direct access
    me.set< Name >("Andres");
    me.set< Surname >("Senac");
    me.set< Age >(29);
    const std::vector< std::string > emails = {"andres@senac.es", "asenac@senac.es"};
    me.set< Emails >(emails);
    me.get< Pets >().push_back(dog);

    std::cout << me.get< Name >() << " "
        << me.get< Surname >() << " "
        << me.get< Age >() << std::endl;

    // Descriptors
    const ClassDescriptor * classDesc = me.getClassDescriptor();
    const FeatureDescriptorVector features =
        classDesc->getAllFeatureDescriptors();

    for (size_t i = 0; i < features.size(); i++)
    {
        const FeatureDescriptor * featureDesc = features[i];

        Holder h = featureDesc->getValue(&me);
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

    std::cout << std::endl;

    JsonSerializer js(std::cout);
    js.serialize(&me);

    std::cout << std::endl;

    return 0;
}
