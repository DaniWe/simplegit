#include <stdio.h>

#include <git2.h>

#include "print.h"

void print_commit(git_commit *wcommit, int with_tree)
{
	const char *cmsg;
	char c;
	int nl;
	const git_signature *cauth;
	char oid_str[80];
	char t[40];
	time_t tt;
	struct tm tm;
	const git_oid *woid;
	const git_oid *toid;

	woid = git_commit_id(wcommit);
	cmsg  = git_commit_message(wcommit);
	cauth = git_commit_author(wcommit);
	tt = cauth->when.time + cauth->when.offset*60;
	tm = *gmtime(&tt);
	tm.tm_gmtoff = cauth->when.offset*60;
	strftime(t,sizeof(t),"%a %d %b %Y %T %z",&tm);
	printf("commit %s\n",git_oid_tostr(oid_str,sizeof(oid_str),woid));
	if (with_tree)
	{
		int num_parents;
		int i;

		toid = git_commit_tree_id(wcommit);
		printf("tree %s\n",git_oid_tostr(oid_str,sizeof(oid_str),toid));

		num_parents = git_commit_parentcount(wcommit);
		for (i=0;i<num_parents;i++)
		{
			git_commit *pcommit;

			if (git_commit_parent(&pcommit,wcommit,i) == GIT_OK)
				printf("parent %s\n",git_oid_tostr(oid_str,sizeof(oid_str),git_commit_id(pcommit)));
		}
	}
	printf("Author: %s <%s>\n",cauth->name,cauth->email);
	printf("Date:   %s\n",t);
	printf("\n");

	nl = 1;
	while ((c = *cmsg++))
	{
		if (nl)
		{
			printf("    ");
			nl = 0;
		}
		putc(c,stdout);
		if (c == '\n') nl = 1;
	}
	printf("\n");
}

