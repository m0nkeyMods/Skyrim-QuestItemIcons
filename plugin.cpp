#pragma once

#include "include/LookupConfigs.h"
#include "LookupForms.hpp"

// This is a snippet you can put at the top of all of your SKSE plugins!

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
}

// Then just call SetupLog() in your SKSE plugin initialization
//
// ^---- don't forget to do this or your logs won't work :)

// string[] function FetchConfigs() global native
std::vector<std::string> FetchConfigs(RE::StaticFunctionTag *) {
    // Fetches all files ending in _ITM from Data/
    std::vector<std::string> files = clib_util::distribution::get_configs(R"(Data\)", "_ITM");
    // Store strings to be returned on SKSE call
    std::vector<std::string> configs;

    if (files.empty()) {
        logger::error("\nUnable to find _ITM files in the current directory. Make sure you're running this from the Skyrim Data "
            "folder!\n");
    }

    for (std::string path : files) {
        RE::ConsoleLog::GetSingleton()->Print(path.c_str());

        CSimpleIniA ini;
        ini.SetUnicode();
        ini.SetMultiKey();

        if (const auto rc = ini.LoadFile(path.c_str()); rc < 0) {
            logger::error("Couldnt read ini");
            continue;
        }

        if (auto values = ini.GetSection(""); values && !values->empty()) {
            auto truncatedPath = path.substr(5);  // strip "Data\\"

            for (auto &[key, entry] : *values) {
                try {
                    std::string entryString = entry;
                    std::string keyString = key.pItem;
                    logger::info("entry {}", entryString);
                    logger::info("value {}", keyString);

                    // TODO see FormData.h in SPID for example of handling FormOrEditorID
                    // TODO move this to a function
                    // TODO sanitize string
                    // Right now only filters is quest
                    const auto sections = clib_util::string::split(entry, "|");
                    const auto recordSplit = clib_util::string::split(sections[0], "?");
                    FormOrEditorID record = clib_util::distribution::get_record(recordSplit[0]);
                    std::string recordString = LookupForms::fetchFormId(record);
                    std::string convertedLine = "";
                    if (sections.size() > 1) {
                        FormOrEditorID quest = clib_util::distribution::get_record(sections[1]);
                        std::string questString = LookupForms::fetchFormId(record);
                        convertedLine = std::format("{}|{}", recordString, questString);
                    } else if (recordSplit.size() > 1) {
                        convertedLine = std::format("{}?{}", recordString, recordSplit[1]);
                    } else {
                        convertedLine = recordString;
                    }

                    configs.push_back(std::format("{}={}", key.pItem, convertedLine));
                } catch (...) {
                    std::string entryString = entry;
                    logger::error("Failed to parse entry {}", entryString);
                }
            }
        }
    }

    return configs;
}

bool PapyrusFunctions(RE::BSScript::IVirtualMachine *vm) {
    vm->RegisterFunction("FetchConfigs", "mkyQuestIcons_SKSEFunctions", FetchConfigs);
    return true;
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);

    SetupLog();

    // Once all plugins and mods are loaded, then the ~ console is ready and can
    // be printed to
    SKSE::GetPapyrusInterface()->Register(PapyrusFunctions);

    return true;
}