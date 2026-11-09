/**
 * @file manifest_verifier.h
 * @brief Strict manifest validation, Ed25519 verification, SHA-256, and safe staging.
 */
#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace Veyra::Updater
{
struct ReleaseManifest
{
    std::string keyId;
    std::string version;
    std::string channel;
    std::string packageUrl;
    std::uint64_t packageSizeBytes = 0;
    std::array<std::uint8_t, 32> packageSha256{};
    std::array<std::uint8_t, 64> signature{};
    std::string canonicalSignedPayload;
};

[[nodiscard]] bool loadAndValidateManifest(
    const std::filesystem::path& manifestPath,
    ReleaseManifest& manifest,
    std::string* error = nullptr);

[[nodiscard]] bool decodeHex32(
    std::string_view hex,
    std::array<std::uint8_t, 32>& output,
    std::string* error = nullptr);

[[nodiscard]] bool verifyManifestSignature(
    const ReleaseManifest& manifest,
    std::string_view expectedKeyId,
    const std::array<std::uint8_t, 32>& trustedPublicKey,
    std::string* error = nullptr);

[[nodiscard]] bool sha256File(
    const std::filesystem::path& path,
    std::array<std::uint8_t, 32>& digest,
    std::uint64_t* sizeBytes = nullptr,
    std::string* error = nullptr);

[[nodiscard]] bool verifyPackage(
    const std::filesystem::path& packagePath,
    const ReleaseManifest& manifest,
    std::string* error = nullptr);

[[nodiscard]] bool stageVerifiedPackage(
    const std::filesystem::path& packagePath,
    const ReleaseManifest& manifest,
    const std::filesystem::path& stagingDirectory,
    std::filesystem::path& stagedPath,
    std::string* error = nullptr);

[[nodiscard]] std::string toHex(
    const std::array<std::uint8_t, 32>& bytes);
} // namespace Veyra::Updater
