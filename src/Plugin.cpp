using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

#include "FissesPlugin.h"

namespace plugin {
    std::optional<std::filesystem::path> getLogDirectory() {
        PWSTR buf;
        SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &buf);
        std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> documentsPath{buf, CoTaskMemFree};
        std::filesystem::path directory{documentsPath.get()};
        directory.append("My Games"sv);

        if (SKYRIM_REL_VR_CONSTEXPR(REL::Module::IsVR())) {
            directory.append("Skyrim VR"sv);
        } else if (std::filesystem::exists("steam_api64.dll"sv)) {
            directory.append("Skyrim Special Edition"sv);
        } else if (std::filesystem::exists("Galaxy64.dll"sv)) {
            directory.append("Skyrim Special Edition GOG"sv);
        } else if (std::filesystem::exists("eossdk-win64-shipping.dll"sv)) {
            directory.append("Skyrim Special Edition EPIC"sv);
        } else {
            return std::filesystem::current_path().append("skselogs");
        }
        return directory.append("SKSE"sv);
    }

    void initializeLogging() {
        auto path = getLogDirectory();
        if (!path) {
            report_and_fail("Can't find SKSE log directory");
        }
        *path /= fmt::format("{}.log"sv, PluginDeclaration::GetSingleton()->GetName());

        std::shared_ptr<spdlog::logger> log;
        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        } else {
            log = std::make_shared<spdlog::logger>("Global",
                                                   std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }
        log->set_level(spdlog::level::info);
        log->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern(PLUGIN_LOGPATTERN_DEFAULT);
    }
}  // namespace plugin

using namespace plugin;

SKSEPluginLoad(const LoadInterface* skse) {
    initializeLogging();
    const auto* plugin = PluginDeclaration::GetSingleton();
    auto version = plugin->GetVersion();
    logger::info("'{} {}' is loading, game version '{}'...", plugin->GetName(), version, REL::Module::get().version());
    Init(skse);

    Fisses::initialize();

    logger::info("{} has finished loading.", plugin->GetName());
    return true;
}
