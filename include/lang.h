#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace core{
    enum class LangType {
        en_US,
        fr_CA,
        es_ES,
        de_DE,
        it_IT,
        pt_BR,
        ru_RU,
        zh_CN,
        zh_TW,
        ja_JP,
        ko_KR,
        None
    };
    class Lang {
    public:
        static Lang* getInstance();
        static std::string langType2String(LangType type);
        static LangType string2LangType(const std::string& str);
        void setLang(LangType type);
        std::string get(const std::string& key) const;
        std::wstring getw(const std::string& key) const;
        LangType getCurrentLangType() const { return current_type; }
    private:
        Lang(LangType type);
        void backtoDefaultLang();
        void backtoNoneLang();
        static Lang* instance;
        LangType current_type;
        nlohmann::json lang_data;
    };
	std::string operator"" _lang(const char* str, size_t len);
}