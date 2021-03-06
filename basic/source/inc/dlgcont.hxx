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

#ifndef INCLUDED_BASIC_SOURCE_INC_DLGCONT_HXX
#define INCLUDED_BASIC_SOURCE_INC_DLGCONT_HXX

#include "namecont.hxx"

#include <com/sun/star/resource/XStringResourceSupplier.hpp>
#include <com/sun/star/resource/XStringResourcePersistence.hpp>

#include <cppuhelper/implbase1.hxx>
#include <comphelper/uno3.hxx>

namespace basic
{



class SfxDialogLibraryContainer : public SfxLibraryContainer
{
    // Methods to distinguish between different library types
    virtual SfxLibrary* SAL_CALL implCreateLibrary( const OUString& aName ) SAL_OVERRIDE;
    virtual SfxLibrary* SAL_CALL implCreateLibraryLink
        ( const OUString& aName, const OUString& aLibInfoFileURL,
          const OUString& StorageURL, bool ReadOnly ) SAL_OVERRIDE;
    virtual css::uno::Any SAL_CALL createEmptyLibraryElement() SAL_OVERRIDE;
    virtual bool SAL_CALL isLibraryElementValid(const css::uno::Any& rElement) const SAL_OVERRIDE;
    virtual void SAL_CALL writeLibraryElement
    (
        const css::uno::Reference< css::container::XNameContainer>& xLibrary,
        const OUString& aElementName,
        const css::uno::Reference< css::io::XOutputStream >& xOutput
    )
        throw(css::uno::Exception) SAL_OVERRIDE;

    virtual css::uno::Any SAL_CALL importLibraryElement
    (
        const css::uno::Reference< css::container::XNameContainer>& xLibrary,
        const OUString& aElementName,
        const OUString& aFile,
        const css::uno::Reference< css::io::XInputStream >& xElementStream ) SAL_OVERRIDE;

    virtual void SAL_CALL importFromOldStorage( const OUString& aFile ) SAL_OVERRIDE;

    virtual SfxLibraryContainer* createInstanceImpl() SAL_OVERRIDE;

    virtual void onNewRootStorage() SAL_OVERRIDE;

    virtual const sal_Char* SAL_CALL    getInfoFileName() const SAL_OVERRIDE;
    virtual const sal_Char* SAL_CALL    getOldInfoFileName() const SAL_OVERRIDE;
    virtual const sal_Char* SAL_CALL    getLibElementFileExtension() const SAL_OVERRIDE;
    virtual const sal_Char* SAL_CALL    getLibrariesDir() const SAL_OVERRIDE;

public:
    SfxDialogLibraryContainer();
    SfxDialogLibraryContainer( const css::uno::Reference< css::embed::XStorage >& xStorage );

    // Methods XStorageBasedLibraryContainer
    virtual void SAL_CALL storeLibrariesToStorage(
        const css::uno::Reference< css::embed::XStorage >& RootStorage )
            throw (css::uno::RuntimeException,
                   css::lang::WrappedTargetException,
                   std::exception) SAL_OVERRIDE;

    // Resource handling
    css::uno::Reference< css::resource::XStringResourcePersistence >
        implCreateStringResource( class SfxDialogLibrary* pDialog );

    // Methods XServiceInfo
    virtual OUString SAL_CALL getImplementationName( )
        throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames( )
        throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    // XLibraryQueryExecutable
    virtual sal_Bool SAL_CALL HasExecutableCode(const OUString&)
        throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
};



typedef ::cppu::ImplHelper1 <   css::resource::XStringResourceSupplier
                            >   SfxDialogLibrary_BASE;

class SfxDialogLibrary  :public SfxLibrary
                        ,public SfxDialogLibrary_BASE
{
    SfxDialogLibraryContainer*                                        m_pParent;
    css::uno::Reference< css::resource::XStringResourcePersistence>   m_xStringResourcePersistence;
    OUString                                                          m_aName;

    // Provide modify state including resources
    virtual bool isModified() SAL_OVERRIDE;
    virtual void storeResources() SAL_OVERRIDE;
    virtual void storeResourcesAsURL( const OUString& URL, const OUString& NewName ) SAL_OVERRIDE;
    virtual void storeResourcesToURL( const OUString& URL,
        const css::uno::Reference< css::task::XInteractionHandler >& xHandler  ) SAL_OVERRIDE;
    virtual void storeResourcesToStorage( const css::uno::Reference
        < css::embed::XStorage >& xStorage ) SAL_OVERRIDE;

public:
    SfxDialogLibrary
    (
        ModifiableHelper& _rModifiable,
        const OUString& aName,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::ucb::XSimpleFileAccess3 >& xSFI,
        SfxDialogLibraryContainer* pParent
    );

    SfxDialogLibrary
    (
        ModifiableHelper& _rModifiable,
        const OUString& aName,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::ucb::XSimpleFileAccess3 >& xSFI,
        const OUString& aLibInfoFileURL, const OUString& aStorageURL, bool ReadOnly,
        SfxDialogLibraryContainer* pParent
    );

    DECLARE_XINTERFACE()
    DECLARE_XTYPEPROVIDER()

    // XStringResourceSupplier
    virtual css::uno::Reference< css::resource::XStringResourceResolver >
        SAL_CALL getStringResource(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    OUString getName()
        { return m_aName; }

    css::uno::Reference< css::resource::XStringResourcePersistence >
        getStringResourcePersistence()
    {
        return m_xStringResourcePersistence;
    }

    static bool containsValidDialog( const css::uno::Any& aElement );

protected:
    virtual bool SAL_CALL isLibraryElementValid(const css::uno::Any& rElement) const SAL_OVERRIDE;
};

}   // namespace basic

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
