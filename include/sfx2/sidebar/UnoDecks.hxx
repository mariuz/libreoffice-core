/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#ifndef INCLUDED_SFX2_SIDEBAR_DECKS_HXX
#define INCLUDED_SFX2_SIDEBAR_DECKS_HXX


#include <com/sun/star/ui/XDecks.hpp>

#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/frame/XFrame.hpp>

#include <cppuhelper/compbase1.hxx>
#include <cppuhelper/weakref.hxx>

#include <sfx2/sidebar/SidebarController.hxx>
#include <sfx2/sidebar/ResourceManager.hxx>

/** get the decks
*/
class SfxUnoDecks : public ::cppu::WeakImplHelper1< css::ui::XDecks >
{

public:

    SfxUnoDecks(const css::uno::Reference<css::frame::XFrame>&);
    virtual ~SfxUnoDecks() {};

// XNameAccess

    virtual css::uno::Any SAL_CALL getByName( const OUString& aName )
                                throw(css::container::NoSuchElementException,
                                    css::lang::WrappedTargetException,
                                    css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    virtual css::uno::Sequence< OUString > SAL_CALL getElementNames()
                                throw(css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    virtual sal_Bool SAL_CALL hasByName( const OUString& aName )
                                throw(css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

// XIndexAccess

    virtual sal_Int32 SAL_CALL getCount()
                                throw(css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 Index )
                                throw(css::lang::IndexOutOfBoundsException,
                                      css::lang::WrappedTargetException,
                                      css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

// XElementAccess
    virtual css::uno::Type SAL_CALL getElementType()
                                throw(css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    virtual sal_Bool SAL_CALL hasElements()
                                throw(css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

private:

    const css::uno::Reference<css::frame::XFrame> xFrame;
    sfx2::sidebar::SidebarController* getSidebarController();

};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */