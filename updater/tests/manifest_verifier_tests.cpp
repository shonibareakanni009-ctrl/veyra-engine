/**
 * @file manifest_verifier_tests.cpp
 * @brief Tests signed manifest validation, package hashing, and safe staging.
 */
#include "manifest_verifier.h"

#include <monocypher-ed25519.h>
#include <nlohmann/json.hpp>

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace
{
using Json = nlohmann::json;
int failures = 0;

void check(bool condition, std::string_view message)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

std::string base64(const std::array<std::uint8_t, 64>& bytes)
{
    static constexpr char alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve(88);
    for (std::size_t i = 0; i < bytes.size(); i += 3U)
    {
        const std::uint32_t a = bytes[i];
        const std::uint32_t b = i + 1U < bytes.size() ? bytes[i + 1U] : 0U;
        const std::uint32_t c = i + 2U < bytes.size() ? bytes[i + 2U] : 0U;
        const std::uint32_t chunk = (a << 16U) | (b << 8U) | c;
        out.push_back(alphabet[(chunk >> 18U) & 63U]);
        out.push_back(alphabet[(chunk >> 12U) & 63U]);
        out.push_back(i + 1U < bytes.size() ? alphabet[(chunk >> 6U) & 63U] : '=');
        out.push_back(i + 2U < bytes.size() ? alphabet[chunk & 63U] : '=');
    }
    return out;
}

struct TestWorkspace
{
    std::filesystem::path root;
    TestWorkspace()
    {
        const auto token = std::chrono::steady_clock::now().time_since_epoch().count();
        root = std::filesystem::temp_directory_path() /
            ("veyra-updater-tests-" + std::to_string(token));
        std::filesystem::create_directories(root);
    }
    ~TestWorkspace()
    {
        std::error_code ec;
        std::filesystem::remove_all(root, ec);
    }
};

Json makeSignedManifest(const std::filesystem::path& package,
                        const std::array<std::uint8_t, 32>& seed,
                        std::array<std::uint8_t, 32>& publicKey)
{
    std::array<std::uint8_t, 64> secretKey{};
    crypto_ed25519_key_pair(secretKey.data(), publicKey.data(),
                            const_cast<std::uint8_t*>(seed.data()));

    std::array<std::uint8_t, 32> hash{};
    std::uint64_t size = 0U;
    std::string error;
    if (!Veyra::Updater::sha256File(package, hash, &size, &error))
        throw std::runtime_error(error);

    Json manifest = {
        {"schemaVersion", 1},
        {"product", "veyra-engine"},
        {"version", "0.2.1"},
        {"channel", "stable"},
        {"platform", "windows"},
        {"architecture", "x64"},
        {"minimumUpdaterVersion", "0.1.0"},
        {"releaseNotesUrl", "https://example.invalid/releases/0.2.1"},
        {"package", {
            {"url", "https://example.invalid/releases/VeyraEngine-0.2.1.exe"},
            {"sizeBytes", size},
            {"sha256", Veyra::Updater::toHex(hash)}
        }},
        {"signature", {
            {"algorithm", "Ed25519"},
            {"keyId", "test-release-key"},
            {"value", base64(std::array<std::uint8_t, 64>{})}
        }}
    };

    Json payload = manifest;
    payload.erase("signature");
    const std::string canonical = payload.dump();
    std::array<std::uint8_t, 64> signature{};
    crypto_ed25519_sign(signature.data(), secretKey.data(),
        reinterpret_cast<const std::uint8_t*>(canonical.data()), canonical.size());
    manifest["signature"]["value"] = base64(signature);
    return manifest;
}

void writeJson(const std::filesystem::path& path, const Json& value)
{
    std::ofstream file(path, std::ios::binary);
    file << value.dump(2);
    if (!file)
        throw std::runtime_error("Could not write test manifest.");
}

void testValidManifestAndStage()
{
    TestWorkspace workspace;
    const auto package = workspace.root / "test-package.bin";
    {
        std::ofstream file(package, std::ios::binary);
        file << "Veyra test update package payload";
    }

    std::array<std::uint8_t, 32> seed{};
    for (std::size_t i = 0; i < seed.size(); ++i)
        seed[i] = static_cast<std::uint8_t>(i + 1U);
    std::array<std::uint8_t, 32> publicKey{};
    Json signedManifest = makeSignedManifest(package, seed, publicKey);
    const auto manifestPath = workspace.root / "manifest.json";
    writeJson(manifestPath, signedManifest);

    Veyra::Updater::ReleaseManifest parsed;
    std::string error;
    check(Veyra::Updater::loadAndValidateManifest(manifestPath, parsed, &error),
          "signed test manifest validates");
    check(Veyra::Updater::verifyManifestSignature(parsed, "test-release-key", publicKey, &error),
          "valid Ed25519 signature verifies");
    check(Veyra::Updater::verifyPackage(package, parsed, &error),
          "package size and SHA-256 match");

    std::filesystem::path staged;
    check(Veyra::Updater::stageVerifiedPackage(
        package, parsed, workspace.root / "staging", staged, &error),
        "verified package stages atomically");
    check(std::filesystem::exists(staged), "staged package exists");
    check(Veyra::Updater::verifyPackage(staged, parsed, &error),
          "staged copy still matches the signed digest");

    std::array<std::uint8_t, 32> wrongKey{};
    check(!Veyra::Updater::verifyManifestSignature(parsed, "test-release-key", wrongKey, &error),
          "wrong public key is rejected");
    check(!Veyra::Updater::verifyManifestSignature(parsed, "another-key", publicKey, &error),
          "wrong key ID is rejected");
    check(!Veyra::Updater::stageVerifiedPackage(
        package, parsed, workspace.root / "staging", staged, &error),
        "staging refuses to overwrite an existing package");
}

void testPackageTamperingAndStrictManifest()
{
    TestWorkspace workspace;
    const auto package = workspace.root / "payload.bin";
    {
        std::ofstream file(package, std::ios::binary);
        file << "original package";
    }

    std::array<std::uint8_t, 32> seed{};
    seed.fill(0x5AU);
    std::array<std::uint8_t, 32> publicKey{};
    Json manifest = makeSignedManifest(package, seed, publicKey);
    const auto manifestPath = workspace.root / "manifest.json";
    writeJson(manifestPath, manifest);

    Veyra::Updater::ReleaseManifest parsed;
    std::string error;
    check(Veyra::Updater::loadAndValidateManifest(manifestPath, parsed, &error),
          "second signed manifest validates");
    {
        std::ofstream file(package, std::ios::binary | std::ios::trunc);
        file << "tampered package";
    }
    check(!Veyra::Updater::verifyPackage(package, parsed, &error),
          "tampered package is rejected");

    manifest["unexpected"] = true;
    writeJson(manifestPath, manifest);
    check(!Veyra::Updater::loadAndValidateManifest(manifestPath, parsed, &error),
          "unexpected top-level fields are rejected");

    manifest.erase("unexpected");
    manifest["platform"] = "linux";
    writeJson(manifestPath, manifest);
    check(!Veyra::Updater::loadAndValidateManifest(manifestPath, parsed, &error),
          "wrong platform is rejected");

    manifest["platform"] = "windows";
    manifest["package"]["url"] = "http://example.invalid/payload";
    writeJson(manifestPath, manifest);
    check(!Veyra::Updater::loadAndValidateManifest(manifestPath, parsed, &error),
          "non-HTTPS package URL is rejected");
}

void testMalformedManifestAndHex()
{
    TestWorkspace workspace;
    const auto manifestPath = workspace.root / "malformed.json";
    {
        std::ofstream file(manifestPath, std::ios::binary);
        file << "{";
    }
    Veyra::Updater::ReleaseManifest parsed;
    std::string error;
    check(!Veyra::Updater::loadAndValidateManifest(manifestPath, parsed, &error),
          "malformed JSON is rejected");

    std::array<std::uint8_t, 32> key{};
    check(!Veyra::Updater::decodeHex32("abcd", key, &error),
          "short trusted public key is rejected");
    check(!Veyra::Updater::decodeHex32(std::string(64, 'z'), key, &error),
          "non-hex trusted public key is rejected");
}
} // namespace

int main()
{
    try
    {
        testValidManifestAndStage();
        testPackageTamperingAndStrictManifest();
        testMalformedManifestAndHex();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "UNCAUGHT TEST ERROR: " << exception.what() << '\n';
        return 2;
    }

    if (failures != 0)
    {
        std::cerr << failures << " updater test(s) failed.\n";
        return 1;
    }
    std::cout << "All updater manifest and package verification tests passed.\n";
    return 0;
}
