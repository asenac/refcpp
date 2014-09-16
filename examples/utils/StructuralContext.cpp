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
        Reference::ReferenceType referenceType;
        const TypeDescriptor * currentNode;

        PathItem()
            : referenceType(Reference::kContained)
            , currentNode()
        {}
    };

    Reference::ReferenceType
    getReferenceType(PointerTypeDescriptor::PointerType type)
    {
        switch (type)
        {
        case PointerTypeDescriptor::kUnique:
            return Reference::kOwned;
        case PointerTypeDescriptor::kShared:
            return Reference::kSharedOwned;
        case PointerTypeDescriptor::kWeak:
            return Reference::kWeak;
        default:
            break;
        }

        return Reference::kRaw;
    }
} // namespace

struct StructuralContext::Impl
{
    typedef const ClassDescriptor * Desc;
    typedef std::vector< Reference > ReferenceVector;

    Desc m_rootClassDesc;

    struct ClassInfo
    {
        std::set< Desc > subclasses;
        std::set< Desc > allSubclasses;
        // incoming
        ReferenceVector inReferences;
        // outgoing
        ReferenceVector outReferences;
    };

    typedef std::unordered_map< Desc, ClassInfo > ClassInfoMap;
    ClassInfoMap m_classInfoMap;
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

            m_classInfoMap[parent].subclasses.insert(current);

            Desc predecessor = current;
            while ((predecessor = predecessor->getParentClassDescriptor()))
            {
                m_classInfoMap[predecessor].allSubclasses.insert(current);
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
                        const Reference ref {currentItem.referenceType, current, feature, classDesc};

                        m_classInfoMap[classDesc].inReferences.push_back(ref);
                        m_classInfoMap[current].outReferences.push_back(ref);

                        if (currentItem.referenceType == Reference::kContained
                                || currentItem.referenceType == Reference::kOwned
                                || currentItem.referenceType == Reference::kSharedOwned)
                        {
                            pending.push_back(classDesc);
                        }
                    }
                    break;
                case TypeDescriptor::kPointer:
                    {
                        auto ptrDesc = static_cast< const PointerTypeDescriptor * >(typeDesc);
                        currentItem.currentNode = ptrDesc->getPointedTypeDescriptor();
                        currentItem.referenceType = getReferenceType(ptrDesc->getPointerType());

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

std::vector< Reference >
StructuralContext::getIncomingReferences(const ClassDescriptor * classDesc) const
{
    Impl::ClassInfoMap::const_iterator it = m_impl->m_classInfoMap.find(classDesc);
    if (it != m_impl->m_classInfoMap.end())
        return it->second.inReferences;
    return std::vector< Reference >(); // TODO Exception?
}

std::vector< Reference >
StructuralContext::getOutgoingReferences(const ClassDescriptor * classDesc) const
{
    Impl::ClassInfoMap::const_iterator it = m_impl->m_classInfoMap.find(classDesc);
    if (it != m_impl->m_classInfoMap.end())
        return it->second.outReferences;
    return std::vector< Reference >(); // TODO Exception?
}

