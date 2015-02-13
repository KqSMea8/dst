// $Revision: 10148 $ $Date: 2009-07-03 18:07:38 -0700 (Fri, 03 Jul 2009) $
// Copyright (c) Bullseye Testing Technology
// This source file contains confidential proprietary information.

// Provides /proc/BullseyeCoverage file, which implements an interface similar
//   to /dev/kmem, except reading at offset 0 returns the address of cov_array,
//   and writing is restricted to zeros within cov_array.
// This supports both module and kernel image.

#if _BullseyeCoverage
	#pragma BullseyeCoverage off
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/elf.h>
#include <asm/uaccess.h>

#if !defined(__user)
	#define __user
#endif

#define COV_ARRAY 1
#define COVRT_PASSIVE 1
#define Libcov_interface 3
#include "../atomic.h"
#include "../libcov.h"
#include "../covrt.c"

static cov_atomic_t fileOpenLock = cov_atomic_initializer;
static int isNegativeOffset;
static int isValidateAddr;

// Export symbols
#if defined(EXPORT_SYMBOL_NOVERS)
	#define libcov_export EXPORT_SYMBOL_NOVERS
#else
	#define libcov_export EXPORT_SYMBOL
#endif
void Libcov_cdecl Libcov_probe(void*, int);
void Libcov_cdecl cov_countDown(void);
void Libcov_cdecl cov_countUp(void);
libcov_export(Libcov_probe);
libcov_export(cov_check);
libcov_export(cov_countDown);
libcov_export(cov_countUp);
libcov_export(cov_reset);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	#include <linux/mm.h>
	#include <linux/highmem.h>
	#include <linux/sched.h>
	#include <asm/pgtable.h>

	// Some 2.4 kernels use pte_offset_map
	#if !defined(pte_offset_map)
		static inline pte_t* pte_offset_map(pmd_t* pmd, unsigned long address)
		{
			return pte_offset(pmd, address);
		}
		static inline void pte_unmap(pte_t* pte)
		{
		}
	#endif
	// Page Upper Directory introduced in vanilla 2.6.11, and may occur in custom 2.4 kernels
	#if !defined(pud_offset)
		typedef pgd_t pud_t;
		static inline pud_t* pud_offset(pgd_t* pgd, unsigned long address)
		{
			return pgd;
		}
		static inline int pud_bad(pud_t pud)
		{
			return 0;
		}
		static inline int pud_present(pud_t pud)
		{
			return 1;
		}
	#endif

	static int validateAddr(unsigned long address)
	{
		int isValid = 0;
		pgd_t* pgd = pgd_offset_k(address);
		if (!pgd_bad(*pgd) && pgd_present(*pgd)) {
			pud_t* pud = pud_offset(pgd, address);
			if (!pud_bad(*pud) && pud_present(*pud)) {
				pmd_t* pmd = pmd_offset(pud, address);
				if (!pmd_bad(*pmd) && pmd_present(*pmd)) {
					pte_t* pte = pte_offset_map(pmd, address);
					isValid = pte_present(*pte);
					pte_unmap(pte);
				}
			}
		}
		return isValid;
	}
#else
	#include <linux/notifier.h>

	static struct notifier_block notifier_blk;

	static int notifier_callback(struct notifier_block* this_, unsigned long state, void* p)
	{
		if (state == MODULE_STATE_GOING) {
			const struct module* module = p;
			const unsigned long modBegin = (unsigned long)module->module_core;
			const unsigned long modEnd = modBegin + module->core_size;
			unsigned i;
			for (i = 1; i < sizeof(cov_array) / sizeof(cov_array[0]); i++) {
				const unsigned long objAddr = (unsigned long)cov_array[i];
				if (objAddr >= modBegin && objAddr <= modEnd) {
					cov_array[i] = NULL;
				}
			}
		}
		return 0;
	}

	static int validateAddr(unsigned long address)
	{
		return 1;
	}
#endif


static int proc_file_open(struct inode* inode, struct file* file)
{
	int status = -EBUSY;
	// If file not already open
	if (cov_atomic_tryLock(&fileOpenLock)) {
		status = 0;
		isNegativeOffset = 0;
		printk(KERN_INFO "BullseyeCoverage: main array at %p\n", cov_array);
		#if defined(MODULE)
			// Check whether address validation is working
			isValidateAddr = validateAddr((unsigned long)cov_array);
		#else
			// Address validation check will not work
			isValidateAddr = 1;
		#endif
	}
	return status;
}

// Need seek implementation because default one limits offset range.
static loff_t proc_file_lseek(struct file* file, loff_t offset, int whence)
{
	switch (whence) {
	case 1:
		offset += file->f_pos;
		//lint -fallthrough
	case 0:
		file->f_pos = offset;
		// read system call does not allow negative offsets
		isNegativeOffset = offset < 0;
		if (isNegativeOffset) {
			file->f_pos = -file->f_pos;
		}
		break;
	default:
		offset = -EINVAL;
		break;
	}
	return offset;
}

static ssize_t proc_file_read(struct file* file, char __user* buf, size_t size, loff_t* ppos)
{
	size_t count = -EFAULT;
	unsigned long address = (unsigned long)*ppos;
	if (isNegativeOffset) {
		address = (unsigned long)-*ppos;
	}
	// If special address
	if (address <= 4) {
		if (address != 2) {
			if (size != 1) {
				count = -EINVAL;
			} else {
				char c;
				switch (address) {
				case 0:
					// Return protocol version
					c = 3;
					break;
				case 1:
					// Return elf machine type
					c = ELF_ARCH;
					break;
				case 3:
					// 3: reset measurements
					cov_reset();
					c = 0;
					break;
				case 4:
					// 4: whether address validation is working
					c = isValidateAddr != 0;
					break;
				default:
					// Impossible
					count = -EINVAL;
					break;
				}
				if (copy_to_user(buf, &c, 1) == 0) {
					count = 1;
					// Update file pointer
					(*ppos)++;
				}
			}
		} else if (size != sizeof(long)) {
			count = -EINVAL;
		} else {
			// Return address of cov_array
			unsigned long cov_array_offset = (unsigned long)cov_array;
			if (copy_to_user(buf, &cov_array_offset, sizeof(long)) == 0) {
				count = sizeof(long);
				// Update file pointer
				*ppos += sizeof(long);
			}
		}
	} else {
		// Attempt to validate address first, to avoid faulting after an
		// instrumented module is unloaded
		int isValidAddr = 0;
		#if !defined(MODULE) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
			// Checking Page Table Entry does not work for kernel image addresses,
			// at least on ARM. So check for address in same range as cov_array.
			// The range should be the largest size expected for the kernel image data.
			// The symptom if the range is too small would be "bad address" from
			// covgetkernel.
			// The symptom if the range is too large would be a kernel "oops", and
			// covgetkernel segfault, after unloading a module.
			// To find the distance between kernel image data and module data use
			//   grep ' [dD] ' /proc/kallsyms | sort | less
			// search for [ and compare address with line above.
			// The actual range seen for Fedora 8 x64 was 256M.
			// Range for ARM not directly determined, seemed to be around 16M.
			const unsigned long mask = ~(8*1024*1024UL - 1);
			isValidAddr = (address & mask) == ((unsigned long)cov_array & mask);
		#endif
		// Otherwise, check that Page Table Entry indicates page is present,
		//   but only if address validation is working
		if (!isValidAddr) {
			if (isValidateAddr) {
				isValidAddr = validateAddr(address);
			} else {
				isValidAddr = 1;
			}
		}
		if (isValidAddr && copy_to_user(buf, (void*)address, size) == 0) {
			count = size;
			// Update file pointer
			*ppos = address + size;
			if (isNegativeOffset) {
				*ppos = -*ppos;
			}
		}
	}
	return count;
}

// Write is only used to zero out a bad pointer in cov_array
static ssize_t proc_file_write(struct file* file, const char __user* buf, size_t size, loff_t* ppos)
{
	size_t count = -EFAULT;
	unsigned long address = (unsigned long)*ppos;
	if (isNegativeOffset) {
		address = (unsigned long)-*ppos;
	}
	// Restrict valid address to cov_array[1..n-1]
	if (size == sizeof(long) &&
		address >= (unsigned long)cov_array + 1 &&
		address <= (unsigned long)(cov_array + (unsigned long)cov_array[0]) - 1)
	{
		// Just write zero directly, instead of from user buf
		memset((void*)address, 0, sizeof(long));
		count = sizeof(long);
	}
	return count;
}

static int proc_file_release(struct inode* inode, struct file* file)
{
	cov_atomic_unlock(&fileOpenLock);
	return 0;
}

static struct file_operations proc_file_ops = {
	.owner   = THIS_MODULE,
	.open    = proc_file_open,
	.llseek  = proc_file_lseek,
	.read    = proc_file_read,
	.write   = proc_file_write,
	.release = proc_file_release
};

static const char procFilename[] = "BullseyeCoverage";

static int libcov_init(void)
{
	int status = -ENOENT;
	// Create /proc entry
	struct proc_dir_entry* entry =
		create_proc_entry(procFilename, 0600, NULL);
	if (entry != NULL) {
		entry->proc_fops = &proc_file_ops;
		status = 0;
	}
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		notifier_blk.notifier_call = notifier_callback;
		register_module_notifier(&notifier_blk);
	#endif
	printk(KERN_INFO "BullseyeCoverage: kernel run-time init %s\n",
		status == 0 ? "succeeded" : "failed");
	return status;
}

static void libcov_exit(void)
{
	remove_proc_entry(procFilename, NULL);
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		unregister_module_notifier(&notifier_blk);
	#endif
	printk(KERN_INFO "BullseyeCoverage: kernel run-time unloaded\n");
}

module_init(libcov_init)
module_exit(libcov_exit)
