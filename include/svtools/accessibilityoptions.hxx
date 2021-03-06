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
#ifndef INCLUDED_SVTOOLS_ACCESSIBILITYOPTIONS_HXX
#define INCLUDED_SVTOOLS_ACCESSIBILITYOPTIONS_HXX

#include <svtools/svtdllapi.h>
#include <unotools/configitem.hxx>
#include <svl/lstner.hxx>
#include <unotools/options.hxx>

class SvtAccessibilityOptions_Impl;

class SVT_DLLPUBLIC SvtAccessibilityOptions:
    public utl::detail::Options, private SfxListener
{
private:
    static SvtAccessibilityOptions_Impl* sm_pSingleImplConfig;
    static sal_Int32                     sm_nAccessibilityRefCount;

public:
    SvtAccessibilityOptions();
    virtual ~SvtAccessibilityOptions();

    // get & set config entries
    bool        GetIsForPagePreviews() const;
    bool        GetIsHelpTipsDisappear() const;
    bool        GetIsAllowAnimatedGraphics() const;
    bool        GetIsAllowAnimatedText() const;
    bool        GetIsAutomaticFontColor() const;
    sal_Int16   GetHelpTipSeconds() const;
    bool        IsSelectionInReadonly() const;
    bool        GetAutoDetectSystemHC() const;

    void        SetIsForPagePreviews(bool bSet);
    void        SetIsHelpTipsDisappear(bool bSet);
    void        SetIsAllowAnimatedGraphics(bool bSet);
    void        SetIsAllowAnimatedText(bool bSet);
    void        SetIsAutomaticFontColor(bool bSet);
    void        SetHelpTipSeconds(sal_Int16 nSet);
    void        SetSelectionInReadonly(bool bSet);
    void        SetAutoDetectSystemHC(bool bSet);

    //SfxListener:
    virtual void        Notify( SfxBroadcaster& rBC, const SfxHint& rHint ) SAL_OVERRIDE;
    void        SetVCLSettings();
};

#endif // #ifndef INCLUDED_SVTOOLS_ACCESSIBILITYOPTIONS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
