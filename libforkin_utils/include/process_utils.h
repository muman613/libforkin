//
// Created by muman on 8/31/18.
//

#ifndef FORKIN_PROCESS_UTILS_H
#define FORKIN_PROCESS_UTILS_H

namespace forkin::process {
    typedef int cap_value_t;

    int get_capabilities_count(const cap_value_t *cap);

    void drop_caps(const cap_value_t *cap);

    void set_capabilities(const cap_value_t *permissive, const cap_value_t *effective);

    int daemonize(const char *name = nullptr, const char *path = nullptr, const char *outfile = nullptr,
                  const char *errfile = nullptr, const char *infile = nullptr);

}
#endif //FORKIN_PROCESS_UTILS_H
