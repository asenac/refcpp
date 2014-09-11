#include <ref/Descriptors.hpp>
#include "StructuralContext.hpp"
#include <unordered_map>
#include <set>
#include <list>

using namespace ref;

namespace
{
    struct PathItem
    {
        bool contained;
        bool canBeASubclass;
        const TypeDescriptor * currentNode;

        PathItem()
            : contained(true)
            , canBeASubclass(false)
            , currentNode(nullptr)
        {}
    };

    bool isContainerPointer(PointerTypeDescriptor::PointerType type)
    {
        switch (type)
        {
        case PointerTypeDescriptor::kUnique:
        case PointerTypeDescriptor::kShared:
            return true;
        default:
            break;
        }

        return false;
    }
} // namespace

struct StructuralContext::Impl
{
    typedef const ClassDescriptor * Desc;

    struct Reference
    {
        Desc classDesc;
        const FeatureDescriptor * featureDesc;
        bool canBeASubclass;
    };

    typedef std::vector< Reference > ReferenceVector;

    Desc m_rootClassDesc;

    struct ClassInfo
    {
        std::set< Desc > subclasses;
        std::set< Desc > allSubclasses;
        // incoming
        ReferenceVector containers;
        ReferenceVector inReferences;
        // outgoing
        ReferenceVector contents;
        ReferenceVector outReferences;
    };

    typedef std::unordered_map< Desc, ClassInfo > InfoMap;
    InfoMap m_infoMap;
    std::vector< const ClassDescriptor * > m_allClasses;

    Impl (const ClassDescriptor * rootClassDesc);
};

StructuralContext::Impl::Impl(const ClassDescriptor * rootClassDesc)
    : m_rootClassDesc(rootClassDesc)
{
    std::list< Desc > pending;
    std::set< Desc > processed;
    pending.push_back(rootClassDesc);

    while (!pending.empty())
    {
        Desc current = pending.front();
        pending.pop_front();

        if (processed.count(current))
            continue;

        Desc parent = current->getParentClassDescriptor();
        if (parent)
        {
            if (!processed.count(parent))
            {
                pending.push_back(parent);
            }

            m_infoMap[parent].subclasses.insert(current);

            Desc predecessor = current;
            while ((predecessor = predecessor->getParentClassDescriptor()))
            {
                m_infoMap[predecessor].allSubclasses.insert(current);
            }
        }

        auto features = current->getFeatureDescriptors();

        for (auto feature: features)
        {
            auto typeDesc = feature->getTypeDescriptor();
            std::list< PathItem > path;
            PathItem item;
            item.currentNode = typeDesc;
            path.push_back(item);

            while (!path.empty())
            {
                PathItem currentItem = path.front();
                auto typeDesc = currentItem.currentNode;
                path.pop_front();

                switch (typeDesc->getKind())
                {
                case TypeDescriptor::kClass:
                    {
                        auto classDesc = static_cast< const ClassDescriptor * >(typeDesc);
                        const Reference ref {current, feature, currentItem.canBeASubclass};
                        const Reference inverseRef {classDesc, feature, currentItem.canBeASubclass};

                        if (currentItem.contained)
                        {
                            m_infoMap[classDesc].containers.push_back(ref);
                            m_infoMap[current].contents.push_back(inverseRef);

                            pending.push_back(classDesc);
                        }
                        else
                        {
                            m_infoMap[classDesc].inReferences.push_back(ref);
                            m_infoMap[current].outReferences.push_back(inverseRef);
                        }
                    }
                    break;
                case TypeDescriptor::kPointer:
                    {
                        auto ptrDesc = static_cast< const PointerTypeDescriptor * >(typeDesc);
                        currentItem.currentNode = ptrDesc->getPointedTypeDescriptor();
                        currentItem.canBeASubclass = true;

                        const bool isContainer = isContainerPointer(ptrDesc->getPointerType());

                        if (!currentItem.contained && isContainer)
                            throw "Invalid model"; // TODO exception
                        currentItem.contained = isContainer;

                        path.push_back(currentItem);
                    }
                    break;
                case TypeDescriptor::kPair:
                    {
                        auto pairDesc = static_cast< const PairTypeDescriptor * >(typeDesc);

                        currentItem.currentNode = pairDesc->getFirstTypeDescriptor();
                        path.push_back(currentItem);

                        currentItem.currentNode = pairDesc->getSecondTypeDescriptor();
                        path.push_back(currentItem);
                    }
                    break;
                // Containers
                case TypeDescriptor::kList:
                case TypeDescriptor::kMap:
                case TypeDescriptor::kSet:
                    {
                        auto containerDesc = static_cast< const ContainerTypeDescriptor * >(typeDesc);
                        currentItem.currentNode = containerDesc->getValueTypeDescriptor();
                        path.push_back(currentItem);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        processed.insert(current);
    }

    m_allClasses = {processed.begin(), processed.end()};
}

StructuralContext::StructuralContext(const ClassDescriptor * rootClassDesc)
    : m_impl(new Impl(rootClassDesc))
{}

StructuralContext::~StructuralContext()
{
    delete m_impl;
}

std::vector< const ClassDescriptor * > StructuralContext::getAllClasses() const
{
    return m_impl->m_allClasses;
}

