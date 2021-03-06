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
#ifndef INCLUDED_CUI_SOURCE_INC_CHARDLG_HXX
#define INCLUDED_CUI_SOURCE_INC_CHARDLG_HXX

#include <svtools/ctrlbox.hxx>
#include <svtools/stdctrl.hxx>
#include <sfx2/tabdlg.hxx>
#include <svx/fntctrl.hxx>
#include <svx/checklbx.hxx>
#include <svx/langbox.hxx>
#include <vcl/layout.hxx>

// forward ---------------------------------------------------------------

class SvxFontListItem;
class FontList;

// class SvxCharBasePage -------------------------------------------------

class SvxCharBasePage : public SfxTabPage
{
protected:
    VclPtr<SvxFontPrevWindow>  m_pPreviewWin;

    bool                m_bPreviewBackgroundToCharacter;

    SvxCharBasePage(vcl::Window* pParent, const OString& rID, const OUString& rUIXMLDescription, const SfxItemSet& rItemset);

    void SetPrevFontWidthScale( const SfxItemSet& rSet );
    void SetPrevFontEscapement( sal_uInt8 nProp, sal_uInt8 nEscProp, short nEsc );

    inline SvxFont&     GetPreviewFont();
    inline SvxFont&     GetPreviewCJKFont();
    inline SvxFont&     GetPreviewCTLFont();

public:
    virtual ~SvxCharBasePage();
    virtual void dispose() SAL_OVERRIDE;

    using SfxTabPage::ActivatePage;
    using SfxTabPage::DeactivatePage;

    virtual void        ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;

};

// class SvxCharNamePage -------------------------------------------------

struct SvxCharNamePage_Impl;

class SvxCharNamePage : public SvxCharBasePage
{
    friend class VclPtr<SvxCharNamePage>;

private:
    static const sal_uInt16 pNameRanges[];
    VclPtr<VclContainer>       m_pWestFrame;
    VclPtr<FixedText>          m_pWestFontNameFT;
    VclPtr<FontNameBox>        m_pWestFontNameLB;
    VclPtr<FixedText>          m_pWestFontStyleFT;
    VclPtr<FontStyleBox>       m_pWestFontStyleLB;
    VclPtr<FixedText>          m_pWestFontSizeFT;
    VclPtr<FontSizeBox>        m_pWestFontSizeLB;
    VclPtr<FixedText>          m_pWestFontLanguageFT;
    VclPtr<SvxLanguageComboBox> m_pWestFontLanguageLB;
    VclPtr<FixedText>          m_pWestFontTypeFT;

    VclPtr<VclContainer>       m_pEastFrame;
    VclPtr<FixedText>          m_pEastFontNameFT;
    VclPtr<FontNameBox>        m_pEastFontNameLB;
    VclPtr<FixedText>          m_pEastFontStyleFT;
    VclPtr<FontStyleBox>       m_pEastFontStyleLB;
    VclPtr<FixedText>          m_pEastFontSizeFT;
    VclPtr<FontSizeBox>        m_pEastFontSizeLB;
    VclPtr<FixedText>          m_pEastFontLanguageFT;
    VclPtr<SvxLanguageBox>     m_pEastFontLanguageLB;
    VclPtr<FixedText>          m_pEastFontTypeFT;

    VclPtr<VclContainer>       m_pCTLFrame;
    VclPtr<FixedText>          m_pCTLFontNameFT;
    VclPtr<FontNameBox>        m_pCTLFontNameLB;
    VclPtr<FixedText>          m_pCTLFontStyleFT;
    VclPtr<FontStyleBox>       m_pCTLFontStyleLB;
    VclPtr<FixedText>          m_pCTLFontSizeFT;
    VclPtr<FontSizeBox>        m_pCTLFontSizeLB;
    VclPtr<FixedText>          m_pCTLFontLanguageFT;
    VclPtr<SvxLanguageBox>     m_pCTLFontLanguageLB;
    VclPtr<FixedText>          m_pCTLFontTypeFT;

    SvxCharNamePage_Impl*   m_pImpl;

                        SvxCharNamePage( vcl::Window* pParent, const SfxItemSet& rSet );

    void                Initialize();
    const FontList*     GetFontList() const;
    void                UpdatePreview_Impl();
    void                FillStyleBox_Impl( const FontNameBox* rBox );
    void                FillSizeBox_Impl( const FontNameBox* rBox );

    enum LanguageGroup
    {
        /** Language for western text.
         */
        Western = 0,

        /** Language for asian text.
         */
        Asian,

        /** Language for ctl text.
         */
        Ctl
    };

    void                Reset_Impl( const SfxItemSet& rSet, LanguageGroup eLangGrp );
    bool                FillItemSet_Impl( SfxItemSet& rSet, LanguageGroup eLangGrp );

    DECL_LINK_TYPED(UpdateHdl_Impl, Idle *, void);
    DECL_LINK(          FontModifyHdl_Impl, void* );

public:
    using SfxTabPage::ActivatePage;
    using SfxTabPage::DeactivatePage;

    virtual void        ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;
    virtual sfxpg       DeactivatePage( SfxItemSet* pSet = 0 ) SAL_OVERRIDE;

public:
                        virtual ~SvxCharNamePage();
    virtual void        dispose() SAL_OVERRIDE;

    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent, const SfxItemSet* rSet );
    static const sal_uInt16* GetRanges() { return pNameRanges; }

    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;

    void                SetFontList( const SvxFontListItem& rItem );
    void                EnableRelativeMode();
    void                EnableSearchMode();
    ///                  the writer uses SID_ATTR_BRUSH as font background
    void                SetPreviewBackgroundToCharacter();

    void                DisableControls( sal_uInt16 nDisable );
    virtual void        PageCreated(const SfxAllItemSet& aSet) SAL_OVERRIDE;
};

// class SvxCharEffectsPage ----------------------------------------------

class SvxCharEffectsPage : public SvxCharBasePage
{
    friend class VclPtr<SvxCharEffectsPage>;

private:
    static const sal_uInt16 pEffectsRanges[];
    VclPtr<FixedText>          m_pFontColorFT;
    VclPtr<ColorListBox>       m_pFontColorLB;

    VclPtr<FixedText>          m_pEffectsFT;
    VclPtr<ListBox>            m_pEffectsLB;

    VclPtr<FixedText>          m_pReliefFT;
    VclPtr<ListBox>            m_pReliefLB;

    VclPtr<TriStateBox>        m_pOutlineBtn;
    VclPtr<TriStateBox>        m_pShadowBtn;
    VclPtr<TriStateBox>        m_pBlinkingBtn;
    VclPtr<TriStateBox>        m_pHiddenBtn;

    VclPtr<ListBox>            m_pOverlineLB;
    VclPtr<FixedText>          m_pOverlineColorFT;
    VclPtr<ColorListBox>       m_pOverlineColorLB;

    VclPtr<ListBox>            m_pStrikeoutLB;

    VclPtr<ListBox>            m_pUnderlineLB;
    VclPtr<FixedText>          m_pUnderlineColorFT;
    VclPtr<ColorListBox>       m_pUnderlineColorLB;

    VclPtr<CheckBox>           m_pIndividualWordsBtn;

    VclPtr<FixedText>          m_pEmphasisFT;
    VclPtr<ListBox>            m_pEmphasisLB;

    VclPtr<FixedText>          m_pPositionFT;
    VclPtr<ListBox>            m_pPositionLB;

    sal_uInt16          m_nHtmlMode;

    OUString            m_aTransparentColorName;

                        SvxCharEffectsPage( vcl::Window* pParent, const SfxItemSet& rSet );

    void                Initialize();
    void                UpdatePreview_Impl();
    void                SetCaseMap_Impl( SvxCaseMap eCaseMap );
    void                ResetColor_Impl( const SfxItemSet& rSet );
    bool                FillItemSetColor_Impl( SfxItemSet& rSet );

    DECL_LINK(          SelectHdl_Impl, ListBox* );
    DECL_LINK(CbClickHdl_Impl, void *);
    DECL_LINK(TristClickHdl_Impl, void *);
    DECL_LINK(UpdatePreview_Impl, void *);
    DECL_LINK(          ColorBoxSelectHdl_Impl, ColorListBox* );

public:
    virtual ~SvxCharEffectsPage();
    virtual void dispose() SAL_OVERRIDE;

    using SfxTabPage::DeactivatePage;
    virtual sfxpg       DeactivatePage( SfxItemSet* pSet = 0 ) SAL_OVERRIDE;

public:
    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent, const SfxItemSet* rSet );
    static const sal_uInt16* GetRanges() { return pEffectsRanges; }

    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;

    void                DisableControls( sal_uInt16 nDisable );
    void                EnableFlash();
    ///                  the writer uses SID_ATTR_BRUSH as font background
    void                SetPreviewBackgroundToCharacter();
    virtual void        PageCreated(const SfxAllItemSet& aSet) SAL_OVERRIDE;
};

// class SvxCharPositionPage ---------------------------------------------


class SvxCharPositionPage : public SvxCharBasePage
{
    friend class VclPtr<SvxCharPositionPage>;
    static const sal_uInt16 pPositionRanges[];

private:
    VclPtr<RadioButton>        m_pHighPosBtn;
    VclPtr<RadioButton>        m_pNormalPosBtn;
    VclPtr<RadioButton>        m_pLowPosBtn;
    VclPtr<FixedText>          m_pHighLowFT;
    VclPtr<MetricField>        m_pHighLowMF;
    VclPtr<CheckBox>           m_pHighLowRB;
    VclPtr<FixedText>          m_pFontSizeFT;
    VclPtr<MetricField>        m_pFontSizeMF;

    VclPtr<VclContainer>       m_pRotationContainer;

    VclPtr<FixedText>          m_pScalingFT;
    VclPtr<FixedText>          m_pScalingAndRotationFT;
    VclPtr<RadioButton>        m_p0degRB;
    VclPtr<RadioButton>        m_p90degRB;
    VclPtr<RadioButton>        m_p270degRB;
    VclPtr<CheckBox>           m_pFitToLineCB;

    VclPtr<MetricField>        m_pScaleWidthMF;

    VclPtr<ListBox>            m_pKerningLB;
    VclPtr<FixedText>          m_pKerningFT;
    VclPtr<MetricField>        m_pKerningMF;
    VclPtr<CheckBox>           m_pPairKerningBtn;

    short               m_nSuperEsc;
    short               m_nSubEsc;

    sal_uInt16              m_nScaleWidthItemSetVal;
    sal_uInt16              m_nScaleWidthInitialVal;

    sal_uInt8                m_nSuperProp;
    sal_uInt8                m_nSubProp;

                        SvxCharPositionPage( vcl::Window* pParent, const SfxItemSet& rSet );

    void                Initialize();
    void                UpdatePreview_Impl( sal_uInt8 nProp, sal_uInt8 nEscProp, short nEsc );
    void                SetEscapement_Impl( sal_uInt16 nEsc );

    DECL_LINK(          PositionHdl_Impl, RadioButton* );
    DECL_LINK(          RotationHdl_Impl, RadioButton* );
    DECL_LINK(          FontModifyHdl_Impl, void *);
    DECL_LINK(          AutoPositionHdl_Impl, CheckBox* );
    DECL_LINK(          FitToLineHdl_Impl, CheckBox* );
    DECL_LINK(          KerningSelectHdl_Impl, void *);
    DECL_LINK(          KerningModifyHdl_Impl, void *);
    DECL_LINK(          LoseFocusHdl_Impl, MetricField* );
    DECL_LINK(          ScaleWidthModifyHdl_Impl, void *);

public:
    virtual ~SvxCharPositionPage();
    virtual void dispose() SAL_OVERRIDE;

    using SfxTabPage::ActivatePage;
    using SfxTabPage::DeactivatePage;

    virtual sfxpg       DeactivatePage( SfxItemSet* pSet = 0 ) SAL_OVERRIDE;
    virtual void        ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;

public:
    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent, const SfxItemSet* rSet );
    static const sal_uInt16*      GetRanges() { return pPositionRanges; }

    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual void        FillUserData() SAL_OVERRIDE;
    ///                  the writer uses SID_ATTR_BRUSH as font background
    void                SetPreviewBackgroundToCharacter();
    virtual void        PageCreated(const SfxAllItemSet& aSet) SAL_OVERRIDE;
};

// class SvxCharTwoLinesPage ---------------------------------------------

class SvxCharTwoLinesPage : public SvxCharBasePage
{
    friend class VclPtr<SvxCharTwoLinesPage>;
private:
    static const sal_uInt16 pTwoLinesRanges[];
    VclPtr<CheckBox>           m_pTwoLinesBtn;
    VclPtr<VclContainer>       m_pEnclosingFrame;
    VclPtr<ListBox>            m_pStartBracketLB;
    VclPtr<ListBox>            m_pEndBracketLB;

    sal_uInt16              m_nStartBracketPosition;
    sal_uInt16              m_nEndBracketPosition;

    SvxCharTwoLinesPage(vcl::Window* pParent, const SfxItemSet& rSet);

    void                UpdatePreview_Impl();
    void                Initialize();
    void                SelectCharacter( ListBox* pBox );
    void                SetBracket( sal_Unicode cBracket, bool bStart );

    DECL_LINK(TwoLinesHdl_Impl, void *);
    DECL_LINK(          CharacterMapHdl_Impl, ListBox* );

public:
    virtual ~SvxCharTwoLinesPage();
    virtual void dispose() SAL_OVERRIDE;

    using SfxTabPage::ActivatePage;
    using SfxTabPage::DeactivatePage;

    virtual void        ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;
    virtual sfxpg       DeactivatePage( SfxItemSet* pSet = 0 ) SAL_OVERRIDE;

    static VclPtr<SfxTabPage>  Create( vcl::Window* pParent, const SfxItemSet* rSet );
    static const sal_uInt16*  GetRanges() { return pTwoLinesRanges; }

    virtual void        Reset( const SfxItemSet* rSet ) SAL_OVERRIDE;
    virtual bool        FillItemSet( SfxItemSet* rSet ) SAL_OVERRIDE;
    ///                  the writer uses SID_ATTR_BRUSH as font background
    void                SetPreviewBackgroundToCharacter();
    virtual void        PageCreated(const SfxAllItemSet& aSet) SAL_OVERRIDE;
};

#endif // INCLUDED_CUI_SOURCE_INC_CHARDLG_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
