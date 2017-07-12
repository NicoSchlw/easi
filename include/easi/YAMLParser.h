/**
 * @file
 * This file is part of SeisSol.
 *
 * @author Carsten Uphoff (c.uphoff AT tum.de, http://www5.in.tum.de/wiki/index.php/Carsten_Uphoff,_M.Sc.)
 *
 * @section LICENSE
 * Copyright (c) 2017, SeisSol Group
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @section DESCRIPTION
 **/
#ifndef EASI_YAMLPARSER_H_
#define EASI_YAMLPARSER_H_

#include <string>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include "easi/parser/YAMLAbstractParser.h"
#include "easi/parser/YAMLComponentParsers.h"

namespace easi {
class YAMLParser : public YAMLAbstractParser {
public:
  YAMLParser(unsigned dimDomain, char firstVariable = 'x');
  virtual ~YAMLParser() {}
  
  template<typename T>
  void registerType(std::string const& tag);

  virtual Component* parse(std::string const& fileName);
  virtual Component* parse(YAML::Node const& node, std::set<std::string> const& in);

private:
  std::set<std::string> m_in;
  std::unordered_map<std::string, Component* (*)(YAML::Node const&, std::set<std::string> const&, YAMLAbstractParser*)> m_creators;
};

YAMLParser::YAMLParser(unsigned dimDomain, char firstVariable) {
  for (unsigned i = 0; i < dimDomain; ++i) {
    m_in.insert(std::string(1, firstVariable + i));
  }
  registerType<Switch>("!Switch");
  registerType<ConstantMap>("!ConstantMap");
  registerType<PolynomialMap>("!PolynomialMap");
  registerType<Any>("!Any");
  registerType<Any>("!IdentityMap");
  registerType<GroupFilter>("!GroupFilter");
  registerType<AxisAlignedCuboidalDomainFilter>("!AxisAlignedCuboidalDomainFilter");
  registerType<SphericalDomainFilter>("!SphericalDomainFilter");
  registerType<AffineMap>("!AffineMap");
  registerType<FunctionMap>("!FunctionMap");
  registerType<LayeredModelBuilder>("!LayeredModel");
  registerType<Include>("!Include");
}

template<typename T>
void YAMLParser::registerType(std::string const& tag) {
  m_creators[tag] = &create<T>;
}

Component* YAMLParser::parse(std::string const& fileName) {
  Component* root;
  YAML::Node config = YAML::LoadFile(fileName);
  
  root = parse(config, m_in);

  return root;
}


Component* YAMLParser::parse(YAML::Node const& node, std::set<std::string> const& in) {
  auto creator = m_creators.find(node.Tag());
  
  if (creator == m_creators.end()) {
    throw std::invalid_argument("Unknown tag " + node.Tag());
  }
  
  return (*creator->second)(node, in, this);
}
}

#endif
