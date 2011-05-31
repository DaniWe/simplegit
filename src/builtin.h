#ifndef BUILTIN_H
#define BUILTIN_H

// original source : https://github.com/vfr-nl/git2

#include "exec-cmd.h"
#include "git-init.h"
#include "git-log.h"
#include "git-ls-files.h"
#include "git-checkout.h"
#include "git-ls-tree.h"
#include "git-update-index.h"
#include "git-mktag.h"

int cmd_init(int argc, const char **argv);

extern cmd_struct commands[];

cmd_handler lookup_handler(const char *cmd);

#endif /* BUILTIN_H */
