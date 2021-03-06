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
#ifndef INCLUDED_CUI_SOURCE_INC_CUITABAREA_HXX
#define INCLUDED_CUI_SOURCE_INC_CUITABAREA_HXX

#include <svtools/valueset.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <svx/dlgctrl.hxx>
#include <svx/xsetit.hxx>
#include <svx/xfillit0.hxx>
#include <svx/xfillit.hxx>
#include <svx/tabarea.hxx>
#include <svx/SvxColorValueSet.hxx>

class SdrModel;
class SdrView;

/************************************************************************/

class SvxAreaTabDialog : public SfxTabDialog
{
    sal_uInt16            m_nAreaTabPage;
    sal_uInt16            m_nShadowTabPage;
    sal_uInt16            m_nTransparenceTabPage;
    sal_uInt16            m_nColorTabPage;
    sal_uInt16            m_nGradientTabPage;
    sal_uInt16            m_nHatchTabPage;
    sal_uInt16            m_nBitmapTabPage;

private:
    SdrModel*           mpDrawModel;

    XColorListRef         mpColorList;
    XColorListRef         mpNewColorList;
    XGradientListRef      mpGradientList;
    XGradientListRef      mpNewGradientList;
    XHatchListRef         mpHatchingList;
    XHatchListRef         mpNewHatchingList;
    XBitmapListRef        mpBitmapList;
    XBitmapListRef        mpNewBitmapList;

    const SfxItemSet&   mrOutAttrs;

    ChangeType          mnColorListState;
    ChangeType          mnBitmapListState;
    ChangeType          mnGradientListState;
    ChangeType          mnHatchingListState;

    sal_uInt16          mnPageType;
    sal_Int32           mnPos;
    bool                mbAreaTP;

    virtual void        PageCreated( sal_uInt16 nId, SfxTabPage &rPage ) SAL_OVERRIDE;

protected:
    virtual short       Ok() SAL_OVERRIDE;
    DECL_LINK( CancelHdlImpl, void * );
    void                SavePalettes();

public:
    SvxAreaTabDialog( vcl::Window* pParent, const SfxItemSet* pAttr, SdrModel* pModel, bool bShadow );

    void                SetNewColorList( XColorListRef pColTab )
                            { mpNewColorList = pColTab; }
    XColorListRef         GetNewColorList() const { return mpNewColorList; }
    const XColorListRef   GetColorList() const { return mpColorList; }

    void                SetNewGradientList( XGradientListRef pGrdLst)
                            { mpNewGradientList = pGrdLst; }
    XGradientListRef       GetNewGradientList() const
                            { return mpNewGradientList; }

    void                 SetNewHatchingList( XHatchListRef pHtchLst)
                            { mpNewHatchingList = pHtchLst; }
    XHatchListRef          GetNewHatchingList() const
                            { return mpNewHatchingList; }

    void                 SetNewBitmapList( XBitmapListRef pBmpLst)
                            { mpNewBitmapList = pBmpLst; }
    XBitmapListRef         GetNewBitmapList() const { return mpNewBitmapList; }
};

/************************************************************************/

class SvxTransparenceTabPage : public SvxTabPage
{
    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;
    static const sal_uInt16 pTransparenceRanges[];

    const SfxItemSet&   rOutAttrs;
    RECT_POINT          eRP;

    sal_uInt16             nPageType;
    sal_uInt16             nDlgType;

    // main selection
    VclPtr<RadioButton>        m_pRbtTransOff;
    VclPtr<RadioButton>        m_pRbtTransLinear;
    VclPtr<RadioButton>        m_pRbtTransGradient;

    /// linear transparency
    VclPtr<MetricField>        m_pMtrTransparent;

    // gradient transparency
    VclPtr<VclGrid>            m_pGridGradient;
    VclPtr<ListBox>            m_pLbTrgrGradientType;
    VclPtr<FixedText>          m_pFtTrgrCenterX;
    VclPtr<MetricField>        m_pMtrTrgrCenterX;
    VclPtr<FixedText>          m_pFtTrgrCenterY;
    VclPtr<MetricField>        m_pMtrTrgrCenterY;
    VclPtr<FixedText>          m_pFtTrgrAngle;
    VclPtr<MetricField>        m_pMtrTrgrAngle;
    VclPtr<MetricField>        m_pMtrTrgrBorder;
    VclPtr<MetricField>        m_pMtrTrgrStartValue;
    VclPtr<MetricField>        m_pMtrTrgrEndValue;

    // preview
    VclPtr<SvxXRectPreview>    m_pCtlBitmapPreview;
    VclPtr<SvxXRectPreview>    m_pCtlXRectPreview;
    bool                bBitmap;

    XFillAttrSetItem    aXFillAttr;
    SfxItemSet&         rXFSet;

    DECL_LINK(ClickTransOffHdl_Impl, void * );
    DECL_LINK(ClickTransLinearHdl_Impl, void * );
    DECL_LINK(ClickTransGradientHdl_Impl, void * );
    DECL_LINK(ModifyTransparentHdl_Impl, void*);
    DECL_LINK(ModifiedTrgrHdl_Impl, void*);

    void ActivateLinear(bool bActivate);
    void ActivateGradient(bool bActivate);
    void SetControlState_Impl(css::awt::GradientStyle eXGS);

    bool InitPreview ( const SfxItemSet& rSet );
    void InvalidatePreview (bool bEnable = true );

public:
    SvxTransparenceTabPage(vcl::Window* pParent, const SfxItemSet& rInAttrs);
    virtual ~SvxTransparenceTabPage();
    virtual void dispose() SAL_OVERRIDE;

    static VclPtr<SfxTabPage> Create(vcl::Window*, const SfxItemSet*);
    static const sal_uInt16* GetRanges() { return pTransparenceRanges; }

    virtual bool FillItemSet(SfxItemSet*) SAL_OVERRIDE;
    virtual void Reset(const SfxItemSet*) SAL_OVERRIDE;
    virtual void ActivatePage(const SfxItemSet& rSet) SAL_OVERRIDE;
    virtual sfxpg DeactivatePage(SfxItemSet* pSet) SAL_OVERRIDE;
    virtual void PointChanged(vcl::Window* pWindow, RECT_POINT eRP) SAL_OVERRIDE;

    void SetPageType(sal_uInt16 nInType) { nPageType = nInType; }
    void SetDlgType(sal_uInt16 nInType) { nDlgType = nInType; }
    virtual void PageCreated(const SfxAllItemSet& aSet) SAL_OVERRIDE;
};

/************************************************************************/

class SvxAreaTabPage : public SvxTabPage
{
    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;
    static const sal_uInt16 pAreaRanges[];
private:
    VclPtr<ListBox>            m_pTypeLB;

    VclPtr<VclBox>             m_pFillLB;
    VclPtr<ColorLB>            m_pLbColor;
    VclPtr<GradientLB>         m_pLbGradient;
    VclPtr<HatchingLB>         m_pLbHatching;
    VclPtr<BitmapLB>           m_pLbBitmap;
    VclPtr<SvxXRectPreview>    m_pCtlBitmapPreview;

    VclPtr<TriStateBox>        m_pTsbStepCount;
    VclPtr<VclFrame>           m_pFlStepCount;
    VclPtr<NumericField>       m_pNumFldStepCount;

    VclPtr<VclFrame>           m_pFlHatchBckgrd;
    VclPtr<CheckBox>           m_pCbxHatchBckgrd;
    VclPtr<ColorLB>            m_pLbHatchBckgrdColor;

    VclPtr<VclBox>             m_pBxBitmap;

    VclPtr<VclFrame>           m_pFlSize;
    VclPtr<TriStateBox>        m_pTsbOriginal;
    VclPtr<TriStateBox>        m_pTsbScale;
    VclPtr<VclGrid>            m_pGridX_Y;
    VclPtr<FixedText>          m_pFtXSize;
    VclPtr<MetricField>        m_pMtrFldXSize;
    VclPtr<FixedText>          m_pFtYSize;
    VclPtr<MetricField>        m_pMtrFldYSize;

    VclPtr<VclFrame>           m_pFlPosition;
    VclPtr<SvxRectCtl>         m_pCtlPosition;
    VclPtr<VclGrid>            m_pGridOffset;
    VclPtr<MetricField>        m_pMtrFldXOffset;
    VclPtr<MetricField>        m_pMtrFldYOffset;
    VclPtr<VclBox>             m_pBxTile;
    VclPtr<TriStateBox>        m_pTsbTile;
    VclPtr<TriStateBox>        m_pTsbStretch;
    VclPtr<VclFrame>           m_pFlOffset;
    VclPtr<RadioButton>        m_pRbtRow;
    VclPtr<RadioButton>        m_pRbtColumn;
    VclPtr<MetricField>        m_pMtrFldOffset;

    VclPtr<SvxXRectPreview>    m_pCtlXRectPreview;

    const SfxItemSet&   rOutAttrs;
    RECT_POINT          eRP;

    XColorListRef         pColorList;
    XGradientListRef      pGradientList;
    XHatchListRef         pHatchingList;
    XBitmapListRef        pBitmapList;

    // Placeholders for pointer-based entries; these will be inited
    // to point to these so that the page is usable without that
    // SvxAreaTabDialog has to call the setter methods (e.g. SetColorChgd).
    // Without that the pages used in SvxAreaTabDialog are not usable
    ChangeType          maFixed_ChangeType;
    bool                maFixed_sal_Bool;

    ChangeType*         pnColorListState;
    ChangeType*         pnBitmapListState;
    ChangeType*         pnGradientListState;
    ChangeType*         pnHatchingListState;

    sal_uInt16 nPageType;
    sal_uInt16 nDlgType;
    sal_Int32  nPos;

    bool*               pbAreaTP;

    XFillAttrSetItem    aXFillAttr;
    SfxItemSet&         rXFSet;

    SfxMapUnit          ePoolUnit;
    FieldUnit           eFUnit;

    //UUUU
    bool                mbOfferImportButton;
    bool                mbDirectGraphicSet;
    Graphic             maDirectGraphic;
    OUString            maDirectName;
    VclPtr<PushButton>         m_pBtnImport;

    DECL_LINK(SelectDialogTypeHdl_Impl, void *);
    DECL_LINK( ModifyColorHdl_Impl, void * );
    DECL_LINK( ModifyHatchBckgrdColorHdl_Impl, void * );
    DECL_LINK( ModifyGradientHdl_Impl, void * );
    DECL_LINK( ModifyHatchingHdl_Impl, void * );
    DECL_LINK( ToggleHatchBckgrdColorHdl_Impl, void * );
    DECL_LINK( ModifyBitmapHdl_Impl, void * );
    DECL_LINK( ModifyStepCountHdl_Impl, void * );

    //UUUU
    DECL_LINK( ClickImportHdl_Impl, void * );

    DECL_LINK( ModifyTileHdl_Impl, void * );
    DECL_LINK( ClickScaleHdl_Impl, void * );
    void ClickInvisibleHdl_Impl();
    void ClickHatchingHdl_Impl();
    void ClickGradientHdl_Impl();
    void ClickColorHdl_Impl();
    void ClickBitmapHdl_Impl();

public:
    SvxAreaTabPage( vcl::Window* pParent, const SfxItemSet& rInAttrs );
    virtual ~SvxAreaTabPage();
    virtual void dispose() SAL_OVERRIDE;

    void    Construct();

    static VclPtr<SfxTabPage> Create( vcl::Window*, const SfxItemSet* );
    static const sal_uInt16* GetRanges() { return pAreaRanges; }

    virtual bool FillItemSet( SfxItemSet* ) SAL_OVERRIDE;
    virtual void Reset( const SfxItemSet * ) SAL_OVERRIDE;
    virtual void ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;
    virtual sfxpg DeactivatePage( SfxItemSet* pSet ) SAL_OVERRIDE;
    virtual void PointChanged( vcl::Window* pWindow, RECT_POINT eRP ) SAL_OVERRIDE;

    void    SetColorList( XColorListRef pColTab ) { pColorList = pColTab; }
    void    SetGradientList( XGradientListRef pGrdLst)
                { pGradientList = pGrdLst; }
    void    SetHatchingList( XHatchListRef pHtchLst)
                { pHatchingList = pHtchLst; }
    void    SetBitmapList( XBitmapListRef pBmpLst) { pBitmapList = pBmpLst; }

    void    SetPageType( sal_uInt16 nInType ) { nPageType = nInType; }
    void    SetDlgType( sal_uInt16 nInType ) { nDlgType = nInType; }
    void    SetPos( sal_uInt16 nInPos ) { nPos = nInPos; }
    void    SetAreaTP( bool* pIn ) { pbAreaTP = pIn; }
    virtual void PageCreated(const SfxAllItemSet& aSet) SAL_OVERRIDE;
    void    SetColorChgd( ChangeType* pIn ) { pnColorListState = pIn; }
    void    SetGrdChgd( ChangeType* pIn ) { pnGradientListState = pIn; }
    void    SetHtchChgd( ChangeType* pIn ) { pnHatchingListState = pIn; }
    void    SetBmpChgd( ChangeType* pIn ) { pnBitmapListState = pIn; }
};


class SvxShadowTabPage : public SvxTabPage
{
    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;
    static const sal_uInt16 pShadowRanges[];

private:
    VclPtr<TriStateBox>        m_pTsbShowShadow;
    VclPtr<VclGrid>            m_pGridShadow;
    VclPtr<SvxRectCtl>         m_pCtlPosition;
    VclPtr<MetricField>        m_pMtrDistance;
    VclPtr<ColorLB>            m_pLbShadowColor;
    VclPtr<MetricField>        m_pMtrTransparent;
    VclPtr<SvxXShadowPreview>  m_pCtlXRectPreview;

    const SfxItemSet&   rOutAttrs;
    RECT_POINT          eRP;

    XColorListRef       pColorList;
    ChangeType*         pnColorListState;
    sal_uInt16          nPageType;
    sal_uInt16          nDlgType;
    bool*               pbAreaTP;

    bool                bDisable;

    XFillAttrSetItem    aXFillAttr;
    SfxItemSet&         rXFSet;
    SfxMapUnit          ePoolUnit;

    DECL_LINK( ClickShadowHdl_Impl, void * );
    DECL_LINK( ModifyShadowHdl_Impl, void * );

public:
    SvxShadowTabPage( vcl::Window* pParent, const SfxItemSet& rInAttrs  );
    virtual ~SvxShadowTabPage();
    virtual void dispose() SAL_OVERRIDE;

    void    Construct();
    static VclPtr<SfxTabPage> Create( vcl::Window*, const SfxItemSet* );
    static const sal_uInt16* GetRanges() { return pShadowRanges; }

    virtual bool FillItemSet( SfxItemSet* ) SAL_OVERRIDE;
    virtual void Reset( const SfxItemSet * ) SAL_OVERRIDE;
    virtual void ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;
    virtual sfxpg DeactivatePage( SfxItemSet* pSet ) SAL_OVERRIDE;
    virtual void PointChanged( vcl::Window* pWindow, RECT_POINT eRP ) SAL_OVERRIDE;

    void    SetColorList( XColorListRef pColTab ) { pColorList = pColTab; }
    void    SetPageType( sal_uInt16 nInType ) { nPageType = nInType; }
    void    SetDlgType( sal_uInt16 nInType ) { nDlgType = nInType; }
    void    SetAreaTP( bool* pIn ) { pbAreaTP = pIn; }
    void    SetColorChgd( ChangeType* pIn ) { pnColorListState = pIn; }
    virtual void PageCreated(const SfxAllItemSet& aSet) SAL_OVERRIDE;
};

/************************************************************************/

class SvxGradientTabPage : public SfxTabPage
{
    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

private:
    VclPtr<ListBox>            m_pLbGradientType;
    VclPtr<FixedText>          m_pFtCenterX;
    VclPtr<MetricField>        m_pMtrCenterX;
    VclPtr<FixedText>          m_pFtCenterY;
    VclPtr<MetricField>        m_pMtrCenterY;
    VclPtr<FixedText>          m_pFtAngle;
    VclPtr<MetricField>        m_pMtrAngle;
    VclPtr<MetricField>        m_pMtrBorder;
    VclPtr<ColorLB>            m_pLbColorFrom;
    VclPtr<MetricField>        m_pMtrColorFrom;
    VclPtr<ColorLB>            m_pLbColorTo;
    VclPtr<MetricField>        m_pMtrColorTo;
    VclPtr<GradientLB>         m_pLbGradients;
    VclPtr<SvxXRectPreview>    m_pCtlPreview;
    VclPtr<PushButton>         m_pBtnAdd;
    VclPtr<PushButton>         m_pBtnModify;
    VclPtr<PushButton>         m_pBtnDelete;
    VclPtr<PushButton>         m_pBtnLoad;
    VclPtr<PushButton>         m_pBtnSave;

    const SfxItemSet&   rOutAttrs;

    XColorListRef         pColorList;
    XGradientListRef      pGradientList;

    ChangeType*         pnGradientListState;
    ChangeType*         pnColorListState;
    sal_uInt16*         pPageType;
    sal_uInt16          nDlgType;
    sal_Int32*          pPos;
    bool*               pbAreaTP;

    XFillStyleItem      aXFStyleItem;
    XFillGradientItem   aXGradientItem;
    XFillAttrSetItem    aXFillAttr;
    SfxItemSet&         rXFSet;

    DECL_LINK( ClickAddHdl_Impl, void * );
    DECL_LINK( ClickModifyHdl_Impl, void * );
    DECL_LINK( ClickDeleteHdl_Impl, void * );
    DECL_LINK( ChangeGradientHdl_Impl, void * );
    DECL_LINK( ModifiedHdl_Impl, void * );
    DECL_LINK( ClickLoadHdl_Impl, void * );
    DECL_LINK( ClickSaveHdl_Impl, void * );

    long CheckChanges_Impl();
    void SetControlState_Impl( css::awt::GradientStyle eXGS );

public:
    SvxGradientTabPage( vcl::Window* pParent, const SfxItemSet& rInAttrs  );
    virtual ~SvxGradientTabPage();
    virtual void dispose() SAL_OVERRIDE;

    void    Construct();

    static VclPtr<SfxTabPage> Create( vcl::Window*, const SfxItemSet* );
    virtual bool FillItemSet( SfxItemSet* ) SAL_OVERRIDE;
    virtual void Reset( const SfxItemSet * ) SAL_OVERRIDE;

    virtual void ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;
    virtual sfxpg DeactivatePage( SfxItemSet* pSet ) SAL_OVERRIDE;

    void    SetColorList( XColorListRef pColTab ) { pColorList = pColTab; }
    void    SetGradientList( XGradientListRef pGrdLst)
                { pGradientList = pGrdLst; }

    void    SetPageType( sal_uInt16* pInType ) { pPageType = pInType; }
    void    SetDlgType( sal_uInt16 nInType ) { nDlgType = nInType; }
    void    SetPos( sal_Int32* pInPos ) { pPos = pInPos; }
    void    SetAreaTP( bool* pIn ) { pbAreaTP = pIn; }

    void    SetGrdChgd( ChangeType* pIn ) { pnGradientListState = pIn; }
    void    SetColorChgd( ChangeType* pIn ) { pnColorListState = pIn; }
};

/************************************************************************/

class SvxHatchTabPage : public SvxTabPage
{
    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

private:
    VclPtr<MetricField>        m_pMtrDistance;
    VclPtr<MetricField>        m_pMtrAngle;
    VclPtr<SvxRectCtl>         m_pCtlAngle;
    VclPtr<ListBox>            m_pLbLineType;
    VclPtr<ColorLB>            m_pLbLineColor;
    VclPtr<HatchingLB>         m_pLbHatchings;
    VclPtr<SvxXRectPreview>    m_pCtlPreview;
    VclPtr<PushButton>         m_pBtnAdd;
    VclPtr<PushButton>         m_pBtnModify;
    VclPtr<PushButton>         m_pBtnDelete;
    VclPtr<PushButton>         m_pBtnLoad;
    VclPtr<PushButton>         m_pBtnSave;

    const SfxItemSet&   rOutAttrs;

    XColorListRef         pColorList;
    XHatchListRef         pHatchingList;

    ChangeType*         pnHatchingListState;
    ChangeType*         pnColorListState;
    sal_uInt16*         pPageType;
    sal_uInt16          nDlgType;
    sal_Int32*          pPos;
    bool*               pbAreaTP;

    XFillStyleItem      aXFStyleItem;
    XFillHatchItem      aXHatchItem;
    XFillAttrSetItem    aXFillAttr;
    SfxItemSet&         rXFSet;

    SfxMapUnit          ePoolUnit;

    DECL_LINK( ChangeHatchHdl_Impl, void * );
    DECL_LINK( ModifiedHdl_Impl, void * );
    DECL_LINK( ClickAddHdl_Impl, void * );
    DECL_LINK( ClickModifyHdl_Impl, void * );
    DECL_LINK( ClickDeleteHdl_Impl, void * );
    DECL_LINK( ClickLoadHdl_Impl, void * );
    DECL_LINK( ClickSaveHdl_Impl, void * );

    long CheckChanges_Impl();

public:
    SvxHatchTabPage( vcl::Window* pParent, const SfxItemSet& rInAttrs  );
    virtual ~SvxHatchTabPage();
    virtual void dispose() SAL_OVERRIDE;

    void    Construct();

    static VclPtr<SfxTabPage> Create( vcl::Window*, const SfxItemSet* );
    virtual bool FillItemSet( SfxItemSet* ) SAL_OVERRIDE;
    virtual void Reset( const SfxItemSet * ) SAL_OVERRIDE;

    virtual void ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;
    virtual sfxpg DeactivatePage( SfxItemSet* pSet ) SAL_OVERRIDE;

    virtual void PointChanged( vcl::Window* pWindow, RECT_POINT eRP ) SAL_OVERRIDE;

    void    SetColorList( XColorListRef pColTab ) { pColorList = pColTab; }
    void    SetHatchingList( XHatchListRef pHtchLst)
                { pHatchingList = pHtchLst; }

    void    SetPageType( sal_uInt16* pInType ) { pPageType = pInType; }
    void    SetDlgType( sal_uInt16 nInType ) { nDlgType = nInType; }
    void    SetPos( sal_Int32* pInPos ) { pPos = pInPos; }
    void    SetAreaTP( bool* pIn ) { pbAreaTP = pIn; }

    void    SetHtchChgd( ChangeType* pIn ) { pnHatchingListState = pIn; }
    void    SetColorChgd( ChangeType* pIn ) { pnColorListState = pIn; }

    virtual void        DataChanged( const DataChangedEvent& rDCEvt ) SAL_OVERRIDE;
};

/************************************************************************/

class SvxBitmapTabPage : public SvxTabPage
{
    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

private:
    VclPtr<VclBox>             m_pBxPixelEditor;
    VclPtr<SvxPixelCtl>        m_pCtlPixel;
    VclPtr<ColorLB>            m_pLbColor;
    VclPtr<ColorLB>            m_pLbBackgroundColor;
    VclPtr<FixedText>          m_pLbBitmapsHidden;
    VclPtr<BitmapLB>           m_pLbBitmaps;
    VclPtr<SvxXRectPreview>    m_pCtlPreview;
    VclPtr<PushButton>         m_pBtnAdd;
    VclPtr<PushButton>         m_pBtnModify;
    VclPtr<PushButton>         m_pBtnImport;
    VclPtr<PushButton>         m_pBtnDelete;
    VclPtr<PushButton>         m_pBtnLoad;
    VclPtr<PushButton>         m_pBtnSave;

    SvxBitmapCtl*       m_pBitmapCtl;

    const SfxItemSet&   rOutAttrs;

    XColorListRef         pColorList;
    XBitmapListRef        pBitmapList;

    ChangeType*         pnBitmapListState;
    ChangeType*         pnColorListState;
    sal_uInt16*         pPageType;
    sal_uInt16          nDlgType;
    sal_Int32*          pPos;
    bool*               pbAreaTP;

    bool                bBmpChanged;

    XFillStyleItem      aXFStyleItem;
    XFillBitmapItem     aXBitmapItem;
    XFillAttrSetItem    aXFillAttr;
    SfxItemSet&         rXFSet;

    DECL_LINK( ClickAddHdl_Impl, void * );
    DECL_LINK( ClickImportHdl_Impl, void * );
    DECL_LINK( ClickModifyHdl_Impl, void * );
    DECL_LINK( ClickDeleteHdl_Impl, void * );
    DECL_LINK( ChangeBitmapHdl_Impl, void * );
    DECL_LINK( ChangePixelColorHdl_Impl, void * );
    DECL_LINK( ChangeBackgrndColorHdl_Impl, void * );
    DECL_LINK( ClickLoadHdl_Impl, void * );
    DECL_LINK( ClickSaveHdl_Impl, void * );

    long CheckChanges_Impl();

public:
    SvxBitmapTabPage( vcl::Window* pParent, const SfxItemSet& rInAttrs  );
    virtual ~SvxBitmapTabPage();
    virtual void dispose() SAL_OVERRIDE;

    void    Construct();

    static VclPtr<SfxTabPage> Create( vcl::Window*, const SfxItemSet* );
    virtual bool FillItemSet( SfxItemSet* ) SAL_OVERRIDE;
    virtual void Reset( const SfxItemSet * ) SAL_OVERRIDE;

    virtual void ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;
    virtual sfxpg DeactivatePage( SfxItemSet* pSet ) SAL_OVERRIDE;

    virtual void PointChanged( vcl::Window* pWindow, RECT_POINT eRP ) SAL_OVERRIDE;

    void    SetColorList( XColorListRef pColTab ) { pColorList = pColTab; }
    void    SetBitmapList( XBitmapListRef pBmpLst) { pBitmapList = pBmpLst; }

    void    SetPageType( sal_uInt16* pInType ) { pPageType = pInType; }
    void    SetDlgType( sal_uInt16 nInType ) { nDlgType = nInType; }
    void    SetPos( sal_Int32* pInPos ) { pPos = pInPos; }
    void    SetAreaTP( bool* pIn ) { pbAreaTP = pIn; }

    void    SetBmpChgd( ChangeType* pIn ) { pnBitmapListState = pIn; }
    void    SetColorChgd( ChangeType* pIn ) { pnColorListState = pIn; }

    /** Return a label that is associated with the given control.  This
        label is used to the determine the name for the control.
        @param pLabeled
            The control for which to return a label.
        @return
            Return a label control that provides a name for the specified
            control.
    */
    virtual vcl::Window* GetParentLabeledBy( const vcl::Window* pLabeled ) const SAL_OVERRIDE;
};

/************************************************************************/

struct SvxColorTabPageShadow;
class SvxColorTabPage : public SfxTabPage
{
    using TabPage::ActivatePage;
    using TabPage::DeactivatePage;

private:
    XPropertyListType   meType;

    VclPtr<Window>             mpTopDlg;
    VclPtr<CheckBox>           m_pBoxEmbed;
    VclPtr<PushButton>         m_pBtnLoad;
    VclPtr<PushButton>         m_pBtnSave;
    VclPtr<FixedText>          m_pTableName;

    DECL_LINK( EmbedToggleHdl_Impl, void * );
    DECL_LINK( ClickLoadHdl_Impl, void * );
    DECL_LINK( ClickSaveHdl_Impl, void * );

    XPropertyListRef GetList();
    bool GetEmbed();
    void SetEmbed( bool bEmbed );
    void UpdateTableName();
    void EnableSave( bool bCanSave );

    SvxColorTabPageShadow *pShadow;
    VclPtr<Edit>               m_pEdtName;
    VclPtr<ColorLB>            m_pLbColor;

    VclPtr<SvxColorValueSet>   m_pValSetColorList;

    VclPtr<SvxXRectPreview>    m_pCtlPreviewOld;
    VclPtr<SvxXRectPreview>    m_pCtlPreviewNew;

    VclPtr<ListBox>            m_pLbColorModel;

    VclPtr<VclContainer>       m_pRGB;
    VclPtr<NumericField>       m_pR;
    VclPtr<NumericField>       m_pG;
    VclPtr<NumericField>       m_pB;

    VclPtr<VclContainer>       m_pCMYK;
    VclPtr<MetricField>        m_pC;
    VclPtr<MetricField>        m_pY;
    VclPtr<MetricField>        m_pM;
    VclPtr<MetricField>        m_pK;

    VclPtr<PushButton>         m_pBtnAdd;
    VclPtr<PushButton>         m_pBtnModify;
    VclPtr<PushButton>         m_pBtnWorkOn;
    VclPtr<PushButton>         m_pBtnDelete;

    const SfxItemSet&   rOutAttrs;

    XColorListRef         pColorList;

    ChangeType*         pnColorListState;
    sal_uInt16*         pPageType;
    sal_uInt16          nDlgType;
    sal_Int32*          pPos;
    bool*               pbAreaTP;

    XFillStyleItem      aXFStyleItem;
    XFillColorItem      aXFillColorItem;
    XFillAttrSetItem    aXFillAttr;
    SfxItemSet&         rXFSet;

    ColorModel          eCM;

    Color               aCurrentColor;

    static void    ConvertColorValues (Color& rColor, ColorModel eModell);
    static void    RgbToCmyk_Impl( Color& rColor, sal_uInt16& rK );
    static void    CmykToRgb_Impl( Color& rColor, const sal_uInt16 nKey );
    sal_uInt16  ColorToPercent_Impl( sal_uInt16 nColor );
    sal_uInt16  PercentToColor_Impl( sal_uInt16 nPercent );

    void ImpColorCountChanged();


    DECL_LINK( ClickAddHdl_Impl, void * );
    DECL_LINK( ClickModifyHdl_Impl, void * );
    DECL_LINK( ClickDeleteHdl_Impl, void * );
    DECL_LINK( ClickWorkOnHdl_Impl, void * );

    DECL_LINK( SelectColorLBHdl_Impl, void * );
    DECL_LINK( SelectValSetHdl_Impl, void * );
    DECL_LINK( SelectColorModelHdl_Impl, void * );
    void ChangeColor(const Color &rNewColor);
    DECL_LINK( ModifiedHdl_Impl, void * );

    long CheckChanges_Impl();

    void UpdateModified();
public:
    SvxColorTabPage( vcl::Window* pParent, const SfxItemSet& rInAttrs );
    virtual ~SvxColorTabPage();
    virtual void dispose() SAL_OVERRIDE;

    void    Construct();

    static VclPtr<SfxTabPage> Create( vcl::Window*, const SfxItemSet* );
    virtual bool FillItemSet( SfxItemSet* ) SAL_OVERRIDE;
    virtual void Reset( const SfxItemSet * ) SAL_OVERRIDE;

    virtual void ActivatePage( const SfxItemSet& rSet ) SAL_OVERRIDE;
    virtual sfxpg DeactivatePage( SfxItemSet* pSet ) SAL_OVERRIDE;

    void             SetPropertyList( XPropertyListType t, const XPropertyListRef &xRef );

    void    SetColorList( XColorListRef pColList );
    XColorListRef GetColorList() { return pColorList; }
    void    SaveToViewFrame( SfxViewFrame *pViewFrame );
    void    SetupForViewFrame( SfxViewFrame *pViewFrame );

    void    SetPageType( sal_uInt16* pInType ) { pPageType = pInType; }
    void    SetDlgType( sal_uInt16 nInType ) { nDlgType = nInType; }
    void    SetPos( sal_Int32* pInPos ) { pPos = pInPos; }
    void    SetAreaTP( bool* pIn ) { pbAreaTP = pIn; }

    void    SetColorChgd( ChangeType* pIn ) { pnColorListState = pIn; }

    virtual void FillUserData() SAL_OVERRIDE;

    bool IsModified()
    {
        return bool(*pnColorListState & ChangeType::MODIFIED);
    }
    void SetModified(bool bIsModified)
    {
        if (bIsModified)
            *pnColorListState |= ChangeType::MODIFIED;
        else
            *pnColorListState &= ~ChangeType::MODIFIED;
    }
    void AddState(ChangeType nState)
    {
        *pnColorListState |= nState;
    }
    void Update(bool bLoaded);
};

#endif // INCLUDED_CUI_SOURCE_INC_CUITABAREA_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
