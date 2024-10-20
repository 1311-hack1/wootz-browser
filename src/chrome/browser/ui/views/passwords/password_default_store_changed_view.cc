// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/passwords/password_default_store_changed_view.h"

#include "chrome/browser/ui/passwords/passwords_model_delegate.h"
#include "chrome/browser/ui/passwords/ui_utils.h"
#include "ui/views/layout/fill_layout.h"

PasswordDefaultStoreChangedView::PasswordDefaultStoreChangedView(
    content::WebContents* web_contents,
    views::View* anchor_view)
    : PasswordBubbleViewBase(web_contents,
                             anchor_view,
                             /*easily_dismissable=*/true),
      controller_(PasswordsModelDelegateFromWebContents(web_contents)) {
  SetLayoutManager(std::make_unique<views::FillLayout>());

  SetButtons((ui::DIALOG_BUTTON_OK | ui::DIALOG_BUTTON_CANCEL));
  SetButtonLabel(ui::DIALOG_BUTTON_OK, controller_.GetContinueButtonText());
  SetButtonLabel(ui::DIALOG_BUTTON_CANCEL,
                 controller_.GetGoToSettingsButtonText());

  SetShowIcon(true);
  SetTitle(controller_.GetTitle());

  auto label = std::make_unique<views::Label>();
  label->SetText(controller_.GetBody());
  label->SetMultiLine(/*multi_line=*/true);
  label->SetHorizontalAlignment(gfx::HorizontalAlignment::ALIGN_LEFT);
  AddChildView(std::move(label));

  SetAcceptCallback(base::BindOnce(
      &DefaultStoreChangedBubbleController::OnContinueButtonClicked,
      base::Unretained(&controller_)));
  SetCancelCallback(base::BindOnce(
      &DefaultStoreChangedBubbleController::OnNavigateToSettingsButtonClicked,
      base::Unretained(&controller_)));
}

PasswordDefaultStoreChangedView::~PasswordDefaultStoreChangedView() = default;

DefaultStoreChangedBubbleController*
PasswordDefaultStoreChangedView::GetController() {
  return &controller_;
}

const DefaultStoreChangedBubbleController*
PasswordDefaultStoreChangedView::GetController() const {
  return &controller_;
}

ui::ImageModel PasswordDefaultStoreChangedView::GetWindowIcon() {
  return ui::ImageModel::FromVectorIcon(GooglePasswordManagerVectorIcon(),
                                        ui::kColorIcon);
}
