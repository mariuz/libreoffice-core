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

#ifndef INCLUDED_ACCESSIBILITY_INC_ACCESSIBILITY_EXTENDED_ACCESSIBLETABBARPAGE_HXX
#define INCLUDED_ACCESSIBILITY_INC_ACCESSIBILITY_EXTENDED_ACCESSIBLETABBARPAGE_HXX

#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase2.hxx>
#include "accessibility/extended/accessibletabbarbase.hxx"

#include <vector>

namespace utl {
class AccessibleStateSetHelper;
}


namespace accessibility
{



    //  class AccessibleTabBarPage


    typedef ::cppu::ImplHelper2<
        css::accessibility::XAccessible,
        css::lang::XServiceInfo > AccessibleTabBarPage_BASE;

    class AccessibleTabBarPage :    public AccessibleTabBarBase,
                                    public AccessibleTabBarPage_BASE
    {
        friend class AccessibleTabBarPageList;

    private:
        sal_uInt16              m_nPageId;
        bool                m_bEnabled;
        bool                m_bShowing;
        bool                m_bSelected;
        OUString                m_sPageText;

        css::uno::Reference< css::accessibility::XAccessible >        m_xParent;

    protected:
        bool                IsEnabled();
        bool                IsShowing();
        bool                IsSelected();

        void                    SetEnabled( bool bEnabled );
        void                    SetShowing( bool bShowing );
        void                    SetSelected( bool bSelected );
        void                    SetPageText( const OUString& sPageText );

        sal_uInt16              GetPageId() const { return m_nPageId; }

        void            FillAccessibleStateSet( utl::AccessibleStateSetHelper& rStateSet );

        // OCommonAccessibleComponent
        virtual css::awt::Rectangle implGetBounds(  ) throw (css::uno::RuntimeException) SAL_OVERRIDE;

        // XComponent
        virtual void SAL_CALL   disposing() SAL_OVERRIDE;

    public:
        AccessibleTabBarPage( TabBar* pTabBar, sal_uInt16 nPageId,
                              const css::uno::Reference< css::accessibility::XAccessible >& rxParent );
        virtual ~AccessibleTabBarPage();

        // XInterface
        DECLARE_XINTERFACE()

        // XTypeProvider
        DECLARE_XTYPEPROVIDER()

        // XServiceInfo
        virtual OUString SAL_CALL getImplementationName() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual sal_Bool SAL_CALL supportsService( const OUString& rServiceName ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames() throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

        // XAccessible
        virtual css::uno::Reference< css::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

        // XAccessibleContext
        virtual sal_Int32 SAL_CALL getAccessibleChildCount(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual css::uno::Reference< css::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i ) throw (css::lang::IndexOutOfBoundsException, css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual css::uno::Reference< css::accessibility::XAccessible > SAL_CALL getAccessibleParent(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual sal_Int32 SAL_CALL getAccessibleIndexInParent(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual sal_Int16 SAL_CALL getAccessibleRole(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual OUString SAL_CALL getAccessibleDescription(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual OUString SAL_CALL getAccessibleName(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual css::uno::Reference< css::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual css::uno::Reference< css::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual css::lang::Locale SAL_CALL getLocale(  ) throw (css::accessibility::IllegalAccessibleComponentStateException, css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

        // XAccessibleComponent
        virtual css::uno::Reference< css::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const css::awt::Point& aPoint ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual void SAL_CALL grabFocus(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual sal_Int32 SAL_CALL getForeground(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual sal_Int32 SAL_CALL getBackground(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;

        // XAccessibleExtendedComponent
        virtual css::uno::Reference< css::awt::XFont > SAL_CALL getFont(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual OUString SAL_CALL getTitledBorderText(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
        virtual OUString SAL_CALL getToolTipText(  ) throw (css::uno::RuntimeException, std::exception) SAL_OVERRIDE;
    };


}   // namespace accessibility


#endif // INCLUDED_ACCESSIBILITY_INC_ACCESSIBILITY_EXTENDED_ACCESSIBLETABBARPAGE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
