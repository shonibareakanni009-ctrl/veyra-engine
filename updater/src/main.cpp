/**
 * @file main.cpp
 * @brief Local fail-closed manifest and package verification/staging CLI.
 */
#include "manifest_verifier.h"

#include <array>
#include <iostream>
#include <string>

namespace
{
void printUsage()
{
    std::cout
        << "Veyra Updater 0.1.0 — local verification prototype\n"
        << "Usage:\n"
        << "  VeyraUpdater verify-local --manifest <manifest.json> --package <package>\n"
        << "      --staging-dir <directory> --key-id <trusted-id> --public-key-hex <64-hex>\n\n"
        << "The key must be supplied by a trusted local configuration, never copied from\n"
        << "the manifest or downloaded from the update server. This prototype does not\n"
        << "download or apply packages; it verifies and stages a local package only.\n";
}

bool readOption(int argc, char* argv[], int& index, std::string& output)
{
    if (index + 1 >= argc)
        return false;
    output = argv[++index];
    return true;
}
} // namespace

int main(int argc, char* argv[])
{
    if (argc == 2 && std::string(argv[1]) == "--help")
    {
        printUsage();
        return 0;
    }
    if (argc < 2 || std::string(argv[1]) != "verify-local")
    {
        printUsage();
        return 2;
    }

    std::string manifestPath;
    std::string packagePath;
    std::string stagingDirectory;
    std::string keyId;
    std::string publicKeyHex;
    for (int i = 2; i < argc; ++i)
    {
        const std::string option = argv[i];
        if (option == "--manifest")
        {
            if (!readOption(argc, argv, i, manifestPath)) { printUsage(); return 2; }
        }
        else if (option == "--package")
        {
            if (!readOption(argc, argv, i, packagePath)) { printUsage(); return 2; }
        }
        else if (option == "--staging-dir")
        {
            if (!readOption(argc, argv, i, stagingDirectory)) { printUsage(); return 2; }
        }
        else if (option == "--key-id")
        {
            if (!readOption(argc, argv, i, keyId)) { printUsage(); return 2; }
        }
        else if (option == "--public-key-hex")
        {
            if (!readOption(argc, argv, i, publicKeyHex)) { printUsage(); return 2; }
        }
        else
        {
            std::cerr << "Unknown option: " << option << '\n';
            printUsage();
            return 2;
        }
    }
    if (manifestPath.empty() || packagePath.empty() || stagingDirectory.empty() ||
        keyId.empty() || publicKeyHex.empty())
    {
        printUsage();
        return 2;
    }

    std::string error;
    Veyra::Updater::ReleaseManifest manifest;
    if (!Veyra::Updater::loadAndValidateManifest(manifestPath, manifest, &error))
    {
        std::cerr << "Manifest rejected: " << error << '\n';
        return 3;
    }

    std::array<std::uint8_t, 32> trustedKey{};
    if (!Veyra::Updater::decodeHex32(publicKeyHex, trustedKey, &error))
    {
        std::cerr << "Trusted key rejected: " << error << '\n';
        return 4;
    }
    if (!Veyra::Updater::verifyManifestSignature(manifest, keyId, trustedKey, &error))
    {
        std::cerr << "Signature rejected: " << error << '\n';
        return 5;
    }

    std::filesystem::path stagedPath;
    if (!Veyra::Updater::stageVerifiedPackage(packagePath, manifest,
                                               stagingDirectory, stagedPath, &error))
    {
        std::cerr << "Package rejected: " << error << '\n';
        return 6;
    }

    std::cout << "Manifest signature valid. Package digest and size valid.\n"
              << "Verified package staged at: " << stagedPath.string() << '\n'
              << "No executable was launched and no installation files were changed.\n";
    return 0;
}
