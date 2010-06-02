/*
  Copyright (c) 2009 by Brush Technology
  Copyright (c) 2010 by Silas Sewell <silas@sewell.ch>
  This file is part of Tyrion. http://www.tyrion.org

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef TYRION_CONFIG_H_
#define TYRION_CONFIG_H_

#include <map>
#include <string>

namespace tyrion {

class Config {
  public:
    /**
     * Construct a Config class from an INI file.
     */
    Config(std::string filename);

    /**
     * Returns a negative number if there were parse errors.
     */
    int ParseError();

    /**
     * Check if value exists in configuration file.
     */
    bool Has(std::string section, std::string name);

    /**
     * Return string representation of option.
     */
    std::string Get(std::string section, std::string name,
                    std::string default_value);

    /**
     * Return int representation of option.
     */
    long GetInt(std::string section, std::string name, long default_value);

  private:
    int error_;
    std::map<std::string, std::string> values_;
    static std::string MakeKey(std::string section, std::string name);
    static int ValueHandler(void* user, const char* section,
                            const char* name, const char* value);
};

}  // namespace tyrion

#endif  // TYRION_CONFIG_H_
