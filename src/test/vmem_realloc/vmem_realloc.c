/*
 * Copyright 2014-2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * vmem_realloc -- unit test for vmem_realloc
 *
 * usage: vmem_realloc [directory]
 */

#include "unittest.h"

int
main(int argc, char *argv[])
{
	const int test_value = 123456;
	char *dir = NULL;
	void *mem_pool = NULL;
	VMEM *vmp;

	START(argc, argv, "vmem_realloc");

	if (argc == 2) {
		dir = argv[1];
	} else if (argc > 2) {
		UT_FATAL("usage: %s [directory]", argv[0]);
	}

	if (dir == NULL) {
		/* allocate memory for function vmem_create_in_region() */
		mem_pool = MMAP_ANON_ALIGNED(VMEM_MIN_POOL, 4 << 20);

		vmp = vmem_create_in_region(mem_pool, VMEM_MIN_POOL);
		if (vmp == NULL)
			UT_FATAL("!vmem_create_in_region");
	} else {
		vmp = vmem_create(dir, VMEM_MIN_POOL);
		if (vmp == NULL)
			UT_FATAL("!vmem_create");
	}

	int *test = vmem_realloc(vmp, NULL, sizeof(int));
	UT_ASSERTne(test, NULL);

	test[0] = test_value;
	UT_ASSERTeq(test[0], test_value);

	/* check that pointer came from mem_pool */
	if (dir == NULL) {
		UT_ASSERTrange(test, mem_pool, VMEM_MIN_POOL);
	}

	test = vmem_realloc(vmp, test, sizeof(int) * 10);
	UT_ASSERTne(test, NULL);
	UT_ASSERTeq(test[0], test_value);
	test[1] = test_value;
	test[9] = test_value;

	/* check that pointer came from mem_pool */
	if (dir == NULL) {
		UT_ASSERTrange(test, mem_pool, VMEM_MIN_POOL);
	}

	vmem_free(vmp, test);

	vmem_delete(vmp);

	DONE(NULL);
}
