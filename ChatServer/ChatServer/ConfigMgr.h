#pragma once
#include "const.h"
// ����key��value���ڲ���
struct SectionInfo {
    SectionInfo() {}

    ~SectionInfo() {
        _section_datas.clear();
    }

    // ��������
    SectionInfo(const SectionInfo& src) {
        _section_datas = src._section_datas;
    }

    // ���ظ�ֵ�����
    SectionInfo& operator = (const SectionInfo& src) {
        if (&src == this) {
            return *this;
        }
        this->_section_datas = src._section_datas;
    }

    std::map<std::string, std::string> _section_datas;

    // ����[]     Ĭ�ϵ� [] ����һ�������ڵļ�����Ҳ�ᴥ������
    std::string  operator[](const std::string& key) {
        if (_section_datas.find(key) == _section_datas.end()) {
            return "";
        }
        // ����������һЩ�߽���  
        return _section_datas[key];
    }
    // ��ȡֵ
    std::string GetValue(const std::string& key)
    {
        if (_section_datas.find(key) == _section_datas.end()) {
            return "";
        }
        // ����������һЩ�߽���  
        return _section_datas[key];
    }
};

// ��ȡ�͹�������
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

    /*ConfigMgr& operator=(const ConfigMgr& src) {
        if (&src == this) {
            return *this;
        }
        this->_config_map = src._config_map;
    };

    ConfigMgr(const ConfigMgr& src) {
        this->_config_map = src._config_map;
    }*/


    ConfigMgr& operator=(const ConfigMgr& src) = delete;
    ConfigMgr(const ConfigMgr& src) = delete;

    // ʵ�ֵ���
    static ConfigMgr& Inst()
    {
        static ConfigMgr cfg_mgr;
        return cfg_mgr;
    }

    // ��ȡֵ
    std::string GetValue(const std::string& section, const std::string& key);
private:
    // ���캯����ʵ��config��ȡ
    ConfigMgr();

    // �洢section��key-value�Ե�map  
    std::map<std::string, SectionInfo> _config_map;
};

