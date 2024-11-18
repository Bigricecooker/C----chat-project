#pragma once
#include "const.h"
// 管理key和value（内部）
struct SectionInfo {
    SectionInfo() {}

    ~SectionInfo() {
        _section_datas.clear();
    }

    // 拷贝构造
    SectionInfo(const SectionInfo& src) {
        _section_datas = src._section_datas;
    }

    // 重载赋值运算符
    SectionInfo& operator = (const SectionInfo& src) {
        if (&src == this) {
            return *this;
        }
        this->_section_datas = src._section_datas;
    }

    std::map<std::string, std::string> _section_datas;

    // 重载[]     默认的 [] 访问一个不存在的键，它也会触发插入
    std::string  operator[](const std::string& key) {
        if (_section_datas.find(key) == _section_datas.end()) {
            return "";
        }
        // 这里可以添加一些边界检查  
        return _section_datas[key];
    }
};

// 读取和管理配置
class ConfigMgr
{
public:
    ~ConfigMgr() {
        _config_map.clear();
    }

    SectionInfo operator[](const std::string& section) {
        if (_config_map.find(section) == _config_map.end()) {
            return SectionInfo();
        }
        return _config_map[section];
    }

    ConfigMgr& operator=(const ConfigMgr& src) {
        if (&src == this) {
            return *this;
        }
        this->_config_map = src._config_map;
    };

    ConfigMgr(const ConfigMgr& src) {
        this->_config_map = src._config_map;
    }

    // 构造函数里实现config读取
    ConfigMgr();
private:
    // 存储section和key-value对的map  
    std::map<std::string, SectionInfo> _config_map;
};

