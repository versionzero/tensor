
#ifndef _FILENO_H_
#define _FILENO_H_

#include <iosfwd>

template <typename charT, typename traits>
int fileno(const std::basic_ios<charT, traits>& stream);

#endif
