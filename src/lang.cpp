#include "lang.h"

#include "base.h"
#include "Log.h"
#include "config.h"
#include <fstream>
#include <filesystem>

using namespace core;

Lang *Lang::instance = nullptr;

Lang::Lang(LangType type){
    setLang(type);
}

void Lang::setLang(LangType type){
    std::string langStr=langType2String(type);
    std::string langFilePath=appPath+"/files/lang/"+langStr+".json";
    if(!std::filesystem::exists(langFilePath)){
        Log.level(Level::ERROR)<<"Lang file not found: "<<langFilePath<<" use default language"<<op::endl;
        backtoDefaultLang();
        return;
    }
    std::ifstream langFile(langFilePath);
    if(!langFile.is_open()){
        Log.level(Level::ERROR)<<"Failed to open the lang file. Back to default lang"<<op::endl;
        backtoDefaultLang();
        return;
    }
    try{
        lang_data=nlohmann::json::parse(langFile);
    }
    catch(nlohmann::json::exception e){
        Log.level(Level::ERROR)<<"error occured when parse lang file. Back to default lang"<<op::endl;
        backtoDefaultLang();
        return;
    }
    current_type = type;
}

void Lang::backtoDefaultLang(){
    current_type = LangType::zh_CN;
    std::string langFilePath=appPath+"/files/lang/zh_CN.json";
    if(!std::filesystem::exists(langFilePath)){
        Log.level(Level::ERROR)<<"Default lang file not found: "<<langFilePath<<op::endl;
        backtoNoneLang();
        return;
    }
    std::ifstream langFile(langFilePath);
    if(!langFile.is_open()){
        Log.level(Level::ERROR)<<"Failed to open default lang file. Back to none lang"<<op::endl;
        backtoNoneLang();
        return;
    }
    try {
        lang_data = nlohmann::json::parse(langFile);
    } catch (const std::ios_base::failure& e) {
        Log.level(Level::ERROR) << "Failed to open default lang file: " << langFilePath << ", error: " << e.what() << op::endl;
        backtoNoneLang();
        return;
    }
}

void Lang::backtoNoneLang(){
    current_type=LangType::None;
    Log.level(Level::WARNING)<<"Using none lang, system will use key as text"<<op::endl;
}

std::string Lang::langType2String(LangType type){
    switch (type)
    {
        case LangType::de_DE:return "de_DE";
        case LangType::en_US:return "en_US";
        case LangType::es_ES:return "es_ES";
        case LangType::fr_CA:return "fr_CA";
        case LangType::it_IT:return "it_IT";
        case LangType::ja_JP:return "ja_JP";
        case LangType::ko_KR:return "ko_KR";
        case LangType::pt_BR:return "pt_BR";
        case LangType::ru_RU:return "ru_RU";
        case LangType::zh_CN:return "zh_CN";
        case LangType::zh_TW:return "zh_TW";
        case LangType::None:return "None";
        default:return "None";
    }
}

LangType Lang::string2LangType(const std::string& str){
    //转化为全小写
    std::string lowerStr=str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    //将下划线替换为连字符
    std::replace(lowerStr.begin(), lowerStr.end(), '_', '-');

    if(lowerStr=="en-us")return LangType::en_US;
    if(lowerStr=="fr-ca")return LangType::fr_CA;
    if(lowerStr=="es-es")return LangType::es_ES;
    if(lowerStr=="de-de")return LangType::de_DE;
    if(lowerStr=="it-it")return LangType::it_IT;
    if(lowerStr=="pt-br")return LangType::pt_BR;
    if(lowerStr=="ru-ru")return LangType::ru_RU;
    if(lowerStr=="zh-cn")return LangType::zh_CN;
    if(lowerStr=="zh-tw")return LangType::zh_TW;
    if(lowerStr=="ja-jp")return LangType::ja_JP;
    if(lowerStr=="ko-kr")return LangType::ko_KR;
    return LangType::None;
}

Lang* Lang::getInstance(){
    if(instance==nullptr){
		std::string configLang = Config::getInstance().get("lang").toString();
        if(configLang=="auto"||configLang.empty())
            instance=new Lang(string2LangType(systemLanguage));
        else
            instance=new Lang(string2LangType(configLang));
    }
    return instance;
}

std::string Lang::get(const std::string& key)const {
    if(current_type==LangType::None){
        return key;
    }
    if(lang_data.contains(key)){
        return lang_data[key].get<std::string>();
    }
    else{
        Log.level(Level::WARNING)<<"Key not found in lang file: "<<key<<op::endl;
        return key;
    }
}

std::wstring Lang::getw(const std::string& key)const {
    if(current_type==LangType::None){
        std::string keyStr=key;
        return string2wstring(keyStr);
    }
    if(lang_data.contains(key)){
        return string2wstring(lang_data[key].get<std::string>());
    }
    else{
        Log.level(Level::WARNING)<<"Key not found in lang file: "<<key<<op::endl;
        std::string keyStr=key;
        return string2wstring(keyStr);
    }
}

std::string core::operator"" _lang(const char* str, size_t len) {
	std::string key(str, len);
	return Lang::getInstance()->get(key);
}

std::string core::lang(const std::string& key) {
    return Lang::getInstance()->get(key);
}