/*
 * internals.cpp - Auxiliary internal functions
 *
 *   Copyright 2010 Jesús Torres <jmtorres@ull.es>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>
#include <iostream>
#include <locale>
#include <unistd.h>

#include <cli/internals.hpp>

#include "fileno.hpp"

namespace cli { namespace internals
{

    bool isCharNoSpace(char c)
    {
        return ! std::isspace(c, std::locale());
    }

    bool isLineEmpty(const std::string& line)
    {
        std::string::const_iterator first;
        first = std::find_if(line.begin(), line.end(), isCharNoSpace);
        return first == line.end() ? true : false;
    }

    bool isStreamTty(const std::ios& stream)
    {
        int fd = ::fileno(stream);
        if (fd >= 0) {
            return ::isatty(fd) != 0 ? true : false;
        }
        return false;
    }

}}
