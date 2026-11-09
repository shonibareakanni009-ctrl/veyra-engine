/**
 * @file manifest_verifier.cpp
 * @brief Implements fail-closed release manifest and package verification.
 */
#include "manifest_verifier.h"

#include <windows.h>
#include <bcrypt.h>

#include <monocypher-ed25519.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <limits>
#include <regex>
#include <set>
#include <string>
#include <vector>

namespace Veyra::Updater
{
namespace
{
using Json = nlohmann::json;
constexpr std::size_t MaximumManifestBytes = 1024U * 1024U;
constexpr std::uint64_t MaximumPackageBytes = 2ULL * 1024ULL * 1024ULL * 1024ULL;

bool fail(std::string* error, std::string message)
{
    if (error != nullptr)
        *error = std::move(message);
    return false;
}

bool hasExactlyKeys(const Json& object, const std::set<std::string>& expected)
{
    if (!object.is_object() || object.size() != expected.size())
        return false;
    for (auto it = object.begin(); it != object.end(); ++it)
        if (!expected.contains(it.key()))
            return false;
    return true;
}

bool isAsciiPrintable(std::string_view value)
{
    return std::all_of(value.begin(), value.end(),
        [](unsigned char ch) { return ch >= 0x21U && ch <= 0x7EU; });
}

bool isHttpsUrl(const std::string& value)
{
    static const std::regex pattern(
        R"(^https://[A-Za-z0-9.-]+(?::[0-9]{1,5})?(?:/[A-Za-z0-9._~:/?#\[\]@!$&'()*+,;=%-]*)?$)");
    return std::regex_match(value, pattern);
}

int hexValue(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

bool decodeBase64(std::string_view input, std::array<std::uint8_t, 64>& output)
{
    static constexpr std::string_view alphabet =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (input.size() != 88U || input.substr(86U) != "==")
        return false;

    std::vector<std::uint8_t> decoded;
    decoded.reserve(66U);
    std::uint32_t accumulator = 0U;
    int bits = 0;
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        const char c = input[i];
        if (c == '=')
        {
            if (i < 86U)
                return false;
            continue;
        }
        const auto pos = alphabet.find(c);
        if (pos == std::string_view::npos)
            return false;
        accumulator = (accumulator << 6U) | static_cast<std::uint32_t>(pos);
        bits += 6;
        if (bits >= 8)
        {
            bits -= 8;
            decoded.push_back(static_cast<std::uint8_t>((accumulator >> bits) & 0xFFU));
        }
    }
    if (decoded.size() != output.size())
        return false;
    std::copy(decoded.begin(), decoded.end(), output.begin());
    return true;
}

bool parseSha256(std::string_view value, std::array<std::uint8_t, 32>& output)
{
    if (value.size() != 64U)
        return false;
    for (std::size_t i = 0; i < output.size(); ++i)
    {
        const int high = hexValue(value[i * 2U]);
        const int low = hexValue(value[i * 2U + 1U]);
        if (high < 0 || low < 0)
            return false;
        output[i] = static_cast<std::uint8_t>((high << 4) | low);
    }
    return true;
}

bool readFileBounded(const std::filesystem::path& path, std::string& output,
                     std::string* error)
{
    std::error_code ec;
    const auto size = std::filesystem::file_size(path, ec);
    if (ec)
        return fail(error, "Could not read the manifest file.");
    if (size == 0U || size > MaximumManifestBytes)
        return fail(error, "Manifest must be between 1 byte and 1 MiB.");
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return fail(error, "Could not open the manifest file.");
    output.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    if (!file.eof() && file.fail())
        return fail(error, "An I/O error occurred while reading the manifest.");
    if (output.size() != size)
        return fail(error, "Manifest changed while it was being read.");
    return true;
}

} // namespace

bool loadAndValidateManifest(const std::filesystem::path& manifestPath,
                             ReleaseManifest& manifest, std::string* error)
{
    std::string bytes;
    if (!readFileBounded(manifestPath, bytes, error))
        return false;

    Json document;
    try
    {
        document = Json::parse(bytes);
    }
    catch (const Json::exception&)
    {
        return fail(error, "Manifest is not valid JSON.");
    }

    const std::set<std::string> topKeys = {
        "schemaVersion", "product", "version", "channel", "platform",
        "architecture", "minimumUpdaterVersion", "releaseNotesUrl",
        "package", "signature"
    };
    if (!hasExactlyKeys(document, topKeys))
        return fail(error, "Manifest contains missing or unexpected top-level fields.");

    if (!document["schemaVersion"].is_number_integer() ||
        document["schemaVersion"].get<int>() != 1 ||
        document["product"] != "veyra-engine" ||
        document["platform"] != "windows" ||
        document["architecture"] != "x64")
        return fail(error, "Manifest schema, product, platform, or architecture is unsupported.");

    const auto version = document["version"];
    const auto channel = document["channel"];
    const auto minimumUpdater = document["minimumUpdaterVersion"];
    const auto releaseNotes = document["releaseNotesUrl"];
    if (!version.is_string() || !channel.is_string() ||
        !minimumUpdater.is_string() || !releaseNotes.is_string())
        return fail(error, "Manifest version, channel, or URL fields have invalid types.");

    const std::string versionText = version.get<std::string>();
    const std::string updaterVersionText = minimumUpdater.get<std::string>();
    const std::string channelText = channel.get<std::string>();
    static const std::regex versionPattern(
        R"(^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)(?:-[0-9A-Za-z.-]+)?$)");
    static const std::regex updaterVersionPattern(
        R"(^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$)");
    if (!std::regex_match(versionText, versionPattern) ||
        !std::regex_match(updaterVersionText, updaterVersionPattern) ||
        (channelText != "stable" && channelText != "beta" && channelText != "developer"))
        return fail(error, "Manifest version or release channel is invalid.");

    const std::string notesUrl = releaseNotes.get<std::string>();
    if (!isAsciiPrintable(notesUrl) || !isHttpsUrl(notesUrl))
        return fail(error, "Release notes URL must be a valid HTTPS URL.");

    const Json& package = document["package"];
    if (!hasExactlyKeys(package, {"url", "sizeBytes", "sha256"}) ||
        !package["url"].is_string() ||
        !package["sizeBytes"].is_number_unsigned() &&
         !(package["sizeBytes"].is_number_integer() && package["sizeBytes"].get<std::int64_t>() > 0) ||
        !package["sha256"].is_string())
        return fail(error, "Manifest package metadata is missing or invalid.");

    const std::string packageUrl = package["url"].get<std::string>();
    const std::uint64_t packageSize = package["sizeBytes"].get<std::uint64_t>();
    if (!isAsciiPrintable(packageUrl) || !isHttpsUrl(packageUrl) ||
        packageSize == 0U || packageSize > MaximumPackageBytes)
        return fail(error, "Package URL or package size is outside the supported bounds.");

    std::array<std::uint8_t, 32> packageHash{};
    if (!parseSha256(package["sha256"].get<std::string>(), packageHash))
        return fail(error, "Package SHA-256 must contain exactly 64 hexadecimal characters.");

    const Json& signature = document["signature"];
    if (!hasExactlyKeys(signature, {"algorithm", "keyId", "value"}) ||
        signature["algorithm"] != "Ed25519" ||
        !signature["keyId"].is_string() ||
        !signature["value"].is_string())
        return fail(error, "Manifest signature metadata is missing or unsupported.");

    const std::string keyId = signature["keyId"].get<std::string>();
    if (keyId.empty() || keyId.size() > 64U ||
        !std::all_of(keyId.begin(), keyId.end(), [](unsigned char c)
        {
            return std::isalnum(c) || c == '.' || c == '_' || c == '-';
        }))
        return fail(error, "Manifest key identifier is invalid.");

    std::array<std::uint8_t, 64> signatureBytes{};
    if (!decodeBase64(signature["value"].get<std::string>(), signatureBytes))
        return fail(error, "Manifest Ed25519 signature must be a valid 64-byte Base64 value.");

    // nlohmann::json uses lexicographically ordered object keys. All strings
    // accepted by this manifest contract are printable ASCII, and numeric
    // fields are integers, so this compact serialization is the supported JCS subset.
    Json signedPayload = document;
    signedPayload.erase("signature");
    const std::string canonical = signedPayload.dump(-1, ' ', false, Json::error_handler_t::strict);

    manifest.keyId = keyId;
    manifest.version = versionText;
    manifest.channel = channelText;
    manifest.packageUrl = packageUrl;
    manifest.packageSizeBytes = packageSize;
    manifest.packageSha256 = packageHash;
    manifest.signature = signatureBytes;
    manifest.canonicalSignedPayload = canonical;
    if (error != nullptr)
        error->clear();
    return true;
}

bool decodeHex32(std::string_view hex, std::array<std::uint8_t, 32>& output,
                 std::string* error)
{
    if (hex.size() != output.size() * 2U)
        return fail(error, "Trusted Ed25519 public key must be exactly 64 hexadecimal characters.");
    for (std::size_t i = 0; i < output.size(); ++i)
    {
        const int high = hexValue(hex[i * 2U]);
        const int low = hexValue(hex[i * 2U + 1U]);
        if (high < 0 || low < 0)
            return fail(error, "Trusted Ed25519 public key contains non-hexadecimal characters.");
        output[i] = static_cast<std::uint8_t>((high << 4) | low);
    }
    return true;
}

bool verifyManifestSignature(const ReleaseManifest& manifest,
                             std::string_view expectedKeyId,
                             const std::array<std::uint8_t, 32>& trustedPublicKey,
                             std::string* error)
{
    if (manifest.keyId != expectedKeyId)
        return fail(error, "Manifest key identifier does not match the locally trusted key.");
    const int valid = crypto_ed25519_check(
        manifest.signature.data(), trustedPublicKey.data(),
        reinterpret_cast<const std::uint8_t*>(manifest.canonicalSignedPayload.data()),
        manifest.canonicalSignedPayload.size());
    if (valid != 0)
        return fail(error, "Manifest Ed25519 signature verification failed.");
    if (error != nullptr)
        error->clear();
    return true;
}

bool sha256File(const std::filesystem::path& path,
                std::array<std::uint8_t, 32>& digest,
                std::uint64_t* sizeBytes, std::string* error)
{
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    std::vector<std::uint8_t> hashObject;
    bool success = false;

    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0U) < 0)
        return fail(error, "Windows CNG could not open the SHA-256 provider.");

    DWORD objectSize = 0U;
    DWORD hashSize = 0U;
    DWORD returned = 0U;
    if (BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH,
                          reinterpret_cast<PUCHAR>(&objectSize), sizeof(objectSize),
                          &returned, 0U) < 0 ||
        BCryptGetProperty(algorithm, BCRYPT_HASH_LENGTH,
                          reinterpret_cast<PUCHAR>(&hashSize), sizeof(hashSize),
                          &returned, 0U) < 0 ||
        hashSize != digest.size())
    {
        BCryptCloseAlgorithmProvider(algorithm, 0U);
        return fail(error, "Windows CNG could not query SHA-256 parameters.");
    }
    hashObject.resize(objectSize);
    if (BCryptCreateHash(algorithm, &hash, hashObject.data(), objectSize,
                         nullptr, 0U, 0U) < 0)
    {
        BCryptCloseAlgorithmProvider(algorithm, 0U);
        return fail(error, "Windows CNG could not initialize SHA-256.");
    }

    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        BCryptDestroyHash(hash);
        BCryptCloseAlgorithmProvider(algorithm, 0U);
        return fail(error, "Could not open the update package for hashing.");
    }

    std::vector<char> buffer(64U * 1024U);
    std::uint64_t total = 0U;
    while (file)
    {
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        const std::streamsize readCount = file.gcount();
        if (readCount <= 0)
            break;
        total += static_cast<std::uint64_t>(readCount);
        if (BCryptHashData(hash, reinterpret_cast<PUCHAR>(buffer.data()),
                           static_cast<ULONG>(readCount), 0U) < 0)
        {
            file.setstate(std::ios::badbit);
            break;
        }
    }

    if (!file.eof() || file.bad() ||
        BCryptFinishHash(hash, digest.data(), static_cast<ULONG>(digest.size()), 0U) < 0)
    {
        BCryptDestroyHash(hash);
        BCryptCloseAlgorithmProvider(algorithm, 0U);
        return fail(error, "An I/O or cryptographic error occurred while hashing the update package.");
    }

    if (sizeBytes != nullptr)
        *sizeBytes = total;
    success = true;
    BCryptDestroyHash(hash);
    BCryptCloseAlgorithmProvider(algorithm, 0U);
    if (error != nullptr && success)
        error->clear();
    return success;
}

bool verifyPackage(const std::filesystem::path& packagePath,
                   const ReleaseManifest& manifest, std::string* error)
{
    std::array<std::uint8_t, 32> actualHash{};
    std::uint64_t actualSize = 0U;
    if (!sha256File(packagePath, actualHash, &actualSize, error))
        return false;
    if (actualSize != manifest.packageSizeBytes)
        return fail(error, "Update package size does not match the signed manifest.");
    if (actualHash != manifest.packageSha256)
        return fail(error, "Update package SHA-256 does not match the signed manifest.");
    if (error != nullptr)
        error->clear();
    return true;
}

bool stageVerifiedPackage(const std::filesystem::path& packagePath,
                          const ReleaseManifest& manifest,
                          const std::filesystem::path& stagingDirectory,
                          std::filesystem::path& stagedPath,
                          std::string* error)
{
    if (!verifyPackage(packagePath, manifest, error))
        return false;

    std::error_code ec;
    std::filesystem::create_directories(stagingDirectory, ec);
    if (ec)
        return fail(error, "Could not create the update staging directory.");
    const auto status = std::filesystem::symlink_status(stagingDirectory, ec);
    if (ec || std::filesystem::is_symlink(status) ||
        !std::filesystem::is_directory(status))
        return fail(error, "Update staging path must be a real directory, not a symlink.");

    const std::filesystem::path destination =
        std::filesystem::absolute(stagingDirectory) /
        ("veyra-engine-" + manifest.version + ".verified-package");
    const std::filesystem::path temporary = destination.string() + ".partial";
    if (std::filesystem::exists(destination) || std::filesystem::exists(temporary))
        return fail(error, "A staged package for this version already exists; refusing to overwrite it.");

    if (!std::filesystem::copy_file(packagePath, temporary,
                                    std::filesystem::copy_options::none, ec) || ec)
        return fail(error, "Could not copy the verified package into staging.");

    if (!verifyPackage(temporary, manifest, error))
    {
        std::filesystem::remove(temporary, ec);
        return false;
    }
    std::filesystem::rename(temporary, destination, ec);
    if (ec)
    {
        std::filesystem::remove(temporary, ec);
        return fail(error, "Could not atomically finalize the staged package.");
    }

    stagedPath = destination;
    if (error != nullptr)
        error->clear();
    return true;
}

std::string toHex(const std::array<std::uint8_t, 32>& bytes)
{
    static constexpr char digits[] = "0123456789abcdef";
    std::string result;
    result.reserve(bytes.size() * 2U);
    for (const std::uint8_t byte : bytes)
    {
        result.push_back(digits[byte >> 4U]);
        result.push_back(digits[byte & 0x0FU]);
    }
    return result;
}
} // namespace Veyra::Updater
