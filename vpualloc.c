#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/user.h>
#include <linux/const.h>

#include <imx-mm/vpu/vpu_wrapper.h>

#define ALIGN(x, a)		__ALIGN_KERNEL((x), (a))
#define PAGE_SHIFT      12
#define PAGE_SIZE       (1 << PAGE_SHIFT)
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)

static VpuMemDesc *vpu_alloc(int s)
{
	VpuMemDesc *mem;
	VpuDecRetCode ret;
	mem = malloc(sizeof(VpuMemDesc));
	memset(mem, 0, sizeof(VpuMemDesc));
	mem->nSize = PAGE_ALIGN(s);

	ret = VPU_DecGetMem(mem);
	if (ret == VPU_DEC_RET_SUCCESS) {
		printf("paddr: %lx vaddr: %lx size: %d\n",
		       mem->nPhyAddr, mem->nVirtAddr, mem->nSize);
		return mem;
	}
	else {
		printf("ERROR: could not get VPU memory of size %d\n", s);
	}

	return NULL;
}


static void vpu_free(VpuMemDesc *mem)
{
	VpuDecRetCode ret;

	ret = VPU_DecFreeMem(mem);
	if (ret != VPU_DEC_RET_SUCCESS) {
		printf("ERROR: could not free VPU memory of size %d\n",
		       mem->nSize);
	}
}


int main(int argc, char *argv[])
{
	if (argc < 4)
		return EINVAL;

	int p = atoi(argv[1]);
	int n = atoi(argv[2]);
	int s = atoi(argv[3]);

	if (p < 0)
		p = -p;

	if (p > 1000)
		p = 1000;


	if (s < 0)
		s = -s;

	if (s > 100000)
		s = 100000;

	if (n < 0)
		n = -n;

	VpuMemDesc **mems = calloc(p, sizeof(VpuMemDesc*));
	for (int i=0; i<p; ++i)
		mems[i] = NULL;

	for (int i=0; i<n; ++i) {
		int k = i % p;

		if (mems[k])
			vpu_free(mems[k]);

		mems[k] = vpu_alloc(rand() % s);
	}

	return 0;
}
