/**
 * process_utils.cpp
 *
 * This module contains the functions which are responsible for setting process
 * capabilities and privileges.
 *
 * @author Michael A. Uman
 * @date   August 31, 2018
 */

#include <cstdio>
#include <cstring>
#include <string>
#include <algorithm>
#include <sys/prctl.h>
#include <linux/capability.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/capability.h>
#include "process_utils.h"

// capabilities array to use for no capabilities.
const cap_value_t no_capabilities[] = {
    0,
};

// Capabilities needed for set the uid (and gid)
const cap_value_t set_uid_gid_capabilities[] = {
    CAP_SETGID, CAP_SETUID, 0
};

/**
 * Take a null terminated array of capabilities and return the count.
 *
 * @param cap array of capabilities
 * @return count of capabilities in the array.
 */
int get_capabilities_count(const cap_value_t *cap) {
    const cap_value_t* p = cap;
    while (*p != 0) {
        p++;
    }
    return p - cap;
}

/**
 *
 * @param cap
 */
void drop_caps(const cap_value_t *cap) {
    auto cnt = get_capabilities_count(cap);
    printf("found %d caps\n", cnt);

    for (int i = 0 ; prctl(PR_CAPBSET_READ, i, 0, 0, 0) >= 0 ; i++) {
        if (std::find(cap, cap + cnt, i) != cap + cnt) {
            printf("cap %d not dropped\n", i);
            continue;
        }

        printf("dropping cap %d\n", i);
        prctl(PR_CAPBSET_DROP, i, 0, 0, 0);
    }

}


// Sets the current capabilities
//
// The effective set defines the capabilities which will be in effect after this call, use
// no_capabilities if none are required.
// The permissive set defines which capabilities we keep, it must be a superset
// of the effective capabilities.
void set_capabilities(const cap_value_t* permissive, const cap_value_t* effective) {

    // Sets the capabilities of this process to only the given ones.
    // All other capabilities are dropped.
    size_t permissive_cnt = get_capabilities_count(permissive);
    size_t effective_cnt = get_capabilities_count(effective);

    // Starting with an empty set of capabilities
    cap_t caps = cap_init();
    if (caps == nullptr) {
        // D("failed to get empty set of capabilities");
        exit(1);
    }
    // Permitting only the new set
    if (permissive_cnt > 0 && cap_set_flag(caps, CAP_PERMITTED, permissive_cnt, permissive, CAP_SET) != 0) {
        fprintf(stderr, "cap_set_flag failed: %s", strerror(errno));
        exit(1);
    }
    // Enabling the capabilities of the new set
    if (effective_cnt > 0 && cap_set_flag(caps, CAP_EFFECTIVE, effective_cnt, effective, CAP_SET) != 0) {
        fprintf(stderr, "cap_set_flag failed: %s", strerror(errno));
        exit(1);
    }

    // Set them in this process
    if (cap_set_proc(caps) != 0) {
        char* caps_as_text = cap_to_text(caps, nullptr);
        fprintf(stderr, "cap_set_proc for %s failed: %s", caps_as_text, strerror(errno));
        //log_cap_free(caps_as_text);
        exit(1);
    }
    cap_free(caps);
    //log_cap_free(caps);
}

#if 0
int main() {
    uid_t myuid = getuid();

    printf("Running as user %d\n", myuid);

    drop_caps(no_capabilities);
    set_capabilities(set_uid_gid_capabilities, set_uid_gid_capabilities);
    if (setuid(1401826305) == 0) {
        printf("setuid success!\n");
    } else {
        fprintf(stderr, "ERROR: Unable to setuid!\n");
    }

    myuid = getuid();

    printf("Running as user %d\n", myuid);

    return 0;
}
#endif