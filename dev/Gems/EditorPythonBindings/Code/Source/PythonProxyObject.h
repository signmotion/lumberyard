/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#pragma once

#include <AzCore/Memory/Memory.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/std/optional.h>

#include <Source/PythonCommon.h>
#include <pybind11/pybind11.h>

namespace EditorPythonBindings
{
    //! Wraps an instance of a Behavior Class that is flagged for 'Editor'
    class PythonProxyObject final
    {
    public:
        AZ_TYPE_INFO(PythonProxyObject, "{448A4480-CCA8-4F14-9F17-41B0491F9FD1}");
        AZ_CLASS_ALLOCATOR(PythonProxyObject, AZ::SystemAllocator, 0);

        PythonProxyObject() = default;
        explicit PythonProxyObject(const AZ::TypeId& typeId);
        explicit PythonProxyObject(const char* typeName);
        explicit PythonProxyObject(const AZ::BehaviorObject& object);

        ~PythonProxyObject();

        //! Gets the AZ RTTI type of the BehaviorObject
        AZStd::optional<AZ::TypeId> GetWrappedType() const;

        //! Returns the wrapped behavior object pointer if it is valid
        AZStd::optional<AZ::BehaviorObject*> GetBehaviorObject();

        //! Gets the name of the type of the wrapped BehaviorObject
        const char* GetWrappedTypeName() const;
        
        //! Assigns a value to a property (by name) a value; the types must match
        void SetPropertyValue(const char* propertyName, pybind11::object value);
        
        //! Gets the value or callable held by a property of a wrapped BehaviorObject
        pybind11::object GetPropertyValue(const char* attributeName);

        //! Creates a default constructed instance a 'typeName'
        bool SetByTypeName(const char* typeName);

        //! Invokes a method by name on a wrapped BehaviorObject
        pybind11::object Invoke(const char* methodName, pybind11::args pythonArgs);

        //! Constructs a BehaviorClass using Python arguments 
        pybind11::object Construct(const AZ::BehaviorClass& behaviorClass, pybind11::args args);

    protected:
        void PrepareWrappedObject(const AZ::BehaviorClass& behaviorClass);
        void ReleaseWrappedObject();
        bool CreateDefault(const AZ::BehaviorClass* behaviorClass);

    private:
        enum class Ownership
        {
            None,
            Owned,
            Released
        };

        AZ::BehaviorObject m_wrappedObject;
        AZStd::string m_wrappedObjectTypeName;
        Ownership m_ownership = Ownership::None;
        AZStd::unordered_map<AZ::Crc32, AZ::BehaviorMethod*> m_methods;
        AZStd::unordered_map<AZ::Crc32, AZ::BehaviorProperty*> m_properties;
    };

    namespace PythonProxyObjectManagement
    {
        //! Creates the 'azlmbr.object' module so that Python script developers can manage proxy objects
        void CreateSubmodule(pybind11::module parentModule, pybind11::module defaultModule);

        //! Creates a Python object storing a BehaviorObject backed by a BehaviorClass
        pybind11::object CreatePythonProxyObject(const AZ::TypeId& typeId, void* data);

        //! Checks if function can be reflected as a class member method
        bool IsMemberLike(const AZ::BehaviorMethod& method, const AZ::TypeId& typeId);
    }
}

namespace pybind11 
{
    namespace detail 
    {
        //! Type caster specialization PythonProxyObject to convert between Python <-> AZ Reflection
        template <> 
        struct type_caster<EditorPythonBindings::PythonProxyObject> 
            : public type_caster_base<EditorPythonBindings::PythonProxyObject>
        {
        public:

            // Conversion (Python -> C++)
            bool load(handle src, bool convert)
            {
                return type_caster_base<EditorPythonBindings::PythonProxyObject>::load(src, convert);
            }

            // Conversion (C++ -> Python)
            static handle cast(const EditorPythonBindings::PythonProxyObject* src, return_value_policy policy, handle parent)
            {
                return type_caster_base<EditorPythonBindings::PythonProxyObject>::cast(src, policy, parent);
            }
        };
    }
}
