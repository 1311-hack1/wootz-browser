// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/network/attribution/boringssl_verification_cryptographer.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "services/network/public/mojom/trust_tokens.mojom-shared.h"
#include "services/network/trust_tokens/boringssl_trust_token_test_utils.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/boringssl/src/include/openssl/base.h"
#include "third_party/boringssl/src/include/openssl/trust_token.h"

namespace network {
namespace {

const network::mojom::TrustTokenProtocolVersion kProtocolVersion =
    network::mojom::TrustTokenProtocolVersion::kTrustTokenV3Pmb;

class BoringsslVerificationCryptographerTest : public testing::Test {
 protected:
  void SetUp() override {
    issuer_ = std::make_unique<TestTrustTokenIssuer>(/*num_keys=*/1,
                                                     /*max_issuance=*/1);
    cryptographer_ = std::make_unique<BoringsslVerificationCryptographer>();
  }

  std::unique_ptr<TestTrustTokenIssuer> issuer_;
  std::unique_ptr<BoringsslVerificationCryptographer> cryptographer_;
};

TEST_F(BoringsslVerificationCryptographerTest, IssuanceAndRedemption) {
  constexpr char kMessage[] = "test-message";

  ASSERT_TRUE(cryptographer_->Initialize(kProtocolVersion));
  for (const TestTrustTokenIssuer::VerificationKey& verification_key :
       issuer_->Keys()) {
    ASSERT_TRUE(cryptographer_->AddKey(std::string(
        reinterpret_cast<const char*>(verification_key.value.data()),
        verification_key.value.size())));
  }

  std::optional<std::string> maybe_blind_message =
      cryptographer_->BeginIssuance(kMessage);
  ASSERT_TRUE(maybe_blind_message.has_value());

  std::optional<std::string> maybe_issuance_response =
      issuer_->Issue(*maybe_blind_message);
  ASSERT_TRUE(maybe_issuance_response.has_value());

  // Sending invalid data should not return a verification string
  ASSERT_FALSE(cryptographer_
                   ->ConfirmIssuanceAndBeginRedemption(
                       /*response_header=*/"some invalid data")
                   .has_value());

  std::optional<std::string> maybe_verification_string =
      cryptographer_->ConfirmIssuanceAndBeginRedemption(
          /*response_header=*/maybe_issuance_response.value());

  ASSERT_TRUE(maybe_verification_string.has_value());

  // Redeeming with an invalid message should not return a trust token
  ASSERT_FALSE(issuer_->RedeemOverMessage(*maybe_verification_string,
                                          /*message=*/"some invalid message"));

  bssl::UniquePtr<TRUST_TOKEN> trust_token = issuer_->RedeemOverMessage(
      *maybe_verification_string, /*message=*/kMessage);
  ASSERT_TRUE(trust_token);
}

}  // namespace
}  // namespace network
