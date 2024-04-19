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

static int vpu_init()
{
	VpuDecRetCode ret;
	VpuVersionInfo version;
	VpuWrapperVersionInfo wrapper_version;

	ret = VPU_DecLoad();
	if (ret != VPU_DEC_RET_SUCCESS) {
		printf("ERROR: VPU_DecLoad failed: %d\n", ret);
		return ENOMEM;
	}

	ret = VPU_DecGetVersionInfo(&version);
	if (ret != VPU_DEC_RET_SUCCESS) {
		printf("ERROR: VPU_DecGetVersionInfo fail: %d\n", ret);
		return ENOMEM;
	}

	ret = VPU_DecGetWrapperVersionInfo(&wrapper_version);
	if (ret != VPU_DEC_RET_SUCCESS) {
		printf("ERROR: VPU_DecGetWrapperVersionInfo fail: %d", ret);
		return ENOMEM;
	}

	printf("====== VPUDEC: on %s %s. ======\n",  __DATE__,__TIME__);
	printf("\twrapper: %d.%d.%d (%s)\n",
	       wrapper_version.nMajor, wrapper_version.nMinor,
	       wrapper_version.nRelease,
	       wrapper_version.pBinary ? wrapper_version.pBinary : "unknow");
	printf("\tvpulib: %d.%d.%d\n",
	       version.nLibMajor, version.nLibMinor, version.nLibRelease);
	printf("\tfirmware: %d.%d.%d.%d\n",
	       version.nFwMajor, version.nFwMinor, version.nFwRelease,
	       version.nFwCode);
}


static int vpu_uninit()
{
	VpuDecRetCode ret;

	ret = VPU_DecUnLoad();
	if (ret !=VPU_DEC_RET_SUCCESS) {
		printf("ERROR: could not un-init VPU memory\n");
		return EINVAL;
	}

	return 0;
}


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


static void print_usage(const char *pname)
{
	printf("Usage:\n%s <#parallel> <#allocs> <size>\n", pname);
}


int main(int argc, char *argv[])
{
	if (argc < 4) {
		print_usage(argv[0]);
		return EINVAL;
	}

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

	int err = vpu_init();
	if (err)
		return err;

	VpuMemDesc **mems = calloc(p, sizeof(VpuMemDesc*));
	for (int i=0; i<p; ++i)
		mems[i] = NULL;

	for (int i=0; i<n; ++i) {
		int k = i % p;

		if (mems[k])
			vpu_free(mems[k]);

		mems[k] = vpu_alloc(rand() % s);
	}


	err = vpu_uninit();
	if (err)
		return err;

	return 0;
}
