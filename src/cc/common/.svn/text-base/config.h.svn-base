/**
* @file       config.h
* @brief      configure files parser
* @author   hangyuncong@baidu.com
* @version  1.0.1.0
* @date     2011-11-29
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_CC_COMMON_CONFIG_H__
#define __DSTREAM_CC_COMMON_CONFIG_H__

#include <iterator>
#include <list>
#include <map>
#include <string>
#include "tinyxml/tinyxml.h"    // NOLINT
#include "common/error.h"       // NOLINT
#include "common/logger.h"

namespace dstream {
namespace config {
/*
  the name of root element of the property-config-file
  if the root is this value, process it as a property-config-file
  otherwise process it as other-config-file
*/
const std::string kDefaultConfigRootName("Property");
/**
* @brief    config node
* @date     2011-11-29
*/
class Config;
class ConfigNode {
public:
    std::string GetValue() const;
    error::Code SetValue(const std::string& value) const;
    std::string GetValue(const std::string& key) const;
    error::Code GetValue(const std::string& subkey, std::string* value) const;
    error::Code GetValue(std::string* value) const;
    error::Code SetValue(const std::string& subkey,
                         const std::string& value, bool create_if_no_exists = true) const;
    // return a node which is represented by the relative subkey.
    ConfigNode GetNode(const std::string& subkey, bool create_if_no_exists = false) const;

    // add by fangjun02@baidu.com
    // return the number of nodes which is represented by the relative subkey.
    int GetNodeNum(const std::string& subkey) const;
    // return the name of the node. Not worked as you think in the Name/Value sets configuration.
    std::string GetName() const;
    // return true when this node has no child-elements
    bool HasNoChildren() const;
    // to judge whether an error occured or not
    bool IsErrorNode() const;
    typedef std::list<ConfigNode> NodeList;
    // return the list of the child-elements.
    NodeList GetChildrenList() const;
    // return the list of the child-elements which is represented by the relative subkey.
    NodeList GetChildrenList(const std::string& subkey) const;
    friend class Config;
    ~ConfigNode();

private:
    static ConfigNode CreateErrorNode();
    ConfigNode(std::string filepath, TiXmlDocument* doc, TiXmlNode* cur_element);
    void set_cur_elem(const std::string& flepath, TiXmlElement* cur_element);
    TiXmlNode*     m_cur_elem;
    TiXmlDocument* m_xml;
    std::string    m_filepath;
}; // class ConfigNode

/**
* @brief    configure file parser
* @date     2011-11-29
*/
class Config {
public:
    error::Code ReadConfig(const std::string& file);
    virtual std::string GetValue(const std::string& key) const;
    virtual error::Code GetValue(const std::string& key, std::string* value) const;
    virtual error::Code GetIntValue(const std::string& key, int32_t* value) const;
    virtual error::Code SetValue(const std::string& key,
                                 const std::string& value,
                                 bool create_if_no_exists = true);
    ConfigNode GetNode(const std::string& key, bool create_if_no_exists = false) const;
    ConfigNode GetRootNode()const;
    void close(); // close the xml file.
    Config();
    virtual ~Config();
    // save file used for test
    void SaveConfig();

private:
    std::string    m_cfg_file;
    TiXmlDocument* m_xml;
    bool           m_load_failed;
}; // class Config


/**
* @brief    config map
* @author   konghui, konghui@baidu.com
* @date   2012-04-06
*/
class ConfigMap : public Config {
public:
    ConfigMap(): m_has_init(false) {}
    virtual ~ConfigMap() {}

    std::string GetValue(const std::string& key) const;
    error::Code GetValue(const std::string& key, std::string* value) const;
    error::Code GetIntValue(const std::string& key, int32_t* value) const;
    error::Code SetValue(const std::string& key, const std::string& value,
                         bool create_if_no_exists = true);
    error::Code SetIntValue(const std::string& key, int32_t value);

protected:
    volatile bool                       m_has_init;
    std::map<std::string, std::string>  m_cfg_map;
};

} // namespace config
} // namespace dstream

#endif // __DSTREAM_CC_COMMON_CONFIG_H__
