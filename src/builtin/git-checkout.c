#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "errors.h"
#include "git-checkout.h"
#include "git-support.h"

static int conflict_cb(const char *conflicting_path, const git_oid *index_oid, unsigned int index_mode, unsigned int wd_mode, void *payload)
{
	fprintf(stderr,"Conflict in %s\n",conflicting_path);
	return 1;
}

/**
 * Create a branch with a given branch name from a reference.
 *
 * @param new_branch_ref where the reference of the new branch is stored to.
 * @param branch_ref the ref which to branch.
 * @param repo the repo
 * @param branch the name of the new branch
 * @return error code or 0 for success.
 */
int git2_create_branch_from_ref(git_reference **new_branch_ref, git_reference *branch_ref, git_repository* repo, const char *branch)
{
	int err;

	git_object *commit;
	git_commit *comm;

	if ((err = git_reference_peel(&commit, branch_ref, GIT_OBJ_COMMIT)) != 0)
		goto out;

	if ((err = git_commit_lookup(&comm, repo, git_object_id(commit))) != 0)
		goto out;

	err = git_branch_create(new_branch_ref, repo, branch, comm, 0);
out:
	return err;
}

int cmd_checkout(git_repository *repo, int argc, char **argv)
{
	int i, err, rc;
	char *branch;
	git_reference *branch_ref;
	git_checkout_opts checkout_opts;

	branch = NULL;
	rc = EXIT_FAILURE;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			if (!branch) branch = argv[i];
		} else
		{
		}
	}

	/* Try local branch */
	if (git_branch_lookup(&branch_ref,repo,branch,GIT_BRANCH_LOCAL) != 0)
	{
		/* No, try remote branch */
		git_reference *remote_branch_ref;
		char remote_buf[256];
		snprintf(remote_buf,sizeof(remote_buf),"%s/%s","origin",branch);
		if (git_branch_lookup(&remote_branch_ref, repo, remote_buf, GIT_BRANCH_REMOTE) == 0)
		{
			/* Exists, now try to create a local one from that reference */
			if ((err = git2_create_branch_from_ref(&branch_ref,remote_branch_ref,repo,branch)) != 0)
			{
				fprintf(stderr,"Error code %d\n",err);
				libgit_error();
				goto out;
			}
			git_reference_free(remote_branch_ref);
		} else
		{
			branch_ref = NULL;
		}
	}

	printf("Checking out %s\n",branch_ref?git_reference_name(branch_ref):branch);
	if ((err = git_repository_set_head(repo,branch_ref?git_reference_name(branch_ref):branch)) != 0)
	{
		fprintf(stderr,"Error code %d\n",err);
		libgit_error();
		goto out;
	}

	/* Default options. Note by default we perform a dry checkout */
	memset(&checkout_opts,0,sizeof(checkout_opts));
	checkout_opts.version = GIT_CHECKOUT_OPTS_VERSION;
	checkout_opts.conflict_cb = conflict_cb;

	checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE|GIT_CHECKOUT_REMOVE_UNTRACKED;//GIT_CHECKOUT_SAFE|GIT_CHECKOUT_UPDATE_UNTRACKED;
	if (git_checkout_head(repo,&checkout_opts) != 0)
	{
		libgit_error();
		goto out;
	}

	rc = EXIT_SUCCESS;
out:
	if (branch_ref)
		git_reference_free(branch_ref);

	return rc;
}
