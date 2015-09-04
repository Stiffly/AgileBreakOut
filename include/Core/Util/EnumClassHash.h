//
// Created by imon on 2015-03-31.
//

#ifndef DAYDREAM_ENUMCLASSHASH_H
#define DAYDREAM_ENUMCLASSHASH_H

// http://stackoverflow.com/a/24847480/417018
struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

#endif //DAYDREAM_ENUMCLASSHASH_H
