#include "JsonSerializer.hpp"
#include <ref/Descriptors.hpp>
#include <ref/Class.hpp>
#include <algorithm>

using namespace ref;
using namespace std;

namespace
{
    int maxLevel = 0;
    const size_t spaces = 4;
    const string _indent[] = {
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
        string(maxLevel++ * spaces, ' '),
    };
} // namespace

const char * JsonSerializer::indent() const
{
    const int index = min(level, maxLevel);
    return _indent[index].c_str();
}

void JsonSerializer::serialize(ModelClass * obj)
{
    if (!obj)
        return;

    ++level;
    os << '{';

    auto classDesc = obj->getClassDescriptor();
    const auto features = classDesc->getFeatureValues(obj);

    for (size_t i = 0; i < features.size(); i++)
    {
        os << endl << indent();
        os << '"' << features[i].first->getXmlTag() << "\" : ";

        serialize(features[i].second);

        if (i + 1 < features.size())
            os << ',';
    }

    --level;
    os << endl << indent();
    os << '}';
}

void JsonSerializer::serialize(Holder h)
{
    if (!h.isValid())
        return;

    auto desc = h.descriptor();

    switch (desc->getKind())
    {
    case TypeDescriptor::kPrimitive:
        {
            auto primDesc = desc->as<PrimitiveTypeDescriptor>();
            os << '"' << primDesc->getString(h) << '"';
        }
        break;
    case TypeDescriptor::kClass:
        {
            auto classDesc = desc->as<ClassDescriptor>();
            ModelClass * obj = classDesc->get(h);
            serialize(obj);
        }
        break;
    case TypeDescriptor::kPair:
        {
            auto pairDesc = desc->as<PairTypeDescriptor>();
            const auto value = pairDesc->getValue(h);

            ++level;
            os << '{';

            os << endl << indent();
            os << "\"first\" : ";
            serialize(value.first);

            os << ',';

            os << endl << indent();
            os << "\"second\" : ";
            serialize(value.second);

            --level;
            os << endl << indent();
            os << '}';
        }
        break;
    case TypeDescriptor::kMap:
    case TypeDescriptor::kList:
    case TypeDescriptor::kSet:
        {
            ++level;
            os << '[';

            auto vecDesc = desc->as<ContainerTypeDescriptor>();
            const auto values = vecDesc->getValue(h);

            for (size_t i = 0; i < values.size(); i++)
            {
                os << endl << indent();

                serialize(values[i]);

                if (i + 1 < values.size())
                    os << ',';
            }

            --level;
            os << endl << indent();
            os << ']';
        }
        break;
    default:
        os << "\"Unsupported type\"";
        break;
    }
}
