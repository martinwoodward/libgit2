#include "clar_libgit2.h"
#include "index.h"
#include "git2/repository.h"

static git_repository *repo;
static git_index *repo_index;

#define TEST_REPO_PATH "mergedrepo"
#define TEST_INDEX_PATH TEST_REPO_PATH "/.git/index"

#define ONE_ANCESTOR_OID "478871385b9cd03908c5383acfd568bef023c6b3"
#define ONE_OUR_OID "4458b8bc9e72b6c8755ae456f60e9844d0538d8c"
#define ONE_THEIR_OID "8b72416545c7e761b64cecad4f1686eae4078aa8"

#define TWO_ANCESTOR_OID "9d81f82fccc7dcd7de7a1ffead1815294c2e092c"
#define TWO_OUR_OID "8f3c06cff9a83757cec40c80bc9bf31a2582bde9"
#define TWO_THEIR_OID "887b153b165d32409c70163e0f734c090f12f673"

// Fixture setup and teardown
void test_index_reuc__initialize(void)
{
	repo = cl_git_sandbox_init("mergedrepo");
	git_repository_index(&repo_index, repo);
}

void test_index_reuc__cleanup(void)
{
	git_index_free(repo_index);
	cl_git_sandbox_cleanup();
}

void test_index_reuc__read_bypath(void)
{
	const git_index_reuc_entry *reuc;
	git_oid oid;

	cl_assert_equal_i(2, git_index_reuc_entrycount(repo_index));

	cl_assert(reuc = git_index_reuc_get_bypath(repo_index, "two.txt"));

	cl_assert(strcmp(reuc->path, "two.txt") == 0);
	cl_assert(reuc->mode[0] == 0100644);
	cl_assert(reuc->mode[1] == 0100644);
	cl_assert(reuc->mode[2] == 0100644);
	git_oid_fromstr(&oid, TWO_ANCESTOR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[0], &oid) == 0);
	git_oid_fromstr(&oid, TWO_OUR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[1], &oid) == 0);
	git_oid_fromstr(&oid, TWO_THEIR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[2], &oid) == 0);

	cl_assert(reuc = git_index_reuc_get_bypath(repo_index, "one.txt"));

	cl_assert(strcmp(reuc->path, "one.txt") == 0);
	cl_assert(reuc->mode[0] == 0100644);
	cl_assert(reuc->mode[1] == 0100644);
	cl_assert(reuc->mode[2] == 0100644);
	git_oid_fromstr(&oid, ONE_ANCESTOR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[0], &oid) == 0);
	git_oid_fromstr(&oid, ONE_OUR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[1], &oid) == 0);
	git_oid_fromstr(&oid, ONE_THEIR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[2], &oid) == 0);
}

void test_index_reuc__ignore_case(void)
{
	const git_index_reuc_entry *reuc;
	git_oid oid;
	int index_caps;

	index_caps = git_index_caps(repo_index);

	index_caps &= ~GIT_INDEXCAP_IGNORE_CASE;
	cl_git_pass(git_index_set_caps(repo_index, index_caps));

	cl_assert(!git_index_reuc_get_bypath(repo_index, "TWO.txt"));

	index_caps |= GIT_INDEXCAP_IGNORE_CASE;
	cl_git_pass(git_index_set_caps(repo_index, index_caps));

	cl_assert_equal_i(2, git_index_reuc_entrycount(repo_index));

	cl_assert(reuc = git_index_reuc_get_bypath(repo_index, "TWO.txt"));

	cl_assert(strcmp(reuc->path, "two.txt") == 0);
	cl_assert(reuc->mode[0] == 0100644);
	cl_assert(reuc->mode[1] == 0100644);
	cl_assert(reuc->mode[2] == 0100644);
	git_oid_fromstr(&oid, TWO_ANCESTOR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[0], &oid) == 0);
	git_oid_fromstr(&oid, TWO_OUR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[1], &oid) == 0);
	git_oid_fromstr(&oid, TWO_THEIR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[2], &oid) == 0);
}

void test_index_reuc__read_byindex(void)
{
	const git_index_reuc_entry *reuc;
	git_oid oid;

	cl_assert_equal_i(2, git_index_reuc_entrycount(repo_index));

	cl_assert(reuc = git_index_reuc_get_byindex(repo_index, 0));

	cl_assert(strcmp(reuc->path, "one.txt") == 0);
	cl_assert(reuc->mode[0] == 0100644);
	cl_assert(reuc->mode[1] == 0100644);
	cl_assert(reuc->mode[2] == 0100644);
	git_oid_fromstr(&oid, ONE_ANCESTOR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[0], &oid) == 0);
	git_oid_fromstr(&oid, ONE_OUR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[1], &oid) == 0);
	git_oid_fromstr(&oid, ONE_THEIR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[2], &oid) == 0);

	cl_assert(reuc = git_index_reuc_get_byindex(repo_index, 1));

	cl_assert(strcmp(reuc->path, "two.txt") == 0);
	cl_assert(reuc->mode[0] == 0100644);
	cl_assert(reuc->mode[1] == 0100644);
	cl_assert(reuc->mode[2] == 0100644);
	git_oid_fromstr(&oid, TWO_ANCESTOR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[0], &oid) == 0);
	git_oid_fromstr(&oid, TWO_OUR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[1], &oid) == 0);
	git_oid_fromstr(&oid, TWO_THEIR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[2], &oid) == 0);
}

void test_index_reuc__updates_existing(void)
{
	const git_index_reuc_entry *reuc;
	git_oid ancestor_oid, our_oid, their_oid, oid;
	int index_caps;

	git_index_clear(repo_index);

	index_caps = git_index_caps(repo_index);

	index_caps |= GIT_INDEXCAP_IGNORE_CASE;
	cl_git_pass(git_index_set_caps(repo_index, index_caps));

	git_oid_fromstr(&ancestor_oid, TWO_ANCESTOR_OID);
	git_oid_fromstr(&our_oid, TWO_OUR_OID);
	git_oid_fromstr(&their_oid, TWO_THEIR_OID);

	cl_git_pass(git_index_reuc_add(repo_index, "two.txt",
		0100644, &ancestor_oid,
		0100644, &our_oid,
		0100644, &their_oid));

	cl_git_pass(git_index_reuc_add(repo_index, "TWO.txt",
		0100644, &our_oid,
		0100644, &their_oid,
		0100644, &ancestor_oid));

	cl_assert_equal_i(1, git_index_reuc_entrycount(repo_index));

	cl_assert(reuc = git_index_reuc_get_byindex(repo_index, 0));

	cl_assert(strcmp(reuc->path, "TWO.txt") == 0);
	git_oid_fromstr(&oid, TWO_OUR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[0], &oid) == 0);
	git_oid_fromstr(&oid, TWO_THEIR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[1], &oid) == 0);
	git_oid_fromstr(&oid, TWO_ANCESTOR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[2], &oid) == 0);
}

void test_index_reuc__remove(void)
{
	git_oid oid;
	const git_index_reuc_entry *reuc;

	cl_assert_equal_i(2, git_index_reuc_entrycount(repo_index));

	cl_git_pass(git_index_reuc_remove(repo_index, 0));
	cl_git_fail(git_index_reuc_remove(repo_index, 1));
	
	cl_assert_equal_i(1, git_index_reuc_entrycount(repo_index));

	cl_assert(reuc = git_index_reuc_get_byindex(repo_index, 0));

	cl_assert(strcmp(reuc->path, "two.txt") == 0);
	cl_assert(reuc->mode[0] == 0100644);
	cl_assert(reuc->mode[1] == 0100644);
	cl_assert(reuc->mode[2] == 0100644);
	git_oid_fromstr(&oid, TWO_ANCESTOR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[0], &oid) == 0);
	git_oid_fromstr(&oid, TWO_OUR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[1], &oid) == 0);
	git_oid_fromstr(&oid, TWO_THEIR_OID);
	cl_assert(git_oid_cmp(&reuc->oid[2], &oid) == 0);
}

void test_index_reuc__write(void)
{
	git_oid ancestor_oid, our_oid, their_oid;
	const git_index_reuc_entry *reuc;

	git_index_clear(repo_index);

	/* Write out of order to ensure sorting is correct */
	git_oid_fromstr(&ancestor_oid, TWO_ANCESTOR_OID);
	git_oid_fromstr(&our_oid, TWO_OUR_OID);
	git_oid_fromstr(&their_oid, TWO_THEIR_OID);

	cl_git_pass(git_index_reuc_add(repo_index, "two.txt",
		0100644, &ancestor_oid,
		0100644, &our_oid,
		0100644, &their_oid));

	git_oid_fromstr(&ancestor_oid, ONE_ANCESTOR_OID);
	git_oid_fromstr(&our_oid, ONE_OUR_OID);
	git_oid_fromstr(&their_oid, ONE_THEIR_OID);

	cl_git_pass(git_index_reuc_add(repo_index, "one.txt",
		0100644, &ancestor_oid,
		0100644, &our_oid,
		0100644, &their_oid));

	cl_git_pass(git_index_write(repo_index));

	cl_git_pass(git_index_read(repo_index));
	cl_assert_equal_i(2, git_index_reuc_entrycount(repo_index));

	/* ensure sort order was round-tripped correct */
	cl_assert(reuc = git_index_reuc_get_byindex(repo_index, 0));
	cl_assert(strcmp(reuc->path, "one.txt") == 0);
	
	cl_assert(reuc = git_index_reuc_get_byindex(repo_index, 1));
	cl_assert(strcmp(reuc->path, "two.txt") == 0);
}

