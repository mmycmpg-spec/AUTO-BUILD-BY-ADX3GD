#pragma once
#include <Geode/Geode.hpp>
#include <map>
#include <string>
using namespace geode::prelude;

// Templates are stored as name -> object string (same format the editor uses for copy/paste).
inline std::map<std::string, std::string> loadTemplates() {
    return Mod::get()->getSavedValue<std::map<std::string, std::string>>("templates");
}

inline void storeTemplates(std::map<std::string, std::string> const& t) {
    Mod::get()->setSavedValue("templates", t);
}
