/**
* @file   config.cc
* @brief  configure files parser
* @author  zhangyuncong@baidu.com
* @version   1.0.1.0
* @date   2011-12-14
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#include "common/config.h"
#include <algorithm>
#include <string>
#include "common/error.h"
#include "common/logger.h"
#include "common/utils.h"

namespace dstream {
namespace config {

ConfigNode ConfigNode::GetNode(const std::string& subkey, bool create_if_no_exists) const {
    if (IsErrorNode()) {
        return CreateErrorNode();
    }
    using std::string;
    string::size_type beg = 0, end = 0;
    string step;
    TiXmlElement* ret_elem = m_cur_elem->ToElement() , *step_elem = NULL;
    while (end != string::npos) {
        end = subkey.find(".", beg);
        if (end == string::npos) {
            step = subkey.substr(beg);
        } else {
            step = subkey.substr(beg, end - beg);
        }
        beg = end + 1;
        if (!step.empty()) {
            step_elem = ret_elem->FirstChildElement(step.c_str());
            if (step_elem == NULL && create_if_no_exists && ret_elem->GetText() == NULL) {
                ret_elem = (ret_elem->LinkEndChild(new TiXmlElement(step.c_str())))->ToElement();
            } else if (step_elem == NULL) {
                return CreateErrorNode();
            } else {
                ret_elem = step_elem;
            }
        }
    }
    return ConfigNode(m_filepath, m_xml, ret_elem);
}

int ConfigNode::GetNodeNum(const std::string& subkey) const {
    if (IsErrorNode()) {
        return 0;
    }
    using std::string;
    string::size_type beg = 0, end = 0;
    string step;
    TiXmlElement* ret_elem = m_cur_elem->ToElement() , *step_elem = NULL;
    int node_num = 0;
    while (end != string::npos) {
        end = subkey.find(".", beg);
        if (end == string::npos) {
            step = subkey.substr(beg);
        } else {
            step = subkey.substr(beg, end - beg);
        }
        beg = end + 1;
        if (!step.empty()) {
            step_elem = ret_elem->FirstChildElement(step.c_str());
            if (step_elem != NULL) {
                node_num++;
                while ((step_elem = step_elem->NextSiblingElement(step.c_str())) != NULL) {
                    node_num++;
                }
            }
        }
    }
    return node_num;
}

error::Code ConfigNode::GetValue(const std::string& subkey, std::string* value) const {
    ConfigNode dest_node = GetNode(subkey);
    if (dest_node.IsErrorNode()) {
        return error::CONFIG_BAD_VALUE;
    }
    *value = dest_node.GetValue();
    return error::OK;
}

error::Code ConfigNode::GetValue(std::string* value) const {
    if (IsErrorNode()) {
        return error::CONFIG_BAD_VALUE;
    }
    *value = GetValue();
    return error::OK;
}

ConfigNode::ConfigNode(std::string filepath, TiXmlDocument* doc, TiXmlNode* cur_element)
    : m_cur_elem(cur_element), m_xml(doc), m_filepath(filepath) {
}

ConfigNode ConfigNode::CreateErrorNode() {
    return ConfigNode("", NULL, NULL);
}

ConfigNode::~ConfigNode() {
}

ConfigNode::NodeList ConfigNode::GetChildrenList() const {
    ConfigNode::NodeList child_list;
    if (!m_cur_elem->NoChildren()) {
        TiXmlElement* child = m_cur_elem->FirstChildElement();
        while (child) {
            child_list.push_back(ConfigNode(m_filepath, m_xml, child));
            child = child->NextSiblingElement();
        }
    }
    return child_list;
}

ConfigNode::NodeList ConfigNode::GetChildrenList(const std::string& subkey) const {
    ConfigNode::NodeList child_list;
    if (!m_cur_elem->NoChildren()) {
        TiXmlElement* child = m_cur_elem->FirstChildElement(subkey.c_str());
        while (child) {
            child_list.push_back(ConfigNode(m_filepath, m_xml, child));
            child = child->NextSiblingElement(subkey.c_str());
        }
    }
    return child_list;
}

bool ConfigNode::HasNoChildren() const {
    return m_cur_elem->FirstChildElement() == NULL;
}

bool ConfigNode::IsErrorNode() const {
    return m_xml == NULL;
}

std::string ConfigNode::GetName() const {
    if (IsErrorNode()) {
        return "";
    }
    return m_cur_elem->Value();
}

std::string ConfigNode::GetValue(const std::string& key) const {
    return GetNode(key).GetValue();
}

std::string ConfigNode::GetValue() const {
    if (IsErrorNode()) {
        return "";
    }
    TiXmlNode* text_node = m_cur_elem->FirstChild();
    while (text_node != NULL && text_node->Type() != TiXmlNode::TINYXML_TEXT) {
        text_node = text_node->NextSibling();
    }
    if (text_node == NULL) {
        return "";
    }
    return m_cur_elem->ToElement()->GetText();
}

error::Code ConfigNode::SetValue(const std::string& key,
                                 const std::string& value,
                                 bool create_if_no_exists) const {
    return GetNode(key, create_if_no_exists).SetValue(value);
}

error::Code ConfigNode::SetValue(const std::string& value) const {
    if (IsErrorNode()) {
        // should return something to show that the node has occur an error.
        return error::CONFIG_ITERM_INVALID;
    }
    if (!HasNoChildren()) {
        return error::CONFIG_ITERM_INVALID;
    }
    TiXmlNode* text_node = m_cur_elem->FirstChild();
    while (text_node != NULL && text_node->Type() != TiXmlNode::TINYXML_TEXT) {
        text_node = text_node->NextSibling();
    }
    if (text_node == NULL) {
        text_node = m_cur_elem->LinkEndChild(new TiXmlText(""));
    }
    text_node -> SetValue(value.c_str());
    return error::OK;
}


Config::Config(): m_xml(NULL), m_load_failed(false) {
}

ConfigNode Config::GetRootNode() const {
    return ConfigNode(m_cfg_file, m_xml, m_xml->RootElement());
}

// Read config from the specific file.
// If the file has been opened, it will release the old resource.
error::Code Config::ReadConfig(const std::string& filename) {
    if (m_xml == NULL) {
        m_xml = new TiXmlDocument();
    }
    if (m_xml == NULL) {
        DSTREAM_WARN("Memory request failed in class Config");
        return error::BAD_MEMORY_ALLOCATION;
    }
    if (filename.empty()) {
        DSTREAM_WARN("Invalid paramter: config file path is empty");
        return error::CONFIG_BAD_ARGUMENT;
    }
    if (access(filename.c_str(), 0) != 0) {
        DSTREAM_WARN("load xml failed,the file [%s] does not exist", filename.c_str());
        return error::CONFIG_BAD_FILE;
    }
    if (m_xml -> LoadFile(filename.c_str()) == false) {
        DSTREAM_WARN("load xml failed, the file[%s] is invalid ,"
                     "please ensure the xml file has an root node",
                     filename.c_str());
        return error::CONFIG_BAD_FILE;
    }
    m_cfg_file = filename;
    return error::OK;
}


Config::~Config() {
    if (m_xml) {
        this->close();
    }
}

void Config::close() {
    delete m_xml;
    m_xml = NULL;
}

std::string Config::GetValue(const std::string& key) const {
    return GetNode(key).GetValue();
}

error::Code Config::SetValue(const std::string& key,
                             const std::string& value,
                             bool create_if_no_exists) {
    return GetNode(key, create_if_no_exists).SetValue(value);
}

ConfigNode Config::GetNode(const std::string& key, bool create_if_no_exists) const {
    return GetRootNode().GetNode(key, create_if_no_exists);
}

error::Code Config::GetValue(const std::string& key, std::string* value) const {
    return GetRootNode().GetValue(key, value);
}

error::Code Config::GetIntValue(const std::string& key, int32_t* value) const {
    error::Code ret;
    std::string str;
    ret = GetValue(key, &str);
    if (ret < error::OK) {
        return ret;
    }
    *value = strtol(str.c_str(), NULL, 10);
    return error::OK;
}

void Config::SaveConfig() {
    if (NULL != m_xml) {
        m_xml->SaveFile();
    }
}

/* add by konghui, konghui@baidu.com
 * @date   2012-04-06 */
std::string ConfigMap::GetValue(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it;
    it = m_cfg_map.find(key);
    if (it != m_cfg_map.end()) {
        return it->second;
    }
    return "";
}

error::Code ConfigMap::GetValue(const std::string& key, std::string* value) const {
    std::map<std::string, std::string>::const_iterator it;
    it = m_cfg_map.find(key);
    if (it != m_cfg_map.end()) {
        *value = it->second;
        return error::OK;
    }
    return error::CONFIG_NO_NODE;
}

error::Code ConfigMap::SetValue(const std::string& key, const std::string& value,
                                bool create_if_no_exists) {
    std::map<std::string, std::string>::iterator it;
    it = m_cfg_map.find(key);
    if (it != m_cfg_map.end()) {
        it->second = value;
        return error::OK;
    }
    if (create_if_no_exists) {
        m_cfg_map.insert(std::make_pair(key, value));
        return error::OK;
    }
    return error::CONFIG_NO_NODE;
}

error::Code ConfigMap::GetIntValue(const std::string& key, int32_t* value) const {
    std::map<std::string, std::string>::const_iterator it;
    it = m_cfg_map.find(key);
    if (it != m_cfg_map.end()) {
        *value = strtol(it->second.c_str(), NULL, 10);
        return error::OK;
    }
    return error::CONFIG_NO_NODE;
}

error::Code ConfigMap::SetIntValue(const std::string& key, int32_t value) {
    std::string str_val;
    StringAppendNumber(&str_val, value);

    std::map<std::string, std::string>::iterator it;
    it = m_cfg_map.find(key);
    if (it != m_cfg_map.end()) {
        it->second = str_val;
        return error::OK;
    }
    m_cfg_map.insert(std::make_pair(key, str_val));
    return error::OK;
}

} // namespace config
} // namespace dstream
