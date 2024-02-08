#include <linux/device-mapper.h>
#include <linux/blk_types.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/bio.h>


struct dmp_stat {
    unsigned long long read_requests;
    unsigned long long write_requests;
    unsigned long long read_size_sum;
    unsigned long long write_size_sum;
};

static struct dmp_stat stat;


/* ----------------------------------------------------- Module ----------------------------------------------------- */

struct my_dm_target {
    struct dm_dev *dev;
    struct dmp_stat stat;
};

static int dmp_map(struct dm_target *ti, struct bio *bio) {
    struct my_dm_target *mdt = (struct my_dm_target *) ti->private;
    // printk(KERN_CRIT "\n<< in function dmp_map \n");

    bio->bi_bdev = mdt->dev->bdev;

    if (bio_data_dir(bio) == WRITE) {
        stat.write_requests++;
        stat.write_size_sum += bio->bi_iter.bi_size;
    } else {
        stat.read_requests++;
        stat.read_size_sum += bio->bi_iter.bi_size;
    }

    submit_bio(bio);

    // printk(KERN_CRIT "\n>> out function dmp_map \n");
    return DM_MAPIO_SUBMITTED;
}

static int dmp_ctr(struct dm_target *ti, unsigned int argc, char **argv) {
    struct my_dm_target *mdt;
    // printk(KERN_CRIT "\n>>in function dmp_ctr \n");

    if (argc != 1) {
        printk(KERN_CRIT "\n Invalid number of arguments.\n");
        ti->error = "Invalid argument count";
        return -EINVAL;
    }

    mdt = kmalloc(sizeof(struct my_dm_target), GFP_KERNEL);

    if (mdt == NULL) {
        printk(KERN_CRIT "\n Mdt is null\n");
        ti->error = "dm-dmp: Cannot allocate linear context";
        return -ENOMEM;
    }

    if (dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &mdt->dev)) {
        ti->error = "dm-dmp: Device lookup failed";
        goto bad;
    }

    ti->private = mdt;


    // printk(KERN_CRIT "\n>> out function dmp_ctr \n");
    return 0;

bad:
    kfree(mdt);
    // printk(KERN_CRIT "\n>> out function dmp_ctr with error \n");
    return -EINVAL;
}

static void dmp_dtr(struct dm_target *ti) {
    struct my_dm_target *mdt = (struct my_dm_target *) ti->private;
    // printk(KERN_CRIT "\n<< in function dmp_dtr \n");

    dm_put_device(ti, mdt->dev);
    kfree(mdt);
    // printk(KERN_CRIT "\n>> out function dmp_dtr \n");
}

static struct target_type dmp_target = {
    .name = "dmp",
    .version = {1, 0, 0},
    .module = THIS_MODULE,
    .ctr = dmp_ctr,
    .dtr = dmp_dtr,
    .map = dmp_map,
};


/* --------------------------------------------------- Statistics --------------------------------------------------- */

static ssize_t volumes_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    unsigned long long read_requests_avg_size = 0;
    unsigned long long write_requests_avg_size = 0;
    unsigned long long all_requests = stat.read_requests + stat.write_requests;
    unsigned long long all_requests_avg_size = 0;

    if (stat.read_requests != 0) {
        read_requests_avg_size = stat.read_size_sum / stat.read_requests;
    }
    if (stat.write_requests != 0) {
        write_requests_avg_size = stat.write_size_sum / stat.write_requests;
    }
    if (all_requests != 0) {
        all_requests_avg_size = (stat.read_size_sum + stat.write_size_sum) / all_requests;
    }

    return sprintf(
        buf,
        "read:\n"
        "  reqs: %llu\n"
        "  avg size: %llu\n"
        "write:\n"
        "  reqs: %llu\n"
        "  avg size: %llu\n"
        "total:\n"
        "  reqs: %llu\n"
        "  avg size: %llu\n",
        stat.read_requests,
        read_requests_avg_size,
        stat.write_requests,
        write_requests_avg_size,
        stat.read_requests + stat.write_requests,
        all_requests_avg_size
    );
}

static ssize_t volumes_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    stat.read_requests = 0;
    stat.write_requests = 0;
    stat.read_size_sum = 0;
    stat.write_size_sum = 0;
    return count;
}

static struct kobj_attribute volumes_attribute = __ATTR_RW(volumes);

static struct attribute *attrs[] = {
    &volumes_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .name = "stat",
    .attrs = attrs,
};

/* -------------------------------------------------- Init & Exit --------------------------------------------------- */

static int __init init_dmp(void) {
    if (dm_register_target(&dmp_target) < 0) {
        printk(KERN_CRIT "\n Error in registering target \n");
        return -EINVAL;
    }

    return sysfs_create_group(&THIS_MODULE->mkobj.kobj, &attr_group);
}

static void __exit cleanup_dmp(void) {
    dm_unregister_target(&dmp_target);
}

module_init(init_dmp);
module_exit(cleanup_dmp);
MODULE_LICENSE("GPL");
