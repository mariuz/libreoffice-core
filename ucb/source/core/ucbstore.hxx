/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef INCLUDED_UCB_SOURCE_CORE_UCBSTORE_HXX
#define INCLUDED_UCB_SOURCE_CORE_UCBSTORE_HXX

#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/ucb/XPropertySetRegistryFactory.hpp>
#include <com/sun/star/ucb/XPropertySetRegistry.hpp>
#include <com/sun/star/ucb/XPersistentPropertySet.hpp>
#include <com/sun/star/beans/XPropertyContainer.hpp>
#include <com/sun/star/beans/XPropertySetInfoChangeNotifier.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <cppuhelper/weak.hxx>
#include <ucbhelper/macros.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase7.hxx>


#define STORE_SERVICE_NAME          "com.sun.star.ucb.Store"
#define PROPSET_REG_SERVICE_NAME    "com.sun.star.ucb.PropertySetRegistry"
#define PERS_PROPSET_SERVICE_NAME   "com.sun.star.ucb.PersistentPropertySet"



struct UcbStore_Impl;

class UcbStore : public cppu::WeakImplHelper3 <
    css::lang::XServiceInfo,
    css::ucb::XPropertySetRegistryFactory,
    css::lang::XInitialization >
{
    com::sun::star::uno::Reference<
                com::sun::star::uno::XComponentContext > m_xContext;
    UcbStore_Impl* m_pImpl;

public:
    explicit UcbStore(
        const com::sun::star::uno::Reference<
                com::sun::star::uno::XComponentContext >& xContext );
    virtual ~UcbStore();

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName )
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    static OUString getImplementationName_Static();
    static css::uno::Sequence< OUString > getSupportedServiceNames_Static();

    static css::uno::Reference< css::lang::XSingleServiceFactory >
    createServiceFactory( const css::uno::Reference<
                          css::lang::XMultiServiceFactory >& rxServiceMgr );

    // XPropertySetRegistryFactory
    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XPropertySetRegistry > SAL_CALL
    createPropertySetRegistry( const OUString& URL )
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XInitialization
    virtual void SAL_CALL
    initialize( const ::com::sun::star::uno::Sequence<
                        ::com::sun::star::uno::Any >& aArguments )
        throw( ::com::sun::star::uno::Exception,
               ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >&
    getInitArgs() const;
};



struct PropertySetRegistry_Impl;
class PersistentPropertySet;

class PropertySetRegistry : public cppu::WeakImplHelper3 <
    css::lang::XServiceInfo,
    css::ucb::XPropertySetRegistry,
    css::container::XNameAccess >
{
    friend class PersistentPropertySet;

    com::sun::star::uno::Reference<
                com::sun::star::uno::XComponentContext > m_xContext;
    PropertySetRegistry_Impl* m_pImpl;

private:
    com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >
    getConfigProvider();

    void add   ( PersistentPropertySet* pSet );
    void remove( PersistentPropertySet* pSet );

    void renamePropertySet( const OUString& rOldKey,
                            const OUString& rNewKey );

public:
    PropertySetRegistry(
        const com::sun::star::uno::Reference<
                com::sun::star::uno::XComponentContext >& xContext,
                const ::com::sun::star::uno::Sequence<
                                                ::com::sun::star::uno::Any >& rInitArgs);
    virtual ~PropertySetRegistry();


    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName )
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    static OUString getImplementationName_Static();
    static css::uno::Sequence< OUString > getSupportedServiceNames_Static();

    // XPropertySetRegistry
    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XPersistentPropertySet > SAL_CALL
    openPropertySet( const OUString& key, sal_Bool create )
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    removePropertySet( const OUString& key )
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XElementAccess ( XNameAccess is derived from it )
    virtual com::sun::star::uno::Type SAL_CALL
    getElementType()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    hasElements()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XNameAccess
    virtual com::sun::star::uno::Any SAL_CALL
    getByName( const OUString& aName )
        throw( com::sun::star::container::NoSuchElementException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual com::sun::star::uno::Sequence< OUString > SAL_CALL
    getElementNames()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL
    hasByName( const OUString& aName )
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // Non-interface methods
    com::sun::star::uno::Reference< com::sun::star::uno::XInterface >
    getRootConfigReadAccess();
    com::sun::star::uno::Reference< com::sun::star::uno::XInterface >
    getConfigWriteAccess( const OUString& rPath );
};



struct PersistentPropertySet_Impl;

class PersistentPropertySet : public cppu::WeakImplHelper7 <
    css::lang::XServiceInfo,
    css::lang::XComponent,
    css::ucb::XPersistentPropertySet,
    css::container::XNamed,
    css::beans::XPropertyContainer,
    css::beans::XPropertySetInfoChangeNotifier,
    css::beans::XPropertyAccess >
{
    css::uno::Reference< css::uno::XComponentContext > m_xContext;
    PersistentPropertySet_Impl* m_pImpl;

private:
    void notifyPropertyChangeEvent(
        const com::sun::star::beans::PropertyChangeEvent& rEvent ) const;
    void notifyPropertySetInfoChange(
        const com::sun::star::beans::PropertySetInfoChangeEvent& evt ) const;

public:
    PersistentPropertySet(
        const com::sun::star::uno::Reference<
                com::sun::star::uno::XComponentContext >& xContext,
        PropertySetRegistry& rCreator,
        const OUString& rKey );
    virtual ~PersistentPropertySet();

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL supportsService( const OUString& ServiceName )
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw( css::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    static OUString getImplementationName_Static();
    static css::uno::Sequence< OUString > getSupportedServiceNames_Static();

    // XComponent
    virtual void SAL_CALL
    dispose()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    addEventListener( const com::sun::star::uno::Reference<
                            com::sun::star::lang::XEventListener >& Listener )
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    removeEventListener( const com::sun::star::uno::Reference<
                            com::sun::star::lang::XEventListener >& Listener )
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XPropertySet
    virtual com::sun::star::uno::Reference<
                com::sun::star::beans::XPropertySetInfo > SAL_CALL
    getPropertySetInfo()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    setPropertyValue( const OUString& aPropertyName,
                      const com::sun::star::uno::Any& aValue )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::beans::PropertyVetoException,
               com::sun::star::lang::IllegalArgumentException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual com::sun::star::uno::Any SAL_CALL
    getPropertyValue( const OUString& PropertyName )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    addPropertyChangeListener( const OUString& aPropertyName,
                               const com::sun::star::uno::Reference<
                                   com::sun::star::beans::XPropertyChangeListener >& xListener )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    removePropertyChangeListener( const OUString& aPropertyName,
                                  const com::sun::star::uno::Reference<
                                      com::sun::star::beans::XPropertyChangeListener >& aListener )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    addVetoableChangeListener( const OUString& PropertyName,
                               const com::sun::star::uno::Reference<
                                   com::sun::star::beans::XVetoableChangeListener >& aListener )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    removeVetoableChangeListener( const OUString& PropertyName,
                                  const com::sun::star::uno::Reference<
                                      com::sun::star::beans::XVetoableChangeListener >& aListener )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XPersistentPropertySet
    virtual com::sun::star::uno::Reference<
                com::sun::star::ucb::XPropertySetRegistry > SAL_CALL
    getRegistry()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual OUString SAL_CALL
    getKey()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XNamed
    virtual OUString SAL_CALL
    getName()
        throw( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    setName( const OUString& aName )
        throw( ::com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XPropertyContainer
    virtual void SAL_CALL
    addProperty( const OUString& Name,
                 sal_Int16 Attributes,
                 const com::sun::star::uno::Any& DefaultValue )
        throw( com::sun::star::beans::PropertyExistException,
               com::sun::star::beans::IllegalTypeException,
               com::sun::star::lang::IllegalArgumentException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    removeProperty( const OUString& Name )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::beans::NotRemoveableException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XPropertySetInfoChangeNotifier
    virtual void SAL_CALL
    addPropertySetInfoChangeListener( const com::sun::star::uno::Reference<
            com::sun::star::beans::XPropertySetInfoChangeListener >& Listener )
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    removePropertySetInfoChangeListener( const com::sun::star::uno::Reference<
            com::sun::star::beans::XPropertySetInfoChangeListener >& Listener )
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // XPropertyAccess
    virtual com::sun::star::uno::Sequence<
                com::sun::star::beans::PropertyValue > SAL_CALL
    getPropertyValues()
        throw( com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;
    virtual void SAL_CALL
    setPropertyValues( const com::sun::star::uno::Sequence<
                            com::sun::star::beans::PropertyValue >& aProps )
        throw( com::sun::star::beans::UnknownPropertyException,
               com::sun::star::beans::PropertyVetoException,
               com::sun::star::lang::IllegalArgumentException,
               com::sun::star::lang::WrappedTargetException,
               com::sun::star::uno::RuntimeException, std::exception ) SAL_OVERRIDE;

    // Non-interface methods.
    PropertySetRegistry& getPropertySetRegistry();
    const OUString& getFullKey();
};

#endif // INCLUDED_UCB_SOURCE_CORE_UCBSTORE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
